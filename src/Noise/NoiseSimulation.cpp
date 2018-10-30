#include "NoiseSimulation.h"

#include "../ParameterViewer.h"
#include "../Store.h"
#include "../Objects/Polar.h"
#include "../Objects/OpPoint.h"
#include "../Graph/NewCurve.h"
#include "NoiseModule.h"
#include "../ColorManager.h"
#include "NoiseOpPoint.h"


NoiseSimulation *NoiseSimulation::newBySerialize() {
	NoiseSimulation *simulation = new NoiseSimulation;
	simulation->serialize();
	return simulation;
}

NoiseSimulation::NoiseSimulation(ParameterViewer<Parameter::NoiseSimulation> *viewer) {
	viewer->storeObject(this);
	pen()->setColor(g_colorManager.getLeastUsedColor(&g_noiseSimulationStore));	
}

void NoiseSimulation::serialize() {
	StorableObject::serialize();
	ShowAsGraphInterface::serialize();
	
	m_parameter.serialize();
	m_calculation.serialize();
}

void NoiseSimulation::restorePointers() {
	StorableObject::restorePointers();

	m_parameter.restorePointers();
}

QPen NoiseSimulation::doGetPen(int curveIndex, int highlightedIndex, bool) {
	if (curveIndex == -1 || highlightedIndex == -1) {
		return m_pen;
	} else {
		QPen pen (m_pen);
		
		if (g_noiseModule->isColorByOpPoint()) {
			pen.setColor (g_colorManager.getColor(curveIndex));
		}
		
		if (curveIndex == highlightedIndex) {
			pen.setWidth(pen.width()+2);
		}
		
		return pen;
	}
}

NewCurve *NoiseSimulation::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType /*graphType*/, int opPointIndex) {
	if (xAxis == "" || yAxis == "")
		return NULL;
	
	bool zeroY = false;
	QVector<double> xVector, yVector;  // because QVector is internally shared there should be no copying
	for (int i = 0; i < 2; ++i) {
		const int index = getAvailableVariables().indexOf(i == 0 ? xAxis : yAxis);
		QVector<double> *vector = (i == 0 ? &xVector : &yVector);
		switch (index) {
		case 0: *vector = NoiseCalculation::CENTRAL_BAND_FREQUENCY; break;
		case 1: *vector = m_calculation.SPLadB()[opPointIndex]; zeroY = true; break;
		case 2: *vector = m_calculation.SPLsdB()[opPointIndex]; zeroY = true; break;
		case 3: *vector = m_calculation.SPLpdB()[opPointIndex]; zeroY = true; break;
		case 4: *vector = m_calculation.SPLdB()[opPointIndex]; break;
		case 5: *vector = m_calculation.SPLdBAW()[opPointIndex]; break;
		case 6: *vector = m_calculation.SPLdBBW()[opPointIndex]; break;
		case 7: *vector = m_calculation.SPLdBCW()[opPointIndex]; break;
		default: return NULL;
		}
	}
	
	NewCurve *curve = new NewCurve (this);
//	curve->setAllPoints(xVector.data(), yVector.data(), xVector.size());
	for (int i = 0; i < xVector.size(); ++i) {  // zero the y values lower 0 for certain outputs
		curve->addPoint(xVector[i], (zeroY && yVector[i] < 0 ? 0.0 : yVector[i]));
	}
	return curve;
}

QStringList NoiseSimulation::getAvailableVariables(NewGraph::GraphType /*graphType*/) {
	QStringList variables;
	
	// WARNING: when changing any variables list, change newCurve as well!
	variables << "Freq [Hz]" << "SPL_alpha" << "SPL_S" << "SPL_P" << "SPL (dB)" << "SPL (dB(A))" << "SPL (dB(B))"
			  << "SPL (dB(C))";
	
	return variables;
}

