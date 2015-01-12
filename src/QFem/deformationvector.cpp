#include "deformationvector.h"

DeformationVector::DeformationVector()

{
    Coords.setZero();
    xAxisAngle=0;
    yAxisAngle=0;
    zAxisAngle=0;

}


DeformationVector DeformationVector::Subtract(DeformationVector VectorB)
{
    //Just create difference between two vectors for Strain Calcs
    //Should use as DeformationVector Diff = VectorA.Substract(VectorB);
    Vector4d DiffVec = Coords - VectorB.Coords;
    DeformationVector DiffNode;
    DiffNode.Coords = DiffVec;
    DiffNode.xAxisAngle = xAxisAngle - VectorB.xAxisAngle;
    DiffNode.yAxisAngle = yAxisAngle - VectorB.yAxisAngle;
    DiffNode.zAxisAngle = zAxisAngle - VectorB.zAxisAngle;
    return DiffNode;


}
