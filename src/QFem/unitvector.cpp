#include "unitvector.h"

UnitVector::UnitVector()

{
    Coords.setZero();
    Coords(3)=0;//No translation availiable on a vector.

}
