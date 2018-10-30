/****************************************************************************

    CPanel Class
	Copyright (C) 2006 Andre Deperrois adeperrois@xflr5.com

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

#ifndef CPANEL_H
#define CPANEL_H


#include "Quaternion.h"
#include "CVector.h"
#include "../XLLT/VortexNode.h"

typedef enum {BOTSURFACE, MIDSURFACE, TOPSURFACE, SIDESURFACE, BODYSURFACE} enumPanelPosition;


class CPanel
{

public:
    CPanel(double vortpos, double aoapos);

	void Reset();
    void SetFrame(CVector &LA, CVector &LB, CVector &TA, CVector &TB);

	double Area;
    bool m_isConverged;

    CVector LATB, TALB;

    double m_Gamma;             //current panel circulation    
    double m_Gamma_last_iteration;         //old panel circulation from last iteration

    double m_Gamma_t_minus_1;    // the gamma values computed at the last wake timestep
    double m_Gamma_t_minus_2;    // the gamma values computed at at the previous last wake timestep

    double m_CL, m_CL_old;      //panel lift coefficient
    double m_CD;                //panel drag coefficient
    double m_CM;                //panel moment coefficient

    double m_RE;                //reynolds number at which CL and CD have been obtained

    double LALB,LBTB,TBTA,TALA;

    CVector m_V_total;          //the relative total velocity on the panel in absolute coordinates
    CVector m_V_inPlane;       //the relative total velocity in a1/a3 plane of the panel in absolute coordinates
    CVector m_V_induced;        //the velocity that is induced on the panel from wake and blade vorticity in absolute coordinates
    CVector m_V_sampled;        //rotationally sampled velocity from wind input
    CVector m_V_relative;     //velocity solely from blade rotation
    CVector m_V_tower;          //velocity induced from tower influence
    CVector m_V_Shed;       //velocity induced from shed vorticity (for DS model)

    CVector m_Store_Wake;       //stores the wake induction that was computed during the first iteration step (this does not change during the iteration as the wake is not convected/updated)


    QList<CVector> m_oldInducedVelocities; //vector containing wake induced velocities from previous timesteps used for extrapolation during sub-timestepping of llt simulations
    QList<double> m_oldPanelGamma; //vector containg circulation from previous timesteps


    CVector VortPtA;
    CVector VortPtB;
    CVector a1,a2,a3;

    double m_AoA, m_AoA_old, m_AoAQS;               //the angle of attack at the vortex position relative to the panel midline
    bool isHub, isTip;
    int fromTimestep;           //the timestep at which the panel was shed
    int fromStation;            //the blade station at which the panel was shed

    VortexNode* pLA;              //pointer to the LA node
    VortexNode* pLB;              //pointer to the LB node
    VortexNode* pTA;              //pointer to the TA node
    VortexNode* pTB;              //pointer to the TB node

    //Dynamic Stall Variables
    double Q1[3], Q2[3];            // DS iteration variables at timesteps t, (t-1) and (t-2)
    double x[3];                    // state variables DS model
    double Q1_new[3], Q2_new[3];    // DS iteration variables at timesteps t, (t-1) and (t-2)
    double x_new[3];
    bool bLEsep;                    // leading edge separation
    bool bLEsep_new;                // leading edge separation
    double Cv;                      // leading edge separation
    double Cv_old;                  // leading edge separation
    double aoa_dot;

    int tau;                        // leading edge separation
    double dx;
    double xLEV, xLEV_old;
    double s_VortexPos, s_CtrlPos;

public:

	CVector Normal; // the unit vector normal to the panel
	CVector CtrlPt; // the control point for VLM analysis or 3D/Thin panels
    CVector Old_CtrlPt; // the control point for VLM analysis or 3D/Thin panels
    CVector tangentialVector, radialVector;
    CVector LiftVector, DragVector, LiftDragVector;
    double angularPos; //the blade position in degree
    int fromBlade;
    int fromStrut;
    double twistAngle;
    double chord;
    double thickness;
    double fromBladelength;
    double radius;


};

#endif
