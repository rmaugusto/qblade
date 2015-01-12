#ifndef QLLTSIMULATION_H
#define QLLTSIMULATION_H

#include "../Objects/CVector.h"
#include "QLLTModule.h"
#include "../XBEM/Blade.h"
#include "../XBEM/360Polar.h"
#include "VortexLine.h"
#include "VortexNode.h"
#include "../XUnsteadyBEM/WindField.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../StorableObject.h"



#include <QObject>


class QLLTModule;


class QLLTSimulation : public StorableObject, public ShowAsGraphInterface
{
    friend class QLLTModule;
    Q_OBJECT
public:

    void serialize ();
    static QLLTSimulation* newBySerialize ();
    void restorePointers ();
    NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // returns NULL if var not available
    QStringList getAvailableVariables (NewGraph::GraphType graphType);
    QString getObjectName () { return m_objectName; }

    QLLTSimulation(CBlade *blade, WindField *windfield);
    QLLTSimulation(QString name,   bool bisVawt,
                                   WindField *usedWindField,
                                   double timeStep,
                                   double numTimeStep,
                                   double aziDisc,
                                   CBlade *usedRotor,
                                   double rotorSpeed,
                                   double rotorYaw,
                                   double rotorTeether,
                                   double inflowAngle,
                                   double turbVisc,
                                   double airDens,
                                   double kinVisc,
                                   double firstWakeRowLength,
                                   double fullWakeRevs,
                                   double fineWakeRevs,
                                   double maxWakeAge,
                                   double vortexTimeOffset,
                                   double epsilon,
                                   double relaxFactor,
                                   double maxIter,
                                   double thinFactor,
                                   double cutOffRadius,
                                   bool bIncludeTower,
                                   double towerDrag,
                                   double towerRadius,
                                   double towerDist,
                                   bool bGround,
                                   double hubHeight,
                                   bool bWakeRollup,
                                   bool bShed,
                                   bool bPrescribed,
                                   bool turbConvection);


    ~QLLTSimulation();


    WindField* getUsedWindField () { return m_WindField; }
    CBlade* getUsedBlade () { return m_Blade; }
    bool getIncludeTower () { return m_bIncludeTower; }
    bool getIncludeGround () { return m_bGround; }
    bool getShed () { return m_bShed; }
    bool getPrescribed () { return m_bPrescribed; }
    bool getTurbConvection () { return m_bTurbConvection; }
    bool getWakeRollup () { return m_bWakeRollUp; }
    double getRunTime () { return dT*double(m_NumTimesteps); }
    double getRotorSpeed () { return m_Omega * 60 / 2 / PI; }
    double getYawAngle () { return m_YawAngle; }
    double getTeetherAngle () { return m_TeetherAngle; }
    double getInflowAngle () { return m_InflowAngle; }
    double getTimeStep () { return dT; }
    double getTowerRadius () { return m_TowerRadius; }
    double getTowerDistance () { return m_TowerDistance; }
    double getTowerDrag () { return m_TowerDrag; }
    double getHubHeight () { return m_HubHeight; }
    double getFirstWakeRowLength () { return m_firstWakeRowLength; }
    double getFullWakeRevolutions () { return m_FullWakeRevs; }
    double getFineWakeRevolutions () { return m_FineWakeRevs; }
    double getMaxWakeRevolutions () { return m_MaxWakeAge; }
    double getVortexTimeOffset () { return m_VortexTimeOffset; }
    double getEpsilon () { return m_Epsilon; }
    double getRelaxFactor () { return m_RelaxFactor; }
    double getMaxIterations () { return m_MaxIterations; }
    double getThinFactor () { return m_Thinfactor; }
    double getCutOffRadius () { return m_CutOffRadius; }
    double getKinVisc () { return m_KinViscosity; }
    double getTurbVisc () { return m_TurbulentViscosity; }
    double getDensity () { return m_Density; }
    double getNumberTimesteps () { return double(m_NumTimesteps); }
    double getAziDisc () { return m_AngularDisc; }


    bool hasLLTResults () { return !m_results.isEmpty(); }


private:

    void FillVariableList(bool isVawt);

    double m_TowerDrag, m_TowerRadius, m_TowerDistance;


    QLLTSimulation ();


    int num_blades; //Anzal der blades

    int m_shownBladeSection;
    int m_shownTimeIndex;

    CBlade *m_pBladeList[50];                        // The array that stores the geometry of all blades of the current rotor

