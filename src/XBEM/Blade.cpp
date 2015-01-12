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


CBlade* CBlade::newBySerialize() {
	CBlade* rotor = new CBlade ();
	rotor->serialize();
	return rotor;
}

CBlade::CBlade (QString name, StorableObject *parent)
	: StorableObject (name, parent)
{
	memset(m_TPos, 0, sizeof(m_TPos));
	memset(m_TCircAngle, 0, sizeof(m_TCircAngle));
	memset(m_TChord, 0, sizeof(m_TChord));
	memset(m_TLength, 0, sizeof(m_TLength));
	memset(m_TOffset, 0, sizeof(m_TOffset));
	memset(m_TDihedral, 0, sizeof(m_TDihedral));
	memset(m_TTwist, 0, sizeof(m_TTwist));
	memset(m_NXPanels, 0, sizeof(m_NXPanels));
	memset(m_NYPanels, 0, sizeof(m_NYPanels));
	
	m_bIsOrtho = false;
	
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
	m_TOffset[0] = 0.0;
	m_TOffset[1] = 0.0;
	m_TRelPos[0] = 0.0;
	m_TRelPos[1] = 1;
	m_TPAxisX[0] = 0;
	m_TPAxisX[1] = 0;
	m_TPAxisZ[0] = 0;
	m_TPAxisZ[1] = 0;
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

void CBlade::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteStringList (&m_Airfoils);
	g_serializer.readOrWriteStringList (&m_Polar);
	
	g_serializer.readOrWriteInt (&m_blades);
	g_serializer.readOrWriteInt (&m_NSurfaces);
	g_serializer.readOrWriteInt (&m_NPanel);
	g_serializer.readOrWriteBool (&m_bIsOrtho);
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
	g_serializer.readOrWriteDoubleArray1D (m_TOffset, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TDihedral, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TTwist, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPAxisX, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPAxisZ, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TPAxisY, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TFoilPAxisX, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TFoilPAxisZ, MAXSPANSECTIONS+1);
	g_serializer.readOrWriteDoubleArray1D (m_TRelPos, MAXSPANSECTIONS+1);	
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
	/* TODO Attention: this is ambiguous! 360Polar can have same names. Waiting for pointer solution in StorableObject */
//    return g_360PolarStore.getObjectByNameOnly(m_Polar[position]);
}

double CBlade::getFASTRadiusAt(int position) {
	return (m_TPos[position] + m_TPos[position+1]) / 2;
}

void CBlade::ComputeGeometry()
{
	m_TLength[0] = 0.0;
	for (int i = 1; i <= m_NPanel; ++i) {
		m_TLength[i] = m_TPos[i] - m_TPos[i-1];
	}
	m_PlanformSpan  = 2.0 * m_TPos[m_NPanel];
}

