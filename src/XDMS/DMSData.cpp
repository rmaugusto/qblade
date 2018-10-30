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

#include <QString>
#include <QList>
#include <QDebug>
#include <QProgressDialog>

#include "../XBEM/Blade.h"
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"
#include "../MainFrame.h"
#include "DData.h"
#include "../Graph/NewCurve.h"
#include "../ColorManager.h"
#include "../ParameterViewer.h"


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
	
	m_windspeed = 7;
	m_tipSpeedFrom = 1;
	m_tipSpeedTo = 10;
	m_tipSpeedDelta = 0.5;
	m_windspeedFrom = 1;
	m_windspeedTo = 20;
	m_windspeedDelta = 0.5;
}

DMSData::DMSData(ParameterViewer<Parameter::DMSData> *viewer) {
	m_bShowPoints = false;
	m_bIsVisible = true;
	m_Style = 0;
	m_Width = 1;
	m_bPowerLaw = false;
	m_bConstant = false;
	m_bLogarithmic = false;
	exponent = 0.4;
	roughness = 1;
	m_bAspectRatio = false;

	viewer->storeObject(this);

	m_DMSName = getName();
}

DMSData::~DMSData()
{
	for (int i = 0; i < m_DData.size(); ++i) {
		delete m_DData[i];
	}
}

QStringList DMSData::prepareMissingObjectMessage() {
	if (g_dmsdataStore.isEmpty()) {
		QStringList message = CBlade::prepareMissingObjectMessage(true);
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == DMS && g_mainFrame->m_iView == BEMSIMVIEW) {
				message = QStringList(">>> Click 'Define Simulation' to create a new DMS Simulation");
			} else {
				message = QStringList(">>> Click 'Define Simulation' to create a new DMS Simulation");
			}
		}
		message.prepend("- No DMS Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

QVariant DMSData::accessParameter(Parameter::DMSData::Key key, QVariant value) {
	typedef Parameter::DMSData P;

	const bool set = value.isValid();
	switch (key) {
	case P::Name: if(set) setName(value.toString()); else value = getName(); break;
	case P::Rho: if(set) rho = value.toDouble(); else value = rho; break;
	case P::Viscosity: if(set) visc = value.toDouble(); else value = visc; break;
	case P::Discretize: if(set) elements = value.toDouble(); else value = elements; break;
	case P::MaxIterations: if(set) iterations = value.toDouble(); else value = iterations; break;
	case P::MaxEpsilon: if(set) epsilon = value.toDouble(); else value = epsilon; break;
	case P::RelaxFactor: if(set) relax = value.toDouble(); else value = relax; break;
	case P::TipLoss: if(set) m_bTipLoss = value.toBool(); else value = m_bTipLoss; break;
	case P::VariableInduction: if(set) m_bVariable = value.toBool(); else value = m_bVariable; break;
	case P::TipSpeedFrom: if(set) m_tipSpeedFrom = value.toDouble(); else value = m_tipSpeedFrom; break;
	case P::TipSpeedTo: if(set) m_tipSpeedTo = value.toDouble(); else value = m_tipSpeedTo; break;
	case P::TipSpeedDelta: if(set) m_tipSpeedDelta = value.toDouble(); else value = m_tipSpeedDelta; break;
	case P::Windspeed: if(set) m_windspeed = value.toDouble(); else value = m_windspeed; break;
	}

	return (set ? QVariant() : value);
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
    m_P.clear();
    m_T.clear();
    m_V.clear();
    m_Omega.clear();
    m_Thrust.clear();
}


void DMSData::Compute(DData *pDData, CBlade *pWing, double lambda, double inflowspeed)
{

    pDData->elements = elements;
    pDData->epsilon = epsilon;
    pDData->iterations = iterations;
    pDData->m_bTipLoss = m_bTipLoss;
    pDData->m_bAspectRatio = m_bAspectRatio;
    pDData->m_bVariable = m_bVariable;

    pDData->relax = relax;
    pDData->rho = rho;
    pDData->visc = visc;
    pDData->Toff = 0;

    pDData->bPowerLaw = m_bPowerLaw;
    pDData->bConstant = m_bConstant;
    pDData->bLogarithmic = m_bLogarithmic;
    pDData->exponent = exponent;
    pDData->roughness = roughness;
    pDData->windspeed = inflowspeed;

    pDData->Init(pWing,lambda,0);
    pDData->OnDMS(pWing);

    double rot = lambda / pWing->m_MaxRadius *60 / 2 / PI * inflowspeed;

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
        m_Omega.append(rot);
        m_P.append(pDData->power);
        m_T.append(pDData->torque);
        m_Thrust.append(pDData->thrust);
        m_V.append(inflowspeed);
    }

}

