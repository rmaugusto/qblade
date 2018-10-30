/****************************************************************************

    Wing Class
    Copyright (C) 2005-2008 Andre Deperrois adeperrois@xflr5.com

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

#ifndef CBLADE_H
#define CBLADE_H

#include <QString>
#include <QStringList>
#include <QColor>
#include <QList>
#include <../src/XDMS/Strut.h>
class QDataStream;
		
#include "../StorableObject.h"
#include "../Objects/Surface.h"
class C360Polar;
class CFoil;

class CBlade : public StorableObject
{
public:
    int m_NPanel;
    int m_blades;
    int m_NSurfaces;
    bool m_bPlaceholder;
    bool m_bIsInverted;
    double m_PlanformSpan;
    double m_HubRadius;
    double m_sweptArea;
    double m_MaxRadius;
    QList< QList <CVector> > m_PanelPoints;

    bool m_bisSinglePolar;


	static CBlade* newBySerialize ();
	CBlade (QString name = "< no name >", StorableObject *parent = NULL);  // TODO remove the first default parameter asap!
	void serialize ();  // override from StorableObject
    void restorePointers();


	// NM new public interface
	double getRotorRadius ();
	int getNumberOfNodes ();
    int getNumberOfBlades () { return m_blades; }
	C360Polar* get360PolarAt (int position);
	double getFASTRadiusAt (int position);
	void drawCoordinateAxes ();
	
	static QStringList prepareMissingObjectMessage(bool forDMS);
	// NM end

    C360Polar* Get360Polar(QString PolarName, QString AirfoilName);
	
	CFoil* GetFoilFromStation(int k);

	void Duplicate(CBlade *pWing);
	
    void CreateSurfaces(bool isVawt = false);  // generic surface, LLT, VLM or Panel
    void CreateLLTPanels(int discType, int numPanels, bool isVawt);

    void ComputeGeometry();

    void ScaleTwist(double Twist);
    void ScaleSpan(double NewSpan);
    void ScaleChord(double NewChord);
    double computeHimmelskamp(double Cl, double radius, double AoA, double chord, double TSR, double slope, double alpha_zero);

    QList<double> getBladeParameters(double radius, double AoA, bool interpolate = true, double Re = 0, bool himmelskamp = false, double TSR = 0, bool singlePolar = false);
    QList<double> getStrutParameters(int numStrut, double AoA, double Re);

	/* NM TODO this two lists should be used instead of the string lists. The replacing will be done after the
	   unused XFoil classes are removed because the needed changes should be diminished */
    QStringList m_Airfoils;
	QStringList m_Polar;
    QStringList m_Range;

    //variables used to store multi polar assginment
    QList< QStringList > m_MultiPolars;
    QStringList m_PolarAssociatedFoils;
    QStringList m_MinMaxReynolds;
    //

    QList<VortexNode> m_BladeNodes; // stores undeflected blade nodes


	QColor m_WingColor;

    QList<double> TThickness, TOffsetX, TPos, TOffsetZ, TChord, TDihedral, TCircAngle, TFoilPAxisX, TFoilPAxisZ, TTwist;

	
	int m_NXPanels[MAXSPANSECTIONS+1]; 		// VLM Panels along chord, for each Wing Panel
    int m_NYPanels[MAXSPANSECTIONS+1]; 		// VLM Panels along span, for each Wing Panel

    double m_TChord[MAXSPANSECTIONS+1];		// Chord length at each panel side
    double m_TLength[MAXSPANSECTIONS+1];		// the length of each panel
    double m_TPos[MAXSPANSECTIONS+1];		// b-position of each panel end on developed surface
    double m_TCircAngle[MAXSPANSECTIONS+1];		// b-position of each panel end projected on horizontal surface
    double m_TOffsetX[MAXSPANSECTIONS+1];		// b-position of each panel end
    double m_TDihedral[MAXSPANSECTIONS+1];	// b-position of each panel end
    double m_TTwist[MAXSPANSECTIONS+1];		//Twist value of each foil (measured to the wing root)
    double m_TPAxisX[MAXSPANSECTIONS+1];    //Definition of the Pitch Axis - Offset in X Direction
    double m_TOffsetZ[MAXSPANSECTIONS+1];    //Definition of the Pitch Axis - Offset in Z Direction
    double m_TPAxisY[MAXSPANSECTIONS+1];    //Definition of the Pitch Axis - Offset in Y Direction
    double m_TFoilPAxisX[MAXSPANSECTIONS+1];    //Definition of the Pitch Axis - Offset in X Direction
    double m_TFoilPAxisZ[MAXSPANSECTIONS+1];    //Definition of the Pitch Axis - Offset in Z Direction
    double m_TRelPos[MAXSPANSECTIONS+1];        //relative position, starting at blade root
	
    QList <CSurface> m_Surface ;  // no need to serialize
    QVector <Strut*> m_StrutList;

};

#endif

