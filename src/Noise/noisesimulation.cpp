#include "noisesimulation.h"
#include "../Graph/NewCurve.h"


NoiseCalculation *NoiseSimulation::Calculation() const
{
    return m_NoiseCalculation;
}

void NoiseSimulation::setCalculation(NoiseCalculation *NoiseCalculation)
{
    m_NoiseCalculation = NoiseCalculation;
}
NoiseSimulation::NoiseSimulation() :
    StorableObject (QString("test"))
{

    m_NoiseCalculation = NULL;

    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);
    m_availableVariables.append("");
    m_availableVariables.append("TIME");
    m_availableVariables.append("SPEED");

    QVector <double> vet1;
    vet1.append(0.0);
    vet1.append(0.6);
    vet1.append(1.0);
    vet1.append(0.5);
    vet1.append(1.0);

    QVector <double> vet2;
    vet2.append(0.0);
    vet2.append(0.6);
    vet2.append(1.0);
    vet2.append(0.0);

    m_results.append(vet1);
    m_results.append(vet2);

}

NewCurve* NoiseSimulation::newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType){


    int xAxisIndex = m_availableVariables.indexOf(xAxis);
    int yAxisIndex = m_availableVariables.indexOf(yAxis);

    if (xAxisIndex == -1 || yAxisIndex == -1) {
        return NULL;
    } else if (m_results.size()){
        NewCurve *curve = new NewCurve (this);

//        curve->addPoint(0.0,0.0);
//        curve->addPoint(0.6,0.6);
//        curve->addPoint(1,1);
//        curve->addPoint(0.5,0.7);
//        curve->addPoint(1,0);

        curve->setAllPoints(m_results[xAxisIndex].data(),
                            m_results[yAxisIndex].data(),
                            m_results[0].size());  // numberOfRows is the same for all results
        return curve;
    }

}

QStringList NoiseSimulation::getAvailableVariables (NewGraph::GraphType graphType){
    switch (graphType) {
    case NewGraph::NoiseSimulationGraph:
        return m_availableVariables;
    default:
        return QStringList();
    }

}

void NoiseSimulation::serialize() {
    StorableObject::serialize();
    ShowAsGraphInterface::serialize();
}

void NoiseSimulation::restorePointers() {
    StorableObject::restorePointers();

}

NoiseSimulation* NoiseSimulation::newBySerialize() {
    NoiseSimulation* fastSimulation = new NoiseSimulation ();
    fastSimulation->serialize();
    return fastSimulation;
}

NoiseSimulation::~NoiseSimulation(){

    if(m_NoiseCalculation)
        delete m_NoiseCalculation;

}
