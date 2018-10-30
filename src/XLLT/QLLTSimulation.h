/****************************************************************************

    QLLTSimulation Class
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

#ifndef QLLTSIMULATION_H
#define QLLTSIMULATION_H

#include <QObject>
#include "../Objects/CVector.h"
#include "../Objects/CVectorf.h"
#include "VortexNode.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../StorableObject.h"
#include "DummyLine.h"


class WindField;
class VortexLine;
class CBlade;
class CPanel;
class CSurface;
class QLLTModule;


class QLLTSimulation : public StorableObject, public ShowAsGraphInterface
{
    friend class QBladeCMDApplication;

	Q_OBJECT
	
public:
    QLLTSimulation ();

    QLLTSimulation(QString name,
                   bool bisVawt,
                   WindField *usedWindField,
                   double timeStep,
                   double numTimeStep,
                   double aziDisc,
                   CBlade *usedRotor,
                   double rotorSpeed,
                   double rotorYaw,
                   double rotorshaftTilt,
                   double yroll,
                   double xroll,
                   double horinflowAngle,
                   double vertinflowSpeed,
                   double turbVisc,
                   double airDens,
                   double kinVisc,
                   double firstWakeRowLength,
                   double minGammaFrac,
                   double fullWakeRevs,
                   double fineWakeRevs,
                   double maxWakeAge,
                   double vortexTimeOffset,
                   double epsilon,
                   double relaxFactor,
                   double maxIter,
                   double thinFactor,
                   double z_leishman,
                   bool bIncludeTower,
                   double towerDrag,
                   double towerTopRadius,
                   double towerBottomRadius,
                   double towerDist,
                   bool bGround,
                   double hubHeight,
                   double towerHeight,
                   bool bWakeRollup,
                   bool bShed,
                   bool bTrail,
                   bool turbConvection,
                   bool storeWake,
                   double inflowSpeed,
                   QList<QList<double> > simParameters,
                   QList<QString> fileHeader,
                   QString fileName,
                   int windInputType,
                   double TStart,
                   int DecFactor,
                   bool isStandStill,
                   double shearCoeff,
                   bool vortexParticle,
                   bool predcorr,
                   bool pc2b,
                   int discType,
                   int numPanels,
                   double coneAngle,
                   bool upWind,
                   bool himmelskamp,
                   double azistart,
                   double momInertia,
                   double initRot,
                   bool isStartup,
                   bool ds,
                   double dsTf,
                   double dsTp,
                   double dsTv, double dsTvl,
                   double boundpos,
                   double aoapos,
                   bool adaptive,
                   bool evalCenter,
                   bool vortexLift,
                   double maxStretch,
                   int wakecoarsening,
                   int wakeCounter,
                   bool ispowerlaw,
                   bool includeStrain,
                   int numStrutPanels,
                   bool circStrut,
                   double refHeight);

    ~QLLTSimulation();

    void Copy(QLLTSimulation * sim);
	static QStringList prepareMissingObjectMessage();

    //virtual public functions
    void serialize ();
    void restorePointers ();
    static QLLTSimulation* newBySerialize ();
    NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // returns NULL if var n.a.
    QStringList getAvailableVariables (NewGraph::GraphType graphType);
    QString getObjectName () { return m_objectName; }

    //public functions
    WindField* getUsedWindField () { return m_WindField; }
    CBlade* getUsedBlade () { return m_Blade; }
    bool getAdaptive () { return m_bAdaptive; }
    bool getIncludeTower () { return m_bIncludeTower; }
    bool getIncludeGround () { return m_bGround; }
    bool getIncludeHimmelskamp () { return m_bHimmelskamp; }
    bool getShed () { return m_bShed; }
    bool getTrail () { return m_bTrail; }
    bool getPrescribed () { return m_dummy; }
    bool getTurbConvection () { return m_bTurbConvection; }
    bool getStoreWake () { return m_bStoreWake; }
    bool getWakeRollup () { return m_bWakeRollUp; }
    double getRunTime () { return init_dT*double(m_numTimesteps); }
    double getRotorSpeed () { return init_Omega * 60 / 2 / PI; }
    double getYawAngle () { if (m_windInputType != SIMFILE) return init_YawAngle; else return 0; }
    double getShaftTiltAngle () { return init_ShaftTiltAngle; }
    double getHorInflowAngle () { if (m_windInputType == CONSTANT) return init_HorInflowAngle; else return 0;}
    double getvertinflowSpeed () { if (m_windInputType == CONSTANT) return init_VertInflowSpeed; else return 0;}
    double getTimeStep () { return init_dT; }
    double getTowerTopRadius () { return m_TowerTopRadius; }
    double getTowerBottomRadius () { return m_TowerBottomRadius; }
    double getOverhang () { return m_RotorOverhang; }
    double getTowerDrag () { return m_TowerDrag; }
    double getHubHeight () { return m_HubHeight; }
    double getTowerHeight () { return m_TowerHeight; }
    double getFirstWakeRowLength () { return m_firstWakeRowLength; }
    double getFullWakeRevolutions () { return m_FullWakeRevs; }
    double getFineWakeRevolutions () { return m_FineWakeRevs; }
    double getMaxWakeRevolutions () { return m_MaxWakeAge; }
    double getVortexTimeOffset () { return m_VortexTimeOffset; }
    double getEpsilon () { return m_Epsilon; }
    double getRelaxFactor () { return m_RelaxFactor; }
    double getMaxIterations () { return m_MaxIterations; }
    double getThinFactor () { return m_Thinfactor; }
    double get_z_Leishman () { return m_z_leishman; }
    double getKinVisc () { return m_KinViscosity; }
    double getTurbVisc () { return m_TurbulentViscosity; }
    double getStretch () { return m_maxStretch; }
    int getWakeCoarsening() { return m_Nth_WakeStep; }
    bool getIncludeStrain() { return m_bIncludeStrain; }
    QVector < QVector<double> > getConvergenceProblems(){ return m_unconvergedPositions; }


    double getDensity () { return m_Density; }
    double getNumberTimesteps () { return double(m_numTimesteps); }
    double getAziDisc () { return init_AngularDisc; }
    double getIsVawt () { return m_bisVawt; }
    double getYRoll () { return init_PlatformPitchAngleY; }
    double getXRoll () { return init_PlatformRollAngleX; }
    double getTStart () { return m_TStart; }
    double getDecFactor () { return m_DecFactor; }
    double isStandStill () { return m_bisStandStill; }
    void setShownBladeSection (double shownBladeSection) { m_shownBladeSection = shownBladeSection; }
    double getShownBladeSection () { return m_shownBladeSection; }
    void setShownTime(double shownTime);
    double getShownTime () { if(m_shownTimeIndex >= 0) return m_results.at(0)->at(m_shownTimeIndex); else return 0; }
    int getShownTimeIndex () { return m_shownTimeIndex; }
    void setShownTimeIndex (int shownTimeIndex) { m_shownTimeIndex = shownTimeIndex; }
    int getfromBlade() {return m_numBlades; }
    int getNumTimestepsComputed () { return m_results.at(0)->size(); }
    int getWindInputType () { return m_windInputType; }
    double getAoAPos(){ return m_AoAPos; }
    double getBoundVortexPos(){ return m_BoundVortexPos; }
    double getInflowSpeed () { return init_InflowSpeed; }
    double getRefHeight () { return m_refHeight; }
    QList< QList<double> > getSimParameters () { return m_SimParameters; }
    QList< QString > getFileHeader () { return m_FileHeader; }
    QString getFileName () { return m_FileName; }
    bool isRunning() { return m_bisRunning ;}
    bool hasStoredData () { return !m_BladeOutputData.isEmpty(); }  // NM TODO this is wrong! Simulation is not certainly finished.
    QVector< QVector <double> *> getResultsArray () { return m_results; }
    void StopRequested(){ m_bStopRequested = true; }
    void drawGL();
    void callGLLists();
    void drawOverpaint(QPainter &painter);
    double getShearCoeff() { if (m_windInputType == CONSTANT) return m_ShearCoeff; else return 0; }
    void setCanContinue(bool canContinue) { m_bContinue = canContinue; }
    bool getCanContinue() { return m_bContinue; }
    int getCurrentTimeStep() { return m_currentTimeStep; }
    double getGammaFrac() { return m_MinGammaFrac; }
    bool getisVortexParticle() { return m_bVortexParticle; }
    bool getisPC() { return m_bPredictorCorrector; }
    bool getEvalPoint() { return m_bisCenterEval; }
    int getNumPanels() { return m_numPanels; }
    int getDiscType() { return m_discType; }
    double getConeAngle() { return m_ConeAngle; }
    bool getisUpWind() { return m_bUpWind; }
    double getAziStart(){ return m_aziStart; }
    double getInitRot(){ return m_initialRot; }
    double getMomInertia(){ return m_momInertia; }
    bool getisStartup(){ return m_bisStartup; }
    bool getIncludeDS () { return m_bisDynStall; }
    double getTf () { return m_Tf; }
    double getTp () { return m_Tp; }
    double getTv () { return m_Tv; }
    double getTvl () { return m_Tvl; }
    bool getisPC2B () { return m_bisPC2B; }
    bool getisVortexLift () { return m_bisVortexLift; }
    QList< QList< DummyLine> > * getVizLines(){ return &m_savedBladeVizLines; }
    bool wasAborted(){ return m_Abort; }
    int getWakeCounter(){ return m_WakeCounter; }
    bool getIsPowerLaw(){ return m_bisPowerLaw; }
    int getNumStrutPanels(){ return m_numStrutPanels; }
    bool getCircStrut(){ return m_bCircStruts; }


    void setDT(double dt){ dT = dt; }
    void setOmega(double omega){ m_Omega = omega; }

    void setForceReRender() {m_bforceReRender = true; }

    QList<QList <CVector> > m_savedCoordinateSystems;

    void InitMultiThreading();

private:

    //private variables
    bool m_bforceReRender;
    bool m_bAllConverged;
    bool m_bGlChanged;
    bool m_bGLPlotWakeNodes;
    bool m_bGLPlotWakeLinesShed;
    bool m_bGLPlotWakeLinesTrail;
    bool m_bGLPlotSingleBlade;
    bool m_bGLPlotRotor;
    bool m_bGLPlotTurbineModel;
    bool m_bGLPlotCoordinates;
    bool m_bGLColorAge;
    bool m_bGLColorStrength;
    bool m_bStopRequested;
    bool m_bisRunning;
    bool m_bContinue;
    bool m_bStopReplay;
    bool m_bisStartup;
    bool m_bisDynStall;
    bool m_bAdaptive;
    bool m_bisPowerLaw;
    bool m_bIncludeStrain;

    double m_Tv;
    double m_Tf;
    double m_Tp;
    double m_Tvl;
    double m_HorShear, m_VertShear, m_LinVertShear, m_GustSpeed; //aerodyn wnd file params
    double m_aziStart, m_initialRot, m_momInertia;
    double m_AoAPos, m_BoundVortexPos;
    double m_refHeight;

    int m_Nth_WakeStep;
    int m_WakeCounter;

    int m_GROUP_SIZE;

    CVector V_in;

    CVector VFoot, VHub, VAxis, VTop;
    QList<CVector> VBOrigins, VBAxes;

    QList <QList <VortexNode *> > TempConnectionNode;
    QList <CPanel *> m_BladePanel;
    QList <VortexLine *> m_BladeLine;
    QList <VortexLine *> m_WakeLine;
    QList <VortexNode *> m_BladeNode;
    QList <VortexNode *> m_WakeNode;
    QList <double > m_WakeParticles;

    // struts
    QList <CPanel *> m_StrutPanel;
    QList <VortexNode *> m_StrutNode;

    QVector< QVector <double> *> m_results;
    QStringList m_availableVariables;
    QStringList m_availableBladeVariables;

    //private variables & arrays to serialize

    QLLTModule *m_module;
    WindField *m_WindField;
    CBlade *m_Blade;
	

    QList<QList <DummyLine> > m_savedWakeLines;
    QList<QList <DummyLine> > m_savedBladeVizLines;
    QList<QList <DummyLine> > m_savedBladeVortexLines;

    QList<QList <double> > m_savedWakeVortexParticles;

    QList<QString> m_FileHeader;

    QList< QList<double> > m_SimParameters;

    CVector m_towerBase[4], m_towerTop[4], m_nacelleYaw[4], m_shaftCoords[4], m_azimuthCoords[4], m_hubCoords[4];
    QList<QList <CVector> > m_bladeCoords, m_conedCoords;

    int m_numBlades;
    int m_shownTimeIndex;
    int m_DecFactor;
    int m_numTimesteps;
    int iterations;
    int m_MaxIterations;
    int m_currentTimeStep;
    int m_InterpolationFactor;
    int m_Thinfactor;
    int m_windInputType;
    int m_nWakeColumn;
    int m_numPanels;
    int m_numStrutPanels;
    int m_discType;

    int m_plotAtTime;

    double m_currentAzimuthalPosition;
    double m_ConeAngle;
    double m_shownBladeSection;
    double m_TowerDrag, m_TowerTopRadius, m_TowerBottomRadius, m_RotorOverhang;
    double m_TStart;
    double m_currentTime;
    double m_z_leishman;
    double m_MinGammaFrac;
    double m_Omega;
    double m_Epsilon;
    double dT;
    CVector m_PlatformTranslation;
    double m_YawAngle;
    double m_ShaftTiltAngle;
    double m_PlatformPitchAngleY;
    double m_PlatformRollAngleX;
    double m_PlatformYawAngleZ;
    double m_HorInflowAngle;
    double m_VertInflowSpeed;
    QVector<double> m_PitchAngle;
    double m_InflowSpeed;
    double m_maxGamma;
    double m_MaxWakeAge;
    double m_AngularDisc;
    double m_FineWakeRevs;
    double m_FullWakeRevs;
    double TSR;
    double m_firstWakeRowLength;
    double m_RelaxFactor;
    double m_Density;
    double m_HubHeight;
    double m_TowerHeight;
    double m_KinViscosity;
    double m_TurbulentViscosity;
    double m_maxStretch;
    double m_VortexTimeOffset;
    double m_ShearCoeff;
    double m_XWindfieldOffset;

    qint64 m_t_overhead;
    qint64 m_t_induction;

    double init_YawAngle;
    double init_ShaftTiltAngle;
    double init_PlatformRollAngleX;
    double init_PlatformPitchAngleY;
    double init_HorInflowAngle;
    double init_VertInflowSpeed;
    double init_dT;
    double init_AngularDisc;
    double init_InflowSpeed;
    double init_Omega;

    bool m_bShed;
    bool m_bGround;
    bool m_bHimmelskamp;
    bool m_bTrail;
    bool m_bisStandStill;
    bool m_bIncludeTower;
    bool m_dummy; ////todo reuse
    bool m_bTurbConvection;
    bool m_bStoreWake;
    bool m_bWakeRollUp;
    bool m_bisVawt;
    bool m_bVortexParticle;
    bool m_bPredictorCorrector;
    bool m_bUpWind;
    bool m_bisOpenMp;
    bool m_bisCenterEval;
    bool m_bisPC2B;
    bool m_bisVortexLift;
    bool m_Abort;
    bool m_noGUI;
    bool m_bCircStruts;

    QString m_FileName;

    //simulation results //

    QVector<QVector<QVector<double> > > m_BladeOutputData;

    //HAWT / VAWT DATA
    QVector <double> m_CP;
    QVector <double> m_CPAV;
    QVector <double> m_CTAV;
    QVector <double> m_CMAV;
    QVector <double> m_PowerAV;
    QVector <double> m_TorqueAV;
    QVector <double> m_ThrustAV;
    QVector <double> m_CT;
    QVector <double> m_CM;
    QVector <double> m_TSR;
    QVector <double> m_Power;
    QVector <double> m_Torque;
    QVector <double> m_Time;
    QVector <double> m_rotSpeed;
    QVector <double> m_ThrustAbs;
    QVector <double> m_ThrustX;
    QVector <double> m_ThrustY;
    QVector <double> m_ThrustZ;
    QVector <double> m_CurrentHHWindSpeed;
    QVector <double> m_CurrentYawAngle;
    QVector <double> m_CurrentHorizontalInflowAngle;
    QVector <double> m_CurrentVerticalInflowSpeed;
    QVector <double> m_Iterations;
    QVector <double> m_CurrentPlatformYaw;
    QVector <double> m_CurrentPlatformRoll;
    QVector <double> m_CurrentPlatformPitch;
    QVector <double> m_CurrentPlatformTranslationX;
    QVector <double> m_CurrentPlatformTranslationY;
    QVector <double> m_CurrentPlatformTranslationZ;
    QVector <double> m_ElapsedSimulationTime;
    QVector <double> m_TimePerTimestep;
    QVector <double> m_CurrentCoreSize;

    QVector < QVector<double> > m_unconvergedPositions;
    QVector <CVector> m_ThrustActingPoint;
    QVector <CVector> m_ThrustVector;

    QVector <QVector<double> > m_angPos;
    QVector <QVector<double> > m_CurrentPitchAngle;
    QVector <QVector<double> > m_BladeTangentialForce;
    QVector <QVector<double> > m_BladeNormalForce;


    //HAWT ONLY
    QVector <QVector<double> > m_RootBendingMomentOOP;
    QVector <QVector<double> > m_RootBendingMomentIP;


    //PRIVATE FUNCTIONS

    double BladeOutputAtSection(QVector<double> output, double section);
    void ClearVortexArrays();
    void CheckWakeNodesForSanity();
    void PrepareOutputVectors();
    VortexNode* IsNode(CVector &Pt, bool strutNodes = false);
    void CreateRotorGeometry();
    void CreateBladePanelsAndNodes();
    void CreateStrutPanelsAndNodes();
    void CreateBladeLLTPanels(CVector m_LA, CVector m_LB, CVector m_TA, CVector m_TB, int blade);
    void CreateStrutLLTPanels(CVector m_LA, CVector m_LB, CVector m_TA, CVector m_TB, int blade, int strut, bool isHub, bool isTip);

    void InitializeBladePanelProperties();
    void InitializeStrutPanelProperties();
    void StoreDSVars();
    VortexNode *IsWakeNode(CVector &Pt);
    void GammaBoundFixedPointIteration();
    void CalcBladePanelVelocities();
    void CalcStrutIterationLoop();
    void CalcStrutAoA();
    void CalcSteadyStrutCoefficients();
    void CalcStrutVelocities();
    void KuttaCondition();
    void LumpWake();
    void TruncateWake();
    void ConvectWake();
    void ConvectWakeOpenMP();
    void PC2BIntegration();
    void UpdateWakeLines();
    void ConvectWakeNoRollup();
    void CalcAoA();
    void drawText(QPainter &painter);
    void drawLegend(QPainter &painter, bool redblue, int component);
    CVector getRelativeBladeVelocity(int ID);
    CVector getRelativeStrutVelocity(int ID);
    CVector CorrespondingAxisPoint( CVector Point, CVector Line1, CVector Line2);
    CVector BiotSavartInduction(CVector EvalPt, bool indWing = false, CPanel *panel = NULL, bool GetShedInduction = false, bool bladeOnly = false);
    CVector BiotSavartInductionFromSave(CVector EvalPt, int timeStep);
    CVector BiotSavartKernel(CVector r1, CVector r2, float Gamma, float coreSizeSquared);
    void CalcSteadyPanelCoefficients();
    void CalcDynamicBladeCoefficients();
    void CalcDynamicStrutCoefficients();
    void CalcDynamicStall(QList<double> parameters, CPanel *panel, bool isStrut, double length = 0, double tsr = 0, int numStrut = 0);
    void CalcBladeCirculation();
    void CalcStrutCirculation();

    void AbortSimulation();
    void UpdateBladePanels();
    void UpdateStrutPanels();
    void UpdateRotorGeometry();
    void InitializePanelPositions();
    void UpdateHAWTCoordinates();
    void UpdateStrutCoordinates();
    void UpdateVAWTCoordinates();
    void ClearOutputArrays();
    void GammaConvergenceCheck();
    void AddFirstWake();
    void AddNewWake();
    void AssignGammaToWingLines();
    CVector getFreeStream(CVector EvalPt, double time = -1);
    CVector getMeanFreeStream(CVector EvalPt);
    void storeOutput();
    void storeGeometry();
    void GLDrawSimulationScene(bool replay = false);
    void LoadRotorCoords(bool replay);
    void DrawCoordinateSystems();
    void CalcVAWTResults();
    void CalcHAWTResults();
    CVector CalcTowerInfluence (CVector EvalPt, CVector V_ref, int timestep = -1);
    void ResetSimulation();
    void UpdateSimulationParameters(double time);
    void RotorSpeedup();
    void SetBoundaryConditions();

public slots:
    void onStartAnalysis();
    void VortexSplitting();
    void onStartReplay();
    void onStopReplay() { m_bStopReplay = true; }

    void onComputeCutPlane(QLLTCutPlane *plane, int timestep);
    void onComputeVelVolume(CVector*** positions, CVector*** velocities, int XSTART, int XEND, int YR, int ZR, int timestep, double time);
    void ComputeCutPlaneVelocitiesOpenMP(QLLTCutPlane *plane, int timestep);
    void ComputeVolumeVelocitiesOpenMP(CVector*** positions, CVector*** velocities, int XSTART, int XEND, int YR, int ZR, int timestep, double time);
    void lockStores();
    void unlockStores();

signals:
    void geomChanged();
    void updateProgress(int i);
    void updateGraphs();
};

#endif // QLLTSIMULATION_H
