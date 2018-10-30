/****************************************************************************

    Vector Class
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

// Simple class implementing the usual properties and methods of a 3D Vector


#include <math.h>
#include "CVectorf.h"
#include "Quaternion.h"
#include "../Globals.h"
#include "../src/Serializer.h"

void CVectorf::serialize(){
    if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100053){
    double dummy;
    g_serializer.readOrWriteDouble(&dummy);
    x = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    y = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    z = dummy;
    }
    else{
        g_serializer.readOrWriteFloat(&x);
        g_serializer.readOrWriteFloat(&y);
        g_serializer.readOrWriteFloat(&z);
    }
}

void CVectorf::RotateN(CVectorf n, float NTilt)
{
    //rotation around an arbitary axis n
    CVectorf r;
    n.Normalize();
    r=CVectorf(x,y,z);
    r = n*(n.dot(r))+(n*r)*n*cos(NTilt/180*PI)+(n*r)*sin(NTilt/180*PI);
    x=r.x;
    y=r.y;
    z=r.z;

}


void CVectorf::RotateX(CVectorf const &O, float XTilt)
{
    //Rotate the vector around the X-axis, by an angle XTilt
    CVectorf OP;
    OP.x = x-O.x;
    OP.y = y-O.y;
    OP.z = z-O.z;

    XTilt *=PI/180.0;
    y = O.y + OP.y * cos(XTilt) - OP.z * sin(XTilt);
    z = O.z + OP.y * sin(XTilt) + OP.z * cos(XTilt);
}

void CVectorf::RotateY(CVectorf const &O, float YTilt)
{
    //Rotate the vector around the Y-axis, by an angle YTilt
    CVectorf OP;
    OP.x = x-O.x;
    OP.y = y-O.y;
    OP.z = z-O.z;

    YTilt *=PI/180.0;

    x = O.x + OP.x * cos(YTilt) + OP.z * sin(YTilt);
    z = O.z - OP.x * sin(YTilt) + OP.z * cos(YTilt);
}


void CVectorf::RotateZ(CVectorf const &O, float ZTilt)
{
    //Rotate the vector around the Z-axis, by an angle ZTilt
    CVectorf OP;
    OP.x = x-O.x;
    OP.y = y-O.y;
    OP.z = z-O.z;

    ZTilt *=PI/180.0;

    x = O.x + OP.x * cos(ZTilt) + OP.y * sin(ZTilt);
    y = O.y - OP.x * sin(ZTilt) + OP.y * cos(ZTilt);
}

void CVectorf::RotZ(float ZTilt)
{
    //Rotate the vector around the Z-axis, by an angle ZTilt
    //with a corrected matrix
    CVectorf OP;
    OP.x = x;
    OP.y = y;

    x =  OP.x * cos(ZTilt) - OP.y * sin(ZTilt);
    y =  OP.x * sin(ZTilt) + OP.y * cos(ZTilt);
}

void CVectorf::RotY(float YTilt)
{
    //Rotate the vector around the Y-axis, by an angle ZTilt
    CVectorf OP;
    OP.x = x;
    OP.z = z;

    x =  OP.x * cos(YTilt) + OP.z * sin(YTilt);
    z = -OP.x * sin(YTilt) + OP.z * cos(YTilt);
}

void  CVectorf::RotateY(float YTilt)
{
    YTilt *=PI/180.0;

    float xo = x;
    float zo = z;
    x =  xo * cos(YTilt) + zo * sin(YTilt);
    z = -xo * sin(YTilt) + zo * cos(YTilt);
}