void CBlade::CreateSurfaces(bool isVawt)
{
	// Constructs the surface objects based on the input data
	// The surfaces are constructed from root to tip, and re-ordered from let tip to right tip
	// One surface object for each of the wing's panels
	// A is the surface's left side, B is the right side
	
	int j, nSurf;
    CVector PLA, PTA, PLB, PTB;
	CVector O(0.0,0.0,0.0);
	CVector VNormal[MAXSPANSECTIONS+1], PitchAxis[MAXSPANSECTIONS+1], VNSide[MAXSPANSECTIONS+1];
	double MinPanelSize;
	
	MinPanelSize = 0.0;
	
	m_NSurfaces = 0;
	
	//define the normal of each surface
	nSurf=0;
	VNormal[0].Set(0.0, 0.0, 1.0);
	VNSide[0].Set(0.0, 0.0, 1.0);
	VNSide[0].RotateX(O,m_TDihedral[0]);
	
	for(j=0; j<m_NPanel;j++)
	{
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			VNormal[nSurf].Set(0.0, 0.0, 1.0);
			VNormal[nSurf].RotateX(O, m_TDihedral[j]);
			
			nSurf++;
		}
	}
	
	///construction of the pitch axis
	for(j=0; j<m_NPanel;j++)
	{
		PLA.x = m_TOffset[j];
		PLB.x = m_TOffset[j+1];
		PLA.y = m_TPos[j];
		PLB.y = m_TPos[j+1];
        PLA.z = m_TPAxisZ[j];
        PLB.z = m_TPAxisZ[j+1]; /// changed from PLA.z = 0 to allow visualization of deformations
		
//		PLB.RotateX(PLA,m_TDihedral[j]); /// commented because dihedral not in use anymore

//        if (j>0)
//        {
//            T1 = CVector(m_TPAxisX[j]-PLA.x,m_TPAxisY[j]-PLA.y,m_TPAxisZ[j]-PLA.z);
//            PLB.Translate(T1);
//            PLA.Translate(T1); //commented for visualizations of deflections
//        }
		
//		PLA.RotateY(O,m_TCircAngle[j]);
//		if (j==m_NPanel-1) PLB.RotateY(O,m_TCircAngle[j+1]);
		
		m_TPAxisX[j]= PLA.x;
		m_TPAxisY[j]= PLA.y;
        m_TPAxisZ[j]= PLA.z;
		
		m_TPAxisX[j+1]= PLB.x;
		m_TPAxisY[j+1]= PLB.y;
        m_TPAxisZ[j+1]= PLB.z;
	}
	
	m_NSurfaces = nSurf;
	
	//define the normals at panel junctions : average between the normals of the two connecting sufaces
	for(j=0; j<nSurf;j++)
	{
		VNSide[j+1] = VNormal[j]+VNormal[j+1];
		VNSide[j+1].Normalize();
	}
	
	for (int m=0;m<=m_NPanel;m++)
	{
        if (isVawt)
			VNSide[m].RotateY(O, m_TCircAngle[m]);
	}
	
	for(j=0; j<=nSurf;j++)
	{
		if (j==0)
			PitchAxis[j] = CVector(m_TPAxisX[j+1]-m_TPAxisX[j],m_TPAxisY[j+1]-m_TPAxisY[j],m_TPAxisZ[j+1]-m_TPAxisZ[j]);
		else if (j==nSurf)
			PitchAxis[j] = CVector(m_TPAxisX[j]-m_TPAxisX[j-1],m_TPAxisY[j]-m_TPAxisY[j-1],m_TPAxisZ[j]-m_TPAxisZ[j-1]);
		else
			PitchAxis[j] = CVector(m_TPAxisX[j+1]-m_TPAxisX[j-1],m_TPAxisY[j+1]-m_TPAxisY[j-1],m_TPAxisZ[j+1]-m_TPAxisZ[j-1]);
		
		PitchAxis[j].Normalize();
	}
	
	int is = 0;
	for (j = 0; j < m_NPanel; ++j)
	{
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			m_Surface[is].m_pFoilA = g_mainFrame->GetFoil(m_Airfoils[j]);
			m_Surface[is].m_pFoilB = g_mainFrame->GetFoil(m_Airfoils[j+1]);
			
			m_Surface[is].m_Length =  m_TPos[j+1] - m_TPos[j];
			

			PLA.x = m_TOffset[j]-m_TChord[j]*m_TFoilPAxisX[j];
			PLB.x = m_TOffset[j+1]-m_TChord[j+1]*m_TFoilPAxisX[j+1];
			PLA.y = m_TPos[j];
			PLB.y = m_TPos[j+1];
            PLA.z = m_TPAxisZ[j]-m_TChord[j]*m_TFoilPAxisZ[j];
            PLB.z = m_TPAxisZ[j+1]-m_TChord[j+1]*m_TFoilPAxisZ[j+1];
			PTA.x = PLA.x+m_TChord[j];
			PTB.x = PLB.x+m_TChord[j+1];
			PTA.y = PLA.y;
			PTB.y = PLB.y;
            PTA.z = PLA.z;
            PTB.z = PLB.z;
			
			m_Surface[is].m_LA.Copy(PLA);
			m_Surface[is].m_TA.Copy(PTA);
			m_Surface[is].m_LB.Copy(PLB);
			m_Surface[is].m_TB.Copy(PTB);
			
			m_Surface[is].SetNormal(); // is (0,0,1)
			
			m_Surface[is].m_XPitchA  =  m_TFoilPAxisX[j];
			m_Surface[is].m_XPitchB  =  m_TFoilPAxisX[j+1];
			m_Surface[is].m_ZPitchA  =  m_TFoilPAxisZ[j];
			m_Surface[is].m_ZPitchB  =  m_TFoilPAxisZ[j+1];
			
			m_Surface[is].m_TwistA   =  -m_TTwist[j];
			m_Surface[is].m_TwistB   =  -m_TTwist[j+1];
			m_Surface[is].SetTwist3(m_TFoilPAxisX[j],m_TFoilPAxisZ[j]);
			
			////newnew code DM
			if(m_bIsOrtho)
			{
				m_Surface[is].m_VecA = CVector(PitchAxis[j]);
				m_Surface[is].m_VecB = CVector(PitchAxis[j+1]);
			}
			else
			{
				m_Surface[is].m_VecA = CVector(0,PitchAxis[j].y,PitchAxis[j].z);
				m_Surface[is].m_VecB = CVector(0,PitchAxis[j+1].y,PitchAxis[j+1].z);
			}
			
            if (!isVawt) if (m_bIsOrtho) m_Surface[is].RotateOrtho();
			
			m_Surface[is].RotateX(CVector(m_TOffset[j], m_TPos[j],0), m_TDihedral[j]);
			m_Surface[is].NormalA.Set(VNSide[is].x,   VNSide[is].y,   VNSide[is].z);
			m_Surface[is].NormalB.Set(VNSide[is+1].x, VNSide[is+1].y, VNSide[is+1].z);
			
            if (!isVawt)
			{		
				if (j>0)
				{
					m_Surface[is].m_LA= m_Surface[is-1].m_LB;
					m_Surface[is].m_TA= m_Surface[is-1].m_TB;
				}
			}
			
            if (!isVawt)
			{
				m_Surface[is].NormalA.Rotate(m_Surface[is].m_VecA, m_Surface[is].m_TwistA);
				m_Surface[is].NormalB.Rotate(m_Surface[is].m_VecB, m_Surface[is].m_TwistB);
			}
			else
			{
				m_Surface[is].NormalA.RotateY(O, m_Surface[is].m_TwistA);
				m_Surface[is].NormalB.RotateY(O, m_Surface[is].m_TwistB);
			}
            m_Surface[is].m_LA.RotateY(m_TCircAngle[j]);
            m_Surface[is].m_TA.RotateY(m_TCircAngle[j]);
            m_Surface[is].m_LB.RotateY(m_TCircAngle[j+1]);
            m_Surface[is].m_TB.RotateY(m_TCircAngle[j+1]);
			m_Surface[is].m_NXPanels = m_NXPanels[j];
			m_Surface[is].m_NYPanels = m_NYPanels[j];
			
			//AVL coding + invert sine and -sine for left wing
			m_Surface[is].m_XDistType = 1;
			m_Surface[is].m_YDistType =  0;
			m_Surface[is].CreateXPoints();
			m_Surface[is].SetFlap();
			m_Surface[is].Init();
			m_Surface[is].m_bIsLeftSurf   = false;
			m_Surface[is].m_bIsRightSurf  = true;
			m_Surface[is].m_bIsInSymPlane = false;
			is++;
		}

	}
	m_Surface[m_NSurfaces-1].m_bIsCenterSurf = true;  // previous left center surface
	m_Surface[m_NSurfaces].m_bIsCenterSurf = true;  // next right center surface
	
	m_Surface[0].m_bIsTipLeft = true;
	if (m_NSurfaces >= 1) {
		m_Surface[m_NSurfaces-1].m_bIsTipRight = true;
	}
}

