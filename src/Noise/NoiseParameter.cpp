#include "NoiseParameter.h"

#include <cmath>

#include "../Objects/OpPoint.h"
#include "../Serializer.h"
#include "NoiseOpPoint.h"


NoiseParameter::NoiseParameter()
	: highFreq (true),
	  lowFreq (true)
{
}

void NoiseParameter::serialize() {
	g_serializer.readOrWriteEnum(&opPointSource);
	g_serializer.readOrWriteStorableObjectVector(&analyzedOpPoints);
	
	g_serializer.readOrWriteDouble(&wettedLength);
	g_serializer.readOrWriteDouble(&distanceObsever);
	g_serializer.readOrWriteDouble(&directivityGreek);
	g_serializer.readOrWriteDouble(&directivityPhi);
	g_serializer.readOrWriteBool(&highFreq);
	g_serializer.readOrWriteBool(&lowFreq);
	g_serializer.readOrWriteBool(&suctionSide);
	g_serializer.readOrWriteBool(&pressureSide);
	g_serializer.readOrWriteBool(&separatedFlow);

	g_serializer.readOrWriteDouble(&dStarChordStation);
	g_serializer.readOrWriteDouble(&dStarScalingFactor);
	g_serializer.readOrWriteDouble(&eddyConvectionMach);
	g_serializer.readOrWriteDouble(&originalMach);
	g_serializer.readOrWriteDouble(&originalChordLength);
	g_serializer.readOrWriteDouble(&originalVelocity);
	
	g_serializer.readOrWriteDouble(&aoa);
	g_serializer.readOrWriteDouble(&chordBasedReynolds);
	g_serializer.readOrWriteEnum(&transition);
}

void NoiseParameter::restorePointers() {
	for (int i = 0; i < analyzedOpPoints.size(); ++i) {
		g_serializer.restorePointer(reinterpret_cast<StorableObject**> (&(analyzedOpPoints[i])));
	}
}

QList<NoiseOpPoint*> NoiseParameter::prepareNoiseOpPointList() {
	QList<NoiseOpPoint*> noiseOpPoints;
	if (opPointSource == OnePolar || opPointSource == MultiplePolars) {
		foreach (OpPoint *opPoint, analyzedOpPoints) {
			noiseOpPoints.append(new NoiseOpPoint(opPoint));
		}
	} else if (opPointSource == OriginalBpm) {
		noiseOpPoints.append(new NoiseOpPoint(chordBasedReynolds, aoa));
	}
	return noiseOpPoints;
}
