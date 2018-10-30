/****************************************************************************

    CVectorf Class
    Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#include <math.h>

#ifndef CVectorf_H
#define CVectorf_H

class CVectorf
{
public:
    float x;
    float y;
    float z;

    void serialize();

    //inline costructors
    CVectorf()
    {
        x  = 0.0;
        y  = 0.0;
        z  = 0.0;
    }

    CVectorf(float const &xi, float const &yi, float const &zi)
    {
        x  = xi;
        y  = yi;
        z  = zi;
    }

    //inline operators
    bool operator ==(CVectorf const &V)
    {
        //used only to compare point positions
        return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
    }

    void operator =(CVectorf const &T)
    {
        x = T.x;
        y = T.y;
        z = T.z;
    }

    void operator+=(CVectorf const &T)
    {
        x += T.x;
        y += T.y;
        z += T.z;
    }

    void operator-=(CVectorf const &T)
    {
        x -= T.x;
        y -= T.y;
        z -= T.z;
    }

    void operator*=(float const &d)
    {
        x *= d;
        y *= d;
        z *= d;
    }

    CVectorf operator *(float const &d)
    {
        CVectorf T(x*d, y*d, z*d);
        return T;
    }

    CVectorf operator *(CVectorf const &T)
    {
        CVectorf C;
        C.x =  y*T.z - z*T.y;
        C.y = -x*T.z + z*T.x;
        C.z =  x*T.y - y*T.x;
        return C;
    }

    CVectorf operator /(float const &d)
    {
        CVectorf T(x/d, y/d, z/d);
        return T;
    }

    CVectorf operator +(CVectorf const &V)
    {
        CVectorf T(x+V.x, y+V.y, z+V.z);
        return T;
    }

    CVectorf operator -(CVectorf const &V)
    {
        CVectorf T(x-V.x, y-V.y, z-V.z);
        return T;
    }


    //inline methods
    void Copy(CVectorf const &V)
    {
        x = V.x;
        y = V.y;
        z = V.z;
    }

    void Set(float const &x0, float const &y0, float const &z0)
    {
        x = x0;
        y = y0;
        z = z0;
    }

    void Set(CVectorf const &V)
    {
        x = V.x;
        y = V.y;
        z = V.z;
    }

    void Normalize()
    {
        float abs = VAbs();
        if(abs< 1.e-10) return;
        x/=abs;
        y/=abs;
        z/=abs;
    }

    float VAbs()
    {
        return sqrt(x*x+y*y+z*z);
    }

    float dot(CVectorf const &V)
    {
        return x*V.x + y*V.y + z*V.z;
    }

    bool IsSame(CVectorf const &V)
    {
        //used only to compare point positions
        return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
    }

    void Translate(CVectorf const &T)
    {
        x += T.x;
        y += T.y;
        z += T.z;
    }

    //other methods
    void RotateN(CVectorf n, float NTilt);
    void RotateX(CVectorf const &O, float XTilt);
    void RotateY(CVectorf const &O, float YTilt);
    void RotateZ(CVectorf const &O, float ZTilt);
    void RotateY(float YTilt);
    void RotZ(float ZTilt);
    void RotY(float YTilt);



};

#endif
