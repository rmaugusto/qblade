/****************************************************************************

    CSurface Class
	Copyright (C) 2005-2009 Andre Deperrois adeperrois@xflr5.com

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

//////////////////////////////////////////////////////////////////////
//
// Surface.cpp: implementation of the CSurface class
// This class is associated to a surface used in VLM calculations
// The surface is always associated to a Wing "Panel" 
// and each of the wing's panels (left and right) is associated to 1 VLM surface
// The Surface's geometry is defined by
//    - its Leading edge  : m_LA, m_LB
//    - its Trailing edge : m_TA, m_TB
//    - its left and right twist
//    - its left and right foils
//    - its Normal vector
//    - its left and right normal vectors NormalA and NormalB are 
//      the average of the normals of two continuous surfaces ; used to
//	define junction between panels
//
// A suffixes refer to the left side, B refers to the right side
// L refers to the leading side refers to the right side
// The corner points are always defined from left tip to right tip,
// i.e. from y<0 to y>0
//
//////////////////////////////////////////////////////////////////////

#include <QMessageBox>
#include <QtDebug>
#include <math.h>
#include "Surface.h"
#include "../Objects/Quaternion.h"


CVector CSurface::LA;//save time by preventing allocation & release of memory
CVector CSurface::LB;
CVector CSurface::TA;
CVector CSurface::TB;

CSurface::CSurface()
{
	m_NXPanels  = 1;
	m_NYPanels  = 2;
	m_XDistType = 0;
	m_YDistType = 3;

	m_pFoilA   = NULL;
	m_pFoilB   = NULL;


	memset(m_xPointA, 0, sizeof(m_xPointA));
	memset(m_xPointB, 0, sizeof(m_xPointB));
}



double CSurface::GetChord(int const &k)
{
	static double y1, y2;
	GetyDist(k, y1, y2);
	return GetChord((y1+y2)/2.0);
}


double CSurface::GetChord(double const &tau)
{
	//assumes LA-TB have already been loaded
	static CVector V1, V2;
	static double ChordA, ChordB;

	V1 = m_TA-m_LA;
	V2 = m_TB-m_LB;

	ChordA = V1.VAbs();
	ChordB = V2.VAbs();

	return ChordA + (ChordB-ChordA) * fabs(tau);
}


void CSurface::GetNormal(double yrel, CVector &N)
{
	N = NormalA * (1.0-yrel) + NormalB * yrel;
	N.Normalize();
}

void CSurface::GetPanel(int const &k, int const &l, enumPanelPosition const &pos)
{
	// Assumption : side points have been set for this surface
	// Loads the corner points of the panel k,l in PTA, PTB, PLA, PLB
	// Method used to generate the panels

	GetyDist(k,y1,y2);
	if(pos==MIDSURFACE)
	{
		LA.x = SideA[l+1].x * (1.0-y1) + SideB[l+1].x* y1;
		LA.y = SideA[l+1].y * (1.0-y1) + SideB[l+1].y* y1;
		LA.z = SideA[l+1].z * (1.0-y1) + SideB[l+1].z* y1;
		TA.x = SideA[l].x   * (1.0-y1) + SideB[l].x  * y1;
		TA.y = SideA[l].y   * (1.0-y1) + SideB[l].y  * y1;
		TA.z = SideA[l].z   * (1.0-y1) + SideB[l].z  * y1;
		LB.x = SideA[l+1].x * (1.0-y2) + SideB[l+1].x* y2;
		LB.y = SideA[l+1].y * (1.0-y2) + SideB[l+1].y* y2;
		LB.z = SideA[l+1].z * (1.0-y2) + SideB[l+1].z* y2;
		TB.x = SideA[l].x   * (1.0-y2) + SideB[l].x  * y2;
		TB.y = SideA[l].y   * (1.0-y2) + SideB[l].y  * y2;
		TB.z = SideA[l].z   * (1.0-y2) + SideB[l].z  * y2;
	}
	else if (pos==BOTSURFACE)
	{
		LA = SideA_B[l+1] * (1.0-y1) + SideB_B[l+1]* y1;
		TA = SideA_B[l]   * (1.0-y1) + SideB_B[l]  * y1;
		LB = SideA_B[l+1] * (1.0-y2) + SideB_B[l+1]* y2;
		TB = SideA_B[l]   * (1.0-y2) + SideB_B[l]  * y2;

		LA.x = SideA_B[l+1].x * (1.0-y1) + SideB_B[l+1].x* y1;
		LA.y = SideA_B[l+1].y * (1.0-y1) + SideB_B[l+1].y* y1;
		LA.z = SideA_B[l+1].z * (1.0-y1) + SideB_B[l+1].z* y1;
		TA.x = SideA_B[l].x   * (1.0-y1) + SideB_B[l].x  * y1;
		TA.y = SideA_B[l].y   * (1.0-y1) + SideB_B[l].y  * y1;
		TA.z = SideA_B[l].z   * (1.0-y1) + SideB_B[l].z  * y1;
		LB.x = SideA_B[l+1].x * (1.0-y2) + SideB_B[l+1].x* y2;
		LB.y = SideA_B[l+1].y * (1.0-y2) + SideB_B[l+1].y* y2;
		LB.z = SideA_B[l+1].z * (1.0-y2) + SideB_B[l+1].z* y2;
		TB.x = SideA_B[l].x   * (1.0-y2) + SideB_B[l].x  * y2;
		TB.y = SideA_B[l].y   * (1.0-y2) + SideB_B[l].y  * y2;
		TB.z = SideA_B[l].z   * (1.0-y2) + SideB_B[l].z  * y2;
	}
	else if (pos==TOPSURFACE)
	{
		LA.x = SideA_T[l+1].x * (1.0-y1) + SideB_T[l+1].x* y1;
		LA.y = SideA_T[l+1].y * (1.0-y1) + SideB_T[l+1].y* y1;
		LA.z = SideA_T[l+1].z * (1.0-y1) + SideB_T[l+1].z* y1;
		TA.x = SideA_T[l].x   * (1.0-y1) + SideB_T[l].x  * y1;
		TA.y = SideA_T[l].y   * (1.0-y1) + SideB_T[l].y  * y1;
		TA.z = SideA_T[l].z   * (1.0-y1) + SideB_T[l].z  * y1;
		LB.x = SideA_T[l+1].x * (1.0-y2) + SideB_T[l+1].x* y2;
		LB.y = SideA_T[l+1].y * (1.0-y2) + SideB_T[l+1].y* y2;
		LB.z = SideA_T[l+1].z * (1.0-y2) + SideB_T[l+1].z* y2;
		TB.x = SideA_T[l].x   * (1.0-y2) + SideB_T[l].x  * y2;
		TB.y = SideA_T[l].y   * (1.0-y2) + SideB_T[l].y  * y2;
		TB.z = SideA_T[l].z   * (1.0-y2) + SideB_T[l].z  * y2;
	}
}


void CSurface::GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos)
{
	static CVector APt, BPt;
	static double TopA, TopB, BotA, BotB;

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		TopA = m_pFoilA->GetUpperY(xArel)*GetChord(0.0);
		TopB = m_pFoilB->GetUpperY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * TopA;
		APt.y +=  Normal.y * TopA;
		APt.z +=  Normal.z * TopA;
		BPt.x +=  Normal.x * TopB;
		BPt.y +=  Normal.y * TopB;
		BPt.z +=  Normal.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		BotA = m_pFoilA->GetLowerY(xArel)*GetChord(0.0);
		BotB = m_pFoilB->GetLowerY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * BotA;
		APt.y +=  Normal.y * BotA;
		APt.z +=  Normal.z * BotA;
		BPt.x +=  Normal.x * BotB;
		BPt.y +=  Normal.y * BotB;
		BPt.z +=  Normal.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}



void CSurface::GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, CVector &PtNormal, int const &pos)
{
	static CVector APt, BPt, Nc, u;
	static double TopA, TopB, BotA, BotB, nxA, nxB, nyA, nyB, theta;
	static Quaternion q;
	
	//define the strip's normal
	GetNormal(yrel, Nc);

	//define the quaternion to rotate the unit vector (0,0,1) to Nc
	//use the dot product to get the rotation angle, and the crossproduct to get the rotation vector
	theta = acos(Nc.z);
	u.x = -Nc.y;
	u.y =  Nc.x;
	u.z =  0.0;
	q.Set(theta*180.0/PI, u);

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetUpperY(xArel, TopA, nxA, nyA);
		m_pFoilB->GetUpperY(xBrel, TopB, nxB, nyB);
		TopA *= GetChord(0.0);
		TopB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * TopA;
		APt.y +=  NormalA.y * TopA;
		APt.z +=  NormalA.z * TopA;
		BPt.x +=  NormalB.x * TopB;
		BPt.y +=  NormalB.y * TopB;
		BPt.z +=  NormalB.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetLowerY(xArel, BotA, nxA, nyA);
		m_pFoilB->GetLowerY(xBrel, BotB, nxB, nyB);
		BotA *= GetChord(0.0);
		BotB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * BotA;
		APt.y +=  NormalA.y * BotA;
		APt.z +=  NormalA.z * BotA;
		BPt.x +=  NormalB.x * BotB;
		BPt.y +=  NormalB.y * BotB;
		BPt.z +=  NormalB.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}

void CSurface::GetyDist(int const &k, double &y1, double &y2)
{
	//leading edge

	double YPanels, dk;
	YPanels = (double)m_NYPanels;
	dk      = (double)k;

	if(m_YDistType==1)
	{
		//cosine case
		y1  = 1.0/2.0*(1.0-cos( dk*PI   /YPanels));
		y2  = 1.0/2.0*(1.0-cos((dk+1)*PI/YPanels));
	}
	else if(m_YDistType==-2)
	{
		//sine case
		y1  = 1.0*(sin( dk*PI   /2.0/YPanels));
		y2  = 1.0*(sin((dk+1)*PI/2.0/YPanels));
	}
	else if(m_YDistType==2)
	{
		//-sine case
		y1  = 1.0*(1.-cos( dk*PI   /2.0/YPanels));
		y2  = 1.0*(1.-cos((dk+1)*PI/2.0/YPanels));
	}
	else
	{
		//equally spaced case
		y1 =  dk     /YPanels;
		y2 = (dk+1.0)/YPanels;
	}
}


void CSurface::Init()
{
	CVector LATB, TALB;

	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}



void CSurface::RotateX(CVector const&O, double XTilt)
{
	m_LA.RotateX(O, XTilt);
	m_LB.RotateX(O, XTilt);
	m_TA.RotateX(O, XTilt);
	m_TB.RotateX(O, XTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateX(Origin, XTilt);
	NormalA.RotateX(Origin, XTilt);
	NormalB.RotateX(Origin, XTilt);
}

void CSurface::RotateY(CVector const &O, double YTilt)
{
	m_LA.RotateY(O, YTilt);
	m_LB.RotateY(O, YTilt);
	m_TA.RotateY(O, YTilt);
	m_TB.RotateY(O, YTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateY(Origin, YTilt);
	NormalA.RotateY(Origin, YTilt);
	NormalB.RotateY(Origin, YTilt);
}


void CSurface::RotateZ(CVector const &O, double ZTilt)
{
	m_LA.RotateZ(O, ZTilt);
	m_LB.RotateZ(O, ZTilt);
	m_TA.RotateZ(O, ZTilt);
	m_TB.RotateZ(O, ZTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateZ(Origin, ZTilt);
	NormalA.RotateZ(Origin, ZTilt);
	NormalB.RotateZ(Origin, ZTilt);
}

void CSurface::RotateN(CVector const &O, double ZTilt)
{
    m_LA.RotateN(O, ZTilt);
    m_LB.RotateN(O, ZTilt);
    m_TA.RotateN(O, ZTilt);
    m_TB.RotateN(O, ZTilt);

    Normal.RotateN(O, ZTilt);
    NormalA.RotateN(O, ZTilt);
    NormalB.RotateN(O, ZTilt);
}

void CSurface::Rotate(CVector &O, CVector const &R, double ZTilt)
{
    m_LA.Rotate(O, R, ZTilt);
    m_LB.Rotate(O, R, ZTilt);
    m_TA.Rotate(O, R, ZTilt);
    m_TB.Rotate(O, R, ZTilt);

    Normal.Rotate(O, R, ZTilt);
    NormalA.Rotate(O, R, ZTilt);
    NormalB.Rotate(O, R, ZTilt);
}

void CSurface::SetNormal()
{
	static CVector LATB, TALB;
	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();

}



void CSurface::SetSidePoints()
{
	//creates the left and right tip points between which the panels will be interpolated
	int l;
	double cosdA, cosdB;

	cosdA = Normal.dot(NormalA);
	cosdB = Normal.dot(NormalB);
	chordA  = GetChord(0.0);//todo : compare with |m_LA-m_TA|
	chordB  = GetChord(1.0);

	cosdA = cosdB = 1.0;

	//SideA, SideB are mid points (VLM) or bottom points (3DPanels)
	//SideA_T, SideB_T, are top points (3DPanels);
	if(m_pFoilA && m_pFoilB)
	{
		zA = m_pFoilA->GetLowerY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetLowerY(m_xPointB[0])*chordB;
		SideA_B[0] = m_TA + NormalA * zA/cosdA;
		SideB_B[0] = m_TB + NormalB * zB/cosdB;

		zA = m_pFoilA->GetUpperY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetUpperY(m_xPointB[0])*chordB;
		SideA_T[0] = m_TA + NormalA * zA/cosdA;
		SideB_T[0] = m_TB + NormalB * zB/cosdB;

		zA = m_pFoilA->GetMidY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetMidY(m_xPointB[0])*chordB;
		SideA[0]   = m_TA + NormalA * zA/cosdA;
		SideB[0]   = m_TB + NormalB * zB/cosdB;
	}
	else
	{
		SideA[0]   = m_TA;
		SideB[0]   = m_TB;
		SideA_T[0] = m_TA;
		SideB_T[0] = m_TB;
		SideA_B[0] = m_TA;
		SideB_B[0] = m_TB;
	}

	for (l=0; l<m_NXPanels; l++)
	{
		xLA = m_xPointA[l+1];
		xLB = m_xPointB[l+1];
		xTA = m_xPointA[l];
		xTB = m_xPointB[l];

		GetPoint(xLA, xLB, 0.0, LA, 0);
		GetPoint(xTA, xTB, 0.0, TA, 0);

		GetPoint(xLA, xLB, 1.0, LB, 0);
		GetPoint(xTA, xTB, 1.0, TB, 0);

		if (m_pFoilA && m_pFoilB)
		{
			//create bottom surface side points
			zA = m_pFoilA->GetLowerY(xLA)*chordA;
			zB = m_pFoilB->GetLowerY(xLB)*chordB;
			SideA_B[l+1]   = LA + NormalA * zA/cosdA;
			SideB_B[l+1]   = LB + NormalB * zB/cosdB;

			//create top surface side points
			zA = m_pFoilA->GetUpperY(xLA)*chordA;
			zB = m_pFoilB->GetUpperY(xLB)*chordB;
			SideA_T[l+1] = LA + NormalA * zA/cosdA;
			SideB_T[l+1] = LB + NormalB * zB/cosdB;

			//create middle surface side points
			zA = m_pFoilA->GetMidY(xLA)*chordA;
			zB = m_pFoilB->GetMidY(xLB)*chordB;
			SideA[l+1]   = LA + NormalA * zA/cosdA;
			SideB[l+1]   = LB + NormalB * zB/cosdB;
		}
		else
		{
			SideA[l+1]   = LA;
			SideB[l+1]   = LB;
			SideA_T[l+1] = LA;
			SideB_T[l+1] = LB;
			SideA_B[l+1] = LA;
			SideB_B[l+1] = LB;
		}
	}

	//merge trailing edge points in case the foil has a T.E. gap

	CVector Node;

	Node = (SideA_B[0] + SideA_T[0])/2.0;
	SideA_B[0].Set(Node);
	SideA_T[0].Set(Node);

	Node = (SideB_B[0] + SideB_T[0])/2.0;
	SideB_B[0].Set(Node);
	SideB_T[0].Set(Node);
}



void CSurface::Translate(CVector const &T)
{
	m_LA.Translate(T);
	m_LB.Translate(T);
	m_TA.Translate(T);
	m_TB.Translate(T);
}




void CSurface::CreateXPoints()
{
	//
	// Creates the points at the surfaces two end sections
	// The points will be used later to create the mesh
	// the chordwise panel distribution is set i.a.w. with the flap hinges, if any;
	//
	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	if(m_pFoilA && m_pFoilA->m_bTEFlap) xHingeA=m_pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(m_pFoilB && m_pFoilB->m_bTEFlap) xHingeB=m_pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * (double)m_NXPanels*1.000123);// to avoid numerical errors if exact division
	NXFlapB = (int)((1.0-xHingeB) * (double)m_NXPanels *1.000123);

	if(m_pFoilA && m_pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(m_pFoilB && m_pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;

	NXLeadA = m_NXPanels - NXFlapA;
	NXLeadB = m_NXPanels - NXFlapB;


	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(m_XDistType==1)
			m_xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(m_XDistType==1)
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(m_XDistType==1)
			m_xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}

	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(m_XDistType==1)
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}

	m_xPointA[m_NXPanels] = 0.0;
	m_xPointB[m_NXPanels] = 0.0;
}

