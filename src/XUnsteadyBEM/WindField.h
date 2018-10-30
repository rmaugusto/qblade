/****************************************************************************

	Additional Globals for QBlade
		Copyright (C) 2010 Guido Weinzierl Smart Blade GmbH
		Copyright (C) 2012 Nikolai Moesus

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef WINDFIELD_H
#define WINDFIELD_H

#include <QTextStream>
#include "../StorableObject.h"
#include "../Objects/CVectorf.h"
#include "../Objects/CVector.h"
#include "../ParameterObject.h"
#include "../ParameterKeys.h"
template <class ParameterGroup>
class ParameterViewer;


class WindField : public StorableObject, public ParameterObject<Parameter::Windfield>
{
	Q_OBJECT
	
public:
	static WindField* newBySerialize ();
	static WindField* newByImport (QDataStream &dataStream);
	WindField (ParameterViewer<Parameter::Windfield> *viewer, bool *cancelCalculation);

	~WindField();
	static QStringList prepareMissingObjectMessage();
	
	float getRotorRadius () { return m_rotorRadius; }
	float getHubheight () { return m_hubheight; }
	float getMeanWindSpeed () { return m_meanWindSpeed; }
    float getMeanWindSpeedAtHub () { return m_meanWindSpeedAtHub; }
	float getTurbulenceIntensity () { return m_turbulenceIntensity; }
	bool getIncludeShear () { return m_includeShear; }
	float getRoughnessLength () { return m_roughnessLength; }
	float getWindSpeedMeasurementHeight () { return m_windSpeedMeasurementHeight; }
	float getSimulationTime () { return m_simulationTime; }
	int getNumberOfTimesteps () { return m_numberOfTimesteps; }
	float getLengthOfTimestep () { return m_simulationTime / (m_numberOfTimesteps-1); }
	int getPointsPerSide () { return m_pointsPerSide; }
    float getFieldDimensions () { return m_fieldDimension; }

    CVector getWindspeed(CVector vec, double time, double overhang = 0);
	
	void setShownTimestep (int shownTimestep) { m_shownTimestep = shownTimestep; }
	int getShownTimestep () { return m_shownTimestep; }
	float minValue () { return m_minValueX; }
	float maxValue () { return m_maxValueX; }
	bool isValid () { return m_isValid; }
	
	void calculateWindField();  // core method; calculates a valid windfield
	void render ();  // renders WindField with OpenGL
    void renderForQLLTSim(double time, double dist, double mean, bool redblue);
	void exportToBinary (QDataStream &dataStream);  // exports as "FF TurbSim Binary File Grid Format"
    void importFromBinary(QDataStream &dataStream);  // imports from "FF TurbSim Binary File Grid Format"
	void exportToTxt (QTextStream &stream);
	void serialize();  // override of StorableObject
	
private:
	WindField ();
	
	QVariant accessParameter(Parameter::Windfield::Key key, QVariant value = QVariant());
	float getDist(const float x1, const float y1, const float x2, const float y2);
	float getCoh(const float frequency, const float spatialDistance);
	float getPSD(const float frequency, const int zIndexOfPoint);
	
	/* control variables */
	int m_glListIndex;  // openGL manages global lists. This list belongs to this windfield
	int m_shownTimestep;  // which timestep will be rendered
    float m_minValueX;  // the minimal velocity value that occures in the windfield over the whole time (X)
    float m_maxValueX;  // the maximal value ~ (X)

    float m_minValueY;  // the minimal velocity value that occures in the windfield over the whole time (X)
    float m_maxValueY;  // the maximal value ~ (X)

    float m_minValueZ;  // the minimal velocity value that occures in the windfield over the whole time (X)
    float m_maxValueZ;  // the maximal value ~ (X)

	bool m_isValid;  // set true, as soon as calculation is succsessfully finished
	bool *m_cancelCalculation;  // a pointer to a bool that reacts on the progress dialog cancelation
	
	/* geometric parameters */
	float m_rotorRadius;  // radius of the rotor [m]
	float m_hubheight;  // hub height, shaft height of rotor [m]
	float m_fieldDimension;  // the windField is a square of dimension x dimension [m]
	
	/* wind parameters */
	float m_meanWindSpeed;  // mean wind speed at reference heigth
	float m_turbulenceIntensity;  // turbulence intesity [% (0 to 100)]
	bool m_includeShear;  // if the shear layer is considered
	float m_roughnessLength;  // surface roughness
	float m_windSpeedMeasurementHeight;  // the heigth where the wind speed was measured
	
	/* calculation parameters */
	float m_simulationTime;  // simulation time in seconds
	int m_numberOfTimesteps;  // the windField is calculated for various timesteps (unsteady)
	int m_pointsPerSide;  // number of points in y and z direction (same value due to square shape)
		
	/* dynamic calculation results */
	float* m_zyCoordinates;  // vector containg y and z values of windfield. As the windfield is square and
							 // equidistant the occuring y and z values [m] for each discrete point are the same
	float* m_zyCoordinatesNormalized;  // to [-2,2] normalized coordinates for the plot
	float* m_timeAtTimestep;  // the discrete resolution of time
	float* m_meanWindSpeedAtHeigth;  // the windspeed for each point in heigth
	float m_meanWindSpeedAtHub;  // ~ for the hub heigth that might not be covered by a grid point
    CVectorf*** m_resultantVelocity;  // 3D array of velocity values at yz-position. Indizes: [z][y][time]
	
signals:
	void updateProgress ();  // emited to increase progress dialog	
};

#endif // WINDFIELD_H
