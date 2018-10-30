/****************************************************************************

    Wing Class
    Copyright (C) 2005-2010 Andre Deperrois adeperrois@xflr5.com

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

//
// This class :
//	- defines the wing object
//	- provides the methods for the calculation of the wing geometric properties
//	- provides methods for LLT, VLM and Panel methods
//

#include <QDebug>

#include "Blade.h"
#include "../Store.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Serializer.h"
#include "BEM.h"
#include "../XDMS/DMS.h"
#include "../GLWidget.h"




CBlade* CBlade::newBySerialize() {
    CBlade* blade = new CBlade ();
    blade->serialize();
    return blade;
}

CBlade::CBlade (QString name, StorableObject *parent)
	: StorableObject (name, parent)
{
	memset(m_TPos, 0, sizeof(m_TPos));
	memset(m_TCircAngle, 0, sizeof(m_TCircAngle));
	memset(m_TChord, 0, sizeof(m_TChord));
	memset(m_TLength, 0, sizeof(m_TLength));
    memset(m_TOffsetX, 0, sizeof(m_TOffsetX));
	memset(m_TDihedral, 0, sizeof(m_TDihedral));
	memset(m_TTwist, 0, sizeof(m_TTwist));
	memset(m_NXPanels, 0, sizeof(m_NXPanels));
	memset(m_NYPanels, 0, sizeof(m_NYPanels));

    m_Range.clear();
    m_MultiPolars.clear();
    m_PolarAssociatedFoils.clear();
    m_MinMaxReynolds.clear();
    m_StrutList.clear();


    m_bisSinglePolar = true;

    m_Surface.clear();
	
    m_bIsInverted = false;

    m_WingColor =  QColor(222,222,222);
	
	m_HubRadius = 0.2;
	m_NSurfaces = 0;
	m_NPanel        =  1;
    m_NYPanels[0]   = 1;
    m_NXPanels[0]   = 1;
    m_NXPanels[1]   = 1;
	
	m_TChord[0]  =  0.2;
	m_TChord[1]  =  0.12;
	m_TLength[0] =  0;
	m_TLength[1] = 1;
    m_TOffsetX[0] = 0.0;
    m_TOffsetX[1] = 0.0;
	m_TRelPos[0] = 0.0;
	m_TRelPos[1] = 1;
	m_TPAxisX[0] = 0;
	m_TPAxisX[1] = 0;
    m_TOffsetZ[0] = 0;
    m_TOffsetZ[1] = 0;
	m_TFoilPAxisX[0]=0.25;
	m_TFoilPAxisX[1]=0.25;
	m_TFoilPAxisZ[0]=0;
	m_TFoilPAxisZ[1]=0;
	
	double length = m_TLength[0] + m_HubRadius;
	for (int i = 0; i <= MAXSPANSECTIONS; ++i) {
		length += m_TLength[i];
		m_TPos[i]     = length;
    }
	
    m_blades = 1;
	m_sweptArea = 0;
}

void CBlade::restorePointers() {
    StorableObject::restorePointers();

    for (int i = 0; i < m_StrutList.size(); ++i) {
        g_serializer.restorePointer(reinterpret_cast<StorableObject**> (&(m_StrutList[i])));
    }
}

void CBlade::serialize() {
	StorableObject::serialize();

    if (g_serializer.getArchiveFormat() >= 100027){

        if (g_serializer.isReadMode()) {
            int n = g_serializer.readInt();
            for (int i = 0; i < n; ++i) {
                QStringList list;
                g_serializer.readOrWriteStringList(&list);
                m_MultiPolars.append(list);
            }
        } else {
            g_serializer.writeInt(m_MultiPolars.size());
            for (int i = 0; i < m_MultiPolars.size(); ++i) {
                g_serializer.readOrWriteStringList(&m_MultiPolars[i]);
            }
        }

        g_serializer.readOrWriteStringList (&m_PolarAssociatedFoils);
        g_serializer.readOrWriteStringList (&m_Range);
        g_serializer.readOrWriteBool(&m_bisSinglePolar);
        g_serializer.readOrWriteStringList(&m_MinMaxReynolds);
    }

    if (g_serializer.getArchiveFormat() >= 100026) 	g_serializer.readOrWriteBool (&m_bIsInverted);
	
	g_serializer.readOrWriteStringList (&m_Airfoils);
	g_serializer.readOrWriteStringList (&m_Polar);

    if (g_serializer.m_isReadMode){
        // refill range array for old project files in case its empty
        if (!m_Range.size()) for (int i=0; i<m_Airfoils.size();i++) m_Range.append("-----");
    }
	
    g_serializer.readOrWriteInt (&m_blades);
	g_serializer.readOrWriteInt (&m_NSurfaces);
	g_serializer.readOrWriteInt (&m_NPanel);

    g_serializer.readOrWriteBool (&m_bPlaceholder);

	g_serializer.readOrWriteDouble (&m_PlanformSpan);
	g_serializer.readOrWriteDouble (&m_HubRadius);
	g_serializer.readOrWriteDouble (&m_sweptArea);
	g_serializer.readOrWriteDouble (&m_MaxRadius);
	
	g_serializer.readOrWriteColor (&m_WingColor);
	
	g_serializer.readOrWriteIntArray1D (m_NXPanels, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteIntArray1D (m_NYPanels, MAXSPANSECTIONS+1);

	g_serializer.readOrWriteDoubleArray1D (m_TChord, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TLength, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPos, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TCircAngle, MAXSPANSECTIONS+1);
    g_serializer.readOrWriteDoubleArray1D (m_TOffsetX, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TDihedral, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TTwist, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPAxisX, MAXSPANSECTIONS+1);
    g_serializer.readOrWriteDoubleArray1D (m_TOffsetZ, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPAxisY, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TFoilPAxisX, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TFoilPAxisZ, MAXSPANSECTIONS+1);
    g_serializer.readOrWriteDoubleArray1D (m_TRelPos, MAXSPANSECTIONS+1);

    if (g_serializer.getArchiveFormat() >= 100047){
        g_serializer.readOrWriteStorableObjectVector(&m_StrutList);
    }

    if (g_serializer.isReadMode()){
        for (int i=0;i<=m_NPanel;i++){
            m_TFoilPAxisZ[i] = 0;
        }
    }

}

double CBlade::getRotorRadius() {
	return m_TPos[m_NPanel];
}

int CBlade::getNumberOfNodes() {
	return m_NPanel + 1;  // increase because NPanel holds the number of sections, not the number of nodes
}

C360Polar *CBlade::get360PolarAt(int position) {
    for (int i=0; i<g_360PolarStore.size();i++){
        if (g_360PolarStore.at(i)->getName() == m_Polar[position] && g_360PolarStore.at(i)->getParent()->getParent()->getName() == m_Airfoils[position]){
            return g_360PolarStore.at(i);
        }
    }
	return NULL;
}


//TODO needs fix eventually, replacing strings with pointers in blade class
C360Polar* CBlade::Get360Polar(QString PolarName, QString AirfoilName) {
    for (int i=0; i < g_360PolarStore.size() ; i++) {
        if (g_360PolarStore.at(i)->m_airfoil->getName() == AirfoilName && g_360PolarStore.at(i)->getName() == PolarName) {
            return g_360PolarStore.at(i);
        }
    }
    return NULL;
}
QList<double> CBlade::getStrutParameters(int numStrut, double AoA, double Re){

    QList<double> params, result;

    if (m_StrutList.at(numStrut)->isMulti){
        QVector<C360Polar*> pVec= m_StrutList.at(numStrut)->m_MultiPolars;
        for (int j=0;j<pVec.size()-1;j++){
            if (Re < pVec.at(0)->reynolds){
                params = pVec.at(0)->GetPropertiesAt(AoA);
                result.append(params[0]);
                result.append(params[1]);
                result.append(Re);
                result.append(params[2]);
                result.append(params[3]);
                result.append(params[4]);
                result.append(m_StrutList.at(numStrut)->getChord());
                result.append(pVec.at(0)->alpha_zero);
                result.append(pVec.at(0)->slope);
                result.append(pVec.at(0)->m_bisDecomposed);
                result.append(params[5]);
                result.append(params[6]);
                result.append(params[7]);
                break;
            }
            else if (Re > pVec.at(pVec.size()-1)->reynolds){
                params = pVec.at(pVec.size()-1)->GetPropertiesAt(AoA);
                result.append(params[0]);
                result.append(params[1]);
                result.append(Re);
                result.append(params[2]);
                result.append(params[3]);
                result.append(params[4]);
                result.append(m_StrutList.at(numStrut)->getChord());
                result.append(pVec.at(pVec.size()-1)->alpha_zero);
                result.append(pVec.at(pVec.size()-1)->slope);
                result.append(pVec.at(pVec.size()-1)->m_bisDecomposed);
                result.append(params[5]);
                result.append(params[6]);
                result.append(params[7]);
                break;
            }
            else if(pVec.at(j)->reynolds < Re && Re < pVec.at(j+1)->reynolds){
                QList<double> par1, par2;
                par1 = pVec.at(j)->GetPropertiesAt(AoA);
                par2 = pVec.at(j+1)->GetPropertiesAt(AoA);
                params.clear();
                for (int m=0;m<par1.size();m++) params.append(par1.at(m)+(par2.at(m)-par1.at(m))*(Re-pVec.at(j)->reynolds)/(pVec.at(j+1)->reynolds-pVec.at(j)->reynolds));
                result.append(params[0]);
                result.append(params[1]);
                result.append(Re);
                result.append(params[2]);
                result.append(params[3]);
                result.append(params[4]);
                result.append(m_StrutList.at(numStrut)->getChord());
                result.append(pVec.at(j)->alpha_zero+(pVec.at(j+1)->alpha_zero-pVec.at(j)->alpha_zero)*(Re-pVec.at(j)->reynolds)/(pVec.at(j+1)->reynolds-pVec.at(j)->reynolds));
                result.append(pVec.at(j)->slope+(pVec.at(j+1)->slope-pVec.at(j)->slope)*(Re-pVec.at(j)->reynolds)/(pVec.at(j+1)->reynolds-pVec.at(j)->reynolds));
                result.append(pVec.at(j)->m_bisDecomposed && pVec.at(j+1)->m_bisDecomposed);
                result.append(params[5]);
                result.append(params[6]);
                result.append(params[7]);
                break;
            }
        }
    }
    else{
    C360Polar *polar = m_StrutList.at(numStrut)->getPolar();
    params = polar->GetPropertiesAt(AoA);
    result.append(params[0]);
    result.append(params[1]);
    result.append(Re);
    result.append(params[2]);
    result.append(params[3]);
    result.append(params[4]);
    result.append(m_StrutList.at(numStrut)->getChord());
    result.append(polar->alpha_zero);
    result.append(polar->slope);
    result.append(polar->m_bisDecomposed);
    result.append(params[5]);
    result.append(params[6]);
    result.append(params[7]);
    }


    return result;

}

QList<double> CBlade::getBladeParameters(double radius, double AoA, bool interpolate, double Re, bool himmelskamp, double TSR, bool singlePolar){

    C360Polar *polar1L = NULL, *polar1H = NULL, *polar2L = NULL, *polar2H = NULL;
    double chord1=0, chord2=0, slope1L=0, slope2L=0, alpha_zero1L=0, alpha_zero2L=0, usedReynolds1=0, usedReynolds2=0;
    QList<double> prop1L, prop1H, prop2L, prop2H;



    int pos = -1;

    QList<double> result;


    if (singlePolar){
        //find polars for interpolation with respect to blade position
        if (radius <= m_TPos[0]){
                    polar1L =  Get360Polar(m_Polar[0], m_Airfoils[0]);
                    chord1 = m_TChord[0];
        }
        else if (radius >= m_TPos[m_NPanel]){
                    polar1L =  Get360Polar(m_Polar[m_NPanel], m_Airfoils[m_NPanel]);
                    chord1 = m_TChord[m_NPanel];
        }
        else{
            for (int i=0;i<m_NPanel;i++){
                if (radius >= m_TPos[i] && radius <= m_TPos[i+1]){
                    pos = i;
                    polar1L =  Get360Polar(m_Polar[i], m_Airfoils[i]);
                    polar2L =  Get360Polar(m_Polar[i+1], m_Airfoils[i+1]);
                    chord1 = m_TChord[i];
                    chord2 = m_TChord[i+1];
                }
            }
        }
        if (polar1L) prop1L = polar1L->GetPropertiesAt(AoA);

        if (!interpolate || polar1L == polar2L) polar2L = NULL; //deactivate interpolation in case no interpolation needed or desired

        if (polar2L) prop2L = polar2L->GetPropertiesAt(AoA);

        if (polar1L && polar2L){
            double slope, alpha_zero;

            for (int i=0; i<prop1L.size()-1;i++) prop1L[i] =  prop1L[i]+(radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(prop2L[i]-prop1L[i]);

            usedReynolds1 = polar1L->reynolds+(radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(polar2L->reynolds-polar1L->reynolds);


                slope = polar1L->slope + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(polar2L->slope-polar1L->slope);
                alpha_zero = polar1L->alpha_zero + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(polar2L->alpha_zero-polar1L->alpha_zero);
                chord1 = chord1 + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(chord2-chord1);

            if (himmelskamp){
                prop1L[0] = computeHimmelskamp(prop1L[0],radius,AoA,chord1,TSR*radius/m_TPos[m_NPanel],slope,alpha_zero);
            }

            result.append(prop1L[0]);
            result.append(prop1L[1]);
            result.append(usedReynolds1);
            result.append(prop1L[2]);
            result.append(prop1L[3]);
            result.append(prop1L[4]);
            result.append(chord1);
            result.append(alpha_zero);
            result.append(slope);
            result.append(polar1L->m_bisDecomposed && polar2L->m_bisDecomposed);
            result.append(prop1L[5]);
            result.append(prop1L[6]);
            result.append(prop1L[7]);



        }
        else if (polar1L){

            if (himmelskamp){
                prop1L[0] = computeHimmelskamp(prop1L[0],radius,AoA,chord1,TSR*radius/m_TPos[m_NPanel],polar1L->slope,polar1L->alpha_zero);
            }

            usedReynolds1 = polar1L->reynolds;

            result.append(prop1L[0]);
            result.append(prop1L[1]);
            result.append(usedReynolds1);
            result.append(prop1L[2]);
            result.append(prop1L[3]);
            result.append(prop1L[4]);
            result.append(chord1);
            result.append(polar1L->alpha_zero);
            result.append(polar1L->slope);
            result.append(polar1L->m_bisDecomposed);
            result.append(prop1L[5]);
            result.append(prop1L[6]);
            result.append(prop1L[7]);


        }
        else{
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);

        }
    }
    else{
        // find polars for interpolation with respect to blade position AND reynolds number
        if (radius <= m_TPos[0]){
            chord1 = m_TChord[0];
            for (int i=0;i<m_PolarAssociatedFoils.size();i++){
                if (m_Airfoils.at(0) == m_PolarAssociatedFoils.at(i)){
                    if (Re <= Get360Polar(m_MultiPolars[i][0],m_Airfoils[0])->reynolds){
                        polar1L = Get360Polar(m_MultiPolars[i][0],m_Airfoils[0]);
                    }
                    else if (Re >= Get360Polar(m_MultiPolars[i][m_MultiPolars.at(i).size()-1],m_Airfoils[0])->reynolds){
                        polar1L = Get360Polar(m_MultiPolars[i][m_MultiPolars.at(i).size()-1],m_Airfoils[0]);
                    }
                    else{
                        for (int j=0;j<m_MultiPolars.at(i).size()-1;j++){
                            if (Re >= Get360Polar(m_MultiPolars.at(i).at(j), m_Airfoils[0])->reynolds && Re <= Get360Polar(m_MultiPolars.at(i).at(j+1), m_Airfoils[0])->reynolds){
                                polar1L = Get360Polar(m_MultiPolars.at(i).at(j), m_Airfoils[0]);
                                polar1H = Get360Polar(m_MultiPolars.at(i).at(j+1), m_Airfoils[0]);
                            }
                        }
                    }
                }
            }
        }
        else if (radius >= m_TPos[m_NPanel]){
            chord1 = m_TChord[m_NPanel];
            for (int i=0;i<m_PolarAssociatedFoils.size();i++){
                if (m_Airfoils.at(m_NPanel) == m_PolarAssociatedFoils.at(i)){

                    if (Re <= Get360Polar(m_MultiPolars[i][0],m_Airfoils[m_NPanel])->reynolds){
                        polar1L = Get360Polar(m_MultiPolars[i][0],m_Airfoils[m_NPanel]);
                    }
                    else if (Re >= Get360Polar(m_MultiPolars[i][m_MultiPolars.at(i).size()-1],m_Airfoils[m_NPanel])->reynolds){
                        polar1L = Get360Polar(m_MultiPolars[i][m_MultiPolars.at(i).size()-1],m_Airfoils[m_NPanel]);
                    }
                    else{
                        for (int j=0;j<m_MultiPolars.at(i).size()-1;j++){
                            if (Re >= Get360Polar(m_MultiPolars.at(i).at(j), m_Airfoils[m_NPanel])->reynolds && Re <= Get360Polar(m_MultiPolars.at(i).at(j+1), m_Airfoils[m_NPanel])->reynolds){
                                polar1L = Get360Polar(m_MultiPolars.at(i).at(j), m_Airfoils[m_NPanel]);
                                polar1H = Get360Polar(m_MultiPolars.at(i).at(j+1), m_Airfoils[m_NPanel]);
                            }
                        }
                    }
                }
            }
        }
        else{
            for (int i=0;i<m_NPanel;i++){
                if (radius >= m_TPos[i] && radius <= m_TPos[i+1]){
                    pos = i;
                    chord1 = m_TChord[i];
                    chord2 = m_TChord[i+1];
                    for (int j=0;j<m_PolarAssociatedFoils.size();j++){
                        if (m_Airfoils.at(i) == m_PolarAssociatedFoils.at(j)){

                            if (Re <= Get360Polar(m_MultiPolars[j][0],m_Airfoils[i])->reynolds){
                                polar1L = Get360Polar(m_MultiPolars[j][0],m_Airfoils[i]);
                            }
                            else if (Re >= Get360Polar(m_MultiPolars[j][m_MultiPolars.at(j).size()-1],m_Airfoils[i])->reynolds){
                                polar1L = Get360Polar(m_MultiPolars[j][m_MultiPolars.at(j).size()-1],m_Airfoils[i]);
                            }
                            else{
                                for (int k=0;k<m_MultiPolars.at(j).size()-1;k++){
                                    if (Re >= Get360Polar(m_MultiPolars.at(j).at(k), m_Airfoils[i])->reynolds && Re <= Get360Polar(m_MultiPolars.at(j).at(k+1), m_Airfoils[i])->reynolds){
                                        polar1L = Get360Polar(m_MultiPolars.at(j).at(k), m_Airfoils[i]);
                                        polar1H = Get360Polar(m_MultiPolars.at(j).at(k+1), m_Airfoils[i]);
                                    }
                                }
                            }
                        }
                    }
                    for (int j=0;j<m_PolarAssociatedFoils.size();j++){
                        if (m_Airfoils.at(i+1) == m_PolarAssociatedFoils.at(j)){

                            if (Re <= Get360Polar(m_MultiPolars[j][0],m_Airfoils[i+1])->reynolds){
                                polar2L = Get360Polar(m_MultiPolars[j][0],m_Airfoils[i+1]);
                            }
                            else if (Re >= Get360Polar(m_MultiPolars[j][m_MultiPolars.at(j).size()-1],m_Airfoils[i+1])->reynolds){
                                polar2L = Get360Polar(m_MultiPolars[j][m_MultiPolars.at(j).size()-1],m_Airfoils[i+1]);
                            }
                            else{
                                for (int k=0;k<m_MultiPolars.at(j).size()-1;k++){
                                    if (Re >= Get360Polar(m_MultiPolars.at(j).at(k), m_Airfoils[i+1])->reynolds && Re <= Get360Polar(m_MultiPolars.at(j).at(k+1), m_Airfoils[i+1])->reynolds){
                                        polar2L = Get360Polar(m_MultiPolars.at(j).at(k), m_Airfoils[i+1]);
                                        polar2H = Get360Polar(m_MultiPolars.at(j).at(k+1), m_Airfoils[i+1]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (polar1L){
            prop1L = polar1L->GetPropertiesAt(AoA);
            slope1L = polar1L->slope;
            alpha_zero1L = polar1L->alpha_zero;
            usedReynolds1 = polar1L->reynolds;
        }

        if (polar1H) prop1H = polar1H->GetPropertiesAt(AoA);

        if (!interpolate || polar1L == polar2L) polar2L = NULL; //deactivate interpolation in case no interpolation needed or desired

        if (polar2L){
            prop2L = polar2L->GetPropertiesAt(AoA);
            slope2L = polar2L->slope;
            alpha_zero2L = polar2L->alpha_zero;
            usedReynolds2 = polar2L->reynolds;
        }

        if (polar2H) prop2H = polar2H->GetPropertiesAt(AoA);

        //RE number interpolation
        if (polar1L && polar1H){
            for (int i=0; i<prop1L.size()-1;i++) prop1L[i] = prop1L[i] + (prop1H[i]-prop1L[i])/(polar1H->reynolds-polar1L->reynolds)*(Re-polar1L->reynolds);
            slope1L = polar1L->slope + (polar1H->slope-polar1L->slope)/(polar1H->reynolds-polar1L->reynolds)*(Re-polar1L->reynolds);
            alpha_zero1L = polar1L->alpha_zero + (polar1H->alpha_zero-polar1L->alpha_zero)/(polar1H->reynolds-polar1L->reynolds)*(Re-polar1L->reynolds);
            usedReynolds1 = polar1L->reynolds + (polar1H->reynolds-polar1L->reynolds)/(polar1H->reynolds-polar1L->reynolds)*(Re-polar1L->reynolds);
        }

        if (polar2L && polar2H){
            for (int i=0; i<prop2L.size()-1;i++) prop2L[i] = prop2L[i] + (prop2H[i]-prop2L[i])/(polar1H->reynolds-polar1L->reynolds)*(Re-polar1L->reynolds);
            slope2L = polar2L->slope + (polar2H->slope-polar2L->slope)/(polar2H->reynolds-polar2L->reynolds)*(Re-polar2L->reynolds);
            alpha_zero2L = polar2L->alpha_zero + (polar2H->alpha_zero-polar2L->alpha_zero)/(polar2H->reynolds-polar2L->reynolds)*(Re-polar2L->reynolds);
            usedReynolds2 = polar2L->reynolds + (polar2H->reynolds-polar2L->reynolds)/(polar2H->reynolds-polar2L->reynolds)*(Re-polar2L->reynolds);
        }
        //end RE interpolation

        if (polar1L && polar2L){

            double RE, slope, alpha_zero;

            for (int i=0; i<prop1L.size()-1;i++) prop1L[i] = prop1L[i]+(radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(prop2L[i]-prop1L[i]);
            RE = usedReynolds1+(radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(usedReynolds2-usedReynolds1);



                slope = slope1L + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(slope2L-slope1L);
                alpha_zero = alpha_zero1L + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(alpha_zero2L-alpha_zero1L);
                chord1 = chord1 + (radius-m_TPos[pos])/(m_TPos[pos+1]-m_TPos[pos])*(chord2-chord1);

            if (himmelskamp){
                prop1L[0] = computeHimmelskamp(prop1L[0],radius,AoA,chord1,TSR*radius/m_TPos[m_NPanel],slope,alpha_zero);
            }

            result.append(prop1L[0]);
            result.append(prop1L[1]);
            result.append(RE);
            result.append(prop1L[2]);
            result.append(prop1L[3]);
            result.append(prop1L[4]);
            result.append(chord1);
            result.append(alpha_zero);
            result.append(slope);
            result.append(polar1L->m_bisDecomposed && polar2L->m_bisDecomposed);
            result.append(prop1L[5]);
            result.append(prop1L[6]);
            result.append(prop1L[7]);


        }
        else if (polar1L){

            if (himmelskamp){
               prop1L[0] = computeHimmelskamp(prop1L[0],radius,AoA,chord1,TSR*radius/m_TPos[m_NPanel],slope1L,alpha_zero1L);
            }

            result.append(prop1L[0]);
            result.append(prop1L[1]);
            result.append(usedReynolds1);
            result.append(prop1L[2]);
            result.append(prop1L[3]);
            result.append(prop1L[4]);
            result.append(chord1);
            result.append(alpha_zero1L);
            result.append(slope1L);
            result.append(polar1L->m_bisDecomposed);
            result.append(prop1L[5]);
            result.append(prop1L[6]);
            result.append(prop1L[7]);


        }
        else{
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);
            result.append(-1);

        }
    }
    return result;
}
double CBlade::computeHimmelskamp(double Cl, double radius, double AoA, double chord, double TSR, double slope, double alpha_zero){

    if (radius <= m_TPos[m_NPanel]*0.8 && AoA >= alpha_zero && Cl > 0 && fabs(slope) > 10e-5 )
    {
        double blend = 0, CL = 0;
        if (AoA < 30 && AoA >= alpha_zero) blend = 1;
        else if (AoA <= 50 && AoA >alpha_zero) blend = 1-(AoA-30)/20;
        if ((2*PI*(AoA-alpha_zero)/360*2*PI-CL)>0){

            CL = Cl + (double(3.1)*pow(TSR,2))/(double(1)+pow(TSR,2))*blend*pow((chord/radius),2)*(slope*(AoA-alpha_zero)-Cl);
            return CL; // return modified lift
        }
    }
    return Cl; // return unmodified lift
}

double CBlade::getFASTRadiusAt(int position) {
	return (m_TPos[position] + m_TPos[position+1]) / 2;
}

void CBlade::drawCoordinateAxes() {
	// Save current OpenGL state
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	const double length = getRotorRadius();
	
	glLineWidth(2.0);
	g_mainFrame->getGlWidget()->qglColor(g_mainFrame->m_TextColor);
	
	glBegin(GL_LINES);
		glVertex3d(-length/30, 0.0, 0.0);
		glVertex3d( length/ 6, 0.0, 0.0);
	glEnd();
	g_mainFrame->getGlWidget()->overpaintText(length/5, 0.0, 0.0, "X");
	
	glBegin(GL_LINES);
		glVertex3d(0.0, -length/ 30, 0.0);
		glVertex3d(0.0,  length*1.2, 0.0);
	glEnd();
	g_mainFrame->getGlWidget()->overpaintText(0.0, length*1.25, 0.0, "Y");
	
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -length/30);
		glVertex3d(0.0, 0.0,  length/ 6);
	glEnd();
	g_mainFrame->getGlWidget()->overpaintText( 0.0, 0.0, length/5, "Z");
	
	// Restore OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

QStringList CBlade::prepareMissingObjectMessage(bool forDMS) {
	if (!forDMS && g_rotorStore.isEmpty() && g_qbem->m_pBlade == NULL) {  // NM: m_pBlade -> blade is under construction
		QStringList message = C360Polar::prepareMissingObjectMessage();
		if (message.isEmpty()) {
			message = QStringList(">>> Create a new HAWT Blade in the HAWT Blade Design Module");
		}
		message.prepend("- No HAWT Blade in Database");
		return message;
	} else if (forDMS && g_verticalRotorStore.isEmpty() && g_qdms->m_pBlade == NULL) {
		QStringList message = C360Polar::prepareMissingObjectMessage();
		if (message.isEmpty()) {
			message = QStringList(">>> Create a new VAWT Blade in the VAWT Blade Design Module");
		}
		message.prepend("- No VAWT Blade in Database");
		return message;
	} else {
		return QStringList();
	}
}

void CBlade::ComputeGeometry()
{
	m_TLength[0] = 0.0;
	for (int i = 1; i <= m_NPanel; ++i) {
		m_TLength[i] = m_TPos[i] - m_TPos[i-1];
	}
	m_PlanformSpan  = 2.0 * m_TPos[m_NPanel];
}

void CBlade::CreateLLTPanels(int discType, int numPanels, bool isVawt)
{

    // Constructs the Panel corner points for the lifting line simulation, also contructs new arrays for panel edges

    m_PanelPoints.clear();
    TOffsetX.clear();
    TPos.clear();
    TOffsetZ.clear();
    TChord.clear();
    TDihedral.clear();
    TCircAngle.clear();
    TFoilPAxisX.clear();
    TFoilPAxisZ.clear();
    TTwist.clear();

    QList<CVector> panel;

    if (discType == 0){
        for (int i=0; i<m_NPanel+1; i++){
            TPos.append(m_TPos[i]);
            TOffsetX.append(m_TOffsetX[i]);
            TThickness.append(g_mainFrame->GetFoil(m_Airfoils[i])->m_fThickness);
            TOffsetZ.append(m_TOffsetZ[i]);
            TDihedral.append(m_TDihedral[i]);
            TChord.append(m_TChord[i]);
            TCircAngle.append(m_TCircAngle[i]);
            TFoilPAxisX.append(m_TFoilPAxisX[i]);
            TFoilPAxisZ.append(m_TFoilPAxisZ[i]);
            TTwist.append(m_TTwist[i]);
        }
    }
    else if (discType == 1){
        for (int i=0;i<=numPanels;i++){
            TPos.append(m_TPos[0]+(m_TPos[m_NPanel]-m_TPos[0])/(numPanels)*i);
        }
        for (int i=0;i<TPos.size();i++){
            for (int j=0;j<m_NPanel;j++){
                if (TPos.at(i) >= m_TPos[j] && TPos.at(i) < m_TPos[j+1]){
                    TThickness.append(g_mainFrame->GetFoil(m_Airfoils[j])->m_fThickness+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(g_mainFrame->GetFoil(m_Airfoils[j+1])->m_fThickness-g_mainFrame->GetFoil(m_Airfoils[j])->m_fThickness));
                    TOffsetX.append(m_TOffsetX[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TOffsetX[j+1]-m_TOffsetX[j]));
                    TOffsetZ.append(m_TOffsetZ[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TOffsetZ[j+1]-m_TOffsetZ[j]));
                    TDihedral.append(m_TDihedral[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TDihedral[j+1]-m_TDihedral[j]));
                    TChord.append(m_TChord[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TChord[j+1]-m_TChord[j]));
                    TCircAngle.append(m_TCircAngle[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TCircAngle[j+1]-m_TCircAngle[j]));
                    TFoilPAxisX.append(m_TFoilPAxisX[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TFoilPAxisX[j+1]-m_TFoilPAxisX[j]));
                    TFoilPAxisZ.append(m_TFoilPAxisZ[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TFoilPAxisZ[j+1]-m_TFoilPAxisZ[j]));
                    TTwist.append(m_TTwist[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TTwist[j+1]-m_TTwist[j]));
                }
            }
        }
        TThickness.append(g_mainFrame->GetFoil(m_Airfoils[m_NPanel])->m_fThickness);
        TOffsetX.append(m_TOffsetX[m_NPanel]);
        TOffsetZ.append(m_TOffsetZ[m_NPanel]);
        TDihedral.append(m_TDihedral[m_NPanel]);
        TChord.append(m_TChord[m_NPanel]);
        TCircAngle.append(m_TCircAngle[m_NPanel]);
        TFoilPAxisX.append(m_TFoilPAxisX[m_NPanel]);
        TFoilPAxisZ.append(m_TFoilPAxisZ[m_NPanel]);
        TTwist.append(m_TTwist[m_NPanel]);

    }
    else{
        double pos = 0;
        double pos2 = m_TPos[0];
        for (int i=0;i<numPanels+1;i++)
        {
            pos += sin(i*PI/(numPanels+1));
        }
        for (int i=0;i<numPanels+1;i++)
        {
            pos2 += sin(i*PI/(numPanels+1))*(m_TPos[m_NPanel]-m_TPos[0])/pos;
            TPos.append(pos2);
        }

        for (int i=0;i<TPos.size();i++){
            for (int j=0;j<m_NPanel;j++){
                if (TPos.at(i) >= m_TPos[j] && TPos.at(i) < m_TPos[j+1]){
                    TOffsetX.append(m_TOffsetX[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TOffsetX[j+1]-m_TOffsetX[j]));
                    TThickness.append(g_mainFrame->GetFoil(m_Airfoils[j])->m_fThickness+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(g_mainFrame->GetFoil(m_Airfoils[j+1])->m_fThickness-g_mainFrame->GetFoil(m_Airfoils[j])->m_fThickness));
                    TOffsetZ.append(m_TOffsetZ[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TOffsetZ[j+1]-m_TOffsetZ[j]));
                    TDihedral.append(m_TDihedral[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TDihedral[j+1]-m_TDihedral[j]));
                    TChord.append(m_TChord[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TChord[j+1]-m_TChord[j]));
                    TCircAngle.append(m_TCircAngle[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TCircAngle[j+1]-m_TCircAngle[j]));
                    TFoilPAxisX.append(m_TFoilPAxisX[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TFoilPAxisX[j+1]-m_TFoilPAxisX[j]));
                    TFoilPAxisZ.append(m_TFoilPAxisZ[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TFoilPAxisZ[j+1]-m_TFoilPAxisZ[j]));
                    TTwist.append(m_TTwist[j]+(TPos.at(i)-m_TPos[j])/(m_TPos[j+1]-m_TPos[j])*(m_TTwist[j+1]-m_TTwist[j]));
                }
            }
        }
        TThickness.append(g_mainFrame->GetFoil(m_Airfoils[m_NPanel])->m_fThickness);
        TOffsetX.append(m_TOffsetX[m_NPanel]);
        TOffsetZ.append(m_TOffsetZ[m_NPanel]);
        TDihedral.append(m_TDihedral[m_NPanel]);
        TChord.append(m_TChord[m_NPanel]);
        TCircAngle.append(m_TCircAngle[m_NPanel]);
        TFoilPAxisX.append(m_TFoilPAxisX[m_NPanel]);
        TFoilPAxisZ.append(m_TFoilPAxisZ[m_NPanel]);
        TTwist.append(m_TTwist[m_NPanel]);
    }


    for (int j = 0; j < TPos.size()-1; ++j)
    {
            CVector PLA, PTA, PLB, PTB;

            if(!isVawt){
                PLA.x = TOffsetX[j]-TChord[j]*TFoilPAxisX[j];
                PLB.x = TOffsetX[j+1]-TChord[j+1]*TFoilPAxisX[j+1];
                PLA.y = TPos[j];
                PLB.y = TPos[j+1];
                PLA.z = TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j];
                PLB.z = TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1];
                PTA.x = PLA.x+TChord[j];
                PTB.x = PLB.x+TChord[j+1];
                PTA.y = PLA.y;
                PTB.y = PLB.y;
                PTA.z = PLA.z;
                PTB.z = PLB.z;
            }
            else{
                PLA.x = TChord[j]*TFoilPAxisX[j];
                PLB.x = TChord[j+1]*TFoilPAxisX[j+1];
                PLA.y = TPos[j];
                PLB.y = TPos[j+1];
                PLA.z = TOffsetX[j];
                PLB.z = TOffsetX[j+1];
                PTA.x = PLA.x-TChord[j];
                PTB.x = PLB.x-TChord[j+1];
                PTA.y = PLA.y;
                PTB.y = PLB.y;
                PTA.z = TOffsetX[j];
                PTB.z = TOffsetX[j+1];
            }

            //this section interpolated between panels to compute the pitch axis that can be tilted in the y-z plane
            CVector twistA, twistB, twist, twistb, twistf;
            if (TPos.size() == 2){
                if (!isVawt){
                    twist.Set(CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j])-CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1]));
                }
                else{
                    twist.Set(CVector(0,TPos[j],TOffsetX[j])-CVector(0,TPos[j+1],TOffsetX[j+1]));
                }
                twist.Normalize();
                twistA = twist;
                twistB = twist;
            }
            else if (j==0){
                if (!isVawt){
                    twist.Set(CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j])-CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1]));
                    twistf.Set(CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1])-CVector(0,TPos[j+2],TOffsetZ[j+2]-TChord[j+2]*TFoilPAxisZ[j+2]));
                }
                else{
                    twist.Set(CVector(0,TPos[j],TOffsetX[j])-CVector(0,TPos[j+1],TOffsetX[j+1]));
                    twistf.Set(CVector(0,TPos[j+1],TOffsetX[j+1])-CVector(0,TPos[j+2],TOffsetX[j+2]));
                }

                twist.Normalize();
                twistf.Normalize();
                twistA = twist;
                twistB = (twist+twistf)/2;
            }
            else if (j==TPos.size()-2){
                if (!isVawt){
                    twist.Set(CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j])-CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1]));
                    twistb.Set(CVector(0,TPos[j-1],TOffsetZ[j-1]-TChord[j-1]*TFoilPAxisZ[j-1])-CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j]));
                }
                else{
                    twist.Set(CVector(0,TPos[j],TOffsetX[j])-CVector(0,TPos[j+1],TOffsetX[j+1]));
                    twistb.Set(CVector(0,TPos[j-1],TOffsetX[j-1])-CVector(0,TPos[j],TOffsetX[j]));
                }

                twist.Normalize();
                twistb.Normalize();
                twistA = (twist+twistb)/2;
                twistB = twist;
            }
            else{
                if (!isVawt){
                    twist.Set(CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j])-CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1]));
                    twistf.Set(CVector(0,TPos[j+1],TOffsetZ[j+1]-TChord[j+1]*TFoilPAxisZ[j+1])-CVector(0,TPos[j+2],TOffsetZ[j+2]-TChord[j+2]*TFoilPAxisZ[j+2]));
                    twistb.Set(CVector(0,TPos[j-1],TOffsetZ[j-1]-TChord[j-1]*TFoilPAxisZ[j-1])-CVector(0,TPos[j],TOffsetZ[j]-TChord[j]*TFoilPAxisZ[j]));
                }
                else{
                    twist.Set(CVector(0,TPos[j],TOffsetX[j])-CVector(0,TPos[j+1],TOffsetX[j+1]));
                    twistf.Set(CVector(0,TPos[j+1],TOffsetX[j+1])-CVector(0,TPos[j+2],TOffsetX[j+2]));
                    twistb.Set(CVector(0,TPos[j-1],TOffsetX[j-1])-CVector(0,TPos[j],TOffsetX[j]));
                }

                twist.Normalize();
                twistf.Normalize();
                twistb.Normalize();
                twistA = (twist+twistb)/2;
                twistB = (twist+twistf)/2;
            }

            CVector RotA, RotB;
            if (!isVawt){
            RotA = CVector(TOffsetX[j], TPos[j], TOffsetZ[j]);
            RotB = CVector(TOffsetX[j+1], TPos[j+1], TOffsetZ[j+1]);
            }
            else{
            RotA = CVector(0, TPos[j], TOffsetX[j]);
            RotB = CVector(0, TPos[j+1], TOffsetX[j+1]);
            }
            ////


            //twist sections
            if (!isVawt){
                PLA.Rotate(RotA,twistA, TTwist[j]);
                PTA.Rotate(RotA,twistA, TTwist[j]);
                PLB.Rotate(RotB,twistB, TTwist[j+1]);
                PTB.Rotate(RotB,twistB, TTwist[j+1]);
            }
            else{
                PLA.Rotate(RotA,twistA, TTwist[j]-90);
                PTA.Rotate(RotA,twistA, TTwist[j]-90);
                PLB.Rotate(RotB,twistB, TTwist[j+1]-90);
                PTB.Rotate(RotB,twistB, TTwist[j+1]-90);
            }


            //circ angle (for vawt)
            PLA.RotateY(TCircAngle[j]);
            PTA.RotateY(TCircAngle[j]);
            PLB.RotateY(TCircAngle[j+1]);
            PTB.RotateY(TCircAngle[j+1]);



        panel.clear();
        panel.append(PLA);
        panel.append(PLB);
        panel.append(PTA);
        panel.append(PTB);
        m_PanelPoints.append(panel);
    }

}

void CBlade::CreateSurfaces(bool isVawt)
{
    m_Surface.clear();
	
    int j;
    CVector PLA, PTA, PLB, PTB;

    CVector O(0,0,0);

	///construction of the pitch axis
	for(j=0; j<m_NPanel;j++)
	{	
        m_TPAxisX[j]= m_TOffsetX[j];
        m_TPAxisY[j]= m_TPos[j];
        m_TPAxisX[j+1]= m_TOffsetX[j+1];
        m_TPAxisY[j+1]= m_TPos[j+1];
	}
    m_NSurfaces = m_NPanel;

	for (j = 0; j < m_NPanel; ++j)
	{
        CSurface surface;

            surface.m_pFoilA = g_mainFrame->GetFoil(m_Airfoils[j]);
            surface.m_pFoilB = g_mainFrame->GetFoil(m_Airfoils[j+1]);

            if (!isVawt){
                PLA.x = m_TOffsetX[j]-m_TChord[j]*m_TFoilPAxisX[j];
                PLB.x = m_TOffsetX[j+1]-m_TChord[j+1]*m_TFoilPAxisX[j+1];
                PLA.y = m_TPos[j];
                PLB.y = m_TPos[j+1];
                PLA.z = m_TOffsetZ[j]-m_TChord[j]*m_TFoilPAxisZ[j];
                PLB.z = m_TOffsetZ[j+1]-m_TChord[j+1]*m_TFoilPAxisZ[j+1];
                PTA.x = PLA.x+m_TChord[j];
                PTB.x = PLB.x+m_TChord[j+1];
                PTA.y = PLA.y;
                PTB.y = PLB.y;
                PTA.z = PLA.z;
                PTB.z = PLB.z;
            }
            else{
                PLA.x = m_TChord[j]*m_TFoilPAxisX[j];
                PLB.x = m_TChord[j+1]*m_TFoilPAxisX[j+1];
                PLA.y = m_TPos[j];
                PLB.y = m_TPos[j+1];
                PLA.z = m_TOffsetX[j];
                PLB.z = m_TOffsetX[j+1];
                PTA.x = PLA.x-m_TChord[j];
                PTB.x = PLB.x-m_TChord[j+1];
                PTA.y = PLA.y;
                PTB.y = PLB.y;
                PTA.z = m_TOffsetX[j];
                PTB.z = m_TOffsetX[j+1];
            }
			
            surface.m_LA.Copy(PLA);
            surface.m_TA.Copy(PTA);
            surface.m_LB.Copy(PLB);
            surface.m_TB.Copy(PTB);

            surface.SetNormal();


            if (!isVawt){
            surface.RotA = CVector(m_TOffsetX[j], m_TPos[j], m_TOffsetZ[j]);
            surface.RotB = CVector(m_TOffsetX[j+1], m_TPos[j+1], m_TOffsetZ[j+1]);
            surface.TwistAxis.Set(CVector(0,m_TPos[j], m_TOffsetZ[j])-CVector(0,m_TPos[j+1], m_TOffsetZ[j+1]));
            }
            else{
            surface.RotA = CVector(0, m_TPos[j], m_TOffsetX[j]);
            surface.RotB = CVector(0, m_TPos[j+1], m_TOffsetX[j+1]);
            surface.TwistAxis.Set(CVector(0,m_TPos[j], m_TOffsetX[j])-CVector(0,m_TPos[j+1], m_TOffsetX[j+1]));
            }
            surface.TwistAxis.Normalize();

            surface.m_NXPanels = m_NXPanels[j];
            surface.m_NYPanels = m_NYPanels[j];
			

        m_Surface.append(surface);
    }

    for (int i=0;i<m_NSurfaces;i++){
        if (m_bIsInverted){
            m_Surface[i].Normal *= -1;
            m_Surface[i].NormalB *= -1;
            m_Surface[i].NormalA *= -1;
        }
    }

    for (int i=0;i<m_Surface.size();i++){
        if (m_NSurfaces == 1){
            m_Surface[i].TwistAxisA = m_Surface[i].TwistAxis;
            m_Surface[i].TwistAxisB = m_Surface[i].TwistAxis;
            m_Surface[i].NormalA = m_Surface[i].Normal;
            m_Surface[i].NormalB = m_Surface[i].Normal;

        }
        else if (i == 0){
            m_Surface[i].TwistAxisA = m_Surface[i].TwistAxis;
            m_Surface[i].TwistAxisB = (m_Surface[i].TwistAxis+m_Surface[i+1].TwistAxis)/2;

            m_Surface[i].NormalA = m_Surface[i].Normal;
            m_Surface[i].NormalB = (m_Surface[i].Normal+m_Surface[i+1].Normal)/2;
        }
        else if (i==m_NSurfaces-1){
            m_Surface[i].TwistAxisA = (m_Surface[i-1].TwistAxis+m_Surface[i].TwistAxis)/2;
            m_Surface[i].TwistAxisB = m_Surface[i].TwistAxis;

            m_Surface[i].NormalA = (m_Surface[i-1].Normal+m_Surface[i].Normal)/2;
            m_Surface[i].NormalB = m_Surface[i].Normal;
        }
        else{
            m_Surface[i].TwistAxisA = (m_Surface[i-1].TwistAxis+m_Surface[i].TwistAxis)/2;
            m_Surface[i].TwistAxisB = (m_Surface[i].TwistAxis+m_Surface[i+1].TwistAxis)/2;

            m_Surface[i].NormalA = (m_Surface[i-1].Normal+m_Surface[i].Normal)/2;
            m_Surface[i].NormalB = (m_Surface[i].Normal+m_Surface[i+1].Normal)/2;
        }

        m_Surface[i].TwistAxisA.Normalize();
        m_Surface[i].TwistAxisB.Normalize();

        m_Surface[i].NormalA.Normalize();
        m_Surface[i].NormalB.Normalize();


    }

    for (int i=0;i<m_NSurfaces;i++){

        //pitch
        if (!isVawt){
        m_Surface[i].m_LA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, m_TTwist[i]);
        m_Surface[i].m_TA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, m_TTwist[i]);
        m_Surface[i].m_LB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, m_TTwist[i+1]);
        m_Surface[i].m_TB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, m_TTwist[i+1]);
        m_Surface[i].NormalA.Rotate(O,m_Surface[i].TwistAxisA, m_TTwist[i]);
        m_Surface[i].NormalB.Rotate(O,m_Surface[i].TwistAxisB, m_TTwist[i+1]);
        }
        else{
        m_Surface[i].m_LA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, m_TTwist[i]-90);
        m_Surface[i].m_TA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, m_TTwist[i]-90);
        m_Surface[i].m_LB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, m_TTwist[i+1]-90);
        m_Surface[i].m_TB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, m_TTwist[i+1]-90);
        m_Surface[i].NormalA.Rotate(O,m_Surface[i].TwistAxisA, m_TTwist[i]-90);
        m_Surface[i].NormalB.Rotate(O,m_Surface[i].TwistAxisB, m_TTwist[i+1]-90);
        }



        m_Surface[i].m_LA.RotateY(m_TCircAngle[i]);
        m_Surface[i].m_TA.RotateY(m_TCircAngle[i]);
        m_Surface[i].m_LB.RotateY(m_TCircAngle[i+1]);
        m_Surface[i].m_TB.RotateY(m_TCircAngle[i+1]);

        m_Surface[i].SetNormal();

        m_Surface[i].m_XDistType = 1;
        m_Surface[i].m_YDistType =  0;
        m_Surface[i].CreateXPoints();
        m_Surface[i].Init();

    }


    for (int i=0;i<m_NSurfaces;i++){
        m_Surface[i].SetSidePoints();
    }

}

void CBlade::Duplicate(CBlade *pWing)
{
    // Copies the wing data from an existing wing
	m_Polar = pWing->m_Polar;  // NM QList is said to make a deep copy as soon as one list is changed (copy-on-write)
	m_Airfoils = pWing->m_Airfoils;
    m_Range = pWing->m_Range;
    m_MultiPolars = pWing->m_MultiPolars;
    m_PolarAssociatedFoils = pWing->m_PolarAssociatedFoils;
    m_MinMaxReynolds = pWing->m_MinMaxReynolds;

    m_bisSinglePolar = pWing->m_bisSinglePolar;


    m_blades        = pWing->m_blades;
    m_NPanel        = pWing->m_NPanel;
    m_PlanformSpan  = pWing->m_PlanformSpan;
    m_objectName    = pWing->getName();    

//    m_Airfoils.clear();
//    for (int i = 0; i < pWing->m_Airfoils.size(); ++i)
//    {
//    m_Airfoils.append(pWing->m_Airfoils[i]);
//    }

    for (int i=0;i<pWing->m_StrutList.size();i++){
        Strut *str = new Strut;
        str->copy(pWing->m_StrutList.at(i));
        str->setSingleParent(this);
        m_StrutList.append(str);
        g_StrutStore.add(str);
    }

    for (int i = 0; i <= MAXSPANSECTIONS; ++i)
    {
        m_TChord[i]     = pWing->m_TChord[i];
        m_TPos[i]       = pWing->m_TPos[i];
        m_TOffsetX[i]    = pWing->m_TOffsetX[i];
        m_TLength[i]    = pWing->m_TLength[i];
        m_NXPanels[i]   = pWing->m_NXPanels[i] ;
        m_NYPanels[i]   = pWing->m_NYPanels[i];
        m_TOffsetX[i]    = pWing->m_TOffsetX[i];
        m_TTwist[i]     = pWing->m_TTwist[i];
        m_TDihedral[i]  = pWing->m_TDihedral[i];
        m_TCircAngle[i] = pWing->m_TCircAngle[i];
        m_TPAxisX[i]    = pWing->m_TPAxisX[i];
        m_TPAxisY[i]    = pWing->m_TPAxisY[i];
        m_TOffsetZ[i]    = pWing->m_TOffsetZ[i];
        m_TFoilPAxisX[i]= pWing->m_TFoilPAxisX[i];
        m_TFoilPAxisZ[i]= pWing->m_TFoilPAxisZ[i];
        m_TRelPos[i]    = pWing->m_TRelPos[i];
    }

    m_HubRadius = pWing->m_HubRadius;
    m_WingColor = pWing->m_WingColor;
}

CFoil* CBlade::GetFoilFromStation(int k)
{
	CFoil  *blade;
	if (k < m_Airfoils.length())
	{
		blade = g_mainFrame->GetFoil(m_Airfoils.at(k));
		return blade;
	}
	else
		return NULL;

}


void CBlade::ScaleChord(double NewChord)
{
	// Scales the wing chord-wise so that the root chord is set to the NewChord value
	double ratio = NewChord/m_TChord[0];
	for (int i=0; i<=MAXSPANSECTIONS; i++)
	{
		m_TChord[i]    *= ratio;
        m_TOffsetX[i]   *= ratio;
	}
	ComputeGeometry();
}

void CBlade::ScaleSpan(double NewSpan)
{
	// Scales the wing span-wise to the NewSpan value
	for (int i=0; i<=MAXSPANSECTIONS; i++)
	{
		m_TPos[i]      *= NewSpan/m_PlanformSpan;
		m_TRelPos[i]   *= NewSpan/m_PlanformSpan;
		m_TLength[i]   *= NewSpan/m_PlanformSpan;
	}
	ComputeGeometry();
}

void CBlade::ScaleTwist(double Twist)
{
	// Scales the twist to the new value
	double ratio = Twist / m_TTwist[0];
	for (int i = 0; i <= m_NPanel; i++)
	{
		m_TTwist[i] *= ratio;
	}
	ComputeGeometry();
}