    QVector <CPanel *> m_BladePanel;                // the blade panel array
    QVector <CPanel *> m_WakePanel;            // the reference current wake panel array

    QVector <VortexLine *> m_BladeLine;                  // rotor line array
    QVector <VortexLine *> m_WakeLine;              // the array stores wake line vortices

    QVector <VortexNode > TempWakeNode;

    QVector <std::vector <VortexNode *> > TempConnectionNode;


    QVector <VortexNode *> m_BladeNode;           // the blade node array
    QVector <VortexNode *> m_WakeNode;            // the wake node array

    CVector m_StoreWakeInfluence[200];  // vector that stores the wake influence during iteration for gamma on blade

//    CVector m_Streamline[VLMMAXMATSIZE*100];

    QStringList m_availableVariables;

    QVector< QVector <double> >  m_results;



    QLLTModule *m_module;
    WindField *m_WindField;
    CBlade *m_Blade;
    double m_HubHeight;
    double m_KinViscosity;
    double m_TurbulentViscosity;
    double m_VortexTimeOffset;
    int m_nWakeColumn;
    bool m_bShed;
    bool m_bGround;
    bool m_bTrail;
    bool m_bWakeLine;
    double m_firstWakeRowLength;
    double m_RelaxFactor;
    double m_Density;
    int m_InterpolationFactor;
    int m_Thinfactor;
    double m_FullWakeRevs;
    double TSR;
    double m_MaxWakeAge; //maximum wake length in rotor diameters
    double m_AngularDisc;
    double m_FineWakeRevs;
    bool m_bIncludeTower;
    bool m_bPrescribed;
    bool m_bTurbConvection;


    CVector HRotorAxis, VRotorAxis;
    double m_YawAngle, Yaw_Old;
    double m_TeetherAngle;
    double m_InflowAngle;

    double m_PitchAngle, Pitch_Old;


    //gl parameters
    bool m_bGLPlotWakeNodes;
    bool m_bGLPlotWakeLines;
    bool m_bGLPlotAllBlades;



    int m_NumTimesteps; //number of timesteps
    double dT; //size of timesteps in [s]

    int iterations;
    int m_MaxIterations;

    int m_ActualTimeStep; //aktuelle Anzahl an verstrichenen Zeitschritten


    double m_ActualTime;
    double m_CutOffRadius;
    double m_MinGammaFrac;

    bool m_bGlChanged;
    bool m_bWakeRollUp;
    bool m_bisVawt;

    CVector V_in;

    bool m_bAllConverged; // flag  falls Gamma auf wing-station konvergiert (default: false)


    double m_Omega; // angular velocity
    double m_Epsilon;

    VortexNode* IsNode(CVector &Pt);
    void createHAWTTurbineBlades(CBlade *blade);
    void createVAWTTurbineBlades(CBlade *blade);
    void CreateElements(CSurface *pSurface);
    void InitializePanels();
    void GuessInitialVortexDistribution();
    void AddFirstWakeRow();
    VortexNode *IsWakeNode(CVector &Pt);
    void GammaBoundFixedPointIteration();
    void CalcBladePanelVelocities();
    void KuttaCondition();
    void WakeConcentration();
    void ConvectWakeNodes();
    void InterpolateWakeSpeeds();
    void CalcAoA();
    CVector BiotSavart_InductEval(CVector EvalPt, bool prescribed = false, bool indWing = false, int tempID = 0);
    void ReadPanelLiftCoef();
    void CalcNewGamma();
    void PitchBlades();
    void RotateHAWTRotorGeometry();
    void RotateVAWTRotorGeometry();
    void RotateGeom();
    void AddNewWakeRow();
    void GammaConvergenceCheck();
    void InterpolateDeltaGammaOnWakeNodes();
    void AddFirstWakeLines();
    void AddNewWakeLines();
    void render();
    void AssignGammaToWingLines();
    CVector GetFreeStream(CVector EvalPt);
    void AppendResults();


    CVector TowerInfluence(CVector EvalPt, CVector V_ref);


    void GLCreateRotorMesh();
    void GLCreateWakeMesh();
    void GLDrawStreamlines();
    void GLDrawVelocityField();

    //simulation results //
    QVector <double> CP;
    QVector <double> angPos;
    QVector <double> CT;
    QVector <double> AoA;
    QVector <double> Time;


public slots:
    void OnStartAnalysis(void);

signals:
    void geomChanged();



};

#endif // QLLTSIMULATION_H