QStringList NoiseSimulation::prepareMissingObjectMessage() {
	if (g_noiseSimulationStore.isEmpty()) {
		QStringList message = CPolar::prepareMissingObjectMessage();
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == NOISEMODULE) {
				message = QStringList(">>> Click 'New' to create a new Noise Simulation");
			} else {
				message = QStringList(">>> unknown hint");
			}
		}
		message.prepend("- No Noise Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

void NoiseSimulation::simulate() {
	m_calculation.setNoiseParam(&m_parameter);
	m_calculation.calculate();
}

void NoiseSimulation::exportCalculation(QTextStream &stream) {
	stream.setRealNumberNotation(QTextStream::FixedNotation);
	stream.setRealNumberPrecision(5);
	
	stream << "Noise prediction file export" << endl;
	stream << endl;
	
	QList<NoiseOpPoint*> noiseOpPoints = m_parameter.prepareNoiseOpPointList();
	for (int i = 0; i < noiseOpPoints.size(); ++i) {
		stream << qSetFieldWidth(0);
		stream << "Alpha: " << noiseOpPoints[i]->getAlphaDegree() <<
				  ", Re = " << noiseOpPoints[i]->getReynolds() << endl;
		stream << "OASPL: " << m_calculation.OASPL()[i] << " dB" << endl;		
		stream << "OASPL (A): " << m_calculation.OASPLA()[i] << " dB(A)" << endl;
		stream << "OASPL (B): " << m_calculation.OASPLB()[i] << " dB(B)" << endl;
		stream << "OASPL (C): " << m_calculation.OASPLC()[i] << " dB(C)" << endl;
		stream << "SPL_a: " << m_calculation.SPLALOG()[i] << "" << endl;
		stream << "SPL_s: " << m_calculation.SPLSLOG()[i] << "" << endl;
		stream << "SPL_p: " << m_calculation.SPLPLOG()[i] << "" << endl;
		stream << endl;
		
		stream << qSetFieldWidth(14) <<
				  "Freq [Hz]" <<
				  "SPL (dB)" <<
				  "SPLa" <<
				  "SPLs" <<
				  "SPLp" <<
				  "SPL (dB(A))" <<
				  "SPL (dB(B))" <<
				  "SPL (dB(C))" << endl;
		
		for (int j = 0; j < NoiseCalculation::FREQUENCY_TABLE_SIZE; ++j) {
			stream << NoiseCalculation::CENTRAL_BAND_FREQUENCY[j] <<
					  m_calculation.SPLdB()[i][j] <<
					  m_calculation.SPLadB()[i][j] <<
					  m_calculation.SPLsdB()[i][j] <<
					  m_calculation.SPLpdB()[i][j] <<
					  m_calculation.SPLdBAW()[i][j] <<
					  m_calculation.SPLdBBW()[i][j] <<
					  m_calculation.SPLdBCW()[i][j] << endl;
		}
		
		stream << endl;
		stream << endl;
	}
	qDeleteAll(noiseOpPoints);	
}

void NoiseSimulation::setAnalyzedOpPoints(QVector<OpPoint *> newList) {
	removeAllParents();
	for (int i = 0; i < newList.size(); ++i) {
		addParent(newList[i]);
	}
	
	m_parameter.analyzedOpPoints = newList;
}

QVariant NoiseSimulation::accessParameter(Parameter::NoiseSimulation::Key key, QVariant value) {
	typedef Parameter::NoiseSimulation P;
	
	const bool set = value.isValid();
	switch (key) {
	case P::Name: if(set) setName(value.toString()); else value = getName(); break;
	case P::WettedLength:
		if(set) m_parameter.wettedLength = value.toDouble();
		else value = m_parameter.wettedLength; break;
	case P::DistanceObsever:
		if(set) m_parameter.distanceObsever = value.toDouble();
		else value = m_parameter.distanceObsever; break;
	case P::OriginalVelocity:
		if(set) m_parameter.originalVelocity = value.toDouble();
		else value = m_parameter.originalVelocity; break;
	case P::OriginalChordLength:
		if(set) m_parameter.originalChordLength = value.toDouble();
		else value = m_parameter.originalChordLength; break;
	case P::OriginalMach:
		if(set) m_parameter.originalMach = value.toDouble();
		else value = m_parameter.originalMach; break;
	case P::DStarChordStation:
		if(set) m_parameter.dStarChordStation = value.toDouble();
		else value = m_parameter.dStarChordStation; break;
	case P::DStarScalingFactor:
		if(set) m_parameter.dStarScalingFactor = value.toDouble();
		else value = m_parameter.dStarScalingFactor; break;
	case P::EddyConvectionMach:
		if(set) m_parameter.eddyConvectionMach = value.toDouble();
		else value = m_parameter.eddyConvectionMach; break;
	case P::DirectivityTheta:
		if(set) m_parameter.directivityGreek = value.toDouble();
		else value = m_parameter.directivityGreek; break;
	case P::DirectivityPhi:
		if(set) m_parameter.directivityPhi = value.toDouble();
		else value = m_parameter.directivityPhi; break;
	case P::SeparatedFlow:
		if(set) m_parameter.separatedFlow = value.toBool();
		else value = m_parameter.separatedFlow; break;
	case P::SuctionSide:
		if(set) m_parameter.suctionSide = value.toBool();
		else value = m_parameter.suctionSide; break;
	case P::PressureSide:
		if(set) m_parameter.pressureSide = value.toBool();
		else value = m_parameter.pressureSide; break;
	case P::Aoa:
		if(set) m_parameter.aoa = value.toDouble();
		else value = m_parameter.aoa; break;
	case P::ChordBasedReynolds:
		if(set) m_parameter.chordBasedReynolds = value.toDouble();
		else value = m_parameter.chordBasedReynolds; break;
	case P::Transition:
		if(set) m_parameter.transition = static_cast<NoiseParameter::TransitionType>(value.toInt());
		else value = static_cast<int>(m_parameter.transition); break;
	}

	return (set ? QVariant() : value);
}
