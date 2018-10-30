/****************************************************************************

    TBEMData Class
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

#include "TBEMData.h"
#include "../Globals.h"
#include "BEM.h"
#include "../Store.h"
#include "../Serializer.h"


TBEMData::TBEMData()
	: BEMData()
{
    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;

    elements = 100;
    epsilon = 0.001;
    iterations = 1000;
    relax = 0.3;
    m_bTipLoss = false;
    m_bRootLoss = false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
    m_bNewTipLoss = false;
    m_bNewRootLoss = false;
    visc = 0.0000178;
    rho = 1.2;

    OuterRadius = 0;
}

void TBEMData::Clear()
{
    m_Omega.clear();
    m_V.clear();
    for (int i=0;i<m_BData.size();i++)
    {
        if (m_BData.at(i)) delete m_BData.at(i);
    }
    m_BData.clear();
    m_P.clear();
    m_Cp.clear();
    m_Ct.clear();
    m_Lambda.clear();
    m_S.clear();
    m_Pitch.clear();
    m_Weibull.clear();
	//m_WeibullV3.clear();
    m_one_over_Lambda.clear();
    m_Kp.clear();
    m_Bending.clear();
    m_Torque.clear();
	m_Cm.clear();
	m_aepk.clear();//new code JW
	m_aepA.clear();//new code JW
}

void TBEMData::Compute(BData *pBData, CBlade *pWing, double lambda, double pitch, double windspeed)
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
    pBData->m_bCdReynolds = m_bCdReynolds;


    pBData->Init(pWing,lambda);
    pBData->OnBEM(pitch, pWing, windspeed);


}

QStringList TBEMData::prepareMissingObjectMessage() {
	if (g_tbemdataStore.isEmpty()) {
		QStringList message = TData::prepareMissingObjectMessage(false);
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == BEM && g_mainFrame->m_iView == TURBINEVIEW) {
				message = QStringList(">>> Click 'Define Simulation' to create a new Turbine Simulation");
			} else {
				message = QStringList(">>> unknown hint");
			}
		}
		message.prepend("- No Turbine Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

TBEMData* TBEMData::newBySerialize() {
	TBEMData* tBemData = new TBEMData ();
	tBemData->serialize();
	return tBemData;	
}

void TBEMData::serialize() {
	BEMData::serialize();
	
	g_serializer.readOrWriteString (&m_TurbineName);
	g_serializer.readOrWriteString (&m_SimName);
	
	g_serializer.readOrWriteDoubleList1D (&m_P);
	g_serializer.readOrWriteDoubleList1D (&m_S);
	g_serializer.readOrWriteDoubleList1D (&m_V);
	g_serializer.readOrWriteDoubleList1D (&m_Omega);
	g_serializer.readOrWriteDoubleList1D (&m_Pitch);
	g_serializer.readOrWriteDoubleList1D (&m_Bending);
	g_serializer.readOrWriteDoubleList1D (&m_Cp_loss);
	g_serializer.readOrWriteDoubleList1D (&m_Torque);
	
	g_serializer.readOrWriteDouble (&OuterRadius);
	
	g_serializer.readOrWriteDoubleList1D (&m_Weibull);
	g_serializer.readOrWriteDoubleList1D (&m_aepk);
	g_serializer.readOrWriteDoubleList1D (&m_aepA);
}
