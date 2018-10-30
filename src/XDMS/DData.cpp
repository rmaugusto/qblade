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
#include "QDebug"
#include "../Store.h"
#include "../Serializer.h"
#include "../Graph/NewCurve.h"
#include "TurDmsModule.h"


NewCurve *DData::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType, int heightIndex) {
	if (xAxis == "" || yAxis == "")
		return NULL;
	
	QList<double> xList, yList;
	for (int i = 0; i < 2; ++i) {  // for both axes
		const int index = getAvailableVariables(graphType).indexOf(i == 0 ? xAxis : yAxis);
		QList<double>* list = (i == 0 ? &xList : &yList);
		
		switch (graphType) {
		case NewGraph::TurbineBlade:
			switch (index) {
			case  0: *list = m_zeta; break;
			case  1: *list = m_eta; break;
			case  2: *list = m_c_local; break;
			case  3: *list = m_delta; break;
			case  4: *list = m_u_up; break;
			case  5: *list = m_u_down; break;
			case  6: *list = m_a_up; break;
			case  7: *list = m_a_down; break;
			case  8: *list = m_velocity_inf; break;
			case  9: *list = m_velocity_up; break;
			case 10: *list = m_velocity_equil; break;
			case 11: *list = m_velocity_down; break;
			case 12: *list = m_velocity_wake; break;
			case 13: *list = m_lambda_up; break;
			case 14: *list = m_lambda_down; break;
			case 15: *list = m_area; break;
			case 16: *list = m_Ftip_up; break;
			case 17: *list = m_Ftip_dw; break;
			case 18: *list = m_it_up; break;
			case 19: *list = m_it_dw; break;
			default: return NULL;
			}
			break;
		case NewGraph::TurbineAzimuthal:
			if (heightIndex == -1) {  // can happen when windspeed is set but not yet height
				return NULL;
			} else {
				switch (index) {
				case  0: *list = m_theta_plot; break;
				case  1: *list = m_iterations[heightIndex]; break;
				case  2: *list = m_u[heightIndex]; break;
				case  3: *list = m_V[heightIndex]; break;
				case  4: *list = m_vrel[heightIndex]; break;
				case  5: *list = m_Re[heightIndex]; break;
				case  6: *list = m_DeltaRe[heightIndex]; break;
				case  7: *list = m_Ftip[heightIndex]; break;
				case  8: *list = m_alpha_deg[heightIndex]; break;
				case  9: *list = m_CL[heightIndex]; break;
				case 10: *list = m_CD[heightIndex]; break;
				case 11: *list = m_LD[heightIndex]; break;
				case 12: *list = m_Cn[heightIndex]; break;
				case 13: *list = m_Ct[heightIndex]; break;
				case 14: *list = m_FT; break;
				case 15: *list = m_CF_length; break;
				case 16: *list = m_CF_cross; break;
				case 17: *list = m_T; break;
				case 18: *list = m_F_length; break;
				case 19: *list = m_F_cross; break;
				case 20: *list = m_FT_tot; break;
				case 21: *list = m_CF_length_tot; break;
				case 22: *list = m_CF_cross_tot; break;
				case 23: *list = m_T_tot; break;
				case 24: *list = m_F_length_tot; break;
				case 25: *list = m_F_cross_tot; break;
				default: return NULL;
				}
				break;
			}
		default:
			return NULL;
		}
	}
	
	NewCurve *curve = new NewCurve(this);
	// dimension can be taken from any list, it's all the same
	curve->setAllPoints(xList.toVector().data(), yList.toVector().data(), xList.size());
	return curve;
}

QStringList DData::getAvailableVariables(NewGraph::GraphType graphType) {
	QStringList variables;
	
	switch (graphType) {  // WARNING: when changing any variables list, change newCurve as well!
	case NewGraph::TurbineBlade:
		variables << "relative height h/H" << "relative radius r/R" << "chord c" << "inclination angle delta" <<
					 "interference factor u_up" <<
					 "interference factor u_dw" << "induction factor a_up" << "induction factor a_dw" <<
					 "inflow velocity V_inf" << "induced velocity V_up" << "equilibrium velocity V_e" <<
					 "induced velocity V_dw" << "wake velocity V_wake" << "tipspeed ratio TSR_up" <<
					 "tipspeed ratio TSR_dw" << "swept area S" << "tiploss factor F_up" << "tiploss factor F_dw" <<
					 "upwind iterations" << "downwind iterations";
		break;
	case NewGraph::TurbineAzimuthal:
		variables << "azimuthal angle theta" << "iterations" << "interference factor u" << "induced velocity V" <<
					 "relative velocity W" << "Reynolds number Re" << "Delta Re (Re Blade - Re Polar)" <<
					 "tiploss factor F" << "angle of attack alpha" << "lift coefficient Cl" << "drag coefficient Cd" <<
					 "lift/drag ratio (Cl/Cd)" << "normal force coefficient Cn" << "tangential force coefficient Ct" <<
					 "blade tangential force coefficient CF_t_bla" << "blade lengthwise force coefficient CF_x_bla" <<
					 "blade crosswise force coefficient CF_y_bla" << "blade torque Tq_bla" <<
					 "blade lengthwise force F_x_bla" << "blade crosswise force F_y_bla" <<
					 "rotor tangential force coefficient CF_t_rot" << "rotor lengthwise force coefficient CF_x_rot" <<
					 "rotor crosswise force coefficient CF_y_rot" << "rotor torque Tq_rot" <<
					 "rotor lengthwise force F_x_rot" << "rotor crosswise force F_y_rot";
	default:
		break;
	}

	return variables;
}

bool DData::isDrawPoints() {
	DMSData *simulation = g_dmsdataStore.getObjectByNameOnly(m_DMSName);
	return (simulation ? simulation->isDrawPoints() : false);
}

bool DData::isDrawCurve() {
	DMSData *simulation = g_dmsdataStore.getObjectByNameOnly(m_DMSName);
	return (simulation ? simulation->isDrawCurve() : true);
}

