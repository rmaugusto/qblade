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



#ifndef CSURFACE_H
#define  CSURFACE_H
#include "Foil.h"
#include "CVector.h"
#include "Panel.h"


class CSurface
{
	friend class MainFrame;
    friend class QBEM;
    friend class CBlade;
    friend class Strut;
    friend class QLLTSimulation;

public:

	CSurface();
	void GetNormal(double yrel, CVector &N);
	void GetPanel(int const &k, int const &l, const enumPanelPosition &pos);
	void GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos=0);
	void GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, CVector &PtNormal, int const &pos=0);
	void GetyDist(int const &k, double &y1, double &y2);
	void Init();
	void RotateX(CVector const &O, double XTilt);
	void RotateY(CVector const &O, double YTilt);
	void RotateZ(CVector const &O, double ZTilt);
    void RotateN(CVector const &O, double ZTilt);
    void Rotate(CVector &O, CVector const &R, double ZTilt);

	void SetNormal();
	void SetSidePoints();
    void Translate(CVector const &T);
	void CreateXPoints();
	double GetChord(int const &k);
	double GetChord(double const &tau);

private :
	CVector SideA[MAXCHORDPANELS];	 
	CVector SideB[MAXCHORDPANELS];	 
	CVector SideA_T[MAXCHORDPANELS];	 
	CVector SideB_T[MAXCHORDPANELS];	 
	CVector SideA_B[MAXCHORDPANELS];	 
	CVector SideB_B[MAXCHORDPANELS];	 
    CVector m_LA, m_LB, m_TA, m_TB;
    CVector Normal, NormalA, NormalB, TwistAxis, TwistAxisA, TwistAxisB, RotA, RotB;

	double y1, y2, xLA, xTA, xLB, xTB;
    double chordA, chordB, zA, zB;
	double m_xPointA[MAXCHORDPANELS], m_xPointB[MAXCHORDPANELS];	//chorwise position of VLM panels
	int m_XDistType;
	int m_YDistType;


public:
	static CVector LA, LB, TA, TB;//leading and trailing corners of strip k
	int m_NYPanels;
	int m_NXPanels;	
	CFoil *m_pFoilA, *m_pFoilB; //Left and right foils
	
};
#endif
