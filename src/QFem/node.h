#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <stdlib.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
using Eigen::Vector4d;
/*!
 * The Node Class is a way to storing the node identifier, its degrees of freedom
 * and its coordinates
 *
 */
class Node
{

public:

    Node();
    int IDnumber; /**< ID Identifier */
    std::vector<int> DOFNums;/**< Vectors of integers which match the DOF numbers */
    Vector4d Coords;/**< Coordinates initialized to 0,0,0,1 in the constructor */
    void PrintCoords();
    Node Subtract(Node VectorB);
};

#endif // NODE_H
