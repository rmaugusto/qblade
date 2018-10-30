#ifndef STRUT_H
#define STRUT_H

#include "../src/XBEM/360Polar.h"
#include "../StorableObject.h"
#include "../Objects/Surface.h"
#include "../Objects/CVector.h"


// class for blade struts
class Strut : public StorableObject
{
public:
    Strut (QString name = "< no name >", StorableObject *parent = NULL);
    Strut (QString name, double hheight, double hdistance, double bheight, double angle, double chord, C360Polar *pol, StorableObject *parent);
    void serialize();
    void restorePointers();

    static Strut* newBySerialize();
    double getHubDistance(){ return hubDistance; }
    double getHubHeight(){ return hubHeight; }
    int getNumPanels(){ return numPanels; }
    double getChord(){ return chord; }
    double getStrutAngle(){ return strutAngle; }
    double getBladeHeight(){ return bladeHeight; }
    bool getIsMulti() { return isMulti; }
    C360Polar* getPolar(){ return polar; }
    QVector<C360Polar*> getMultiPolar(){ return m_MultiPolars; }

    void copy(Strut *str);
    void CreateSurfaces(int num = 1);

    bool isMulti;


    QList <CSurface> m_Surface; //no need to serialize
    QList< QList <CVector> > m_PanelPoints;
    QList<VortexNode> m_StrutNodes; // stores undeflected strut nodes

    CVector point_b;
    CVector point_h;
    double circAngle;
    double pitch_axis;


    //variables used to store multi polar assginment
    QVector<C360Polar*> m_MultiPolars;
    QString m_MinMaxReynolds;
    //

private:
    double hubDistance;
    double hubHeight;
    double bladeHeight;
    int numPanels;
    double chord;
    double strutAngle;
    C360Polar *polar;

};

#endif // STRUT_H
