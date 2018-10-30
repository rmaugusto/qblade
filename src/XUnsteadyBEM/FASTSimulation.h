#ifndef FASTSIMULATION_H
#define FASTSIMULATION_H

#include <QBitArray>
#include <QStringList>
class QDir;

#include "../StorableObject.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../Graph/NewGraph.h"
#include "FASTResult.h"
class WindField;
class CBlade;
class TData;
class BladeStructure;
class NewCurve;


class FASTSimulation : public StorableObject, public ShowAsGraphInterface
{
public:
	enum FASTValue {BEDDOES, STEADY, USE_CM, NO_CM, DYNIN, EQUIL, NONE, WAKE, SWIRL, PRANDTL, GTECH};
	static const QString valueString[];
	static const QString fastRotorOutputParameters[];
	static const QString fastBladeOutputParameters[];
	static const int numberOfFastRotorOutputParameters;
	static const int numberOfFastBladeOutputParameters;
    static const QString fastRotorOutputParametersTooltips[];
    static const QString fastBladeOutputParametersTooltips[];
	
	void serialize();
	void restorePointers();
	static FASTSimulation* newBySerialize ();
    FASTSimulation(QString name,
                   WindField *usedWindField,
                   double totalRunTime,
                   double timeStep,
                   CBlade *usedRotor,
                   int numberOfBlades,
                   double rotorSpeed,
                   double nacelleYaw,
                   double gravity,
                   double airDens,
                   double kinVisc,
                   FASTValue stallMod,
                   FASTValue useCm,
                   FASTValue infModel,
                   FASTValue indModel,
                   FASTValue tlModel,
                   FASTValue hlModel,
                   BladeStructure *usedBladeStructure,
                   bool useFlapDof1,
                   bool useFlapDof2,
                   bool useEdgeDof,
                   QVector<int> *strainGages,
                   QBitArray *aeroDynOutput,
                   double tStart,
                   int decFact,
                   QBitArray *rotorParameters,
                   QVector<int> *bladeOutput,
                   QBitArray *bladeParameters,
                   double aeroTimeStep,
                   QString windfieldFile,
                   bool bFromQBlade,
                   double hubheight);
	~FASTSimulation ();
	static QStringList prepareMissingObjectMessage();
	void writeAllFiles(QDir &simulationDirectory);  // can throw exeption QString errorMessage
	void readOutputFile(QString fileToRead);  // can throw exeption QString errorMessage; call only once!
	NewCurve* newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // override of ShowAsGraphInterface
	QStringList getAvailableVariables (NewGraph::GraphType graphType);
	QString getObjectName() { return m_objectName; }
	
	
	WindField* getUsedWindField () { return m_usedWindField; }
	double getTotalRunTime () { return m_totalRunTime; }
    double getHubHeight () { return m_hubHeight; }
    double getTimeStep () { return m_timeStep; }
    double getAeroTimeStep () { return m_aeroTimeStep; }
    CBlade* getUsedRotor () { return m_usedRotor; }
    int getNumberOfBlades () { return m_numberOfBlades; }
    double getRotorSpeed () { return m_rotorSpeed; }
    double getNacelleYaw () { return m_nacelleYaw; }
    double getGravity () { return m_gravity; }
    double getAirDens () { return m_airDens; }
    double getKinVisc () { return m_kinVisc; }
    FASTValue getStallMod () { return m_stallMod; }
    FASTValue getUseCm () { return m_useCm; }
    FASTValue getInfModel () { return m_infModel; }
    FASTValue getIndModel () { return m_indModel; }
    FASTValue getTlModel () { return m_tlModel; }
    FASTValue getHlModel () { return m_hlModel; }
    BladeStructure* getUsedBladeStructure () { return m_usedBladeStructure; }
    bool getUseFlapDof1 () { return m_useFlapDof1; }
    bool getUseFlapDof2 () { return m_useFlapDof2; }
    bool getUseEdgeDof () { return m_useEdgeDof; }
	QVector<int> getStrainGages () { return m_strainGages; }
	QVector<int> getBladeOutput () { return m_bladeOutput; }
	QBitArray getAeroDynOutput () { return m_aeroDynOutput; }
	QBitArray getRotorParameters () { return m_rotorParameters; }
	QBitArray getBladeParameters () { return m_bladeParameters; }
	double getTStart () { return m_tStart; }
	int getDecFact () { return m_decFact; }
	bool hasFastResults () { return !m_fastResults.isEmpty(); }
	bool hasAeroDynResults () { return !m_aeroDynResults.isEmpty(); }
	FASTResult* getTimeData () { return (hasAeroDynResults()?m_aeroDynResults[0]:NULL); }
	
	int getShownBladeSection () { return m_shownBladeSection; }
	void setShownBladeSection (int shownBladeSection) { m_shownBladeSection = shownBladeSection; }
	int getShownTimeIndex () { return m_shownTimeIndex; }
	void setShownTimeIndex (int shownTimeIndex) { m_shownTimeIndex = shownTimeIndex; }
	float getShownTime () { return m_aeroDynResults[0]->data[m_shownTimeIndex]; }
	void setShownTime (float shownTime);  // changes timeIndex only if shownTime is available
    QString getWindFieldPath(){ return m_WindFieldPathName; }
    bool getIsWindFromQBlade(){ return m_bWindFromQBlade; }
	
private:
	FASTSimulation ();
	
	WindField *m_usedWindField;
	CBlade *m_usedRotor;
	BladeStructure *m_usedBladeStructure;
	FASTValue m_stallMod, m_useCm, m_infModel, m_indModel, m_tlModel, m_hlModel;
    double m_totalRunTime, m_timeStep, m_rotorSpeed, m_nacelleYaw, m_gravity, m_airDens, m_kinVisc, m_tStart, m_aeroTimeStep, m_hubHeight;
	int m_numberOfBlades, m_decFact;
	bool m_useFlapDof1, m_useFlapDof2, m_useEdgeDof;
	QVector<int> m_strainGages, m_bladeOutput;
	QBitArray m_aeroDynOutput, m_rotorParameters, m_bladeParameters;
	QVector<FASTResult*> m_fastResults, m_aeroDynResults;
	QStringList m_availableVariablesFastGraph, m_availableVariablesBladeGraph;
    bool m_bWindFromQBlade;
    QString m_WindFieldPathName;
	
	int m_shownBladeSection;  // which blade section is shown in the graphs
	int m_shownTimeIndex;  // which index in the time data is shown in the graph
};

#endif // FASTSIMULATION_H
