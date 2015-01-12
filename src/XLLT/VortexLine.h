#ifndef VORTEXLINE_H
#define VORTEXLINE_H

#include "VortexNode.h"
#include <qobject.h>

// Class for a Vortex Line Element;

class VortexLine
{
    friend class QLLTSimulation;
public:
    VortexLine();

    double  GetLength();
    void    Update(double dT);
    void    SetGamma(double gamma);
    double  GetGamma();
    void    Delete();


private:

    double Gamma;
    double Length;
    double oldLength;
    int m_iL, m_iT;//leading and trailing Node numbers
    bool isTip, isHub;
    bool isTrailing, isShed;

    CVector m_V_induced;        //the velocity that is induced on the panel from wake and blade vorticity in absolute coordinates

    int fromTimestep;           //the timestep at which the panel was shed

    double age;
    double coreSize;
    double m_KinViscosity;
    double m_TurbulentViscosity;
    double m_VortexTimeOffset;

    int fromStation;            //the blade station at which the panel was shed

    bool isConcentrated;

    QVector <VortexNode *> *m_Nodes;
    QVector <VortexLine *> *m_Lines;

    VortexNode* pL;              //pointer to the LA node
    VortexNode* pT;              //pointer to the LB node
};

#endif // VORTEXLINE_H