void DMSData::serialize() {
	StorableObject::serialize();
    if (g_serializer.getArchiveFormat() >= 100052) {
		ShowAsGraphInterface::serialize();
	}

	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_DMSName);

    if (g_serializer.getArchiveFormat() >= 100027) {
		g_serializer.readOrWriteDoubleList1D (&m_P);
		g_serializer.readOrWriteDoubleList1D (&m_T);
		g_serializer.readOrWriteDoubleList1D (&m_V);
		g_serializer.readOrWriteDoubleList1D (&m_Omega);
		g_serializer.readOrWriteDoubleList1D (&m_Thrust);
    } else if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100027){
      for (int i=0;i<m_Cp.size();i++){
          m_P.append(0);
          m_T.append(0);
          m_V.append(0);
          m_Omega.append(0);
          m_Thrust.append(0);
      }
    }
	
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
	
	if (g_serializer.getArchiveFormat() >= 100032) {
		g_serializer.readOrWriteDouble (&m_windspeedFrom);
		g_serializer.readOrWriteDouble (&m_windspeedTo);
		g_serializer.readOrWriteDouble (&m_windspeedDelta);
	}
}

void DMSData::restorePointers() {
	StorableObject::restorePointers();  // TODO more here?
}

QStringList DMSData::getAvailableVariables(NewGraph::GraphType graphType) {
	QStringList variables;

	switch (graphType) {  // WARNING: when changing any variables list, change newCurve as well!
	case NewGraph::TurbineRotor:
		variables << "Cp" << "Cp upwind" << "Cp downwind" << "Cm" << "Cm upwind" << "Cm downwind" << "Kp" <<
					 "Tip Speed Ratio" << "1 / Tip Speed Ratio" << "Power" << "Thrust" << "Torque" <<
					 "Rotational Speed" << "Windspeed";
		break;
	default:
		break;
	}

	return variables;
}

NewCurve *DMSData::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType) {
	if (xAxis == "" || yAxis == "" || !hasResults())
		return NULL;

	QList<double> xList, yList;
	switch (graphType) {
	case NewGraph::TurbineRotor:
	{
		for (int i = 0; i < 2; ++i) {
			const int index = getAvailableVariables(graphType).indexOf(i == 0 ? xAxis : yAxis);
			QList<double>* list = (i == 0 ? &xList : &yList);

			switch (index) {
			case  0: *list = m_Cp; break;
			case  1: *list = m_Cp1; break;
			case  2: *list = m_Cp2; break;
			case  3: *list = m_Cm; break;
			case  4: *list = m_Cm1; break;
			case  5: *list = m_Cm2; break;
			case  6: *list = m_Kp; break;
			case  7: *list = m_Lambda; break;
			case  8: *list = m_one_over_Lambda; break;
			case  9: *list = m_P; break;
			case 10: *list = m_Thrust; break;
			case 11: *list = m_T; break;
			case 12: *list = m_Omega; break;
			case 13: *list = m_V; break;
			default: return NULL;
			}
		}

		NewCurve *curve = new NewCurve(this);
		// dimension can be taken from any list (here m_Lambda.size()), it's all the same
		curve->setAllPoints(xList.toVector().data(), yList.toVector().data(), m_Lambda.size());
		return curve;
	}
	default:
		return NULL;
	}
}

void DMSData::startSimulation() {  // NM copied from QDMS::OnStartRotorSimulation
	const int times = int((m_tipSpeedTo-m_tipSpeedFrom)/m_tipSpeedDelta);

	QProgressDialog progress("", "Abort DMS", 0, times, g_mainFrame);
	progress.setModal(true);

	CBlade *blade = static_cast<CBlade*> (getParent());

	for (int i = 0; i <= times; ++i) {
		if (progress.wasCanceled())
			break;

		QString curlambda;
		curlambda.sprintf("%.2f", m_tipSpeedFrom + i*m_tipSpeedDelta);
		QString text = "Compute DMS for Lambda " + curlambda;
		progress.setLabelText(text);
		progress.setValue(i);

		DData *data = new DData (m_objectName);
		Compute(data, blade, m_tipSpeedFrom + i*m_tipSpeedDelta, m_windspeed);  // NM appends data to m_DData

		if (!data->m_bBackflow) {
			data->m_Color = g_mainFrame->GetColor(15);  // old DMS
			data->pen()->setColor(g_colorManager.getColor(m_DData.size()-1));
		}
	}
}

QPen DMSData::doGetPen (int curveIndex, int highlightedIndex, bool forTheDot) {
	if (highlightedIndex == -1) {  // in case of "only one curve"
		return m_pen;
	} else {
		QPen pen (m_pen);
		pen.setColor(g_colorManager.getColor(curveIndex));
		if (curveIndex == highlightedIndex && !forTheDot) {
			pen.setWidth(pen.width()+2);
		}
		return pen;
	}
}

DMSData* DMSData::newBySerialize() {
	DMSData* dmsData = new DMSData ();
	dmsData->serialize();
	return dmsData;
}
