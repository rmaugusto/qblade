/****************************************************************************

    DMSData Class
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

#include "DMSData.h"
#include "../XBEM/Blade.h"
#include <QString>
#include <QList>
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"


DMSData::DMSData()
	: StorableObject ("name")
{

    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;


    elements    =   100;
    epsilon     =   0.001;
    iterations  =   1000;
    relax       =   0.4;
    rho         =   1.2;
    visc        =   0.0000178;

    m_bPowerLaw = false;
    m_bConstant = false;
    m_bLogarithmic = false;
    exponent = 0.4;
    roughness = 1;
    m_bTipLoss = false;
    m_bVariable = false;
    m_bAspectRatio = false;
}

DMSData::~DMSData()
{
	for (int i = 0; i < m_DData.size(); ++i) {
		delete m_DData[i];
	}
}


void DMSData::Clear()
{
    for (int i=0;i<m_DData.size();i++)
    {
        delete m_DData.at(i);
    }
    m_DData.clear();
    m_Cp.clear();
    m_Cp1.clear();
    m_Cp2.clear();
    m_Lambda.clear();
    m_one_over_Lambda.clear();
    m_Cm.clear();
    m_Cm1.clear();
    m_Cm2.clear();
    m_Kp.clear();

}


void DMSData::Compute(DData *pDData, CBlade *pWing, double lambda, double pitch)
{

    pDData->elements = elements;
    pDData->epsilon = epsilon;
    pDData->iterations = iterations;
    pDData->m_bTipLoss = m_bTipLoss;
    pDData->m_bAspectRatio = m_bAspectRatio;
    pDData->m_bVariable = m_bVariable;

    /*
    pDData->m_bRootLoss = m_bRootLoss;
    pDData->m_b3DCorrection = m_b3DCorrection;
    pDData->m_bInterpolation = m_bInterpolation;
    pDData->m_bNewRootLoss = m_bNewRootLoss;
    pDData->m_bNewTipLoss = m_bNewTipLoss;
    */
    pDData->relax = relax;
    pDData->rho = rho;
    pDData->visc = visc;
    pDData->Toff = 0;
    pDData->windspeed = 0;

    pDData->bPowerLaw = m_bPowerLaw;
    pDData->bConstant = m_bConstant;
    pDData->bLogarithmic = m_bLogarithmic;
    pDData->exponent = exponent;
    pDData->roughness = roughness;

	pDData->Init(pWing,lambda,pitch);
	pDData->OnDMS();

    if (!pDData->m_bBackflow)
    {

        m_DData.append(pDData);
        m_Cp.append(pDData->cp);
        m_Cp1.append(pDData->cp1);
        m_Cp2.append(pDData->cp2);
        m_Lambda.append(pDData->lambda_global);
        m_one_over_Lambda.append(1/pDData->lambda_global);
        m_Cm.append(pDData->cm);
        m_Cm1.append(pDData->cm1);
        m_Cm2.append(pDData->cm2);
        m_w.append(pDData->w);
        m_Kp.append(pDData->cp / pow(pDData->lambda_global, 3));


    }

}


