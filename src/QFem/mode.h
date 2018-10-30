#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#ifndef MODE_H
#define MODE_H


#include <Eigen/Dense>
#include "taperedelem.h"
#include "node.h"
#include "unitvector.h"
#include "deformationvector.h"
#include <QVector>
using Eigen::MatrixXd;
using Eigen::Matrix2f;
using Eigen::MatrixXcd;
enum ModeTypesEnum{Flap, Edge, Torsion, Radial, UnSorted};
class Mode
{
public:
    Mode();

    MatrixXd Polynomial;
    double Frequency;
    QVector<DeformationVector> ModeShape;
    ModeTypesEnum ModeType;

    void PrintMode();


};

#endif // MODE_H
