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

#include <QProgressDialog>

#include "TDMSData.h"
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"
#include "../MainFrame.h"
#include "../Graph/NewCurve.h"
#include "TurDmsModule.h"
#include "../ColorManager.h"
#include <QDebug>
#include <QPen>
#include "../ParameterViewer.h"


QList<double> TDMSData::kWeibull;
QList<double> TDMSData::aWeibull;

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

TDMSData::TDMSData(ParameterViewer<Parameter::TDMSData> *viewer) {
	m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;
    m_bAspectRatio = false;
    m_bPowerLaw = false;
    m_bConstant = false;
    m_bLogarithmic = false;
    exponent = 0.4;
    roughness = 1;

	viewer->storeObject(this);
	
	m_SimName = getName();
	m_Color = g_mainFrame->GetColor(14);  // for the old DMS module
	
	calculateWeibull();
}

TDMSData::~TDMSData() {
	
}

QStringList TDMSData::prepareMissingObjectMessage() {
	if (g_tdmsdataStore.isEmpty()) {
		QStringList message = TData::prepareMissingObjectMessage(true);
		if (message.isEmpty()) {
			if ((g_mainFrame->m_iApp == DMS && g_mainFrame->m_iView == TURBINEVIEW) || g_mainFrame->m_iApp == TURDMSMODULE) {
				message = QStringList(">>> Click 'Define Simulation' to create a new Turbine Simulation");
			} else {
				message = QStringList(">>> Click 'Define Simulation' to create a new Turbine Simulation");
			}
		}
		message.prepend("- No Turbine Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

void TDMSData::calculateWeibull() {  //NM copied from QDMS::OnSetWeibull()
	double A,k,PMk,PMA,V,f,energy;

	g_turDmsModule->getWeibullParameters(&k, &PMk, &A, &PMA);

	m_Weibull.clear();
	// reset selected A and k
	A = aWeibull[50];
	k = kWeibull[50];
	
	// calc weibull probability for selected A and k
	for (int j=0;j<m_V.size();j++)
	{
		V=m_V.at(j);
		
		f=k/A*pow((V/A),(k-1))*exp(-pow((V/A),k));
		m_Weibull.append(f);
	}
	
	
	// calc aep for A range
	m_aepA.clear();
	
	for (int j=0; j<aWeibull.size(); j++)
	{
		A = aWeibull[j];
		k = kWeibull[50];
		
		// sum up annual yield for temp A
		energy=0;
		for (int m=0;m<m_V.size()-1;m++)
		{
			f = exp(-pow(m_V.at(m)/A,k))-exp(-pow(m_V.at(m+1)/A,k));
			
			energy = energy + 0.5*(m_P.at(m)+m_P.at(m+1))*f*8760;
		}
		m_aepA.append(energy);
	}
	
	// calc aep for k range
	m_aepk.clear();
	
	for (int j=0; j<kWeibull.size(); j++)
	{
		A = aWeibull[50];
		k = kWeibull[j];
		
		// sum up annual yield for temp k
		energy=0;
		for (int m=0;m<m_V.size()-1;m++)
		{
			f = exp(-pow(m_V.at(m)/A,k))-exp(-pow(m_V.at(m+1)/A,k));
			
			energy = energy + 0.5*(m_P.at(m)+m_P.at(m+1))*f*8760;
		}
		m_aepk.append(energy);
	}
}

QVariant TDMSData::accessParameter(Parameter::TDMSData::Key key, QVariant value) {
	typedef Parameter::TDMSData P;
	
	const bool set = value.isValid();
	switch (key) {
	case P::Name: if(set) setName(value.toString()); else value = getName(); break;
	case P::Rho: if(set) rho = value.toDouble(); else value = rho; break;
	case P::Viscosity: if(set) visc = value.toDouble(); else value = visc; break;
	case P::Discretize: if(set) elements = value.toDouble(); else value = elements; break;
	case P::MaxIterations: if(set) iterations = value.toDouble(); else value = iterations; break;
	case P::MaxEpsilon: if(set) epsilon = value.toDouble(); else value = epsilon; break;
	case P::RelaxFactor: if(set) relax = value.toDouble(); else value = relax; break;
	case P::WindspeedFrom: if(set) m_windspeedFrom = value.toDouble(); else value = m_windspeedFrom; break;
	case P::WindspeedTo: if(set) m_windspeedTo = value.toDouble(); else value = m_windspeedTo; break;
	case P::WindspeedDelta: if(set) m_windspeedDelta = value.toDouble(); else value = m_windspeedDelta; break;
	case P::TipLoss: if(set) m_bTipLoss = value.toBool(); else value = m_bTipLoss; break;
	case P::VariableInduction: if(set) m_bVariable = value.toBool(); else value = m_bVariable; break;
	case P::AnnualYield: if (set) m_aepk[50] = value.toDouble(); else value = m_aepk[50]; break;
	}

	return (set ? QVariant() : value);
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
    pDData->Toff = Toff;

    pDData->bPowerLaw = m_bPowerLaw;
    pDData->bConstant = m_bConstant;
    pDData->bLogarithmic = m_bLogarithmic;
    pDData->exponent = exponent;
    pDData->roughness = roughness;
    pDData->windspeed = windspeed;

	pDData->Init(pWing,lambda,pitch);
    pDData->OnDMS(pWing);

}

void TDMSData::startSimulation() {  // NM copied from void QDMS::OnStartTurbineSimulation()
	DData *data;
	double windspeed, lambda, rot, Toff;
	const int times = int((m_windspeedTo-m_windspeedFrom)/m_windspeedDelta);
	
	TData *turbine = dynamic_cast<TData*> (this->getParent());
	CBlade *blade = dynamic_cast<CBlade*> (turbine->getParent());
	
	QProgressDialog progress("", "Abort DMS", 0, times, g_mainFrame);
	progress.setModal(true);
	
	for (int i = 0; i <= times; ++i) {
		if (progress.wasCanceled())
			break;
		
		windspeed = m_windspeedFrom + m_windspeedDelta * i;
		
		//// check which rotational speed is used (for fixed, 2step and variable)////
		if (turbine->isFixed)
			rot = turbine->Rot1;
		
		if (turbine->isVariable) {
			rot = turbine->Lambda0*windspeed*60/2/PI/turbine->MaxRadius;
			if (rot<turbine->Rot1) rot = turbine->Rot1;
			if (rot>turbine->Rot2) rot = turbine->Rot2;
		}
		
		////// gets the prescribed rotspeed lists and interpolated between windspeeds if neccessary
		if (turbine->isPrescribedRot)
		{
			for (int p=0;p<turbine->rotwindspeeds.size()-1;p++)
			{
				if (windspeed < turbine->rotwindspeeds.at(0))
				{
					rot = 200;
					break;
				}
				if (windspeed > turbine->rotwindspeeds.at(turbine->rotwindspeeds.size()-1))
				{
					rot = 200;
					break;
				}
				if (windspeed == turbine->rotwindspeeds.at(turbine->rotwindspeeds.size()-1))
				{
					rot = turbine->rotspeeds.at(turbine->rotwindspeeds.size()-1);
					break;
				}
				if (windspeed == turbine->rotwindspeeds.at(p))
				{
					rot = turbine->rotspeeds.at(p);
					break;
				}
				if (windspeed > turbine->rotwindspeeds.at(p) && windspeed < turbine->rotwindspeeds.at(p+1))
				{
					rot = turbine->rotspeeds.at(p) + (turbine->rotspeeds.at(p+1)-turbine->rotspeeds.at(p)) *
						  (windspeed-turbine->rotwindspeeds.at(p)) / 
						  (turbine->rotwindspeeds.at(p+1) - turbine->rotwindspeeds.at(p));
					break;
				}
			}
		}
		
		QString curwind;
		curwind.sprintf("%.2f",windspeed);
		progress.setLabelText("Compute DMS for Windspeed " + curwind);
		progress.setValue(i);
		
		lambda = (rot*turbine->MaxRadius/60*2*PI)/windspeed;
		//lambda = turbine->OuterRadius*2*PI/60/windspeed*rot;
		
		Toff = turbine->Offset;
		
		if (windspeed >= turbine->CutIn && windspeed <= turbine->CutOut)
		{
			data = new DData (m_objectName);
			Compute(data,blade,lambda,0,Toff,windspeed);
			
			if (!data->m_bBackflow)
			{
				// fill turbine data
				m_Omega.append(rot);
				m_V.append(windspeed);
				m_DData.append(data);
				
				double P = data->power;
				m_P.append(P);
				
				double Thrust = data->thrust;
				m_Thrust.append(Thrust);
				
				double T = data->torque;
				m_T.append(T);
				
				double P_loss = (1-turbine->VariableLosses) * P - turbine->m_fixedLosses;
				if (P_loss > 0)
				{
					m_P_loss.append(P_loss);
					m_Cp_loss.append(P_loss/P);
				}
				else
				{
					m_P_loss.append(0);
					m_Cp_loss.append(0);
				}
				
//				m_S.append(pow(turbine->OuterRadius,2)*PI*rho/2*pow(windspeed,2)*data->cm);
				
				m_Cp.append(data->cp);
				m_Cp1.append(data->cp1);
				m_Cp2.append(data->cp2);
				m_Cm.append(data->cm);
				m_Cm1.append(data->cm1);
				m_Cm2.append(data->cm2);
				m_w.append(data->w);
				m_Lambda.append(lambda);
				m_one_over_Lambda.append(1/data->lambda_global);
				m_Kp.append(data->cp/pow(data->lambda_global,3));
				m_Weibull.append(0);
				
				data->m_Color = g_mainFrame->m_crColors[(m_DData.size()-1)%24];  // old DMS
				data->pen()->setColor(g_colorManager.getColor(m_DData.size()-1));
			}
			
		}
	}
	
	calculateWeibull();
}

QStringList TDMSData::getAvailableVariables(NewGraph::GraphType graphType, bool xAxis) {
	QStringList variables;
	
	switch (graphType) {  // WARNING: when changing any variables list, change newCurve as well!
	case NewGraph::TurbineRotor:
		variables << "Power" << "Torque" << "Windspeed" << "Tip Speed Ratio" << "1 / Tip Speed Ratio" <<
					 "Rotational Speed" << "Cp" << "Cp upwind" << "Cp downwind" << "Cm" << "Cm upwind" <<
					 "Cm downwind" << "Kp" << "Power P_loss" << "Power loss coefficient Cp_loss" << "f Weibull";
		break;
	case NewGraph::TurbineWeibull:
		if (xAxis) {
			variables << "shape factor k" << "scale factor A";
		} else {
			variables << "Annual Energy Production AEP [kWh]";
		}
		break;
	default:
		break;
	}
	
	return variables;
}

NewCurve *TDMSData::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType) {
	if (xAxis == "" || yAxis == "" || !hasResults())
		return NULL;
	
	QList<double> xList, yList;
	switch (graphType) {
	case NewGraph::TurbineRotor:
	{
		for (int i = 0; i < 2; ++i) {
			const int index = getAvailableVariables(graphType, true).indexOf(i == 0 ? xAxis : yAxis);
			QList<double>* list = (i == 0 ? &xList : &yList);
			
			switch (index) {
			case  0: *list = m_P; break;
			case  1: *list = m_T; break;
			case  2: *list = m_V; break;
			case  3: *list = m_Lambda; break;
			case  4: *list = m_one_over_Lambda; break;
			case  5: *list = m_Omega; break;
			case  6: *list = m_Cp; break;
			case  7: *list = m_Cp1; break;
			case  8: *list = m_Cp2; break;
			case  9: *list = m_Cm; break;
			case 10: *list = m_Cm1; break;
			case 11: *list = m_Cm2; break;
			case 12: *list = m_Kp; break;
			case 13: *list = m_P_loss; break;
			case 14: *list = m_Cp_loss; break;
			case 15: *list = m_Weibull; break;
			default: return NULL;
			}
		}

		NewCurve *curve = new NewCurve(this);
		// dimension can be taken from any list (here m_P.size()), it's all the same
		curve->setAllPoints(xList.toVector().data(), yList.toVector().data(), m_P.size());
		return curve;
	}
	case NewGraph::TurbineWeibull:
	{
		// for the Weibull graph the y array depends only on the selected x array.
		const int index = getAvailableVariables(graphType, true).indexOf(xAxis);
		if (index == -1) {
			return NULL;
		} else {
			QList<double> yList = (index == 0 ? m_aepk : m_aepA);
			for (int i = 0; i < yList.size(); ++i) {  // divide by 1000 because graph shows kWh, but Wh is stored in list
				yList[i] /= 1000;
			}
			
			NewCurve *curve = new NewCurve(this);
			curve->setAllPoints((index == 0 ? kWeibull : aWeibull).toVector().data(),
								yList.toVector().data(),
								kWeibull.size());
			return curve;
		}
	}
	default:
		return NULL;
	}
}

TDMSData* TDMSData::newBySerialize() {
	TDMSData* tDmsData = new TDMSData ();
	tDmsData->serialize();
	return tDmsData;
}

void TDMSData::serialize() {
	DMSData::serialize();
	if (g_serializer.getArchiveFormat() >= 100032) {
		ShowAsGraphInterface::serialize();
	}
	
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
