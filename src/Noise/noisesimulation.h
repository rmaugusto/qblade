#ifndef NOISESIMULATION_H
#define NOISESIMULATION_H

#include "../Objects/CVector.h"
#include "NoiseModule.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../StorableObject.h"

class NoiseModule;

class NoiseSimulation : public StorableObject, public ShowAsGraphInterface
{
    Q_OBJECT

private:
    QStringList m_availableVariables;
    QVector< QVector <double> >  m_results;

public:
    explicit NoiseSimulation();
    ~NoiseSimulation();

    void serialize ();
    static NoiseSimulation* newBySerialize ();
    void restorePointers ();

    NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // returns NULL if var not available
    QStringList getAvailableVariables (NewGraph::GraphType graphType);
    QString getObjectName () { return m_objectName; }

signals:
    void updateProgress ();  // emited to increase progress dialog

public slots:

};

#endif // NOISESIMULATION_H
