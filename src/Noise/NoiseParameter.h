#ifndef NOISEPARAMETER_H
#define NOISEPARAMETER_H

#include <QList>
#include <QVector>

class OpPoint;
class NoiseOpPoint;


/* This class holds all parameters for a 2D noise simulation. Its only purpose is to reduce the amount of variables
 * in the NoiseSimulation class and keep its namespace clean.
 * */

class NoiseParameter
{
public:
	enum OpPointSource {OnePolar, MultiplePolars, OriginalBpm};
	enum TransitionType {FullyTurbulent, TransitionFlow};

    NoiseParameter();
	void serialize ();
	void restorePointers ();

	QList<NoiseOpPoint*> prepareNoiseOpPointList();  // transfers ownership to caller, don't forget to delete the list

	OpPointSource opPointSource;  // where the analyzed OpPoints originate from
	QVector<OpPoint*> analyzedOpPoints;  // list of the analyzed oppoints
		
	double wettedLength;
	double distanceObsever;
	double directivityGreek;
	double directivityPhi;
	bool highFreq;  // NM the two frequency members are not included in the GUI
	bool lowFreq;
	bool suctionSide; //SPLs
	bool pressureSide; //SPLp
	bool separatedFlow; //SPLa
	
	//XFoil correlation
	double dStarChordStation;
	double dStarScalingFactor;
	double eddyConvectionMach;
	double originalMach;
	double originalChordLength;
	double originalVelocity;
	
	//Original BPM correlations
	double aoa;
	double chordBasedReynolds;
	TransitionType transition;
};

#endif // NOISEPARAMETER_H
