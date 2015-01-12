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
	friend class CSurface;
	friend class CWing;
	friend class QMiarex;
	friend class CBody;
	friend class PanelAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;
    /////////new code DM/////////
    friend class CBlade;
    friend class QLLTSimulation;
    ////end new code DM//////

public:
	CPanel();

	void RotatePanel(CVector const &O, Quaternion & Qt);
	void RotateBC(CVector const &HA, Quaternion & Qt);
	void Reset();
	void SetFrame();
	void SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB);
	bool Intersect(CVector const &A, CVector const &U, CVector &I, double &dist);
	bool Invert33(double *l);
	CVector GlobalToLocal(CVector const &V);
	CVector GlobalToLocal(double const &Vx, double const &Vy, double const &Vz);
	CVector LocalToGlobal(CVector const &V);

	double Width();
	double GetArea();

protected:	
	bool m_bIsInSymPlane;
	bool m_bIsLeftPanel;
	bool m_bIsWakePanel;

	int m_iElement; // panel identification number ; used when panel array is re-arranged in non sequential order
	int m_iSym; //reference of the symetric panel, or -1 if none
	int m_iPL, m_iPR, m_iPU, m_iPD;//Panels left, rigth; upstream, downstream
	int m_iWake;//-1 if not followed by a wake panel, else equal to wake panel number
	int m_iWakeColumn;

	//Local frame of refernce
    CVector VortexPos; // the vortex mid position
    CVector Vortex; // the vortex vector
    CVector P1, P2, P3, P4;//point's local coordinates
    CVector m, l;

	double dl;
	double Area;
	double Size, SMP, SMQ;
	double lij[9];

    static CVector *s_pNode;
    static CVector smp, smq, MidA, MidB;
    static CVector ILA, ILB, ITA, ITB, T, V, W, P, LATB, TALB;

	static double s_VortexPos;//between 0 and 1; usually the vortex is positioned at the panel's quarter chord : s_VortexPos=0.25
	static double s_CtrlPos;//between 0 and 1; usually the control point is positioned at the panel's 3/4 chord : s_VortexPos=0.75
	static double det;
	static double mat[9];

    //stuff needed for LLT
private:
    double m_Gamma;             //current panel circulation
    double m_Gamma_old;         //old panel circulation from last iteration
    double m_Gamma_new;         //new computed panel circulation
    double m_GammaLALB;         //delta circulation on vortex line from Leading Edge A to Leading Edge B
    double m_GammaLBTB;         //delta circulation on vortex line from Leading Edge B to Trailing Edge B
    double m_GammaTBTA;         //delta circulation on vortex line from Trailing Edge B to Trailing Edge A
    double m_GammaTALA;         //delta circulation on vortex line from Trailing Edge A to Leading Edge A
    double m_DeltaGamma;        //difference between gamma from last iteration and current value
    double m_Gamma_Node_LB;     //interpolated delta circulation on point from all adjacend vortex lines in wake on point LB
    double m_Gamma_Node_TB;     //interpolated delta circulation on point from all adjacend vortex lines in wake on point TB
    double m_Gamma_Node_TA;     //interpolated delta circulation on point from all adjacend vortex lines in wake on point TA
    double m_Gamma_Node_LA;     //interpolated delta circulation on point from all adjacend vortex lines in wake on point LA
    double m_Gamma_lastTimestep;

    double m_CL;                //panel lift coefficient
    double m_CD;                //panel drag coefficient

    double LALB,LBTB,TBTA,TALA;

    CVector m_V_total;          //the relative total velocity on the panel in absolute coordinates
    CVector m_V_induced;        //the velocity that is induced on the panel from wake and blade vorticity in absolute coordinates

    CVector VortPtA;
    CVector VortPtB;
    CVector a1,a2,a3;

    double m_AoA;               //the angle of attack at the vortex position relative to the panel midline


    bool isHub, isTip;

    int fromTimestep;           //the timestep at which the panel was shed

    int fromStation;            //the blade station at which the panel was shed

    VortexNode* pLA;              //pointer to the LA node
    VortexNode* pLB;              //pointer to the LB node
    VortexNode* pTA;              //pointer to the TA node
    VortexNode* pTB;              //pointer to the TB node

    //end LLT stuff





public:
	enumPanelPosition m_Pos; // defines if the panel is positioned on a top, middle, bottom, side or body surface
	bool m_bIsLeading, m_bIsTrailing;
	int m_iLA, m_iLB, m_iTA, m_iTB;//Corner Node numbers
	CVector Normal; // the unit vector normal to the panel
	CVector CtrlPt; // the control point for VLM analysis or 3D/Thin panels
    CVector Old_CtrlPt; // the control point for VLM analysis or 3D/Thin panels
	CVector CollPt; // the collocation point for 3d panel analysis
	CVector VA, VB; //the left and right end points of the vortex on this panel
    double angularPos; //the blade position in degree
    int blade_number;
};

#endif
