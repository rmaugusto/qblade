/****************************************************************************

    QLLTCutPlane Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

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

#ifndef QLLTCUTPLANE_H
#define QLLTCUTPLANE_H

#include <QtWidgets>
#include "../src/Objects/CVector.h"
#include "../src/StorableObject.h"



class QLLTCutPlane : public StorableObject
{
    Q_OBJECT

public:
    QLLTCutPlane(QString name = "< no name >", StorableObject *parent = NULL);
    static QLLTCutPlane* newBySerialize ();
    void serialize ();
    void Update();
    void exportPlane(QString fileName);
    void exportPlaneCSV(QString fileName);
    void exportPlaneVTK(QString fileName);


    CVector CorrespondingAxisPoint(CVector Point, CVector Line1, CVector Line2);
    void render(bool redblue = true, bool vectors = false, int component = 0, double meanFrac = 0.3);
    void drawFrame();

    QList< QList< CVector > > m_velocities;
    QList< QList< CVector > > m_points;

    double m_length, m_width, m_X, m_Y, m_Z, m_X_rot, m_Y_rot, m_Z_rot, m_time;
    CVector m_curVelocity;
    double m_meanVelocity;
    int m_X_res, m_Y_res;
    bool is_computed;
    CVector m_Hub, m_Axis;

private:


};

#endif // QLLTCUTPLANE_H
