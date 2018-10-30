#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif
#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif
#ifndef TAPEREDELEM_H
#define TAPEREDELEM_H


#include <Eigen/Dense>
#include "node.h"
#include "structelem.h"
using Eigen::MatrixXd;
using Eigen::Matrix2f;
using Eigen::Matrix3d;
/*!
 * The TaperedElem class is designed to be a local element with in the FEM solution.
 * This function is responsible for creating the local element matricies. The Tapered Element
 * is a combination of two StructElem which contain the structral properties at each end.
 * Properties0 is designed to be the inner radial node and the Properties1 is the outer. This is not a
 * critical part of the maths more a convention followed through out the program to avoid confusion in programming.
 * Note: the gyroscopic matrix is not included as it is considered negligible and would be mean the
 * eigensolution would be solved via the duncan method. This object is essentially a c++ implementation of the CurveFEM
 * subroutines responsible for the element matricies. \n Ref: Dynamic Analysis Tool Development for Advanced Geometry Wind Turbine Blades
 * By SCOTT MICHAEL LARWOOD 2009. All credit too him for deriving the matrix formulas and coordinate transforms.
 */
class TaperedElem
{
public:
        /** Constructor Handles most of the construction of the local matricies apart from the
        * the Axial Stiff. It is a little bit
        * ugly but done because so many variables are used it is easier to self contain this code.
        *@param _Properties0 is a StructElem containing the inner radius structural properties.
        *@param _Properties1 is a StructElem containing the outer radius structural properties.
        *@param _omega Nominal Turbine operating speed.*/
        TaperedElem(StructElem _Properties0,
                             StructElem _Properties1,double _omega);
        TaperedElem();

        void InitAxialStiff(QVector<TaperedElem> ElemArray, int CurrentPos);
        /** InitAxial Stiff is responsible for creating the AxialStiff matrix which requires
        * the properties of the other elements on the blade.
        *@param ElemArray is the vector of TaperedElem elements.
        *@param CurrentPos is the location of the instance within the Elem Array */
        static const int Row = 12; /**< The Row is used to set the Matrix sizes*/
        MatrixXd MassMatrix; /**< Mass Matrix container*/
        MatrixXd ElasticStiff; /**< Elastic Stiffness*/
        MatrixXd SpinStiff; /**< Spin Stiffness Matrix*/
        MatrixXd AxialStiff; /**< Axial Stiffness Matrix*/
        MatrixXd AxialRed; /**< Axial Reduction Matrix container*/
        MatrixXd K;/**< Total Stiffness Matrix*/
        double l; /**< Length between nodes*/
        double omega; /**< Wind turbine nominal rotational speed*/
        StructElem Properties0;/**< The 2D structural properties at the inner radius node of the Element */
        StructElem Properties1; /**< The 2D structural properties at the outer radius node of the Element */
        std::vector<int> Connectors; /**< The global coordinates of the DOF */
        Matrix3d b; /**< 3X3 Transformation Matrix */
        MatrixXd GiantB;/**< 12X12 Transformation Matrix constructed from 4 copies of the b Matrix */
        MatrixXd GiantBtr; /**< Transpose of GiantB */
        double cy;/**< y coordinate*/
        double cz;/**< z coordinate*/

private:
        double c0, d0 , e0; /**< Intermediate step values*/
};

#endif // TAPEREDELEM_H