void CBlade::Duplicate(CBlade *pWing)
{
    // Copies the wing data from an existing wing
	m_Polar = pWing->m_Polar;  // NM QList is said to make a deep copy as soon as one list is changed (copy-on-write)
	m_Airfoils = pWing->m_Airfoils;

	m_blades = pWing->m_blades;
    m_NPanel        = pWing->m_NPanel;
    m_PlanformSpan  = pWing->m_PlanformSpan;
    m_objectName    = pWing->getName();

    m_Airfoils.clear();
    for (int i = 0; i < pWing->m_Airfoils.size(); ++i)
    {
    m_Airfoils.append(pWing->m_Airfoils[i]);
    }


    for (int i = 0; i <= MAXSPANSECTIONS; ++i)
    {
        m_TChord[i]     = pWing->m_TChord[i];
        m_TPos[i]       = pWing->m_TPos[i];
        m_TOffset[i]    = pWing->m_TOffset[i];
        m_TLength[i]    = pWing->m_TLength[i];
        m_NXPanels[i]   = pWing->m_NXPanels[i] ;
        m_NYPanels[i]   = pWing->m_NYPanels[i];
//		m_Airfoils[i]      = pWing->m_Airfoils[i];  // NM DELETE
        m_TTwist[i]     = pWing->m_TTwist[i];
        m_TDihedral[i]  = pWing->m_TDihedral[i];
        m_TCircAngle[i]     = pWing->m_TCircAngle[i];
        m_TPAxisX[i]    = pWing->m_TPAxisX[i];
        m_TPAxisY[i]    = pWing->m_TPAxisY[i];
        m_TPAxisZ[i]    = pWing->m_TPAxisZ[i];
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
		m_TOffset[i]   *= ratio;
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


bool CBlade::SerializeWing(QDataStream &ar, bool bIsStoring, int ProjectFormat)
{
    //saves or loads the wing data to the archive ar
    int i;
    int ArchiveFormat;  // identifies the format of the file
	QString strong;

    if(bIsStoring)
    {	// storing code
            if(ProjectFormat>9)             ar << 1014;// JW modification
            else if(ProjectFormat>7)        ar << 1013;// JW modification
            else if(ProjectFormat==6)       ar << 1010;
            else if(ProjectFormat==5)       ar << 1009;
            //1014 : (new code DM) added storage of m_TYRel for Circumferential Angles
            //1013 : (new code JW) added storage of swept area and max radius
            //1012 : (new code DM) added storage of Othogonal sections
            //1011 : (new code DM) added storage of Hub radius
            //1010 : added storage of alpha channel + added a provision for ints and floats
            //1009 : QFLR5 v0.03 : added mass properties for inertia calculations
            //1008 : QFLR5 v0.02 : Added wing description field
            //1007 : Changed length units to m
            //1006 : Added Wing Color v2.99-15
            //1005 : Added Chordwise spacing (v2.99-00)
            //1004 : corrected m_NXPanels, m_NYPanels, m_YPanelDist to int (v1.99-33)
            //1003 : AVL Format (v1.99-18)
            //1002 : save VLM Mesh (v1.99-12)
            //1001 : initial format
        WriteCString(ar, m_objectName);
		if(ProjectFormat>=5) WriteCString(ar, "");

        ar << 0; //non elliptic...

        ar << 1;

        ar << m_NPanel;

		for (i=0; i<=m_NPanel; i++) WriteCString(ar, m_Airfoils[i]);
		//// duplicated to keep compatibility with old format with LFoil and RFoil (RFoil was removed)
		for (i=0; i<=m_NPanel; i++) WriteCString(ar, m_Airfoils[i]);
        for (i=0; i<=m_NPanel; i++) ar << (float)m_TChord[i];
        for (i=0; i<=m_NPanel; i++) ar << (float)m_TPos[i];
        for (i=0; i<=m_NPanel; i++) ar << (float)m_TOffset[i];
        for (i=0; i<=m_NPanel; i++) ar << (float)m_TDihedral[i];
        for (i=0; i<=m_NPanel; i++) ar << (float)m_TTwist[i];

        ar << 0.0f;

        ar<<0;
        for (i=0; i<=m_NPanel; i++) ar << m_NXPanels[i];
        for (i=0; i<=m_NPanel; i++) ar << m_NYPanels[i];
        for (i=0; i<=m_NPanel; i++) ar << 0;//1005
        for (i=0; i<=m_NPanel; i++) ar << 0;

        WriteCOLORREF(ar,m_WingColor);

        if(ProjectFormat>=5)
        {
            ar << (float) 0.0;
            ar << 0;
        }
        if(ProjectFormat>5)
        {
            ar << m_WingColor.alpha();
            for(int i=0; i<20; i++) ar<<(float)0.0f;
            for(int i=0; i<20; i++) ar<<0;
        }

                //new code DM///////
                ar << (float) m_Polar.size();
                QString polar;
                for (i=0;i<m_Polar.size();i++)
                {
                polar = m_Polar.at(i);
                WriteCString(ar, polar);
                }
                ar << (float) m_blades;

                ar << (float) m_HubRadius;

                for (i=0; i<=m_NPanel; i++)
                {
                    ar << (float)m_TRelPos[i];
                }

                for (i=0; i<=m_NPanel; i++)
                {
                    ar << (float)m_TFoilPAxisX[i];
                    ar << (float)m_TFoilPAxisZ[i];
                }

                ar << (float) m_bIsOrtho;

                //end new code DM/////////

                //new code JW///////
                ar << (float) m_sweptArea;
                ar << (float) m_MaxRadius;
                //end new code JW///////

                for (i=0; i<=m_NPanel;i++)
                {
                    ar << (float) m_TCircAngle[i];
                }

        return true;
    }
    else
    {
        // loading code
        float f;
        int k, dummyInt;
		QString dummyString;
        ar >> ArchiveFormat;

        if (ArchiveFormat <1001 || ArchiveFormat>1100) {
            m_objectName = "";
            return false;
        }

        ReadCString(ar,m_objectName);
        if (m_objectName.length() == 0) return false;

        if (ArchiveFormat >=1008)
        {
            ReadCString(ar, dummyString);
        }

        ar >> k;
        if(k!=0){
            m_objectName = "";
            return false;
        }

        ar >> k;

        ar >> m_NPanel;
        if(m_NPanel <=0 || m_NPanel>=100)
        {
            m_objectName = "";
            return false;
        }
		for (i=0; i<=m_NPanel; i++) ReadCString(ar, strong);
		for (i = 0; i <= m_NPanel; i++) {
			ReadCString(ar, strong);
			m_Airfoils.append(strong);
		}
		
        for (i=0; i<=m_NPanel; i++)
        {
            ar >> f; m_TChord[i]=f;
            if (fabs(m_TChord[i]) <0.0)
            {
                m_objectName = "";
                return false;
            }
        }

        for (i=0; i<=m_NPanel; i++)
        {
            ar >> f; m_TPos[i]=f;
            if (fabs(m_TPos[i]) <0.0)
            {
                m_objectName = "";
                return false;
            }
        }

        for (i=0; i<=m_NPanel; i++)
        {
            ar >> f; m_TOffset[i]=f;
        }

        if(ArchiveFormat<1007)
        {
            //convert mm to m
            for (i=0; i<=m_NPanel; i++)
            {
                m_TPos[i]    /= 1000.0;
                m_TChord[i]  /= 1000.0;
                m_TOffset[i] /= 1000.0;
            }

        }
        for (i=0; i<=m_NPanel; i++)
        {
            ar >> f; m_TDihedral[i]=f;
        }
        for (i=0; i<=m_NPanel; i++)
        {
            ar >> f; m_TTwist[i]=f;
        }

        ar >> f;
        ar >> k;

        for (i=0; i<=m_NPanel; i++)
        {
            if(ArchiveFormat<=1003)
            {
                ar >> f;
                m_NXPanels[i] = (int)f;
            }
            else
                ar >> m_NXPanels[i];
        }

        for (i=0; i<=m_NPanel; i++)
        {
            if(ArchiveFormat<=1003)
            {
                ar >> f;
                m_NYPanels[i] = (int)f;
            }
            else 	ar >> m_NYPanels[i];
        }
        int total = 0;
        for (i=0; i<m_NPanel; i++)
        {
            total += m_NYPanels[i];
        }

        if(total*2>=MAXSPANSTATIONS)
        {
            double ratio = MAXSPANSTATIONS/total/2.0;
            for (i=0; i<=m_NPanel; i++)
            {
                m_NYPanels[i] = (int) (ratio*m_NYPanels[i]);
            }
        }

        int spanpos = 0;
        int vlmpanels = 0;
        for (int l=0; l<m_NPanel; l++)
        {
            spanpos  += m_NYPanels[l];
            vlmpanels +=m_NXPanels[l]*m_NYPanels[l];
        }


        if (ArchiveFormat >=1005)
        {
            for (i=0; i<=m_NPanel; i++) ar >> dummyInt;
        }

        for (i=0; i<=m_NPanel; i++)
        {
            ar >> dummyInt;
        }

        if(ArchiveFormat>=1006)
        {
            ReadCOLORREF(ar, m_WingColor);
        }

        if(ArchiveFormat>=1009)
        {
            ar >> f;
            ar >> dummyInt;
        }

        if(ArchiveFormat>=1010)
        {
            ar >> k; m_WingColor.setAlpha(k);
            for(int i=0; i<20; i++) ar>>f;
            for(int i=0; i<20; i++) ar>>k;
        }

        ComputeGeometry();

        //new code DM////////
        float l,m;
        QString polar;
        ar >> l;
        for (i=0;i<l;i++)
        {
            ReadCString(ar, polar);
            m_Polar.append(polar);
        }

        ar >> m;
        m_blades = (int) m;


        if (ArchiveFormat>=1011)
        {
            ar >> m;
            m_HubRadius = (double) m;
        }
        else
        {
            m_HubRadius = m_TPos[0];
        }

        if (ArchiveFormat>=1011)
        {
            for (i=0; i<=m_NPanel; i++)
            {
                ar >> f;
                m_TRelPos[i]=f;
            }
        }
        else
        {
        for (i=0; i<=m_NPanel; i++)
            {
               m_TRelPos[i]=m_TPos[i]-m_HubRadius;
            }
        }

        if (ArchiveFormat>=1012)
        {
            for (i=0; i<=m_NPanel; i++)
            {
                ar >> f;
                m_TFoilPAxisX[i]=f;
                ar >> f;
                m_TFoilPAxisZ[i]=f;
            }
        }
        else
        {
        for (i=0; i<=m_NPanel; i++)
            {
                m_TFoilPAxisX[i]=0.25;
                m_TFoilPAxisZ[i]=0;
            }
        }

        if (ArchiveFormat>=1012)
        {
            ar >> f;
            m_bIsOrtho = bool(f);
        }
        else m_bIsOrtho = false;
        //////////end new code DM///////


        /////new code JW////////
        if (ArchiveFormat>=1013)
        {
            float n;
            ar >> n;
            m_sweptArea = (float) n;
            ar >> n;
            m_MaxRadius = (float) n;
        }
        /////end new code JW////////

        if (ArchiveFormat>=1014)
        {
            for (i=0; i<=m_NPanel; i++)
            {
                ar >> f;
                m_TCircAngle[i]=f;
            }
        }
        return true;
    }
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
