#ifndef BLADESTRUCTURELOADING_H
#define BLADESTRUCTURELOADING_H

#include <QString>
#include <QVector>
#include <QObject>
#include "../StorableObject.h"
#include "BladeStructure.h"

class BladeStructureLoading : public StorableObject
{
    Q_OBJECT

signals:
    void updateProgress ();  // emited to increase progress dialog

public:
    static BladeStructureLoading* newBySerialize ();

    BladeStructureLoading();
    BladeStructureLoading(QString name, BladeStructure *structure);
    void Duplicate(BladeStructureLoading *pLoading);

    bool *m_cancelCalculation;

    ~BladeStructureLoading();
    void serialize();        // override from StorableObject
    void restorePointers();  // override from StorableObject
	static QStringList prepareMissingObjectMessage();	
    bool RunModalTest();

    bool simulationFinished;

    BladeStructure *m_structure;

    QVector<double> sectionWidth;
    QVector<double> normalLoading;
    QVector<double> tangentialLoading;
    QVector< QVector <double> > nodeTranslations;
    QVector< QVector <double> > VMStresses;

};

#endif // BLADESTRUCTURELOADING_H
