/****************************************************************************

    BData Class
        Copyright (C) 2010 David Marten qblade@web.de

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

#include "BData.h"
#include <QDebug>
#include <math.h>
#include "../Objects/Polar.h"
#include "../Globals.h"
#include "../XGlobals.h"
#include "../Store.h"
#include "../Serializer.h"


BData::BData()
{
    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;

    m_bTipLoss = false;
    m_bRootLoss= false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
    m_bNewRootLoss = false;
    m_bNewTipLoss = false;
    m_bCdReynolds = false;

    elements    =   100;
    epsilon     =   0.001;
    iterations  =   1000;
    relax       =   0.4;
}

QString BData::GetWindspeed(){
    return windspeedStr;
}


C360Polar* BData::Get360Polar(QString m_FoilName, QString PolarName)
{
//TODO:: needs to be fixed

		C360Polar *pPolar;
        for (int i=0; i < g_360PolarStore.size() ; i++)
        {
                pPolar = g_360PolarStore.at(i);
				if (pPolar->m_airfoil->getName() == m_FoilName && pPolar->getName() == PolarName)
                {
                        return pPolar;

                }
        }
        return NULL;
}

void BData::Init(CBlade *pWing, double lambda)
{

    m_between.clear();
    m_pos.clear();
    m_c_local.clear();
    m_lambda_local.clear();
    m_theta.clear();
    m_polar.clear();
    m_polarTO.clear();
    m_foil.clear();
    m_foilTO.clear();
    deltas.clear();

    //discretization of a wing

    m_WingName = pWing->getName();
    lambda_global=lambda;
    outer_radius = pWing->m_TPos[pWing->m_NPanel];
    inner_radius = pWing->m_TPos[0];
    length=outer_radius-inner_radius;

    blades = double(pWing->m_blades);

    lambdaglobal.sprintf("%.2f",lambda_global);

    double between=0;
    double chord=0;
    double lambdalocal=0;
    double theta=0;
    double sections = elements+1;
    double deltaangle = PI/sections;
    double tot=0;
    double thisdelta=0;
    double position=0;

    //this is the sinusoidal spacing of the elements
    for (double i=deltaangle;i<=PI;i=i+deltaangle)
    {
        dist.append(sin(i));
        tot = tot + sin(i);
    }
    thisdelta = length/tot;


    for (int i=0;i<dist.size();i++)
    {
        //deltas hold the width of each element
        deltas.append(thisdelta*dist.at(i));
    }

    //the root radius of the wing as first position
    position = pWing->m_TPos[0];

    //now the discretization starts
    for (int i=0;i<elements;i++)
    {

        //the positions mark the centers of each element
        position=position+deltas[i]/2;

        for (int j=0;j<pWing->m_NPanel+1;j++)
        {

        //here it is computed between which stations the current element center lies
        if (position >= pWing->m_TPos[j] && position <= pWing->m_TPos[j+1])
            {
            from=j;
            to=j+1;
            }
        }
    //between stores how close the element is to the next station (between = 1 -> on the next station, between = 0 -> on the last station)
    between = (position-pWing->m_TPos[from]) / (pWing->m_TPos[to]-pWing->m_TPos[from]);
    //the chord for every element
    chord = pWing->m_TChord[from]+between*(pWing->m_TChord[to]-pWing->m_TChord[from]);
    //the local tip speed ratio for every element
    lambdalocal = lambda_global*position / outer_radius;
    //the build angle for every element
    theta = pWing->m_TTwist[from]+between*(pWing->m_TTwist[to]-pWing->m_TTwist[from]);

    m_between.append(double(1)-between);
    m_pos.append(position);
    m_c_local.append(chord);
    m_lambda_local.append(lambdalocal);
    m_theta.append(theta);
    //this stores a pointer to the polar from the last section
    m_polar.append(pWing->m_Polar[from]);
    //this stores a pointer to the polar at the next section
    m_polarTO.append(pWing->m_Polar[to]);
    //this stores a pointer to the foil from the last section
	m_foil.append(pWing->m_Airfoils[from]);
    //this stores a pointer to the foil from the next section
	m_foilTO.append(pWing->m_Airfoils[to]);


    position=position+deltas[i]/2;

	}

}



void BData::OnBEM(double pitch)
{


double a_a, a_a_old,a_a_older;
double a_r, a_r_old;
double phi, alpha, Fl;
double eps, CL, CD, Cn, Ct, CL2, CD2, CT;
double DeltaAlpha, sigma, localReynolds;
int i;
double F, Reynolds;

windspeedStr.sprintf("%.2f",windspeed);

m_PolarPointers.clear();

//every element gets a pointer assigned to the polar at the section before (m_PolarPointers)
//the elements center and after the elements center (m_PolarPointersTO)
for (int i=0;i<m_polar.size();i++)
{
    m_PolarPointers.append(Get360Polar(m_foil.at(i),m_polar.at(i)));
    m_PolarPointersTO.append(Get360Polar(m_foilTO.at(i),m_polarTO.at(i)));
}

//now the first loop starts over all elements
for (i=0;i<m_pos.size();i++)
{
    //the induction factors and epsilon are initialized
    eps = 10000;
    a_a=0;
    a_r=0;
    a_a_old=0;
    a_a_older=0;
    a_r_old=0;

    //the counter for the number of iterations is set to zero
    int count =0;

    //this is the criterion for an iteration to converge
    while (eps > epsilon)
    {

    //when the maximum number of iterations is reached the iterations are stopped
        count++;
        if (count == iterations)
        {
            break;
        }

        //the angle phi is computed
        phi = atan( (1-a_a)/(1+a_r) / m_lambda_local.at(i) )/2/PI*360;

        //alpha is computed
        alpha=phi-m_theta.at(i)-pitch;

        while (alpha < -180 ) alpha+=360;
        while (alpha > 180 ) alpha-=360;



        bool found=false;
        bool found2=false;

        //here the polar data from the polar at the last section is extracted
        for (int j=0; j< m_PolarPointers.at(i)->m_Alpha.size();j++)
        {

            if (m_PolarPointers.at(i)->m_Alpha.at(j) >= alpha && !found)
            {


                DeltaAlpha = m_PolarPointers.at(i)->m_Alpha.at(j)-m_PolarPointers.at(i)->m_Alpha.at(j-1);


                CL = m_PolarPointers.at(i)->m_Cl.at(j-1)+(m_PolarPointers.at(i)->m_Cl.at(j)-m_PolarPointers.at(i)->m_Cl.at(j-1))/DeltaAlpha*(alpha-m_PolarPointers.at(i)->m_Alpha.at(j-1));
                CD = m_PolarPointers.at(i)->m_Cd.at(j-1)+(m_PolarPointers.at(i)->m_Cd.at(j)-m_PolarPointers.at(i)->m_Cd.at(j-1))/DeltaAlpha*(alpha-m_PolarPointers.at(i)->m_Alpha.at(j-1));

                if (m_bCdReynolds)
                {
                    localReynolds = pow(pow(windspeed,2)+pow(m_lambda_local.at(i)*windspeed,2),0.5)*m_c_local.at(i)/visc*rho;
                    CD = CD * pow(m_PolarPointers.at(i)->reynolds/localReynolds,0.2);
                }


                Reynolds = m_PolarPointers.at(i)->reynolds;

                //implementation the 3D correction after SNEL
                if (m_b3DCorrection && m_pos.at(i) <= outer_radius*0.8 && alpha >= m_PolarPointers.at(i)->alpha_zero && CL > 0)
                {
//					qDebug() << m_PolarPointers.at(i)->alpha_zero << m_PolarPointers.at(i)->m_FoilName << m_PolarPointers.at(i)->slope;

                    double blend = 0;
                    if (alpha < 30 && alpha >= m_PolarPointers.at(i)->alpha_zero) blend = 1;
                    else if (alpha <= 50 && alpha >m_PolarPointers.at(i)->alpha_zero) blend = 1-(alpha-30)/20;

                    if ((2*PI*(alpha-m_PolarPointers.at(i)->alpha_zero)/360*2*PI-CL)>0) CL = CL + (double(3.1)*pow(m_lambda_local.at(i),2))/(double(1)+pow(m_lambda_local.at(i),2))*blend*pow((m_c_local.at(i)/m_pos.at(i)),2)*(m_PolarPointers.at(i)->slope*(alpha-m_PolarPointers.at(i)->alpha_zero)-CL);
                }
                found = true;
            }
        }

        //here the polar data from the polar at the next section is extracted
		if (m_bInterpolation && m_PolarPointers.at(i)->getName() != m_PolarPointersTO.at(i)->getName())
        {
            for (int k=0; k< m_PolarPointersTO.at(i)->m_Alpha.size();k++)
            {

                if (m_PolarPointersTO.at(i)->m_Alpha.at(k) >= alpha && !found2)
                {

                    DeltaAlpha = m_PolarPointersTO.at(i)->m_Alpha.at(k)-m_PolarPointersTO.at(i)->m_Alpha.at(k-1);


                    CL2 = m_PolarPointersTO.at(i)->m_Cl.at(k-1)+(m_PolarPointersTO.at(i)->m_Cl.at(k)-m_PolarPointersTO.at(i)->m_Cl.at(k-1))/DeltaAlpha*(alpha-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));
                    CD2 = m_PolarPointersTO.at(i)->m_Cd.at(k-1)+(m_PolarPointersTO.at(i)->m_Cd.at(k)-m_PolarPointersTO.at(i)->m_Cd.at(k-1))/DeltaAlpha*(alpha-m_PolarPointersTO.at(i)->m_Alpha.at(k-1));

                    if (m_bCdReynolds)
                    {
                        localReynolds = pow(pow(windspeed,2)+pow(m_lambda_local.at(i)*windspeed,2),0.5)*m_c_local.at(i)/visc*rho;
                        CD2 = CD2 * pow(m_PolarPointersTO.at(i)->reynolds/localReynolds,0.2);
                    }


                    //implementation the 3D correction after SNEL
                    if (m_b3DCorrection && m_pos.at(i) <= outer_radius*0.8 && alpha >= m_PolarPointers.at(i)->alpha_zero && CL > 0)
					{
    //					qDebug() << m_PolarPointers.at(i)->alpha_zero << m_PolarPointers.at(i)->m_FoilName << m_PolarPointers.at(i)->slope;

						double blend = 0;
                        if (alpha < 30 && alpha >= m_PolarPointers.at(i)->alpha_zero) blend = 1;
                        else if (alpha <= 50 && alpha >m_PolarPointers.at(i)->alpha_zero) blend = 1-(alpha-30)/20;

                        if ((2*PI*(alpha-m_PolarPointers.at(i)->alpha_zero)/360*2*PI-CL2)>0)
                        CL2 = CL2 + (double(3.1)*pow(m_lambda_local.at(i),2))/(double(1)+pow(m_lambda_local.at(i),2))*blend*pow((m_c_local.at(i)/m_pos.at(i)),2)*(m_PolarPointers.at(i)->slope*(alpha-m_PolarPointers.at(i)->alpha_zero)-CL2);
					}
                    //here the data is interpolated between the two polars if foil interpolation is on
                    Reynolds = m_PolarPointers.at(i)->reynolds*m_between.at(i)+(1-m_between.at(i))*m_PolarPointersTO.at(i)->reynolds;
                    double newCL = CL*m_between.at(i)+(1.0-m_between.at(i))*CL2;
                    double newCD = CD*m_between.at(i)+(1.0-m_between.at(i))*CD2;

                    CL = newCL;
                    CD = newCD;
                found2 = true;
                }
            }




        }

        //computation of normal and thrust coefficient
        Cn=CL*cos(phi/360*2*PI)+CD*sin(phi/360*2*PI);
        Ct=CL*sin(phi/360*2*PI)-CD*cos(phi/360*2*PI);

        //computation of solidity

        sigma = m_c_local.at(i)*fabs(cos(m_theta.at(i)/360*2*PI))*blades/2/PI/m_pos.at(i);


        //the old induction factors are stored to compute the convergence criterion
        a_a_older=a_a_old;
        a_a_old=a_a;
        a_r_old=a_r;

        //computation of the PRANDTL tip loss factor
        F=1;

        if (m_bTipLoss || m_bNewTipLoss)
        {
        double f=sin(phi/360*2*PI);
        double g=(outer_radius-m_pos.at(i))/m_pos.at(i);
        double Ft=2/PI*acos(exp(-blades/2*fabs(g/f)));
        F = F*Ft;
        }

        if(m_bRootLoss || m_bNewRootLoss)
        {
        double f=sin(phi/360*2*PI);
        double g=(m_pos.at(i)-inner_radius)/m_pos.at(i);
        double Fr=2/PI*acos(exp(-blades/2*fabs(g/f)));
        F = F*Fr;
        }


        //here the new tip loss model is implemented
        Fl = 1;

        if (m_bNewTipLoss || m_bNewRootLoss)
        {
            if (m_bNewTipLoss)
            {
                double f=sin(phi/360*2*PI);
                double g=(outer_radius-m_pos.at(i))/m_pos.at(i);
                double Flt = 2/PI*acos(exp(-blades/2*fabs(g/f)*(exp(-0.15*(blades*m_lambda_local[i]-21))+0.1)));
                Fl = Fl * Flt;
            }


            if (m_bNewRootLoss)
            {
                double f=sin(phi/360*2*PI);
                double g=(m_pos.at(i)-inner_radius)/m_pos.at(i);
                double Flt = 2/PI*acos(exp(-blades/2*fabs(g/f)*(exp(-0.15*(blades*m_lambda_local[i]-21))+0.1)));
                Fl = Fl * Flt;
            }

            if (m_bNewTipLoss || m_bNewRootLoss)
            {
                Cn = Fl * Cn;
                Ct = Fl * Ct;
            }
        }

        //computation of the local thrust coefficient
        CT = sigma*pow(1-a_a,2)*Cn/pow(sin(phi/360*2*PI),2);

        //computation of the axial induction factor
		if (CT <= 0.96*F)
		{
					a_a=1/((4*F*pow(sin(phi/360*2*PI),2))/(sigma*Cn)+1);
        }
		else
		{
			a_a = (18*F-20-3*pow(fabs(CT*(50-36*F)+12*F*(3*F-4)),0.5))/(36*F-50);
		}

        //computation of the tangential induction factor
//        a_r=1/((4*cos(phi/360*2*PI)*sin(phi/360*2*PI))/(sigma*Ct)-1);

        a_r = 0.5 * (pow(fabs(1+4/pow(m_lambda_local.at(i),2)*a_a*(1-a_a)),0.5)-1);


        //imlementation of the relaxation factor
        if (count <10)
        {
        a_a=a_a;
        }
        if (count <11)
        {
        a_a=0.25*a_a+0.5*a_a_old+0.25*a_a_older;
        }
        else
        {
        a_a=relax*a_a+(1-relax)*a_a_old;
        }

        //computation of epsilon
        if (fabs(a_a-a_a_old)>fabs(a_r-a_r_old))
        {
            eps=fabs(a_a-a_a_old);
        }
        else
        {
            eps=fabs(a_r-a_r_old);
        }


    }




    //now results are appended in the arrays, if the results are computed later, during a
    //turbine simulation a zero as placeholder is appended
    m_a_axial.append(a_a);
    m_a_tangential.append(a_r);
    m_Fa_axial.append(F*a_a);
    m_Fa_radial.append(F*a_r);
    double Vrel2 = (pow((1-m_a_axial.at(i)),2)+pow(m_lambda_local.at(i)*(1+m_a_tangential.at(i)),2));
	m_p_normal.append(Cn*0.5*Vrel2*m_c_local.at(i));
    m_p_tangential.append(Ct*0.5*Vrel2*m_c_local.at(i));
    m_phi.append(phi);
    m_alpha.append(phi-m_theta.at(i)-pitch);
    m_CL.append(CL);
    m_CD.append(CD);
    m_LD.append(CL/CD);
    m_Cn.append(Cn);
    m_Ct.append(Ct);
    m_F.append(F);
    m_Reynolds.append(pow(Vrel2,0.5)*m_c_local.at(i));
    m_DeltaReynolds.append(Reynolds);
    m_Roughness.append(0);
    m_Windspeed.append(0);
    m_Iterations.append(count);
    m_Mach.append(0);
    m_circ.append(0);

}


    //calculation of power coefficient Cp//
    double power=0, windenergy, thrust;
    for (int i=0;i<m_pos.size();i++)
    {
        power = power + m_pos.at(i)*m_p_tangential.at(i)*deltas.at(i);
    }

    power=power*blades*lambda_global/outer_radius;
    windenergy= PI/2*pow(outer_radius,2);
    cp = power/windenergy;

//    if (cp < 0) cp = 0;


    //calculation of thrust coefficient Ct//
    power=0;
    for (int i=0;i<m_pos.size();i++)
    {
       power = power + m_p_normal.at(i)*deltas.at(i)*blades;
    }

    thrust = PI/2*pow(outer_radius,2);
    ct = power/thrust;

    if (ct < 0) ct = 0;



}


void BData::Serialize(QDataStream &ar, bool bIsStoring)
{
	int i,n,j;
	float f;
	QString strong;
	
	if(bIsStoring)
	{
		
		n=m_pos.size();
		
		if (m_bIsVisible)  ar<<1; else ar<<0;
		if (m_bShowPoints) ar<<1; else ar<<0;
		ar << (int) m_Style;
		ar << (int) m_Width;
		ar << (float) elements;
		ar << (float) rho;
		ar << (float) epsilon;
		ar << (float) iterations;
		ar << (float) relax;
		//        if (m_bTipLoss) ar << 1; else ar<<0;
		//        if (m_bRootLoss) ar << 1; else ar<<0;
		//        if (m_b3DCorrection) ar << 1; else ar<<0;
		WriteCOLORREF(ar,m_Color);
		WriteCString(ar,m_WingName);
		WriteCString(ar,m_BEMName);
		WriteCString(ar,lambdaglobal);
		WriteCString(ar,windspeedStr);
		
		ar << (int) n;
		
		for (i=0;i<n;i++)
		{
			ar << (float) m_pos[i] << (float) m_c_local[i] << (float) m_lambda_local[i] << (float) m_p_tangential[i];
            ar << (float) m_p_normal[i] << (float) m_a_axial[i] << (float) m_a_tangential[i] << (float) m_theta[i];
			ar << (float) m_alpha[i] << (float) m_phi[i] << (float) m_CL[i] << (float) m_CD[i] << (float) m_LD[i];
			ar << (float) m_Cn[i] << (float) m_Ct[i] << (float) m_F[i] << (float) m_Reynolds[i] << (float) m_DeltaReynolds[i] << (float) m_Roughness[i] << (float) m_Windspeed[i];
			ar << (float) m_Iterations[i] << (float) m_Mach[i] << (float) m_Fa_axial[i] << (float) m_Fa_radial[i] << (float) m_circ[i];
			
			
//			strong = m_polar[i];
//			WriteCString(ar,strong);
//			strong = m_foil[i];
//			WriteCString(ar,strong);
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
//		ar >> f;
//		if (f) m_bTipLoss = true; else m_bTipLoss = false;
//		ar >> f;
//		if (f) m_bRootLoss = true; else m_bRootLoss = false;
//		ar >> f;
//		if (f) m_b3DCorrection = true; else m_b3DCorrection = false;
		ReadCOLORREF(ar,m_Color);
		ReadCString(ar,m_WingName);
		ReadCString(ar,m_BEMName);
		ReadCString(ar,lambdaglobal);
		ReadCString(ar,windspeedStr);
		
		ar >> n;
		for (i=0;i<n;i++)
		{
			ar >> f;
			m_pos.append(f);
			ar >> f;
			m_c_local.append(f);
			ar >> f;
			m_lambda_local.append(f);
			ar >> f;
			m_p_tangential.append(f);
			ar >> f;
			m_p_normal.append(f);
			ar >> f;
			m_a_axial.append(f);
			ar >> f;
            m_a_tangential.append(f);
			ar >> f;
			m_theta.append(f);
			ar >> f;
			m_alpha.append(f);
			ar >> f;
			m_phi.append(f);
			ar >> f;
			m_CL .append(f);
			ar >> f;
			m_CD.append(f);
			ar >> f;
			m_LD.append(f);
			ar >> f;
			m_Cn.append(f);
			ar >> f;
			m_Ct.append(f);
			ar >> f;
			m_F.append(f);
			ar >> f;
			m_Reynolds.append(f);
			ar >> f;
			m_DeltaReynolds.append(f);
			ar >> f;
			m_Roughness.append(f);
			ar >> f;
			m_Windspeed.append(f);
			ar >> f;
			m_Iterations.append(f);
			ar >> f;
			m_Mach.append(f);
			ar >> f;
			m_Fa_axial.append(f);
			ar >> f;
			m_Fa_radial.append(f);
			ar >> f;
			m_circ.append(f);
//			ar >> f;
//			m_between.append(f);

//			ReadCString(ar,strong);
//			m_polar.append(strong);
//			ReadCString(ar,strong);
//			m_foil.append(strong);
		}
	}
}

void BData::serialize() {
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bRootLoss);
	g_serializer.readOrWriteBool (&m_b3DCorrection);
	g_serializer.readOrWriteBool (&m_bInterpolation);
	g_serializer.readOrWriteBool (&m_bNewRootLoss);
	g_serializer.readOrWriteBool (&m_bNewTipLoss);
	g_serializer.readOrWriteBool (&m_bCdReynolds);
	
	g_serializer.readOrWriteDouble (&elements);
	g_serializer.readOrWriteDouble (&epsilon);
	g_serializer.readOrWriteDouble (&iterations);
	g_serializer.readOrWriteDouble (&relax);
	g_serializer.readOrWriteDouble (&rho);
	g_serializer.readOrWriteDouble (&visc);
	
	g_serializer.readOrWriteDouble (&windspeed);
	g_serializer.readOrWriteString (&windspeedStr);
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_BEMName);
	g_serializer.readOrWriteString (&lambdaglobal);
	
	g_serializer.readOrWriteDouble (&lambda_global);
	g_serializer.readOrWriteDouble (&length);
	g_serializer.readOrWriteDouble (&outer_radius);
	g_serializer.readOrWriteDouble (&inner_radius);
	g_serializer.readOrWriteDouble (&cp);
	g_serializer.readOrWriteDouble (&ct);
	g_serializer.readOrWriteInt (&from);
	g_serializer.readOrWriteInt (&to);
	g_serializer.readOrWriteDouble (&blades);
	
	g_serializer.readOrWriteDoubleList1D (&m_pos);
	g_serializer.readOrWriteDoubleList1D (&m_c_local);
	g_serializer.readOrWriteDoubleList1D (&m_lambda_local);
	g_serializer.readOrWriteDoubleList1D (&m_p_tangential);
	g_serializer.readOrWriteDoubleList1D (&m_p_normal);
	g_serializer.readOrWriteDoubleList1D (&m_a_axial);
    g_serializer.readOrWriteDoubleList1D (&m_a_tangential);
	g_serializer.readOrWriteDoubleList1D (&m_Fa_axial);
	g_serializer.readOrWriteDoubleList1D (&m_Fa_radial);
	g_serializer.readOrWriteDoubleList1D (&m_circ);
	g_serializer.readOrWriteDoubleList1D (&m_theta);
	g_serializer.readOrWriteDoubleList1D (&m_alpha);
	g_serializer.readOrWriteDoubleList1D (&m_phi);
	g_serializer.readOrWriteDoubleList1D (&m_CL);
	g_serializer.readOrWriteDoubleList1D (&m_CD);
	g_serializer.readOrWriteDoubleList1D (&m_LD);
	g_serializer.readOrWriteDoubleList1D (&m_Cn);
	g_serializer.readOrWriteDoubleList1D (&m_Ct);
	g_serializer.readOrWriteDoubleList1D (&m_F);
	g_serializer.readOrWriteDoubleList1D (&m_between);
	g_serializer.readOrWriteDoubleList1D (&m_Reynolds);
	g_serializer.readOrWriteDoubleList1D (&m_DeltaReynolds);
	g_serializer.readOrWriteStringList (&m_polar);
	g_serializer.readOrWriteStringList (&m_foil);
	g_serializer.readOrWriteStringList (&m_polarTO);
	g_serializer.readOrWriteStringList (&m_foilTO);
	g_serializer.readOrWriteDoubleList1D (&m_Roughness);
	g_serializer.readOrWriteDoubleList1D (&m_Windspeed);
	g_serializer.readOrWriteDoubleList1D (&m_Iterations);
	g_serializer.readOrWriteDoubleList1D (&m_Mach);
	
	g_serializer.readOrWriteStorableObjectVector (&m_PolarPointers);	
	g_serializer.readOrWriteStorableObjectVector (&m_PolarPointersTO);	
	
	g_serializer.readOrWriteDoubleList1D (&deltas);
	g_serializer.readOrWriteDoubleList1D (&dist);
	
	g_serializer.readOrWriteBool (&m_bShowPoints);
	g_serializer.readOrWriteBool (&m_bIsVisible);
	g_serializer.readOrWriteInt (&m_Style);
	g_serializer.readOrWriteInt (&m_Width);
	g_serializer.readOrWriteColor (&m_Color);	
}

BData *BData::newBySerialize() {
	BData* bData = new BData ();
	bData->serialize();
	return bData;
}
