#ifndef NOISESIMULATION_H
#define NOISESIMULATION_H

#include <QTextStream>

#include "../Objects/CVector.h"
#include "noisemodule.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../StorableObject.h"
#include "noisecalculation.h"

class NoiseModule;

class NoiseSimulation : public StorableObject, public ShowAsGraphInterface
{
    Q_OBJECT

private:
    QStringList m_availableVariables;
    QVector< QVector < QVector<double> > >  m_results;
    NoiseCalculation * m_NoiseCalculation;

public:
    explicit NoiseSimulation();
    ~NoiseSimulation();

    void serialize ();
    static NoiseSimulation* newBySerialize ();
    void restorePointers ();
    /**
     * @brief simulate Read all NoiseCalculation data and store the result to plot the graph
     */
    void simulate();
    void exportCalculation(QTextStream &out);

    NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType, int opPointIdx);  // returns NULL if var not available
    NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // returns NULL if var not available
    QStringList getAvailableVariables (NewGraph::GraphType graphType);
    QString getObjectName () { return m_objectName; }

    NoiseCalculation *Calculation() const;
    void setCalculation(NoiseCalculation *NoiseCalculation);

    double getPositiveNumber(double number);
signals:
    void updateProgress ();  // emited to increase progress dialog

public slots:

};

#endif // NOISESIMULATION_H
