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
    StorableObject (QString(""))
{

    m_NoiseCalculation = NULL;

    setShownInGraph(true);
    setDrawCurve(true);
    setDrawPoints(true);

    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);


    m_availableVariables.clear();
    m_availableVariables.append("Freq [Hz]");
    m_availableVariables.append("SPL_alpha");
    m_availableVariables.append("SPL_S");
    m_availableVariables.append("SPL_P");
    m_availableVariables.append("SPL (dB)");
    m_availableVariables.append("SPL (dB(A))");
    m_availableVariables.append("SPL (dB(B))");
    m_availableVariables.append("SPL (dB(C))");

}

NewCurve* NoiseSimulation::newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType,int opPointIdx){


    int xAxisIndex = m_availableVariables.indexOf(xAxis);
    int yAxisIndex = m_availableVariables.indexOf(yAxis);

    if (xAxisIndex == -1 || yAxisIndex == -1) {
        return NULL;
    } else if (m_results.size()){
        NewCurve *curve = new NewCurve (this);

        //g_NoiseSimulationStore.at(i)->Calculation()->NoiseParam()->OpPoints();

        curve->setAllPoints(m_results[opPointIdx][xAxisIndex].data(),
                            m_results[opPointIdx][yAxisIndex].data(),
                            m_results[opPointIdx][0].size());  // numberOfRows is the same for all results
        return curve;
    }

    return NULL;

}

NewCurve *NoiseSimulation::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType)
{
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

double NoiseSimulation::getPositiveNumber(double number)
{
    return number < 0 ? 0 : number ;
}

void NoiseSimulation::simulate()
{

    m_results.clear();


    for (int j = 0; j < m_NoiseCalculation->NoiseParam()->OpPoints().size(); ++j) {

        QVector < QVector <double> > resultOp;

        QVector <double> vetFreq;
        QVector <double> vetA;
        QVector <double> vetS;
        QVector <double> vetP;
        QVector <double> vetDb;
        QVector <double> vetDbA;
        QVector <double> vetDbB;
        QVector <double> vetDbC;

        for (int i = 0; i < Noise::FREQUENCY_TABLE_SIZE; ++i) {
            //vetFreq.append( Noise::CENTRAL_BAND_FREQUENCY[i] );
            //vetA.append( sin(Noise::CENTRAL_BAND_FREQUENCY[i])*10 );

            vetFreq.append( Noise::CENTRAL_BAND_FREQUENCY[i] );
            vetA.append( getPositiveNumber(m_NoiseCalculation->SPLadB()[j][i]) );
            vetS.append( getPositiveNumber(m_NoiseCalculation->SPLsdB()[j][i]) );
            vetP.append( getPositiveNumber(m_NoiseCalculation->SPLpdB()[j][i]) );
            vetDb.append( m_NoiseCalculation->SPLdB()[j][i] );
            vetDbA.append( m_NoiseCalculation->SPLdBAW()[j][i] );
            vetDbB.append( m_NoiseCalculation->SPLdBBW()[j][i] );
            vetDbC.append( m_NoiseCalculation->SPLdBCW()[j][i] );
        }

        resultOp.append(vetFreq);
        resultOp.append(vetA);
        resultOp.append(vetS);
        resultOp.append(vetP);
        resultOp.append(vetDb);
        resultOp.append(vetDbA);
        resultOp.append(vetDbB);
        resultOp.append(vetDbC);

        m_results.append(resultOp);

    }


}

void NoiseSimulation::exportCalculation(QTextStream &out)
{

    out << "Noise prediction file export" << endl;
    out << endl;
    for (int i = 0; i < m_NoiseCalculation->NoiseParam()->OpPoints().size(); ++i) {
        out << "Alpha: " << m_NoiseCalculation->NoiseParam()->OpPoints()[i]->AlphaDeg() << ", Re = " << m_NoiseCalculation->NoiseParam()->OpPoints()[i]->Reynolds() << endl;
        out << "OASPL: " << m_NoiseCalculation->OASPL()[i] << endl;
        out << endl;
        out << "Freq [Hz]\t\tSPL (dB)\t\tSPL (dB(A))\t\tSPL (dB(B))\t\tSPL (dB(C))" << endl;

        for (int j = 0; j < Noise::FREQUENCY_TABLE_SIZE; ++j) {
            out << Noise::CENTRAL_BAND_FREQUENCY[j]
                   << "\t\t" << m_NoiseCalculation->SPLdB()[i][j]
                   << "\t\t" << m_NoiseCalculation->SPLdBAW()[i][j]
                   << "\t\t" << m_NoiseCalculation->SPLdBBW()[i][j]
                   << "\t\t" << m_NoiseCalculation->SPLdBCW()[i][j]
                   << endl;
        }

        out << endl;
        out << endl;
    }

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
