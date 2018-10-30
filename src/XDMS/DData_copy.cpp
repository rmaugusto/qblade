/****************************************************************************

    DData Class
        Copyright (C) 2012 Juliane Wendler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "DData.h"
#include <math.h>
#include "../Objects/Polar.h"
#include "../Globals.h"
#include "../MainFrame.h"

QList <void*> *DData::s_poa360Polar;

DData::DData()
{
    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;

    m_bTipLoss = false;
    /*
    m_bRootLoss= false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
    m_bNewRootLoss = false;
    m_bNewTipLoss = false;
    */

    elements    =   100;
    epsilon     =   0.001;
    iterations  =   1000;
    relax       =   0.4;

}


DData::~DData()
{

}


CPolar* DData::Get360Polar(QString m_FoilName, QString PolarName)
{
    CPolar *pPolar;
    for (int i=0; i < s_poa360Polar->size() ; i++)
    {
            pPolar = (CPolar *) s_poa360Polar->at(i);
            if (pPolar->m_FoilName == m_FoilName && pPolar->m_PlrName == PolarName)
            {
                    return pPolar;

            }
    }
    return NULL;
}


void DData::Init(CBlade *pWing, double lambda)
{
    //discretization of a wing
    m_WingName=pWing->m_WingName;
    lambda_global=lambda;
    m_sections = pWing->m_NPanel;

    //calc blade height
    h_bottom = pWing->m_TPos[0];
    h_top = pWing->m_TPos[m_sections];
    height=fabs(h_top - h_bottom);


    //find max blade radius
    max_radius = 0;
    max_r_count = 0;

    blades = double(pWing->blades);

    lambdaglobal.sprintf("%.2f",lambda_global);

    double between = 0;
    double chord = 0;
    double thickness = 0;
    double radius = 0;
    double circ = 0;
    double sections = elements+1;
    double deltaangle = PI/sections;
    QList <double> dist;
    double tot = 0;

    //sinusoidal element spacing
    for (double i=deltaangle;i<=PI;i=i+deltaangle)
    {
        dist.append(sin(i));
        tot = tot + sin(i);
    }
    double thisdelta = height/tot;


    for (int i=0;i<dist.size();i++)
    {
        //width of each element
        deltas.append(thisdelta*dist.at(i));
    }

    //top radius of the wing as first position
    double position = pWing->m_TPos[0];

    int from, to;
    //discretization in height direction
    for (int i=0;i<elements;i++)
    {

        // element center
        position=position+deltas[i]/2;

        for (int j=0;j<m_sections+1;j++)
        {

        // sections surrounding the current element center
        if (position >= pWing->m_TPos[j] && position <= pWing->m_TPos[j+1])
            {
            from=j;
            to=j+1;
            }
        }

        // distance to the next station (between=1 -> on next station, between=0 -> on last station)
        between = (position-pWing->m_TPos[from]) / (pWing->m_TPos[to]-pWing->m_TPos[from]);
        // element chord
        chord = pWing->m_TChord[from]+between*(pWing->m_TChord[to]-pWing->m_TChord[from]);
        // element thickness
        MainFrame *pMainFrame = (MainFrame*)pWing->s_pMainFrame;
        CFoil *pFoil0 = pMainFrame->GetFoil(pWing->m_LFoil[from]);
        CFoil *pFoil1 = pMainFrame->GetFoil(pWing->m_LFoil[to]);
        thickness = chord*(pFoil0->m_fThickness+between*(pFoil1->m_fThickness-pFoil0->m_fThickness));
        // element radius
        radius = pWing->m_TOffset[from]+between*(pWing->m_TOffset[to]-pWing->m_TOffset[from]);

        if (radius > max_radius)
        {
            max_radius = radius;
            max_r_pos = i;
            max_r_count++;
        }

        // element circular angle
        circ = pWing->m_TCircAngle[from]+between*(pWing->m_TCircAngle[to]-pWing->m_TCircAngle[from]) - pWing->m_TCircAngle[0];

        m_between.append(double(1)-between);
        m_pos.append(position);
        m_c_local.append(chord);
        m_t_local.append(thickness);
        m_radius_local.append(radius);
        m_theta_local.append(circ/180.0*PI);

        // polar of previous section
        m_polar.append(pWing->m_Polar[from]);
        // polar of next section
        m_polarTO.append(pWing->m_Polar[to]);
        // foil of previous section
        m_foil.append(pWing->m_RFoil[from]);
        // foil of next section
        m_foilTO.append(pWing->m_RFoil[to]);

        position=position+deltas[i]/2;

    }

    // dimensionless height and radius coordinates
    for (int i=0;i<elements;i++)
    {
        m_zeta.append((m_pos.at(i)-m_pos.at(0))/(0.5*height)-1);
        m_eta.append(m_radius_local.at(i)/max_radius);
    }

    // calculate projection area per element
    double dr, dz;
    for (int i=0;i<elements-1;i++)
    {
        dz = fabs(m_pos.at(i)-m_pos.at(i+1));
        dr = 0.5*(m_radius_local.at(i)+m_radius_local.at(i+1));
        m_area.append(2*dr*dz);
    }
    m_area.append(0);// adjust # of area-elements

    // check swept area (via optimize or projection area)
    sweptArea = pWing->m_sweptArea;
    if (sweptArea == 0)
    {
        for (int i=0;i<elements;i++)
            sweptArea = sweptArea + m_area.at(i);
        pWing->m_sweptArea=sweptArea;
    }


    //calc local inflow velocity depending on wind profile selection
    double velocitylocal;

    if (bPowerLaw)
    {
        for (int i=0;i<elements;i++)
        {
            velocitylocal = windspeed * pow(((m_pos.at(i)+Toff)/(m_pos.at(max_r_pos)+Toff)),exponent);
            m_velocity_inf.append(velocitylocal);
        }
    }

    else if (bLogarithmic)
    {
        for (int i=0;i<elements;i++)
        {
            velocitylocal = windspeed * log((m_pos.at(i)+Toff)/roughness) / log((m_pos.at(max_r_pos)+Toff)/roughness);

            if (velocitylocal<0)
                    velocitylocal = 0;
            m_velocity_inf.append(velocitylocal);
        }
    }

    else
    {
        if (windspeed!=0)
            velocitylocal = windspeed;
        else
            velocitylocal = 1;

        for (int i=0;i<elements;i++)
        {
            m_velocity_inf.append(velocitylocal);
        }
    }


    //azimuthal angle for a whole blade revolution
    // [-pi/2 , pi/2[ -> upwind
    // [ pi/2 ,-pi/2[ -> downwind
    double theta = -90;
    for (int i=0; i<72; i++)
    {
        m_theta_deg.append(theta);
        m_theta.append(theta/180*PI);
        theta = theta+5;

        m_alpha_eq.append(0);
        m_CD_eq.append(0);
        m_CL_eq.append(0);
        m_LD_eq.append(0);
        m_Cn_eq.append(0);
        m_Ct_eq.append(0);
        m_FN.append(0);
        m_FT.append(0);
        m_T.append(0);
        m_CFN_eq.append(0);
        m_CFT_eq.append(0);

    }

}


