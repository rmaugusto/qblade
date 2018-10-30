#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#ifndef STRUCTELEM_H
#define STRUCTELEM_H

#include "structintegrator.h"
#include "Clipper/clipper.hpp"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "node.h"
#include "unitvector.h"
#include "deformationvector.h"
using namespace ClipperLib;
using Eigen::Vector3d;
using Eigen::Vector4d;
/*!
 * This object is used to create and store the 2D structural data found at each node.
 * The methods with in this object create the geometry coordinates to create the hollow offset as well
 * as the spar.
 * The Hollow and Spar features are added by implementing the ClipperLib (http://www.angusj.com/) which allows
 * the boolean subtraction and addition of arbitrary polygons.
 * The properties are then combined into a single section and that localised about the principle axes using the mathematics
 * described in Hansens Aerodynamics of Wind turbines 2ed 2008 P109-111.
 */
class StructElem
{
public:
	StructIntegrator Outer;/**< StructIntegrator of the Outer Foil coordinates*/
	StructIntegrator Inner;/**< StructIntegrator of the Inner Foil coordinates, That is the properties of the hollow*/
	StructIntegrator Spar;/**< StructIntegrator of the Spar*/
	StructIntegrator Total;/**< The combination of the all of the sections*/
	Node NodeID; /**< The Node which the element lies on-QBLade Location Global Coordinates*/
	Node ElasticAxisNode; /**< The Node which the element lies on the Elastic Axis Global Coordinates*/
	UnitVector PrincipleAxis1,PrincipleAxis2, AxialAxis;/**< Unit Vectors of the Principle Axes*/
	double ShellE; /**< Elastic Modulus of the Shell/Solid structure (ie the part that is usually composite) */
	bool Hollow;/**< Is the foil hollow?*/
	bool WithSpar; /**< Does it have a spar?*/
	//Localising Variables See Hansen 111
	double Xe;/**< x distance from reference coordinate system to the point of elasticity*/
	double Ye;/**< y distance from reference coordinate system to the point of elasticity*/
	double EIx;/**< Moment of stiffness inertia x direction(reference coordinate system)*/
	double EIy;/**< Moment of stiffness inertia y direction(reference coordinate system)*/
	double EDXY;/**< Moment of centrifugal stiffness*/
	double alpha;/**< Angle in between the reference axis and the Elastic Axis */
	double EI1;/**< Moment of stiffness inertia about the first principle axis*/
	double EI2; /**< Moment of stiffness inertia about the second principle axis*/
	double EA; /**< Axial Stiffness*/
	double RhoA; /**< Linear Density, ie how many kg per m of blade structure */
	double JPrime; /**< Polar Moment of Inertia about the centroidal axis */
	double GJ; /**< Polar Moment of Stiffness inertia */
	double ChordAngle; /**< Angle between global reference and the chord line */
	double CenY; /**< Centroid Coordinate Y  */
	double CenX; /**< Centroid Coordinate X */
	double LocalYOffset,LocalXOffset; /**< Coordinates of the QBlade Node in terms of the Local Coordinates */
	double StructuralTwist;/**< Angle between the reference and the elastic axis, needed as an input for fast */
	double Area;/**< Area */
	std::vector<double> xCoords; /**< Outer Foil Coordinates Relative to the QBLADE node*/
	std::vector<double> yCoords;/**< Outer Foil Coordinates QBLADE node*/
	double ChordLength;/**< Chord Length */
	Eigen::Transform<double,2,Eigen::Affine> QbladeNode2BladeDefTrans;
	Eigen::Transform<double,3,Eigen::Affine> QbladeNode2ElasticTrans;
	Eigen::Transform<double,2,Eigen::Affine> StructuralTwistMatrix;
    QVector<Vector4d> GlobalCoords;
public:
    StructElem();
	/**< The constructor takes the coordinates, scales them and creates the Outer StructIntegrator
	*which is required by default
	* @param NodeID The Node which the structural properties are located on.
	* @param _xCoords Input normalized (by chord length of 1) coordinates
	* @param _yCoords Input normalized (by chord length of 1) coordinates
	* @param _ChordAngle Angle between the global reference and the chord.
	* @param _E Elastic Modulus of the shell material
	* @param _ChordLength
	* @param ShellDensity is the desity of the Shell/Solid material.
	*/
	StructElem(Node NodeID, std::vector<double> _xCoords,std::vector<double> _yCoords,double _ChordAngle,
			   double _E, double _ChordLength, double ShellDensity,double _LocalXOffset, double _LocalYOffset);

	void CreateSpar(double Position,double Thickness, double Angle,double _SparMaterialE, double SparDensity);
	/**< Creates a rectangular spar with four coordinates. This of a thickness and rotation according to the
	* inputs. This rough rectangle is feed through a boolean operation and trimmed to the size of the foil inner
	* @param Position is the normalized chordwise position of the spar
	* @param Thickness is the normalized thickness of the spar centred about the Position
	* @param Angle is the rotation applied to the section
	* @param _SparMaterialE Elastic Modulos of the Spar Material
	* @param SparDensity is the density of the Spar Material
	*/
	void CreateInner(double Offset);
	/**< CreateInner takes the outer foils coordinates and offsets the coordinates inwardly. This infomation is
	* then feed into an integrator Inner. This integrator then finds the properties of the foil if it was real.
	* This is then subtacted from the properties later
	* @param Offset is a double specifying the offset as a fraction of chordlength. */

	void LocaliseAboutElasticAxis();
	/**< Combines the properties of the Outer, Inner and spar sections and then
	* Localises them about the elastic axes using the formulas from Hansen
	*/

	void PrintProperties();
	/**< Prints the properties to console.*/

	void CalcOuterRef();
	void CalcInnerRef();
	void CalcSparRef();


    void PrintGlobalCoords();/**< Creates Global Coordinates*/
    void PrintGlobalCoords(DeformationVector DeformedNode, double Amplification);
	/**< Creates Global Coordinates but takes a node input, that a deformed node can be put in */
private:
	void TotalStruct();
	/**< Total Struct places the combined totals of Inner, outer and Spar into the Total object */

	void CalcJPrime();
	/**< Finds the polar moment of inertia about the centroid by calling the integrator objects
	to return their respective polar moment of interias.*/

};

#endif // STRUCTELEM_H
