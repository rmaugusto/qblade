#ifndef STRUCTINTEGRATOR_H
#define STRUCTINTEGRATOR_H
#include <vector>
#include <stdlib.h>
#include "node.h"
#include <QVector>
class StructIntegrator
/*! This class takes the input coordinates and integrates over the areas to find the
* values required to get the structural properties of a composite structure in the
* sense of a wind turbine blade with structural spars.This object represents a single element
* like a spar, inner or outer foil.
* This is achieved by implementing the integrals found in
* Hansens Aerodynamics of Wind turbines 2ed 2008 P109-111.
* The member variables are all named as seen in Hansen so if some of the mathematics doesn't immediately
* make sense consult the textbook.
*/
{
public:
    double ESXR;/**< Moment of stiffness about the Reference X axis - Consult Hansen if this term is unfamiliar */
    double ESYR;/**< Moment of stiffness about the reference Y the axis - Consult Hansen if this term is unfamiliar */
    double EA;/**< Longitudinal Stiffness*/
    double EIXR;/**< Moment of stiffness inertia x direction(reference coordinate system)*/
    double EIYR;/**< Moment of stiffness inertia y direction(reference coordinate system)*/
    double EDXYR;/**< Moment of centrifugal stiffness*/
    double E;/**< Modulus of Elasticity*/
    double CenY; /**< Centroid Coordinate Y*/
    double CenX;/**< Centroid Coordinate X*/
    double rhoA;/**< Linear Density, ie how many kg per m of blade structure */
    double rho; /**< Material Density */
    double JPrime; /**< Moment of inertia */
    double Area;/**< Area */
    int resolution;
    QVector<Vector4d> GlobalCoords;

    StructIntegrator(std::vector<double> _xCoords,
                     std::vector<double> _yCoords,double _E, double _rho);    /**< One of two constructors provided where the Coordinates and other properties are stored
    * @param _xCoords are the x(ref) coordinates of the section to be integrated
    * @param _xCoords are the x(ref) coordinates of the section to be integrated
    * @param _E Modulus of Elasticity
    * @param _Rho Material Density
    */

    StructIntegrator();/**< The second constructor, when the Integrator is just used a storage container
      thus no inputs
    */


    bool pointInPolygon(std::vector<double> polyX,std::vector<double> polyY, double x,double y);
    /**< Determines whether point is inside or outside
    * This section of code was sourced from the internet and I would like to make a
    * strong note of that. The author was emailed and permission obtained for its use.\n
    * The code was slightly adapted for c++ rather than c.\n
    * Here on is from the site:\n
    *  http://alienryderflex.com/polygon/  Darel Rex Finley \n
    *  Globals which should be set before calling this function: \n
    *  The function will return YES if the point x,y is inside the polygon, or
    *  NO if it is not.  If the point is exactly on the edge of the polygon,
    *  then the function may return YES or NO.\n
    *  Note that division by zero is avoided because the division is protected
    *  by the "if" clause which surrounds it.
    */

    void CalculateRef();
    /**<
    * Calculates the values required to get the structural properties of the foil.
    * Works by iterating over the a rectangle over the max size of the foil
    * calculates only for points inside the foil/or arbitrary structural shape.\n
    * Formulas derived from Hansen in Aerodynamics of wind turbines
    * Pages 110.
    * add values for the first integration \n
    * Three integration Terms required to extract the section properties.\n
    *  • Moment of stiffness inertia about the axis XR: [EIXR] = ∫A EYR2d A. \n
    *  • Moment of stiffness inertia about the axis YR: [EIYR] = ∫A EXR2d A. \n
    *  • Moment of centrifugal stiffness: [EDXYR] = ∫A EXRYRd A
    */

    void CalculateJRef(double RefX, double RefY);
    /**<
    * Calculates the Polar moment of inertia centred about the centroid
    * @param RefX is the x Coordinate of the Centroid w.r.t the reference system
    * @param RefY is the y Coordinate of the Centroid w.r.t the reference system
    */


    void PrintGlobalCoords(Node NodeID, double ChordAngle);

    void PrintCoords();
    void PrintValues();

    std::vector<double> xCoords;/**< Section Coordinates */
    std::vector<double> yCoords;/**< Section Coordinates */
};

#endif // STRUCTINTEGRATOR_H
