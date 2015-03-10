#include "noisesimulation.h"
#include "../Graph/NewCurve.h"

NoiseSimulation::NoiseSimulation() :
    StorableObject (QString("test"))
{

    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);
    //m_module = g_NoiseModule;

}

NewCurve* NoiseSimulation::newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType){

    NewCurve *curve = new NewCurve (this);
    curve->addPoint(0.0,0.0);
    curve->addPoint(1.0,1.0);
    curve->addPoint(5.0,5.0);
//    curve->setAllPoints(m_results[xAxisIndex].data(),
//                        m_results[yAxisIndex].data(),
//                        m_results[0].size());  // numberOfRows is the same for all results


    return curve;
}

QStringList NoiseSimulation::getAvailableVariables (NewGraph::GraphType graphType){
    return QStringList();
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

}