void DMSData::Serialize(QDataStream &ar, bool bIsStoring, int /*ArchiveFormat*/)
{
    int i,n,j;
    float f;

    if (bIsStoring)
    {
        n= (int) m_Cp.size();

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

        if (m_bTipLoss) ar << 1; else ar<<0;
        if (m_bAspectRatio) ar << 1; else ar<<0;
        if (m_bVariable) ar << 1; else ar<<0;
        if (m_bConstant) ar << 1; else ar<<0;
        if (m_bPowerLaw) ar << 1; else ar<<0;
        if (m_bLogarithmic) ar << 1; else ar<<0;

        WriteCOLORREF(ar,m_Color);
        WriteCString(ar, m_WingName);
        WriteCString(ar, m_DMSName);

        ar << (int) n;
        for (i=0;i<n;i++)
        {
            ar << (float) m_Cp[i] << (float) m_Cp1[i] << (float) m_Cp2[i];
            ar << (float) m_Cm[i] << (float) m_Cm1[i] << (float) m_Cm2[i];
            ar << (float) m_Lambda[i] << (float) m_one_over_Lambda[i] << (float) m_Kp[i];
        }
        for (i=0;i<n;i++)
        {
            m_DData.at(i)->Serialize(ar,bIsStoring);
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
        if (f) m_bTipLoss = true; else m_bTipLoss = false;
        ar >> f;
        if (f) m_bAspectRatio = true; else m_bAspectRatio = false;
        ar >> f;
        if (f) m_bVariable = true; else m_bVariable = false;
        ar >> f;
        if (f) m_bConstant = true; else m_bConstant = false;
        ar >> f;
        if (f) m_bPowerLaw = true; else m_bPowerLaw = false;
        ar >> f;
        if (f) m_bLogarithmic = true; else m_bLogarithmic = false;

        ReadCOLORREF(ar,m_Color);
        ReadCString(ar,m_WingName);
//		setParentName(m_WingName);  // NM REPLACE
		setSingleParent(g_verticalRotorStore.getObjectByNameOnly(m_WingName));  // only needed for downwards compatibility
        ReadCString(ar,m_DMSName);
		setName(m_DMSName);

        ar >> n;

        for (i=0;i<n;i++)
        {
            ar >> f;
            m_Cp.append(f);
            ar >> f;
            m_Cp1.append(f);
            ar >> f;
            m_Cp2.append(f);
            ar >> f;
            m_Cm.append(f);
            ar >> f;
            m_Cm1.append(f);
            ar >> f;
            m_Cm2.append(f);
            ar >> f;
            m_Lambda.append(f);
            ar >> f;
            m_one_over_Lambda.append(f);
            ar >> f;
            m_Kp.append(f);
        }

        for (i=0;i<n;i++)
        {
            DData *pDData = new DData;
            pDData->Serialize(ar,bIsStoring);
            m_DData.append(pDData);
        }

	}
}

void DMSData::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_DMSName);
	
	g_serializer.readOrWriteDoubleList1D (&m_Cp);
	g_serializer.readOrWriteDoubleList1D (&m_Cp1);
	g_serializer.readOrWriteDoubleList1D (&m_Cp2);
	g_serializer.readOrWriteDoubleList1D (&m_Cm);
	g_serializer.readOrWriteDoubleList1D (&m_Cm1);
	g_serializer.readOrWriteDoubleList1D (&m_Cm2);
	g_serializer.readOrWriteDoubleList1D (&m_w);
	g_serializer.readOrWriteDoubleList1D (&m_Lambda);
	g_serializer.readOrWriteDoubleList1D (&m_one_over_Lambda);

	// serialize the DData array m_DData
	if (g_serializer.isReadMode()) {
		int n = g_serializer.readInt();
		for (int i = 0; i < n; ++i) {
			m_DData.append(DData::newBySerialize());
		}
	} else {
		g_serializer.writeInt(m_DData.size());
		for (int i = 0; i < m_DData.size(); ++i) {
			m_DData[i]->serialize();
		}
	}	
	
	g_serializer.readOrWriteDoubleList1D (&m_Kp);
	
	g_serializer.readOrWriteBool (&m_bShowPoints);
	g_serializer.readOrWriteBool (&m_bIsVisible);
	g_serializer.readOrWriteInt (&m_Style);
	g_serializer.readOrWriteInt (&m_Width);
	g_serializer.readOrWriteColor (&m_Color);
	
	g_serializer.readOrWriteDouble (&rho);
	g_serializer.readOrWriteDouble (&elements);
	g_serializer.readOrWriteDouble (&epsilon);
	g_serializer.readOrWriteDouble (&iterations);
	g_serializer.readOrWriteDouble (&relax);
	g_serializer.readOrWriteDouble (&visc);
	g_serializer.readOrWriteBool (&m_bPowerLaw);
	g_serializer.readOrWriteBool (&m_bConstant);
	g_serializer.readOrWriteBool (&m_bLogarithmic);
	g_serializer.readOrWriteDouble (&exponent);
	g_serializer.readOrWriteDouble (&roughness);
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bVariable);
	g_serializer.readOrWriteBool (&m_bAspectRatio);
}

void DMSData::restorePointers() {
	StorableObject::restorePointers();
	
	DData *dData;
	for (int i = 0; i < m_DData.size(); ++i) {
		dData = m_DData[i];
		for (int j = 0; j < dData->m_PolarPointers.size(); ++j) {
			g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&dData->m_PolarPointers[j]));	
		}
		for (int j = 0; j < dData->m_PolarPointersTO.size(); ++j) {
			g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&dData->m_PolarPointersTO[j]));	
		}
	}
}


DMSData* DMSData::newBySerialize() {
	DMSData* dmsData = new DMSData ();
	dmsData->serialize();
	return dmsData;
}
