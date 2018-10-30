#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif
#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif
#ifndef EQNMOTION_H
#define EQNMOTION_H


#include <Eigen/Dense>
#include "taperedelem.h"
#include "node.h"
#include "unitvector.h"
#include "deformationvector.h"
#include "mode.h"
#include <QVector>
using Eigen::MatrixXd;
using Eigen::Matrix2f;
using Eigen::MatrixXcd;
/*!
 * The EqnMotion Class is implemented to handle the eigenvalue problem
 * used to create the mode shapes. This class will take the local
 * element properties and assemble the global matrix. Next the Eigenvalue
 *problem is solved and then post processed to produce the modeshapes
 *fitted to a polynomial.
 *
 */
class EqnMotion //: public MatrixXd
{
public:
    MatrixXd MassMat;/**< <Global Mass Matrix*/
    MatrixXd StiffMat;/**< Global Stiffness Matrix*/
    MatrixXcd EigVector;/**< Container for the EigVectors number of DOF rows and Mode Col*/
    MatrixXcd EigValues;/**< Container for the EigValues 1 Col*/
    QVector<Mode> ModeContainer;
    void ModeSorter(Eigen::MatrixXd ConMatrix, double xCount, double yCount, double zCount, double TorsionCount, bool Verbose, int j);
    QVector<Node> NodeVec;
    QVector<UnitVector> PrincipleAxis1Vec,AxialAxisVec,PrincipleAxis2Vec;

public:
    /**
    *Constructor.
    * @param _Row Size of the equations. ie how many DOF
    */
    EqnMotion(int _Row){
        MassMat.resize(_Row,_Row);
        MassMat.setZero();
        StiffMat.resize(_Row,_Row);
        StiffMat.setZero();
    }

    /**
    *Removes multiple DOF by calling DeleteDOF multiple times
    *@param DOFList Vector of integers of the DOF numbers to delete
    */
    void DeleteDOFs(std::vector<int> DOFList);
    /**
    *Takes a TaperedElem and localises it into the global matrix
    *@param ElementIn TaperedElem containing the Stiffness and Mass Matricies
    */
    void AddTaperedElem(TaperedElem ElementIn);
    /**
    *Localise the indivdual element into the global matrix using the DOF provided in DOFs
    * Designed to be generic so can be used for Spring and Mass Matricies
    * @param ElemMatrix Local Matrix to be inserted
    * @param Connectors DOF numbers to localise
    * @param Mat Reference to the global matrix to insert into
    */
    void Insert(MatrixXd ElemMatrix,std::vector<int> Connectors,MatrixXd &Mat);
    /**
    *Solve the Eigenvalue problem to get the EigVector and EigValues
    *
    */
    void GetEigSoln();
    /**
    *Take a previously deleted DOF by adding a row of
    * zeros and then shift the rows around until the origonal
    * order is restored
    *@param Insert DOF number to replace
    */
    void ReplaceRigidDOF(int Insert);
    /**
    *Use ReplaceRigidDOF to replace multiple lines
    *@param DOFList Vector of integers of the DOF numbers to replace
    */
    void ReplaceRigidDOFs(std::vector<int> DOFList);
    //Obs
//    /**
//    This function takes the matrix of mode shapes and fits the polynomial
//      to them. The mode shapes are weighted such that the height of the blade is
//      normalised to 1 and the tip deflection is also normalized to one. This is
//      a big weakness in this code but is following the guidelines of FAST. They state
//      (in 2005 mind you) that this may be fixed in later versions. The polynomial fit is
//      created by finding the magnitude of deflection (ignoring angle deflections) at each
//      node and then creating a matrix like this,\n
//      [x(0)^0 x(0)^1 x(0)^2 x(0)^4  |coeff0     |       |Deflection Mag Node 0  |\n
//       ......... ......... ......... .........           |coef....    | =     |Deflection Mag Node ...|\n
//       x(1)^0 x(1)^1 x(1)^2 x(1)^4] |coeff(dof) |       |Deflection Mag Node dof|\n
//       Take the inverse the the equation gives you the coeffs.
//       This can be improved by adding extra terms which allows control of the
//       gradient at the ends. This is interesting to constrain the angle of the polynomial
//       at node 0

//       The modes are then dumped into Flapwise, Edgewise containers based on which
//       component dominates.
//    */
//    void FitPolynomial();
    /**
    This function applies FitPolynomial across all modes.
    This takes the matrix of mode shapes and fits the polynomial
      to them. The mode shapes are weighted such that the height of the blade is
      normalised to 1 and the tip deflection is also normalized to one. This is
      a big weakness in this code but is following the guidelines of FAST. They state
      (in 2005 mind you) that this may be fixed in later versions.
      The polynomial fitting is achieved by feeding into a gsl tool box function.
    @param Degree = Highest order power +1 ie X^6 means degree = 7
    */
    void FitPolynomials();



private:

    /**
    *Remove DOF by shifting it to the last position
    *then removing the final row.
    */
    void DeleteDOF(int DelDOF, Eigen::MatrixXd &InputMat);
    /**
    *Sets any near zero elements to 0. Used to clear up
    * numerical problems
    */
    void TrashFilter(Eigen::MatrixXd &InputMat);
    /**
    *Sets any near zero elements to 0. Used to clear up
    * numerical problems
    */
    void TrashFilter(Eigen::MatrixXcd &InputMat);
    /**
    See FitPolynomials
    @param mode = mode number being fitted
    */
    void FitPolynomial(int mode);
    void SortModes(QVector<Mode> &ModeCon);

    MatrixXd pinv(MatrixXd& m, double epsilon = 1E-9);
};

#endif // EQNMOTION_H