void DData::OnDMS()
{
    double alpha, alpha_deg, delta, theta, F;
    double u, u_old, u_older, u2, u2_old, u2_older, delta_u;
    double omega, X, V, W, Re, CL, CD, CL2, CD2, DeltaAlpha;
    double Cn, Ct, f, zeta, eta, h_tower;
    QList <double> CFN, CFT, tempCFN, tempCFT, T, tempT;//, T2;
    double CQ1, CQ2, CP1, CP2, CP;
    int stop;

    windspeedStr.sprintf("%.2f",windspeed);

    m_PolarPointers.clear();

    // initilization of local variables for blade loads, torque and power
    for (int l=0; l<36; l++)
    {
        CFN.append(0);
        CFT.append(0);
        T.append(0);
        //T2.append(0);
        tempCFN.append(0);
        tempCFT.append(0);
        tempT.append(0);
    }

    CQ1=0;
    CQ2=0;

    // rotational speed
    omega = lambda_global*m_velocity_inf.at(max_r_pos)/max_radius;
    w=omega;

    // polar at previous section (m_PolarPointers) and next section (m_PolarPointersTO)
    for (int i=0;i<m_polar.size();i++)
    {
        m_PolarPointers.append(Get360Polar(m_foil.at(i),m_polar.at(i)));
        m_PolarPointersTO.append(Get360Polar(m_foilTO.at(i),m_polarTO.at(i)));
    }

    stop=0;
    // tower height
    h_tower = m_pos.at(elements-1) - height;

    // loop over all elements
    for (int i=0;i<elements;i++)
    {
        // initializations        
        // dimensionless height and radius
        zeta = m_zeta.at(i);
        eta  = m_eta.at(i);

        // upwind interference factor
        u = 1;
        delta_u = 10000;
        u_old=u;
        u_older=u;

        // iterations counter
        int count =0;

        // straight blade
        if (max_r_count == 1)
            delta = 0;
        else
        {
            // angle horizontal axis to blade chord plane normal
            delta = atan(2*fabs(m_pos.at(i)-m_pos.at(max_r_pos))/(0.5*height));

        }


        // convergence criterion
        while (delta_u > epsilon || u>=1 || u<0.5)
        {
            // stop when maximum number of iterations reached
            count++;
            if (count == iterations)
            {
                stop++;
                break;
            }

            // local induced velocity and local tip speed ratio
            V = u*m_velocity_inf.at(i);
            //X = (m_radius_local.at(i)/max_radius) * lambda_global * m_velocity_inf.at(i)/V;
            X = m_radius_local.at(i) * omega/V;

            //upwind function
            f = 0;

            // implementation of the tiploss correction
            // (Willmer modification of Prandtl method)
            if (m_bTipLoss)
            {
                double s_tip, a_tip;

                // tiploss factor
                s_tip = fabs(PI*(2*u-1)*m_velocity_inf.at(i)/(blades*omega));
                //qDebug() << "u = " << u << ", omega = " << omega;
                a_tip = 0.5*height - fabs(m_pos.at(i) - h_tower - 0.5*height);
                F = acos(exp(-PI*a_tip/s_tip))/acos(exp(-PI*0.5*height/s_tip));
            }
            else
            {
                F = 1;
            }

            // loop over upwind azimuthal angles theta
            for (int l=0; l<36; l++)
            {
                theta = m_theta.at(l);

                // local relative velocity
                W = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));

                // local angle of attack
                alpha = asin(F * cos(theta) * cos(delta) * V/W);
                alpha_deg = alpha*180/PI;

                // blade Reynolds number for local conditions
                Re = W * m_c_local.at(i)/visc;

                // lift/drag coefficients
                // extract polar data from polar at previous section
                bool found=false;

                int j=0;
                while (j < m_PolarPointers.at(i)->m_Alpha.size() && !found)
                {
                    if (m_PolarPointers.at(i)->m_Alpha.at(j) >= alpha_deg)
                    {
                        DeltaAlpha = m_PolarPointers.at(i)->m_Alpha.at(j)-m_PolarPointers.at(i)->m_Alpha.at(j-1);

                        CL = m_PolarPointers.at(i)->m_Cl.at(j-1)+(m_PolarPointers.at(i)->m_Cl.at(j)-m_PolarPointers.at(i)->m_Cl.at(j-1))/DeltaAlpha*(alpha_deg-m_PolarPointers.at(i)->m_Alpha.at(j-1));
                        CD = m_PolarPointers.at(i)->m_Cd.at(j-1)+(m_PolarPointers.at(i)->m_Cd.at(j)-m_PolarPointers.at(i)->m_Cd.at(j-1))/DeltaAlpha*(alpha_deg-m_PolarPointers.at(i)->m_Alpha.at(j-1));

                        found = true;
                    }
                    j++;
                }

                // extract polar data from polar at next section
                found=false;

                //if (m_bInterpolation && m_PolarPointers.at(i)->m_PlrName != m_PolarPointersTO.at(i)->m_PlrName)
                if (m_PolarPointers.at(i)->m_PlrName != m_PolarPointersTO.at(i)->m_PlrName)
                {
                    int k=0;
                    while (k< m_PolarPointersTO.at(i)->m_Alpha.size() && !found)
                    {
                        if (m_PolarPointersTO.at(i)->m_Alpha.at(k) >= alpha_deg)
                        {
                            DeltaAlpha = m_PolarPointersTO.at(i)->m_Alpha.at(k)-m_PolarPointersTO.at(i)->m_Alpha.at(k-1);

                            CL2 = m_PolarPointersTO.at(i)->m_Cl.at(k-1)+(m_PolarPointersTO.at(i)->m_Cl.at(k)-m_PolarPointersTO.at(i)->m_Cl.at(k-1))/DeltaAlpha*(alpha_deg-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));
                            CD2 = m_PolarPointersTO.at(i)->m_Cd.at(k-1)+(m_PolarPointersTO.at(i)->m_Cd.at(k)-m_PolarPointersTO.at(i)->m_Cd.at(k-1))/DeltaAlpha*(alpha_deg-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));

                            double newCL = CL*m_between.at(i)+(1.0-m_between.at(i))*CL2;
                            double newCD = CD*m_between.at(i)+(1.0-m_between.at(i))*CD2;

                            // blade element lift and drag coefficients
                            CL = newCL;
                            CD = newCD;

                            found = true;
                        }
                        k++;
                    }
                }
                // implementation of the tiploss correction
                // (Willmer modification of Prandtl method)
                if (m_bTipLoss)
                {
                    double a0 = 1.8*PI*(1+0.8*m_t_local.at(i)/m_c_local.at(i));
                    double AR = height/m_c_local.at(i);
                    CL = CL/(1 + a0/(PI*AR));
                    double CDi = pow(CL,2)/(PI*AR);
                    CD = CD + CDi;
                }

                // blade element normal and tangential force coefficient
                Cn = CL * cos(alpha) + CD * sin(alpha);
                Ct = CL * sin(alpha) - CD * cos(alpha);

                tempCFN[l] = Cn * pow((W/m_velocity_inf.at(i)),2) * eta/cos(delta);
                tempCFT[l] = Ct * pow((W/m_velocity_inf.at(i)),2) * eta/cos(delta);
                tempT[l]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                if (i == max_r_pos)
                {
                    m_alpha_eq[l]=alpha*180.0/PI;
                    m_CL_eq[l]=CL;
                    m_CD_eq[l]=CD;
                    m_LD_eq[l]=CL/CD;
                    m_Cn_eq[l]=Cn;
                    m_Ct_eq[l]=Ct;
                    m_CFN_eq[l]=tempCFN[l];
                    m_CFT_eq[l]=tempCFT[l];
                }

                // upwind function
                double tmp = ( Cn * cos(theta)/fabs(cos(theta)) - Ct * sin(theta)/cos(delta)/fabs(cos(theta)) ) * pow((W/V),2);
                if (l!=0)
                    f = f + tmp;

            }//azimuthal

            // iteration final upwind function
            f = f * m_c_local.at(i) * blades/(8*max_radius*36);
            // update interference factor
            u_older = u_old;
            u_old   = u;
            u       = PI*m_radius_local.at(i)/max_radius / (f + PI*m_radius_local.at(i)/max_radius);

            //implementation of the relaxation factor
            if (count < 3)
            {
            }
            else if (count == 3)
            {
                u       = 0.25*u+0.5*u_old+0.25*u_older;
            }
            else
            {
                u       = relax*u+(1-relax)*u_old;
            }

            delta_u = fabs(u_old-u);

        }// convergence

        m_Ftip_up.append(F);
        m_it_up.append(count);

        //load distribution
        for (int l=0; l<36; l++)
        {
            // add load contribution of current height for all upwind theta
            CFN[l]=CFN[l]+m_c_local.at(i) * tempCFN[l];
            CFT[l]=CFT[l]+m_c_local.at(i) * tempCFT[l];
            T[l]  =T[l]  +m_c_local.at(i) * tempT[l];
            //T2[l] =T2[l] +m_c_local.at(i) * tempT[l]/pow(m_velocity_inf.at(i),2);
        }

        // local upwind interference factor
        m_u_up.append(u);

        // local upwind induction factor
        m_a_up.append(1-u);

        // local induced velocity
        V = u * m_velocity_inf.at(i);
        m_velocity_up.append(V);

        // local tipspeed ratio
        //X = (m_radius_local.at(i)/max_radius) * lambda_global * m_velocity_inf.at(i)/V;
        X = m_radius_local.at(i)*omega/V;
        m_lambda_up.append(X);

        // azimuthal blade data at the equator
        if (i == int(elements/2))
        {

            for (int l=0; l<36; l++)
            {
                theta = m_theta.at(l);
                //equatorial local relative velocity
                W  = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));
                m_vrel_eq.append(W);
                //equatorial local Reynolds Number// blade Reynolds number for local conditions
                Re = W * m_c_local.at(i)/visc;
                m_Re_eq.append(Re);
                //equatorial local angle of attack// local angle of attack
                alpha = asin(F * cos(theta) * cos(delta) * V/W);
                m_alpha_eq[l]=alpha*180/PI;
            }
        }

    }//elements

    for (int l=0; l<36; l++)
    {
        // loads for each upwind blade position
        m_FN[l] = 0.5*height/sweptArea * 2/elements *CFN[l];
        m_FT[l] = 0.5*height/sweptArea * 2/elements *CFT[l];
        m_T[l]  = rho*0.5*height/elements*T[l];
        // average half-torque coefficient upwind
        //CQ1=CQ1 + T2[l];
        CQ1 = CQ1 + m_FT[l];

        // example data comparison
        //m_FN[l] = CFN[l];
        //m_FT[l] = CFT[l];

        // initilization for downwind
        CFN[l]=0;
        CFT[l]=0;
        T[l]=0;
        //T2[l]=0;
    }

    // upwind torque coefficient
    //CQ1 = blades*0.5*height/(max_radius*sweptArea*elements*36) * CQ1;
    CQ1 = blades/72 * CQ1;
    // upwind power coefficient
    CP1 = lambda_global*CQ1;


    // initializations for downwind part
    for (int i=0;i<elements;i++)
    {
        m_u_down.append(m_u_up.at(i));
        m_velocity_equil.append((2*m_u_up.at(i)-1)*m_velocity_inf.at(i));
    }

    stop=0;
    // loop over all elements
    for (int i=0;i<elements;i++)
    {
        // initializations
        // dimensionless height and radius
        zeta = m_zeta.at(i);
        eta  = m_eta.at(i);

        // up and downwind interference factor
        u = m_u_up.at(i);
        u2 = u;
        delta_u = 10000;
        u2_old=u2;
        u2_older=u2;
        // iterations counter
        int count =0;

        // straight blade
        if (max_r_count == 1)
            delta = 0;
        else
        {
            // angle horizontal axis to blade chord plane normal
            delta = atan(2*fabs(m_pos.at(i)-m_pos.at(max_r_pos))/(0.5*height));
            //delta = atan(2*fabs(m_pos.at(i)-m_pos.at(max_r_pos))/height);

        }

        // convergence criterion
        while (delta_u > epsilon || u2<=0)// || u2>=u)
        {
            // stop when maximum number of iterations reached
            count++;
            if (count == iterations)
            {
                stop++;
                break;
            }

            // local induced velocity and local tip speed ratio
            V = u2*m_velocity_equil.at(i);
            //X = (m_radius_local.at(i)/max_radius) * lambda_global * m_velocity_inf.at(i)/V;;
            X = m_radius_local.at(i)*omega/V;

            // downwind function
            f = 0;

            // implementation of the tiploss correction
            // (Willmer modification of Prandtl method)
            if (m_bTipLoss)
            {
                double s_tip, a_tip;

                // tiploss factor
                s_tip = fabs(PI*(2*u2-1)*m_velocity_equil.at(i)/(blades*omega));
                a_tip = 0.5*height - fabs(m_pos.at(i) - h_tower - 0.5*height);
                F = acos(exp(-PI*a_tip/s_tip))/acos(exp(-PI*0.5*height/s_tip));

            }
            else
            {
                F = 1;
            }

            // loop over downwind azimuthal angles theta
            for (int l=36; l<72; l++)
            {
                theta = m_theta.at(l);


                // local relative velocity
                W = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));

                // local angle of attack
                alpha = asin(F * cos(theta) * cos(delta) * V/W);
                alpha_deg = alpha*180/PI;

                // blade Reynolds number for local conditions
                Re = W * m_c_local.at(i)/visc;

                // lift/drag coefficients
                // extract polar data from polar at previous section
                bool found=false;

                int j=0;
                while (j < m_PolarPointers.at(i)->m_Alpha.size() && !found)
                {
                    if (m_PolarPointers.at(i)->m_Alpha.at(j) >= alpha_deg)
                    {
                        DeltaAlpha = m_PolarPointers.at(i)->m_Alpha.at(j)-m_PolarPointers.at(i)->m_Alpha.at(j-1);

                        CL = m_PolarPointers.at(i)->m_Cl.at(j-1)+(m_PolarPointers.at(i)->m_Cl.at(j)-m_PolarPointers.at(i)->m_Cl.at(j-1))/DeltaAlpha*(alpha_deg-m_PolarPointers.at(i)->m_Alpha.at(j-1));
                        CD = m_PolarPointers.at(i)->m_Cd.at(j-1)+(m_PolarPointers.at(i)->m_Cd.at(j)-m_PolarPointers.at(i)->m_Cd.at(j-1))/DeltaAlpha*(alpha_deg-m_PolarPointers.at(i)->m_Alpha.at(j-1));

                        found = true;
                    }
                    j++;
                }

                // extract polar data from polar at next section
                found=false;

                //if (m_bInterpolation && m_PolarPointers.at(i)->m_PlrName != m_PolarPointersTO.at(i)->m_PlrName)
                if (m_PolarPointers.at(i)->m_PlrName != m_PolarPointersTO.at(i)->m_PlrName)
                {
                    int k=0;
                    while (k< m_PolarPointersTO.at(i)->m_Alpha.size() && !found)
                    {
                        if (m_PolarPointersTO.at(i)->m_Alpha.at(k) >= alpha_deg)
                        {
                            DeltaAlpha = m_PolarPointersTO.at(i)->m_Alpha.at(k)-m_PolarPointersTO.at(i)->m_Alpha.at(k-1);

                            CL2 = m_PolarPointersTO.at(i)->m_Cl.at(k-1)+(m_PolarPointersTO.at(i)->m_Cl.at(k)-m_PolarPointersTO.at(i)->m_Cl.at(k-1))/DeltaAlpha*(alpha_deg-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));
                            CD2 = m_PolarPointersTO.at(i)->m_Cd.at(k-1)+(m_PolarPointersTO.at(i)->m_Cd.at(k)-m_PolarPointersTO.at(i)->m_Cd.at(k-1))/DeltaAlpha*(alpha_deg-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));

                            double newCL = CL*m_between.at(i)+(1.0-m_between.at(i))*CL2;
                            double newCD = CD*m_between.at(i)+(1.0-m_between.at(i))*CD2;

                            // blade element lift and drag coefficients
                            CL = newCL;
                            CD = newCD;

                            found = true;
                        }
                        k++;
                    }
                }

                // implementation of the tiploss correction
                // (Willmer modification of Prandtl method)
                if (m_bTipLoss)
                {
                    double a0 = 1.8*PI*(1+0.8*m_t_local.at(i)/m_c_local.at(i));
                    double AR = height/m_c_local.at(i);
                    CL = CL/(1 + a0/(PI*AR));
                    double CDi = pow(CL,2)/(PI*AR);
                    CD = CD + CDi;
                }

                // blade element normal and tangential force coefficient
                Cn = CL * cos(alpha) + CD * sin(alpha);
                Ct = CL * sin(alpha) - CD * cos(alpha);

                tempCFN[l-36] = Cn * pow((W/m_velocity_inf.at(i)),2) * eta/cos(delta);
                tempCFT[l-36] = Ct * pow((W/m_velocity_inf.at(i)),2) * eta/cos(delta);
                tempT[l-36]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                if (i == max_r_pos)
                {

                    m_alpha_eq[l]=alpha*180/PI;
                    m_CL_eq[l]=CL;
                    m_CD_eq[l]=CD;
                    m_LD_eq[l]=CL/CD;
                    m_Cn_eq[l]=Cn;
                    m_Ct_eq[l]=Ct;
                    m_CFN_eq[l]=tempCFN[l-36];
                    m_CFT_eq[l]=tempCFT[l-36];
                }


                // downwind function
                double tmp = ( Cn * cos(theta)/fabs(cos(theta)) - Ct * sin(theta)/cos(delta)/fabs(cos(theta)) ) * pow((W/V),2);
                if (l!=36)
                    f = f + tmp;

            }//azimuthal

            // iteration final downwind function
            f = f * m_c_local.at(i) * blades/(8*max_radius*36);
            // update interference factor
            u2_older = u2_old;
            u2_old   = u2;
            u2       = PI*m_radius_local.at(i)/max_radius / (f + PI*m_radius_local.at(i)/max_radius);

            //implementation of the relaxation factor
            if (count < 3)
            {
            }
            else if (count == 3)
            {
                u2       = 0.25*u2+0.5*u2_old+0.25*u2_older;
            }
            else
            {
                u2       = relax*u2+(1-relax)*u2_old;
            }

            delta_u = fabs(u2_old-u2);

        }// convergence

        m_Ftip_dw.append(F);
        m_it_dw.append(count);

        //load distribution
        for (int l=36; l<72; l++)
        {
            // add load contribution of current height for all upwind theta
            CFN[l-36]=CFN[l-36]+m_c_local.at(i) * tempCFN[l-36];
            CFT[l-36]=CFT[l-36]+m_c_local.at(i) * tempCFT[l-36];
            T[l-36]  =T[l-36]+m_c_local.at(i) * tempT[l-36];
            //T2[l-36] =T2[l-36] +m_c_local.at(i) * tempT[l-36]/pow(m_velocity_inf.at(i),2);
        }

        // local downwind interference factor
        m_u_down[i]=u2;

        // local downwind induction factor
        m_a_down.append(1-u2);

        // local induced velocity
        V = u2 * m_velocity_equil.at(i);
        m_velocity_down.append(V);

        // local tipspeed ratio
        //X = (m_radius_local.at(i)/max_radius) * lambda_global  * m_velocity_inf.at(i)/V;
        X = m_radius_local.at(i)*omega/V;
        m_lambda_down.append(X);

        // azimuthal blade data at the equator
        if (i == int(elements/2))
        {
            for (int l=36; l<72; l++)
            {
                theta = m_theta.at(l);
                //equatorial local relative velocity
                W  = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));
                m_vrel_eq.append(W);
                //equatorial local Reynolds Number// blade Reynolds number for local conditions
                Re = W * m_c_local.at(i)/visc;
                m_Re_eq.append(Re);
                //equatorial local angle of attack// local angle of attack
                alpha = asin(F * cos(theta) * cos(delta) * V/W);
                m_alpha_eq[l]=alpha*180/PI;
            }
        }

        // wake velocity
        m_velocity_wake.append((2*m_u_down.at(i)-1)*m_velocity_equil.at(i));

    }//elements

    for (int l=36; l<72; l++)
    {
        // loads and torque for each downwind blade position
        m_FN[l] = 0.5*height/sweptArea * 2/elements *CFN[l-36];
        m_FT[l] = 0.5*height/sweptArea * 2/elements *CFT[l-36];
        m_T[l]  = rho*0.5*height/elements*T[l-36];

        // average half-torque coefficient downwind
        //CQ2=CQ2 + T2[l-36];
        CQ2 = CQ2 + m_FT[l];

        // example data comparison
        //m_FN[l] = CFN[l-36];
        //m_FT[l] = CFT[l-36];
    }

    // downwind torque coefficient
    //CQ2 = blades*0.5*height/(max_radius*sweptArea*elements*36) * CQ2;
    CQ2 = blades/72 * CQ2;
    // downwind power coefficient
    CP2 = lambda_global*CQ2;
    // power coefficient
    CP = CP1 + CP2;

    // store torque and power coefficients
    ct1=CQ1;
    ct2=CQ2;
    ct=CQ1+CQ2;
    cp1=CP1;
    cp2=CP2;
    cp=CP1+CP2;

    // calculate torque and power
    if (windspeed>0)
    {
        torque = 0;
        for (int l=0; l<72; l++)
        {
            torque = torque + blades/72.0*m_T[l];
        }


        power = 0;
        for (int i=0; i < elements-1; i++)
        {
            power = power + 0.5*rho*pow(0.5*(m_velocity_inf.at(i)+m_velocity_inf.at(i+1)),3)*m_area.at(i)*cp;
        }
    }

}


