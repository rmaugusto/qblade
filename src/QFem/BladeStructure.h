#ifndef BLADESTRUCTURE_H
#define BLADESTRUCTURE_H

#include <QString>
#include <QVector>
#include <QObject>

#include "../StorableObject.h"
#include "../XBEM/Blade.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../Graph/NewGraph.h"
#include "BladeStructureLoading.h"
class NewCurve;


class BladeStructure : public StorableObject, public ShowAsGraphInterface
{
    Q_OBJECT

signals:
    void updateProgress ();  // emited to increase progress dialog

public:
	static BladeStructure* newBySerialize ();

    void addQVecToResults(BladeStructureLoading *loading = NULL);
    void FillVariableList(BladeStructureLoading *loading = NULL);

    NewCurve* newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType);
    QStringList getAvailableVariables(NewGraph::GraphType graphType);
	QString getObjectName() { return m_objectName; }

	BladeStructure ();
	BladeStructure(QString name, CBlade *rotor);
	~BladeStructure();
    void serialize();        // override from StorableObject
    void restorePointers();  // override from StorableObject
	static QStringList prepareMissingObjectMessage();	

	void Duplicate(BladeStructure *pStructure);
	void ReadSectionCoordinates();
    bool RunModalTest();



	CBlade *m_rotor;

    double ShellEMod; /**< List of shell material E-Modulus at the blade stations */
    double ShellRho; /**< List of shell material density at the blade stations */
    double blademass;
    double SparEMod; /**< List of spar material E-Modulus at the blade stations */
    double SparRho; /**< List of spar material density at the blade stations */
    double Omega; /**< Rotational speed at the design point to account for rotational stiffening effect */

    double xFoilCoords[100][200], yFoilCoords[100][200];

    int StructType; /**< =0 hollow shell with spar; =1 hollow shell; =2 solid section, no spar */
    int m_numElems;
    int m_numFoilPoints;

    bool QFEMCompleted;
    bool AbsoluteShell;
    bool AbsoluteSpar;

    QColor sparColor, shellColor;

	//QVectors with structural and material properties at the blade stations

	QVector<double> EIx;/**< Moment of stiffness inertia x direction(reference coordinate system)*/
	QVector<double> EIy;/**< Moment of stiffness inertia y direction(reference coordinate system)*/
	QVector<double> EI1;/**< Moment of stiffness inertia about the first principle axis*/
	QVector<double> EI2; /**< Moment of stiffness inertia about the second principle axis*/
	QVector<double> EA; /**< Axial Stiffness*/
	QVector<double> RhoA; /**< Linear Density, ie how many kg per m of blade structure */
	QVector<double> GJ; /**< Polar Moment of Stiffness inertia */
	QVector<double> StructuralTwist;/**< Angle between the reference and the elastic axis, needed as an input for fast */
	QVector<double> ShellThickness; /**< List of shell thickness in % of chord at the blade stations */
	QVector<double> SparLocation; /**< List of spar locations at the blade stations */
	QVector<double> SparThickness; /**< List of spar thickness in % of chord at the blade stations */
	QVector<double> SparAngle; /**< List of spar angles at the blade stations */

	//QVectors of the modal analysis results

	QVector<double> FlapwiseFrequencies; /**< Eigenfrequencies of the blades mode shapes, sorted from low to high frequencies */
	QVector<double> EdgewiseFrequencies; /**< Eigenfrequencies of the blades mode shapes, sorted from low to high frequencies */
	QVector<double> TorsionalFrequencies; /**< Eigenfrequencies of the blades mode shapes, sorted from low to high frequencies */
	QVector<double> RadialFrequencies; /**< Eigenfrequencies of the blades mode shapes, sorted from low to high frequencies */
	QVector<double> UnsortedFrequencies; /**< Eigenfrequencies of the blades mode shapes, sorted from low to high frequencies */
    QVector<double> AscendingFrequencies; /**< Eigenfrequencies in ascending Order */

    QVector< QVector <double> > FlapwiseCoefficients; /**< Polynomial coefficients of the mode shapes in the order of  x^2, x^3, x^4, x^5 ,x^6 */
    QVector< QVector <double> > EdgewiseCoefficients; /**< Polynomial coefficients of the mode shapes in the order of  x^2, x^3, x^4, x^5 ,x^6 */
    QVector< QVector <double> > TorsionalCoefficients; /**< Polynomial coefficients of the mode shapes in the order of  x^2, x^3, x^4, x^5 ,x^6 */
    QVector< QVector <double> > RadialCoefficients; /**< Polynomial coefficients of the mode shapes in the order of  x^2, x^3, x^4, x^5 ,x^6 */
    QVector< QVector <double> > UnsortedCoefficients; /**< Polynomial coefficients of the mode shapes in the order of  x^2, x^3, x^4, x^5 ,x^6 */

    QVector < QVector < QVector < double > > > FlapWiseNodeTranslations; /**< Translations of the blade nodes due to the mode shapes, sorted from low to high frequencies */
    QVector < QVector < QVector < double > > > EdgeWiseNodeTranslations; /**< Translations of the blade nodes due to the mode shapes, sorted from low to high frequencies */
    QVector < QVector < QVector < double > > > TorsionalTranslations; /**< Translations of the blade nodes due to the mode shapes, sorted from low to high frequencies */
    QVector < QVector < QVector < double > > > RadialNodeTranslations; /**< Translations of the blade nodes due to the mode shapes, sorted from low to high frequencies */
    QVector < QVector < QVector < double > > > UnsortedNodeTranslations; /**< Translations of the blade nodes due to the mode shapes, sorted from low to high frequencies */


	//QVectors holding Blade Foil Coordinates and Geometric Information
	QVector<double> ChordLengths;
	QVector<double> Radii;
	QVector<double> ChordTwist;
	QVector<double> LocalXOffset;
	QVector<double> LocalYOffset;
	QVector<double> PAxisX;
	QVector<double> PAxisY;
	QVector<double> PAxisZ;


    bool *m_cancelCalculation;  // a pointer to a bool that reacts on the progress dialog cancelation


private:
    QVector< QVector <double> >  m_results;
    QStringList m_availableVariables;


};

#endif // BLADESTRUCTURE_H