DData::DData(QString dmsName)
	: m_DMSName(dmsName)
{
    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_bBackflow     =   false;
    m_Style        =   0;
    m_Width        =   1;

    m_bTipLoss = false;
    m_bVariable = false;
    m_bAspectRatio = false;
    m_bIsInverted = false;


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


C360Polar* DData::Get360Polar(QString m_FoilName, QString PolarName)
{
    //TODO:: needs to be fixed
	C360Polar *pPolar;
    for (int i=0; i < g_360PolarStore.size() ; i++)
    {
			pPolar = (C360Polar *) g_360PolarStore.at(i);
			if (pPolar->m_airfoil->getName() == m_FoilName && pPolar->getName() == PolarName)
            {
                    return pPolar;

            }
    }
	return NULL;
}

QPen DData::doGetPen(int /*curveIndex*/, int highlightedIndex, bool /*forTheDot*/) {
	if (highlightedIndex == -1) {
		DMSData *simulation = g_dmsdataStore.getObjectByNameOnly(m_DMSName);  // TODO add reference to simulation to DData
		if (simulation) {
			return simulation->getPen();
		}
	}
	return m_pen;
}


void DData::Init(CBlade *pWing, double lambda, double pitch)
{
    //discretization of a wing
    m_WingName = pWing->getName();
    lambda_global=lambda;
    m_sections = pWing->m_NPanel;

    m_bIsInverted = pWing->m_bIsInverted;

    //calc blade height
    h_bottom = pWing->m_TPos[0];
    h_top = pWing->m_TPos[m_sections];
    height=fabs(h_top - h_bottom);


    //find max blade radius
    max_radius = 0;
    max_r_count = 0;

    blades = double(pWing->m_blades);

    lambdaglobal.sprintf("%.2f",lambda_global);

    double between = 0;
    double chord = 0;
    double thickness = 0;
    double radius = 0;
    double circ = 0;
    double sections = elements+1;
    double deltaangle = PI/sections;
    double twist = 0;
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

    int from = 0, to = 0;
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
		CFoil *pFoil0 = g_mainFrame->GetFoil(pWing->m_Airfoils[from]);
		CFoil *pFoil1 = g_mainFrame->GetFoil(pWing->m_Airfoils[to]);
        thickness = chord*(pFoil0->m_fThickness+between*(pFoil1->m_fThickness-pFoil0->m_fThickness));
        // element radius
        radius = pWing->m_TOffsetX[from]+between*(pWing->m_TOffsetX[to]-pWing->m_TOffsetX[from]);

        if (radius > max_radius)
        {
            max_radius = radius;
            max_r_pos = i;
            max_r_count++;
        }

        // element circular angle
        circ = pWing->m_TCircAngle[from]+between*(pWing->m_TCircAngle[to]-pWing->m_TCircAngle[from]) - pWing->m_TCircAngle[0];
        twist = (pWing->m_TTwist[from]+between*(pWing->m_TTwist[to]-pWing->m_TTwist[from]))-pitch;

        m_pos.append(position);
        m_c_local.append(chord);
        m_t_local.append(thickness);
        m_radius_local.append(radius);
        m_theta_local.append(circ);
        m_twist.append(90-twist);
        m_area.append(radius * 2 * deltas[i]);

        position=position+deltas[i]/2;

    }

    // dimensionless height and radius coordinates
    for (int i=0;i<elements;i++)
    {
        m_zeta.append((m_pos.at(i)-pWing->m_TPos[0])/(0.5*height)-1);
        m_eta.append(m_radius_local.at(i)/max_radius);
    }

//    // calculate projection area per element
//    double dr, dz;
//    for (int i=0;i<elements-1;i++)
//    {
//        dz = fabs(m_pos.at(i)-m_pos.at(i+1));
//        dr = 0.5*(m_radius_local.at(i)+m_radius_local.at(i+1));
//        m_area.append(2*dr*dz);
//    }
//    m_area.append(0);// adjust # of area-elements

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
        velocitylocal = windspeed;

        for (int i=0;i<elements;i++)
        {
            m_velocity_inf.append(velocitylocal);
        }
    }


    //azimuthal angle for a whole blade revolution
    // [-pi/2 , pi/2[ -> upwind
    // [ pi/2 ,-pi/2[ -> downwind
    double theta = -87.5;
    for (int i=0; i<72; i++)
    {
        m_theta_deg.append(theta);
        m_theta_plot.append(theta+90);
        m_theta.append(theta/180*PI);
        theta = theta+5;
        /*
        m_alpha_eq.append(0);
        m_CD_eq.append(0);
        m_CL_eq.append(0);
        m_LD_eq.append(0);
        m_Cn_eq.append(0);
        m_Ct_eq.append(0);
        m_CFN_eq.append(0);
        m_CFT_eq.append(0);
        */
        m_FN.append(0);
        m_FT.append(0);
        m_T.append(0);
        m_FN_tot.append(0);
        m_FT_tot.append(0);
        m_T_tot.append(0);
		m_CF_cross.append(0);
		m_CF_length.append(0);
		m_CF_cross_tot.append(0);
		m_CF_length_tot.append(0);
		m_F_length_tot.append(0);
		m_F_cross_tot.append(0);
		m_F_length.append(0);
		m_F_cross.append(0);
    }

}


