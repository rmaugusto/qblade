#ifndef VORTEXNODE_H
#define VORTEXNODE_H

#include "../Objects/CVector.h"
#include "QObject"

class VortexNode : public CVector
{
public:
    VortexNode(const double &xi, const double &yi, const double &zi);
    VortexNode();

    QList <void*> attachedLines;

    void attachLine(void *line);
    void detachLine(void *line);
    bool hasLines();

    int     fromTimestep;
    int     fromStation;
    int     fromBlade;
    bool    wasConvected;
    QList<CVector> oldPositions;

    //inline operators

    bool operator ==(VortexNode const &V)
    {
        //used only to compare point positions
        return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
    }

    bool operator ==(CVector const &V)
    {
        //used only to compare point positions
        return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
    }

    void operator =(VortexNode const &T)
    {
        x = T.x;
        y = T.y;
        z = T.z;
    }

    void operator =(CVector const &T)
    {
        x = T.x;
        y = T.y;
        z = T.z;
    }

    void operator+=(VortexNode const &T)
    {
        x += T.x;
        y += T.y;
        z += T.z;
    }

    void operator+=(CVector const &T)
    {
        x += T.x;
        y += T.y;
        z += T.z;
    }

    void operator-=(VortexNode const &T)
    {
        x -= T.x;
        y -= T.y;
        z -= T.z;
    }

    void operator-=(CVector const &T)
    {
        x -= T.x;
        y -= T.y;
        z -= T.z;
    }

    void operator*=(double const &d)
    {
        x *= d;
        y *= d;
        z *= d;
    }

    VortexNode operator *(double const &d)
    {
        VortexNode T(x*d, y*d, z*d);
        return T;
    }

    VortexNode operator *(VortexNode const &T)
    {
        VortexNode C;
        C.x =  y*T.z - z*T.y;
        C.y = -x*T.z + z*T.x;
        C.z =  x*T.y - y*T.x;
        return C;
    }

    VortexNode operator *(CVector const &T)
    {
        VortexNode C;
        C.x =  y*T.z - z*T.y;
        C.y = -x*T.z + z*T.x;
        C.z =  x*T.y - y*T.x;
        return C;
    }

    VortexNode operator /(double const &d)
    {
        VortexNode T(x/d, y/d, z/d);
        return T;
    }

    VortexNode operator +(VortexNode const &V)
    {
        VortexNode T(x+V.x, y+V.y, z+V.z);
        return T;
    }

    VortexNode operator +(CVector const &V)
    {
        VortexNode T(x+V.x, y+V.y, z+V.z);
        return T;
    }

    VortexNode operator -(VortexNode const &V)
    {
        VortexNode T(x-V.x, y-V.y, z-V.z);
        return T;
    }

    VortexNode operator -(CVector const &V)
    {
        VortexNode T(x-V.x, y-V.y, z-V.z);
        return T;
    }




};

#endif // VORTEXNODE_H
