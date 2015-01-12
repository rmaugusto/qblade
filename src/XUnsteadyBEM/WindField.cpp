#include "WindField.h"

#include <cmath>
#include <ctime>  // For time()
#include <cstdlib>  // For srand() and rand()
#include <limits>
#include <omp.h>
#include <GL/gl.h>
#include <QDebug>

#include "../Serializer.h"
#include "../GlobalFunctions.h"



WindField::WindField ()
	: StorableObject ("< no name >")
{
	
}

WindField::WindField (QString name, double rotorRadius, double hubHeight, double meanWindSpeed,
					  double turbulenceIntensity, bool includeShear, double measurementHeigth,
					  double roughnessLength, double simulationTime, double numberOfTimesteps,
					  double pointsPerSide, bool *cancelCalculation)
	: StorableObject (name)
{
	m_glListIndex = glGenLists(1);
	m_shownTimestep = 0;  // allways shows first timestep first
	m_minValue = std::numeric_limits<float>::max();
	m_maxValue = - std::numeric_limits<float>::max();
	m_isValid = false;
	
	m_rotorRadius = static_cast<float>(rotorRadius);
	m_fieldDimension = m_rotorRadius*2;
	m_hubheight = static_cast<float>(hubHeight);	
	
	m_meanWindSpeed = static_cast<float>(meanWindSpeed);
	m_turbulenceIntensity = static_cast<float>(turbulenceIntensity);
	m_includeShear = includeShear;
	m_roughnessLength = static_cast<float>(roughnessLength);
	m_windSpeedMeasurementHeight = static_cast<float>(measurementHeigth);

	m_simulationTime = static_cast<float>(simulationTime);
	m_numberOfTimesteps = static_cast<int>(numberOfTimesteps);
	m_pointsPerSide = static_cast<int>(pointsPerSide);
	
	m_cancelCalculation = cancelCalculation;
	
	/* m_zyCoordinates */
	const float deltaYZ = m_fieldDimension / (m_pointsPerSide-1);
	//qDebug() << "deltaYZ: " << deltaYZ;
	m_zyCoordinates = new float[m_pointsPerSide];
	for (int i = 0; i < m_pointsPerSide; ++i) {
		m_zyCoordinates[i] = -m_fieldDimension/2 + i*deltaYZ;
		//qDebug() << "yzCoordinates: " << m_zyCoordinates[j];
	}
	
	/* m_zyCoordinatesNormalize */
	m_zyCoordinatesNormalized = new float[m_pointsPerSide];
	for (int i = 0; i < m_pointsPerSide; ++i) {
		m_zyCoordinatesNormalized[i] = m_zyCoordinates[i] * 4 / m_fieldDimension;
		//qDebug() << "m_zyCoordinatesNormalized: " << m_zyCoordinatesNormalized[i];
	}	
	
	/* m_timeAtTimestep */
	const float deltaT = m_simulationTime/(m_numberOfTimesteps-1);
	//qDebug() << "delta t: " << deltaT;
	m_timeAtTimestep = new float[m_numberOfTimesteps];
	for (int i = 0; i < m_numberOfTimesteps; ++i) {
		m_timeAtTimestep[i] = i * deltaT;
		//qDebug () << "Timesteps: " << m_timeAtTimestep[j];
	}

	/* m_meanWindSpeedAtHeigth */
	m_meanWindSpeedAtHeigth = new float[m_pointsPerSide];
	for (int i = 0; i < m_pointsPerSide; ++i) {
		if (m_includeShear) {
			// calculated with log wind profile. Should not be used with heigth above 100m (see wikipedia)
			m_meanWindSpeedAtHeigth[i] = m_meanWindSpeed * 
										 log((m_hubheight+m_zyCoordinates[i]) / m_roughnessLength) /
										 log(m_windSpeedMeasurementHeight / m_roughnessLength);
		} else {
			m_meanWindSpeedAtHeigth[i] = m_meanWindSpeed;
		}
		//qDebug() << "Mean wind speed at " << m_zyCoordinates[j] << ": " << m_meanWindSpeedAtHeigth[j];
	}
	
	/* m_meanWindSpeedAtHub */
	if (m_includeShear) {
		/* Calculated with log wind profile. See my Bachelor Thesis for more information.
		 * For more flexibility the wind profile power law should be implemented as well, so that the user
		 * can choose the appropriate profile for his situation
		 * */
		m_meanWindSpeedAtHub = m_meanWindSpeed * 
							   log((m_hubheight+0) / m_roughnessLength) /
							   log(m_windSpeedMeasurementHeight / m_roughnessLength);
	} else {
		m_meanWindSpeedAtHub = m_meanWindSpeed;
	}
	
	/* m_resultantVelocity */
	m_resultantVelocity = new float**[m_pointsPerSide];
	for (int z = 0; z < m_pointsPerSide; ++z) {
		m_resultantVelocity[z] = new float*[m_pointsPerSide];
		for (int y = 0; y < m_pointsPerSide; ++y) {
			// empty paranthesis initialize the whole new array with 0
			m_resultantVelocity[z][y] = new float[m_numberOfTimesteps] ();
		}
	}
}