void DData::OnDMS(CBlade *pBlade)
{
    double alpha = 0, alpha_deg, delta, theta, F, RE = 0, REBlade = 0;
    double u, u_old, u2, u2_old, delta_u;
    double omega, X, V, W = 0, CL = 0, CD = 0;
    QList <double> it_loc;
    QList <double> alpha_loc, alpha_deg_loc;
    QList <double> Ftip_loc, u_loc, V_loc, Re_loc, vrel_loc, DeltaRe_loc;
	QList <double> CD_loc, CL_loc, LD_loc, Cn_loc, Ct_loc;
    double Cn = 0, Ct = 0, f, zeta, eta;
	QList <double> tempCFN, tempCFT, tempT, CFN, CFT, T, tempFN, tempFT;
	QList<QList<double> > tempCFNlist, tempCFTlist, tempTlist, tempFNlist, tempFTlist;
    QList <double> bladepos;
    QList <int> bladeto, bladefrom;
	QList <double> CF_blade_length, CF_blade_cross, F_blade_length, F_blade_cross;
    double CQ1, CQ2, CP1, CP2, CP;
    int stop;

    windspeedStr.sprintf("%.2f",windspeed);

    // initilization of local variables for blade loads, torque and power
    for (int l=0; l<72; l++)
    {
        CFN.append(0);
        CFT.append(0);
        T.append(0);
        tempCFN.append(0);
        tempCFT.append(0);
		tempFN.append(0);
		tempFT.append(0);
        tempT.append(0);
		CF_blade_length.append(0);
		CF_blade_cross.append(0);
		F_blade_length.append(0);
		F_blade_cross.append(0);
    }

    for (int l=0; l<72; l++)
    {
        it_loc.append(0);
        vrel_loc.append(0);
        u_loc.append(0);
        V_loc.append(0);
        Re_loc.append(0);
        DeltaRe_loc.append(0);
        alpha_loc.append(0);
        alpha_deg_loc.append(0);
        Ftip_loc.append(0);
        CD_loc.append(0);
        CL_loc.append(0);
        LD_loc.append(0);
        Cn_loc.append(0);
        Ct_loc.append(0);
    }

    CQ1=0;
    CQ2=0;

    // rotational speed
    omega = lambda_global*m_velocity_inf.at(max_r_pos)/max_radius;
    w=omega;

    stop=0;
    // tower height
//    h_tower = m_pos.at(elements-1) - height;


    // variable interference factors
    if (m_bVariable)
    {

        // loop over all elements
        for (int i=0;i<elements;i++)
        {
            // initializations
            // dimensionless height and radius
            zeta = m_zeta.at(i);
            eta  = m_eta.at(i);

            //computation of the blade inclination angle delta

			if (i==0)               delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i))/fabs(m_pos.at(i+1)-m_pos.at(i)));
			else if (i==elements-1) delta = atan(fabs(m_radius_local.at(i)-m_radius_local.at(i-1))/fabs(m_pos.at(i)-m_pos.at(i-1)));
			else                    delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i-1))/fabs(m_pos.at(i+1)-m_pos.at(i-1)));

			m_delta.append(delta/PI*180);

            // loop over all upwind azi angles
            for (int l=0; l<36; l++)
            {
                theta = m_theta.at(l);

                // upwind interference factor
				u = 0;
                delta_u = 10000;
                u_old=u;
//                u_older=u;

                // iterations counter
                int count = 0;
                int save = 0;

                // loop over upwind azimuthal angles theta


                // convergence criterion
                while (delta_u > epsilon || u<=0 || save==0)
                {
                    // save after convergence
                    if (delta_u <= epsilon && u>0)
                        // calculate final results
                        save=1;

                    count++;
                    // save and stop when maximum number of iterations reached
                    if (count == iterations)
                        // calculate final results
                        save=1;
                    else if (count==iterations+1)
                    {
                        stop++;
                        break;
                    }

                    // local induced velocity and local tip speed ratio
					V = (1-u)*m_velocity_inf.at(i);
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
                        a_tip = 0.5*height - fabs(zeta*0.5*height);
                        F = acos(exp(-PI*a_tip/s_tip))/acos(exp(-PI*0.5*height/s_tip));
                    }
                    else
                    {
                        F = 1;
                    }

                    // local relative velocity
                    W = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));

                    // local angle of attack
                    if (F * cos(theta) * cos(delta) * V/W >= 1)
                    {
						alpha = asin(1);
                    }
                    else
                    {
						alpha = asin(F * cos(theta) * cos(delta) * V/W);
					}

					alpha_deg  = asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);

                    if ((X-sin(theta)<0))
                    {
                        alpha = PI-alpha;
                        alpha_deg = 180.0-alpha_deg;
						alpha_deg = 180.0-asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);
					}

                    double alpha_corrected;
                    alpha_corrected = alpha_deg;
                    if (alpha_corrected > 180) alpha_corrected-=360;
                    if (m_bIsInverted) alpha_corrected *= -1;

                    QList<double> ClCd;

                    //getting lift and drag coefficients

                    RE = W * m_c_local.at(i)/visc;

                    ClCd = pBlade->getBladeParameters(m_pos[i], alpha_corrected, true, RE, false, 0, pBlade->m_bisSinglePolar);
                    CL = ClCd.at(0);
                    CD = ClCd.at(1);
                    REBlade = ClCd.at(2);

                    if (m_bIsInverted){
                        CL *= -1;
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
					tempFN[l] = Cn * pow(W,2)/cos(delta);
					tempFT[l] = Ct * pow(W,2)/cos(delta);
                    tempT[l]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                    // normal iteration
                    if (save==0)
                    {

						double Fx;

						Fx = blades*m_c_local.at(i)/8/PI/m_radius_local.at(i)*pow(W/m_velocity_inf.at(i),2)*((Cn*cos(theta)+Ct*sin(theta)/cos(delta)));

						u_old   = u;
						u = pow(u,2)+Fx;


												double CTT;
												CTT = 4*u*(1-u);

												if (CTT>0.96*F)
												{

													u = (18*F-20-3*pow(fabs(CTT*(50-36*F)+12*F*(3*F-4)),0.5))/(36*F-50);
												}

												if (u<=0) u=0.01;
												if (u>=1) u=0.99;

						delta_u = fabs(u_old-u);

                    }

				}// convergence

				u = (1-u);

                // save final azi results
                it_loc[l] = count-1;
                vrel_loc[l] = W;
                u_loc[l] = u;
                V_loc[l] = V;
                Re_loc[l]= RE;
                DeltaRe_loc[l]= RE - REBlade;
                alpha_loc[l] = alpha;
                Ftip_loc[l] = F;
                CD_loc[l]=CD;
                CL_loc[l]=CL;
                LD_loc[l]=CL/CD;
                Cn_loc[l]=Cn;
                Ct_loc[l]=Ct;

            }//azimuthal

            // load distribution
            tempCFNlist.append(tempCFN);
            tempCFTlist.append(tempCFT);
            tempTlist.append(tempT);
			tempFNlist.append(tempFN);
			tempFTlist.append(tempFT);

			// average upwind interference and tiploss factor and local tipspeed ratio
            u = 0;
            F = 0;
            double count = 0;
            for (int l=0; l<36; l++)
            {
                u = u + u_loc[l];
                F = F + Ftip_loc[l];
                count = count + it_loc[l];
            }
            u = u/36.0;
            F = F/36.0;
            count = count/36.0;

            //// save upwind data
            m_it_up.append(count);
            // average height data
            m_Ftip_up.append(F);
            m_u_up.append(u);
            m_a_up.append(1-u);
            V = u * m_velocity_inf.at(i);
            m_velocity_up.append(V);
            X = m_radius_local.at(i)*omega/V;
            m_lambda_up.append(X);
            //local azimuthal data
            m_iterations.append(it_loc);
            m_vrel.append(vrel_loc);
            m_u.append(u_loc);
            m_V.append(V_loc);
            m_Re.append(Re_loc);
            m_DeltaRe.append(DeltaRe_loc);
            m_alpha.append(alpha_loc);
            m_Ftip.append(Ftip_loc);
            m_CL.append(CL_loc);
            m_CD.append(CD_loc);
            m_LD.append(LD_loc);
            m_Cn.append(Cn_loc);
            m_Ct.append(Ct_loc);

        }//elements

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

            // force distribution
            tempCFN = tempCFNlist.at(i);
            tempCFT = tempCFTlist.at(i);
            tempT   = tempTlist.at(i);
			tempFN = tempFNlist.at(i);
			tempFT = tempFTlist.at(i);

            //computation of the blade inclination angle delta

			if (i==0)               delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i))/fabs(m_pos.at(i+1)-m_pos.at(i)));
			else if (i==elements-1) delta = atan(fabs(m_radius_local.at(i)-m_radius_local.at(i-1))/fabs(m_pos.at(i)-m_pos.at(i-1)));
			else                    delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i-1))/fabs(m_pos.at(i+1)-m_pos.at(i-1)));

            // loop over downwind azimuthal angles theta
            for (int l=36; l<72; l++)
            {
                theta = m_theta.at(l);

                // up and downwind interference factor
				u2 = 0;
                delta_u = 10000;
                u2_old=u2;
//                u2_older=u2;

                // iterations counter
                int count =0;
                int save=0;

                // convergence criterion
                while (delta_u > epsilon || u2<=0 || save==0)// || u2>=u)
                {
                    // save after convergence
                    if (delta_u <= epsilon && u2>0)
                        // calculate final results
                        save=1;

                    count++;
                    // save and stop when maximum number of iterations reached
                    if (count == iterations)
                        // calculate final results
                        save=1;
                    else if (count==iterations+1)
                    {
                        stop++;
                        break;
                    }

                    // local induced velocity and local tip speed ratio
                    //V = u2*m_velocity_equil.at(i);
					V = (1-u2)*m_velocity_inf.at(i)*(2*m_u[i].at(36-(l-35))-1);
					if (V<=0) V=0.01;                    //X = (m_radius_local.at(i)/max_radius) * lambda_global * m_velocity_inf.at(i)/V;;
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
                        a_tip = 0.5*height - fabs(zeta*0.5*height);
                        F = acos(exp(-PI*a_tip/s_tip))/acos(exp(-PI*0.5*height/s_tip));

                    }
                    else
                    {
                        F = 1;
                    }

                    // local relative velocity
                    W = V * sqrt(pow((X-sin(theta)),2) + pow(F,2) * pow(cos(theta),2)*pow(cos(delta),2));

                    // local angle of attack
                    alpha = asin(F * cos(theta) * cos(delta) * V/W);
                    alpha_deg = asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);

                    if ((X-sin(theta)<0))
                    {
                        alpha = -PI-alpha;
                        alpha_deg = -180.0-asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);
					}

                    double alpha_corrected;
                    alpha_corrected = alpha_deg;
                    if (alpha_corrected > 180) alpha_corrected-=360;
                    if (m_bIsInverted) alpha_corrected *= -1;

                    QList<double> ClCd;

                    //getting lift and drag coefficients

                    RE = W * m_c_local.at(i)/visc;

                    ClCd = pBlade->getBladeParameters(m_pos[i], alpha_corrected, true, RE, false, 0, pBlade->m_bisSinglePolar);
                    CL = ClCd.at(0);
                    CD = ClCd.at(1);
                    REBlade = ClCd.at(2);


                    if (m_bIsInverted){
                        CL *= -1;
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
					tempFN[l] = Cn * pow(W,2)/cos(delta);
					tempFT[l] = Ct * pow(W,2)/cos(delta);
                    tempT[l]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                    //normal iteration
                    if (save==0)
                    {

						double Fx;

						Fx = blades*m_c_local.at(i)/8/PI/m_radius_local.at(i)*pow(W/m_velocity_equil.at(i),2)*((Cn*cos(theta)+Ct*sin(theta)/cos(delta)));

						u2_old   = u2;
						u2 = pow(u2,2)+Fx;

						if (u2>=1) u2 = 0.99;
						if (u2<=0) u2 = 0.01;

						delta_u = fabs(u2_old-u2);

                    }

                }// convergence
				u2 = 1-u2;

                // save final azi results
                it_loc[l] = count-1;
                vrel_loc[l] = W;
                u_loc[l] = u2;
                V_loc[l] = V;
                Re_loc[l]= RE;
                DeltaRe_loc[l] = RE - REBlade;
                alpha_loc[l] = alpha;
                Ftip_loc[l] = F;
                CD_loc[l]=CD;
                CL_loc[l]=CL;
                LD_loc[l]=CL/CD;
                Cn_loc[l]=Cn;
                Ct_loc[l]=Ct;
            }//azimuthal

            // load distribution
            tempCFNlist[i] = tempCFN;
            tempCFTlist[i] = tempCFT;
            tempTlist[i]   = tempT;
			tempFNlist[i] = tempFN;
			tempFTlist[i] = tempFT;

            // average downwind interference factor
            u2 = 0;
            F = 0;
            double count = 0;
            for (int l=36; l<72; l++)
            {
                u2 = u2 + u_loc[l];
                F = F + Ftip_loc[l];
                count = count + it_loc[l];
            }
            u2 = u2/36.0;
            F = F/36.0;
            count = count/36.0;

            //// save downwind data
			m_it_dw.append(count);
            // average height data
            m_Ftip_dw.append(F);
            m_u_down[i]=u2;
            m_a_down.append(1-u2);
            V = u2 * m_velocity_equil.at(i);
            m_velocity_down.append(V);
            m_velocity_wake.append((2*m_u_down.at(i)-1)*m_velocity_equil.at(i));
            X = m_radius_local.at(i)*omega/V;
            m_lambda_down.append(X);
            // azimuthal blade data
            for (int l=0; l<36; l++)
            {
                it_loc[l] = m_iterations[i].at(l);
                vrel_loc[l] = m_vrel[i].at(l);
                u_loc[l] = m_u[i].at(l);
                V_loc[l] = m_V[i].at(l);
                Re_loc[l] = m_Re[i].at(l);
                DeltaRe_loc[l] = m_DeltaRe[i].at(l);
                alpha_loc[l] = m_alpha[i].at(l);
                Ftip_loc[l] = m_Ftip[i].at(l);
                CL_loc[l] = m_CL[i].at(l);
                CD_loc[l] = m_CD[i].at(l);
                LD_loc[l] = m_LD[i].at(l);
                Cn_loc[l] = m_Cn[i].at(l);
                Ct_loc[l] = m_Ct[i].at(l);
            }
            m_iterations.replace(i, it_loc);
            m_vrel.replace(i, vrel_loc);
            m_u.replace(i, u_loc);
            m_V.replace(i, V_loc);
            m_Re.replace(i, Re_loc);
            m_DeltaRe.replace(i, DeltaRe_loc);
            m_alpha.replace(i, alpha_loc);
            for (int l=0; l<72; l++) alpha_deg_loc[l] = m_alpha[i].at(l)*180.0/PI;
            m_alpha_deg.append(alpha_deg_loc);
            m_Ftip.replace(i, Ftip_loc);
            m_CL.replace(i, CL_loc);
            m_CD.replace(i, CD_loc);
            m_LD.replace(i, LD_loc);
            m_Cn.replace(i, Cn_loc);
            m_Ct.replace(i, Ct_loc);

        }//elements
    }

    // constant interference factors
    else
    {
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
//            u_older=u;

            // iterations counter
            int count =0;
            int save = 0;

            //computation of the blade inclination angle delta

			if (i==0)               delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i))/fabs(m_pos.at(i+1)-m_pos.at(i)));
			else if (i==elements-1) delta = atan(fabs(m_radius_local.at(i)-m_radius_local.at(i-1))/fabs(m_pos.at(i)-m_pos.at(i-1)));
			else                    delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i-1))/fabs(m_pos.at(i+1)-m_pos.at(i-1)));

			m_delta.append(delta/PI*180);

            // convergence criterion
            while (delta_u > epsilon || u<=0 || save==0)
            {
                // save after convergence
                if (delta_u <= epsilon && u>0)
                    // calculate final results
                    save=1;

                count++;
                // save and stop after saving when maximum number of iterations reached
                if (count == iterations)
                    // calculate final results
                    save=1;
                else if (count==iterations+1)
                {
                    stop++;
                    break;
                }

                // local induced velocity and local tip speed ratio
                V = u*m_velocity_inf.at(i);
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
                    a_tip = 0.5*height - fabs(zeta*0.5*height);
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
                    if (F * cos(theta) * cos(delta) * V/W >= 1)
                    {
                    alpha = asin(1);
                    }
                    else
                    {
                        alpha = asin(F * cos(theta) * cos(delta) * V/W);
                    }
                    alpha_deg  = asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);
                    if ((X-sin(theta)<0))
                    {
                        alpha = PI-alpha;
                        alpha_deg = 180.0-alpha_deg;
                        alpha_deg = 180.0-asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);

					}

                    double alpha_corrected;
                    alpha_corrected = alpha_deg;
                    if (alpha_corrected > 180) alpha_corrected-=360;
                    if (m_bIsInverted) alpha_corrected *= -1;

                    QList<double> ClCd;

                    //getting lift and drag coefficients

                    RE = W * m_c_local.at(i)/visc;

                    ClCd = pBlade->getBladeParameters(m_pos[i], alpha_corrected, true, RE, false, 0, pBlade->m_bisSinglePolar);
                    CL = ClCd.at(0);
                    CD = ClCd.at(1);
                    REBlade = ClCd.at(2);


                    if (m_bIsInverted){
                        CL *= -1;
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
					tempFN[l] = Cn * pow(W,2)/cos(delta);
					tempFT[l] = Ct * pow(W,2)/cos(delta);
                    tempT[l]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                    // normal iteration
                    if (save==0)
                    {
                        // upwind function
                        double tmp = ( Cn * cos(theta)/fabs(cos(theta)) - Ct * sin(theta)/cos(delta)/fabs(cos(theta)) ) * pow((W/V),2);

                        f = f + tmp;
                    }

                    // save final azi results
                    else
                    {
                        it_loc[l] = count-1;
                        vrel_loc[l] = W;
                        u_loc[l] = u;
                        V_loc[l] = V;
                        Re_loc[l]= RE;
                        DeltaRe_loc[l] = RE - REBlade;
                        alpha_loc[l] = alpha;
                        Ftip_loc[l] = F;
                        CD_loc[l]=CD;
                        CL_loc[l]=CL;
                        LD_loc[l]=CL/CD;
                        Cn_loc[l]=Cn;
                        Ct_loc[l]=Ct;
                    }


                }//azimuthal

                if (save==0)
                {
                    // iteration final upwind function
                    f = f * m_c_local.at(i) * blades/(8*max_radius*36);
                    // update interference factor
//                    u_older = u_old;
                    u_old   = u;
                    u       = PI*m_radius_local.at(i)/max_radius / (f + PI*m_radius_local.at(i)/max_radius);

                    //// glauert correction for highly loaded rotors
                    double CTT;
                    CTT = 4*u*(1-u);


                    if (CTT>0.96*F)
                    {

						u = 1 - (18*F-20-3*pow(fabs(CTT*(50-36*F)+12*F*(3*F-4)),0.5))/(36*F-50);
                    }

					if (u>=1) u2 = 0.99;
					if (u<=0) u2 = 0.01;

                    //implementation of the relaxation factor
                    u       = relax*u+(1-relax)*u_old;
                    delta_u = fabs(u_old-u);

                }

            }// convergence

            // load distribution
            tempCFNlist.append(tempCFN);
            tempCFTlist.append(tempCFT);
            tempTlist.append(tempT);
			tempFNlist.append(tempFN);
			tempFTlist.append(tempFT);

            //// save upwind data
            m_it_up.append(count-1);
            // height constant data
            m_Ftip_up.append(F);
            m_u_up.append(u);
            m_a_up.append(1-u);
            V = u * m_velocity_inf.at(i);
            m_velocity_up.append(V);
			X = m_radius_local.at(i)*omega/V;
            m_lambda_up.append(X);
            m_velocity_equil.append((2*u-1)*m_velocity_inf.at(i));
            // azimuthal blade data
            m_iterations.append(it_loc);
            m_vrel.append(vrel_loc);
            m_u.append(u_loc);
            m_V.append(V_loc);
            m_Re.append(Re_loc);
            m_DeltaRe.append(DeltaRe_loc);
            m_alpha.append(alpha_loc);
            m_Ftip.append(Ftip_loc);
            m_CL.append(CL_loc);
            m_CD.append(CD_loc);
            m_LD.append(LD_loc);
            m_Cn.append(Cn_loc);
            m_Ct.append(Ct_loc);

        }//elements

        // initializations for downwind part
        for (int i=0;i<elements;i++)
        {
        m_u_down.append(0);
        }

        stop=0;
        // loop over all elements
        for (int i=0;i<elements;i++)
        {
            // initializations
            // dimensionless height and radius
            zeta = m_zeta.at(i);
            eta  = m_eta.at(i);

            // downwind interference factors
            u2 = 1;
            delta_u = 10000;
            u2_old=u2;
            // iterations counter
            int count =0;
            int save = 0;

            //computation of the blade inclination angle delta
			if (i==0)               delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i))/fabs(m_pos.at(i+1)-m_pos.at(i)));
			else if (i==elements-1) delta = atan(fabs(m_radius_local.at(i)-m_radius_local.at(i-1))/fabs(m_pos.at(i)-m_pos.at(i-1)));
			else                    delta = atan(fabs(m_radius_local.at(i+1)-m_radius_local.at(i-1))/fabs(m_pos.at(i+1)-m_pos.at(i-1)));

            // force distribution
            tempCFN = tempCFNlist.at(i);
            tempCFT = tempCFTlist.at(i);
            tempT   = tempTlist.at(i);
			tempFN = tempFNlist.at(i);
			tempFT = tempFTlist.at(i);


            // convergence criterion
            while (delta_u > epsilon || u2<=0 || save==0)// || u2>=u)
            {

                // save after convergence
                if (delta_u <= epsilon && u2>0)
                    // calculate final results
                    save=1;

                count++;
                // save and stop after saving when maximum number of iterations reached
                if (count == iterations)
                    // calculate final results
                    save=1;
                else if (count==iterations+1)
                {
                    stop++;
                    break;
                }

                // local induced velocity and local tip speed ratio
                V = u2*m_velocity_equil.at(i);
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
                    a_tip = 0.5*height - fabs(zeta*0.5*height);
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
                    alpha_deg = asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);
                    if ((X-sin(theta)<0))
                    {
                        alpha = -PI-alpha;
                        alpha_deg = -180.0-asin(F * cos(theta) * cos(delta) * V/W)*180/PI+m_twist[i]*cos(delta);
                    }

                    double alpha_corrected;
                    alpha_corrected = alpha_deg;
                    if (alpha_corrected > 180) alpha_corrected-=360;
                    if (m_bIsInverted) alpha_corrected *= -1;

                    QList<double> ClCd;

                    //getting lift and drag coefficients

                    RE = W * m_c_local.at(i)/visc;

                    ClCd = pBlade->getBladeParameters(m_pos[i], alpha_corrected, true, RE, false, 0, pBlade->m_bisSinglePolar);
                    CL = ClCd.at(0);
                    CD = ClCd.at(1);
                    REBlade = ClCd.at(2);


                    if (m_bIsInverted){
                        CL *= -1;
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
					tempFN[l] = Cn * pow(W,2)/cos(delta);
					tempFT[l] = Ct * pow(W,2)/cos(delta);
                    tempT[l]   = Ct * pow(W,2) * m_radius_local.at(i)/cos(delta);

                    // normal iteration
                    if (save==0)
                    {
                        // downwind function
                        double tmp = ( Cn * cos(theta)/fabs(cos(theta)) - Ct * sin(theta)/cos(delta)/fabs(cos(theta)) ) * pow((W/V),2);

                        f = f + tmp;
                    }

                    // save final azi results
                    else
                    {
                        it_loc[l] = count-1;
                        vrel_loc[l] = W;
                        u_loc[l] = u2;
                        V_loc[l] = V;
                        Re_loc[l]= RE;
                        DeltaRe_loc[l]= RE - REBlade;
                        alpha_loc[l] = alpha;
                        Ftip_loc[l] = F;
                        CD_loc[l]=CD;
                        CL_loc[l]=CL;
                        LD_loc[l]=CL/CD;
                        Cn_loc[l]=Cn;
                        Ct_loc[l]=Ct;
                    }

                }//azimuthal

                if (save==0)
                {
                    // iteration final downwind function
                    f = f * m_c_local.at(i) * blades/(8*max_radius*36);
                    // update interference factor
                    u2_old   = u2;
                    u2       = PI*m_radius_local.at(i)/max_radius / (f + PI*m_radius_local.at(i)/max_radius);

					if (u2>=1) u2 = 0.99;
					if (u2<=0) u2 = 0.01;

                    //implementation of the relaxation factor
                    u2       = relax*u2+(1-relax)*u2_old;
                    delta_u = fabs(u2_old-u2);
                }

            }// convergence

            // load distribution
            tempCFNlist[i] = tempCFN;
            tempCFTlist[i] = tempCFT;
            tempTlist[i]   = tempT;
			tempFNlist[i] = tempFN;
			tempFTlist[i] = tempFT;

            //// save downwind data
            m_it_dw.append(count-1);
            // height constant data
            m_Ftip_dw.append(F);
            m_u_down[i]=u2;
            m_a_down.append(1-u2);
            V = u2 * m_velocity_equil.at(i);
            m_velocity_down.append(V);
            m_velocity_wake.append((2*m_u_down.at(i)-1)*m_velocity_equil.at(i));
            X = m_radius_local.at(i)*omega/V;
            m_lambda_down.append(X);
            // azimuthal blade data
            for (int l=0; l<36; l++)
            {
                it_loc[l] = m_iterations[i].at(l);
                vrel_loc[l] = m_vrel[i].at(l);
                u_loc[l] = m_u[i].at(l);
                V_loc[l] = m_V[i].at(l);
                Re_loc[l] = m_Re[i].at(l);
                DeltaRe_loc[l] = m_DeltaRe[i].at(l);
                alpha_loc[l] = m_alpha[i].at(l);
                Ftip_loc[l] = m_Ftip[i].at(l);
                CL_loc[l] = m_CL[i].at(l);
                CD_loc[l] = m_CD[i].at(l);
                LD_loc[l] = m_LD[i].at(l);
                Cn_loc[l] = m_Cn[i].at(l);
                Ct_loc[l] = m_Ct[i].at(l);
            }
            m_iterations.replace(i, it_loc);
            m_vrel.replace(i, vrel_loc);
            m_u.replace(i, u_loc);
            m_V.replace(i, V_loc);
            m_Re.replace(i, Re_loc);
            m_DeltaRe.replace(i, DeltaRe_loc);
            m_alpha.replace(i, alpha_loc);
            for (int l=0; l<72; l++) alpha_deg_loc[l] = m_alpha[i].at(l)*180.0/PI;
            m_alpha_deg.append(alpha_deg_loc);
            m_Ftip.replace(i, Ftip_loc);
            m_CL.replace(i, CL_loc);
            m_CD.replace(i, CD_loc);
            m_LD.replace(i, LD_loc);
            m_Cn.replace(i, Cn_loc);
            m_Ct.replace(i, Ct_loc);

        }//elements

    }//const/var interference factors

    // load distribution height integral
    for (int i=0; i<elements; i++)
    {
        tempCFN = tempCFNlist.at(i);
        tempCFT = tempCFTlist.at(i);
        tempT   = tempTlist.at(i);
		tempFN = tempFNlist.at(i);
		tempFT = tempFTlist.at(i);

        // add load contribution of current height for all theta
        for (int l=0; l<72; l++)
        {
            double tempTheta = m_theta_deg.at(l) + m_theta_local.at(i) - m_theta_local[int(elements/2)];
            while (tempTheta > 269.999)
                tempTheta = tempTheta - 360;
            while (tempTheta < -90.0)
                tempTheta = tempTheta + 360;

            int k=0;
            while (k<72 && m_theta_deg.at(k) < tempTheta)
                k++;

            if (k==72) k=0;

            int from, to;
            if (k==0)   from=71;
            else        from=k-1;
            to=k;

            //Delta theta = 5 deg
            double between = (tempTheta-m_theta_deg.at(from))/(m_theta_deg.at(to)-m_theta_deg.at(from));

			CFN[l] = CFN[l] + m_c_local.at(i) * (tempCFN[from] + between * (tempCFN[to]-tempCFN[from])) * deltas.at(i);
			CFT[l] = CFT[l] + m_c_local.at(i) * (tempCFT[from] + between * (tempCFT[to]-tempCFT[from])) * deltas.at(i);
            T[l]   = T[l]   + m_c_local.at(i) * (tempT[from]   + between * (tempT[to]-tempT[from]))     * deltas.at(i);
			CF_blade_length[l] = CF_blade_length[l] + m_c_local.at(i) * cos(tempTheta/360.0*2*PI)*(tempCFN[from] + between * (tempCFN[to]-tempCFN[from])) * deltas.at(i)+ m_c_local.at(i) * sin(tempTheta/360.0*2*PI) * (tempCFT[from] + between * (tempCFT[to]-tempCFT[from])) * deltas.at(i);
			CF_blade_cross[l] = CF_blade_cross[l] + m_c_local.at(i) *sin(tempTheta/360.0*2*PI)*(tempCFN[from] + between * (tempCFN[to]-tempCFN[from])) * deltas.at(i)- m_c_local.at(i) * cos(tempTheta/360.0*2*PI) * (tempCFT[from] + between * (tempCFT[to]-tempCFT[from])) * deltas.at(i);
			F_blade_length[l] = F_blade_length[l] + m_c_local.at(i) * cos(tempTheta/360.0*2*PI)*(tempFN[from] + between * (tempFN[to]-tempFN[from])) * deltas.at(i)+ m_c_local.at(i) * sin(tempTheta/360.0*2*PI) * (tempFT[from] + between * (tempFT[to]-tempFT[from])) * deltas.at(i);
			F_blade_cross[l] = F_blade_cross[l] + m_c_local.at(i) *sin(tempTheta/360.0*2*PI)*(tempFN[from] + between * (tempFN[to]-tempFN[from])) * deltas.at(i)- m_c_local.at(i) * cos(tempTheta/360.0*2*PI) * (tempFT[from] + between * (tempFT[to]-tempFT[from])) * deltas.at(i);
        }
    }

    // blade positions
    for (int m=0; m<blades; m++)
    {
        bladepos.append(m*360.0/blades);
        bladefrom.append(0);
        bladeto.append(0);
    }

    // loads and torque for each blade position
    for (int l=0; l<72; l++)
    {
        // one blade
		m_FN[l] = CFN[l]/sweptArea;
		m_FT[l] = CFT[l]/sweptArea;
		m_CF_cross[l] = CF_blade_cross[l]/sweptArea;
		m_CF_length[l] = CF_blade_length[l]/sweptArea;
		m_F_cross[l] = F_blade_cross[l] * 0.5 * rho;
		m_F_length[l] = F_blade_length[l] * 0.5 * rho;
		m_T[l]  = 0.5*rho*T[l];
    }

    for (int l=0; l<72; l++)
    {
        // all blades
        for (int m=0; m<blades; m++)
        {
            double tempTheta = m_theta_deg.at(l) + bladepos.at(m);
            while (tempTheta > 269.999)
                tempTheta = tempTheta - 360;
            while (tempTheta < -90.0)
                tempTheta = tempTheta + 360;

            int k=0;
            while (k<72 && m_theta_deg.at(k) < tempTheta)
                k++;

            if (k==72) k=0;

            int from, to;
            if (k==0)   from=71;
            else        from=k-1;
            to=k;

            //Delta theta = 5 deg
            double between = (tempTheta-m_theta_deg.at(from))/(m_theta_deg.at(to)-m_theta_deg.at(from));

            m_FN_tot[l] = m_FN_tot[l] + m_FN[from] + between * (m_FN[to] - m_FN[from]);
            m_FT_tot[l] = m_FT_tot[l] + m_FT[from] + between * (m_FT[to] - m_FT[from]);
			m_CF_cross_tot[l] = m_CF_cross_tot[l] + m_CF_cross[from] + between * (m_CF_cross[to] - m_CF_cross[from]);
			m_CF_length_tot[l] = m_CF_length_tot[l] + m_CF_length[from] + between * (m_CF_length[to] - m_CF_length[from]);
			m_F_cross_tot[l] = m_F_cross_tot[l] + m_F_cross[from] + between * (m_F_cross[to] - m_F_cross[from]);
			m_F_length_tot[l] = m_F_length_tot[l] + m_F_length[from] + between * (m_F_length[to] - m_F_length[from]);
            m_T_tot[l]  = m_T_tot[l]  + m_T[from]  + between * (m_T[to]  - m_T[from]);
        }
    }

    double uu2=0;
    for (int k=0;k<m_u_down.length();k++)
    {
        uu2 += m_u_up.at(k)*m_u_down.at(k)*deltas.at(k)/height;
    }

    // average half-torque coefficients
    for (int l=0; l<36; l++)
		CQ1 = CQ1 + m_FT[l];
    for (int l=36; l<72; l++)
        CQ2 = CQ2 + m_FT[l];

    CQ1 = blades/72 * CQ1;
    CQ2 = blades/72 * CQ2;

    // power coefficients
    CP1 = lambda_global*CQ1;
    CP2 = lambda_global*CQ2;
    CP = CP1 + CP2;

    ////////////////////////thrust coefficient test/////////////////
    //calculation of thrust coefficient Ct//
    ct  = 0;
    ct1 = 0;
    ct2 = 0;

    for (int l=0; l<36; l++)
        ct1 = ct1 + blades/72 *fabs(m_FN[l]*cos(m_theta.at(l)));
    for (int l=36; l<72; l++)
        ct2 = ct2 + blades/72 *fabs(m_FN[l]*cos(m_theta.at(l)));
    /////////////////////////////////////////////////////////////////

    // storage
    // thrust coefficients
    ct = ct1+ct2;
    // torque coefficients
    cm1=CQ1;
    cm2=CQ2;
    cm=CQ1+CQ2;
    // power coefficients
    cp1=CP1;
    cp2=CP2;
    cp=CP;

    // calculate torque and power for turbine
    if (windspeed>0)
	{
        torque = 0;
        for (int l=0; l<72; l++)
        {
            torque = torque + blades/72.0*m_T[l];
        }

//		torque -= TPA;


        power = 0;
        for (int i=0; i < elements-1; i++)
        {
            power = power + 0.5*rho*pow(0.5*(m_velocity_inf.at(i)+m_velocity_inf.at(i+1)),3)*m_area.at(i)*cp;
        }

        thrust = 0;
        for (int i=0; i < elements-1; i++)
        {
            thrust = thrust + 0.5*rho*pow(0.5*(m_velocity_inf.at(i)+m_velocity_inf.at(i+1)),2)*m_area.at(i)*ct;
        }
    }

}

