#include "Strut.h"
#include "../Store.h"
#include "../Serializer.h"
#include <QDebug>


Strut::Strut (QString name, double hheight, double hdistance, double bheight, double angle, double cho, C360Polar *pol, StorableObject *parent)
    : StorableObject (name, parent)
{
    hubHeight = hheight;
    hubDistance = hdistance;
    bladeHeight = bheight;
    chord = cho;
    polar = pol;
    polar = pol;
    strutAngle = angle;
    isMulti = false;
}

void Strut::copy(Strut *str){
    m_objectName = str->getName();
    polar = str->polar;
    hubDistance =str->hubDistance;
    hubHeight = str->hubHeight;
    bladeHeight = str->bladeHeight;
    circAngle = str->circAngle;
    chord = str->chord;
    strutAngle = str->strutAngle;
    point_b = str->point_b;
    point_h = str->point_h;
    isMulti = str->isMulti;
    m_MultiPolars = str->m_MultiPolars;
    m_MinMaxReynolds = str->m_MinMaxReynolds;
}

Strut::Strut (QString name, StorableObject *parent)
    : StorableObject (name, parent){
    hubHeight = 0;
    hubDistance = 0;
    bladeHeight = 0;
    chord = 0;
    polar = 0;
    numPanels = 0;
    polar = NULL;
    strutAngle = 0;
    pitch_axis = 0.5;
    isMulti = false;
}

void Strut::CreateSurfaces(int num)
{
    m_Surface.clear();
    m_PanelPoints.clear();
    QList<CVector> panel;

    CVector incr = (point_b-point_h)/num;
    CVector pos = point_h;

    int j;
    CVector PLA, PTA, PLB, PTB;

    CVector O(0,0,0);

    for (j = 0; j < num; ++j)
    {
        CSurface surface;

        surface.m_pFoilA = g_foilStore.getObjectByNameOnly(polar->getParent()->getParent()->getName());
        surface.m_pFoilB = g_foilStore.getObjectByNameOnly(polar->getParent()->getParent()->getName());

        PLA.x = chord*pitch_axis+incr.z;
        PLB.x = chord*pitch_axis+pos.z+incr.z;
        PLA.y = pos.y;
        PLB.y = pos.y+incr.y;
        PLA.z = pos.x;
        PLB.z = pos.x+incr.x;
        PTA.x = PLA.x-chord;
        PTB.x = PLB.x-chord;
        PTA.y = PLA.y;
        PTB.y = PLB.y;
        PTA.z = pos.x;
        PTB.z = pos.x+incr.x;

        surface.m_LA.Copy(PLA);
        surface.m_TA.Copy(PTA);
        surface.m_LB.Copy(PLB);
        surface.m_TB.Copy(PTB);

        surface.SetNormal();

        surface.RotA = CVector(pos.z, pos.y, pos.x);
        surface.RotB = CVector(pos.z+incr.z, pos.y+incr.y, pos.x+incr.x);
        surface.TwistAxis = surface.RotA - surface.RotB;
        surface.TwistAxis.Normalize();

        surface.m_NXPanels = 1;
        surface.m_NYPanels = 1;

        m_Surface.append(surface);

        pos += incr;
    }

//    for (int i=0;i<m_NSurfaces;i++){
//        if (m_bIsInverted){
//            m_Surface[i].Normal *= -1;
//            m_Surface[i].NormalB *= -1;
//            m_Surface[i].NormalA *= -1;
//        }
//    }

    for (int i=0;i<m_Surface.size();i++){
        if (num == 1){
            m_Surface[i].TwistAxisA = m_Surface[i].TwistAxis;
            m_Surface[i].TwistAxisB = m_Surface[i].TwistAxis;
            m_Surface[i].NormalA = m_Surface[i].Normal;
            m_Surface[i].NormalB = m_Surface[i].Normal;

        }
        else if (i == 0){
            m_Surface[i].TwistAxisA = m_Surface[i].TwistAxis;
            m_Surface[i].TwistAxisB = (m_Surface[i].TwistAxis+m_Surface[i+1].TwistAxis)/2;

            m_Surface[i].NormalA = m_Surface[i].Normal;
            m_Surface[i].NormalB = (m_Surface[i].Normal+m_Surface[i+1].Normal)/2;
        }
        else if (i==num-1){
            m_Surface[i].TwistAxisA = (m_Surface[i-1].TwistAxis+m_Surface[i].TwistAxis)/2;
            m_Surface[i].TwistAxisB = m_Surface[i].TwistAxis;

            m_Surface[i].NormalA = (m_Surface[i-1].Normal+m_Surface[i].Normal)/2;
            m_Surface[i].NormalB = m_Surface[i].Normal;
        }
        else{
            m_Surface[i].TwistAxisA = (m_Surface[i-1].TwistAxis+m_Surface[i].TwistAxis)/2;
            m_Surface[i].TwistAxisB = (m_Surface[i].TwistAxis+m_Surface[i+1].TwistAxis)/2;

            m_Surface[i].NormalA = (m_Surface[i-1].Normal+m_Surface[i].Normal)/2;
            m_Surface[i].NormalB = (m_Surface[i].Normal+m_Surface[i+1].Normal)/2;
        }

        m_Surface[i].TwistAxisA.Normalize();
        m_Surface[i].TwistAxisB.Normalize();
        m_Surface[i].NormalA.Normalize();
        m_Surface[i].NormalB.Normalize();

    }

    for (int i=0;i<num;i++){
        //pitch
        m_Surface[i].m_LA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, strutAngle);
        m_Surface[i].m_TA.Rotate(m_Surface[i].RotA,m_Surface[i].TwistAxisA, strutAngle);
        m_Surface[i].m_LB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, strutAngle);
        m_Surface[i].m_TB.Rotate(m_Surface[i].RotB,m_Surface[i].TwistAxisB, strutAngle);
        m_Surface[i].NormalA.Rotate(O,m_Surface[i].TwistAxisA, strutAngle);
        m_Surface[i].NormalB.Rotate(O,m_Surface[i].TwistAxisB, strutAngle);

        m_Surface[i].m_LA.RotateY(circAngle);
        m_Surface[i].m_TA.RotateY(circAngle);
        m_Surface[i].m_LB.RotateY(circAngle);
        m_Surface[i].m_TB.RotateY(circAngle);

        m_Surface[i].SetNormal();

        m_Surface[i].m_XDistType = 1;
        m_Surface[i].m_YDistType =  0;
        m_Surface[i].CreateXPoints();
        m_Surface[i].Init();

        panel.clear();
        panel.append(m_Surface[i].m_LA);
        panel.append(m_Surface[i].m_LB);
        panel.append(m_Surface[i].m_TA);
        panel.append(m_Surface[i].m_TB);
        m_PanelPoints.append(panel);
    }

    for (int i=0;i<num;i++){
        m_Surface[i].SetSidePoints();
    }

}


