/****************************************************************************

    BEMData Class
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

#include "BEMData.h"
#include "../Objects/Wing.h"
#include <QString>
#include <QList>
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"
#include <QDebug>


BEMData::BEMData()
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

    m_bNewTipLoss = false;
    m_bNewRootLoss = false;
    visc = 0.0000178;

    m_bTipLoss = false;
    m_bRootLoss = false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
}

BEMData::~BEMData() {
	for (int i = 0; i < m_BData.size(); ++i) {
		delete m_BData[i];
	}
}

QVector <BData *> BEMData::GetBData(){
    return m_BData;
}


void BEMData::Clear()
{
    for (int i=0;i<m_BData.size();i++)
    {
        if (m_BData.at(i)) delete m_BData.at(i);
    }
    m_BData.clear();
    m_Cp.clear();
    m_Lambda.clear();
    m_one_over_Lambda.clear();
    m_Ct.clear();
    m_Kp.clear();
    m_Cm.clear();
}

void BEMData::Compute(BData *pBData, CBlade *pWing, double lambda, double pitch)
{

    pBData->elements = elements;
    pBData->epsilon = epsilon;
    pBData->iterations = iterations;
    pBData->m_bTipLoss = m_bTipLoss;
    pBData->m_bRootLoss = m_bRootLoss;
    pBData->m_b3DCorrection = m_b3DCorrection;
    pBData->m_bInterpolation = m_bInterpolation;
    pBData->relax = relax;
    pBData->rho = rho;
    pBData->visc = visc;
    pBData->m_bNewRootLoss = m_bNewRootLoss;
    pBData->m_bNewTipLoss = m_bNewTipLoss;
    pBData->m_bCdReynolds = false;


    pBData->Init(pWing,lambda);

    pBData->OnBEM(pitch);

    m_BData.append(pBData);
    m_Cp.append(pBData->cp);
    m_Cm.append(pBData->cp/pBData->lambda_global);
    m_Lambda.append(pBData->lambda_global);
    m_one_over_Lambda.append(1/pBData->lambda_global);
    m_Ct.append(pBData->ct);
    m_Kp.append(pBData->cp / pBData->lambda_global / pBData->lambda_global / pBData->lambda_global);
}

void BEMData::Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
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

        if (m_bTipLoss) ar << 1; else ar<<0;
        if (m_bRootLoss) ar << 1; else ar<<0;
        if (m_b3DCorrection) ar << 1; else ar<<0;
        if (m_bInterpolation) ar << 1; else ar<<0;
        if (m_bNewTipLoss) ar << 1; else ar<<0;
        if (m_bNewRootLoss) ar << 1; else ar<<0;
        if (m_bCdReynolds) ar << 1; else ar<<0;

        WriteCOLORREF(ar,m_Color);
        WriteCString(ar, m_WingName);
        WriteCString(ar, m_BEMName);
		ar << (int) n;
        for (i=0;i<n;i++)
        {
            ar << (float) m_Cp[i] << (float) m_Ct [i] << (float) m_Lambda[i] << (float) m_one_over_Lambda[i] << (float) m_Kp[i] << (float) m_Cm[i];
        }
        for (i=0;i<n;i++)
        {
            m_BData.at(i)->Serialize(ar,bIsStoring);
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
        if (f) m_bTipLoss = true; else m_bTipLoss = false;
        ar >> f;
        if (f) m_bRootLoss = true; else m_bRootLoss = false;
        ar >> f;
        if (f) m_b3DCorrection = true; else m_b3DCorrection = false;
        ar >> f;
        if (f) m_bInterpolation = true; else m_bInterpolation = false;
        ar >> f;
        if (f) m_bNewTipLoss = true; else m_bNewTipLoss = false;
        ar >> f;
        if (f) m_bNewRootLoss = true; else m_bNewRootLoss = false;
        if (ArchiveFormat >= 100021)
        {
            ar >> f;
            if (f) m_bCdReynolds = true; else m_bCdReynolds = false;
        }
        ReadCOLORREF(ar,m_Color);
        ReadCString(ar,m_WingName);
//		setParentName(m_WingName);  // NM REPLACE
		setSingleParent(g_rotorStore.getObjectByNameOnly(m_WingName));  // NM only needed for backwards compatibility
        ReadCString(ar,m_BEMName);
		setName(m_BEMName);

        ar >> n;

        for (i=0;i<n;i++)
        {
            ar >> f;
            m_Cp.append(f);
            ar >> f;
            m_Ct.append(f);
            ar >> f;
            m_Lambda.append(f);
            ar >> f;
            m_one_over_Lambda.append(f);
            ar >> f;
            m_Kp.append(f);
            if (ArchiveFormat >= 100017)
            {
                ar >> f;
                m_Cm.append(f);
            }
            else m_Cm.append(0);
        }

        for (i=0;i<n;i++)
        {
            BData *pBData = new BData;
            pBData->Serialize(ar,bIsStoring);
            m_BData.append(pBData);
        }

	}
}

BEMData* BEMData::newBySerialize() {
	BEMData* bemData = new BEMData ();
	bemData->serialize();
	return bemData;
}

void BEMData::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_BEMName);
	
	g_serializer.readOrWriteDoubleList1D (&m_Cp);
	g_serializer.readOrWriteDoubleList1D (&m_Ct);
	g_serializer.readOrWriteDoubleList1D (&m_Cm);
	g_serializer.readOrWriteDoubleList1D (&m_Lambda);
	g_serializer.readOrWriteDoubleList1D (&m_one_over_Lambda);
	g_serializer.readOrWriteDoubleList1D (&m_Kp);
	
	// serialize the BData array m_BData
	if (g_serializer.isReadMode()) {
		int n = g_serializer.readInt();
		for (int i = 0; i < n; ++i) {
			m_BData.append(BData::newBySerialize());
		}
	} else {
		g_serializer.writeInt(m_BData.size());
		for (int i = 0; i < m_BData.size(); ++i) {
			m_BData[i]->serialize();
		}
	}
	
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
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bRootLoss);
	g_serializer.readOrWriteBool (&m_b3DCorrection);
	g_serializer.readOrWriteBool (&m_bInterpolation);
	g_serializer.readOrWriteBool (&m_bNewTipLoss);
	g_serializer.readOrWriteBool (&m_bNewRootLoss);
	g_serializer.readOrWriteBool (&m_bCdReynolds);
}

void BEMData::restorePointers() {
	StorableObject::restorePointers();
	
	BData *bData;
	for (int i = 0; i < m_BData.size(); ++i) {
		bData = m_BData[i];
		for (int j = 0; j < bData->m_PolarPointers.size(); ++j) {
			g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&bData->m_PolarPointers[j]));	
		}
		for (int j = 0; j < bData->m_PolarPointersTO.size(); ++j) {
			g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&bData->m_PolarPointersTO[j]));	
		}
	}
}