WindField::~WindField() {
//	glDeleteLists(m_glListIndex, 1);
	delete [] m_zyCoordinates;
	delete [] m_zyCoordinatesNormalized;
	delete [] m_timeAtTimestep;
	delete [] m_meanWindSpeedAtHeigth;
	
	for (int z = 0; z < m_pointsPerSide; ++z) {
		for (int y = 0; y < m_pointsPerSide; ++y) {
			delete [] m_resultantVelocity[z][y];
		}
		delete [] m_resultantVelocity[z];
	}
	delete [] m_resultantVelocity;
}

void WindField::setShownTimestep(int shownTimestep) {
	if (m_shownTimestep != shownTimestep) {
		m_shownTimestep = shownTimestep;
	}
}

void WindField::render() {
        const int grid = 1234, foot = 12345, field = 123456;

        if (glIsList(foot)) glDeleteLists(foot,1);
        if (glIsList(field)) glDeleteLists(field,1);
        if (glIsList(grid)) glDeleteLists(grid,1);


        int z, y;

        hsv hs;
        hs.s = 1.0;
        hs.v = 1.0;

        glDisable(GL_POLYGON_SMOOTH);  // NM TODO this probabely should not be here but in the module initialisation
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        glNewList(foot, GL_COMPILE);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
        glPolygonOffset(1.0, 0);

		/* ground grid with z = -0.1 to avoid zFighting with WindField */
		glColor3f (0.5, 0.5, 0.5);
		glBegin(GL_LINES);

		for (z = 0; z < m_pointsPerSide; z++) {
			glVertex3f (m_zyCoordinatesNormalized[z], m_zyCoordinatesNormalized[0], -0.1);  // vertical
			glVertex3f (m_zyCoordinatesNormalized[z], m_zyCoordinatesNormalized[m_pointsPerSide-1], -0.1);
			glVertex3f (m_zyCoordinatesNormalized[0], m_zyCoordinatesNormalized[z], -0.1);  // horizontal
			glVertex3f (m_zyCoordinatesNormalized[m_pointsPerSide-1], m_zyCoordinatesNormalized[z], -0.1);
		}
		glEnd();
		
		/* grid foot for signaling floor side of the field */
		glBegin (GL_LINES);
		for (z = -5; z <= 5; ++z) {
			glVertex3f (m_zyCoordinatesNormalized[0], m_zyCoordinatesNormalized[0], z/40.0 - 0.1);
			glVertex3f (m_zyCoordinatesNormalized[m_pointsPerSide-1], m_zyCoordinatesNormalized[0], z/40.0 - 0.1);
		}
        glEnd();
        glEndList();

        glNewList(field, GL_COMPILE);


		/* the windfield */
		const float difference = m_maxValue - m_minValue;
		float normalized;
		for (z = 0; z < m_pointsPerSide - 1; ++z) {
            glBegin(GL_TRIANGLE_STRIP);  // the surface
            glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
            glPolygonOffset(1.0, 0);
            for (y = 0; y < m_pointsPerSide; ++y) {
                /* normalized to [0,1] */
                normalized = (m_resultantVelocity[z][y][m_shownTimestep] - m_minValue) / difference;
                normalized = (m_resultantVelocity[z][y][m_shownTimestep]) / m_maxValue;


                hs.h = (1-normalized)*225;


                glColor3f (hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
                glVertex3f (m_zyCoordinatesNormalized[y], m_zyCoordinatesNormalized[z], 2*normalized);


                normalized = (m_resultantVelocity[z+1][y][m_shownTimestep] - m_minValue) / difference;
                normalized = (m_resultantVelocity[z+1][y][m_shownTimestep]) / m_maxValue;


                hs.h = (1-normalized)*225;

                glColor3f (hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
                glVertex3f (m_zyCoordinatesNormalized[y], m_zyCoordinatesNormalized[z+1], 2*normalized);
            }
            glEnd();
			

        }
        glEndList();

        glNewList(grid, GL_COMPILE);

        for (z = 0; z < m_pointsPerSide - 1; ++z) {

            glColor3f (0, 0, 0);
            glBegin(GL_LINE_STRIP);  // the zigzag lines on the surface
            glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
            glPolygonOffset(-2, -2);
            for (y = 0; y < m_pointsPerSide; ++y) {
                normalized = (m_resultantVelocity[z][y][m_shownTimestep] - m_minValue) / difference;
                normalized = (m_resultantVelocity[z][y][m_shownTimestep]) / m_maxValue;

                glVertex3f (m_zyCoordinatesNormalized[y], m_zyCoordinatesNormalized[z], 2*normalized);
                normalized = (m_resultantVelocity[z+1][y][m_shownTimestep] - m_minValue) / difference;
                normalized = (m_resultantVelocity[z+1][y][m_shownTimestep]) / m_maxValue;

                glVertex3f (m_zyCoordinatesNormalized[y], m_zyCoordinatesNormalized[z+1], 2*normalized);
            }
            glEnd();

        }
		
		glColor3f (0, 0, 0);
		for (z = 0; z < m_pointsPerSide; ++z) {
			glBegin(GL_LINE_STRIP);  // the straigth lines 
            glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
            glPolygonOffset(-2, -2);
			for (y = 0; y < m_pointsPerSide; ++y) {
				/* normalized to [0,1] */
                normalized = (m_resultantVelocity[z][y][m_shownTimestep] - m_minValue) / difference;
                normalized = (m_resultantVelocity[z][y][m_shownTimestep]) / m_maxValue;

                glVertex3f (m_zyCoordinatesNormalized[y], m_zyCoordinatesNormalized[z], 2*normalized);
			}
			glEnd ();
		}
		glEndList();
		
	
    glCallList(foot);
    glCallList(field);
    glCallList(grid);


}

void WindField::exportToBinary(QDataStream &dataStream) {
	dataStream.setByteOrder(QDataStream::LittleEndian);  // AeroDyn expects this
	dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);  // float32
	
	dataStream << qint16(7) <<  // ID: Identifies the file as a TurbSim binary file
				  qint32(m_pointsPerSide) <<  // NumGrid_Z: The number of grid points in the vertical direction
				  qint32(m_pointsPerSide) <<  // NumGrid_Y: The number of grid points in the horizontal direction
				  qint32(0) <<  // n_tower: The number of tower points below the grid (TODO ???)
				  qint32(m_numberOfTimesteps) <<  // n_t: The number of time steps
				  qreal(m_fieldDimension / (m_pointsPerSide-1)) <<  // dz: The distance in meters between two adjacent points in the vertical direction
				  qreal(m_fieldDimension / (m_pointsPerSide-1)) <<  // dy: The distance in meters between two adjacent points in the horizontal direction.
				  qreal(m_simulationTime / (m_numberOfTimesteps-1)) <<  // TimeStep: The time in seconds between consecutive grids
				  qreal(m_meanWindSpeedAtHub) <<  // u_hub: The mean wind speed in m/s at hub height
				  qreal(m_hubheight) <<  // HubHt: The height in meters of the hub
				  qreal(m_hubheight - m_rotorRadius);  // Z_bottom: The height in meters of the bottom of the grid

	/* calculate V_slope and V_intercept
	 * In the binary file the values for velocity are compressed to int16. According to the manuel the formular is:
	 * V_grid = (V_grid_norm - V_intercept) / V_slope
	 * where V_grid is the original value and V_grid_norm is the normalized int16 value.
	 * The TurbSim code (Fortran) looks as follows:
	 * UScl = IntRng/REAL( VMax(1) - VMin(1) , SiKi )
	 * UOff = IntMin - UScl*REAL( VMin(1)    , SiKi )
	 * */
	const float valueRange = m_maxValue - m_minValue;
	const float V_slope = 65535 / valueRange;
	const float V_intercept = -32768 - V_slope*m_minValue;
	dataStream << qreal(V_slope) <<  // V_slope_X
				  qreal(V_intercept) <<  // V-intercept_X
				  qreal(1) <<  // V_slope_Y
				  qreal(0) <<  // V-intercept_Y
				  qreal(1) <<  // V_slope_Z
				  qreal(0);  // V-intercept_Z
	
	/* write the info text */
	QString infoString ("Full-field file for " + m_objectName + ". Generated by QBlade.");
	QByteArray infoByteArray = infoString.toLatin1();
	dataStream << qint32(infoByteArray.size());
	for (int i = 0; i < infoByteArray.size(); i++) {
		dataStream << qint8(infoByteArray[i]);
	}
	
	/* write the velocity values. Only the X-direction is set */
	for (int timestep = 0; timestep < m_numberOfTimesteps; ++timestep) {
		for (int zIndex = 0; zIndex < m_pointsPerSide; ++zIndex) {
			for (int yIndex = 0; yIndex < m_pointsPerSide; ++yIndex) {
				dataStream << qint16(m_resultantVelocity[zIndex][yIndex][timestep] * V_slope + V_intercept) <<
							  qint16(0) <<
							  qint16(0);
			}
		}
	}
}

void WindField::exportToTxt(QTextStream &stream) {
	stream << "Windfield Export File Created with QBlade v0.8 on "<<QDate::currentDate().toString("dd.MM.yyyy") <<
			  " at " << QTime::currentTime().toString("hh:mm:ss") << endl <<
			  "Timesteps: " << m_numberOfTimesteps << endl <<
			  "Temporal Stepwidth: " << m_simulationTime / (m_numberOfTimesteps-1) << " s" << endl <<
			  "Points per Side: " << m_pointsPerSide << endl <<
			  "Spatial Stepwidth: " << m_fieldDimension / (m_pointsPerSide-1) << " m" << endl <<
			  "Hub Height: " << m_hubheight << " m" << endl <<
			  "Mean Wind Speed at Hub: " << m_meanWindSpeedAtHub << " m/s" << endl <<
			  "Heigth of lowest Point: " << m_hubheight - m_rotorRadius << " m" << endl <<
			  endl;
			  
	for (int timestep = 0; timestep < m_numberOfTimesteps; ++timestep) {
		for (int zIndex = 0; zIndex < m_pointsPerSide; ++zIndex) {
			for (int yIndex = 0; yIndex < m_pointsPerSide; ++yIndex) {
				stream << QString("%1 ").arg(m_resultantVelocity[zIndex][yIndex][timestep], 7, 'f', 3);
			}
			stream << endl;
		}
		stream << endl;
	}
}

void WindField::serialize() {
	StorableObject::serialize();
	
	if (g_serializer.isReadMode()) {
		m_glListIndex = glGenLists(1);
	}
	g_serializer.readOrWriteInt (&m_shownTimestep);
	g_serializer.readOrWriteFloat (&m_minValue);
	g_serializer.readOrWriteFloat (&m_maxValue);
	g_serializer.readOrWriteBool (&m_isValid);
	
	/* geometric parameters */
	g_serializer.readOrWriteFloat (&m_rotorRadius);
	g_serializer.readOrWriteFloat (&m_hubheight);
	g_serializer.readOrWriteFloat (&m_fieldDimension);
	
	/* wind parameters */
	g_serializer.readOrWriteFloat (&m_meanWindSpeed);
	g_serializer.readOrWriteFloat (&m_turbulenceIntensity);
	g_serializer.readOrWriteBool (&m_includeShear);
	g_serializer.readOrWriteFloat (&m_roughnessLength);
	g_serializer.readOrWriteFloat (&m_windSpeedMeasurementHeight);
	
	/* calculation parameters */
	g_serializer.readOrWriteFloat (&m_simulationTime);
	g_serializer.readOrWriteInt (&m_numberOfTimesteps);
	g_serializer.readOrWriteInt (&m_pointsPerSide);
		
	/* dynamic calculation results */
	g_serializer.readOrWriteFloatArray1D (&m_zyCoordinates, m_pointsPerSide);
	g_serializer.readOrWriteFloatArray1D (&m_zyCoordinatesNormalized, m_pointsPerSide);
	g_serializer.readOrWriteFloatArray1D (&m_timeAtTimestep, m_numberOfTimesteps);
	g_serializer.readOrWriteFloatArray1D (&m_meanWindSpeedAtHeigth, m_pointsPerSide);
	g_serializer.readOrWriteFloat (&m_meanWindSpeedAtHub);
	g_serializer.readOrWriteFloatArray3D (&m_resultantVelocity, m_pointsPerSide, m_pointsPerSide, m_numberOfTimesteps);
}

void WindField::calculateWindField() {
	/* The turbulent windfield calculation is based on the model of Veers. To understand the following code
	 * it is essential to read the Sandia report "Three-Dimensional Wind Simulation", 1988, or revie
	 * Chapter 14 of "Aerodynamics of Wind Turbines 2nd edition" from M. O.L. Hansen.
	 *
	 * This functions creates a 3 dimensional matrix, where the first 2 dimensions refer to the points in
	 * space, and the third dimension is the time. The spatial distribution of the points is equidistant and
	 * square, expressed in a cartesian coordinate system with it's point of origin in the middle of the
	 * windfield.
	 * The windfield is divided into jxk (j=k) points in space. The points are numbered like this (view from
	 * the front to the turbine):
	 *   7 8 9
	 *   4 5 6
	 *   1 2 3
	 * The coordinate (so z is the heigth):
	 *  z ^
	 *    |
	 *    | --> y
	 * */
	
	int j;  // j and k refer both to all discrete points of the windfield
	int k;
	int m;  // refers to all the frequencies
	float sum;  // used for several summations
	
	const int pointsInTotal = m_pointsPerSide * m_pointsPerSide;  // total number of points in windflied
	const int numberOfFrequencies = m_numberOfTimesteps / 2;  // see Hansen p. 152
	const float deltaF = 1 / m_simulationTime;  // frequency steps in [Hz]
		
	float* frequency;  // contains all calculated N/2 frequencies
	
	/* Matrix H: According to the number of points this Matrix can get very big. There shouldn't be more
	 * than around 100 points in one direction, as the size of this matrix is 100^2 x 100^2 ( /2, as the
	 * matrix is lower triangular).
	 * The Matrix H represents the weighted factors for the linear combination of N independant,
	 * unit-magnitude, white noise inputs, that will yield in N correlated outputs with the correct spectral
	 * matrix. Each row of H gives the contributions of all the inputs to the outputpoint at k.
	 * Each column gives the contributions of the j^(th) input to all of the outputs
	 * */
	float **H;
	
	/* Matrix S is the spectral matrix. The diagonals of S are the PSDs. Each off-diagonal term S_jk is
	 * the cross spectral density between points j and k.
	 * To save memory there is only need for the k inputs (columns) for each j (row). Therefore S_j is
	 * only a 1D Vector foreach current j. Additionally there are the diagonal terms needed
	 * */
	float *S_j  = NULL;
	float *S_diagonal = NULL;
	
	/* Check Hansen for additional information */
	float **amplitude = NULL;
	float **phi = NULL;
	
	/* Random matrix */
	float **random = NULL;
	srand (time(NULL));
	//srand (0);  // for testing: gives always the same sequence of random numbers
	
	/* In Veers a factor deltaF/2 is used to normalize the PSD, while in Hansen the integral of
	 * the PSD over all calculated frequencys must be one. In general Hansens method should be
	 * more accurate but at all test deltaF/2 made the resultant turbulense intensity to fit better
	 * with the value in the setup
	 * */
	float *psdNormalizationFactor = NULL;
	
	
	/* * * * * * allocate thread shared memory * * * * * */
	frequency = new float[numberOfFrequencies];
	for (m = 0; m < numberOfFrequencies; ++m) {
		frequency[m] = (m+1) / m_simulationTime;
		//qDebug() << "Frequencies: " << frequency[m];
	}
	
	amplitude = new float*[pointsInTotal];
	for (j = 0; j < pointsInTotal; ++j) {
		amplitude[j] = new float[numberOfFrequencies] ();
	}
	
	phi = new float*[pointsInTotal];
	for (j = 0; j < pointsInTotal; ++j) {
		phi[j] = new float[numberOfFrequencies] ();
	}
	
	random = new float*[pointsInTotal];
	for (j = 0; j < pointsInTotal; ++j) {
		random[j] = new float[numberOfFrequencies];
		for (m = 0; m < numberOfFrequencies; ++m) {
			random[j][m] = (rand()*1.0)/RAND_MAX * 2 * M_PI;  // values from 0 to 2*Pi
		}
	}
	
	psdNormalizationFactor = new float[m_pointsPerSide];
	for (int zIndex = 0; zIndex < m_pointsPerSide; ++zIndex) {
//		sum = 0;
//		for (m = 0; m < numberOfFrequencies; ++m) {
//			sum += getPSD(frequency[m], zIndex);
//		}
//		psdNormalizationFactor[zIndex] = 1 / sum;
		psdNormalizationFactor[zIndex] = deltaF / 2;
//		qDebug() << "deltaF/2: " << deltaF/2 << " factor: " << psdNormalizationFactor[zIndex];
	}
	
		
    #pragma omp parallel default (none) \
                private (j, k, sum, S_j, S_diagonal, H) \
                shared (frequency, random, amplitude, phi, psdNormalizationFactor)
    {
		/* * * * * * allocate thread private memory * * * * * */
		int j_y;  // the column in the windfield of point j
		int j_z;  // the row (or height) in the windfield of point j
		int k_y;
		int k_z;
		int l;  // refers to the column count within the H matrix
		float Re;
		float Im;
		
		H = NULL;
		try {  // allocation of especially matrix H consumes a lot of memory
			S_j = new float[pointsInTotal] ();
			S_diagonal = new float[pointsInTotal] ();
			
			H = new float*[pointsInTotal];
			for (j = 0; j < pointsInTotal; ++j) {
				H[j] = new float[j+1] ();  // lower triangular matrix
			}
		} catch (std::bad_alloc&) {  // will probably never happen
			emit badAlloc();
		}
		
		
		/* * * * * * calculation * * * * * */
        #pragma omp for
        for (m = 0; m < numberOfFrequencies; ++m)  // independent loop, therefore parallelized
		{
			j_z = 0; j_y = 0;
			for (j = 0; j < pointsInTotal && ! *m_cancelCalculation; ++j) {
				if (j_y == 0) {  // calculate new PSD only at new height
					S_diagonal[j] = getPSD(frequency[m], j_z) * psdNormalizationFactor[j_z];
				} else {  // otherwise take the last one, because it's the same
					S_diagonal[j] = S_diagonal[j-1];
				}
				
				k_z = 0; k_y = 0;
				Re = 0; Im = 0;
				for (k = 0; k <= j; ++k) {
					if (j != k) {  // calculate S_jk
						S_j[k] = getCoh (frequency[m], getDist(m_zyCoordinates[j_y], m_zyCoordinates[j_z],
															   m_zyCoordinates[k_y], m_zyCoordinates[k_z] )
																	) * sqrt(S_diagonal[j] * S_diagonal[k]);					
					} else {  // calculate S_kk
						S_j[k] = S_diagonal[k];
					}

					sum = 0;
					if (j != k) {  // calculate H_jk
						for (l = 0; l <= k-1; ++l) {
							sum = H[j][l]*H[k][l] + sum;
						}
						H[j][k] = (S_j[k] - sum) / H[k][k];
					} else {  // calculate H_kk
						for (l = 0; l <= k-1; ++l) {
							sum = pow(H[k][l],2) + sum;
						}
						H[k][k] = sqrt(S_j[k] - sum);
					}
					
					Re = Re + (H[j][k]*cos(random[k][m]));
					Im = Im + (H[j][k]*sin(random[k][m]));
					
					++k_y;
					if (k_y == m_pointsPerSide) {
						k_y = 0;
						++k_z;
					}
				}  // for k
				
				amplitude[j][m] = sqrt (pow(Re,2) + pow(Im,2));
				phi[j][m] = atan2 (Im, Re);
				
				emit updateProgress();						
				++j_y;
				if (j_y == m_pointsPerSide) {
					j_y = 0;
					++j_z;
				}
			}  // for j
		}  // for m
		
		/* * * * * * free thread private memory * * * * * */
		delete [] S_j;
		delete [] S_diagonal;
		for (j = 0; j < pointsInTotal; ++j) {
			delete [] H[j];
		}
		delete [] H;
    }  // omp parallel END

	
	/* * * * * * superposition of frequencies * * * * * */
	if (! *m_cancelCalculation)
	{
		int z = 0;  // the row (or height) in the windfield
		int y = 0;  // the column in the windfield
		int t = 0;  // refers to the timesteps
		for (j = 0; j < pointsInTotal && ! *m_cancelCalculation; ++j)  // for every point j
		{
			for (t = 0; t < m_numberOfTimesteps; ++t)  // for every timestep t
			{
				sum = 0;
				for (m = 0; m < numberOfFrequencies; ++m)  // for every frequency m
				{
					sum = sum + (2*amplitude[j][m]*cos(2*M_PI*frequency[m]*m_timeAtTimestep[t]-phi[j][m]));
				}
				m_resultantVelocity[z][y][t] = m_meanWindSpeedAtHeigth[z] + sum;  // store final result
				
				/* find the max and min value for the windfield */
				if (m_resultantVelocity[z][y][t] < m_minValue) {
					m_minValue = m_resultantVelocity[z][y][t];
				}
				if (m_resultantVelocity[z][y][t] > m_maxValue) {
					m_maxValue = m_resultantVelocity[z][y][t];
				}
				emit updateProgress();						
			}

			++y;
			if (y == m_pointsPerSide) {
				y = 0;
				++z;
			}
		}
	}
	
	
	/* * * * * * free thread shared memory * * * * * */
	delete [] frequency;
	delete [] psdNormalizationFactor;
	
	for (j = 0; j < pointsInTotal; ++j) {
		delete [] amplitude[j];
	}
	delete [] amplitude;
	
	for (j = 0; j < pointsInTotal; ++j) {
		delete [] phi[j];
	}
	delete [] phi;
	
	for (j = 0; j < pointsInTotal; ++j) {
		delete [] random[j];
	}
	delete [] random;
	
	
	/* * * * * * finish calculation * * * * * */
	if (! *m_cancelCalculation) {
		m_isValid = true;
		emit updateProgress();  // emits last update signal. Calculation is finished
	}
}



CVector WindField::getWindspeed(CVector vec, double time){
    double z,y;
    z = vec.z;
    y = vec.y;
    z += m_fieldDimension/2;
    y += m_fieldDimension/2;

    //here the windfield is marched trhough the domain with the hub height wind speed. initially it is marched through the domain for half a rotor diameter

    time += m_fieldDimension/2/m_meanWindSpeedAtHub - vec.x / m_meanWindSpeedAtHub;



    if (fabs(z) > m_fieldDimension || fabs(y) > m_fieldDimension || time > m_simulationTime || z < 0 || y < 0){
//        qDebug() << "::::::::::requested Velocity out of Windfield bounds::::::::: returned V_MeanAtHub";
        return CVector(m_meanWindSpeedAtHub,0,0);
    }
    else{

        // start interpolation
        double spatialwidth = m_fieldDimension / (m_pointsPerSide-1);
        double temporalwidth = m_simulationTime / (m_numberOfTimesteps-1);

        int zindex = floor(z / spatialwidth);
        int yindex = floor(y / spatialwidth);
        int tindex = floor(time / temporalwidth);

        double mZYfloorTfloor = m_resultantVelocity[zindex][yindex][tindex]+(m_resultantVelocity[zindex+1][yindex][tindex]-m_resultantVelocity[zindex][yindex][tindex])*(z-spatialwidth*zindex)/spatialwidth;
        double mZYceiltTfloor = m_resultantVelocity[zindex][yindex+1][tindex]+(m_resultantVelocity[zindex+1][yindex+1][tindex]-m_resultantVelocity[zindex][yindex+1][tindex])*(z-spatialwidth*zindex)/spatialwidth;
        double meanTfloor = mZYfloorTfloor+(mZYceiltTfloor-mZYfloorTfloor)*(y-spatialwidth*yindex)/spatialwidth;

        double mZYfloorTceil = m_resultantVelocity[zindex][yindex][tindex+1]+(m_resultantVelocity[zindex+1][yindex][tindex+1]-m_resultantVelocity[zindex][yindex][tindex+1])*(z-spatialwidth*zindex)/spatialwidth;
        double mZYceiltTceil = m_resultantVelocity[zindex][yindex+1][tindex+1]+(m_resultantVelocity[zindex+1][yindex+1][tindex+1]-m_resultantVelocity[zindex][yindex+1][tindex+1])*(z-spatialwidth*zindex)/spatialwidth;
        double meanTceil = mZYfloorTceil+(mZYceiltTceil-mZYfloorTceil)*(y-spatialwidth*yindex)/spatialwidth;

        double Vint = meanTfloor+(meanTceil-meanTfloor)*(time-temporalwidth*tindex)/temporalwidth;

        return CVector(Vint,0,0);
    }

}


float WindField::getDist (const float y1, const float z1, const float y2, const float z2)
{
	const float dy = (y1-y2);
	const float dz = (z1-z2);
	
	if (dy != 0 && dz != 0) {
		return sqrt(dy*dy + dz*dz);
	} else if (dz != 0) {  // therefore dy must be 0
		return dz;
	} else if (dy != 0) {
		return dy;
	} else {
		return 0;
	}
}

float WindField::getCoh(const float frequency, const float spatialDistance)
{
	/* Coherence function according to Frost.
	 * The coherence decrement C is set to 12 as Hansen suggested.
	 * */
	return exp(-12 * frequency * spatialDistance / m_meanWindSpeed);  // meanWindSpeed not depending on height?
}

float WindField::getPSD (const float frequency, const int zIndexOfPoint)
{
	const int heigthAboveGround = m_hubheight + m_zyCoordinates[zIndexOfPoint];
	float l;
	if (heigthAboveGround <= 30) {
		l = heigthAboveGround*20;
	} else {
		l = 600;
	}
	
	/* PSD function after Kaimal as in Hansen p. 149 */
	const float turbulenceDecimal = m_turbulenceIntensity / 100;
	return ( turbulenceDecimal*turbulenceDecimal * m_meanWindSpeedAtHeigth[zIndexOfPoint] * l ) /
			( pow((1 + frequency * 1.5 * l / m_meanWindSpeedAtHeigth[zIndexOfPoint]), (5.0/3)) );
}

WindField* WindField::newBySerialize() {
	WindField* windfield = new WindField ();
	windfield->serialize();
	return windfield;
}