void Strut::serialize() {
    StorableObject::serialize();

    g_serializer.readOrWriteStorableObject(&polar);

    g_serializer.readOrWriteInt(&numPanels);

    g_serializer.readOrWriteDouble(&hubDistance);
    g_serializer.readOrWriteDouble(&hubHeight);
    g_serializer.readOrWriteDouble(&bladeHeight);
    g_serializer.readOrWriteDouble(&chord);
    g_serializer.readOrWriteDouble(&strutAngle);
    g_serializer.readOrWriteDouble(&circAngle);

    point_b.serialize();
    point_h.serialize();

    if (g_serializer.getArchiveFormat() >= 100048){
        g_serializer.readOrWriteStorableObjectVector(&m_MultiPolars);
        g_serializer.readOrWriteString(&m_MinMaxReynolds);
        g_serializer.readOrWriteBool(&isMulti);
    }

    if (g_serializer.getArchiveFormat() >= 100050) g_serializer.readOrWriteDouble(&pitch_axis);

}

void Strut::restorePointers() {
    StorableObject::restorePointers();

    g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&polar));
    for (int i = 0; i < m_MultiPolars.size(); ++i) {
        g_serializer.restorePointer(reinterpret_cast<StorableObject**> (&(m_MultiPolars[i])));
    }
}

Strut* Strut::newBySerialize() {
    Strut* strut = new Strut ();
    strut->serialize();
    return strut;
}
