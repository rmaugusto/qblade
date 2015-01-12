#ifndef DEFORMATIONVECTOR_H
#define DEFORMATIONVECTOR_H
#include "node.h"
class DeformationVector : public Node
{
public:
    DeformationVector();
    double zAxisAngle;
    double xAxisAngle;
    double yAxisAngle;
    DeformationVector Subtract(DeformationVector VectorB);

};

#endif // DEFORMATIONVECTOR_H
