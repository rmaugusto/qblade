#ifndef VORTEXLINE_H
#define VORTEXLINE_H

#include "VortexNode.h"
#include <qobject.h>
#include "../src/Objects/Panel.h"


// Class for a Vortex Line Element;

class VortexLine
{
    friend class QLLTSimulation;
public:
    VortexLine();

    void    Update(double dT);
    void    InitVortexSize();
    void    InitLength();
    void    SetGamma(double gamma);
    double  GetGamma();
    void    Delete();
    CVector Velocity;
    CVector CorrVelocity;



//private:
    double Gamma;
    double VizGamma;          //for visualization purpose
    double stretchFactor;

    double Length;
    double oldLengthPredCorr;
    double oldLength;

    bool isTip, isHub;
    bool isTrailing, isShed;
    bool deleteLater;
    bool includeStrain;

    int fromTimestep;           //the timestep at which the panel was shed
    double fromTime;               //the time at which the panel was shed
    double fromRevolution;         //the revolution at which the panel was shed


    double age;
    double coreSizeSquared;
    double initial_coreSizeSquared;
    double m_KinViscosity;
    double m_TurbulentViscosity;
    double m_VortexTimeOffset;

    int fromStation;            //the blade line number at which the panel was shed

    bool isConcentrated;
    bool isStrut;

    QList <VortexNode *> *m_Nodes;
    QList <VortexLine *> *m_Lines;
    CPanel *leftPanel;
    CPanel *rightPanel;
    VortexLine *fromLine;

    VortexNode* pL;              //pointer to the LA node
    VortexNode* pT;              //pointer to the LB node
};

#endif // VORTEXLINE_H
