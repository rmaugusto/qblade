/****************************************************************************

    BEMData Class
        Copyright (C) 2010 David Marten david.marten@tu-berlin.de

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
#include <QString>
#include <QList>
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"
#include <QDebug>
#include "../MainFrame.h"


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
    m_Omega.clear();
    m_V.clear();
    m_P.clear();
    m_Torque.clear();
    m_S.clear();
    m_Bending.clear();
}

void BEMData::Compute(BData *pBData, CBlade *pWing, double lambda, double windspeed)
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

    pBData->OnBEM(0, pWing, windspeed);

    m_BData.append(pBData);
    m_Cp.append(pBData->cp);
    m_Cm.append(pBData->cp/pBData->lambda_global);
    m_Lambda.append(pBData->lambda_global);
    m_one_over_Lambda.append(1/pBData->lambda_global);
    m_Ct.append(pBData->ct);
    m_Kp.append(pBData->cp / pBData->lambda_global / pBData->lambda_global / pBData->lambda_global);

    double rot = lambda*windspeed*60/2/PI/pWing->m_TPos[pWing->m_NPanel];
    m_Omega.append(rot);
    m_V.append(windspeed);
    m_P.append(PI/2*pow(pWing->m_TPos[pWing->m_NPanel],2)*rho*pow(windspeed,3)*pBData->cp);
    m_Torque.append(PI/2*pow(pWing->m_TPos[pWing->m_NPanel],2)*rho*pow(windspeed,3)*pBData->cp/(rot/60*2*PI));
    m_S.append(pow(pWing->m_TPos[pWing->m_NPanel],2)*PI*rho/2*pow(windspeed,2)*pBData->ct);
    double bending = 0;
    for (int d=0;d<pBData->m_Reynolds.size();d++)
    {
        bending = bending + pBData->m_p_normal.at(d)*pBData->deltas.at(d)*pBData->m_pos.at(d);
    }
    m_Bending.append(bending);
}

QStringList BEMData::prepareMissingObjectMessage() {
	if (g_bemdataStore.isEmpty()) {
		QStringList message = CBlade::prepareMissingObjectMessage(false);
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == BEM && g_mainFrame->m_iView == BEMSIMVIEW) {
				message = QStringList(">>> Click 'Define Simulation' to create a new BEM Simulation");
			} else {
				message = QStringList(">>> unknown hint");
			}
		}
		message.prepend("- No BEM Simulation in Database");
		return message;
	} else {
		return QStringList();
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

    if (g_serializer.getArchiveFormat() >= 100027){
    g_serializer.readOrWriteDoubleList1D (&m_P);
    g_serializer.readOrWriteDoubleList1D (&m_S);
    g_serializer.readOrWriteDoubleList1D (&m_V);
    g_serializer.readOrWriteDoubleList1D (&m_Omega);
    g_serializer.readOrWriteDoubleList1D (&m_Bending);
    g_serializer.readOrWriteDoubleList1D (&m_Torque);
    }
    else if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100027){
      for (int i=0;i<m_Cp.size();i++){
          m_P.append(0);
          m_S.append(0);
          m_V.append(0);
          m_Omega.append(0);
          m_Bending.append(0);
          m_Torque.append(0);
      }
    }

	
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
}
