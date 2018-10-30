#ifndef STATICEQUATION_H
#define STATICEQUATION_H
#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif
#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

#include <Eigen/Dense>
#include "taperedelem.h"
#include "node.h"
#include "unitvector.h"
#include "deformationvector.h"
#include "mode.h"
#include "eqnmotion.h"
#include "forcingterm.h"
#include <QVector>
using Eigen::MatrixXd;
using Eigen::Matrix2f;
using Eigen::MatrixXcd;


/*!
 * The StaticEquation class is set up take the stiffness matrix set up functionality of
 * EqnMotion but instead of solving the eigenvlaue problem, solving the static equation.
 *
 */
class StaticEquation : public EqnMotion
{
public:
    MatrixXd ForceVector;/**< <Force Vector*/
    MatrixXd DispVector; /**< Displacement Vector*/
    QVector<DeformationVector> DeformedShape; /**< Vector of Deformations*/
    QVector<DeformationVector> StrainVectors;
    QVector < QVector <double> > VMStresses;
    QVector < double > VMStation;

public:
    StaticEquation(int _Row) : EqnMotion(_Row)
    {
        ForceVector.resize(_Row,1);
        ForceVector.setZero();
        DispVector.resize(_Row,1);
        DispVector.setZero();

    }

    /*!
     * Deletes the Forces from the degrees of freedom that are fixed
     * and reduces the matrix
     *
     */
    virtual void DeleteForceDOF(int DelDOF, MatrixXd &InputMat);
    /*!
     * Restores the Force Matrix back to zeros.
     *
     */
    virtual void ReplaceRigidForceDOF(int Insert);
    /*!
     * Does the Same as EqnMotion::ReplaceRigidDOFs
     * but also replaces the constrained force vector to zero.
     */
    virtual void ReplaceRigidDOFs(std::vector<int> DOFList);
    /*!
     * Does the Same as EqnMotion::DeleteDOFs
     * but also constrains the force vector to zero.
     */
    virtual void DeleteDOFs(std::vector<int> DOFList);
    /*!
     * Inserts forcing terms into the Force Vector
     */
    void InsertForcingTerm(ForcingTerm InputForce);
    /*!
     * Performs the Matrix inversion to the strain vector
     */
    void StaticSolution();
    /*!
     * Loads the results into a deformation vector.
     */
    void CreateDeformCoords();
    /*!
     * Takes the nodal strains and back calculates the stresses.
     */
    void BackStrains();
    void BackStresses(QVector<TaperedElem> InputElems);
};

#endif // STATICEQUATION_H