void DData::Serialize(QDataStream &ar, bool bIsStoring)
{
    int i,n,j;
    float f;

    if(bIsStoring)
    {

        if (m_bIsVisible)  ar<<1; else ar<<0;
        if (m_bShowPoints) ar<<1; else ar<<0;
        ar << (int) m_Style;
        ar << (int) m_Width;

        ar << (float) elements;
        ar << (float) rho;
        ar << (float) epsilon;
        ar << (float) iterations;
        ar << (float) relax;
        ar << (float) visc;
        ar << (float) exponent;
        ar << (float) roughness;
        ar << (float) Toff;
        ar << (float) windspeed;
        ar << (float) sweptArea;
        ar << (float) max_radius;
        ar << (float) height;

        if (m_bTipLoss) ar << 1; else ar<<0;
        if (bConstant) ar << 1; else ar<<0;
        if (bPowerLaw) ar << 1; else ar<<0;
        if (bLogarithmic) ar << 1; else ar<<0;

        WriteCOLORREF(ar,m_Color);
        WriteCString(ar,m_WingName);
        WriteCString(ar,m_DMSName);
        WriteCString(ar,lambdaglobal);
        WriteCString(ar,windspeedStr);

        n=m_pos.size();
        ar << (int) n;

        for (i=0;i<n;i++)
        {
            ar << (float) m_pos[i] << (float) m_zeta[i] << (float) m_c_local[i] << (float) m_t_local[i];
            ar << (float) m_radius_local[i] << (float) m_eta[i] << (float) m_between[i] << (float) m_area[i];
            ar << (float) m_lambda_up[i] << (float) m_lambda_down[i];
            ar << (float) m_velocity_inf[i] << (float) m_velocity_up[i] << (float) m_velocity_equil[i];
            ar << (float) m_velocity_down[i] << (float) m_velocity_wake[i];
            ar << (float) m_u_up[i] << (float) m_u_down[i] << (float) m_a_up[i] << (float) m_a_down[i];
            ar << (float) m_Ftip_up[i] << (float) m_Ftip_dw[i] << (float) m_it_up[i] << (float) m_it_dw[i];
        }

        n=m_theta.size();
        ar << (int) n;

        for (i=0;i<n;i++)
        {
            ar << (float) m_theta_deg[i] << (float) m_theta[i] << (float) m_alpha_eq[i];
            ar << (float) m_Re_eq[i] << (float) m_vrel_eq[i];
            ar << (float) m_CD_eq[i] << (float) m_CL_eq[i] << (float) m_LD_eq[i];
            ar << (float) m_Cn_eq[i] << (float) m_Ct_eq[i];
            ar << (float) m_FN[i] << (float) m_FT[i] << (float) m_T[i];
        }

    }
    else
    {

        ar >> f;
        if (f) m_bIsVisible = true; else m_bIsVisible = false;
        ar >> f;
        if (f) m_bShowPoints = true; else m_bShowPoints = false;
        ar >> j;
        m_Style = j;
        ar >> j;
        m_Width = j;
        ar >> f;
        elements = f;
        ar >> f;
        rho = f;
        ar >> f;
        epsilon = f;
        ar >> f;
        iterations = f;
        ar >> f;
        relax = f;
        ar >> f;
        visc = f;
        ar >> f;
        exponent = f;
        ar >> f;
        roughness = f;
        ar >> f;
        Toff = f;
        ar >> f;
        windspeed = f;
        ar >> f;
        sweptArea = f;
        ar >> f;
        max_radius = f;
        ar >> f;
        height = f;

        ar >> f;
        if (f) m_bTipLoss = true; else m_bTipLoss = false;
        ar >> f;
        if (f) bConstant = true; else bConstant = false;
        ar >> f;
        if (f) bPowerLaw = true; else bPowerLaw = false;
        ar >> f;
        if (f) bLogarithmic = true; else bLogarithmic = false;

        ReadCOLORREF(ar,m_Color);
        ReadCString(ar,m_WingName);
        ReadCString(ar,m_DMSName);
        ReadCString(ar,lambdaglobal);
        ReadCString(ar,windspeedStr);

        ar >> n;

        for (i=0;i<n;i++)
        {
            ar >> f;
            m_pos.append(f);
            ar >> f;
            m_zeta.append(f);
            ar >> f;
            m_c_local.append(f);
            ar >> f;
            m_t_local.append(f);
            ar >> f;
            m_radius_local.append(f);
            ar >> f;
            m_eta.append(f);
            ar >> f;
            m_between.append(f);
            ar >> f;
            m_area.append(f);
            ar >> f;
            m_lambda_up.append(f);
            ar >> f;
            m_lambda_down.append(f);
            ar >> f;
            m_velocity_inf.append(f);
            ar >> f;
            m_velocity_up.append(f);
            ar >> f;
            m_velocity_equil.append(f);
            ar >> f;
            m_velocity_down.append(f);
            ar >> f;
            m_velocity_wake .append(f);
            ar >> f;
            m_u_up.append(f);
            ar >> f;
            m_u_down.append(f);
            ar >> f;
            m_a_up.append(f);
            ar >> f;
            m_a_down.append(f);
            ar >> f;
            m_Ftip_up.append(f);
            ar >> f;
            m_Ftip_dw.append(f);
            ar >> f;
            m_it_up.append(f);
            ar >> f;
            m_it_dw.append(f);

        }

        ar >> n;

        for (i=0;i<n;i++)
        {
            ar >> f;
            m_theta_deg.append(f);
            ar >> f;
            m_theta.append(f);
            ar >> f;
            m_alpha_eq.append(f);
            ar >> f;
            m_Re_eq.append(f);
            ar >> f;
            m_vrel_eq.append(f);
            ar >> f;
            m_CD_eq.append(f);
            ar >> f;
            m_CL_eq.append(f);
            ar >> f;
            m_LD_eq.append(f);
            ar >> f;
            m_Cn_eq.append(f);
            ar >> f;
            m_Ct_eq.append(f);
            ar >> f;
            m_FN.append(f);
            ar >> f;
            m_FT .append(f);
            ar >> f;
            m_T.append(f);
        }
    }
}

