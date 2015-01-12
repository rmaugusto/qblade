/****************************************************************************

    TDMSData Class
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

#include "TDMSData.h"
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"


TDMSData::TDMSData()
	: DMSData()
{

    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;

    elements = 10;
    epsilon = 0.001;
    iterations = 1000;
    relax = 0.3;
    m_bTipLoss = false;
    m_bVariable = false;
    m_bAspectRatio = false;

    visc = 0.0000178;
    rho = 1.2;

    m_bPowerLaw = false;
    m_bConstant = false;
    m_bLogarithmic = false;
    exponent = 0.4;
    roughness = 1;

}

TDMSData::~TDMSData()
{

}

void TDMSData::Clear()
{
    m_Omega.clear();
    m_V.clear();
    for (int i=0;i<m_DData.size();i++)
    {
        delete m_DData.at(i);
    }
    m_DData.clear();
    m_P.clear();
    m_Thrust.clear();
    m_T.clear();
    m_P_loss.clear();
    m_Cp_loss.clear();
    m_Cp.clear();
    m_Cp1.clear();
    m_Cp2.clear();
    m_Cm.clear();
    m_Cm1.clear();
    m_Cm2.clear();
    m_Lambda.clear();
    m_one_over_Lambda.clear();
    m_Kp.clear();
	m_Weibull.clear();
	m_aepk.clear();
	m_aepA.clear();

}

void TDMSData::Compute(DData *pDData, CBlade *pWing, double lambda, double pitch, double Toff, double windspeed)
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
    pDData->windspeed = windspeed;
    pDData->Toff = Toff;

    pDData->bPowerLaw = m_bPowerLaw;
    pDData->bConstant = m_bConstant;
    pDData->bLogarithmic = m_bLogarithmic;
    pDData->exponent = exponent;
    pDData->roughness = roughness;

	pDData->Init(pWing,lambda,pitch);
	pDData->OnDMS();

}

void TDMSData::Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
    int i,n,j;
    float f;

    if (bIsStoring)
    {
        n= (int) m_P.size();

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
        WriteCString(ar, m_TurbineName);
        WriteCString(ar, m_SimName);

        ar << (int) n;
        for (i=0;i<n;i++)
        {
            ar << (float) m_P[i] << (float) m_P_loss[i] << (float) m_Thrust[i];
            ar << (float) m_T[i] << (float) m_V[i] << (float) m_Omega[i];
            ar << (float) m_Lambda[i] << (float) m_one_over_Lambda[i];
            ar << (float) m_Cp[i] << (float) m_Cp1[i] << (float) m_Cp2[i];
            ar << (float) m_Cm[i] << (float) m_Cm1[i] << (float) m_Cm2[i];
			ar << (float) m_Weibull[i];
            ar << (float) m_Cp_loss[i] << (float) m_Kp[i];
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
		if (ArchiveFormat<100024)
		{
			ar >> f;
			//k = f;
			ar >> f;
			//A = f;
		}
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
        ReadCString(ar,m_TurbineName);
//		setParentName(m_TurbineName);  // NM REPLACE
		setSingleParent(g_verttdataStore.getObjectByNameOnly(m_TurbineName));  // only needed for downwards compatibility
        ReadCString(ar,m_SimName);
		setName(m_SimName);

        ar >> n;

        for (i=0;i<n;i++)
        {
            ar >> f;
            m_P.append(f);
            ar >> f;
            m_P_loss.append(f);
            ar >> f;
            m_Thrust.append(f);
            ar >> f;
            m_T.append(f);
            ar >> f;
            m_V.append(f);
            ar >> f;
            m_Omega.append(f);
            ar >> f;
            m_Lambda.append(f);
            ar >> f;
            m_one_over_Lambda.append(f);
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
            m_Weibull.append(f);
			if (ArchiveFormat<100024)
			{
				ar >> f;
				//m_WeibullV3.append(f);
			}
            ar >> f;
            m_Cp_loss.append(f);
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

void TDMSData::SerializeDummy(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
    int i,n,j;
    float f;

    if (bIsStoring)
    {
		// won't happen
    }
    else
    {
        ar >> f;
        ar >> f;
        ar >> j;
        ar >> j;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
		if (ArchiveFormat<100024)
		{
			ar >> f;
			ar >> f;
		}
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;
        ar >> f;

        ReadCOLORREF(ar,m_Color);
        ReadCString(ar,m_TurbineName);
        ReadCString(ar,m_SimName);

        ar >> n;
        for (i=0;i<n;i++)
        {
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
            ar >> f;
			if (ArchiveFormat<100024)
			{
				ar >> f;
			}
            ar >> f;
            ar >> f;
        }

        for (i=0;i<n;i++)
        {
            DData *pDData = new DData;
            pDData->Serialize(ar,bIsStoring);
			delete pDData;
        }
	}
}

TDMSData* TDMSData::newBySerialize() {
	TDMSData* tDmsData = new TDMSData ();
	tDmsData->serialize();
	return tDmsData;
}

void TDMSData::serialize() {
	DMSData::serialize();
	
	g_serializer.readOrWriteString (&m_TurbineName);
	g_serializer.readOrWriteString (&m_SimName);
	
	g_serializer.readOrWriteDoubleList1D (&m_P);
	g_serializer.readOrWriteDoubleList1D (&m_P_loss);
	g_serializer.readOrWriteDoubleList1D (&m_T);
	g_serializer.readOrWriteDoubleList1D (&m_Thrust);
	g_serializer.readOrWriteDoubleList1D (&m_V);
	g_serializer.readOrWriteDoubleList1D (&m_Omega);
	g_serializer.readOrWriteDoubleList1D (&m_Weibull);
	g_serializer.readOrWriteDoubleList1D (&m_aepk);
	g_serializer.readOrWriteDoubleList1D (&m_aepA);
	g_serializer.readOrWriteDoubleList1D (&m_Cp_loss);
}
