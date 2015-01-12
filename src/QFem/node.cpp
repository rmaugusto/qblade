#include "node.h"
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
using Eigen::Vector4d;
Node::Node()
{
    Coords.setZero();
    Coords(3)=1;


}

void Node::PrintCoords(){
    std::cout<<Coords(0)<<","<<Coords(1)<<","<<Coords(2)<<std::endl;
}

Node Node::Subtract(Node VectorB)
{
    Vector4d DiffVec = Coords - VectorB.Coords;
    Node DiffNode;
    DiffNode.Coords = DiffVec;
    return DiffNode;
}