void DData::serialize(){
	if (g_serializer.getArchiveFormat() >= 100032) {
		ShowAsGraphInterface::serialize();
	}

    if (g_serializer.getArchiveFormat() >= 100026) 	g_serializer.readOrWriteBool (&m_bIsInverted);

	g_serializer.readOrWriteDouble (&elements);
	g_serializer.readOrWriteDouble (&epsilon);
	g_serializer.readOrWriteDouble (&iterations);
	g_serializer.readOrWriteDouble (&relax);
	g_serializer.readOrWriteDouble (&rho);
	g_serializer.readOrWriteDouble (&visc);
	g_serializer.readOrWriteBool (&bPowerLaw);
	g_serializer.readOrWriteBool (&bConstant);
	g_serializer.readOrWriteBool (&bLogarithmic);
	g_serializer.readOrWriteDouble (&exponent);
	g_serializer.readOrWriteDouble (&roughness);
	
	g_serializer.readOrWriteDouble (&Toff);
	g_serializer.readOrWriteDouble (&windspeed);
	g_serializer.readOrWriteString (&windspeedStr);
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bVariable);
	g_serializer.readOrWriteBool (&m_bAspectRatio);
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_DMSName);
	g_serializer.readOrWriteString (&lambdaglobal);
	
	g_serializer.readOrWriteDouble (&sweptArea);
	g_serializer.readOrWriteDouble (&lambda_global);
	g_serializer.readOrWriteDouble (&max_radius);
	g_serializer.readOrWriteDouble (&max_r_pos);
	g_serializer.readOrWriteDouble (&max_r_count);
	g_serializer.readOrWriteDouble (&positions);
	g_serializer.readOrWriteInt (&m_sections);
	g_serializer.readOrWriteDouble (&h_top);
	g_serializer.readOrWriteDouble (&h_bottom);
	g_serializer.readOrWriteDouble (&height);
	g_serializer.readOrWriteDouble (&delta);
	g_serializer.readOrWriteDouble (&cp);
	g_serializer.readOrWriteDouble (&cp1);
	g_serializer.readOrWriteDouble (&cp2);
	g_serializer.readOrWriteDouble (&cm);
	g_serializer.readOrWriteDouble (&cm1);
	g_serializer.readOrWriteDouble (&cm2);
	g_serializer.readOrWriteDouble (&w);
	g_serializer.readOrWriteDouble (&power);
	g_serializer.readOrWriteDouble (&torque);
	g_serializer.readOrWriteDouble (&thrust);
	g_serializer.readOrWriteDouble (&ct);
	g_serializer.readOrWriteDouble (&ct1);
	g_serializer.readOrWriteDouble (&ct2);
	g_serializer.readOrWriteDouble (&blades);
	
	g_serializer.readOrWriteDoubleList1D (&deltas);
	
	g_serializer.readOrWriteDoubleList1D (&m_pos);
	g_serializer.readOrWriteDoubleList1D (&m_zeta);
	g_serializer.readOrWriteDoubleList1D (&m_c_local);
	g_serializer.readOrWriteDoubleList1D (&m_t_local);
	g_serializer.readOrWriteDoubleList1D (&m_delta);
	g_serializer.readOrWriteDoubleList1D (&m_radius_local);
	g_serializer.readOrWriteDoubleList1D (&m_theta_local);
	g_serializer.readOrWriteDoubleList1D (&m_eta);

    if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100027){ // compatibility after removing m_polarPointers
        QList <double> m_between;
        g_serializer.readOrWriteDoubleList1D (&m_between);
    }


	g_serializer.readOrWriteDoubleList1D (&m_lambda_up);
	g_serializer.readOrWriteDoubleList1D (&m_lambda_down);
	g_serializer.readOrWriteDoubleList1D (&m_velocity_inf);
	g_serializer.readOrWriteDoubleList1D (&m_velocity_up);
	g_serializer.readOrWriteDoubleList1D (&m_velocity_equil);
	g_serializer.readOrWriteDoubleList1D (&m_velocity_down);
	g_serializer.readOrWriteDoubleList1D (&m_velocity_wake);

	g_serializer.readOrWriteDoubleList1D (&m_u_up);
	g_serializer.readOrWriteDoubleList1D (&m_u_down);
	
	g_serializer.readOrWriteDoubleList1D (&m_a_up);
	g_serializer.readOrWriteDoubleList1D (&m_a_down);
	g_serializer.readOrWriteDoubleList1D (&m_area);
	g_serializer.readOrWriteDoubleList1D (&m_Ftip_up);
	g_serializer.readOrWriteDoubleList1D (&m_Ftip_dw);
	g_serializer.readOrWriteDoubleList1D (&m_it_up);
	g_serializer.readOrWriteDoubleList1D (&m_it_dw);
	g_serializer.readOrWriteDoubleList1D (&m_twist);
	
	g_serializer.readOrWriteDoubleList1D (&m_theta);
	g_serializer.readOrWriteDoubleList1D (&m_theta_deg);
	g_serializer.readOrWriteDoubleList1D (&m_theta_plot);
	
	g_serializer.readOrWriteDoubleList1D (&m_alpha_eq);
	g_serializer.readOrWriteDoubleList1D (&m_Re_eq);
	g_serializer.readOrWriteDoubleList1D (&m_vrel_eq);
	g_serializer.readOrWriteDoubleList1D (&m_CD_eq);
	g_serializer.readOrWriteDoubleList1D (&m_CL_eq);
	g_serializer.readOrWriteDoubleList1D (&m_LD_eq);
	g_serializer.readOrWriteDoubleList1D (&m_Cn_eq);
	g_serializer.readOrWriteDoubleList1D (&m_Ct_eq);
	g_serializer.readOrWriteDoubleList1D (&m_CFN_eq);
	g_serializer.readOrWriteDoubleList1D (&m_CFT_eq);
	g_serializer.readOrWriteDoubleList1D (&m_CF_length);
	g_serializer.readOrWriteDoubleList1D (&m_CF_cross);
	g_serializer.readOrWriteDoubleList1D (&m_CF_length_tot);
	g_serializer.readOrWriteDoubleList1D (&m_CF_cross_tot);
	
	g_serializer.readOrWriteDoubleList1D (&m_FN);
	g_serializer.readOrWriteDoubleList1D (&m_FT);
	g_serializer.readOrWriteDoubleList1D (&m_T);
	g_serializer.readOrWriteDoubleList1D (&m_F_length_tot);
	g_serializer.readOrWriteDoubleList1D (&m_F_cross_tot);
	g_serializer.readOrWriteDoubleList1D (&m_F_length);
	g_serializer.readOrWriteDoubleList1D (&m_F_cross);
	
	g_serializer.readOrWriteDoubleList1D (&m_FN_tot);
	g_serializer.readOrWriteDoubleList1D (&m_FT_tot);
	g_serializer.readOrWriteDoubleList1D (&m_T_tot);
	
	g_serializer.readOrWriteDoubleList2D (&m_iterations);
	g_serializer.readOrWriteDoubleList2D (&m_alpha);
	g_serializer.readOrWriteDoubleList2D (&m_alpha_deg);
	g_serializer.readOrWriteDoubleList2D (&m_Ftip);
	g_serializer.readOrWriteDoubleList2D (&m_u);
	g_serializer.readOrWriteDoubleList2D (&m_V);
	g_serializer.readOrWriteDoubleList2D (&m_Re);
    if (g_serializer.getArchiveFormat() >= 100027) g_serializer.readOrWriteDoubleList2D (&m_DeltaRe);



	g_serializer.readOrWriteDoubleList2D (&m_vrel);
	g_serializer.readOrWriteDoubleList2D (&m_CD);
	g_serializer.readOrWriteDoubleList2D (&m_CL);
	g_serializer.readOrWriteDoubleList2D (&m_LD);
	g_serializer.readOrWriteDoubleList2D (&m_Cn);
	g_serializer.readOrWriteDoubleList2D (&m_Ct);
	
    if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100027){ // compatibility after removing m_polarPointers
        QStringList m_polar;
        QStringList m_foil;
        QStringList m_polarTO;
        QStringList m_foilTO;
        QVector <C360Polar *> m_PolarPointers;
        QVector <C360Polar *> m_PolarPointersTO;
        g_serializer.readOrWriteStringList (&m_polar);
        g_serializer.readOrWriteStringList (&m_foil);
        g_serializer.readOrWriteStringList (&m_polarTO);
        g_serializer.readOrWriteStringList (&m_foilTO);
        g_serializer.readOrWriteStorableObjectVector (&m_PolarPointers);
        g_serializer.readOrWriteStorableObjectVector (&m_PolarPointersTO);
        for (int i=0;i<m_Re.size();i++){
              m_DeltaRe.append(m_Re.at(i));
        }
    }
	
	g_serializer.readOrWriteBool (&m_bShowPoints);
	g_serializer.readOrWriteBool (&m_bIsVisible);
	g_serializer.readOrWriteBool (&m_bBackflow);
	g_serializer.readOrWriteInt (&m_Style);
	g_serializer.readOrWriteInt (&m_Width);
	g_serializer.readOrWriteColor (&m_Color);
}

DData* DData::newBySerialize() {
	DData* dData = new DData ("<name>");
	dData->serialize();
	return dData;
}
