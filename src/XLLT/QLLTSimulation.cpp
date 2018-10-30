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

#include "QLLTSimulation.h"
#include "../GLWidget.h"
#include <QDebug>
#include <QtOpenGL>
#include "../GlobalFunctions.h"
#include "../MainFrame.h"
#include "../Store.h"
#include "../Graph/NewCurve.h"
#include "../XBEM/Blade.h"
#include "../XUnsteadyBEM/WindField.h"
#include "QLLTModule.h"
#include "QLLTDock.h"
#include "VortexLine.h"
#include "../Serializer.h"
#include "QLLTToolBar.h"
#include "../ColorManager.h"


QLLTSimulation::QLLTSimulation()
    : StorableObject ("< no name >"),
      ShowAsGraphInterface (false)
{
    m_module = g_QLLTModule;
    m_shownBladeSection = 1;
    m_shownTimeIndex = 0;
    m_bStopRequested = false;
    m_bGlChanged = true;
    m_bisRunning = false;
    m_bContinue = false;
    m_bHimmelskamp = false;
    m_noGUI = false;
    V_in.Set(0,0,0);
    m_maxGamma = 0;
    m_bforceReRender = false;
    m_bisDynStall = false;
    m_plotAtTime = -1;
    m_currentTimeStep= -1;
    m_bisStartup = false;
    m_initialRot = 1;
    m_momInertia = 10;
    m_Tf = 3;
    m_Tp = 1.7;
    m_Tv = 2;
    m_Tvl = 11;
    m_AoAPos = 0.75;
    m_BoundVortexPos = 0.75;
    m_Abort = false;
    m_bisPowerLaw = true;
    m_bIncludeStrain = true;
    m_bCircStruts = false;
    m_numStrutPanels = 5;
    m_refHeight = 30;
}

QLLTSimulation::QLLTSimulation(QString name,
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
                               QList< QList<double> > simParameters,
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
                               double dsTv,
                               double dsTvl,
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
                               double refHeight)
    : StorableObject (name)
{
    if (usedWindField) addParent(usedWindField);
    addParent(usedRotor);

    m_bCircStruts = circStrut;
    m_numStrutPanels = numStrutPanels;

    m_refHeight = refHeight;
    m_maxGamma = 0;
    m_bisVawt = bisVawt;
    m_module = g_QLLTModule;
    m_bContinue = false;
    V_in.Set(0,0,0);
    m_discType = discType;
    m_numPanels = numPanels;
    m_ConeAngle = coneAngle;
    m_bUpWind = upWind;
    m_bHimmelskamp = himmelskamp;
    m_plotAtTime = -1;
    m_currentTimeStep = -1;
    m_aziStart = azistart;
    m_initialRot = initRot;
    m_bisStartup = isStartup;
    m_momInertia = momInertia;
    m_BoundVortexPos = boundpos;
    m_AoAPos = aoapos;
    m_bisCenterEval = evalCenter;
    m_bisVortexLift = vortexLift;
    m_maxStretch = maxStretch;
    m_Nth_WakeStep = wakecoarsening;
    m_Abort = false;
    m_WakeCounter = wakeCounter;
    m_noGUI = false;
    m_bisPowerLaw = ispowerlaw;
    m_bIncludeStrain = includeStrain;



    m_bisDynStall = ds;
    m_Tf = dsTf;
    m_Tp = dsTp;
    m_Tv = dsTv;
    m_Tvl = dsTvl;

    m_FileHeader = fileHeader;

    if (m_bisVawt) m_pen.setColor(g_colorManager.getLeastUsedColor(&g_QLLTVAWTSimulationStore));
    else m_pen.setColor(g_colorManager.getLeastUsedColor(&g_QLLTHAWTSimulationStore));


    m_Blade = usedRotor;
    m_WindField = usedWindField;
    m_bVortexParticle = vortexParticle;
    m_bPredictorCorrector = predcorr;
    m_bisPC2B = pc2b;
    if (m_bisPC2B) m_bPredictorCorrector = true;
    m_bAdaptive = adaptive;

    m_windInputType = windInputType;
    m_SimParameters = simParameters;
    m_bGlChanged = true;
    m_FileName = fileName;
    m_bisStandStill = isStandStill;
    m_bisRunning = false;
    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);
    m_shownBladeSection = 1;
    m_shownTimeIndex = 0;
    m_currentTimeStep = -1;
    m_numBlades = m_Blade->m_blades;
    m_ShearCoeff = shearCoeff;

    /// Main Settings for Op-Point

    init_YawAngle = rotorYaw;
    init_ShaftTiltAngle = rotorshaftTilt;
    init_PlatformRollAngleX = xroll;
    init_PlatformPitchAngleY = yroll;
    init_HorInflowAngle = horinflowAngle;
    init_VertInflowSpeed = vertinflowSpeed;
    init_dT = timeStep;
    init_AngularDisc = aziDisc;
    init_InflowSpeed = inflowSpeed;
    init_Omega = rotorSpeed / 60.0 * 2 * PI;

    m_YawAngle = init_YawAngle;
    m_ShaftTiltAngle = init_ShaftTiltAngle;
    m_PlatformRollAngleX = init_PlatformRollAngleX;
    m_PlatformPitchAngleY = init_PlatformPitchAngleY;
    m_HorInflowAngle = init_HorInflowAngle;
    m_VertInflowSpeed = init_VertInflowSpeed;
    dT = init_dT;
    m_AngularDisc = init_AngularDisc;
    m_InflowSpeed = init_InflowSpeed;
    m_Omega = init_Omega;

    m_PlatformYawAngleZ = 0;
    m_PlatformTranslation.Set(0,0,0);
    m_numTimesteps = numTimeStep;
    m_currentTime = 0;
    m_bTurbConvection = turbConvection;
    m_bStoreWake = storeWake;
    m_TStart = TStart;
    m_DecFactor = DecFactor;

    /// Additional Modells / Tower Shadow / Prescribed Wake / Goundeffects

    m_bIncludeTower = bIncludeTower;
    m_HubHeight = hubHeight;
    if (m_bisVawt) m_TowerHeight = towerHeight;
    if (!m_bisVawt) m_TowerHeight = hubHeight;


    m_bGround = bGround;

    /// Algorithm Parameters

    m_firstWakeRowLength = firstWakeRowLength;
    m_FullWakeRevs = fullWakeRevs;
    m_FineWakeRevs = fineWakeRevs;
    m_MaxWakeAge = maxWakeAge;
    m_VortexTimeOffset = vortexTimeOffset;
    m_Epsilon = epsilon;
    m_RelaxFactor = relaxFactor;
    m_MaxIterations = maxIter;
    m_Thinfactor = thinFactor;
    m_MinGammaFrac = minGammaFrac;
    m_z_leishman = z_leishman;
    m_bShed  = bShed;
    m_bTrail = bTrail;
    m_bWakeRollUp = bWakeRollup;

    m_TowerDrag = towerDrag;
    m_TowerTopRadius = towerTopRadius;
    m_TowerBottomRadius = towerBottomRadius;
    if (!m_bisVawt){
    m_RotorOverhang = towerDist;
    }
    else m_RotorOverhang = 0;

    ///Environmental Variables
    m_KinViscosity = kinVisc;
    m_TurbulentViscosity = turbVisc;
    m_Density = airDens;
    m_currentAzimuthalPosition = m_aziStart;

    for (int i=0;i<m_numBlades;i++){
        QVector<double> dummy;
        m_PitchAngle.append(0);
        m_angPos.append(dummy);
        m_CurrentPitchAngle.append(dummy);
        m_RootBendingMomentIP.append(dummy);
        m_RootBendingMomentOOP.append(dummy);
        m_BladeTangentialForce.append(dummy);
        m_BladeNormalForce.append(dummy);
    }   

    CreateRotorGeometry();
    PrepareOutputVectors();
}

QLLTSimulation::~QLLTSimulation() {
        ClearVortexArrays();
		ClearOutputArrays();
}

void QLLTSimulation::Copy(QLLTSimulation * sim){

    m_bCircStruts = sim->m_bCircStruts;
    m_numStrutPanels = sim->m_numStrutPanels;
    m_bisVawt = sim->m_bisVawt;
    m_module = sim->m_module;
    m_bContinue = false;
    V_in = sim->V_in;
    m_discType = sim->m_discType;
    m_numPanels = sim->m_numPanels;
    m_ConeAngle = sim->m_ConeAngle;
    m_bUpWind = sim->m_bUpWind;
    m_bHimmelskamp = sim->m_bHimmelskamp;
    m_plotAtTime = sim->m_plotAtTime;
    m_currentTimeStep = sim->m_currentTimeStep;
    m_aziStart = sim->m_aziStart;
    m_initialRot = sim->m_initialRot;
    m_bisStartup = sim->m_bisStartup;
    m_momInertia = sim->m_momInertia;
    m_BoundVortexPos = sim->m_BoundVortexPos;
    m_AoAPos = sim->m_AoAPos;
    m_bisCenterEval = sim->m_bisCenterEval;
    m_bisVortexLift = sim->m_bisVortexLift;
    m_maxStretch = sim->m_maxStretch;
    m_Nth_WakeStep = sim->m_Nth_WakeStep;
    m_Abort = sim->m_Abort;
    m_WakeCounter = sim->m_WakeCounter;
    m_noGUI = sim->m_noGUI;
    m_refHeight = sim->m_refHeight;

    m_bisDynStall = sim->m_bisDynStall;
    m_Tf = sim->m_Tf;
    m_Tp = sim->m_Tp;
    m_Tv = sim->m_Tv;
    m_Tvl = sim->m_Tvl;

    m_FileHeader = sim->m_FileHeader;

    m_Blade = sim->m_Blade;
    m_WindField = sim->m_WindField;
    m_bVortexParticle = sim->m_bVortexParticle;
    m_bPredictorCorrector = sim->m_bPredictorCorrector;
    m_bisPC2B = sim->m_bisPC2B;
    m_bAdaptive = sim->m_bAdaptive;

    m_windInputType = sim->m_windInputType;
    m_SimParameters = sim->m_SimParameters;
    m_bGlChanged = sim->m_bGlChanged;
    m_FileName = sim->m_FileName;
    m_bisStandStill = sim->m_bisStandStill;
    m_bisRunning = sim->m_bisRunning;
    m_numBlades = sim->m_numBlades;
    m_ShearCoeff = sim->m_ShearCoeff;
    m_bisPowerLaw = sim->m_bisPowerLaw;
    m_bIncludeStrain = sim->m_bIncludeStrain;

    ///OUTPUT Things

    m_pen = sim->m_pen;
    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);
    m_shownBladeSection = 1;
    m_shownTimeIndex = 0;
    m_currentTimeStep = -1;

    /// Main Settings for Op-Point

    init_YawAngle = sim->init_YawAngle;
    init_ShaftTiltAngle = sim->init_ShaftTiltAngle;
    init_PlatformRollAngleX = sim->init_PlatformRollAngleX;
    init_PlatformPitchAngleY = sim->init_PlatformPitchAngleY;
    init_HorInflowAngle = sim->init_HorInflowAngle;
    init_VertInflowSpeed = sim->init_VertInflowSpeed;
    init_dT = sim->init_dT;
    init_AngularDisc = sim->init_AngularDisc;
    init_InflowSpeed = sim->init_InflowSpeed;
    init_Omega = sim->init_Omega;

    m_YawAngle = init_YawAngle;
    m_ShaftTiltAngle = init_ShaftTiltAngle;
    m_PlatformRollAngleX = init_PlatformRollAngleX;
    m_PlatformPitchAngleY = init_PlatformPitchAngleY;
    m_HorInflowAngle = init_HorInflowAngle;
    m_VertInflowSpeed = init_VertInflowSpeed;
    dT = init_dT;
    m_AngularDisc = init_AngularDisc;
    m_InflowSpeed = init_InflowSpeed;
    m_Omega = init_Omega;

    m_PlatformYawAngleZ = sim->m_PlatformYawAngleZ;
    m_PlatformTranslation = sim->m_PlatformTranslation;
    m_numTimesteps = sim->m_numTimesteps;
    m_currentTime = 0;
    m_bTurbConvection = sim->m_bTurbConvection;
    m_bStoreWake = sim->m_bStoreWake;
    m_TStart = sim->m_TStart;
    m_DecFactor = sim->m_DecFactor;

    /// Additional Modells / Tower Shadow / Prescribed Wake / Goundeffects

    m_bIncludeTower = sim->m_bIncludeTower;
    m_HubHeight = sim->m_HubHeight;
    m_TowerHeight = sim->m_TowerHeight;
    m_bGround = sim->m_bGround;

    /// Algorithm Parameters

    m_firstWakeRowLength = sim->m_firstWakeRowLength;
    m_FullWakeRevs = sim->m_FullWakeRevs;
    m_FineWakeRevs = sim->m_FineWakeRevs;
    m_MaxWakeAge = sim->m_MaxWakeAge;
    m_VortexTimeOffset = sim->m_VortexTimeOffset;
    m_Epsilon = sim->m_Epsilon;
    m_RelaxFactor = sim->m_RelaxFactor;
    m_MaxIterations = sim->m_MaxIterations;
    m_Thinfactor = sim->m_Thinfactor;
    m_MinGammaFrac = sim->m_MinGammaFrac;
    m_z_leishman = sim->m_z_leishman;
    m_bShed  = sim->m_bShed;
    m_bTrail = sim->m_bTrail;
    m_bWakeRollUp = sim->m_bWakeRollUp;

    m_TowerDrag = sim->m_TowerDrag;
    m_TowerTopRadius = sim->m_TowerTopRadius;
    m_TowerBottomRadius = sim->m_TowerBottomRadius;
    m_RotorOverhang = sim->m_RotorOverhang;


    ///Environmental Variables
    m_KinViscosity = sim->m_KinViscosity;
    m_TurbulentViscosity = sim->m_TurbulentViscosity;
    m_Density = sim->m_Density;
    m_currentAzimuthalPosition = sim->m_currentAzimuthalPosition;

    setSingleParent(sim->getParent());
    if (m_WindField) addParent(m_WindField);


    for (int i=0;i<m_numBlades;i++){
        QVector<double> dummy;
        m_PitchAngle.append(0);
        m_angPos.append(dummy);
        m_CurrentPitchAngle.append(dummy);
        m_RootBendingMomentIP.append(dummy);
        m_RootBendingMomentOOP.append(dummy);
        m_BladeTangentialForce.append(dummy);
        m_BladeNormalForce.append(dummy);
    }

    CreateRotorGeometry();
    PrepareOutputVectors();
}

void QLLTSimulation::AddFirstWake()
{

    VortexNode *L, *T;
    QVector <VortexNode*> leadingNodes, trailingNodes;

    if (!m_bShed && !m_bTrail) return;

    for (int i =0;i<m_BladeLine.size();i++){

    // create new nodes
    T = new VortexNode;
    L = new VortexNode;
    *T = *m_BladeLine[i]->pT;
    *L = *T;
    T->Rotate(m_hubCoords[3], m_hubCoords[0],-m_Omega*dT*m_Nth_WakeStep*(m_firstWakeRowLength)*180/PI);
    *T += getMeanFreeStream(m_hubCoords[3])*dT*m_Nth_WakeStep*m_firstWakeRowLength;

    T->fromTimestep = m_currentTimeStep-m_Nth_WakeStep;
    T->fromStation = m_BladeLine[i]->fromStation;
    trailingNodes.push_back(T);
    L->fromTimestep = m_currentTimeStep;
    L->fromStation = m_BladeLine[i]->fromStation;
    leadingNodes.push_back(L);
    }

    if (m_bShed){
        for (int i=0;i<m_BladeLine.size();i++){
            if (!m_BladeLine.at(i)->isTip){
                VortexLine *line = new VortexLine;
                line->fromStation = m_BladeLine.at(i)->fromStation;
                line->isHub = false;
                line->isTip = false;
                line->rightPanel = m_BladeLine.at(i)->rightPanel;
                line->includeStrain = m_bIncludeStrain;
                line->fromTimestep = m_currentTimeStep;
                line->fromTime = m_currentTime;
                line->fromRevolution = m_currentAzimuthalPosition;
                line->isShed = true;
                line->m_KinViscosity = m_KinViscosity;
                line->m_TurbulentViscosity = m_TurbulentViscosity;
                line->m_VortexTimeOffset = m_VortexTimeOffset;
                line->m_Lines = &m_WakeLine;
                line->m_Nodes = &m_WakeNode;
                L = trailingNodes.at(i);
                T = trailingNodes.at(i+1);
                line->pL = L;
                line->pT = T;
                L->attachLine(line);
                T->attachLine(line);
                line->InitVortexSize();
                line->InitLength();
                m_WakeLine.push_back(line);
            }
        }
    }

    if (m_bTrail){
        //create new trailing vortex line

        for (int i =0;i<m_BladeLine.size();i++){
            L = leadingNodes.at(i);
            T = trailingNodes.at(i);
            VortexLine *line = new VortexLine;
            line->fromStation = m_BladeLine.at(i)->fromStation;
            line->isHub = m_BladeLine.at(i)->isHub;
            line->isTip = m_BladeLine.at(i)->isTip;
            line->includeStrain = m_bIncludeStrain;
            line->fromTimestep = m_currentTimeStep;
            line->fromTime = m_currentTime;
            line->fromLine = m_BladeLine.at(i);
            line->fromRevolution = m_currentAzimuthalPosition;
            line->m_KinViscosity = m_KinViscosity;
            line->m_TurbulentViscosity = m_TurbulentViscosity;
            line->m_VortexTimeOffset = m_VortexTimeOffset;
            line->isTrailing = true;
            line->m_Lines = &m_WakeLine;
            line->m_Nodes = &m_WakeNode;
            line->pL = L;
            line->pT = T;
            L->attachLine(line);
            T->attachLine(line);
            line->InitVortexSize();
            line->InitLength();
            m_WakeLine.push_back(line);
        }
    }


    for (int i=0;i<trailingNodes.size();i++){
        m_WakeNode.push_back(trailingNodes.at(i));
    }

    for (int i=0;i<leadingNodes.size();i++){
        m_WakeNode.push_back(leadingNodes.at(i));
    }
}

void QLLTSimulation::AddNewWake(){
//create the new shed wake line
    VortexNode *L = NULL, *T = NULL;
    VortexLine *line = NULL;

    if (m_bShed){
        int pos = 0;
        for (int i=m_WakeNode.size()-m_BladeLine.size();i<m_WakeNode.size();i++){
            if (!m_BladeLine.at(pos)->isTip){
                VortexLine *line = new VortexLine;
                line->fromStation = m_BladeLine.at(pos)->fromStation;
                line->rightPanel = m_BladeLine.at(pos)->rightPanel;
                line->includeStrain = m_bIncludeStrain;
                line->fromTimestep = m_currentTimeStep;
                line->fromTime = m_currentTime;
                line->fromRevolution = m_currentAzimuthalPosition;
                line->isShed = true;
                line->m_Lines = &m_WakeLine;
                line->m_Nodes = &m_WakeNode;
                line->m_KinViscosity = m_KinViscosity;
                line->m_TurbulentViscosity = m_TurbulentViscosity;
                line->m_VortexTimeOffset = m_VortexTimeOffset;
                L = m_WakeNode.at(i);
                T = m_WakeNode.at(i+1);
                line->pL = L;
                line->pT = T;
                L->attachLine(line);
                T->attachLine(line);
                line->InitVortexSize();
                line->InitLength();
                m_WakeLine.push_back(line);
            }
            pos++;
        }
    }

    for (int i=0;i<m_BladeLine.size();i++)
    {
        //search for the corresponding downstream node
        for (int j=0;j<m_WakeLine.size();j++){
            if (m_bTrail && m_WakeLine.at(j)->isTrailing && (m_WakeLine.at(j)->fromTimestep == m_currentTimeStep-m_Nth_WakeStep) && (m_WakeLine.at(j)->fromStation == m_BladeLine[i]->fromStation)){
                 T = m_WakeLine.at(j)->pL;
            }
        }

        if (m_bTrail || m_bShed){
            //create new leading node
            L = new VortexNode;
            *L = *m_BladeLine.at(i)->pT;
            L->fromTimestep = m_currentTimeStep;
            L->fromStation  = m_BladeLine.at(i)->fromStation;
            m_WakeNode.push_back(L);
        }

        //add trailing vorticity
        if (m_bTrail){
            line = new VortexLine;
            line->pL = L;
            line->pT = T;
            line->fromStation = m_BladeLine.at(i)->fromStation;
            line->isHub = m_BladeLine.at(i)->isHub;
            line->isTip = m_BladeLine.at(i)->isTip;
            line->includeStrain = m_bIncludeStrain;
            line->fromTimestep = m_currentTimeStep;
            line->fromTime = m_currentTime;
            line->fromLine = m_BladeLine.at(i);
            line->fromRevolution = m_currentAzimuthalPosition;
            line->m_KinViscosity = m_KinViscosity;
            line->m_TurbulentViscosity = m_TurbulentViscosity;
            line->m_VortexTimeOffset = m_VortexTimeOffset;
            line->isTrailing = true;
            line->m_Lines = &m_WakeLine;
            line->m_Nodes = &m_WakeNode;
            line->InitVortexSize();
            line->InitLength();
            //connect node to line vortex
            T->attachLine(line);
            L->attachLine(line);
            m_WakeLine.push_back(line);
        }
    }

}

void QLLTSimulation::AssignGammaToWingLines(){

    for (int i=0;i<m_BladeLine.size();i++){
        // this is just for the visualization of the circ strength along the blade
        m_BladeLine.at(i)->VizGamma = 0;
        if (m_BladeLine.at(i)->leftPanel && m_BladeLine.at(i)->rightPanel) m_BladeLine.at(i)->VizGamma += (m_BladeLine.at(i)->leftPanel->m_Gamma+m_BladeLine.at(i)->rightPanel->m_Gamma)/2;
        else if (m_BladeLine.at(i)->rightPanel) m_BladeLine.at(i)->VizGamma -= m_BladeLine.at(i)->rightPanel->m_Gamma;
        else if (m_BladeLine.at(i)->leftPanel) m_BladeLine.at(i)->VizGamma += m_BladeLine.at(i)->leftPanel->m_Gamma;
    }

    if ((m_currentTimeStep)%m_Nth_WakeStep == 0 || m_currentTimeStep == 0){
        // only store bladeline circulation during wakestep
        for (int i=0;i<m_BladeLine.size();i++){
            m_BladeLine.at(i)->Gamma = 0;
            if (m_BladeLine.at(i)->leftPanel) m_BladeLine.at(i)->Gamma += m_BladeLine.at(i)->leftPanel->m_Gamma;
            if (m_BladeLine.at(i)->rightPanel) m_BladeLine.at(i)->Gamma -= m_BladeLine.at(i)->rightPanel->m_Gamma;
        }
        for (int i=0;i < m_BladeLine.size();i++){
            if (m_BladeLine[i]->Gamma > m_maxGamma) m_maxGamma = m_BladeLine[i]->Gamma;
        }
    }

}

CVector QLLTSimulation::BiotSavartInduction (CVector EvalPt, bool indWing, CPanel *panel, bool GetShedInduction, bool bladeOnly) {

    CVector VGamma_total(0,0,0);
    VortexNode R1, R2, R3, R4;  // EvalPt-P1
    /////////////
    //Wing Panels
    /////////////
    /// \brief coreSizeSquared
    double a = 1.25643;
    double coreSizeSquared = 4*a*m_TurbulentViscosity*m_KinViscosity*m_VortexTimeOffset; // core size for bound vortices

    for (int ID = 0; ID < m_BladePanel.size(); ++ID) {
            if (m_BladePanel[ID]->m_Gamma != 0){
            //calc vector P1234 -> EvalPt on WingPanels
            R1 = EvalPt - m_BladePanel[ID]->VortPtA;  // EvalPt-P1
            R2 = EvalPt - m_BladePanel[ID]->VortPtB;  // EvalPt-P2
            R3 = EvalPt - *m_BladePanel[ID]->pTB;  // EvalPt-P3
            R4 = EvalPt - *m_BladePanel[ID]->pTA;  // EvalPt-P4
    //        /////////////////////////////////////
    //        //calc induction of wing Panels
    //        /////////////////////////////////////
            //Induction Vortexelement 1-2 on WingPanels
            if (m_bShed || indWing){
                CVector ind = BiotSavartKernel(R1,R2,m_BladePanel[ID]->m_Gamma,coreSizeSquared);
                VGamma_total += ind;
            }
            if (m_bShed) {
                if (m_WakeLine.size() == 0) {
                    //Induction Vortexelement 3-4 on WingPanels
                    CVector ind = BiotSavartKernel(R3,R4,m_BladePanel[ID]->m_Gamma,coreSizeSquared);
                    VGamma_total += ind;
                 }
            }
            if (m_bTrail || indWing){
                //Induction Vortexelement 4-1 on WingPanels
            VGamma_total += BiotSavartKernel(R4,R1,m_BladePanel[ID]->m_Gamma,coreSizeSquared);
            //Induction Vortexelement 2-3 on WingPanels
            VGamma_total += BiotSavartKernel(R2,R3,m_BladePanel[ID]->m_Gamma,coreSizeSquared);}

        }
    }

    if (m_bGround) {
        for (int ID = 0; ID < m_BladePanel.size(); ++ID) {
            if (m_BladePanel[ID]->m_Gamma != 0){
                //calc vector P1234 -> EvalPt on WingPanels
                R1 = EvalPt - CVector(m_BladePanel[ID]->VortPtA.x, m_BladePanel[ID]->VortPtA.y, -m_BladePanel[ID]->VortPtA.z);
                R2 = EvalPt - CVector(m_BladePanel[ID]->VortPtB.x, m_BladePanel[ID]->VortPtB.y, -m_BladePanel[ID]->VortPtB.z);
                R3 = EvalPt - CVector(m_BladePanel[ID]->pTB->x, m_BladePanel[ID]->pTB->y, -m_BladePanel[ID]->pTB->z);
                R4 = EvalPt - CVector(m_BladePanel[ID]->pTA->x, m_BladePanel[ID]->pTA->y, -m_BladePanel[ID]->pTA->z);
                /////////////////////////////////////
                //calc induction of wing Panels mirrored at the ground
                /////////////////////////////////////
                //Induction Vortexelement 1-2 on WingPanels
                if (m_bShed || indWing){
                    CVector ind = BiotSavartKernel(R1,R2,-m_BladePanel[ID]->m_Gamma,coreSizeSquared);
                    VGamma_total += ind;
                }
                if (m_bShed){
                    if (m_WakeLine.size() == 0){
                        //Induction Vortexelement 3-4 on WingPanels
                        CVector ind = BiotSavartKernel(R3,R4,-m_BladePanel[ID]->m_Gamma,coreSizeSquared);
                        VGamma_total += ind;
                    }
                }
                //Induction Vortexelement 4-1 on WingPanels
                if (m_bTrail || indWing){
                VGamma_total += BiotSavartKernel(R4,R1,-m_BladePanel[ID]->m_Gamma,coreSizeSquared);
                //Induction Vortexelement 2-3 on WingPanels
                VGamma_total += BiotSavartKernel(R2,R3,-m_BladePanel[ID]->m_Gamma,coreSizeSquared);}

            }
        }
    }

    /////////////
    //Strut Panels
    /////////////

    if (m_bCircStruts){
        for (int ID = 0; ID < m_StrutPanel.size(); ++ID) {
                if (m_StrutPanel[ID]->m_Gamma != 0){
                //calc vector P1234 -> EvalPt on WingPanels
                R1 = EvalPt - m_StrutPanel[ID]->VortPtA;  // EvalPt-P1
                R2 = EvalPt - m_StrutPanel[ID]->VortPtB;  // EvalPt-P2
                R3 = EvalPt - *m_StrutPanel[ID]->pTB;  // EvalPt-P3
                R4 = EvalPt - *m_StrutPanel[ID]->pTA;  // EvalPt-P4
        //        /////////////////////////////////////
        //        //calc induction of wing Panels
        //        /////////////////////////////////////
                //Induction Vortexelement 1-2 on WingPanels
                if (m_bShed || indWing){
                    CVector ind = BiotSavartKernel(R1,R2,m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                    VGamma_total += ind;
                }
                if (m_bShed) {
                    if (m_WakeLine.size() == 0) {
                        //Induction Vortexelement 3-4 on WingPanels
                        CVector ind = BiotSavartKernel(R3,R4,m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                        VGamma_total += ind;
                     }
                }
                if (m_bTrail || indWing){
                    //Induction Vortexelement 4-1 on WingPanels
                VGamma_total += BiotSavartKernel(R4,R1,m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                //Induction Vortexelement 2-3 on WingPanels
                VGamma_total += BiotSavartKernel(R2,R3,m_StrutPanel[ID]->m_Gamma,coreSizeSquared);}

            }
        }

        if (m_bGround) {
            for (int ID = 0; ID < m_StrutPanel.size(); ++ID) {
                if (m_StrutPanel[ID]->m_Gamma != 0){
                    //calc vector P1234 -> EvalPt on WingPanels
                    R1 = EvalPt - CVector(m_StrutPanel[ID]->VortPtA.x, m_StrutPanel[ID]->VortPtA.y, -m_StrutPanel[ID]->VortPtA.z);
                    R2 = EvalPt - CVector(m_StrutPanel[ID]->VortPtB.x, m_StrutPanel[ID]->VortPtB.y, -m_StrutPanel[ID]->VortPtB.z);
                    R3 = EvalPt - CVector(m_StrutPanel[ID]->pTB->x, m_StrutPanel[ID]->pTB->y, -m_StrutPanel[ID]->pTB->z);
                    R4 = EvalPt - CVector(m_StrutPanel[ID]->pTA->x, m_StrutPanel[ID]->pTA->y, -m_StrutPanel[ID]->pTA->z);
                    /////////////////////////////////////
                    //calc induction of wing Panels mirrored at the ground
                    /////////////////////////////////////
                    //Induction Vortexelement 1-2 on WingPanels
                    if (m_bShed || indWing){
                        CVector ind = BiotSavartKernel(R1,R2,-m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                        VGamma_total += ind;
                    }
                    if (m_bShed){
                        if (m_WakeLine.size() == 0){
                            //Induction Vortexelement 3-4 on WingPanels
                            CVector ind = BiotSavartKernel(R3,R4,-m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                            VGamma_total += ind;
                        }
                    }
                    //Induction Vortexelement 4-1 on WingPanels
                    if (m_bTrail || indWing){
                    VGamma_total += BiotSavartKernel(R4,R1,-m_StrutPanel[ID]->m_Gamma,coreSizeSquared);
                    //Induction Vortexelement 2-3 on WingPanels
                    VGamma_total += BiotSavartKernel(R2,R3,-m_StrutPanel[ID]->m_Gamma,coreSizeSquared);}

                }
            }
        }
    }


/////////////
//Wake Panels
/////////////
    // if wake induction is computed for the wing panels this happens only during the first iteration, the induction is then stored in the m_storeWakeInfluence array for the timestep
    if ((indWing && iterations > 0) || bladeOnly) return VGamma_total;

    if (indWing) {
        panel->m_Store_Wake.Set(0, 0, 0);
        panel->m_V_Shed.Set(0, 0, 0);
    }

        CVector gamma_cont;
        for(int ID=0;ID<m_WakeLine.size();ID++){
            if (m_WakeLine[ID]->Gamma != 0){
            //calc vector P1/P2 -> EvalPt on WakeLines
            R1 = EvalPt - *m_WakeLine[ID]->pL; //EvalPt-P1
            R2 = EvalPt - *m_WakeLine[ID]->pT; //EvalPt-P2
            /////////////////////////////////////
            //calc induction of wake Lines
            /////////////////////////////////////
            gamma_cont = BiotSavartKernel(R1,R2,m_WakeLine[ID]->Gamma,m_WakeLine[ID]->coreSizeSquared);
            if (GetShedInduction && m_WakeLine.at(ID)->isShed && (panel == m_WakeLine.at(ID)->rightPanel) && ((m_currentTimeStep - m_WakeLine[ID]->fromTimestep)*dT*panel->m_V_relative.VAbs())<8*panel->chord) panel->m_V_Shed += gamma_cont;

            VGamma_total += gamma_cont;
            if (indWing) panel->m_Store_Wake += gamma_cont;
            }
        }
        if (m_bGround){
            for(int ID=0;ID<m_WakeLine.size();ID++){
                if (m_WakeLine[ID]->Gamma != 0){
                //calc vector P1/P2 -> EvalPt on WakeLines
                R1 = EvalPt - CVector(m_WakeLine[ID]->pL->x, m_WakeLine[ID]->pL->y, -m_WakeLine[ID]->pL->z);
                R2 = EvalPt - CVector(m_WakeLine[ID]->pT->x, m_WakeLine[ID]->pT->y, -m_WakeLine[ID]->pT->z);
                /////////////////////////////////////
                //calc induction of wake Lines
                /////////////////////////////////////
                gamma_cont =  BiotSavartKernel(R1,R2,-m_WakeLine[ID]->Gamma,m_WakeLine[ID]->coreSizeSquared);
                if (GetShedInduction && m_WakeLine.at(ID)->isShed && (panel == m_WakeLine.at(ID)->rightPanel) && ((m_currentTimeStep - m_WakeLine[ID]->fromTimestep)*dT*panel->m_V_relative.VAbs())<8*panel->chord) panel->m_V_Shed += gamma_cont;
                VGamma_total += gamma_cont;
                if (indWing) panel->m_Store_Wake += gamma_cont ;
                }
            }
        }

    return VGamma_total;
}

CVector QLLTSimulation::BiotSavartInductionFromSave (CVector EvalPt, int timeStep) {

    CVector VGamma_total(0,0,0);
    VortexNode R1, R2;  // EvalPt-P1
            /////////////
            //Blade Panels
            /////////////
            for(int ID=0;ID<m_savedBladeVortexLines.at(timeStep).size();ID++){
                if (m_savedBladeVortexLines[timeStep][ID].Gamma != 0){
                R1 = EvalPt - CVector(m_savedBladeVortexLines[timeStep][ID].Lx,m_savedBladeVortexLines[timeStep][ID].Ly,m_savedBladeVortexLines[timeStep][ID].Lz); //EvalPt-P1
                R2 = EvalPt - CVector(m_savedBladeVortexLines[timeStep][ID].Tx,m_savedBladeVortexLines[timeStep][ID].Ty,m_savedBladeVortexLines[timeStep][ID].Tz); //EvalPt-P2
                VGamma_total += BiotSavartKernel(R1,R2,m_savedBladeVortexLines[timeStep][ID].Gamma,m_savedBladeVortexLines[timeStep][ID].CoreSizeSquared);
                }
            }
            if (m_bGround){
            for(int ID=0;ID<m_savedBladeVortexLines.at(timeStep).size();ID++){
                if (m_savedBladeVortexLines[timeStep][ID].Gamma != 0){
                R1 = EvalPt - CVector(m_savedBladeVortexLines[timeStep][ID].Lx,m_savedBladeVortexLines[timeStep][ID].Ly,-m_savedBladeVortexLines[timeStep][ID].Lz); //EvalPt-P1
                R2 = EvalPt - CVector(m_savedBladeVortexLines[timeStep][ID].Tx,m_savedBladeVortexLines[timeStep][ID].Ty,-m_savedBladeVortexLines[timeStep][ID].Tz); //EvalPt-P2
                VGamma_total += BiotSavartKernel(R1,R2,-m_savedBladeVortexLines[timeStep][ID].Gamma,m_savedBladeVortexLines[timeStep][ID].CoreSizeSquared);
                    }
                }
            }
        /////////////
        //Wake Panels
        /////////////
        for(int ID=0;ID<m_savedWakeLines.at(timeStep).size();ID++){
            if (m_savedWakeLines[timeStep][ID].Gamma != 0){
            R1 = EvalPt - CVector(m_savedWakeLines[timeStep][ID].Lx,m_savedWakeLines[timeStep][ID].Ly,m_savedWakeLines[timeStep][ID].Lz); //EvalPt-P1
            R2 = EvalPt - CVector(m_savedWakeLines[timeStep][ID].Tx,m_savedWakeLines[timeStep][ID].Ty,m_savedWakeLines[timeStep][ID].Tz); //EvalPt-P2
            VGamma_total += BiotSavartKernel(R1,R2,m_savedWakeLines[timeStep][ID].Gamma,m_savedWakeLines[timeStep][ID].CoreSizeSquared);
            }
        }
        if (m_bGround){
        for(int ID=0;ID<m_savedWakeLines.at(timeStep).size();ID++){
            if (m_savedWakeLines[timeStep][ID].Gamma != 0){
            R1 = EvalPt - CVector(m_savedWakeLines[timeStep][ID].Lx,m_savedWakeLines[timeStep][ID].Ly,-m_savedWakeLines[timeStep][ID].Lz); //EvalPt-P1
            R2 = EvalPt - CVector(m_savedWakeLines[timeStep][ID].Tx,m_savedWakeLines[timeStep][ID].Ty,-m_savedWakeLines[timeStep][ID].Tz); //EvalPt-P2
            VGamma_total += BiotSavartKernel(R1,R2,-m_savedWakeLines[timeStep][ID].Gamma,m_savedWakeLines[timeStep][ID].CoreSizeSquared);
                }
            }
        }
    return VGamma_total;
}

CVector QLLTSimulation::BiotSavartKernel(CVector r1, CVector r2, float Gamma, float coreSizeSquared){
    CVectorf R1 = CVectorf(r1.x,r1.y,r1.z);
    CVectorf R2 = CVectorf(r2.x,r2.y,r2.z);
    float R1Vabs = R1.VAbs();
    float R2Vabs = R2.VAbs();
    float R1R2 = R1Vabs*R2Vabs;
    float R1R2_skal = R1.dot(R2);
    float R1_plus_R2 = R1.VAbs() + R2.VAbs();
    CVectorf R1R2_cross = R1*R2;
    if (CVectorf(R1+R2).VAbs()*2/(R1_plus_R2) < 0.00001)return CVector(0,0,0);
    if (m_z_leishman != -1){
    if (R1R2+R1R2_skal < 0.00001) return CVector(0,0,0);
    float dist = fabs(R1Vabs - R2Vabs);
    if (dist == 0) dist = 0.00001;
    float h = R1R2_cross.VAbs()/dist;
    float Kv = pow(h,2)/pow(pow(coreSizeSquared,m_z_leishman-2)+pow(h,2*m_z_leishman),1/m_z_leishman);
    CVectorf ret = R1R2_cross*Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal))*Kv;
    if (std::isnan((ret.VAbs()))) return CVector (0,0,0);
    else return CVector(ret.x, ret.y, ret.z);
    }
    else{
        CVectorf ret = R1R2_cross*Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+coreSizeSquared);
        if (std::isnan((ret.VAbs()))) return CVector (0,0,0);
        else return CVector(ret.x, ret.y, ret.z);    }
}

double QLLTSimulation::BladeOutputAtSection(QVector<double> output, double section){
    //radius at (6)
    if (!m_BladeOutputData.size()) return -1;
    QVector<double> positions = m_BladeOutputData.at(0).at(5*m_numBlades);
    double radius = m_Blade->getRotorRadius()*section;
    if (radius <= positions.at(0)) return output.at(0);
    else if (radius >= positions.at(positions.size()-1)) return output.at(positions.size()-1);
    else{
        for (int i=0;i<positions.size()-1;i++){
            if (positions.at(i) < radius && radius < positions.at(i+1)){
                return output.at(i) + (output.at(i+1)-output.at(i)) * (radius-positions.at(i))/(positions.at(i+1)-positions.at(i));
            }
        }
    }
    return -1;
}

void QLLTSimulation::CalcAoA()
{
    for (int i=0; i<m_BladePanel.size();i++){

        m_BladePanel[i]->m_AoA = atan((m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a3))/(m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1)))*180/PI;
        m_BladePanel[i]->m_AoAQS = atan((CVector(m_BladePanel[i]->m_V_total-m_BladePanel[i]->m_V_Shed).dot(m_BladePanel[i]->a3))/(CVector(m_BladePanel[i]->m_V_total-m_BladePanel[i]->m_V_Shed).dot(m_BladePanel[i]->a1)))*180/PI;

        if (m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1) < 0 ) m_BladePanel[i]->m_AoA += 180;
        if (m_BladePanel[i]->m_AoA < -180.0) m_BladePanel[i]->m_AoA+=360.0;
        if (m_BladePanel[i]->m_AoA >  180.0) m_BladePanel[i]->m_AoA-=360.0;

        if (CVector(m_BladePanel[i]->m_V_total-m_BladePanel[i]->m_V_Shed).dot(m_BladePanel[i]->a1) < 0 ) m_BladePanel[i]->m_AoAQS += 180;
        if (m_BladePanel[i]->m_AoAQS < -180.0) m_BladePanel[i]->m_AoAQS+=360.0;
        if (m_BladePanel[i]->m_AoAQS >  180.0) m_BladePanel[i]->m_AoAQS-=360.0;

    }
}

void QLLTSimulation::CalcStrutIterationLoop(){
    CalcStrutVelocities();
    CalcStrutAoA();
    CalcSteadyStrutCoefficients();
    CalcStrutCirculation();
}

void QLLTSimulation::CalcStrutAoA(){
    for (int i=0; i<m_StrutPanel.size();i++){

        m_StrutPanel[i]->m_AoA = atan((m_StrutPanel[i]->m_V_total.dot(m_StrutPanel[i]->a3))/(m_StrutPanel[i]->m_V_total.dot(m_StrutPanel[i]->a1)))*180/PI;
        m_StrutPanel[i]->m_AoAQS = atan((CVector(m_StrutPanel[i]->m_V_total-m_StrutPanel[i]->m_V_Shed).dot(m_StrutPanel[i]->a3))/(CVector(m_StrutPanel[i]->m_V_total-m_StrutPanel[i]->m_V_Shed).dot(m_StrutPanel[i]->a1)))*180/PI;

        if (m_StrutPanel[i]->m_V_total.dot(m_StrutPanel[i]->a1) < 0 ) m_StrutPanel[i]->m_AoA += 180;
        if (m_StrutPanel[i]->m_AoA < -180.0) m_StrutPanel[i]->m_AoA+=360.0;
        if (m_StrutPanel[i]->m_AoA >  180.0) m_StrutPanel[i]->m_AoA-=360.0;

        if (CVector(m_StrutPanel[i]->m_V_total-m_StrutPanel[i]->m_V_Shed).dot(m_StrutPanel[i]->a1) < 0 ) m_StrutPanel[i]->m_AoAQS += 180;
        if (m_StrutPanel[i]->m_AoAQS < -180.0) m_StrutPanel[i]->m_AoAQS+=360.0;
        if (m_StrutPanel[i]->m_AoAQS >  180.0) m_StrutPanel[i]->m_AoAQS-=360.0;
    }
}

void QLLTSimulation::CalcSteadyStrutCoefficients(){

    for (int i=0;i<m_StrutPanel.size();i++){

        double angle = m_StrutPanel[i]->m_AoA;
        double reynolds = m_StrutPanel[i]->chord*m_StrutPanel[i]->m_V_inPlane.VAbs()/m_KinViscosity;

//        if (m_Strut->m_bIsInverted) angle *= -1;

        QList<double> parameters = m_Blade->getStrutParameters(m_StrutPanel[i]->fromStrut,angle,reynolds);

        m_StrutPanel[i]->m_CL = parameters.at(0);
        m_StrutPanel[i]->m_CD = parameters.at(1);
        m_StrutPanel[i]->m_RE = parameters.at(2);


//        if (m_Blade->m_bIsInverted) m_StrutPanel[i]->m_CL *= -1;

        m_StrutPanel[i]->LiftVector = CVector(m_StrutPanel[i]->a3*m_StrutPanel[i]->m_CL);
        m_StrutPanel[i]->LiftVector.RotateN(m_StrutPanel[i]->a2,m_StrutPanel[i]->m_AoA);
        m_StrutPanel[i]->DragVector = CVector(m_StrutPanel[i]->a3*m_StrutPanel[i]->m_CD);
        m_StrutPanel[i]->DragVector.RotateN(m_StrutPanel[i]->a2,-90+m_StrutPanel[i]->m_AoA);
        m_StrutPanel[i]->LiftDragVector = m_StrutPanel[i]->LiftVector + m_StrutPanel[i]->DragVector;
    }
}

void QLLTSimulation::CalcStrutVelocities(){
#pragma omp parallel default (none)
{
    #pragma omp for
    for(int ID = 0; ID < m_StrutPanel.size(); ++ID) {

        //calcs m_V_induced for every WingPanel in WingPanel Control Point      

        if ((m_currentTimeStep)%m_Nth_WakeStep == 0) {
            m_StrutPanel[ID]->m_V_induced = BiotSavartInduction(m_StrutPanel[ID]->CtrlPt, true, m_StrutPanel[ID], true);
            if (iterations > 0 && m_bCircStruts) m_StrutPanel[ID]->m_V_induced += m_StrutPanel[ID]->m_Store_Wake;
        }
        else{
            m_StrutPanel[ID]->m_V_induced = BiotSavartInduction(m_StrutPanel[ID]->CtrlPt, true, m_StrutPanel[ID], false, true);
            m_StrutPanel[ID]->m_V_induced += m_StrutPanel[ID]->m_oldInducedVelocities.at(m_StrutPanel[ID]->m_oldInducedVelocities.size()-1);

            if (m_currentTimeStep>10*m_Nth_WakeStep){
                //3 step backwards Adams-Bashforth extrapolation of the velocity induced from wake
                CVector slope = (m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-1]-m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-2])/m_Nth_WakeStep;
                m_StrutPanel[ID]->m_V_induced += slope * ((m_currentTimeStep)%m_Nth_WakeStep)*23/12;

                slope = (m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-2]-m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-3])/m_Nth_WakeStep;
                m_StrutPanel[ID]->m_V_induced -= slope * ((m_currentTimeStep)%m_Nth_WakeStep)*4/3;

                slope = (m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-3]-m_StrutPanel[ID]->m_oldInducedVelocities[m_StrutPanel[ID]->m_oldInducedVelocities.size()-4])/m_Nth_WakeStep;
                m_StrutPanel[ID]->m_V_induced += slope * ((m_currentTimeStep)%m_Nth_WakeStep)*5/12;
            }
        }

        m_StrutPanel[ID]->m_V_relative = getRelativeStrutVelocity(ID);

        m_StrutPanel[ID]->m_V_sampled = getFreeStream(m_StrutPanel[ID]->CtrlPt);

        m_StrutPanel[ID]->m_V_total = CalcTowerInfluence(m_StrutPanel[ID]->CtrlPt, m_StrutPanel[ID]->m_V_sampled +
                                                      m_StrutPanel[ID]->m_V_induced) - m_StrutPanel[ID]->m_V_relative;

        m_StrutPanel[ID]->m_V_inPlane = CVector(m_StrutPanel[ID]->m_V_total.dot(m_StrutPanel[ID]->a1), m_StrutPanel[ID]->m_V_total.dot(m_StrutPanel[ID]->a3), 0);

    }
}
}

void QLLTSimulation::CalcBladePanelVelocities() {

    #pragma omp parallel default (none)
    {
        #pragma omp for
        for(int ID = 0; ID < m_BladePanel.size(); ++ID) {
            //calcs m_V_induced for every WingPanel in WingPanel Control Point
            if ((m_currentTimeStep)%m_Nth_WakeStep == 0) {
                m_BladePanel[ID]->m_V_induced = BiotSavartInduction(m_BladePanel[ID]->CtrlPt, true, m_BladePanel[ID], true);
                if (iterations > 0) m_BladePanel[ID]->m_V_induced += m_BladePanel[ID]->m_Store_Wake;
            }
            else{
                m_BladePanel[ID]->m_V_induced = BiotSavartInduction(m_BladePanel[ID]->CtrlPt, true, m_BladePanel[ID], false, true);
                m_BladePanel[ID]->m_V_induced += m_BladePanel[ID]->m_oldInducedVelocities.at(m_BladePanel[ID]->m_oldInducedVelocities.size()-1);

                if (m_currentTimeStep>10*m_Nth_WakeStep){
                    //3 step backwards Adams-Bashforth extrapolation of the velocity induced from wake
                    CVector slope = (m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-1]-m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-2])/m_Nth_WakeStep;
                    m_BladePanel[ID]->m_V_induced += slope * ((m_currentTimeStep)%m_Nth_WakeStep)*23/12;

                    slope = (m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-2]-m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-3])/m_Nth_WakeStep;
                    m_BladePanel[ID]->m_V_induced -= slope * ((m_currentTimeStep)%m_Nth_WakeStep)*4/3;

                    slope = (m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-3]-m_BladePanel[ID]->m_oldInducedVelocities[m_BladePanel[ID]->m_oldInducedVelocities.size()-4])/m_Nth_WakeStep;
                    m_BladePanel[ID]->m_V_induced += slope * ((m_currentTimeStep)%m_Nth_WakeStep)*5/12;
                }
            }

            m_BladePanel[ID]->m_V_sampled = getFreeStream(m_BladePanel[ID]->CtrlPt);
            m_BladePanel[ID]->m_V_relative = getRelativeBladeVelocity(ID);
            //m_V_total for new Gamma calc
            m_BladePanel[ID]->m_V_total =  CalcTowerInfluence(m_BladePanel[ID]->CtrlPt, m_BladePanel[ID]->m_V_sampled + m_BladePanel[ID]->m_V_induced) - m_BladePanel[ID]->m_V_relative;
            m_BladePanel[ID]->m_V_inPlane = CVector(m_BladePanel[ID]->m_V_total.dot(m_BladePanel[ID]->a1), m_BladePanel[ID]->m_V_total.dot(m_BladePanel[ID]->a3), 0);
            m_BladePanel[ID]->m_V_tower = m_BladePanel[ID]->m_V_total - (m_BladePanel[ID]->m_V_sampled + m_BladePanel[ID]->m_V_induced - m_BladePanel[ID]->m_V_relative);
        }
    }
}

void QLLTSimulation::CalcDynamicStall(QList<double> parameters, CPanel *panel, bool isStrut, double length, double tsr, int numStrut){


    // aoa at current and previous timestep
    double aoa_old = panel->m_AoA_old*PI/180;
    if (m_Blade->m_bIsInverted) aoa_old *= -1;
    double aoa_new = panel->m_AoA*PI/180;
    if (m_Blade->m_bIsInverted) aoa_new *= -1;

    double aoa_qs = panel->m_AoAQS*PI/180;
    if (m_Blade->m_bIsInverted) aoa_qs *= -1;

    // pitch rate
    double aoaDot = aoa_new-aoa_old;
    while (aoaDot > PI) aoaDot -= 2*PI;
    while (aoaDot < -PI) aoaDot += 2*PI;
    aoaDot = aoaDot/dT;

    // unsteady velocity
    double U_new = panel->m_V_inPlane.VAbs();

    // effective aoa in degrees
    double angle = panel->m_AoA;
    if (m_Blade->m_bIsInverted) angle *= -1;


//    double Cl = parameters.at(0);
    double CdE = parameters.at(1);
    panel->m_RE = parameters.at(2);
    double ClAttE = parameters.at(3);// circular, attached lift
    double ClSepE = parameters.at(4);
    double FstE = parameters.at(5);
    double chord = parameters.at(6);
    double alphazero = parameters.at(7);
    double slope = parameters.at(8);
    double clMax = parameters.at(10);
    double Cd0 = parameters.at(11);
    double clMin = parameters.at(12);

    // condition for lift generating airfoil
    if (!slope == 0){

    // time constants
    double Tu = chord/(2*U_new);
    double Tf = m_Tf*Tu;
    double Tp = m_Tp*Tu;
    double Tv = m_Tv*Tu;
    double Tvl= m_Tvl;
    if (Tp==0) Tp=0.00001;
    if (Tf==0) Tf=0.00001;
    if (Tv==0) Tv=0.00001;
    if (Tvl==0) Tvl=0.00001;


//    double ClMass = PI*Tu*aoaDot;// added mass effects
    double ClPot = ClAttE /* + ClMass */;  //potential lift

    // dynamic
    double   P[3], Q[3], Q1[3], Q2[3], C[3], I[3], x[3];

    P[0]=1.0/Tp;
    P[1]=1.0/Tf;
    P[2]=1.0/Tv;

    for (int k=0;k<3;k++)
    {
        Q2[k]=panel->Q2[k];
        Q1[k]=panel->Q1[k];
        x[k]=panel->x[k];
    }

    Q[0]=1.0/Tp*ClPot;

    // equiv aoa with same quasi steady lift
    double aoaQ = x[0]/slope + alphazero;

    // this blends the dynamic x[0] curve with the static ClAtt curve for a smooth aoaQ, which was causing problems before the angle 50° is chosen arbitrarily
    if ((panel->m_AoA >50 &&  panel->m_AoA < 130) || (panel->m_AoA < -50 && panel->m_AoA>-130)){
        if (panel->m_AoA > 90 || panel->m_AoA < -90){ aoaQ = ClAttE/(slope*0.8) + alphazero;}
        else aoaQ = ClAttE/slope + alphazero;
    }

    if (panel->m_AoA + alphazero > 90 || panel->m_AoA+ alphazero < -90) aoaQ += 180;

    while (aoaQ < -180.0) aoaQ+=360.0;
    while (aoaQ >  180.0) aoaQ-=360.0;

    if (m_currentTimeStep == 0){
        x[0] = ClAttE;
        aoaDot = 0;
        aoa_qs = aoa_new;
        aoaQ = aoa_new/PI*180;
    }

    // separation point for equiv aoa
    double FstQ;

    if (!isStrut)
        FstQ = m_Blade->getBladeParameters(length,aoaQ,true,panel->chord*panel->m_V_inPlane.VAbs()/m_KinViscosity,m_bHimmelskamp,tsr,m_Blade->m_bisSinglePolar).at(5);
    else
        FstQ = m_Blade->getStrutParameters(numStrut,aoaQ,panel->chord*panel->m_V_inPlane.VAbs()/m_KinViscosity).at(5);

    Q[1] = 1.0/Tf*FstQ;

    for (int k=0; k<2; k++)
    {
        // update decay and increment of state variables
        C[k] = exp(-P[k]*dT);
        I[k] = Q1[k]/P[k]*(1-C[k])+(Q[k]-Q2[k])/(2*dT*P[k])*(dT-1/P[k]*(1-C[k]))+(Q[k]-2*Q1[k]+Q2[k])/(2*dT*dT*P[k])*(dT*dT-2*dT/P[k]+2/(P[k]*P[k])*(1-C[k]));// parabolic assumption

        // update state variables
        // x[0] potential lift coefficient with pressure time lag [-]
        // x[1] dyn. separation point fDyn [-]
        // x[2] LE vortex lift [-]
        x[k] = C[k]*x[k]+I[k];
    }

    // this fixes the creation of NAN in case of very large dT (or low TSR); x[1] could become negative before!
    if (x[1] < 0) x[1] = 0;


    // LE vortex lift
    double Cv, K;//dx,
    //double LEsep = panel->bLEsep;
    double xLEV = panel->xLEV_old;
    double Cvold = panel->Cv_old;


    K=pow(1+sqrt(x[1]),2)/4;
    Cv=ClAttE*(1-K);

//    // method 1: vortex lift increments and decay
//    if (aoaDot>0 && (Cv-Cvold)>0 && alphazero < angle && angle < 50)
//    {
//        C[2] = exp(-P[2]/2*dT);
//        I[2] = (Cv-Cvold)*sqrt(C[2]);
//    }
//    else
//    {
//        C[2] = exp(-P[2]*dT);
//        I[2] = 0;
//    }

    // method 2: vortex lift increments and decay + vortex release and tracking
    // LE vortex
    if (aoaDot>0 && angle>alphazero && angle<50)
    {
        // LE vortex convection
        if (panel->m_CL_old > clMax)
            xLEV=xLEV+0.45/Tu*dT/Tvl;// vortex position increment

        C[2] = exp(-P[2]/2*dT);

        // vortex lift accumulation:
        if (xLEV<1 && (Cv-Cvold)>0)
            I[2] = (Cv-Cvold)*sqrt(C[2]);
        else
            I[2] = 0;

    }
    else if (aoaDot<0 && angle<alphazero && angle>-50)
    {
        // LE vortex convection
        if (panel->m_CL_old < clMin)
            xLEV=xLEV+0.45/Tu*dT/Tvl;// vortex position increment

        C[2] = exp(-P[2]/2*dT);

        // vortex lift accumulation:
        if (xLEV<1 && (Cv-Cvold)<0)
            I[2] = (Cv-Cvold)*sqrt(C[2]);
        else
            I[2] = 0;

    }
    // vortex lift decay
    else
    {
        C[2] = exp(-P[2]*dT);
        I[2] = 0;
        xLEV=0;
    }

    if (m_bisVortexLift) x[2] = C[2]*x[2]+I[2];
    else x[2] = 0;

    panel->xLEV=xLEV;
    panel->Cv=Cv;


    double ClDyn = ClAttE*x[1] + ClSepE*(1-x[1])  + x[2]/*+ ClMass */;// dyn. lift = dyn.circ.lift + non-circ. lift + vortex lift

    double deltaAOA = aoa_qs - aoa_new;
    if (deltaAOA > PI)  deltaAOA -= 2*PI;
    if (deltaAOA < -PI) deltaAOA += 2*PI;

    double CdInd = deltaAOA*ClDyn;
    double CdVisc = (CdE - Cd0)*(0.5*(sqrt(FstE)-sqrt(x[1])) - 0.25*(FstE-x[1]));
    double CdDyn = CdE + CdInd + CdVisc;// dyn. drag = static drag + induced drag + viscous drag

    double CmDyn = 0;//CmE+ClDyn*(ast1-ast2)-0.5*PI*Tu*aoaDot;// 360 extrapolation of moment coefficient missing

    for (int k=0;k<3;k++)
    {
        panel->Q2_new[k]=Q1[k];
        panel->Q1_new[k]=Q[k];
        panel->x_new[k]=x[k];
    }

    panel->m_CL = ClDyn;
    panel->m_CD = CdDyn;
    panel->m_CM = CmDyn;
    panel->aoa_dot = aoaDot;

    }
    else{
        panel->m_CL = 0;
        panel->m_CD = CdE;
        panel->m_CM = 0;
        panel->aoa_dot = aoaDot;
    }


}

void QLLTSimulation::CalcDynamicBladeCoefficients(){
    for (int i=0;i<m_BladePanel.size();i++){

        double angle = m_BladePanel[i]->m_AoA;
        double tsr;


        if (m_Blade->m_bIsInverted) angle *= -1;
        if (!m_bisVawt){
            tsr = m_Omega *m_Blade->getRotorRadius() / getFreeStream(m_hubCoords[3]).VAbs();
        }
        else{
            tsr = m_Omega *m_Blade->m_MaxRadius / getFreeStream(m_hubCoords[3]).VAbs();
        }

        QList<double> parameters = m_Blade->getBladeParameters(m_BladePanel[i]->fromBladelength,angle,true,m_BladePanel[i]->chord*m_BladePanel[i]->m_V_inPlane.VAbs()/m_KinViscosity,m_bHimmelskamp,tsr,m_Blade->m_bisSinglePolar);

        CalcDynamicStall(parameters, m_BladePanel[i], false, m_BladePanel[i]->fromBladelength, tsr);

        m_BladePanel[i]->m_RE = parameters.at(2);

        if (m_Blade->m_bIsInverted) m_BladePanel[i]->m_CL *= -1;

        m_BladePanel[i]->LiftVector = CVector(m_BladePanel[i]->a3*m_BladePanel[i]->m_CL);
        m_BladePanel[i]->LiftVector.RotateN(m_BladePanel[i]->a2,m_BladePanel[i]->m_AoA);
        m_BladePanel[i]->DragVector = CVector(m_BladePanel[i]->a3*m_BladePanel[i]->m_CD);
        m_BladePanel[i]->DragVector.RotateN(m_BladePanel[i]->a2,-90+m_BladePanel[i]->m_AoA);
        m_BladePanel[i]->LiftDragVector = m_BladePanel[i]->LiftVector + m_BladePanel[i]->DragVector;
    }
}


void QLLTSimulation::CalcDynamicStrutCoefficients(){

    if (!m_bCircStruts) return;

    for (int i=0;i<m_StrutPanel.size();i++){

        double angle = m_StrutPanel[i]->m_AoA;
        double reynolds = m_StrutPanel[i]->chord*m_StrutPanel[i]->m_V_inPlane.VAbs()/m_KinViscosity;

//        if (m_Strut->m_bIsInverted) angle *= -1;

        QList<double> parameters = m_Blade->getStrutParameters(m_StrutPanel[i]->fromStrut,angle,reynolds);

        CalcDynamicStall(parameters, m_StrutPanel[i], true);

        m_StrutPanel[i]->m_RE = parameters.at(2);

//        if (m_Blade->m_bIsInverted) m_StrutPanel[i]->m_CL *= -1;

        m_StrutPanel[i]->LiftVector = CVector(m_StrutPanel[i]->a3*m_StrutPanel[i]->m_CL);
        m_StrutPanel[i]->LiftVector.RotateN(m_StrutPanel[i]->a2,m_StrutPanel[i]->m_AoA);
        m_StrutPanel[i]->DragVector = CVector(m_StrutPanel[i]->a3*m_StrutPanel[i]->m_CD);
        m_StrutPanel[i]->DragVector.RotateN(m_StrutPanel[i]->a2,-90+m_StrutPanel[i]->m_AoA);
        m_StrutPanel[i]->LiftDragVector = m_StrutPanel[i]->LiftVector + m_StrutPanel[i]->DragVector;
    }
}



void QLLTSimulation::CalcSteadyPanelCoefficients()
{
    for(int i=0;i<m_BladePanel.size();i++){
        double angle = m_BladePanel[i]->m_AoA;
        double tsr;

        if (m_Blade->m_bIsInverted) angle *= -1;
        if (!m_bisVawt){
            tsr = m_Omega *m_Blade->getRotorRadius() / getFreeStream(m_hubCoords[3]).VAbs();
        }
        else{
            tsr = m_Omega *m_Blade->m_MaxRadius / getFreeStream(m_hubCoords[3]).VAbs();
        }

        QList<double> parameters = m_Blade->getBladeParameters(m_BladePanel[i]->fromBladelength,angle,true,m_BladePanel[i]->chord*m_BladePanel[i]->m_V_inPlane.VAbs()/m_KinViscosity,m_bHimmelskamp,tsr,m_Blade->m_bisSinglePolar);

        m_BladePanel[i]->m_CL = parameters.at(0);
        m_BladePanel[i]->m_CD = parameters.at(1);

        m_BladePanel[i]->m_RE = parameters.at(2);

        if (m_Blade->m_bIsInverted) m_BladePanel[i]->m_CL *= -1;

        m_BladePanel[i]->LiftVector = CVector(m_BladePanel[i]->a3*m_BladePanel[i]->m_CL);
        m_BladePanel[i]->LiftVector.RotateN(m_BladePanel[i]->a2,m_BladePanel[i]->m_AoA);
        m_BladePanel[i]->DragVector = CVector(m_BladePanel[i]->a3*m_BladePanel[i]->m_CD);
        m_BladePanel[i]->DragVector.RotateN(m_BladePanel[i]->a2,-90+m_BladePanel[i]->m_AoA);
        m_BladePanel[i]->LiftDragVector = m_BladePanel[i]->LiftVector + m_BladePanel[i]->DragVector;
    }
}

void QLLTSimulation::CalcHAWTResults(){

    double Cp, Ct, Cn, Cm, power=0, torque=0, totalThrustAbs=0;
    CVector InflowNorm, thrustVec(0,0,0), thrustActingPoint(0,0,0);
    QVector<double> bendingoop, bendingip, azipos, bladetangential, bladenormal;
    QVector<QVector<double> > BladeTimestepData;
    QVector<QVector<double> > CL, CD, RE, VIND, VTOWER, DYNP, VABS, VABSNOIND, CN, CT, AXIND, TANIND, FORCEN, FORCET, AOA, RADIUS, CHORD, CLCD, TWIST, GAMMA, VSAMPLE, VROTATIONAL, DELTA_RE;

    TSR = m_Omega *m_Blade->getRotorRadius() / getFreeStream(m_hubCoords[3]).VAbs();

    if (m_bisStandStill){
        TSR = 0;
        m_Omega = 0;
    }
    QVector<double> dummy;
    for (int m=0;m<m_numBlades;m++){
        bendingoop.append(0);
        bendingip.append(0);
        azipos.append(0);
        bladenormal.append(0);
        bladetangential.append(0);
        CL.append(dummy);
        CD.append(dummy);
        VABS.append(dummy);
        VABSNOIND.append(dummy);
        VSAMPLE.append(dummy);
        VROTATIONAL.append(dummy);
        VIND.append(dummy);
        VTOWER.append(dummy);
        CN.append(dummy);
        CT.append(dummy);
        FORCEN.append(dummy);
        FORCET.append(dummy);
        AOA.append(dummy);
        RADIUS.append(dummy);
        RE.append(dummy);
        AXIND.append(dummy);
        TANIND.append(dummy);
        CHORD.append(dummy);
        CLCD.append(dummy);
        TWIST.append(dummy);
        GAMMA.append(dummy);
        DYNP.append(dummy);
        DELTA_RE.append(dummy);
    }


    for (int k = 0; k < m_BladePanel.size(); ++k) {

        InflowNorm = m_BladePanel[k]->m_V_sampled;
        InflowNorm.Normalize();

        Ct = m_BladePanel[k]->LiftDragVector.dot(m_BladePanel[k]->tangentialVector);
        Cn =  m_BladePanel[k]->LiftDragVector.dot(m_hubCoords[0]);

        torque += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * m_BladePanel[k]->radius * 0.5*m_Density;
        power += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * m_BladePanel[k]->radius * m_Omega * 0.5*m_Density;
        thrustVec += m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5 * m_Density;
        totalThrustAbs += CVector(m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5 * m_Density).VAbs();
        thrustActingPoint += m_BladePanel[k]->CtrlPt*CVector(m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5 * m_Density).VAbs();

        bladenormal[m_BladePanel[k]->fromBlade] += Cn * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5*m_Density;;
        bladetangential[m_BladePanel[k]->fromBlade] += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5*m_Density;;

        double REY = m_BladePanel[k]->chord*m_BladePanel[k]->m_V_inPlane.VAbs()/m_KinViscosity;


        bendingoop[m_BladePanel[k]->fromBlade] += Cn * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5 * m_Density * m_BladePanel[k]->radius;
        bendingip[m_BladePanel[k]->fromBlade] +=  Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->Area * 0.5 * m_Density * m_BladePanel[k]->radius;
        azipos[m_BladePanel[k]->fromBlade] = m_BladePanel[k]->angularPos;
        CL[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CL);
        CD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CD);
        VABS[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_V_inPlane.VAbs());
        VABSNOIND[m_BladePanel[k]->fromBlade].append(CVector(CVector(m_BladePanel[k]->m_V_total-m_BladePanel[k]->m_V_induced).dot(m_BladePanel[k]->a1), CVector(m_BladePanel[k]->m_V_total-m_BladePanel[k]->m_V_induced).dot(m_BladePanel[k]->a3), 0).VAbs());
        VSAMPLE[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_sampled.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_sampled.dot(m_BladePanel[k]->a3), 0).VAbs());
        VROTATIONAL[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_relative.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_relative.dot(m_BladePanel[k]->a3), 0).VAbs());
        VIND[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_induced.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_induced.dot(m_BladePanel[k]->a3), 0).VAbs());
        VTOWER[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_tower.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_tower.dot(m_BladePanel[k]->a3), 0).VAbs());
        CN[m_BladePanel[k]->fromBlade].append(Cn);
        CT[m_BladePanel[k]->fromBlade].append(Ct);
        FORCEN[m_BladePanel[k]->fromBlade].append(Cn * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->chord * 0.5 * m_Density);
        FORCET[m_BladePanel[k]->fromBlade].append(Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->chord * 0.5 * m_Density);
        AOA[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_AoA);
        RADIUS[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->fromBladelength);
        RE[m_BladePanel[k]->fromBlade].append(REY);
        AXIND[m_BladePanel[k]->fromBlade].append(m_hubCoords[0].dot(m_BladePanel[k]->m_V_induced) / m_hubCoords[0].dot(m_BladePanel[k]->m_V_sampled)*(-1));
        TANIND[m_BladePanel[k]->fromBlade].append(-m_BladePanel[k]->tangentialVector.dot(m_BladePanel[k]->m_V_induced) / m_BladePanel[k]->radius / m_Omega - 1);
        CHORD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->chord);
        CLCD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CL/m_BladePanel[k]->m_CD);
        TWIST[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->twistAngle);
        GAMMA[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_Gamma);
        DYNP[m_BladePanel[k]->fromBlade].append(0.5*m_Density*pow(m_BladePanel[k]->m_V_total.VAbs(),2));
        DELTA_RE[m_BladePanel[k]->fromBlade].append(REY-m_BladePanel[k]->m_RE);

    }

    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CL[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CLCD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(GAMMA[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(AOA[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(RADIUS[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CHORD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(TWIST[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CN[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CT[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(FORCEN[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(FORCET[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(DYNP[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VABS[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VABSNOIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VSAMPLE[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VROTATIONAL[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VTOWER[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(AXIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(TANIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(RE[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(DELTA_RE[m]);


    m_BladeOutputData.append(BladeTimestepData);

    Cp = power / (pow(m_Blade->getRotorRadius(),2) * PI * 0.5*m_Density * pow(getMeanFreeStream(m_hubCoords[3]).VAbs(),3));
    Cm = Cp / TSR;
    Ct = thrustVec.VAbs() / (pow(m_Blade->getRotorRadius(),2) * PI * 0.5*m_Density * pow(getMeanFreeStream(m_hubCoords[3]).VAbs(),2));

    m_CP.append(Cp);
    m_CT.append(Ct);
    m_CM.append(Cm);
    m_TSR.append(TSR);
    m_Power.append(power/1000);
    m_Torque.append(torque);
    m_Time.append(m_currentTime);
    m_rotSpeed.append(m_Omega/2/PI*60);
    m_ThrustAbs.append(thrustVec.VAbs());
    m_ThrustVector.append(thrustVec);
    m_ThrustActingPoint.append(thrustActingPoint/totalThrustAbs);
    m_ThrustX.append(thrustVec.x);
    m_ThrustY.append(thrustVec.y);
    m_ThrustZ.append(thrustVec.z);
    m_CurrentHHWindSpeed.append(getFreeStream(m_hubCoords[3]).VAbs());
    m_CurrentYawAngle.append(m_YawAngle);
    m_CurrentHorizontalInflowAngle.append(m_HorInflowAngle);
    m_CurrentVerticalInflowSpeed.append(m_VertInflowSpeed);

    m_CurrentPlatformYaw.append(m_PlatformYawAngleZ);
    m_CurrentPlatformRoll.append(m_PlatformRollAngleX);
    m_CurrentPlatformPitch.append(m_PlatformPitchAngleY);
    m_CurrentPlatformTranslationX.append(m_PlatformTranslation.x);
    m_CurrentPlatformTranslationY.append(m_PlatformTranslation.y);
    m_CurrentPlatformTranslationZ.append(m_PlatformTranslation.z);
    m_CurrentCoreSize.append(sqrt(4*1.25643*m_TurbulentViscosity*m_KinViscosity*(m_VortexTimeOffset+m_currentTime)));

    for (int i=0;i<m_numBlades;i++){
        m_angPos[i].append(azipos[i]);
        m_CurrentPitchAngle[i].append(m_PitchAngle[i]);
        m_RootBendingMomentIP[i].append(bendingip[i]);
        m_RootBendingMomentOOP[i].append(bendingoop[i]);
        m_BladeNormalForce[i].append(bladenormal[i]);
        m_BladeTangentialForce[i].append(bladetangential[i]);
    }

    if (std::isnan(Cp)) AbortSimulation();

}

void QLLTSimulation::AbortSimulation(){
    m_Abort = true;
    m_bStopRequested = true;
    return;
}

void QLLTSimulation::CalcBladeCirculation()
{
    // calculate with equation (van Garrel S. 11 (18)) Gamma _cal
    for (int i=0; i<m_BladePanel.size();i++){

        double Vcp_a1_skal = m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1);
        double Vcp_a3_skal = m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a3);

        CVector LALB = m_BladePanel[i]->VortPtB - m_BladePanel[i]->VortPtA;

        double Vcp_dl_cross_a1_skal = CVector(m_BladePanel[i]->m_V_total*LALB).dot(m_BladePanel[i]->a1);
        double Vcp_dl_cross_a3_skal = CVector(m_BladePanel[i]->m_V_total*LALB).dot(m_BladePanel[i]->a3);

        //Calculate new gamma
        m_BladePanel[i]->m_Gamma = m_BladePanel[i]->m_CL*0.5*m_BladePanel[i]->Area*(pow(Vcp_a1_skal,2)+pow(Vcp_a3_skal,2)) / sqrt(pow(Vcp_dl_cross_a1_skal,2)+pow(Vcp_dl_cross_a3_skal,2));
    }
}

void QLLTSimulation::CalcStrutCirculation()
{
    if (!m_bCircStruts) return;

    // calculate with equation (van Garrel S. 11 (18)) Gamma _cal
    for (int i=0; i<m_StrutPanel.size();i++){

        double Vcp_a1_skal = m_StrutPanel[i]->m_V_total.dot(m_StrutPanel[i]->a1);
        double Vcp_a3_skal = m_StrutPanel[i]->m_V_total.dot(m_StrutPanel[i]->a3);

        CVector LALB = m_StrutPanel[i]->VortPtB - m_StrutPanel[i]->VortPtA;

        double Vcp_dl_cross_a1_skal = CVector(m_StrutPanel[i]->m_V_total*LALB).dot(m_StrutPanel[i]->a1);
        double Vcp_dl_cross_a3_skal = CVector(m_StrutPanel[i]->m_V_total*LALB).dot(m_StrutPanel[i]->a3);

        //Calculate new gamma
        m_StrutPanel[i]->m_Gamma = m_StrutPanel[i]->m_CL*0.5*m_StrutPanel[i]->Area*(pow(Vcp_a1_skal,2)+pow(Vcp_a3_skal,2)) / sqrt(pow(Vcp_dl_cross_a1_skal,2)+pow(Vcp_dl_cross_a3_skal,2));
    }
}

CVector QLLTSimulation::CalcTowerInfluence(CVector EvalPt, CVector V_ref, int timestep) {

    if (!m_bIncludeTower) return V_ref;

    CVector O(0,0,0);

    if (timestep < 0){
    // rotate evaluation point back to reference rotor position
    EvalPt.Translate(m_PlatformTranslation*-1);
    EvalPt.RotateZ(O,-m_PlatformYawAngleZ);
    EvalPt.RotateY(O,-m_PlatformPitchAngleY);
    EvalPt.RotateX(O,-m_PlatformRollAngleX);

    // rotate reference velocity back to reference rotor position
    V_ref.RotateZ(O,-m_PlatformYawAngleZ);
    V_ref.RotateY(O,-m_PlatformPitchAngleY);
    V_ref.RotateX(O,-m_PlatformRollAngleX);
    }
    else{
        EvalPt.Translate(CVector(m_CurrentPlatformTranslationX.at(timestep), m_CurrentPlatformTranslationY.at(timestep), m_CurrentPlatformTranslationZ.at(timestep))*-1);
        EvalPt.RotateZ(O,-m_CurrentPlatformYaw.at(timestep));
        EvalPt.RotateY(O,-m_CurrentPlatformPitch.at(timestep));
        EvalPt.RotateX(O,-m_CurrentPlatformRoll.at(timestep));

        // rotate reference velocity back to reference rotor position
        V_ref.RotateZ(O,-m_CurrentPlatformYaw.at(timestep));
        V_ref.RotateY(O,-m_CurrentPlatformPitch.at(timestep));
        V_ref.RotateX(O,-m_CurrentPlatformRoll.at(timestep));
    }

    if (EvalPt.z > m_TowerHeight) return V_ref;

    double radius = m_TowerBottomRadius + (m_TowerTopRadius-m_TowerBottomRadius) * EvalPt.z / m_TowerHeight;

    if (CVector(EvalPt.x,EvalPt.y,0).VAbs() <= radius) return V_ref;

    EvalPt.z = 0;
    CVector Vref2 = CVector(V_ref.x, V_ref.y, 0);
    double alpha = acos(CVector(1,0,0).dot(Vref2) / (Vref2).VAbs());
    if (Vref2.y < 0 && Vref2.x > 0)
        alpha = 2*PI-alpha;

    //now the coordinates need to be rotated
    const double sqrtOfD = sqrt(EvalPt.VAbs()/radius);

    CVector newCoord = EvalPt;
    newCoord.RotZ(-alpha);

    double x = newCoord.x/radius;
    double y = newCoord.y/radius;

    double utower;
    if (x < 0 || fabs(y) > sqrtOfD) {
        utower = 0;
    } else {
        utower = m_TowerDrag / sqrtOfD * pow(cos(PI/2 * y/sqrtOfD), 2);
    }

    const double u = 1-(pow((x+0.1),2)-pow(y,2))/pow((pow((x+0.1),2)+pow(y,2)),2)-utower;
    const double v = -2*((x+0.1)*y)/pow((pow((x+0.1),2)+pow(y,2)),2);

    CVector res(u*V_ref.VAbs(), v*V_ref.VAbs(), 0);
    res.RotZ(alpha);
    res.z = V_ref.z;

    //rotate resulting velocity vector back into current rotor position
    if (timestep < 0){
    res.RotateX(O,m_PlatformRollAngleX);
    res.RotateY(O,m_PlatformPitchAngleY);
    res.RotateZ(O,m_PlatformYawAngleZ);
    }
    else{
    res.RotateZ(O,m_CurrentPlatformYaw.at(timestep));
    res.RotateY(O,m_CurrentPlatformPitch.at(timestep));
    res.RotateX(O,m_CurrentPlatformRoll.at(timestep));
    }

    return res;
}

void QLLTSimulation::CalcVAWTResults(){

    double Cp, Ct, Cn, Cm, CtS, /*CnS,*/ power=0, area=0, torque=0, torqueStrut=0, powerStrut=0;
    CVector InflowNorm, Thrust(0,0,0), ThrustStrut(0,0,0);
    CVector thrustActingPoint(0,0,0);
    double totalThrustAbs = 0;
    QVector<double> azipos, bladetangential, bladenormal;
    QVector<QVector<double> > BladeTimestepData;
    QVector<QVector<double> > CL, CD, RE, VIND, VTOWER, DYNP, VABS, VABSNOIND, CN, CT, AXIND, INTER, FORCEN, FORCET, AOA, RADIUS, CHORD, CLCD, TWIST, GAMMA, VSAMPLE, VROTATIONAL, DELTA_RE;

    TSR = m_Omega *m_Blade->m_MaxRadius / getFreeStream(m_hubCoords[3]).VAbs();

    if (m_bisStandStill){
        TSR = 0;
        m_Omega = 0;
    }

    QVector<double> dummy;
    for (int m=0;m<m_numBlades;m++){
        azipos.append(0);
        bladenormal.append(0);
        bladetangential.append(0);
        CL.append(dummy);
        CD.append(dummy);
        VABS.append(dummy);
        VABSNOIND.append(dummy);
        VSAMPLE.append(dummy);
        VROTATIONAL.append(dummy);
        VIND.append(dummy);
        VTOWER.append(dummy);
        CN.append(dummy);
        CT.append(dummy);
        FORCEN.append(dummy);
        FORCET.append(dummy);
        AOA.append(dummy);
        RADIUS.append(dummy);
        RE.append(dummy);
        AXIND.append(dummy);
        INTER.append(dummy);
        CHORD.append(dummy);
        CLCD.append(dummy);
        TWIST.append(dummy);
        GAMMA.append(dummy);
        DYNP.append(dummy);
        DELTA_RE.append(dummy);
    }
    for (int ar=0; ar<m_Blade->TPos.size()-1; ar++){
        area += (m_Blade->TOffsetX[ar]+m_Blade->TOffsetX[ar+1])*(m_Blade->TPos[ar+1]-m_Blade->TPos[ar]);
    }
    for (int k = 0; k < m_BladePanel.size(); ++k) {

                InflowNorm = m_BladePanel[k]->m_V_sampled;
                InflowNorm.Normalize();

                Ct = m_BladePanel[k]->LiftDragVector.dot(m_BladePanel[k]->tangentialVector);
                Cn = m_BladePanel[k]->LiftDragVector.dot(m_BladePanel[k]->radialVector);

                torque += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * m_BladePanel[k]->radius * 0.5 * m_Density;
                power += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * m_Omega * m_BladePanel[k]->radius * 0.5 * m_Density;
                Thrust += m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * 0.5 * m_Density;
                totalThrustAbs += (m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * 0.5 * m_Density).VAbs();
                thrustActingPoint += m_BladePanel[k]->CtrlPt*CVector(m_BladePanel[k]->LiftDragVector * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * 0.5 * m_Density).VAbs();

                bladenormal[m_BladePanel[k]->fromBlade] += Cn * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * 0.5 * m_Density;;
                bladetangential[m_BladePanel[k]->fromBlade] += Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * 0.5 * m_Density;
                double REY = m_BladePanel[k]->chord*m_BladePanel[k]->m_V_inPlane.VAbs()/m_KinViscosity;

                azipos[m_BladePanel[k]->fromBlade] = m_BladePanel[k]->angularPos;
                CL[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CL);
                CD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CD);
                VABS[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_V_inPlane.VAbs());
                VABSNOIND[m_BladePanel[k]->fromBlade].append(CVector(CVector(m_BladePanel[k]->m_V_total-m_BladePanel[k]->m_V_induced).dot(m_BladePanel[k]->a1), CVector(m_BladePanel[k]->m_V_total-m_BladePanel[k]->m_V_induced).dot(m_BladePanel[k]->a3), 0).VAbs());
                VSAMPLE[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_sampled.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_sampled.dot(m_BladePanel[k]->a3), 0).VAbs());
                VROTATIONAL[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_relative.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_relative.dot(m_BladePanel[k]->a3), 0).VAbs());
                VIND[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_induced.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_induced.dot(m_BladePanel[k]->a3), 0).VAbs());
                VTOWER[m_BladePanel[k]->fromBlade].append(CVector(m_BladePanel[k]->m_V_tower.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_tower.dot(m_BladePanel[k]->a3), 0).VAbs());
                CN[m_BladePanel[k]->fromBlade].append(Cn);
                CT[m_BladePanel[k]->fromBlade].append(Ct);
                FORCEN[m_BladePanel[k]->fromBlade].append(Cn * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->chord * 0.5 * m_Density);
                FORCET[m_BladePanel[k]->fromBlade].append(Ct * pow(m_BladePanel[k]->m_V_inPlane.VAbs(), 2) * m_BladePanel[k]->chord * 0.5 * m_Density);
                AOA[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_AoA);
                RADIUS[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->fromBladelength);
                RE[m_BladePanel[k]->fromBlade].append(REY);
                double axind = 1 - InflowNorm.dot(m_BladePanel[k]->m_V_induced+m_BladePanel[k]->m_V_sampled) / InflowNorm.dot(m_BladePanel[k]->m_V_sampled);
                AXIND[m_BladePanel[k]->fromBlade].append(axind);
                INTER[m_BladePanel[k]->fromBlade].append(1-axind);
                CHORD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->chord);
                CLCD[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_CL/m_BladePanel[k]->m_CD);
                TWIST[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->twistAngle);
                GAMMA[m_BladePanel[k]->fromBlade].append(m_BladePanel[k]->m_Gamma);
                DYNP[m_BladePanel[k]->fromBlade].append(0.5*m_Density*pow(m_BladePanel[k]->m_V_total.VAbs(),2));
                DELTA_RE[m_BladePanel[k]->fromBlade].append(REY-m_BladePanel[k]->m_RE);
    }

    for (int k = 0; k < m_StrutPanel.size(); ++k) {

        CVector LiftDragVector;
        if (m_bCircStruts){
        LiftDragVector = m_StrutPanel[k]->LiftDragVector;
        }
        else{
        LiftDragVector = m_StrutPanel[k]->DragVector;
        }

        CtS = LiftDragVector.dot(m_StrutPanel[k]->tangentialVector);
//        CnS =  LiftDragVector.dot(m_StrutPanel[k]->radialVector);

        torqueStrut += CtS * pow(m_StrutPanel[k]->m_V_inPlane.VAbs(),2) * m_StrutPanel[k]->Area * m_StrutPanel[k]->radius * 0.5 * m_Density;
        powerStrut += CtS * pow(m_StrutPanel[k]->m_V_inPlane.VAbs(),2) * m_StrutPanel[k]->Area * m_Omega * m_StrutPanel[k]->radius * 0.5 * m_Density;
        ThrustStrut += LiftDragVector * pow(m_StrutPanel[k]->m_V_inPlane.VAbs(),2) * m_StrutPanel[k]->Area * 0.5 * m_Density;
        totalThrustAbs += (LiftDragVector * pow(m_StrutPanel[k]->m_V_inPlane.VAbs(),2) * m_StrutPanel[k]->Area * 0.5 * m_Density).VAbs();
        thrustActingPoint += m_StrutPanel[k]->CtrlPt*CVector(LiftDragVector * pow(m_StrutPanel[k]->m_V_inPlane.VAbs(),2) * m_StrutPanel[k]->Area * 0.5 * m_Density).VAbs();
    }

    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CL[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CLCD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(GAMMA[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(AOA[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(RADIUS[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CHORD[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(TWIST[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CN[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(CT[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(FORCEN[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(FORCET[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(DYNP[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VABS[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VABSNOIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VSAMPLE[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VROTATIONAL[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(VTOWER[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(AXIND[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(INTER[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(RE[m]);
    for (int m=0;m<m_numBlades;m++) BladeTimestepData.append(DELTA_RE[m]);


    m_BladeOutputData.append(BladeTimestepData);

    Cp = (power+powerStrut) / ( area * 0.5*m_Density * pow(getMeanFreeStream(m_hubCoords[3]).VAbs(),3));
    Cm = Cp / TSR;
    Ct = (Thrust.VAbs()+ThrustStrut.VAbs()) / (area * PI * 0.5*m_Density * pow(getMeanFreeStream(m_hubCoords[3]).VAbs(),2));

    m_CP.append(Cp);
    m_CT.append(Ct);
    m_CM.append(Cm);
    m_TSR.append(TSR);
    m_Power.append((power+powerStrut)/1000);
    m_Torque.append(torque+torqueStrut);
    m_Time.append(m_currentTime);
    m_rotSpeed.append(m_Omega/2/PI*60);
    m_ThrustAbs.append(Thrust.VAbs()+ThrustStrut.VAbs());
    m_ThrustActingPoint.append(thrustActingPoint/totalThrustAbs);
    m_ThrustVector.append(Thrust+ThrustStrut);
    m_ThrustX.append(Thrust.x+ThrustStrut.x);
    m_ThrustY.append(Thrust.y+ThrustStrut.y);
    m_ThrustZ.append(Thrust.z+ThrustStrut.z);
    m_CurrentHHWindSpeed.append(getFreeStream(m_hubCoords[3]).VAbs());
    m_CurrentYawAngle.append(m_YawAngle);
    m_CurrentHorizontalInflowAngle.append(m_HorInflowAngle);
    m_CurrentVerticalInflowSpeed.append(m_VertInflowSpeed);
    m_CurrentPlatformYaw.append(m_PlatformYawAngleZ);
    m_CurrentPlatformRoll.append(m_PlatformRollAngleX);
    m_CurrentPlatformPitch.append(m_PlatformPitchAngleY);
    m_CurrentPlatformTranslationX.append(m_PlatformTranslation.x);
    m_CurrentPlatformTranslationY.append(m_PlatformTranslation.y);
    m_CurrentPlatformTranslationZ.append(m_PlatformTranslation.z);
    m_CurrentCoreSize.append(sqrt(4*1.25643*m_TurbulentViscosity*m_KinViscosity*(m_VortexTimeOffset+m_currentTime)));

    for (int i=0;i<m_numBlades;i++){
        m_angPos[i].append(azipos[i]);
        m_CurrentPitchAngle[i].append(m_PitchAngle[i]);
        m_BladeNormalForce[i].append(bladenormal[i]);
        m_BladeTangentialForce[i].append(bladetangential[i]);
    }

    // here the averaged values (over one full revolution) are computed

    double cpAv = 0;
    double powAv = 0;
    double thrAv = 0;
    double torAv = 0;
    double ctAv = 0;
    double cmAv = 0;

    double totang = 0;
    if (m_angPos[0].size() >= 2){
        for (int k=m_angPos[0].size()-2;k>=0;k--){
            double ang_incr = m_angPos[0].at(k+1)-m_angPos[0].at(k);
            if (ang_incr < 0) ang_incr += 360;
            totang += ang_incr;
            cpAv += m_CP.at(k+1)*ang_incr/360;
            ctAv += m_CT.at(k+1)*ang_incr/360;
            cmAv += m_CM.at(k+1)*ang_incr/360;
            powAv += m_Power.at(k+1)*ang_incr/360;
            thrAv += m_ThrustAbs.at(k+1)*ang_incr/360;
            torAv += m_Torque.at(k+1)*ang_incr/360;

            if (totang >= 360){
                cpAv  += (360 - totang) * m_CP.at(k+1) / 360;
                ctAv  += (360 - totang) * m_CT.at(k+1) / 360;
                cmAv  += (360 - totang) * m_CM.at(k+1) / 360;
                powAv += (360 - totang) * m_Power.at(k+1) / 360;
                thrAv += (360 - totang) * m_ThrustAbs.at(k+1) / 360;
                torAv += (360 - totang) * m_Torque.at(k+1) / 360;
                break;
            }
        }
    if (totang  < 360){
        cpAv = 0;
        powAv = 0;
        torAv = 0;
        ctAv = 0;
        cmAv = 0;
        thrAv = 0;
    }
    }
    else{
        cpAv = 0;
        powAv = 0;
        thrAv = 0;
        torAv = 0;
        ctAv = 0;
        cmAv = 0;
    }

    m_CPAV.append(cpAv);
    m_CTAV.append(ctAv);
    m_CMAV.append(cmAv);
    m_PowerAV.append(powAv);
    m_TorqueAV.append(torAv);
    m_ThrustAV.append(thrAv);


    if (std::isnan(Cp)) AbortSimulation();


}

void QLLTSimulation::callGLLists(){

    if (g_mainFrame->getCurrentModule() == m_module && m_module->GLView) {

        double HorInfAngle = m_HorInflowAngle;
        if (m_bStoreWake && hasStoredData() && m_plotAtTime != -1){
            HorInfAngle = m_CurrentHorizontalInflowAngle.at(m_plotAtTime);}

        glEnable(GL_DEPTH_TEST);
        glLineWidth(1.0);
        glCallList(GLTURBINECOORDINATES);
        glDisable(GL_DEPTH_TEST);
        glCallList(GLWINGWAKEPANELS);
        glEnable(GL_DEPTH_TEST);
        glCallList(GLBODYMESHPANELS);
        glDisable(GL_DEPTH_TEST);
        glCallList(GLBLADEOUTLINES);
        glEnable(GL_DEPTH_TEST);
        glRotated(HorInfAngle,0,0,1);
        glCallList(GLWINDFIELD);
        glRotated(-HorInfAngle,0,0,1);
        glPolygonOffset(1.0, 0);
        if (m_bGLPlotCoordinates) DrawCoordinateSystems();
    }
}

void QLLTSimulation::ConvectWakeNoRollup(){

    if (!m_WakeNode.size()) return;
    CVector TempVelocities[m_WakeNode.size()-1];

    if (m_bisCenterEval){
        for (int i=0;i<m_WakeLine.size();i++){
            CVector center = (*m_WakeLine[i]->pL+*m_WakeLine[i]->pT)/2;
                 if (m_bTurbConvection) m_WakeLine[i]->Velocity = CalcTowerInfluence(center,getFreeStream(center));
                 else m_WakeLine[i]->Velocity = getMeanFreeStream(center);

        }
    }
    else{
    for (int i=0;i<m_WakeNode.size();i++){
             if (m_bTurbConvection) TempVelocities[i] = CalcTowerInfluence(*m_WakeNode[i],getFreeStream(*m_WakeNode[i]));
             else TempVelocities[i] = getMeanFreeStream(*m_WakeNode[i]);
      }
   }

    if (m_bisCenterEval){
        for (int i=0;i<m_WakeNode.size();i++){
            if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                CVector interpolatedVel(0,0,0);
                double totalLength = 0;
                for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                    VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                    interpolatedVel += line->Velocity * line->Length;
                    totalLength += line->Length;
                }
                *m_WakeNode[i] += interpolatedVel/totalLength *dT*m_Nth_WakeStep;
            }
            else{
                CVector interpolatedVel(0,0,0);
                double totalLength = 0;
                    for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                    VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                    interpolatedVel += line->Velocity * line->Length;
                    totalLength += line->Length;
                    }
                *m_WakeNode[i] += interpolatedVel/totalLength *dT*m_Nth_WakeStep*m_firstWakeRowLength;
                CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                *m_WakeNode[i] = TWN;
            }
        }
    }
    else{
        for (int i=0;i<m_WakeNode.size();i++){
            if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                *m_WakeNode[i] += TempVelocities[i]*dT*m_Nth_WakeStep;
            }
            else{
                *m_WakeNode[i] += TempVelocities[i]*dT*m_Nth_WakeStep*m_firstWakeRowLength;
                CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                *m_WakeNode[i] = TWN;
            }
        }
    }

}


CVector QLLTSimulation::CorrespondingAxisPoint(CVector Point, CVector Line1, CVector Line2)
{
     CVector v = Line1 - Line2;
     CVector w = Point - Line1;

     double c1 = w.dot(v);
     double c2 = v.dot(v);
     double b = c1 / c2;

     CVector Pb = Line1 + v * b;
     return Pb;
}


void QLLTSimulation::ClearOutputArrays(){

    m_BladeOutputData.clear();
    m_results.clear();
    m_availableVariables.clear();
    m_availableBladeVariables.clear();



    m_CP.clear();
    m_CT.clear();
    m_CM.clear();
    m_TSR.clear();
    m_Power.clear();
    m_Torque.clear();
    m_Torque.clear();
    m_Time.clear();
    m_rotSpeed.clear();
    m_ThrustAbs.clear();
    m_ThrustAV.clear();
    m_ThrustX.clear();
    m_ThrustY.clear();
    m_ThrustZ.clear();
    m_CurrentHHWindSpeed.clear();
    m_CurrentYawAngle.clear();
    m_CurrentHorizontalInflowAngle.clear();
    m_CurrentVerticalInflowSpeed.clear();
    m_Iterations.clear();

    m_CurrentPlatformYaw.clear();
    m_CurrentPlatformRoll.clear();
    m_CurrentPlatformPitch.clear();
    m_CurrentPlatformTranslationX.clear();
    m_CurrentPlatformTranslationY.clear();
    m_CurrentPlatformTranslationZ.clear();
    m_ElapsedSimulationTime.clear();
    m_TimePerTimestep.clear();
    m_CurrentCoreSize.clear();
    m_CPAV.clear();
    m_CTAV.clear();
    m_CMAV.clear();
    m_PowerAV.clear();
    m_TorqueAV.clear();
    m_unconvergedPositions.clear();
    m_ThrustActingPoint.clear();
    m_ThrustVector.clear();

    for (int i=0;i<m_numBlades;i++){
        m_angPos[i].clear();
        m_CurrentPitchAngle[i].clear();
        m_RootBendingMomentIP[i].clear();
        m_RootBendingMomentOOP[i].clear();
        m_BladeTangentialForce[i].clear();
        m_BladeNormalForce[i].clear();
    }
}

void QLLTSimulation::ClearVortexArrays(){

    for (int i=0; i<m_BladePanel.size();i++){
        delete m_BladePanel.at(i);
    }
    m_BladePanel.clear();

    for (int i=0; i<m_StrutPanel.size();i++){
        delete m_StrutPanel.at(i);
    }
    m_StrutPanel.clear();

    for (int i=0; i<m_BladeLine.size();i++){
        delete m_BladeLine.at(i);
    }
    m_BladeLine.clear();

    for (int i=0; i<m_WakeLine.size();i++){
        delete m_WakeLine.at(i);
    }
    m_WakeLine.clear();

    for (int i=0; i<m_BladeNode.size();i++){
        delete m_BladeNode.at(i);
    }
    m_BladeNode.clear();

    for (int i=0; i<m_WakeNode.size();i++){
        delete m_WakeNode.at(i);
    }
    m_WakeNode.clear();

    for (int i=0; i<m_StrutNode.size();i++){
        delete m_StrutNode.at(i);
    }
    m_StrutNode.clear();

    m_savedWakeLines.clear();

    m_savedBladeVizLines.clear();

    m_savedBladeVortexLines.clear();

    m_savedCoordinateSystems.clear();
}

void QLLTSimulation::CheckWakeNodesForSanity(){

    for (int i=m_WakeLine.size()-1; i>=0;i--){
        VortexLine *l = m_WakeLine.at(i);
        if (std::isnan(l->pT->x) || std::isnan(l->pT->y) || std::isnan(l->pT->z) || std::isnan(l->pL->x) || std::isnan(l->pL->y) || std::isnan(l->pL->z)){
            l->Delete();
        }
    }
}

void QLLTSimulation::ConvectWake(){

    for(int ID = 0; ID < m_BladePanel.size(); ++ID) {
            m_BladePanel[ID]->m_Gamma = m_BladePanel[ID]->m_Gamma_t_minus_1;
    }

    if (m_bWakeRollUp){
        ConvectWakeOpenMP();
    }
    else{
        ConvectWakeNoRollup();
    }

    CheckWakeNodesForSanity();

    if (m_bisPC2B) PC2BIntegration();

    UpdateWakeLines();
}

void QLLTSimulation::UpdateWakeLines(){

    for (int i=0; i<m_WakeLine.size(); i++)
    {
        m_WakeLine[i]->Update(dT*m_Nth_WakeStep);
        if (m_WakeLine[i]->stretchFactor > m_maxStretch) m_WakeLine[i]->Delete();
    }
}

void QLLTSimulation::PC2BIntegration(){

    for (int i=0;i<m_WakeNode.size();i++){
        if (m_WakeNode[i]->oldPositions.size() > 2){
            *m_WakeNode[i] = m_WakeNode[i]->oldPositions[m_WakeNode[i]->oldPositions.size()-1] + (*m_WakeNode[i]*3 - m_WakeNode[i]->oldPositions[m_WakeNode[i]->oldPositions.size()-1] -m_WakeNode[i]->oldPositions[m_WakeNode[i]->oldPositions.size()-2]*3 + m_WakeNode[i]->oldPositions[m_WakeNode[i]->oldPositions.size()-3])/4;
        }
        if (m_WakeNode[i]->oldPositions.size() > 2) m_WakeNode[i]->oldPositions.removeFirst();

        m_WakeNode[i]->oldPositions.append(CVector(m_WakeNode[i]->x, m_WakeNode[i]->y, m_WakeNode[i]->z));
    }
}

void QLLTSimulation::ConvectWakeOpenMP(){

    if (!m_WakeNode.size()) return;

    //temporary storage of wake nodes
    QList <VortexNode > TempWakeNode;

    CVector TempVelocities[m_WakeNode.size()];
    CVector TempVelocities2[m_WakeNode.size()];

    for (int i=0;i<m_WakeNode.size();i++){
        VortexNode C;
        C = *m_WakeNode.at(i);
        C.wasConvected = false;
        TempWakeNode.push_back(C);
    }

        // first step of the predictor corrector method
        if (m_bisOpenMp){
            #pragma omp parallel default (none) shared (TempVelocities)
            {
                if (m_bisCenterEval){
                #pragma omp for
                    for (int i=0;i<m_WakeLine.size();i++){
                        CVector center = (*m_WakeLine[i]->pL+*m_WakeLine[i]->pT)/2;
                            if (m_bTurbConvection) m_WakeLine[i]->Velocity = CalcTowerInfluence(center,(getFreeStream(center)+BiotSavartInduction(center)));
                            else                   m_WakeLine[i]->Velocity = (getMeanFreeStream(center)+BiotSavartInduction(center));
                    }
                }
                else{
                #pragma omp for
                    for (int i=0;i<m_WakeNode.size();i++){
                            if (m_bTurbConvection) TempVelocities[i] = CalcTowerInfluence(*m_WakeNode[i],(getFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i])));
                            else TempVelocities[i] = (getMeanFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i]));
                    }
                }
            }
        }
        else{
            if (m_bisCenterEval){
                for (int i=0;i<m_WakeLine.size();i++){
                    CVector center = (*m_WakeLine[i]->pL+*m_WakeLine[i]->pT)/2;
                        if (m_bTurbConvection) m_WakeLine[i]->Velocity = CalcTowerInfluence(center,(getFreeStream(center)+BiotSavartInduction(center)));
                        else                   m_WakeLine[i]->Velocity = (getMeanFreeStream(center)+BiotSavartInduction(center));
                }
            }
            else{
                for (int i=0;i<m_WakeNode.size();i++){
                        if (m_bTurbConvection) TempVelocities[i] = CalcTowerInfluence(*m_WakeNode[i],(getFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i])));
                        else TempVelocities[i] = (getMeanFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i]));
                  }
           }
        }

        if (m_bisCenterEval){
            for (int i=0;i<m_WakeNode.size();i++){
                if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                    CVector interpolatedVel(0,0,0);
                    double totalLength = 0;
                    for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                        VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                        interpolatedVel += line->Velocity * line->Length;
                        totalLength += line->Length;
                    }
                    *m_WakeNode[i] += interpolatedVel/totalLength *dT*m_Nth_WakeStep;
                }
                else{

                    if (m_bTrail){
                        CVector interpolatedVel(0,0,0);
                        double totalLength = 0;

                        for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                        VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                        interpolatedVel += line->Velocity * line->Length;
                        totalLength += line->Length;
                        }

                        *m_WakeNode[i] += interpolatedVel/totalLength *dT*m_Nth_WakeStep*m_firstWakeRowLength;
                        CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                        TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                        *m_WakeNode[i] = TWN;
                    }
                    else{
                        *m_WakeNode[i] += (getMeanFreeStream(*m_WakeNode[i]))*dT*m_Nth_WakeStep*m_firstWakeRowLength;
                        CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                        TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                        *m_WakeNode[i] = TWN;
                    }
                }
            }
        }
        else{
            for (int i=0;i<m_WakeNode.size();i++){
                if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                    *m_WakeNode[i] += TempVelocities[i]*dT*m_Nth_WakeStep;
                }
                else{
                    *m_WakeNode[i] += TempVelocities[i]*dT*m_Nth_WakeStep*m_firstWakeRowLength;
                    CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                    TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                    *m_WakeNode[i] = TWN;
                }
            }
        }

        // Update the wake lines
        for (int i=0; i<m_WakeLine.size(); i++)
        {
        m_WakeLine[i]->InitLength();
        }

        if (m_bPredictorCorrector){

            // second step of the predictor corrector method
            if(m_bisOpenMp){
                #pragma omp parallel default (none) shared (TempVelocities2)
                {
                    if (m_bisCenterEval){
                    #pragma omp for
                        for (int i=0;i<m_WakeLine.size();i++){
                            CVector center = (*m_WakeLine[i]->pL+*m_WakeLine[i]->pT)/2;
                                if (m_bTurbConvection) m_WakeLine[i]->CorrVelocity = CalcTowerInfluence(center,(getFreeStream(center)+BiotSavartInduction(center)));
                                else                   m_WakeLine[i]->CorrVelocity = (getMeanFreeStream(center)+BiotSavartInduction(center));
                        }
                    }
                    else{
                    #pragma omp for
                    for (int i=0;i<m_WakeNode.size();i++){
                            if (m_bTurbConvection) TempVelocities2[i] = CalcTowerInfluence(*m_WakeNode[i],(getFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i])));
                            else TempVelocities2[i] = (getMeanFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i]));
                    }
                   }
                }
            }
            else{
                if (m_bisCenterEval){
                    for (int i=0;i<m_WakeLine.size();i++){
                        CVector center = (*m_WakeLine[i]->pL+*m_WakeLine[i]->pT)/2;
                            if (m_bTurbConvection) m_WakeLine[i]->CorrVelocity = CalcTowerInfluence(center,(getFreeStream(center)+BiotSavartInduction(center)));
                            else                   m_WakeLine[i]->CorrVelocity = (getMeanFreeStream(center)+BiotSavartInduction(center));
                    }
                }
                else{
                    for (int i=0;i<m_WakeNode.size();i++){
                            if (m_bTurbConvection) TempVelocities2[i] = CalcTowerInfluence(*m_WakeNode[i],(getFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i])));
                            else TempVelocities2[i] = (getMeanFreeStream(*m_WakeNode[i])+BiotSavartInduction(*m_WakeNode[i]));
                    }
                }
            }


            if (m_bisCenterEval){
                for (int i=0;i<m_WakeNode.size();i++){
                    if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                        CVector interpolatedVel(0,0,0);
                        CVector interpolatedVelCorr(0,0,0);
                        double totalLength = 0;
                        double totalOldLength = 0;

                        for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                            VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                            interpolatedVel += line->Velocity * line->oldLengthPredCorr;
                            interpolatedVelCorr += line->CorrVelocity * line->Length;
                            totalLength += line->Length;
                            totalOldLength += line->oldLengthPredCorr;
                        }
                        *m_WakeNode[i] = TempWakeNode[i] + (interpolatedVel/totalOldLength + interpolatedVelCorr/totalLength) /2 *dT*m_Nth_WakeStep;
                    }
                    else{
                        if (m_bTrail){
                        CVector interpolatedVel(0,0,0);
                        CVector interpolatedVelCorr(0,0,0);
                        double totalLength = 0;
                        double totalOldLength = 0;

                        for (int j=0;j<m_WakeNode[i]->attachedLines.size();j++){
                        VortexLine *line = (VortexLine*)  m_WakeNode[i]->attachedLines[j];
                        interpolatedVelCorr += line->CorrVelocity * line->Length;
                        interpolatedVel += line->Velocity * line->oldLengthPredCorr;
                        totalLength += line->Length;
                        totalOldLength += line->oldLengthPredCorr;

                        }
                        *m_WakeNode[i] = TempWakeNode[i] + (interpolatedVel/totalOldLength + interpolatedVelCorr/totalLength) /2 * dT*m_Nth_WakeStep * m_firstWakeRowLength;
                        CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                        TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                        *m_WakeNode[i] = TWN;
                        }
                        else{
                            *m_WakeNode[i] = TempWakeNode[i] + (getMeanFreeStream(*m_WakeNode[i]))*dT*m_Nth_WakeStep*m_firstWakeRowLength;
                            CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                            TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                            *m_WakeNode[i] = TWN;
                    }
                   }
                }
            }
            else{
                for (int i=0;i<m_WakeNode.size();i++){
                    if (m_WakeNode.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep){
                        *m_WakeNode[i] = TempWakeNode[i]+(TempVelocities[i]+TempVelocities2[i])*dT*m_Nth_WakeStep*0.5;
                    }
                    else{
                        *m_WakeNode[i] = TempWakeNode[i]+(TempVelocities[i]+TempVelocities2[i])*dT*m_Nth_WakeStep*0.5*m_firstWakeRowLength;
                        CVector TWN(m_WakeNode[i]->x,m_WakeNode[i]->y,m_WakeNode[i]->z);
                        TWN.Rotate(m_hubCoords[3], m_hubCoords[0], m_Omega*dT*m_Nth_WakeStep*(1.0-m_firstWakeRowLength)*180/PI);
                        *m_WakeNode[i] = TWN;
                    }
                }
            }

            // Update the wake lines
            for (int i=0; i<m_WakeLine.size(); i++)
            {
            m_WakeLine[i]->InitLength();
            }


    }


}

void QLLTSimulation::CreateBladeLLTPanels(CVector m_LA, CVector m_LB, CVector m_TA, CVector m_TB, int blade)
{
        // Creates the panel elements that will be used by the lifting line method
        // The panels are created from left to right on a surface
        VortexNode *n0, *n1, *n2, *n3;

        CPanel *panel;
        panel = new CPanel(m_BoundVortexPos, m_AoAPos);

                        n0 = IsNode(m_LA);
                        n1 = IsNode(m_TA);
                        n2 = IsNode(m_LB);
                        n3 = IsNode(m_TB);

                        if(n0)
                        {
                                panel->pLA = n0;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_LA;
                                panel->pLA = vec;
                                vec->fromBlade = blade;
                                m_BladeNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_BladeNodes.append(node);
                                }
                        }


                        if(n1)
                        {
                                panel->pTA = n1;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_TA;
                                panel->pTA = vec;
                                vec->fromBlade = blade;
                                m_BladeNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_BladeNodes.append(node);
                                }
                        }


                        if(n2)
                        {
                                panel->pLB = n2;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_LB;
                                panel->pLB = vec;
                                vec->fromBlade = blade;
                                m_BladeNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_BladeNodes.append(node);
                                }
                        }


                        if(n3)
                        {
                                panel->pTB = n3;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_TB;
                                panel->pTB = vec;
                                vec->fromBlade = blade;
                                m_BladeNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_BladeNodes.append(node);
                                }
                        }


                        panel->SetFrame(m_LA, m_LB, m_TA, m_TB);
                        panel->fromBlade = blade;

                        m_BladePanel.push_back(panel);

}

void QLLTSimulation::CreateStrutLLTPanels(CVector m_LA, CVector m_LB, CVector m_TA, CVector m_TB, int blade, int strut, bool isHub, bool isTip)
{
        // Creates the panel elements that will be used by the lifting line method
        // The panels are created from left to right on a surface
        VortexNode *n0, *n1, *n2, *n3;

        CPanel *panel;
        panel = new CPanel(m_BoundVortexPos, m_AoAPos);

                        n0 = IsNode(m_LA, true);
                        n1 = IsNode(m_TA, true);
                        n2 = IsNode(m_LB, true);
                        n3 = IsNode(m_TB, true);

                        if(n0)
                        {
                                panel->pLA = n0;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_LA;
                                panel->pLA = vec;
                                vec->fromBlade = blade;
                                m_StrutNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_StrutList.at(strut)->m_StrutNodes.append(node);
                                }
                        }


                        if(n1)
                        {
                                panel->pTA = n1;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_TA;
                                panel->pTA = vec;
                                vec->fromBlade = blade;
                                m_StrutNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_StrutList.at(strut)->m_StrutNodes.append(node);
                                }
                        }


                        if(n2)
                        {
                                panel->pLB = n2;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_LB;
                                panel->pLB = vec;
                                vec->fromBlade = blade;
                                m_StrutNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_StrutList.at(strut)->m_StrutNodes.append(node);
                                }
                        }


                        if(n3)
                        {
                                panel->pTB = n3;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = m_TB;
                                panel->pTB = vec;
                                vec->fromBlade = blade;
                                m_StrutNode.push_back(vec);

                                if (blade == 0){
                                VortexNode node = *vec;
                                m_Blade->m_StrutList.at(strut)->m_StrutNodes.append(node);
                                }
                        }


                        panel->SetFrame(m_LA, m_LB, m_TA, m_TB);
                        panel->isHub = isHub;
                        panel->isTip = isTip;
                        panel->fromBlade = blade;
                        panel->fromStrut = strut;

                        m_StrutPanel.push_back(panel);

}

void QLLTSimulation::CreateBladePanelsAndNodes(){

    m_Blade->m_BladeNodes.clear();

    m_Blade->CreateLLTPanels(m_discType,m_numPanels,m_bisVawt);

    for (int i=0;i<m_numBlades;i++){
        for (int j = 0; j < m_Blade->m_PanelPoints.size();j++){
            if (i != 0){
                for (int k=0; k<m_Blade->m_PanelPoints.at(j).size();k++){

                if (!m_bisVawt) m_Blade->m_PanelPoints[j][k].RotateZ(CVector(0,0,0), 360/m_numBlades);
                else m_Blade->m_PanelPoints[j][k].RotateY(CVector(0,0,0), 360/m_numBlades);
                }
            }
            CreateBladeLLTPanels(m_Blade->m_PanelPoints.at(j).at(0),m_Blade->m_PanelPoints.at(j).at(1),m_Blade->m_PanelPoints.at(j).at(2),m_Blade->m_PanelPoints.at(j).at(3),i);
        }
    }

}



void QLLTSimulation::CreateStrutPanelsAndNodes(){

    for (int l=0;l<m_Blade->m_StrutList.size();l++){
        m_Blade->m_StrutList.at(l)->m_StrutNodes.clear();
        m_Blade->m_StrutList.at(l)->CreateSurfaces(m_numStrutPanels);

    }

        for (int i=0;i<m_numBlades;i++){

            for (int l=0;l<m_Blade->m_StrutList.size();l++){
            for (int j = 0; j < m_Blade->m_StrutList.at(l)->m_PanelPoints.size();j++){
                bool hub = false;
                bool tip = false;

                    for (int k=0; k<m_Blade->m_StrutList.at(l)->m_PanelPoints.at(j).size();k++){
                    m_Blade->m_StrutList.at(l)->m_PanelPoints[j][k].RotateY(CVector(0,0,0), 360/m_numBlades*i);

                }
                if (j == 0) hub = true;
                if (j == m_Blade->m_StrutList.at(l)->m_PanelPoints.size()-1) tip = true;
                CreateStrutLLTPanels(m_Blade->m_StrutList.at(l)->m_PanelPoints.at(j).at(0),m_Blade->m_StrutList.at(l)->m_PanelPoints.at(j).at(1),m_Blade->m_StrutList.at(l)->m_PanelPoints.at(j).at(2),m_Blade->m_StrutList.at(l)->m_PanelPoints.at(j).at(3),i,l,hub,tip);
                }
            }
        }
}

void QLLTSimulation::CreateRotorGeometry(){

        CreateBladePanelsAndNodes();
        CreateStrutPanelsAndNodes();
        InitializeBladePanelProperties();
        InitializeStrutPanelProperties();
        UpdateRotorGeometry();
        storeGeometry();
        emit geomChanged();
}

void QLLTSimulation::DrawCoordinateSystems(){

    double scale = -m_Blade->getRotorRadius()/3, rad = m_Blade->getRotorRadius()*1.2;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glLineWidth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);
    glColor4d(1,0,0,1);



    if(m_bGLPlotCoordinates){
    glColor4d(0,0,0,1);
    glVertex3d(VHub.x, VHub.y, VHub.z);
    if (!m_bisVawt){ glVertex3d(VHub.x+VAxis.x*scale, VHub.y+VAxis.y*scale, VHub.z+VAxis.z*scale);
        if (m_module->GetQLLTDock()->m_showText->isChecked()) g_mainFrame->getGlWidget()->overpaintText(VHub.x+VAxis.x*scale*1.8, VHub.y+VAxis.y*scale*1.8, VHub.z+VAxis.z*scale*1.8, "Rotor Axis");
    }
    if (m_bisVawt){  glVertex3d(VHub.x-VAxis.x*scale, VHub.y-VAxis.y*scale, VHub.z-VAxis.z*scale);
        if (m_module->GetQLLTDock()->m_showText->isChecked()) g_mainFrame->getGlWidget()->overpaintText(VHub.x-VAxis.x*scale*1.8, VHub.y-VAxis.y*scale*1.8, VHub.z-VAxis.z*scale*1.8, "Rotor Axis");
    }

    for (int i=0;i<VBAxes.size();i++){
        if (!m_bisVawt){
            glVertex3d(VBOrigins[i].x, VBOrigins[i].y, VBOrigins[i].z);
            glVertex3d(CVector(VBOrigins[i]+VBAxes[i]*rad).x, CVector(VBOrigins[i]+VBAxes[i]*rad).y, CVector(VBOrigins[i]+VBAxes[i]*rad).z);
            if (m_module->GetQLLTDock()->m_showText->isChecked()) g_mainFrame->getGlWidget()->overpaintText(CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).x, CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).y, CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).z, QString("Blade "+QString().number(i+1,'f',0)));
        }
        if (m_bisVawt) {
            glVertex3d(CVector(VBOrigins[i]-VBAxes[i]*rad/6).x, CVector(VBOrigins[i]-VBAxes[i]*rad/6).y, CVector(VBOrigins[i]-VBAxes[i]*rad/6).z);
            glVertex3d(CVector(VBOrigins[i]+VBAxes[i]*rad).x, CVector(VBOrigins[i]+VBAxes[i]*rad).y, CVector(VBOrigins[i]+VBAxes[i]*rad).z);
            if (m_module->GetQLLTDock()->m_showText->isChecked()) g_mainFrame->getGlWidget()->overpaintText(CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).x, CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).y, CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).z, QString("Blade "+QString().number(i+1,'f',0)));
            }
            }
    }
    glVertex3d(0,0,0);
    glVertex3d(rad,0,0);
    glVertex3d(0,0,0);
    glVertex3d(0,rad,0);
    glVertex3d(0,0,0);
    glVertex3d(0,0,rad);
    glEnd();

    if (m_module->GetQLLTDock()->m_showText->isChecked()) {
        if (!m_bisVawt) g_mainFrame->getGlWidget()->overpaintText(VHub.x+VAxis.x*scale*1.8, VHub.y+VAxis.y*scale*1.8, VHub.z+VAxis.z*scale*1.8, "Rotor Axis");
        if (m_bisVawt)  g_mainFrame->getGlWidget()->overpaintText(VHub.x-VAxis.x*scale*1.8, VHub.y-VAxis.y*scale*1.8, VHub.z-VAxis.z*scale*1.8, "Rotor Axis");
        for (int i=0;i<VBAxes.size();i++){
            if (!m_bisVawt) g_mainFrame->getGlWidget()->overpaintText(CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).x, CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).y, CVector(VBOrigins[i]+VBAxes[i]*rad*1.3).z, QString("Blade "+QString().number(i+1,'f',0)));
            if (m_bisVawt) g_mainFrame->getGlWidget()->overpaintText(CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).x, CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).y, CVector(VBOrigins[i]+VBAxes[i]*rad/2*1.6).z, QString("Blade "+QString().number(i+1,'f',0)));
        }
        g_mainFrame->getGlWidget()->overpaintText(rad*1.3,0,0,"X Global");
        g_mainFrame->getGlWidget()->overpaintText(0,rad*1.3,0,"Y Global");
        g_mainFrame->getGlWidget()->overpaintText(0,0,rad*1.3,"Z Global");
    }

    glPopAttrib();

}

void QLLTSimulation::drawGL() {

    if (g_mainFrame->getCurrentModule() == m_module && m_module->GLView) {

        m_plotAtTime = -1;
        if (m_bStoreWake && hasStoredData()){
            if (m_shownTimeIndex != -1) m_plotAtTime = m_shownTimeIndex;
            else m_plotAtTime = m_savedBladeVizLines.size()-1;
        }

        if (m_bGlChanged || m_bforceReRender) {

            if (m_module->m_QLLTDock->m_perspectiveView->isChecked()) m_module->getGLWidget()->camera()->setType(qglviewer::Camera::PERSPECTIVE);
            else m_module->getGLWidget()->camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);

            m_bGLPlotWakeLinesShed = m_module->m_QLLTDock->m_shed->isChecked();
            m_bGLPlotWakeLinesTrail = m_module->m_QLLTDock->m_trail->isChecked();
            m_bGLPlotWakeNodes = m_module->m_QLLTDock->m_nodes->isChecked();
            m_bGLPlotSingleBlade = m_module->m_QLLTDock->m_singleblade->isChecked();
            m_bGLPlotRotor = m_module->m_QLLTDock->m_showRotor->isChecked();
            m_bGLPlotTurbineModel = m_module->m_QLLTDock->m_showModel->isChecked();
            m_bGLPlotCoordinates = m_module->m_QLLTDock->m_showCoordinates->isChecked();
            m_bGLColorStrength = m_module->m_QLLTDock->m_colorStrength->isChecked();
            m_bGLColorAge = m_module->m_QLLTDock->m_colorAge->isChecked();


            if (glIsList(GLWINDFIELD)) glDeleteLists(GLWINDFIELD,1);
            if (glIsList(GLWINGWAKEPANELS)) glDeleteLists(GLWINGWAKEPANELS, 1);
            if (glIsList(GLBLADEOUTLINES)) glDeleteLists(GLBLADEOUTLINES, 1);
            if (glIsList(GLBODYMESHPANELS))  glDeleteLists(GLBODYMESHPANELS, 1);
            if (glIsList(GLTURBINECOORDINATES))  glDeleteLists(GLTURBINECOORDINATES, 1);

            m_bGlChanged = false;
            if (m_WindField && m_module->m_QLLTDock->m_showWindfield->isChecked()) {
                if (m_bStoreWake && hasStoredData()){
                    m_WindField->renderForQLLTSim(m_results.at(0)->at(m_plotAtTime) - dT/2, -m_WindField->getFieldDimensions(),getMeanFreeStream(CVector(-m_RotorOverhang, 0, m_HubHeight)).VAbs(),m_module->m_QLLTDock->m_colorMapButton->button(0)->isChecked());
                }
                else{
                    m_WindField->renderForQLLTSim(m_currentTime - dT/2, -m_WindField->getFieldDimensions(),getMeanFreeStream(CVector(-m_RotorOverhang, 0, m_HubHeight)).VAbs(),m_module->m_QLLTDock->m_colorMapButton->button(0)->isChecked());
                }
            }
            GLDrawSimulationScene(m_bStoreWake && hasStoredData() && !isRunning());
            LoadRotorCoords(m_bStoreWake && hasStoredData() && !isRunning());
        }
    }

    m_bforceReRender = false;
}

void QLLTSimulation::drawLegend(QPainter &painter, bool redblue, int component){
	const int barWidth = 25;
	const int barHeight = 100;
	const int marginLeft = 15;
	const int marginTop = 55;
	
	QLinearGradient gradient (QPointF(0, marginTop), QPointF(0, marginTop+barHeight));
	if (redblue) {
		for (int i = 0; i < 30; ++i) {
			double vel = 2-2.0/29*i;
			QColor color;
			if (vel > 1) color.setRgb(255,0,0,255*(vel-1));
			else color.setRgb(0,0,255,255*(1-vel));
			gradient.setColorAt(1.0/29*i, color);
		}
	} else {
		for (int i = 0; i < 30; ++i) {
			QColor color;
			color.setHsv(225.0/29*i, 255, 255);
			gradient.setColorAt(1.0/29*i, color);
		}
	}
	
	painter.setPen(QPen(QBrush("black"), 1));
	painter.setBrush(gradient);
	painter.drawRect(marginLeft, marginTop, barWidth, barHeight);
	
    CVector meanSpeed = getMeanFreeStream(CVector(-m_RotorOverhang,0,m_TowerHeight));
    double meanWindspeed = meanSpeed.VAbs();

    if (m_windInputType==SIMFILE || m_windInputType == AERODYN){
        meanWindspeed = 0;
        for (int i = 0; i < m_CurrentHHWindSpeed.size(); ++i) {
            meanWindspeed += m_CurrentHHWindSpeed.at(i) / m_CurrentHHWindSpeed.size();
		}
    }

    CVector curWindSpeed;
    double time;

    if (m_WindField){
        if (m_WindField->getIncludeShear()){
        curWindSpeed.Set(m_WindField->getMeanWindSpeedAtHub()*log(CVector(-m_RotorOverhang,0,m_HubHeight).z/m_WindField->getRoughnessLength())/log(m_WindField->getWindSpeedMeasurementHeight()/m_WindField->getRoughnessLength()),0,0);
        }
        else curWindSpeed.Set(m_WindField->getMeanWindSpeedAtHub(),0,0);
        curWindSpeed.RotZ(m_HorInflowAngle/180 * PI);
    }
    else{
        if (m_bStoreWake) time = m_Time.at(m_shownTimeIndex);
        else time = m_Time.at(m_Time.size()-1);
        curWindSpeed = getFreeStream(m_hubCoords[3],time);
    }


    double max, min, mean;
    QString strong;

    double range = m_module->GetQLLTDock()->GetRange();
    const double fac =  1 + 10*(1-range);


    if (component == 0){
        max = curWindSpeed.VAbs()*(1/fac+1);
        mean = curWindSpeed.VAbs();
        min = (1-1/fac)*curWindSpeed.VAbs();
        strong = "magnitude: ";
    }
    else if (component == 1){
        max = curWindSpeed.x + meanWindspeed*range;
        mean = curWindSpeed.x;
        min = curWindSpeed.x - meanWindspeed*range;
        strong = "x-component: ";
    }
    else if (component == 2){
        max = curWindSpeed.y + meanWindspeed*range;
        mean = curWindSpeed.y;
        min = curWindSpeed.y - meanWindspeed*range;
        strong = "y-component: ";
    }
    else if (component == 3){
        max = curWindSpeed.z + meanWindspeed*range;
        mean = curWindSpeed.z;
        min = curWindSpeed.z - meanWindspeed*range;
        strong = "z-component: ";
    }
    else if (component == 4){
        max = curWindSpeed.x + meanWindspeed*range;
        mean = curWindSpeed.x;
        min = curWindSpeed.x - meanWindspeed*range;
        strong = "axial component: ";
    }
    else if (component == 5){
        max = curWindSpeed.y + meanWindspeed*range;
        mean = curWindSpeed.y;
        min = curWindSpeed.y - meanWindspeed*range;
        strong = "tang. component: ";
    }
    else /*if (component == 6)*/{
        max = curWindSpeed.z + meanWindspeed*range;
        mean = curWindSpeed.z;
        min = curWindSpeed.z - meanWindspeed*range;
        strong = "radial component: ";
    }

	
	painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 12));
	painter.drawText(marginLeft+barWidth+5, marginTop+6,
                     QString(strong+"%1 m/s").arg(max, 0, 'f', 2));
	painter.drawText(marginLeft+barWidth+5, marginTop+barHeight/2+6,
                     QString(strong+"%1 m/s").arg(mean, 0, 'f', 2));
	painter.drawText(marginLeft+barWidth+5, marginTop+barHeight+6,
                     QString(strong+"%1 m/s").arg(min, 0, 'f', 2));
}

void QLLTSimulation::drawOverpaint(QPainter &painter) {
    if (g_mainFrame->getCurrentModule() == m_module && m_module->GLView) {
        if (m_module->GetQLLTDock()->m_showText->isChecked()){
            drawText(painter);
        };
        if (m_module->IsCutPlaneRendered() || (m_WindField && m_module->m_QLLTDock->m_showWindfield->isChecked())) {
            drawLegend(painter, m_module->m_QLLTDock->m_colorMapButton->button(0)->isChecked(), m_module->GetQLLTDock()->m_componentButton->checkedId());
        }
    }
}

void QLLTSimulation::drawText(QPainter &painter) {
    const double width = m_module->m_glWidget->width();
    const double height = m_module->m_glWidget->height();

	painter.setPen(g_mainFrame->m_TextColor);

    if (width > 300) {
		painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 15));
		painter.drawText(10, 25, QString(getName() + " : " + m_Blade->getName()));
	
		painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 10));
        if (hasStoredData()) {
            painter.drawText(width-220, 40, QString("Time: %1 s").arg(m_bStoreWake ?
                                                                          m_Time.at(m_shownTimeIndex) :
                                                                          m_Time.at(m_results.at(0)->size()-1)));
            if (m_bisVawt){
            painter.drawText(width-220, 60, QString("Averaged Power: %1 kW").arg(m_bStoreWake ?
                                                                      m_PowerAV.at(m_shownTimeIndex) :
                                                                      m_PowerAV.at(m_results.at(4)->size()-1)));
            }
            else painter.drawText(width-220, 60, QString("Power: %1 kW").arg(m_bStoreWake ?
                                                                            m_Power.at(m_shownTimeIndex) :
                                                                            m_Power.at(m_results.at(0)->size()-1)));
            if (m_bisVawt){
            painter.drawText(width-220, 80, QString("Averaged Cp: %1").arg(m_bStoreWake ?
                                                                      m_CPAV.at(m_shownTimeIndex) :
                                                                      m_CPAV.at(m_results.at(4)->size()-1)));
            }
            else{
            painter.drawText(width-220, 80, QString("Cp: %1").arg(m_bStoreWake ?
                                                                      m_CP.at(m_shownTimeIndex) :
                                                                      m_CP.at(m_results.at(4)->size()-1)));
            }

            painter.drawText(width-220, 100, QString("V_in @ hub: %1 m/s").arg(m_bStoreWake ?
                                                                            m_CurrentHHWindSpeed.at(m_shownTimeIndex) :
                                                                            m_CurrentHHWindSpeed.at(m_results.at(4)->size()-1)));


			if (!m_bStoreWake || m_savedWakeLines.size()) {
                painter.drawText(width-220, height-40, QString("Vortex Elements: %1").arg(m_bStoreWake ?
																				   m_savedWakeLines.at(m_shownTimeIndex).size() :
																				   m_WakeLine.size()));
			}
		}
    }
}

void QLLTSimulation::GammaConvergenceCheck(){
    //compares Gamma_old and Gamma at all stations
    //if somewhere the convergence condition is violated set m_bAllConverged = false

    m_bAllConverged = true;

    for(int i = 0; i< m_BladePanel.size();i++){
        m_BladePanel[i]->m_isConverged = true;
        m_BladePanel[i]->m_Gamma = m_BladePanel[i]->m_Gamma_last_iteration + m_RelaxFactor*(m_BladePanel[i]->m_Gamma - m_BladePanel[i]->m_Gamma_last_iteration);

        if(fabs(m_BladePanel[i]->m_Gamma - m_BladePanel[i]->m_Gamma_last_iteration) > m_Epsilon){
            m_bAllConverged = false;
            m_BladePanel[i]->m_isConverged = false;
        }
        m_BladePanel[i]->m_Gamma_last_iteration = m_BladePanel[i]->m_Gamma;
    }

    if (m_bCircStruts){
        for(int i = 0; i< m_StrutPanel.size();i++){
            m_StrutPanel[i]->m_isConverged = true;
            m_StrutPanel[i]->m_Gamma = m_StrutPanel[i]->m_Gamma_last_iteration + m_RelaxFactor*(m_StrutPanel[i]->m_Gamma - m_StrutPanel[i]->m_Gamma_last_iteration);

            if(fabs(m_StrutPanel[i]->m_Gamma - m_StrutPanel[i]->m_Gamma_last_iteration) > m_Epsilon){
                m_bAllConverged = false;
                m_StrutPanel[i]->m_isConverged = false;
            }
            m_StrutPanel[i]->m_Gamma_last_iteration = m_StrutPanel[i]->m_Gamma;
        }
    }
}

QStringList QLLTSimulation::getAvailableVariables(NewGraph::GraphType graphType) {
    switch (graphType) {
    case NewGraph::QLLTTimeGraph:
        return m_availableVariables;
    case NewGraph::QLLTBladeGraph:
        return m_availableBladeVariables;
    default:
        return QStringList();
    }
}

CVector QLLTSimulation::getFreeStream (CVector EvalPt, double time) {
        if (time == -1) time = m_currentTime;
        else UpdateSimulationParameters(time); // reconstruction for cut-plane
    if (m_WindField) {
        // rotor coordinates need to be transformed!!!!!! (instead of transforming windfield for skewed inflow)
        EvalPt.RotZ(-m_HorInflowAngle/180*PI);
        CVector V_free;
        V_free.Set(m_WindField->getWindspeed (EvalPt, time, -m_XWindfieldOffset));
        V_free.RotZ(m_HorInflowAngle/180*PI);
        return V_free;
    } else if (m_windInputType == CONSTANT){
        CVector Vz;
        if (m_bisPowerLaw) Vz.Set(m_InflowSpeed*pow(EvalPt.z/m_refHeight,m_ShearCoeff),0,m_VertInflowSpeed);
        else Vz.Set(m_InflowSpeed*log(EvalPt.z/m_ShearCoeff)/log(m_refHeight/m_ShearCoeff),0,m_VertInflowSpeed);
        Vz.RotZ(m_HorInflowAngle/180*PI);
        return Vz;
    }
    else if (m_windInputType == SIMFILE) {
        CVector V_free(0,0,0);
        EvalPt.RotZ(-m_HorInflowAngle/180*PI);
        V_free.x = V_in.x * pow((EvalPt.z/m_hubCoords[3].z),m_VertShear);
        V_free.z = V_in.z;
        V_free.RotZ(m_HorInflowAngle/180*PI);
        return V_free;
    }
    else if (m_windInputType == AERODYN) {
        CVector V_free(0,0,0);
        double radius = m_Blade->getRotorRadius();
        EvalPt.RotZ(-m_HorInflowAngle/180*PI);
        V_free.x = EvalPt.y/radius*V_in.x*m_HorShear/2 + (EvalPt.z-m_HubHeight)/radius*V_in.x*m_LinVertShear/2 + V_in.x * pow((EvalPt.z/m_hubCoords[3].z),m_VertShear)+m_GustSpeed;
        V_free.z = V_in.z;
        V_free.RotZ(m_HorInflowAngle/180*PI);
        return V_free;
    }
    return CVector(0,0,0);
}

CVector QLLTSimulation::getMeanFreeStream (CVector EvalPt) {
    if (m_WindField) {
        CVector V_free;
        if (m_WindField->getIncludeShear()){
        V_free.Set(m_WindField->getMeanWindSpeedAtHub()*log(EvalPt.z/m_WindField->getRoughnessLength())/log(m_WindField->getWindSpeedMeasurementHeight()/m_WindField->getRoughnessLength()),0,0);
        }
        else V_free.Set(m_WindField->getMeanWindSpeedAtHub(),0,0);
        V_free.RotZ(m_HorInflowAngle/180 * PI);
        return V_free;
    } else if (m_windInputType == CONSTANT){
        CVector Vz;
        if (m_bisPowerLaw) Vz.Set(m_InflowSpeed*pow(EvalPt.z/m_refHeight,m_ShearCoeff),0,m_VertInflowSpeed);
        else Vz.Set(m_InflowSpeed*log(EvalPt.z/m_ShearCoeff)/log(m_refHeight/m_ShearCoeff),0,m_VertInflowSpeed);
        Vz.RotZ(m_HorInflowAngle/180*PI);
        return Vz;
    }
    else if (m_windInputType == SIMFILE) {
        CVector V_free(0,0,0);
        EvalPt.RotZ(-m_HorInflowAngle/180*PI);
        V_free.x = V_in.x * pow((EvalPt.z/m_hubCoords[3].z),m_VertShear);
        V_free.z = V_in.z;
        V_free.RotZ(m_HorInflowAngle/180*PI);
        return V_free;
    }
    else if (m_windInputType == AERODYN) {
        CVector V_free(0,0,0);
        double radius = m_Blade->getRotorRadius();
        EvalPt.RotZ(-m_HorInflowAngle/180*PI);
        V_free.x = EvalPt.y/radius*V_in.x*m_HorShear/2 + (EvalPt.z-m_HubHeight)/radius*V_in.x*m_LinVertShear/2 + V_in.x * pow((EvalPt.z/m_hubCoords[3].z),m_VertShear)+m_GustSpeed;
        V_free.z = V_in.z;
        V_free.RotZ(m_HorInflowAngle/180*PI);
        return V_free;
    }
    return CVector(0,0,0);
}

CVector QLLTSimulation::getRelativeBladeVelocity(int ID){

    //calculates the rotational velocity based on then tangent of the current position so rotational velocity is insensitive against the azimuthal discretization
    //possible blade motions (pitching,yawing, deflection etc) are added via a first order backwards differential scheme
    //where the blade translation due to rotation is substracted because it was previously added via the tangent

    CVector  TangentVelocity, Motion, LastAzimuthPos;

    TangentVelocity = m_BladePanel[ID]->tangentialVector * m_Omega * m_BladePanel[ID]->radius;

    LastAzimuthPos = m_BladePanel[ID]->CtrlPt;
    LastAzimuthPos.Rotate(m_hubCoords[3], m_hubCoords[0],-180/PI*m_Omega*dT);
    Motion = ((m_BladePanel[ID]->CtrlPt - m_BladePanel[ID]->Old_CtrlPt) - (m_BladePanel[ID]->CtrlPt - LastAzimuthPos)) /dT;

    return TangentVelocity + Motion;

}

CVector QLLTSimulation::getRelativeStrutVelocity(int ID){

    //calculates the rotational velocity based on then tangent of the current position so rotational velocity is insensitive against the azimuthal discretization
    //possible blade motions (pitching,yawing, deflection etc) are added via a first order backwards differential scheme
    //where the blade translation due to rotation is substracted because it was previously added via the tangent

    CVector  TangentVelocity, Motion, LastAzimuthPos;

    TangentVelocity = m_StrutPanel[ID]->tangentialVector * m_Omega * m_StrutPanel[ID]->radius;

    LastAzimuthPos = m_StrutPanel[ID]->CtrlPt;
    LastAzimuthPos.Rotate(m_hubCoords[3], m_hubCoords[0],-180/PI*m_Omega*dT);
    Motion = ((m_StrutPanel[ID]->CtrlPt - m_StrutPanel[ID]->Old_CtrlPt) - (m_StrutPanel[ID]->CtrlPt - LastAzimuthPos)) /dT;

    return TangentVelocity + Motion;

}

void QLLTSimulation::GLDrawSimulationScene(bool replay){

    int m;

    if (replay && m_shownTimeIndex != -1) {
        m = m_shownTimeIndex;
    } else {
        m = m_savedBladeVizLines.size()-1;
    }

    hsv hs;
    hs.s = 1;
    hs.v = 1;

    glDisable(GL_DEPTH_TEST);

    glNewList(GLWINGWAKEPANELS, GL_COMPILE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    //Panele einfärben

    ///debug vectors


//    glColor4d(1,0,0,1);
//    glLineWidth(m_module->GetQLLTDock()->m_linesize->value());
//    glBegin(GL_LINES);
//    {
//    glVertex3d(m_ThrustActingPoint.at(m).x, m_ThrustActingPoint.at(m).y, m_ThrustActingPoint.at(m).z);
//    glVertex3d(m_ThrustActingPoint.at(m).x+m_ThrustVector.at(m).x, m_ThrustActingPoint.at(m).y+m_ThrustVector.at(m).y, m_ThrustActingPoint.at(m).z+m_ThrustVector.at(m).z);
//    }
//    glEnd();

    glPolygonOffset(1.0, 1.0);
    glLineWidth(m_module->GetQLLTDock()->m_linesize->value());
    double normalized;



//        for (int i=0; i<m_BladePanel.size();i++){
//            glBegin(GL_LINES);
//            {
//                glColor4d(0,1,0,1);

    //        glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z);
    //        glVertex3d(m_BladePanel[i]->CtrlPt.x+m_BladePanel[i]->LiftVector.x, m_BladePanel[i]->CtrlPt.y+m_BladePanel[i]->LiftVector.y, m_BladePanel[i]->CtrlPt.z+m_BladePanel[i]->LiftVector.z);
    //        glColor4d(0,0,1,1);

    //        glVertex3d(m_StrutPanel[i]->CtrlPt.x, m_StrutPanel[i]->CtrlPt.y, m_StrutPanel[i]->CtrlPt.z);
    //        glVertex3d(m_StrutPanel[i]->CtrlPt.x+m_StrutPanel[i]->DragVector.x*30, m_StrutPanel[i]->CtrlPt.y+m_StrutPanel[i]->DragVector.y*30, m_StrutPanel[i]->CtrlPt.z+m_StrutPanel[i]->DragVector.z*30);

    //        glColor4d(1,0,0,1);

    //        glVertex3d(m_StrutPanel[i]->CtrlPt.x, m_StrutPanel[i]->CtrlPt.y, m_StrutPanel[i]->CtrlPt.z);
    //        glVertex3d(m_StrutPanel[i]->CtrlPt.x+m_StrutPanel[i]->LiftVector.x*2, m_StrutPanel[i]->CtrlPt.y+m_StrutPanel[i]->LiftVector.y*2, m_StrutPanel[i]->CtrlPt.z+m_StrutPanel[i]->LiftVector.z*2);



    //        glColor4d(1,0,0,1);

    //        glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z);
    //        glVertex3d(m_BladePanel[i]->CtrlPt.x-m_BladePanel[i]->m_V_total.x, m_BladePanel[i]->CtrlPt.y-m_BladePanel[i]->m_V_total.y, m_BladePanel[i]->CtrlPt.z-m_BladePanel[i]->m_V_total.z);
    //        glColor4d(0,0,0,1);
    //        glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z);
    //        glVertex3d(m_BladePanel[i]->CtrlPt.x-m_BladePanel[i]->tangentialVector.x, m_BladePanel[i]->CtrlPt.y-m_BladePanel[i]->tangentialVector.y, m_BladePanel[i]->CtrlPt.z-m_BladePanel[i]->tangentialVector.z);
    //        glEnd();

//            glColor4d(1,0,0,1);
//            glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z);
//            glVertex3d(m_BladePanel[i]->CtrlPt.x+m_BladePanel[i]->tangentialVector.x*10, m_BladePanel[i]->CtrlPt.y+m_BladePanel[i]->tangentialVector.y*10, m_BladePanel[i]->CtrlPt.z+m_BladePanel[i]->tangentialVector.z*10);

//    //        glColor4d(1,0,0,1);
//    //        glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z);
//    //        glVertex3d(m_BladePanel[i]->CtrlPt.x-m_BladePanel[i]->a2.x*10, m_BladePanel[i]->CtrlPt.y-m_BladePanel[i]->a2.y*10, m_BladePanel[i]->CtrlPt.z-m_BladePanel[i]->a2.z*10);
//            glEnd();
//            }

//        }

        if (m_savedWakeLines.size()){
            for (int p=0; p<m_savedWakeLines.at(m).size(); p++){
                    if (m_savedWakeLines.at(m).at(p).fromStation < m_savedBladeVizLines.at(m).size()/m_numBlades || !m_bGLPlotSingleBlade){
                        if ((m_savedWakeLines.at(m).at(p).Shed && m_bGLPlotWakeLinesShed) || (!m_savedWakeLines.at(m).at(p).Shed && m_bGLPlotWakeLinesTrail)){
                        glBegin(GL_LINES);
                        {
                            glColor4d(0,0,0,1);

                            if (m_bGLColorStrength){
                            if (fabs(m_savedWakeLines.at(m).at(p).Gamma/m_maxGamma*2)>1) normalized = 1; else normalized = fabs(m_savedWakeLines.at(m).at(p).Gamma/m_maxGamma*2);
                            hs.h = (1-normalized)*225;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            if (m_bGLColorAge){
                            hs.h = m_savedWakeLines.at(m).at(p).fromTimeL%360;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            glVertex3d(m_savedWakeLines.at(m).at(p).Lx, m_savedWakeLines.at(m).at(p).Ly, m_savedWakeLines.at(m).at(p).Lz);
                            if (m_bGLColorAge){
                            hs.h = m_savedWakeLines.at(m).at(p).fromTimeT%360;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            glVertex3d(m_savedWakeLines.at(m).at(p).Tx, m_savedWakeLines.at(m).at(p).Ty, m_savedWakeLines.at(m).at(p).Tz);
                        }
                        glEnd();
                        }
                    glPointSize(m_module->GetQLLTDock()->m_pointsize->value());
                    if (m_bGLPlotWakeNodes){
                        glBegin(GL_POINTS);
                        {
                            glColor4d(0,0,0,1);
                            if (m_bGLColorAge){
                            hs.h = m_savedWakeLines.at(m).at(p).fromTimeL%360;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            if (m_bGLColorStrength){
                            if (fabs(m_savedWakeLines.at(m).at(p).Gamma/m_maxGamma)>1) normalized = 1; else normalized = fabs(m_savedWakeLines.at(m).at(p).Gamma/m_maxGamma);
                            hs.h = (1-normalized)*225;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            glVertex3d(m_savedWakeLines.at(m).at(p).Lx, m_savedWakeLines.at(m).at(p).Ly, m_savedWakeLines.at(m).at(p).Lz);

                            if (m_bGLColorAge){
                            hs.h = m_savedWakeLines.at(m).at(p).fromTimeT%360;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                            }
                            glVertex3d(m_savedWakeLines.at(m).at(p).Tx, m_savedWakeLines.at(m).at(p).Ty, m_savedWakeLines.at(m).at(p).Tz);

                        }
                        glEnd();
                    }
                }
            }
}
        glLineWidth(0.5);
        glPointSize(1.5);
            if (m_savedBladeVortexLines.size()){
                for (int p=0; p<m_savedBladeVortexLines.at(m).size(); p++){
                            if ((m_savedBladeVortexLines.at(m).at(p).Shed &&  m_bGLPlotWakeLinesShed) || (!m_savedBladeVortexLines.at(m).at(p).Shed && m_bGLPlotWakeLinesTrail))
                            glBegin(GL_LINES);
                            {
                                if (fabs(m_savedBladeVortexLines.at(m).at(p).Gamma/m_maxGamma)>1) normalized = 1; else normalized = fabs(m_savedBladeVortexLines.at(m).at(p).Gamma/m_maxGamma);
                                hs.h = (1-normalized)*225;
                                glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,1);
                                glVertex3d(m_savedBladeVortexLines.at(m).at(p).Lx, m_savedBladeVortexLines.at(m).at(p).Ly, m_savedBladeVortexLines.at(m).at(p).Lz);
                                glVertex3d(m_savedBladeVortexLines.at(m).at(p).Tx, m_savedBladeVortexLines.at(m).at(p).Ty, m_savedBladeVortexLines.at(m).at(p).Tz);
                            }
                            glEnd();

                        if (m_bGLPlotWakeNodes){
                            glBegin(GL_POINTS);
                            {
                                glColor4d(0,0,0,1);
                                glVertex3d(m_savedBladeVortexLines.at(m).at(p).Lx, m_savedBladeVortexLines.at(m).at(p).Ly, m_savedBladeVortexLines.at(m).at(p).Lz);
                                glVertex3d(m_savedBladeVortexLines.at(m).at(p).Tx, m_savedBladeVortexLines.at(m).at(p).Ty, m_savedBladeVortexLines.at(m).at(p).Tz);

                            }
                            glEnd();
                        }

                }
            }



    glEndList();






    glDisable(GL_DEPTH_TEST);

    glLineWidth(1.0);

    glNewList(GLBLADEOUTLINES, GL_COMPILE);
    {
        if(m_bGLPlotRotor){
            glColor4d(0,0,0,1);
            for (int p=0; p<m_savedBladeVizLines.at(m).size()-1; p++){

                glBegin(GL_LINES);
                {
                    glVertex3d(m_savedBladeVizLines.at(m).at(p).Lx, m_savedBladeVizLines.at(m).at(p).Ly, m_savedBladeVizLines.at(m).at(p).Lz);
                    glVertex3d(m_savedBladeVizLines.at(m).at(p).Tx, m_savedBladeVizLines.at(m).at(p).Ty, m_savedBladeVizLines.at(m).at(p).Tz);

                    if (!m_savedBladeVizLines.at(m).at(p).isTip){
                        glVertex3d(m_savedBladeVizLines.at(m).at(p).Tx, m_savedBladeVizLines.at(m).at(p).Ty, m_savedBladeVizLines.at(m).at(p).Tz);
                        glVertex3d(m_savedBladeVizLines.at(m).at(p+1).Tx, m_savedBladeVizLines.at(m).at(p+1).Ty, m_savedBladeVizLines.at(m).at(p+1).Tz);

                        glVertex3d(m_savedBladeVizLines.at(m).at(p).Lx, m_savedBladeVizLines.at(m).at(p).Ly, m_savedBladeVizLines.at(m).at(p).Lz);
                        glVertex3d(m_savedBladeVizLines.at(m).at(p+1).Lx, m_savedBladeVizLines.at(m).at(p+1).Ly, m_savedBladeVizLines.at(m).at(p+1).Lz);
                    }
                }
                glEnd();

           }

        }
    }
    glEndList();

    glNewList(GLBODYMESHPANELS, GL_COMPILE);
    {
    if(m_bGLPlotRotor){
            double max = 0;
            for (int p=0; p<m_savedBladeVizLines.at(m).size(); p++){
                if(fabs(m_savedBladeVizLines.at(m).at(p).Gamma) > max) max = fabs(m_savedBladeVizLines.at(m).at(p).Gamma);
            }
            if (max != 0){
            for (int p=0; p<m_savedBladeVizLines.at(m).size()-1; p++){

                glBegin(GL_QUADS);
                {
                    if(!m_savedBladeVizLines.at(m).at(p).isTip){

                        hs.h = (1-fabs(m_savedBladeVizLines.at(m).at(p).Gamma)/max)*225;
                        glColor4d(hsv2rgb(hs).r,hsv2rgb(hs).g,hsv2rgb(hs).b,1);

                    glVertex3d(m_savedBladeVizLines.at(m).at(p).Tx, m_savedBladeVizLines.at(m).at(p).Ty, m_savedBladeVizLines.at(m).at(p).Tz);
                    glVertex3d(m_savedBladeVizLines.at(m).at(p).Lx, m_savedBladeVizLines.at(m).at(p).Ly, m_savedBladeVizLines.at(m).at(p).Lz);

                        hs.h = (1-fabs(m_savedBladeVizLines.at(m).at(p+1).Gamma)/max)*225;
                        glColor4d(hsv2rgb(hs).r,hsv2rgb(hs).g,hsv2rgb(hs).b,1);

                    glVertex3d(m_savedBladeVizLines.at(m).at(p+1).Lx, m_savedBladeVizLines.at(m).at(p+1).Ly, m_savedBladeVizLines.at(m).at(p+1).Lz);
                    glVertex3d(m_savedBladeVizLines.at(m).at(p+1).Tx, m_savedBladeVizLines.at(m).at(p+1).Ty, m_savedBladeVizLines.at(m).at(p+1).Tz);
                    }
                }
                glEnd();
           }
        }
    }
        }
        glEndList();


    CVector X, Y, A, B, C, D, Foot, Hub, Top;
    double scale = -m_Blade->getRotorRadius()/4;

    Hub     = m_savedCoordinateSystems.at(m).at(1);
    Foot    = m_savedCoordinateSystems.at(m).at(2);
    Top     = m_savedCoordinateSystems.at(m).at(3);
    X       = m_savedCoordinateSystems.at(m).at(4);


    glNewList(GLTURBINECOORDINATES,GL_COMPILE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    if(m_bGLPlotTurbineModel){
    glBegin(GL_LINES);
    glLineWidth(1.0);

    glColor4d(0,0,0,1);
    glVertex3d(Foot.x, Foot.y, Foot.z);
    glVertex3d(Top.x, Top.y, Top.z);
    if (!m_bisVawt) glVertex3d(Top.x, Top.y, Top.z);
    if (!m_bisVawt) glVertex3d(Hub.x, Hub.y, Hub.z);
    glEnd();
    }

    if(m_bGLPlotTurbineModel){
    glBegin(GL_POLYGON);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glColor4d(0.8,0.8,0.8,1.0);

    A = Foot;
    B = Top;
    Y = X*(A-B);
    Y.Normalize();
    A = (Foot+X*scale+Y*scale);
    B = (Foot+X*scale-Y*scale);
    C = (Foot-X*scale-Y*scale);
    D = (Foot-X*scale+Y*scale);

    int disc = 100;

    for (int i=0; i<disc;i++){
    A = Foot+X*scale*sin(2*PI/disc*i)+Y*scale*cos(2*PI/disc*i);
        glVertex3d(A.x,A.y,A.z);
    }

    glEnd();
    glPointSize(5.0);
    glColor4d(0,0,1,1);

    glBegin(GL_POINTS);
    {
        glVertex3d(Top.x, Top.y, Top.z);
        glVertex3d(Foot.x, Foot.y, Foot.z);
        glVertex3d(Hub.x, Hub.y, Hub.z);
    }

    glEnd();
    glLineWidth(0.5);
    glColor4d(0,0,0,1);
    glBegin(GL_LINE_LOOP);
    {
        for (int i=0; i<disc;i++){
        A = Foot+X*scale*sin(2*PI/disc*i)+Y*scale*cos(2*PI/disc*i);
            glVertex3d(A.x,A.y,A.z);
        }
    }
    glEnd();
    }

    glEndList();
}

void QLLTSimulation::GammaBoundFixedPointIteration()
{
    //1)calculate panel velocities
    //2)calculate new AoA from velocities
    //3)read Cl and Cd from polar data
    //4)calculate new panel circulation
    //5)check for convergence

    iterations = 0;
    m_bAllConverged = false;
    // starting value for iteration comes from last wake timestep
    for(int i = 0; i < m_BladePanel.size(); ++i) m_BladePanel[i]->m_Gamma_last_iteration = m_BladePanel[i]->m_Gamma_t_minus_1;
    for(int i = 0; i < m_StrutPanel.size(); ++i) m_StrutPanel[i]->m_Gamma_last_iteration = m_StrutPanel[i]->m_Gamma_t_minus_1;

    while ((m_bAllConverged == false) && (iterations < m_MaxIterations)){

        //1)
        CalcBladePanelVelocities();

        //2)
        CalcAoA();

        //3)
        CalcSteadyPanelCoefficients();

        //4)
        CalcBladeCirculation();

        if (m_bCircStruts) CalcStrutIterationLoop();

        //5)
        GammaConvergenceCheck();

    iterations++;
    }

    if (!m_bCircStruts) CalcStrutIterationLoop();

    m_Iterations.append(iterations);

    if (!m_bAllConverged){
        QVector<double> problems;
        problems.append(m_currentTimeStep);
        problems.append(m_currentTime);
        for(int i = 0; i< m_BladePanel.size();i++){
            if(!m_BladePanel.at(i)->m_isConverged){
                problems.append(m_BladePanel.at(i)->fromBlade);
                problems.append(m_BladePanel.at(i)->fromBladelength);
            }
        }
        m_unconvergedPositions.append(problems);
    }

    // moved dynamic stall out of iteration loop for consistency with FAST; DS was influencing convergence heavily - often lead to faster convergence hich is questionable
    // alternatively replace CalcSteadyPanelCoefficients() with CalcDynamicBladeCoefficients() and remove CalcDynamicBladeCoefficients() below for in-iteration DS
    if (m_bisDynStall){
        CalcDynamicBladeCoefficients();
        CalcDynamicStrutCoefficients();
        CalcBladeCirculation();
        CalcStrutCirculation();
        StoreDSVars();
    }

    AssignGammaToWingLines();

    if ((m_currentTimeStep)%m_Nth_WakeStep == 0){
        for(int i = 0; i < m_BladePanel.size(); ++i) {
                m_BladePanel[i]->m_Gamma_t_minus_2 = m_BladePanel[i]->m_Gamma_t_minus_1;
                m_BladePanel[i]->m_Gamma_t_minus_1 = m_BladePanel[i]->m_Gamma;
                m_BladePanel[i]->m_oldInducedVelocities.append(m_BladePanel[i]->m_Store_Wake);
                if (m_BladePanel[i]->m_oldInducedVelocities.size()>=10) m_BladePanel[i]->m_oldInducedVelocities.removeFirst();
        }

        for(int i = 0; i < m_StrutPanel.size(); ++i) {
                m_StrutPanel[i]->m_Gamma_t_minus_2 = m_StrutPanel[i]->m_Gamma_t_minus_1;
                m_StrutPanel[i]->m_Gamma_t_minus_1 = m_StrutPanel[i]->m_Gamma;
                m_StrutPanel[i]->m_oldInducedVelocities.append(m_StrutPanel[i]->m_Store_Wake);
                if (m_StrutPanel[i]->m_oldInducedVelocities.size()>=10) m_StrutPanel[i]->m_oldInducedVelocities.removeFirst();
        }
    }

}

void QLLTSimulation::StoreDSVars(){

    for (int i=0;i<m_BladePanel.size();i++){
       for (int k=0;k<3;k++){
       m_BladePanel[i]->Q1[k] = m_BladePanel[i]->Q1_new[k];
       m_BladePanel[i]->Q2[k] = m_BladePanel[i]->Q2_new[k];
       m_BladePanel[i]->x[k] = m_BladePanel[i]->x_new[k];
       }

       m_BladePanel[i]->m_AoA_old = m_BladePanel[i]->m_AoA;
       m_BladePanel[i]->Cv_old = m_BladePanel[i]->Cv;

       m_BladePanel[i]->xLEV_old = m_BladePanel[i]->xLEV;
       m_BladePanel[i]->m_CL_old = m_BladePanel[i]->m_CL;
     }

    if (m_bCircStruts){
        for (int i=0;i<m_StrutPanel.size();i++){
           for (int k=0;k<3;k++){
           m_StrutPanel[i]->Q1[k] = m_StrutPanel[i]->Q1_new[k];
           m_StrutPanel[i]->Q2[k] = m_StrutPanel[i]->Q2_new[k];
           m_StrutPanel[i]->x[k] = m_StrutPanel[i]->x_new[k];
           }

           m_StrutPanel[i]->m_AoA_old = m_StrutPanel[i]->m_AoA;
           m_StrutPanel[i]->Cv_old = m_StrutPanel[i]->Cv;

           m_StrutPanel[i]->xLEV_old = m_StrutPanel[i]->xLEV;
           m_StrutPanel[i]->m_CL_old = m_StrutPanel[i]->m_CL;
         }
    }
}



void QLLTSimulation::InitializePanelPositions(){

    //save initial rotor position for windfield computation
    m_XWindfieldOffset = m_hubCoords[3].x;

    for (int i=0;i<m_BladePanel.size();i++){
        m_BladePanel.at(i)->Old_CtrlPt = m_BladePanel.at(i)->CtrlPt;
        m_BladePanel.at(i)->Old_CtrlPt.Rotate(m_hubCoords[3], m_hubCoords[0],-180/PI*m_Omega*dT);
    }

    for (int i=0;i<m_StrutPanel.size();i++){
        m_StrutPanel.at(i)->Old_CtrlPt = m_StrutPanel.at(i)->CtrlPt;
        m_StrutPanel.at(i)->Old_CtrlPt.Rotate(m_hubCoords[3], m_hubCoords[0],-180/PI*m_Omega*dT);
    }
    m_bGlChanged = true;
}


void QLLTSimulation::InitMultiThreading(){

        if (m_module->GetDeviceType() == 0){
            m_bisOpenMp = false;
        }
        else if (m_module->GetDeviceType() == 1){
            m_bisOpenMp = true;
        }


}

void QLLTSimulation::InitializeBladePanelProperties()
{
        int blade = 0, pos=0;

        for (int i=0;i<m_BladePanel.size();i++){
            m_BladePanel[i]->fromBlade = blade;
            if (i % (m_BladePanel.size()/m_numBlades) == 0){
                pos=0;
                m_BladePanel[i]->isHub = true;
            }
            if ((i+1) % (m_BladePanel.size()/m_numBlades) == 0){
                m_BladePanel[i]->isTip = true;
                blade += 1;
            }

            m_BladePanel[i]->chord = (m_Blade->TChord[pos]+m_Blade->TChord[pos+1])/2;
            m_BladePanel[i]->thickness = (m_Blade->TThickness[pos]+m_Blade->TThickness[pos+1])/2;
            m_BladePanel[i]->fromBladelength = (m_Blade->TPos[pos]+m_Blade->TPos[pos+1])/2;
            m_BladePanel[i]->twistAngle = (m_Blade->TTwist[pos]+m_Blade->TTwist[pos+1])/2;
            if (m_bisVawt) m_BladePanel[i]->twistAngle = 90 - m_BladePanel[i]->twistAngle;

            pos++;
        }

        // initialize rotor lines for line vortices
        for (int i=0;i<m_BladePanel.size();i++){
            if (m_BladePanel[i]->isHub){
                VortexLine *line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLA;
                line->pT = m_BladePanel[i]->pTA;
                line->isHub = true;
                line->fromStation = m_BladeLine.size()-1;
                line->rightPanel = m_BladePanel.at(i);

                line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLB;
                line->pT = m_BladePanel[i]->pTB;
                line->fromStation = m_BladeLine.size()-1;
                line->leftPanel = m_BladePanel.at(i);
                if (m_BladePanel[i]->isTip){
                    line->isTip = true;
                }
                else{
                    line->rightPanel = m_BladePanel.at(i+1);
                }

            }
            else{
                VortexLine *line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLB;
                line->pT = m_BladePanel[i]->pTB;
                line->fromStation = m_BladeLine.size()-1;
                line->leftPanel = m_BladePanel.at(i);
                if (m_BladePanel[i]->isTip){
                    line->isTip = true;
                }
                else{
                    line->rightPanel = m_BladePanel.at(i+1);
                }
            }
        }
}

void QLLTSimulation::InitializeStrutPanelProperties()
{
        for (int i=0;i<m_StrutPanel.size();i++){
            m_StrutPanel[i]->chord = VortexNode(*m_StrutPanel[i]->pLA-*m_StrutPanel[i]->pTA).VAbs();
        }

        if (m_bCircStruts){
            // initialize strut lines for line vortices
            for (int i=0;i<m_StrutPanel.size();i++){
                if (m_StrutPanel[i]->isHub){
                    VortexLine *line = new VortexLine;
                    m_BladeLine.push_back(line);
                    line->pL = m_StrutPanel[i]->pLA;
                    line->pT = m_StrutPanel[i]->pTA;
                    line->isStrut = true;
                    line->isHub = true;
                    line->fromStation = m_BladeLine.size()-1;
                    line->rightPanel = m_StrutPanel.at(i);

                    line = new VortexLine;
                    m_BladeLine.push_back(line);
                    line->pL = m_StrutPanel[i]->pLB;
                    line->pT = m_StrutPanel[i]->pTB;
                    line->isStrut = true;
                    line->fromStation = m_BladeLine.size()-1;
                    line->leftPanel = m_StrutPanel.at(i);
                    if (m_StrutPanel[i]->isTip){
                        line->isTip = true;
                    }
                    else{
                        line->rightPanel = m_StrutPanel.at(i+1);
                    }

                }
                else{
                    VortexLine *line = new VortexLine;
                    m_BladeLine.push_back(line);
                    line->pL = m_StrutPanel[i]->pLB;
                    line->pT = m_StrutPanel[i]->pTB;
                    line->isStrut = true;
                    line->fromStation = m_BladeLine.size()-1;
                    line->leftPanel = m_StrutPanel.at(i);
                    if (m_StrutPanel[i]->isTip){
                        line->isTip = true;
                    }
                    else{
                        line->rightPanel = m_StrutPanel.at(i+1);
                    }
                }
            }
        }
}


VortexNode *QLLTSimulation::IsNode(CVector &Pt, bool strutNodes)
{
        //
        // returns the index of a node if found, else returns NULL
        //
        int in;
        // explore in reverse order, since we have better chance of
        // finding the node close to the last point when creating elements
        //
        if (strutNodes){
            for (in=m_StrutNode.size()-1; in>=0; in--)
            {
                    if(Pt.IsSame(*m_StrutNode[in])) return m_StrutNode[in];
            }
        }
        else{
            for (in=m_BladeNode.size()-1; in>=0; in--)
            {
                    if(Pt.IsSame(*m_BladeNode[in])) return m_BladeNode[in];
            }
        }
        return NULL;
}

VortexNode* QLLTSimulation::IsWakeNode(CVector &Pt)
{
        //
        // returns the index of a wake node if found, else returns NULL
        //


        int in;
        for (in=0; in<m_WakeNode.size(); in++)
        {
                if(Pt.IsSame(*m_WakeNode[in])) return m_WakeNode[in];
        }
        return NULL;
}

void QLLTSimulation::KuttaCondition()
{
    //apply the kutto kondition to shed and trailing wake elements
        for (int i=0; i<m_WakeLine.size();i++){
            if (m_WakeLine.at(i)->fromTimestep == m_currentTimeStep){
                if (m_WakeLine.at(i)->isTrailing){
                    m_WakeLine.at(i)->Gamma = m_WakeLine.at(i)->fromLine->Gamma;
                }
                else if (m_WakeLine.at(i)->isShed){
                    if (m_currentTimeStep==0) m_WakeLine.at(i)->Gamma = -m_WakeLine.at(i)->rightPanel->m_Gamma;
                    else m_WakeLine.at(i)->Gamma = m_WakeLine.at(i)->rightPanel->m_Gamma_t_minus_2-m_WakeLine.at(i)->rightPanel->m_Gamma_t_minus_1;
                }
            }
        }
}

void QLLTSimulation::lockStores(){
    QString message = "Objects cannot be saved or removed because a LLT simulation or animation is running\n- Stop the simulation or animation first";

    g_windFieldStore.lockStore();
    g_windFieldStore.setLockMessage(message);
    g_rotorStore.lockStore();
    g_rotorStore.setLockMessage(message);
    g_360PolarStore.lockStore();
    g_360PolarStore.setLockMessage(message);
    g_verticalRotorStore.lockStore();
    g_verticalRotorStore.setLockMessage(message);
    g_foilStore.lockStore();
    g_foilStore.setLockMessage(message);
    g_polarStore.lockStore();
    g_polarStore.setLockMessage(message);
    g_QLLTVAWTSimulationStore.lockStore();
    g_QLLTVAWTSimulationStore.setLockMessage(message);
    g_QLLTHAWTSimulationStore.lockStore();
    g_QLLTHAWTSimulationStore.setLockMessage(message);
}

QLLTSimulation* QLLTSimulation::newBySerialize() {
    QLLTSimulation* simulation = new QLLTSimulation ();
    simulation->serialize();
    return simulation;
}

NewCurve* QLLTSimulation::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType) {
	if (!m_BladeOutputData.size()) return NULL;
	if (!m_results.size()) return NULL;
	
	switch (graphType) {
	case NewGraph::QLLTTimeGraph:
	{
		const int xAxisIndex = m_availableVariables.indexOf(xAxis);
		const int yAxisIndex = m_availableVariables.indexOf(yAxis);
		const bool xAxisIsBladeData = xAxisIndex >= m_results.size();
		const bool yAxisIsBladeData = yAxisIndex >= m_results.size();
		if (xAxisIndex == -1 || yAxisIndex == -1) {
			return NULL;
		}
		else if (!xAxisIsBladeData && !yAxisIsBladeData) {
			NewCurve *curve = new NewCurve (this);
			curve->setAllPoints(m_results[xAxisIndex]->data(),
								m_results[yAxisIndex]->data(),
								m_results[0]->size());  // numberOfRows is the same for all results
			return curve;
		}
		else if (xAxisIsBladeData && yAxisIsBladeData) {
			NewCurve *curve = new NewCurve (this);
			QVector<double> xOut, yOut;
			for (int m=0;m<m_BladeOutputData.size();m++){
				if (m_BladeOutputData.at(m).size() > xAxisIndex-m_results.size()) xOut.append(BladeOutputAtSection(m_BladeOutputData.at(m).at(xAxisIndex-m_results.size()), m_shownBladeSection));
				if (m_BladeOutputData.at(m).size() > yAxisIndex-m_results.size()) yOut.append(BladeOutputAtSection(m_BladeOutputData.at(m).at(yAxisIndex-m_results.size()), m_shownBladeSection));
			}
			curve->setAllPoints(xOut.data(), yOut.data(), m_BladeOutputData.size());  // numberOfRows is the same for all results
			return curve;
		}
		else if (xAxisIsBladeData && !yAxisIsBladeData) {
			NewCurve *curve = new NewCurve (this);
			QVector<double> xOut;
			for (int m=0;m<m_BladeOutputData.size();m++){
				if (m_BladeOutputData.at(m).size() > xAxisIndex-m_results.size()) xOut.append(BladeOutputAtSection(m_BladeOutputData.at(m).at(xAxisIndex-m_results.size()), m_shownBladeSection));
			}
			curve->setAllPoints(xOut.data(), m_results[yAxisIndex]->data(), m_BladeOutputData.size());  // numberOfRows is the same for all results
			return curve;
		}
		else if (!xAxisIsBladeData && yAxisIsBladeData) {
			NewCurve *curve = new NewCurve (this);
			QVector<double> yOut;
			for (int m=0;m<m_BladeOutputData.size();m++){
				if (m_BladeOutputData.at(m).size() > yAxisIndex-m_results.size()) yOut.append(BladeOutputAtSection(m_BladeOutputData.at(m).at(yAxisIndex-m_results.size()), m_shownBladeSection));
			}
			curve->setAllPoints(m_results[xAxisIndex]->data(), yOut.data(), m_BladeOutputData.size());  // numberOfRows is the same for all results
			return curve;
		}
		else return NULL;
		break;
	}
	case NewGraph::QLLTBladeGraph:
	{
		if (m_shownTimeIndex == -1) return NULL;
		const int xAxisIndex = m_availableBladeVariables.indexOf(xAxis);
		const int yAxisIndex = m_availableBladeVariables.indexOf(yAxis);
		if (xAxisIndex == -1 || yAxisIndex == -1) {
			return NULL;
		}
		else{
			NewCurve *curve = new NewCurve (this);
			curve->setAllPoints(m_BladeOutputData[m_shownTimeIndex][xAxisIndex].data(),
								m_BladeOutputData[m_shownTimeIndex][yAxisIndex].data(),
								m_BladeOutputData[0][0].size());  // numberOfRows is the same for all results
			return curve;
		}
		return NULL;
		break;
	}
	default:
		return NULL;
	}
}

void QLLTSimulation::ComputeCutPlaneVelocitiesOpenMP(QLLTCutPlane *plane, int timestep){

    #pragma omp parallel default (none) shared (plane, timestep)
    {
        #pragma omp for
        for (int i=0;i<plane->m_points.size();i++){
            for (int j=0;j<plane->m_points.at(i).size();j++){
                plane->m_velocities[i][j]=(CalcTowerInfluence(plane->m_points.at(i).at(j),BiotSavartInductionFromSave(plane->m_points.at(i).at(j),timestep)+getFreeStream(plane->m_points.at(i).at(j), plane->m_time),timestep));
            }
        }
    }
}

void QLLTSimulation::ComputeVolumeVelocitiesOpenMP(CVector*** positions, CVector*** velocities, int XSTART, int XEND, int YR, int ZR, int timestep, double time){

    int XDELTA = XEND-XSTART;
    #pragma omp parallel default (none) shared (positions, velocities, YR, ZR, timestep, time, XDELTA, XSTART)
    {
        #pragma omp for
        for (int i=0;i<XDELTA;i++){
            for (int j=0;j<YR;j++){
                for (int k=0;k<ZR;k++){
                velocities[i+XSTART][j][k] = (CalcTowerInfluence(positions[i+XSTART][j][k],BiotSavartInductionFromSave(positions[i+XSTART][j][k],timestep)+getFreeStream(positions[i+XSTART][j][k], time),timestep));
                }
            }
        }
    }
}

void QLLTSimulation::onComputeCutPlane(QLLTCutPlane *plane, int timestep){

    if (!plane) return;

    InitMultiThreading();

    ComputeCutPlaneVelocitiesOpenMP(plane, timestep);

    plane->m_curVelocity = getFreeStream(m_hubCoords[3], plane->m_time);

    CVector meanSpeed = getMeanFreeStream(CVector(-m_RotorOverhang,0,m_TowerHeight));
    double meanWindspeed = meanSpeed.VAbs();
    if (m_windInputType==SIMFILE || m_windInputType == AERODYN){
        meanWindspeed = 0;
//        int speedpos;
//        if (m_bisVawt) speedpos = 17;
//        else speedpos = 12;
        for (int i = 0; i < m_CurrentHHWindSpeed.size(); ++i) {
            meanWindspeed += m_CurrentHHWindSpeed.at(i) /m_CurrentHHWindSpeed.size();
        }
    }
    plane->m_meanVelocity = meanWindspeed;

    plane->is_computed = true;
}

void QLLTSimulation::onComputeVelVolume(CVector*** positions, CVector*** velocities, int XSTART, int XEND, int YR, int ZR, int timestep, double time){

ComputeVolumeVelocitiesOpenMP(positions, velocities, XSTART, XEND, YR, ZR, timestep, time);

}




void QLLTSimulation::RotorSpeedup(){
    if (m_currentTimeStep == 0) return;
    double torque = m_Torque.at(m_Torque.size()-1);
    m_Omega += torque/m_momInertia*dT;
}

void QLLTSimulation::SetBoundaryConditions(){

    if (m_windInputType == AERODYN ||  m_windInputType == SIMFILE) UpdateSimulationParameters(m_currentTime);
    if (m_bisStartup) RotorSpeedup();
    if (m_bAdaptive && m_Omega * 60 / 2 / PI > 0.5) dT = m_AngularDisc/360.0 * 2.0 * PI / m_Omega;
    if (m_bisStandStill) m_Omega = 0;
}


void QLLTSimulation::onStartAnalysis() {
    qDebug() << "start analysis";

    InitMultiThreading();

    connect(this, SIGNAL(geomChanged()),  m_module, SLOT(UpdateView()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(updateProgress(int)), m_module->m_QLLTDock, SLOT(updateProgress(int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(updateGraphs()), m_module, SLOT(reloadAllGraphs()), Qt::BlockingQueuedConnection);

    if (!m_bContinue) ResetSimulation();
    if (m_bContinue) m_currentTimeStep++;

    lockStores();

    m_bStopRequested = false;
    m_bisRunning = true;

    //start timer
    QElapsedTimer timer;
    timer.start();

    for (; m_currentTimeStep <= m_numTimesteps; ++m_currentTimeStep) {

        emit updateProgress(m_currentTimeStep);

        qDebug() << "start calculation for timestep:" << m_currentTimeStep << "Input Type"<<m_windInputType<< "Azi Pos Blade 1"<<m_currentAzimuthalPosition;

        SetBoundaryConditions();

        UpdateRotorGeometry();

        if (m_currentTimeStep == m_Nth_WakeStep){
            //start first wake timestep
            AddFirstWake();
            KuttaCondition();
        }
        else if(m_currentTimeStep > m_Nth_WakeStep && (m_currentTimeStep)%m_Nth_WakeStep == 0){
            //start sucessive wake timesteps
            TruncateWake();
            LumpWake();                         m_t_overhead += timer.restart();
            ConvectWake();                      m_t_induction+=timer.restart();
            AddNewWake();
            KuttaCondition();
        }

        //iteration for wing circulation distribution
                                                m_t_overhead+=timer.restart();
        GammaBoundFixedPointIteration();        m_t_induction+=timer.restart();

        storeOutput();

        if (!m_results.at(0)->isEmpty()) m_shownTimeIndex = m_results.at(0)->size() - 1;
        else m_shownTimeIndex = -1;
        m_module->setShownTimeForAllSimulations();

        emit geomChanged();
        emit updateGraphs();

        m_currentAzimuthalPosition += m_Omega*dT/PI*180;
        m_currentTime += dT;

        if (m_bStopRequested)
            break;
    }

    if(m_currentTimeStep == m_numTimesteps) {
        m_bContinue = false;
    }

    //timer

    m_t_overhead += timer.elapsed();
    double timeover = double(m_t_overhead / 1000.0);
    double timeinduction = double(m_t_induction / 1000.0);

    qDebug() << "Process took: " << timeover+timeinduction << "seconds " << "[" << timeover << "overhead and" << timeinduction << " calculation time ] ratio is:"<<int(100*timeover/(timeover+timeinduction)) << "% to" << int(100*timeinduction/(timeover+timeinduction))<<"%" << endl;

    m_bisRunning = false;

    disconnect(this, SIGNAL(geomChanged()), 0, 0);
    disconnect(this, SIGNAL(updateGraphs()), 0, 0);
    disconnect(this, SIGNAL(updateProgress(int)), 0, 0);

    unlockStores();
}

void QLLTSimulation::VortexSplitting(){
    /////////////////
    // \brief ConvectWakeOpenMP
    double maxStrain = 2;
    QList<VortexNode *> newNodes;
    QList<VortexLine *> newLines;


    for (int i=m_WakeLine.size()-1;i>=0;i--){


        if (m_WakeLine.at(i)->stretchFactor > maxStrain){

//                    qDebug() << "STRAIN"<<m_WakeLine.at(i)->stretchFactor<<m_WakeLine.at(i)->isShed;

//            //                    m_WakeLine.at(i)->Delete();

            VortexNode *pNode = m_WakeLine.at(i)->pT;
            pNode->detachLine(m_WakeLine.at(i));

            VortexNode *newNode = new VortexNode(pNode->x/2+m_WakeLine.at(i)->pL->x/2,pNode->y/2+m_WakeLine.at(i)->pL->y/2, pNode->z/2+m_WakeLine.at(i)->pL->z/2);
            newNode->attachLine(m_WakeLine.at(i));
            newNode->fromTimestep = pNode->fromTimestep;
            newNode->fromStation = pNode->fromStation;
            newNode->fromBlade = pNode->fromBlade;
            newNode->wasConvected = pNode->wasConvected;
            m_WakeLine.at(i)->pT = newNode;

            m_WakeLine.at(i)->stretchFactor /= 2;
            m_WakeLine.at(i)->oldLength /= 2;
            m_WakeLine.at(i)->Length /= 2;


            VortexLine *newLine = new VortexLine;
            newNode->attachLine(newLine);
            pNode->attachLine(newLine);
            newLine->pT = pNode;
            newLine->pL = newNode;
            newLine->m_Lines = &m_WakeLine;
            newLine->m_Nodes = &m_WakeNode;

            newLine->SetGamma(m_WakeLine.at(i)->GetGamma());
            newLine->stretchFactor = m_WakeLine.at(i)->stretchFactor;
            newLine->oldLength = m_WakeLine.at(i)->oldLength;
            newLine->Length = m_WakeLine.at(i)->Length;
            newLine->isHub = m_WakeLine.at(i)->isHub;
            newLine->isTip = m_WakeLine.at(i)->isTip;
            newLine->fromTimestep = m_WakeLine.at(i)->fromTimestep;
            newLine->includeStrain = m_bIncludeStrain;
            newLine->fromTime = m_WakeLine.at(i)->fromTime;
            newLine->fromRevolution = m_WakeLine.at(i)->fromRevolution;
            newLine->VizGamma = m_WakeLine.at(i)->VizGamma;
            newLine->age = m_WakeLine.at(i)->age;
            newLine->coreSizeSquared = m_WakeLine.at(i)->coreSizeSquared;
            newLine->m_KinViscosity = m_WakeLine.at(i)->m_KinViscosity;
            newLine->m_TurbulentViscosity = m_WakeLine.at(i)->m_TurbulentViscosity;
            newLine->m_VortexTimeOffset = m_WakeLine.at(i)->m_VortexTimeOffset;
            newLine->fromStation = m_WakeLine.at(i)->fromStation;
            newLine->isConcentrated = m_WakeLine.at(i)->isConcentrated;
            newLine->isShed = m_WakeLine.at(i)->isShed;
            newLine->isTrailing = m_WakeLine.at(i)->isTrailing;



            newNodes.append(newNode);
            newLines.append(newLine);
        }
    }
    ////////////////////
    /// \brief TruncateWake
    ///
    for (int i=0; i<newNodes.size();i++) m_WakeNode.prepend(newNodes.at(i));
    for (int i=0; i<newLines.size();i++) m_WakeLine.prepend(newLines.at(i));
}


void QLLTSimulation::onStartReplay(){

    lockStores();
    connect(this, SIGNAL(updateProgress(int)), m_module->GetQLLTToolBar(), SLOT(SetTimeStepSlider(int)), Qt::BlockingQueuedConnection);

    m_bStopReplay = false;
    int i;
    while (!m_bStopReplay){
        i = m_module->GetQLLTToolBar()->GetTimeStepSlider()->value();
        if (i<m_module->GetQLLTToolBar()->GetTimeStepSlider()->maximum()) i++;
        else i=0;
        emit updateProgress(i);
//        Sleep(m_module->GetQLLTToolBar()->GetDelay()*1000);
        QThread::msleep (m_module->GetQLLTToolBar()->GetDelay()*1000);
    }

    disconnect(this, SIGNAL(updateProgress(int)), m_module->GetQLLTToolBar(), SLOT(SetTimeStepSlider(int)));
    unlockStores();
}

QStringList QLLTSimulation::prepareMissingObjectMessage() {
    if (g_QLLTModule->m_bisVAWT && g_QLLTVAWTSimulationStore.isEmpty()) {
        QStringList message1 = WindField::prepareMissingObjectMessage();
        QStringList message2 = CBlade::prepareMissingObjectMessage(true);
        if (!message1.isEmpty() && !message2.isEmpty()) {
            message1.removeLast();  // remove the "create WindField" hint
        }
        QStringList message = message1 + message2;
        if (message.isEmpty()) {
            if (g_mainFrame->m_iApp == QLLTMODULE) {
                message = QStringList(">>> Click 'New' to create a new LLT VAWT Simulation");
            } else {
                message = QStringList(">>> unknown hint");
            }
        }
        message.prepend("- No LLT VAWT Simulation in Database");
        return message;
    } else if (!g_QLLTModule->m_bisVAWT && g_QLLTHAWTSimulationStore.isEmpty()) {
            QStringList message1 = WindField::prepareMissingObjectMessage();
            QStringList message2 = CBlade::prepareMissingObjectMessage(false);
            if (!message1.isEmpty() && !message2.isEmpty()) {
                message1.removeLast();  // remove the "create WindField" hint
            }
            QStringList message = message1 + message2;
            if (message.isEmpty()) {
                if (g_mainFrame->m_iApp == QLLTMODULE) {
                    message = QStringList(">>> Click 'New' to create a new LLT HAWT Simulation");
                } else {
                    message = QStringList(">>> unknown hint");
                }
            }
            message.prepend("- No LLT HAWT Simulation in Database");
            return message;
    } else {
        return QStringList();
    }
}

void QLLTSimulation::PrepareOutputVectors() {

    m_results.clear();
    m_availableBladeVariables.clear();
    m_availableVariables.clear();

    if (!m_bisVawt){
        m_results.append(&m_Time);
        m_availableVariables.append("Time [s]");
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_angPos[i]);
            m_availableVariables.append("Azimuthal Position Blade "+QString().number(i+1,'f',0)+" [deg]");
        }
        m_results.append(&m_rotSpeed);
        m_availableVariables.append("Rotational Speed [rpm]");
        m_results.append(&m_TSR);
        m_availableVariables.append("Tip Speed Ratio [-]");
        m_results.append(&m_CurrentHHWindSpeed);
        m_availableVariables.append("Wind Speed at Hub Height [m/s]");
        m_results.append(&m_CP);
        m_availableVariables.append("Power Coefficient [-]");
        m_results.append(&m_CM);
        m_availableVariables.append("Torque Coefficient [-]");
        m_results.append(&m_CT);
        m_availableVariables.append("Thrust Coefficient [-]");
        m_results.append(&m_Power);
        m_availableVariables.append("Power [kW]");
        m_results.append(&m_Torque);
        m_availableVariables.append("Torque [Nm]");
        m_results.append(&m_ThrustAbs);
        m_availableVariables.append("Thrust [N]");
        m_results.append(&m_ThrustX);
        m_availableVariables.append("Thrust in X Direction [N]");
        m_results.append(&m_ThrustY);
        m_availableVariables.append("Thrust in Y Direction [N]");
        m_results.append(&m_ThrustZ);
        m_availableVariables.append("Thrust in Z Direction [N]");
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_BladeTangentialForce[i]);
            m_availableVariables.append("Total Tangential Load Blade "+QString().number(i+1,'f',0)+" [N]");
        }
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_BladeNormalForce[i]);
            m_availableVariables.append("Total Normal Load Blade "+QString().number(i+1,'f',0)+" [N]");
        }
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_CurrentPitchAngle[i]);
            m_availableVariables.append("Pitch Angle Blade "+QString().number(i+1,'f',0)+" [deg]");
        }
        m_results.append(&m_CurrentYawAngle);
        m_availableVariables.append("Yaw Angle [deg]");
        m_results.append(&m_CurrentHorizontalInflowAngle);
        m_availableVariables.append("Horizontal Inflow Angle [deg]");
        m_results.append(&m_CurrentVerticalInflowSpeed);
        m_availableVariables.append("Vertical Inflow Angle [deg]");
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_RootBendingMomentOOP[i]);
            m_availableVariables.append("Out Of Plane Root Bending Moment Blade "+QString().number(i+1,'f',0)+" [Nm]");
        }
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_RootBendingMomentIP[i]);
            m_availableVariables.append("In Plane Root Bending Moment Blade "+QString().number(i+1,'f',0)+" [Nm]");
        }
        m_results.append(&m_CurrentPlatformPitch);
        m_availableVariables.append("Platform Pitch Angle Y [deg]");
        m_results.append(&m_CurrentPlatformRoll);
        m_availableVariables.append("Platform Roll Angle X [deg]");
        m_results.append(&m_CurrentPlatformYaw);
        m_availableVariables.append("Platform Yaw Angle Z [deg]");
        m_results.append(&m_CurrentPlatformTranslationX);
        m_availableVariables.append("Platform Translation X [m]");
        m_results.append(&m_CurrentPlatformTranslationY);
        m_availableVariables.append("Platform Translation Y [m]");
        m_results.append(&m_CurrentPlatformTranslationZ);
        m_availableVariables.append("Platform Translation Z [m]");
        m_results.append(&m_CurrentCoreSize);
        m_availableVariables.append("Vortex Core Size Evolution from t=0s [m]");
        m_results.append(&m_Iterations);
        m_availableVariables.append("Iterations [-]");
        m_results.append(&m_ElapsedSimulationTime);
        m_availableVariables.append("Elapsed Simulation Time [s]");


        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Lift Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Drag Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Lift to Drag Ratio (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Circulation (at section) Blade "+QString().number(i+1,'f',0)+" [m^2/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Angle of Attack (at section) Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Length (at section) Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Chord (at section) Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Blade Twist Angle (at section) Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Normal Force Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Tangential Force Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Normal Force (at section) Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Tangential Force (at section) Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Dynamic Pressure (at section) Blade "+QString().number(i+1,'f',0)+" [Pa]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Total Velocity (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Total Velocity (w/o Induction) (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Inflow Velocity (w/o induction) (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Relative Velocity from Blade Movement (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Velocity induced from Wake (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Velocity induced from Tower (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Axial Induction (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Tangential Induction (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Reynolds Number (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Delta Re Blade - Re Polar (at section) Blade "+QString().number(i+1,'f',0)+" [-]");

        m_availableBladeVariables.clear();
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Lift Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Drag Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Lift to Drag Ratio Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Circulation Blade "+QString().number(i+1,'f',0)+" [m^2/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Angle of Attack Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Length Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Chord Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Blade Twist Angle Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Normal Force Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Tangential Force Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Normal Force Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Tangential Force Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Dynamic Pressure Blade "+QString().number(i+1,'f',0)+" [Pa]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Total Velocity (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Total Velocity (w/o Induction) (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Inflow Velocity (w/o induction) (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Relative Velocity from Blade Movement (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Velocity induced from Wake (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Velocity induced from Tower (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Axial Induction Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Tangential Induction Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Reynolds Number Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Delta Re Blade - Re Polar Blade "+QString().number(i+1,'f',0)+" [-]");
    }
    else if (m_bisVawt){
        m_results.append(&m_Time);
        m_availableVariables.append("Time [s]");
        for(int i=0;i<m_numBlades;i++){
        m_results.append(&m_angPos[i]);
        m_availableVariables.append("Azimuthal Position Blade "+QString().number(i+1,'f',0)+" [deg]");
        }
        m_results.append(&m_rotSpeed);
        m_availableVariables.append("Rotational Speed [rpm]");
        m_results.append(&m_TSR);
        m_availableVariables.append("Tip Speed Ratio [-]");
        m_results.append(&m_CurrentHHWindSpeed);
        m_availableVariables.append("Wind Speed at Hub Height [m/s]");
        m_results.append(&m_CP);
        m_availableVariables.append("Momentary Power Coefficient [-]");
        m_results.append(&m_CM);
        m_availableVariables.append("Momentary Torque Coefficient [-]");
        m_results.append(&m_CT);
        m_availableVariables.append("Momentary Thrust Coefficient [-]");
        m_results.append(&m_Power);
        m_availableVariables.append("Momentary Power [kW]");
        m_results.append(&m_Torque);
        m_availableVariables.append("Momentary Torque [Nm]");
        m_results.append(&m_ThrustAbs);
        m_availableVariables.append("Momentary Thrust [N]");
        m_results.append(&m_ThrustX);
        m_availableVariables.append("Momentary Thrust in X Direction [N]");
        m_results.append(&m_ThrustY);
        m_availableVariables.append("Momentary Thrust in Y Direction [N]");
        m_results.append(&m_ThrustZ);
        m_availableVariables.append("Momentary Thrust in Z Direction [N]");
        m_results.append(&m_CPAV);
        m_availableVariables.append("Averaged Power Coefficient [-]");
        m_results.append(&m_CTAV);
        m_availableVariables.append("Averaged Thrust Coefficient [-]");
        m_results.append(&m_CMAV);
        m_availableVariables.append("Averaged Torque Coefficient [-]");
        m_results.append(&m_PowerAV);
        m_availableVariables.append("Averaged Power [kW]");
        m_results.append(&m_TorqueAV);
        m_availableVariables.append("Averaged Torque [Nm]");
        m_results.append(&m_ThrustAV);
        m_availableVariables.append("Averaged Thrust [N]");
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_BladeTangentialForce[i]);
            m_availableVariables.append("Total Tangential Load Blade "+QString().number(i+1,'f',0)+" [m]");
        }
        for (int i=0;i<m_numBlades;i++){
            m_results.append(&m_BladeNormalForce[i]);
            m_availableVariables.append("Total Normal Load Blade "+QString().number(i+1,'f',0)+" [N]");
        }
        for(int i=0;i<m_numBlades;i++){
        m_results.append(&m_CurrentPitchAngle[i]);
        m_availableVariables.append("Pitch Angle Blade "+QString().number(i+1,'f',0)+" [deg]");
        }
        m_results.append(&m_CurrentHorizontalInflowAngle);
        m_availableVariables.append("Horizontal Inflow Angle [deg]");
        m_results.append(&m_CurrentVerticalInflowSpeed);
        m_availableVariables.append("Vertical Inflow Angle [deg]");
        m_results.append(&m_CurrentPlatformPitch);
        m_availableVariables.append("Platform Pitch Angle Y [deg]");
        m_results.append(&m_CurrentPlatformRoll);
        m_availableVariables.append("Platform Roll Angle X [deg]");
        m_results.append(&m_CurrentPlatformYaw);
        m_availableVariables.append("Platform Yaw Angle Z [deg]");
        m_results.append(&m_CurrentPlatformTranslationX);
        m_availableVariables.append("Platform Translation X [m]");
        m_results.append(&m_CurrentPlatformTranslationY);
        m_availableVariables.append("Platform Translation Y [m]");
        m_results.append(&m_CurrentPlatformTranslationZ);
        m_availableVariables.append("Platform Translation Z [m]");
        m_results.append(&m_CurrentCoreSize);
        m_availableVariables.append("Vortex Core Size Evolution from t=0s [m]");
        m_results.append(&m_Iterations);
        m_availableVariables.append("Iterations [-]");
        m_results.append(&m_ElapsedSimulationTime);
        m_availableVariables.append("Elapsed Simulation Time [s]");

        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Lift Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Drag Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Lift to Drag Ratio (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Circulation Blade "+QString().number(i+1,'f',0)+" [m^2/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Angle of Attack (at section) Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Height (at section) Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Chord (at section) Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Blade Twist Angle (at section) Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Normal Force Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Tangential Force Coefficient (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Normal Force (at section) Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Tangential Force (at section) Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Dynamic Pressure (at section) Blade "+QString().number(i+1,'f',0)+" [Pa]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Total Velocity (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Total Velocity (w/o Induction) (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Inflow Velocity (w/o induction) (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Relative Velocity from Blade Movement (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Velocity induced from Wake (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Velocity induced from Tower (in plane of section) Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Axial Induction a (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Interference Factor (1-a) (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Reynolds Number (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableVariables.append("Delta Re Blade - Re Polar (at section) Blade "+QString().number(i+1,'f',0)+" [-]");

        m_availableBladeVariables.clear();
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Lift Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Drag Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Lift to Drag Ratio Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Circulation Blade "+QString().number(i+1,'f',0)+" [m^2/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Angle of Attack Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Height Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Chord Blade "+QString().number(i+1,'f',0)+" [m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Blade Twist Angle Blade "+QString().number(i+1,'f',0)+" [deg]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Normal Force Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Tangential Force Coefficient Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Normal Force Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Tangential Force Blade "+QString().number(i+1,'f',0)+" [N/m]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Dynamic Pressure Blade "+QString().number(i+1,'f',0)+" [Pa]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Total Velocity (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Total Velocity (w/o Induction) (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Inflow Velocity (w/o induction) (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Relative Velocity from Blade Movement (in plane of sections) of Blade "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Velocity induced from Wake Blade (in plane of sections) of "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Velocity induced from Tower Blade (in plane of sections) of "+QString().number(i+1,'f',0)+" [m/s]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Axial Induction a Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Interference Factor (1-a) Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Reynolds Number Blade "+QString().number(i+1,'f',0)+" [-]");
        for (int i=0;i<m_numBlades;i++) m_availableBladeVariables.append("Delta Re Blade - Re Polar (at section) Blade "+QString().number(i+1,'f',0)+" [-]");
    }
}

void QLLTSimulation::ResetSimulation(){

    m_currentTimeStep = 0;
    m_currentAzimuthalPosition = m_aziStart;
    m_maxGamma = 0;
    m_currentTime = 0;
    m_t_overhead = 0;
    m_t_induction = 0;
    m_Abort = false;
    m_PitchAngle.clear();
    for (int i=0;i<m_numBlades;i++){
        m_PitchAngle.append(0);
    }
    m_PlatformTranslation.Set(0,0,0);
    if (m_bisStartup) m_Omega = m_initialRot/60.0*2.0*PI;

    for (int i=0;i<m_BladePanel.size();i++)
    {
        m_BladePanel[i]->m_Gamma = 0;
        m_BladePanel[i]->m_Gamma_last_iteration = 0;
    }

    for (int i=0;i<m_StrutPanel.size();i++)
    {
        m_StrutPanel[i]->m_Gamma = 0;
        m_StrutPanel[i]->m_Gamma_last_iteration = 0;
    }
    ClearVortexArrays();
    ClearOutputArrays();
    PrepareOutputVectors();
    CreateRotorGeometry();
}

void QLLTSimulation::restorePointers() {
    StorableObject::restorePointers();
    if (m_WindField) g_serializer.restorePointer(reinterpret_cast<StorableObject**>(&m_WindField));
    g_serializer.restorePointer(reinterpret_cast<StorableObject**>(&m_Blade));
}

void QLLTSimulation::LoadRotorCoords(bool replay){
    int m;
    if (replay && m_shownTimeIndex != -1) m = m_shownTimeIndex;
    else m = m_savedBladeVizLines.size()-1;

    if (VBAxes.size()*2+5 < m_savedCoordinateSystems.at(m).size()){
        for (int i=5;i<m_savedCoordinateSystems.at(m).size();i++){
        VBAxes.append(CVector(0,0,0));
        VBOrigins.append(CVector(0,0,0));
        i++;
        }
    }

    if (m==-1) return;

    VAxis    = m_savedCoordinateSystems.at(m).at(0);
    VHub     = m_savedCoordinateSystems.at(m).at(1);
    VFoot    = m_savedCoordinateSystems.at(m).at(2);
    VTop     = m_savedCoordinateSystems.at(m).at(3);
    int k=0;
    for (int i=5;i<m_savedCoordinateSystems.at(m).size();i++){
        VBAxes[k]=(m_savedCoordinateSystems.at(m).at(i));
        i++;
        VBOrigins[k]=(m_savedCoordinateSystems.at(m).at(i));
        k++;
    }
}

void QLLTSimulation::serialize() {
    StorableObject::serialize();
    ShowAsGraphInterface::serialize();

    bool dummy;

    if (g_serializer.m_archiveFormat >= 100027) g_serializer.readOrWriteBool(&m_bHimmelskamp);
    if (g_serializer.m_archiveFormat >= 100030) g_serializer.readOrWriteBool(&dummy);
    if (g_serializer.m_archiveFormat >= 100051) g_serializer.readOrWriteBool(&m_bIncludeStrain);
    if (g_serializer.m_archiveFormat >= 100055) g_serializer.readOrWriteDouble(&m_refHeight);


    if (g_serializer.m_archiveFormat >= 100031){
        g_serializer.readOrWriteBool(&m_bisStartup);
        g_serializer.readOrWriteDouble(&m_momInertia);
        g_serializer.readOrWriteDouble(&m_aziStart);
        g_serializer.readOrWriteDouble(&m_initialRot);
        g_serializer.readOrWriteDouble(&init_YawAngle);
        g_serializer.readOrWriteDouble(&init_ShaftTiltAngle);
        g_serializer.readOrWriteDouble(&init_PlatformRollAngleX);
        g_serializer.readOrWriteDouble(&init_PlatformPitchAngleY);
        g_serializer.readOrWriteDouble(&init_HorInflowAngle);
        g_serializer.readOrWriteDouble(&init_VertInflowSpeed);
        g_serializer.readOrWriteDouble(&init_dT);
        g_serializer.readOrWriteDouble(&init_AngularDisc);
        g_serializer.readOrWriteDouble(&init_InflowSpeed);
        g_serializer.readOrWriteDouble(&init_Omega);
    }

    if (g_serializer.m_archiveFormat >= 100054){
        g_serializer.readOrWriteBool(&m_bCircStruts);
        g_serializer.readOrWriteInt(&m_numStrutPanels);
    }


    if (g_serializer.m_archiveFormat >= 100049){
        g_serializer.readOrWriteBool(&m_bisPowerLaw);
    }


    if (g_serializer.m_archiveFormat >= 100033)
    {
        g_serializer.readOrWriteBool(&m_bisDynStall);
        g_serializer.readOrWriteDouble(&m_Tf);
        g_serializer.readOrWriteDouble(&m_Tp);
        g_serializer.readOrWriteDouble(&m_Tv);
        g_serializer.readOrWriteDouble(&m_Tvl);
    }

    if (g_serializer.m_archiveFormat >= 100034){
        g_serializer.readOrWriteDouble(&m_AoAPos);
        g_serializer.readOrWriteDouble(&m_BoundVortexPos);
    }

    if (g_serializer.m_archiveFormat >= 100036){
        g_serializer.readOrWriteBool(&m_bAdaptive);
    }
    else{
        m_bAdaptive = false;
    }

    if (g_serializer.m_archiveFormat >= 100037){
        g_serializer.readOrWriteBool(&m_bisCenterEval);
        g_serializer.readOrWriteBool(&m_bisPC2B);
        g_serializer.readOrWriteBool(&m_bisVortexLift);
    }
    else{
        m_bisCenterEval = false;
        m_bisPC2B = false;
        m_bisVortexLift = false;
    }

    if (g_serializer.m_archiveFormat >= 100039){
        g_serializer.readOrWriteDouble(&m_maxStretch);
    }
    else{
        m_maxStretch = 10e5;
    }

    if (g_serializer.m_archiveFormat >= 100043){
        g_serializer.readOrWriteInt(&m_Nth_WakeStep);
    }
    else{
        m_Nth_WakeStep = 1;
    }



    g_serializer.readOrWriteInt(&m_windInputType);

    bool isWind;
    if (g_serializer.isReadMode()) { g_serializer.readOrWriteBool(&isWind);}
    else { isWind = m_WindField; g_serializer.readOrWriteBool(&isWind); }
    if (isWind) g_serializer.readOrWriteStorableObject(&m_WindField);
    else m_WindField = NULL;

    g_serializer.readOrWriteStorableObject(&m_Blade);

    g_serializer.readOrWriteDoubleVector3D(&m_BladeOutputData);

    g_serializer.readOrWriteDoubleList2D(&m_SimParameters);
    g_serializer.readOrWriteStringList1D(&m_FileHeader);
    g_serializer.readOrWriteString(&m_FileName);

    g_serializer.readOrWriteDouble(&m_VortexTimeOffset);
    g_serializer.readOrWriteDouble(&m_TurbulentViscosity);
    g_serializer.readOrWriteDouble(&m_KinViscosity);
    g_serializer.readOrWriteDouble(&m_TowerHeight);
    g_serializer.readOrWriteDouble(&m_HubHeight);
    g_serializer.readOrWriteDouble(&m_Density);
    g_serializer.readOrWriteDouble(&m_RelaxFactor);
    g_serializer.readOrWriteDouble(&m_firstWakeRowLength);
    g_serializer.readOrWriteDouble(&TSR);
    g_serializer.readOrWriteDouble(&m_FullWakeRevs);
    g_serializer.readOrWriteDouble(&m_FineWakeRevs);
    g_serializer.readOrWriteDouble(&m_AngularDisc);
    g_serializer.readOrWriteDouble(&m_MaxWakeAge);
    g_serializer.readOrWriteDouble(&m_maxGamma);
    g_serializer.readOrWriteDouble(&m_InflowSpeed);
    if (g_serializer.m_archiveFormat >= 100044){
        g_serializer.readOrWriteDoubleVector1D(&m_PitchAngle);
    }
    else{
        double dummy;
        g_serializer.readOrWriteDouble(&dummy);
        m_PitchAngle.append(dummy);
    }

    g_serializer.readOrWriteDouble(&m_VertInflowSpeed);
    g_serializer.readOrWriteDouble(&m_HorInflowAngle);
    g_serializer.readOrWriteDouble(&m_PlatformYawAngleZ);
    g_serializer.readOrWriteDouble(&m_PlatformRollAngleX);
    g_serializer.readOrWriteDouble(&m_PlatformPitchAngleY);
    g_serializer.readOrWriteDouble(&m_ShaftTiltAngle);
    g_serializer.readOrWriteDouble(&m_YawAngle);
    g_serializer.readOrWriteDouble(&dT);
    g_serializer.readOrWriteDouble(&m_Epsilon);
    g_serializer.readOrWriteDouble(&m_Omega);
    g_serializer.readOrWriteDouble(&m_MinGammaFrac);
    g_serializer.readOrWriteDouble(&m_z_leishman);
    g_serializer.readOrWriteDouble(&m_currentTime);
    g_serializer.readOrWriteDouble(&m_TStart);
    g_serializer.readOrWriteDouble(&m_RotorOverhang);
    g_serializer.readOrWriteDouble(&m_TowerTopRadius);
    g_serializer.readOrWriteDouble(&m_TowerBottomRadius);
    g_serializer.readOrWriteDouble(&m_TowerDrag);
    g_serializer.readOrWriteDouble(&m_ShearCoeff);
    g_serializer.readOrWriteDouble(&m_shownBladeSection);
    g_serializer.readOrWriteDouble(&m_ConeAngle);

    g_serializer.readOrWriteInt(&m_numBlades);
    g_serializer.readOrWriteInt(&m_shownTimeIndex);
    g_serializer.readOrWriteInt(&m_DecFactor);
    g_serializer.readOrWriteInt(&m_numTimesteps);
    g_serializer.readOrWriteInt(&iterations);
    g_serializer.readOrWriteInt(&m_MaxIterations);
    g_serializer.readOrWriteInt(&m_currentTimeStep);
    g_serializer.readOrWriteInt(&m_InterpolationFactor);
    g_serializer.readOrWriteInt(&m_Thinfactor);
    g_serializer.readOrWriteInt(&m_nWakeColumn);
    g_serializer.readOrWriteInt(&m_numPanels);
    g_serializer.readOrWriteInt(&m_discType);

    if (g_serializer.m_archiveFormat >= 100045){
    g_serializer.readOrWriteInt(&m_WakeCounter);
    }
    else if (g_serializer.isReadMode()){
        if (m_bisStartup || m_windInputType == SIMFILE) m_WakeCounter = TIMESTEPS;
        else m_WakeCounter = REVOLUTIONS;
    }

    // missing entries for old project files are filled with zeroes (m_numblades is needed for this)
    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100044){
        for (int i=1;i<m_numBlades;i++) m_PitchAngle.append(m_PitchAngle[0]);
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100044){
        if (m_BladeOutputData.size()){
            QVector<double> dummy;
            for (int j=0;j<m_BladeOutputData[0][0].size();j++){
                dummy.append(0);
            }
            int varsize = m_BladeOutputData[0].size();
            for (int i=0;i<m_BladeOutputData.size();i++){
                for (int j=0;j<varsize;j++){
                    for (int b=0;b<m_numBlades;b++){
                    m_BladeOutputData[i].append(dummy);
                    }
                }
            }
        }
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100047){
        if (m_BladeOutputData.size()){
            for (int j=0;j<m_BladeOutputData.size();j++){
                QVector< QVector<double> > temp;
                for (int i=0;i<m_BladeOutputData[j].size()/m_numBlades;i++){
                    for (int l=0;l<m_numBlades;l++){
                        temp.append(m_BladeOutputData[j].at(l*m_BladeOutputData[j].size()/m_numBlades+i));
                    }
                }
               m_BladeOutputData[j].clear();
               m_BladeOutputData[j] = temp;
            }
        }
    }

    g_serializer.readOrWriteBool(&m_bShed);
    g_serializer.readOrWriteBool(&m_bGround);
    g_serializer.readOrWriteBool(&m_bTrail);
    g_serializer.readOrWriteBool(&m_bisStandStill);
    g_serializer.readOrWriteBool(&m_bIncludeTower);
    g_serializer.readOrWriteBool(&m_dummy);
    g_serializer.readOrWriteBool(&m_bTurbConvection);
    g_serializer.readOrWriteBool(&m_bStoreWake);
    g_serializer.readOrWriteBool(&m_bWakeRollUp);
    g_serializer.readOrWriteBool(&m_bisVawt);
    g_serializer.readOrWriteBool(&m_bVortexParticle);
    g_serializer.readOrWriteBool(&m_bPredictorCorrector);
    g_serializer.readOrWriteBool(&m_bUpWind);

    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformYaw);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformRoll);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformPitch);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformTranslationX);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformTranslationY);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentPlatformTranslationZ);
    g_serializer.readOrWriteDoubleVector1D(&m_Iterations);

    if (g_serializer.m_archiveFormat >= 100044){
        g_serializer.readOrWriteDoubleVector2D(&m_RootBendingMomentIP);
        g_serializer.readOrWriteDoubleVector2D(&m_RootBendingMomentOOP);
        }
    else{
        QVector<double> dummy;
        g_serializer.readOrWriteDoubleVector1D(&dummy);
        QVector<double> dummy2;
        for (int j=0;j<dummy.size();j++){
            dummy2.append(0);
        }
        m_RootBendingMomentIP.append(dummy);
        for (int i=1;i<m_numBlades;i++){
                m_RootBendingMomentIP.append(dummy2);
        }
        dummy.clear();
        g_serializer.readOrWriteDoubleVector1D(&dummy);
        m_RootBendingMomentOOP.append(dummy);
        for (int i=1;i<m_numBlades;i++){
                m_RootBendingMomentOOP.append(dummy2);
        }
    }

    g_serializer.readOrWriteDoubleVector1D(&m_CurrentVerticalInflowSpeed);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentHorizontalInflowAngle);
    g_serializer.readOrWriteDoubleVector1D(&m_CurrentYawAngle);

    if (g_serializer.m_archiveFormat >= 100044){
        g_serializer.readOrWriteDoubleVector2D(&m_CurrentPitchAngle);
    }
    else{
        QVector<double> dummy;
        g_serializer.readOrWriteDoubleVector1D(&dummy);
        QVector<double> dummy2;
        for (int j=0;j<dummy.size();j++){
            dummy2.append(0);
        }
        m_CurrentPitchAngle.append(dummy);
        for (int i=1;i<m_numBlades;i++){
                m_CurrentPitchAngle.append(dummy2);
        }
    }

    g_serializer.readOrWriteDoubleVector1D(&m_CurrentHHWindSpeed);
    g_serializer.readOrWriteDoubleVector1D(&m_ThrustZ);
    g_serializer.readOrWriteDoubleVector1D(&m_ThrustY);
    g_serializer.readOrWriteDoubleVector1D(&m_ThrustX);
    g_serializer.readOrWriteDoubleVector1D(&m_rotSpeed);

    if (g_serializer.m_archiveFormat >= 100044){
        g_serializer.readOrWriteDoubleVector2D(&m_angPos);
    }
    else{
        QVector<double> dummy;
        g_serializer.readOrWriteDoubleVector1D(&dummy);
        QVector<double> dummy2;
        for (int j=0;j<dummy.size();j++){
            dummy2.append(0);
        }
        m_angPos.append(dummy);
        for (int i=1;i<m_numBlades;i++){
                m_angPos.append(dummy2);
        }
    }

    g_serializer.readOrWriteDoubleVector1D(&m_Time);
    g_serializer.readOrWriteDoubleVector1D(&m_Torque);
    g_serializer.readOrWriteDoubleVector1D(&m_Power);
    g_serializer.readOrWriteDoubleVector1D(&m_TSR);
    g_serializer.readOrWriteDoubleVector1D(&m_CM);
    g_serializer.readOrWriteDoubleVector1D(&m_CT);
    g_serializer.readOrWriteDoubleVector1D(&m_CP);
    g_serializer.readOrWriteDoubleVector1D(&m_CPAV);

    if (g_serializer.m_archiveFormat >= 100048){
        g_serializer.readOrWriteDoubleVector2D(&m_BladeTangentialForce);
        g_serializer.readOrWriteDoubleVector2D(&m_BladeNormalForce);
    }
    else{
        QVector<double> dummy;
        for (int j=0;j<m_Time.size();j++){
            dummy.append(0);
        }
        for (int i=0;i<m_numBlades;i++){
            m_BladeTangentialForce.append(dummy);
            m_BladeNormalForce.append(dummy);
        }
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100029){
        for (int i=0; i<m_CPAV.size();i++){
            m_CTAV.append(0);
            m_CMAV.append(0);
            m_PowerAV.append(0);
            m_TorqueAV.append(0);
        }
    }
    else{
        g_serializer.readOrWriteDoubleVector1D(&m_CTAV);
        g_serializer.readOrWriteDoubleVector1D(&m_CMAV);
        g_serializer.readOrWriteDoubleVector1D(&m_PowerAV);
        g_serializer.readOrWriteDoubleVector1D(&m_TorqueAV);
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100047){
        for (int i=0; i<m_Time.size();i++){
            m_ThrustAbs.append(0);
            m_ThrustAV.append(0);
            m_ThrustActingPoint.append(CVector(0,0,0));
            m_ThrustVector.append(CVector(0,0,0));
        }
    }
    else{
        g_serializer.readOrWriteDoubleVector1D(&m_ThrustAbs);
        g_serializer.readOrWriteDoubleVector1D(&m_ThrustAV);

        if (g_serializer.isReadMode()){
            int n;
            n = g_serializer.readInt();
            for (int i=0;i<n;i++){
                CVector vec;
                vec.serialize();
                m_ThrustActingPoint.append(vec);
            }
            for (int i=0;i<n;i++){
                CVector vec;
                vec.serialize();
                m_ThrustVector.append(vec);
            }
        }
        else{
            int n = m_ThrustActingPoint.size();
            g_serializer.readOrWriteInt(&n);
            for (int i=0;i<m_ThrustActingPoint.size();i++){
                m_ThrustActingPoint[i].serialize();
            }
            for (int i=0;i<m_ThrustVector.size();i++){
                m_ThrustVector[i].serialize();
            }
        }
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100031)
        for (int i=0; i<m_CP.size();i++){
            m_ElapsedSimulationTime.append(0);
            m_TimePerTimestep.append(0);
        }
    else{
         g_serializer.readOrWriteDoubleVector1D(&m_ElapsedSimulationTime);
         g_serializer.readOrWriteDoubleVector1D(&m_TimePerTimestep);
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100038)
        for (int i=0; i<m_CP.size();i++){
            m_CurrentCoreSize.append(0);
        }
    else{
         g_serializer.readOrWriteDoubleVector1D(&m_CurrentCoreSize);
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100031){
        if (m_bisVawt) m_aziStart = 270;
        else m_aziStart = 0;
        init_YawAngle = m_YawAngle;
        init_ShaftTiltAngle = m_ShaftTiltAngle;
        init_PlatformRollAngleX = m_PlatformRollAngleX;
        init_PlatformPitchAngleY = m_PlatformPitchAngleY;
        init_HorInflowAngle = m_HorInflowAngle;
        init_VertInflowSpeed = m_VertInflowSpeed;
        init_dT = dT;
        init_AngularDisc = m_AngularDisc;
        init_InflowSpeed = m_InflowSpeed;
        init_Omega = m_Omega;
    }

    if (g_serializer.isReadMode() && g_serializer.m_archiveFormat < 100036){
        m_AoAPos = 1 - m_AoAPos;
        m_BoundVortexPos = 1 - m_BoundVortexPos;
    }


    m_PlatformTranslation.serialize();

    // serialize the visualization and post processing arrays
    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<DummyLine> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                DummyLine line;
                line.serialize();
                list.append(line);
            }
            m_savedWakeLines.append(list);
        }
    } else {
        g_serializer.writeInt(m_savedWakeLines.size());
        for (int i = 0; i < m_savedWakeLines.size(); ++i) {
            g_serializer.writeInt(m_savedWakeLines.at(i).size());
            for (int j = 0; j < m_savedWakeLines.at(i).size(); ++j) {
                m_savedWakeLines[i][j].serialize();
            }
        }
    }

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<double> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                double line;
                list.append(line);
            }
            m_savedWakeVortexParticles.append(list);
        }
    } else {
        g_serializer.writeInt(m_savedWakeVortexParticles.size());
        for (int i = 0; i < m_savedWakeVortexParticles.size(); ++i) {
            g_serializer.writeInt(m_savedWakeVortexParticles.at(i).size());
            for (int j = 0; j < m_savedWakeVortexParticles.at(i).size(); ++j) {
                g_serializer.readOrWriteDouble(&m_savedWakeVortexParticles[i][j]);
            }
        }
    }

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<DummyLine> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                DummyLine line;
                line.serialize();
                list.append(line);
            }
            m_savedBladeVizLines.append(list);
        }
    } else {
        g_serializer.writeInt(m_savedBladeVizLines.size());
        for (int i = 0; i < m_savedBladeVizLines.size(); ++i) {
            g_serializer.writeInt(m_savedBladeVizLines.at(i).size());
            for (int j = 0; j < m_savedBladeVizLines.at(i).size(); ++j) {
                m_savedBladeVizLines[i][j].serialize();
            }
        }
    }

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<DummyLine> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                DummyLine line;
                line.serialize();
                list.append(line);
            }
            m_savedBladeVortexLines.append(list);
        }
    } else {
        g_serializer.writeInt(m_savedBladeVortexLines.size());
        for (int i = 0; i < m_savedBladeVortexLines.size(); ++i) {
            g_serializer.writeInt(m_savedBladeVortexLines.at(i).size());
            for (int j = 0; j < m_savedBladeVortexLines.at(i).size(); ++j) {
                m_savedBladeVortexLines[i][j].serialize();
            }
        }
    }

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<CVector> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                CVector vec;
                vec.serialize();
                list.append(vec);
            }
            m_savedCoordinateSystems.append(list);
        }
    } else {
        g_serializer.writeInt(m_savedCoordinateSystems.size());
        for (int i = 0; i < m_savedCoordinateSystems.size(); ++i) {
            g_serializer.writeInt(m_savedCoordinateSystems.at(i).size());
            for (int j = 0; j < m_savedCoordinateSystems.at(i).size(); ++j) {
                m_savedCoordinateSystems[i][j].serialize();
            }
        }
    }

    if (g_serializer.m_archiveFormat >= 100049){
        if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100054) {
            int n = g_serializer.readInt();
            for (int i = 0; i < n; ++i) {
                int m = g_serializer.readInt();
                for (int j = 0; j < m; ++j) {
                    DummyLine line;
                    line.serialize();
                }
            }
        }
    }

    PrepareOutputVectors();
}

void QLLTSimulation::setShownTime(double shownTime) {
    if (hasStoredData()) {

        int timeIndex = (shownTime - m_TStart) / dT;

        if (timeIndex < 0) timeIndex = 0;

        if (timeIndex > m_results.at(0)->size()-1) timeIndex = m_results.at(0)->size()-1;

        if (fabs(m_results.at(0)->at(timeIndex) - shownTime) < 10e-5) {
            m_shownTimeIndex = timeIndex;
            return;
        }

        while (m_results.at(0)->at(timeIndex) < shownTime && timeIndex < m_results[0]->size()-1) {
            timeIndex++;
            if (fabs(m_results.at(0)->at(timeIndex) - shownTime) <10e-5) {
                m_shownTimeIndex = timeIndex;
                return;
            } else if (m_results.at(0)->at(timeIndex) > shownTime) {
                m_shownTimeIndex = -1;
                return;  // searched from lower to higher value but didn't find a matching one
            }
        }

        while (m_results.at(0)->at(timeIndex) > shownTime && timeIndex > 0) {
            timeIndex--;
            if (fabs(m_results.at(0)->at(timeIndex) - shownTime) <10e-5) {
                m_shownTimeIndex = timeIndex;
                return;
            } else if (m_results.at(0)->at(timeIndex) < shownTime) {
                m_shownTimeIndex = -1;
                return;  // searched from higher to lower value but didn't find a matching one
            }
        }

        if (shownTime > m_results.at(0)->at(m_results.at(0)->size()-1)){ m_shownTimeIndex = -1; return;}
        if (shownTime < m_results.at(0)->at(0) ) { m_shownTimeIndex = -1; return;}
    }
}

void QLLTSimulation::storeGeometry(){

    if (!m_bStoreWake || m_currentTimeStep == 0 || (m_currentTime >= m_TStart && m_currentTime < m_TStart+dT) || m_currentTime < m_TStart){
        m_savedWakeLines.clear();
        m_savedBladeVizLines.clear();
        m_savedBladeVortexLines.clear();
        m_savedCoordinateSystems.clear();
    }

    QList<DummyLine> vizLines;
    for (int v=0; v<m_WakeLine.size();v++){
        DummyLine d(m_WakeLine.at(v)->pL->x,m_WakeLine.at(v)->pL->y,m_WakeLine.at(v)->pL->z,m_WakeLine.at(v)->pT->x,m_WakeLine.at(v)->pT->y,m_WakeLine.at(v)->pT->z,m_WakeLine.at(v)->Gamma,m_WakeLine.at(v)->fromStation,m_WakeLine.at(v)->isShed,m_WakeLine.at(v)->coreSizeSquared, m_WakeLine.at(v)->pL->fromTimestep, m_WakeLine.at(v)->pT->fromTimestep);
        vizLines.append(d);
    }
    m_savedWakeLines.append(vizLines);

    QList<DummyLine> vizBladeLines;
    for (int v=0; v<m_BladeLine.size();v++){
        DummyLine d(m_BladeLine.at(v)->pL->x,m_BladeLine.at(v)->pL->y,m_BladeLine.at(v)->pL->z,m_BladeLine.at(v)->pT->x,m_BladeLine.at(v)->pT->y,m_BladeLine.at(v)->pT->z,m_BladeLine.at(v)->VizGamma,m_BladeLine.at(v)->fromStation,m_BladeLine.at(v)->isShed,m_BladeLine.at(v)->coreSizeSquared,0,0,m_BladeLine.at(v)->isHub,m_BladeLine.at(v)->isTip);
        vizBladeLines.append(d);
    }

    if (!m_bCircStruts){
            // because strut lines are not present in bladelines this is constructed just for viz purposes
            for (int i=0;i<m_StrutPanel.size();i++){
                if (m_StrutPanel[i]->isHub){
                    DummyLine d(m_StrutPanel.at(i)->pLA->x,m_StrutPanel.at(i)->pLA->y,m_StrutPanel.at(i)->pLA->z,m_StrutPanel.at(i)->pTA->x,m_StrutPanel.at(i)->pTA->y,m_StrutPanel.at(i)->pTA->z,0,0,0,0,0,0,m_StrutPanel.at(i)->isHub,m_StrutPanel.at(i)->isTip);
                    vizBladeLines.append(d);
                    DummyLine e(m_StrutPanel.at(i)->pLB->x,m_StrutPanel.at(i)->pLB->y,m_StrutPanel.at(i)->pLB->z,m_StrutPanel.at(i)->pTB->x,m_StrutPanel.at(i)->pTB->y,m_StrutPanel.at(i)->pTB->z,0,0,0,0,0,0,m_StrutPanel.at(i)->isHub,m_StrutPanel.at(i)->isTip);
                    vizBladeLines.append(e);
                }
                else{
                    DummyLine d(m_StrutPanel.at(i)->pLB->x,m_StrutPanel.at(i)->pLB->y,m_StrutPanel.at(i)->pLB->z,m_StrutPanel.at(i)->pTB->x,m_StrutPanel.at(i)->pTB->y,m_StrutPanel.at(i)->pTB->z,0,0,0,0,0,0,m_StrutPanel.at(i)->isHub,m_StrutPanel.at(i)->isTip);
                    vizBladeLines.append(d);
                }
            }
    }



    m_savedBladeVizLines.append(vizBladeLines);



    double a = 1.25643;
    double coreSizeSquared = 4*a*m_TurbulentViscosity*m_KinViscosity*(m_VortexTimeOffset);

    QList<DummyLine> vortexBladeLines;
    for (int v=0; v<m_BladePanel.size();v++){

        DummyLine lalb(m_BladePanel.at(v)->VortPtA.x,m_BladePanel.at(v)->VortPtA.y,m_BladePanel.at(v)->VortPtA.z,m_BladePanel.at(v)->VortPtB.x,m_BladePanel.at(v)->VortPtB.y,m_BladePanel.at(v)->VortPtB.z,m_BladePanel.at(v)->m_Gamma,-1,true,coreSizeSquared);
        vortexBladeLines.append(lalb);

        if(m_currentTimeStep == 0){
        DummyLine tbta(m_BladePanel.at(v)->pTB->x,m_BladePanel.at(v)->pTB->y,m_BladePanel.at(v)->pTB->z,m_BladePanel.at(v)->pTA->x,m_BladePanel.at(v)->pTA->y,m_BladePanel.at(v)->pTA->z,m_BladePanel.at(v)->m_Gamma,-1,true,coreSizeSquared);
        vortexBladeLines.append(tbta);}

        if (m_BladePanel.at(v)->isHub){
            DummyLine tala(m_BladePanel.at(v)->pTA->x,m_BladePanel.at(v)->pTA->y,m_BladePanel.at(v)->pTA->z,m_BladePanel.at(v)->VortPtA.x,m_BladePanel.at(v)->VortPtA.y,m_BladePanel.at(v)->VortPtA.z,m_BladePanel.at(v)->m_Gamma,-1,false,coreSizeSquared,0,0,true,false);
            vortexBladeLines.append(tala);
        }
        else{
            DummyLine tala(m_BladePanel.at(v)->pTA->x,m_BladePanel.at(v)->pTA->y,m_BladePanel.at(v)->pTA->z,m_BladePanel.at(v)->VortPtA.x,m_BladePanel.at(v)->VortPtA.y,m_BladePanel.at(v)->VortPtA.z,m_BladePanel.at(v)->m_Gamma-m_BladePanel.at(v-1)->m_Gamma,-1,false,coreSizeSquared);
            vortexBladeLines.append(tala);
        }
        if (m_BladePanel.at(v)->isTip){
            DummyLine lbtb(m_BladePanel.at(v)->VortPtB.x,m_BladePanel.at(v)->VortPtB.y,m_BladePanel.at(v)->VortPtB.z,m_BladePanel.at(v)->pTB->x,m_BladePanel.at(v)->pTB->y,m_BladePanel.at(v)->pTB->z,m_BladePanel.at(v)->m_Gamma,-1,false,coreSizeSquared,0,0,false,true);
            vortexBladeLines.append(lbtb);
        }
    }

    for (int v=0; v<m_StrutPanel.size();v++){

        DummyLine lalb(m_StrutPanel.at(v)->VortPtA.x,m_StrutPanel.at(v)->VortPtA.y,m_StrutPanel.at(v)->VortPtA.z,m_StrutPanel.at(v)->VortPtB.x,m_StrutPanel.at(v)->VortPtB.y,m_StrutPanel.at(v)->VortPtB.z,m_StrutPanel.at(v)->m_Gamma,-1,true,coreSizeSquared);
        vortexBladeLines.append(lalb);

        if(m_currentTimeStep == 0){
        DummyLine tbta(m_StrutPanel.at(v)->pTB->x,m_StrutPanel.at(v)->pTB->y,m_StrutPanel.at(v)->pTB->z,m_StrutPanel.at(v)->pTA->x,m_StrutPanel.at(v)->pTA->y,m_StrutPanel.at(v)->pTA->z,m_StrutPanel.at(v)->m_Gamma,-1,true,coreSizeSquared);
        vortexBladeLines.append(tbta);}

        if (m_StrutPanel.at(v)->isHub){
            DummyLine tala(m_StrutPanel.at(v)->pTA->x,m_StrutPanel.at(v)->pTA->y,m_StrutPanel.at(v)->pTA->z,m_StrutPanel.at(v)->VortPtA.x,m_StrutPanel.at(v)->VortPtA.y,m_StrutPanel.at(v)->VortPtA.z,m_StrutPanel.at(v)->m_Gamma,-1,false,coreSizeSquared,0,0,true,false);
            vortexBladeLines.append(tala);
        }
        else{
            DummyLine tala(m_StrutPanel.at(v)->pTA->x,m_StrutPanel.at(v)->pTA->y,m_StrutPanel.at(v)->pTA->z,m_StrutPanel.at(v)->VortPtA.x,m_StrutPanel.at(v)->VortPtA.y,m_StrutPanel.at(v)->VortPtA.z,m_StrutPanel.at(v)->m_Gamma-m_StrutPanel.at(v-1)->m_Gamma,-1,false,coreSizeSquared);
            vortexBladeLines.append(tala);
        }
        if (m_StrutPanel.at(v)->isTip){
            DummyLine lbtb(m_StrutPanel.at(v)->VortPtB.x,m_StrutPanel.at(v)->VortPtB.y,m_StrutPanel.at(v)->VortPtB.z,m_StrutPanel.at(v)->pTB->x,m_StrutPanel.at(v)->pTB->y,m_StrutPanel.at(v)->pTB->z,m_StrutPanel.at(v)->m_Gamma,-1,false,coreSizeSquared,0,0,false,true);
            vortexBladeLines.append(lbtb);
        }
    }
    m_savedBladeVortexLines.append(vortexBladeLines);

    QList<CVector> vizCoordinateSystem;
    CVector axis, hub, foot, top, X;
    X.Set(1,0,0);
    axis = m_hubCoords[0];
    hub = m_hubCoords[3];
    foot = m_towerBase[3];
    top = m_towerTop[3];
    X.RotateY(CVector(0,0,0),m_PlatformPitchAngleY);
    X.RotateZ(CVector(0,0,0),m_PlatformYawAngleZ);
    vizCoordinateSystem.append(axis);
    vizCoordinateSystem.append(hub);
    vizCoordinateSystem.append(foot);
    vizCoordinateSystem.append(top);
    vizCoordinateSystem.append(X);
    for(int i=0; i<m_numBlades;i++){
       vizCoordinateSystem.append(m_bladeCoords.at(i).at(2));
       vizCoordinateSystem.append(m_bladeCoords.at(i).at(3));
    }
    m_savedCoordinateSystems.append(vizCoordinateSystem);

}

void QLLTSimulation::storeOutput () {
    if (m_currentTime >= m_TStart){
        if (m_bisVawt) CalcVAWTResults();
        else if (!m_bisVawt) CalcHAWTResults();
        m_ElapsedSimulationTime.append(double(m_t_overhead/1000.0)+double(m_t_induction/1000.0));
    }
    storeGeometry();
}

void QLLTSimulation::unlockStores(){
    g_windFieldStore.unlockStore();
    g_rotorStore.unlockStore();
    g_360PolarStore.unlockStore();
    g_verticalRotorStore.unlockStore();
    g_foilStore.unlockStore();
    g_polarStore.unlockStore();
    g_QLLTVAWTSimulationStore.unlockStore();
    g_QLLTHAWTSimulationStore.unlockStore();
}

void QLLTSimulation::UpdateBladePanels(){

    CVector WLA,WLB,WTA,WTB,LATB,TALB, PointOnAxis, RadialVector, TangentVector;
    double Radius;

    for (int i=0;i<m_BladePanel.size();i++)
    {
        WLA.Copy(*m_BladePanel[i]->pLA);
        WLB.Copy(*m_BladePanel[i]->pLB);
        WTA.Copy(*m_BladePanel[i]->pTA);
        WTB.Copy(*m_BladePanel[i]->pTB);
        LATB.x = WTB.x - WLA.x;
        LATB.y = WTB.y - WLA.y;
        LATB.z = WTB.z - WLA.z;
        TALB.x = WLB.x - WTA.x;
        TALB.y = WLB.y - WTA.y;
        TALB.z = WLB.z - WTA.z;

        m_BladePanel[i]->Normal = LATB * TALB;
        m_BladePanel[i]->Area =  m_BladePanel[i]->Normal.VAbs()/2.0;
        m_BladePanel[i]->Normal.Normalize();

        m_BladePanel[i]->SetFrame(WLA, WLB, WTA, WTB);

        PointOnAxis = CorrespondingAxisPoint(m_BladePanel[i]->CtrlPt,m_hubCoords[3],m_hubCoords[3]+m_hubCoords[0]);
        RadialVector = CVector(m_BladePanel[i]->CtrlPt-PointOnAxis);
        Radius = RadialVector.VAbs();
        TangentVector = m_hubCoords[0] * RadialVector;
        TangentVector.Normalize();
        RadialVector.Normalize();

        m_BladePanel[i]->tangentialVector = TangentVector;
        m_BladePanel[i]->radialVector = RadialVector*(-1.0);
        m_BladePanel[i]->radius = Radius;

        m_BladePanel[i]->angularPos = m_currentAzimuthalPosition + 360/m_numBlades*m_BladePanel[i]->fromBlade;
        m_BladePanel[i]->angularPos = double((int(1000*m_BladePanel[i]->angularPos)%360000) / 1000.0);
    }

}

void QLLTSimulation::UpdateStrutPanels(){

    CVector WLA,WLB,WTA,WTB,LATB,TALB, PointOnAxis, RadialVector, TangentVector;
    double Radius;

    for (int i=0;i<m_StrutPanel.size();i++)
    {
        WLA.Copy(*m_StrutPanel[i]->pLA);
        WLB.Copy(*m_StrutPanel[i]->pLB);
        WTA.Copy(*m_StrutPanel[i]->pTA);
        WTB.Copy(*m_StrutPanel[i]->pTB);
        LATB.x = WTB.x - WLA.x;
        LATB.y = WTB.y - WLA.y;
        LATB.z = WTB.z - WLA.z;
        TALB.x = WLB.x - WTA.x;
        TALB.y = WLB.y - WTA.y;
        TALB.z = WLB.z - WTA.z;

        m_StrutPanel[i]->Normal = LATB * TALB;
        m_StrutPanel[i]->Area =  m_StrutPanel[i]->Normal.VAbs()/2.0;
        m_StrutPanel[i]->Normal.Normalize();

        m_StrutPanel[i]->SetFrame(WLA, WLB, WTA, WTB);

        PointOnAxis = CorrespondingAxisPoint(m_StrutPanel[i]->CtrlPt,m_hubCoords[3],m_hubCoords[3]+m_hubCoords[0]);
        RadialVector = CVector(m_StrutPanel[i]->CtrlPt-PointOnAxis);
        Radius = RadialVector.VAbs();
        TangentVector = m_hubCoords[0] * RadialVector;
        TangentVector.Normalize();
        RadialVector.Normalize();

        m_StrutPanel[i]->tangentialVector = TangentVector;
        m_StrutPanel[i]->radialVector = RadialVector*(-1.0);
        m_StrutPanel[i]->radius = Radius;

        m_StrutPanel[i]->angularPos = m_currentAzimuthalPosition + 360/m_numBlades*m_StrutPanel[i]->fromBlade;
        m_StrutPanel[i]->angularPos = double((int(1000*m_StrutPanel[i]->angularPos)%360000) / 1000.0);
    }

}

void QLLTSimulation::UpdateHAWTCoordinates(){

    CVector O(0,0,0);

    m_towerBase[0] = CVector(1,0,0);
    m_towerBase[1] = CVector(0,1,0);
    m_towerBase[2] = CVector(0,0,1);
    m_towerBase[3] = CVector(0,0,0);

    // rotate translate tower
    CVector rollaxis = m_towerBase[0];
    CVector pitchaxis = m_towerBase[1];
    CVector yawaxis = m_towerBase[2];

    for (int i=0;i<3;i++){
    m_towerBase[i].Rotate(O,rollaxis,m_PlatformRollAngleX);
    m_towerBase[i].Rotate(O,pitchaxis,m_PlatformPitchAngleY);
    m_towerBase[i].Rotate(O,yawaxis,m_PlatformYawAngleZ);
    }
    m_towerBase[3].Translate(m_PlatformTranslation);
    // end rotate translate tower


    m_towerTop[0] = m_towerBase[0];
    m_towerTop[1] = m_towerBase[1];
    m_towerTop[2] = m_towerBase[2];
    m_towerTop[3] = m_towerBase[3]+m_towerBase[2]*m_HubHeight;

    // rotate translate tower top
    // end rotate translate tower top

    m_nacelleYaw[0] = m_towerTop[0];
    m_nacelleYaw[1] = m_towerTop[1];
    m_nacelleYaw[2] = m_towerTop[2];
    m_nacelleYaw[3] = m_towerTop[3];

    // yaw turbine
    yawaxis = m_nacelleYaw[2];
    for (int i=0;i<3;i++){
    m_nacelleYaw[i].Rotate(O,yawaxis,m_YawAngle);
    }
    // end yaw turbine


    m_shaftCoords[0] = m_nacelleYaw[0];
    m_shaftCoords[1] = m_nacelleYaw[1];
    m_shaftCoords[2] = m_nacelleYaw[2];
    m_shaftCoords[3] = m_nacelleYaw[3]/*+m_nacelleYaw[2]*m_Twr2Shft*/;

    // shaft tilt
    CVector tiltaxis = m_shaftCoords[1];
    for (int i=0;i<3;i++){
    m_shaftCoords[i].Rotate(O,tiltaxis,m_ShaftTiltAngle);
    }
    // end shaft tilt


    m_azimuthCoords[0] = m_shaftCoords[0];
    m_azimuthCoords[1] = m_shaftCoords[1];
    m_azimuthCoords[2] = m_shaftCoords[2];
    m_azimuthCoords[3] = m_shaftCoords[3];

    // rotate rotor
    for (int i=1;i<3;i++){
    m_azimuthCoords[i].Rotate(O,m_azimuthCoords[0],m_currentAzimuthalPosition);
    }
    // end rotate rotor

    m_hubCoords[0] = m_azimuthCoords[0];
    m_hubCoords[1] = m_azimuthCoords[1];
    m_hubCoords[2] = m_azimuthCoords[2];

    if (m_bUpWind){
    m_hubCoords[3] = m_azimuthCoords[3]-m_shaftCoords[0]*m_RotorOverhang;
    }
    else{
    m_hubCoords[3] = m_azimuthCoords[3]+m_shaftCoords[0]*m_RotorOverhang;
    }

    m_conedCoords.clear();
    m_bladeCoords.clear();
    for (int j=0; j<m_numBlades;j++){

        QList<CVector> BCoords, CCoords;
        for (int l=0; l<4;l++){
        BCoords.append(CVector());
        CCoords.append(CVector());
        }

        CCoords[0] = m_hubCoords[0];
        CCoords[1] = m_hubCoords[1];
        CCoords[2] = m_hubCoords[2];
        CCoords[3] = m_hubCoords[3];

        // cone angle
        CVector coneaxis = CCoords[1];
        for (int i=0;i<3;i++){
        CCoords[i].Rotate(O,coneaxis,-m_ConeAngle);
        }
        // end cone angle

        BCoords[0] = CCoords[0];
        BCoords[1] = CCoords[1];
        BCoords[2] = CCoords[2];
        BCoords[3] = m_hubCoords[3]+m_hubCoords[2]*m_Blade->m_TPos[0]+m_hubCoords[1]*m_Blade->m_TOffsetX[0];

        for (int i=0; i<3;i++){
            BCoords[i].Rotate(O,m_azimuthCoords[0],  360.0/m_numBlades*j);
            CCoords[i].Rotate(O,m_azimuthCoords[0],  360.0/m_numBlades*j);
        }

        // pitch the blade
        for (int i=0; i<2;i++) BCoords[i].Rotate(O,BCoords[2],  m_PitchAngle[j]);

        BCoords[3].Rotate(m_hubCoords[3], m_azimuthCoords[0], 360.0/m_numBlades*j);
        m_conedCoords.append(CCoords);
        m_bladeCoords.append(BCoords);

        // construct the blade with the blade axes and the reference nodes
        for (int i=0;i<m_Blade->m_BladeNodes.size();i++){
            m_BladeNode[j*(m_Blade->m_BladeNodes.size())+i]->Set(m_hubCoords[3]+m_bladeCoords[j][0]*m_Blade->m_BladeNodes[i].z+m_bladeCoords[j][1]*m_Blade->m_BladeNodes[i].x+m_bladeCoords[j][2]*m_Blade->m_BladeNodes[i].y);
        }
    }

}

void QLLTSimulation::UpdateStrutCoordinates(){
    // construct the blade with the blade axes and the reference nodes

    int strutNodesPerBlade = 0;

    for (int i=0;i<m_Blade->m_StrutList.size();i++){

        strutNodesPerBlade += m_Blade->m_StrutList.at(i)->m_StrutNodes.size();
    }


    for (int j=0; j<m_numBlades;j++){
        int pos = 0;
        for (int k=0; k<m_Blade->m_StrutList.size();k++){
            for (int i=0;i<m_Blade->m_StrutList.at(k)->m_StrutNodes.size();i++){
                m_StrutNode.at(j*(strutNodesPerBlade)+pos)->Set(m_bladeCoords[j][3]+m_bladeCoords[j][0]*(m_Blade->m_StrutList.at(k)->m_StrutNodes.at(i).z-m_Blade->m_MaxRadius)-m_bladeCoords[j][1]*(m_Blade->m_StrutList.at(k)->m_StrutNodes.at(i).x)+m_bladeCoords[j][2]*m_Blade->m_StrutList.at(k)->m_StrutNodes.at(i).y);
                pos++;
            }
        }

    }
}

void QLLTSimulation::UpdateVAWTCoordinates(){

    CVector O(0,0,0);

    m_towerBase[0] = CVector(1,0,0);
    m_towerBase[1] = CVector(0,1,0);
    m_towerBase[2] = CVector(0,0,1);
    m_towerBase[3] = CVector(0,0,0);

    // rotate translate tower
    CVector rollaxis = m_towerBase[0];
    CVector pitchaxis = m_towerBase[1];
    CVector yawaxis = m_towerBase[2];

    for (int i=0;i<3;i++){
    m_towerBase[i].Rotate(O,rollaxis,m_PlatformRollAngleX);
    m_towerBase[i].Rotate(O,pitchaxis,m_PlatformPitchAngleY);
    m_towerBase[i].Rotate(O,yawaxis,m_PlatformYawAngleZ);
    }
    m_towerBase[3].Translate(m_PlatformTranslation);

    m_towerTop[0] = m_towerBase[0];
    m_towerTop[1] = m_towerBase[1];
    m_towerTop[2] = m_towerBase[2];
    m_towerTop[3] = m_towerBase[3]+m_towerBase[2]*m_TowerHeight;

    m_azimuthCoords[0] = m_towerTop[2];
    m_azimuthCoords[1] = m_towerTop[1]*(-1);
    m_azimuthCoords[2] = m_towerTop[0];
    m_azimuthCoords[3] = m_towerTop[3];

    // rotate rotor
    for (int i=0;i<3;i++){
    m_azimuthCoords[i].Rotate(O,m_azimuthCoords[0],m_currentAzimuthalPosition + 90);
    }
    // end rotate rotor

    m_hubCoords[0] = m_azimuthCoords[0];
    m_hubCoords[1] = m_azimuthCoords[1];
    m_hubCoords[2] = m_azimuthCoords[2];
    m_hubCoords[3] = m_towerBase[3]+m_towerBase[2]*m_HubHeight;

    m_bladeCoords.clear();
    for (int j=0; j<m_numBlades;j++){

        QList<CVector> BCoords;
        for (int l=0; l<4;l++){
        BCoords.append(CVector());
        }

        BCoords[0] = m_hubCoords[2];
        BCoords[1] = m_hubCoords[1];
        BCoords[2] = m_hubCoords[0];
        BCoords[3] = m_hubCoords[3]+m_hubCoords[2]*m_Blade->m_MaxRadius;

        for (int i=0; i<3;i++) BCoords[i].Rotate(O,m_azimuthCoords[0],  360.0/m_numBlades*j);

        // pitch the blades
        for (int i=0; i<3;i++) BCoords[i].Rotate(O, BCoords[2], m_PitchAngle[j]);


        BCoords[3].Rotate(m_hubCoords[3], m_azimuthCoords[0], 360.0/m_numBlades*j);
        m_bladeCoords.append(BCoords);

        // construct the blade with the blade axes and the reference nodes
        for (int i=0;i<m_Blade->m_BladeNodes.size();i++){
            m_BladeNode[j*(m_Blade->m_BladeNodes.size())+i]->Set(m_bladeCoords[j][3]+m_bladeCoords[j][0]*(m_Blade->m_BladeNodes[i].z-m_Blade->m_MaxRadius)-m_bladeCoords[j][1]*(m_Blade->m_BladeNodes[i].x)+m_bladeCoords[j][2]*m_Blade->m_BladeNodes[i].y);
        }
    }

}

void QLLTSimulation::UpdateRotorGeometry()
{
    if(!m_bisVawt) UpdateHAWTCoordinates();
    if(m_bisVawt)  UpdateVAWTCoordinates();

    UpdateStrutCoordinates();
    UpdateBladePanels();
    UpdateStrutPanels();

    if (m_currentTimeStep == 0) InitializePanelPositions();
    m_bGlChanged = true;
}

void QLLTSimulation::UpdateSimulationParameters(double time){

    QList<double> curParams;

    if (!m_SimParameters.size()) return;

    for (int i=0; i<m_SimParameters.size()-1;i++){
        if (m_SimParameters.at(i).at(0)<= time && m_SimParameters.at(i+1).at(0) > time){
            for (int j=0;j<m_SimParameters.at(i).size();j++){
            curParams.append(m_SimParameters.at(i).at(j)+(time-m_SimParameters.at(i).at(0))/(m_SimParameters.at(i+1).at(0)-m_SimParameters.at(i).at(0))*(m_SimParameters.at(i+1).at(j)-m_SimParameters.at(i).at(j)));
            }
        }
    }
    if (m_SimParameters.at(m_SimParameters.size()-1).at(0) <= time ){
        for (int j=0;j<m_SimParameters.at(m_SimParameters.size()-1).size();j++){
        curParams.append(m_SimParameters.at(m_SimParameters.size()-1).at(j));
        }
    }
    if (m_SimParameters.at(0).at(0) > time ){
        for (int j=0;j<m_SimParameters.at(0).size();j++){
        curParams.append(m_SimParameters.at(0).at(j));
        }
    }

    if (m_windInputType == SIMFILE){
        m_Omega = curParams.at(1)/60.0*2.0*PI;
        V_in.x = curParams.at(2);
        if (curParams.size()>3) V_in.z = curParams.at(3);
        else V_in.z = 0;
        if (curParams.size()>4) m_HorInflowAngle = curParams.at(4);
        else m_HorInflowAngle = 0;
        if (curParams.size()>5) m_YawAngle = curParams.at(5);
        else m_YawAngle = 0;
        if (curParams.size()>6) m_PlatformRollAngleX = curParams.at(6);
        else m_PlatformRollAngleX = 0;
        if (curParams.size()>7) m_PlatformPitchAngleY = curParams.at(7);
        else m_PlatformPitchAngleY = 0;
        if (curParams.size()>8) m_PlatformYawAngleZ = curParams.at(8);
        else m_PlatformYawAngleZ = 0;
        if (curParams.size()>9) m_PlatformTranslation.x = curParams.at(9);
        else m_PlatformTranslation.x = 0;
        if (curParams.size()>10) m_PlatformTranslation.y = curParams.at(10);
        else m_PlatformTranslation.y = 0;
        if (curParams.size()>11) m_PlatformTranslation.z = curParams.at(11);
        else m_PlatformTranslation.z = 0;
        for (int i=0;i<m_numBlades;i++){
        if (curParams.size()>12+i) m_PitchAngle[i] = curParams.at(12+i);
        else m_PitchAngle[i] = 0;
        }
        m_VertShear = 0;
    }
    if (m_windInputType == AERODYN){
        V_in.x = curParams.at(1);
        if (curParams.size()>2) m_HorInflowAngle = -curParams.at(2);
        else m_HorInflowAngle = 0;
        if (curParams.size()>3) V_in.z = curParams.at(3);
        else V_in.z = 0;
        if (curParams.size()>4) m_HorShear = curParams.at(4);
        else m_HorShear = 0;
        if (curParams.size()>5) m_VertShear = curParams.at(5);
        else m_VertShear = 0;
        if (curParams.size()>6) m_LinVertShear = curParams.at(6);
        else m_LinVertShear = 0;
        if (curParams.size()>7) m_GustSpeed = curParams.at(7);
        else m_GustSpeed = 0;
    }
}

void QLLTSimulation::LumpWake(){

    if (m_bisVawt) return; // only for HAWT's


    // here the wake is lumped into concentrated vortices after the specified number of "m_FullWakeRevs"
    QVector<VortexLine *> lines;
    QVector<VortexLine *> newLines;
    QList<QList <VortexNode *> > nodes;
    QList<VortexNode *> newVNodes;
    double Gamma_Max = 0;
    double age = 0;
    int fromTimeStep = 0, fromTimeStepL=0, fromTimeStepT=0;
    int num_bladelines = m_BladeLine.size() / m_numBlades;
    double fromRevolution = 0, fromTime = 0;

    // here the vortexlines that have to be converted into lumped vortices are searched and appended into the lines array; the shed vorticity is removed

    for (int blade = 0;blade < m_numBlades; blade++){
        int linestart = blade * num_bladelines;
        int lineend = (blade+1) * num_bladelines;
            lines.clear();
            for (int i=m_WakeLine.size()-1;i>=0;i--){
                if (m_WakeLine.at(i)->fromTimestep != m_currentTimeStep -m_Nth_WakeStep){
                    if ((m_WakeCounter == REVOLUTIONS && (m_currentAzimuthalPosition - m_WakeLine.at(i)->fromRevolution) / 360.0 > m_FullWakeRevs) || (m_WakeCounter == TIMESTEPS && (m_currentTimeStep - m_WakeLine.at(i)->fromTimestep) > m_FullWakeRevs) || (m_WakeCounter == TIME && (m_currentTime - m_WakeLine.at(i)->fromTime) > m_FullWakeRevs)){
                        if (m_WakeLine.at(i)->isShed) {
                            m_WakeLine.at(i)->Delete();
                        }
                        else if (!m_WakeLine.at(i)->isConcentrated && m_WakeLine.at(i)->fromStation >= linestart &&  m_WakeLine.at(i)->fromStation < lineend){
                            lines.append(m_WakeLine.at(i));
                        }
                    }
                }
            }

            if (lines.size()){

            fromTimeStep = lines.at(0)->fromTimestep;
            fromRevolution = lines.at(0)->fromRevolution;
            fromTime = lines.at(0)->fromTime;

            fromTimeStepL = lines.at(0)->pL->fromTimestep;
            fromTimeStepT = lines.at(0)->pT->fromTimestep;


            double Gamma = 0, Weight = 0, CoreSize = 0, Strain = 0;
            CVector Vec = CVector(0,0,0);
            CVector VecT = CVector(0,0,0);

            QVector<double>  VGamma, VWeight, VCoreSize, VStrain;
            QVector<CVector> VVec, VVecT;

            Gamma_Max = 0;
            for (int n=0;n< lines.size();n++){
                if (fabs(lines.at(n)->GetGamma()) > Gamma_Max) Gamma_Max = fabs(lines.at(n)->GetGamma());
            }
            // here the zero crossings of the gamma derivative are found, for every zero crossing a vortex is created later that rotates in the appropriate direction
            for (int l = 0; l<lines.size()-1;l++){
                    age = lines.at(0)->age;

            //gamma stores the total vortex strength of the vortex that will be created
            //vec and vect are used to compute a weighted position for the shedding of the vortex

                    Gamma += lines.at(l)->GetGamma();
                    Strain += lines.at(l)->stretchFactor * fabs(lines.at(l)->GetGamma());
                    Weight += fabs(lines.at(l)->GetGamma());
                    CoreSize += fabs(lines.at(l)->GetGamma()) * lines.at(l)->coreSizeSquared; //core size is increased when vortices are merging! assumed that tow vortices of equal strength double the sqared vortex size
                    // from the paper: three dimensional instability during vortex merging
                    Vec += *lines.at(l)->pL*fabs(lines.at(l)->GetGamma());
                    VecT += *lines.at(l)->pT*fabs(lines.at(l)->GetGamma());

                    if (lines.at(l)->GetGamma() * lines.at(l+1)->GetGamma() < 0){

                        VGamma.append(Gamma);
                        VVec.append(Vec);
                        VVecT.append(VecT);
                        VWeight.append(Weight);
                        VCoreSize.append(CoreSize);
                        VStrain.append(Strain);

                        Gamma = 0;
                        Weight = 0;
                        Strain = 0;
                        CoreSize = 0;
                        Vec = CVector(0,0,0);
                        VecT = CVector(0,0,0);
                    }
                    if (l==lines.size()-2){
                            Gamma += lines.at(l+1)->GetGamma();
                            Vec += *lines.at(l+1)->pL*fabs(lines.at(l+1)->GetGamma());
                            VecT += *lines.at(l+1)->pT*fabs(lines.at(l+1)->GetGamma());
                            Weight += fabs(lines.at(l+1)->GetGamma());
                            CoreSize += fabs(lines.at(l+1)->GetGamma()) * lines.at(l+1)->coreSizeSquared;
                            Strain += lines.at(l+1)->stretchFactor * fabs(lines.at(l+1)->GetGamma());

                            VGamma.append(Gamma);
                            VVec.append(Vec);
                            VVecT.append(VecT);
                            VWeight.append(Weight);
                            VCoreSize.append(CoreSize);
                            VStrain.append(Strain);
                    }
            }

            //the array of lumped vortexes is searched for too small vortices which are then added to the nearest vortex of same orientation

            for (int m=0; m<VGamma.size();m++){
                if (VGamma.at(m)/Gamma_Max < m_MinGammaFrac){
                    int nearest = 10e6;
                    bool exists = false;
                    for (int l=0;l<VGamma.size();l++){
                        if (l!=m && fabs(VVec[l].VAbs()-VVec[m].VAbs()) < nearest && VGamma.at(m)*VGamma.at(l)>0){
                            nearest = l;
                            exists = true;
                        }
                    }
                    if (exists){
                        VGamma[nearest] += VGamma.at(m);
                        VVec[nearest] += VVec.at(m);
                        VVecT[nearest] += VVecT.at(m);
                        VWeight[nearest] += VWeight.at(m);
                        VCoreSize[nearest] += VCoreSize.at(m);
                        VStrain[nearest] += VStrain.at(m);


                        VGamma.removeAt(m);
                        VVec.removeAt(m);
                        VVecT.removeAt(m);
                        VWeight.removeAt(m);
                        VCoreSize.removeAt(m);
                        VStrain.removeAt(m);
                        m = 0;
                    }
                }
            }

            int fromStation = 0;

                fromStation = lines.at(0)->fromStation;

                for (int m=lines.size()-1;m>=0;m--){
                    lines.at(m)->Delete();
                }


            newVNodes.clear();
            for (int z = 0; z<VGamma.size();z++){

                        VortexLine *line = new VortexLine;
                        line->isConcentrated = true;
                        line->SetGamma(VGamma.at(z));
                        line->isTrailing = true;
                        line->m_Lines = &m_WakeLine;
                        line->m_Nodes = &m_WakeNode;
                        line->m_KinViscosity = m_KinViscosity;
                        line->m_TurbulentViscosity = m_TurbulentViscosity;
                        line->m_VortexTimeOffset = m_VortexTimeOffset;
                        line->age = age;
                        line->includeStrain = m_bIncludeStrain;
                        line->fromStation = fromStation;
                        line->fromTimestep = fromTimeStep;
                        line->fromTime = fromTime;
                        line->fromRevolution = fromRevolution;


                        bool found = false;
                        CVector Vec = VVec.at(z);
                        CVector VecT = VVecT.at(z);
                        Vec = Vec / fabs(VWeight.at(z));
                        VecT = VecT / fabs(VWeight.at(z));
                        line->stretchFactor = VStrain[z] / fabs(VWeight.at(z));
                        line->coreSizeSquared = VCoreSize[z] / fabs(VWeight.at(z));



                        if (TempConnectionNode.size()){
                                if (TempConnectionNode.at(blade).size() == VGamma.size()){
                                    if (CVector(*TempConnectionNode.at(blade).at(z)-VecT).VAbs() < m_Blade->getRotorRadius()/8){
                                        found = true;
                                        TempConnectionNode.at(blade).at(z)->attachLine(line);
                                        line->pT = TempConnectionNode.at(blade).at(z);
                                    }
                                }
                        }

                        if (!found){
                            VortexNode *node = new VortexNode;
                            *node = VecT;
                            node->fromTimestep = fromTimeStepT;
                            node->attachLine(line);
                            line->pT = node;
                            m_WakeNode.insert(m_WakeNode.begin(),node);
                        }


                        VortexNode *node = new VortexNode;
                        *node = Vec;
                        node->fromTimestep = fromTimeStepL;
                        node->attachLine(line);
                        line->pL = node;
                        line->InitLength();

                        m_WakeNode.insert(m_WakeNode.begin(),node);
                        newVNodes.push_back(node);
                        newLines.append(line);
            }

            nodes.push_back(newVNodes);

        }
    }

    TempConnectionNode.clear();
    for (int j=0;j<m_numBlades;j++)
        if (nodes.size()){
                TempConnectionNode.push_back(nodes.at(j));
        }

    for (int i=0; i< newLines.size();i++){
        m_WakeLine.insert(m_WakeLine.begin(),newLines.at(i));
    }

//now thin out wake lines
        VortexLine *vLine1;
        for (int i=m_WakeLine.size()-1; i>=0;i--){
                if ((m_WakeCounter == REVOLUTIONS && (m_currentAzimuthalPosition - m_WakeLine.at(i)->fromRevolution) / 360 > m_FineWakeRevs + m_FullWakeRevs) || (m_WakeCounter == TIMESTEPS && (m_currentTimeStep - m_WakeLine.at(i)->fromTimestep)> m_FineWakeRevs + m_FullWakeRevs) || (m_WakeCounter == TIME && (m_currentTime - m_WakeLine.at(i)->fromTime) > m_FineWakeRevs + m_FullWakeRevs)){
                    if((m_WakeLine.at(i)->fromTimestep/m_Nth_WakeStep % m_Thinfactor != 0)){

                  if (m_WakeLine.at(i)->pL->attachedLines.size()==2 && m_WakeLine.at(i)->pT->attachedLines.size()==2){
                      if (m_WakeLine.at(i)->pT->attachedLines[0] == m_WakeLine.at(i))  vLine1 = (VortexLine *) m_WakeLine.at(i)->pT->attachedLines[1];
                      else vLine1 = (VortexLine *) m_WakeLine.at(i)->pT->attachedLines[0];

                      m_WakeLine.at(i)->pT->detachLine(vLine1);
                      m_WakeLine.at(i)->pL->attachLine(vLine1);
                      vLine1->pL = m_WakeLine.at(i)->pL;
                      vLine1->InitLength();
                      vLine1->oldLength = vLine1->Length; // this prevents "artificial strain" from thinning out!
                      m_WakeLine.at(i)->Delete();
                  }
                }
        }

        }



}

void QLLTSimulation::TruncateWake(){

    for (int i=m_WakeLine.size()-1;i>=0;i--){
        if (m_WakeLine.at(i)->fromTimestep != m_currentTimeStep-m_Nth_WakeStep){
            if ((m_WakeCounter == REVOLUTIONS && (m_currentAzimuthalPosition - m_WakeLine.at(i)->fromRevolution) / 360  > m_MaxWakeAge) || (m_WakeCounter == TIMESTEPS && (m_currentTimeStep - m_WakeLine.at(i)->fromTimestep  > m_MaxWakeAge)) || (m_WakeCounter == TIME && (m_currentTime - m_WakeLine.at(i)->fromTime  > m_MaxWakeAge))) m_WakeLine.at(i)->Delete();
        }
    }

    // cut off implementation for VAWT, from the newly shed wake lines a number of lines based on the minGammeFrac value with the lowest circulation from a custom number of fixed wake revs are deleted
    if (m_bisVawt){
        QList <VortexLine *> tempLines;
        tempLines.clear();

                for (int i=0; i<m_WakeLine.size();i++){
                    if (m_WakeLine.at(i)->fromTimestep == m_currentTimeStep - 2*m_Nth_WakeStep){
                        tempLines.append(m_WakeLine.at(i));
                    }
                    if (m_WakeLine.at(i)->fromTimestep != m_currentTimeStep - m_Nth_WakeStep && m_WakeLine.at(i)->fromTimestep != m_currentTimeStep - 2*m_Nth_WakeStep){
                        if ((m_WakeCounter == REVOLUTIONS && (m_currentAzimuthalPosition - m_WakeLine.at(i)->fromRevolution) / 360  <= m_FullWakeRevs) || (m_WakeCounter == TIMESTEPS && (m_currentTimeStep - m_WakeLine.at(i)->fromTimestep) <= m_FullWakeRevs) || (m_WakeCounter == TIME && (m_currentTime - m_WakeLine.at(i)->fromTime) <= m_FullWakeRevs))
                        tempLines.append(m_WakeLine.at(i));
                    }
                }

                int num = tempLines.size()*m_MinGammaFrac;

                for (int k=0; k<num;k++){
                    double gam = 10e10;
                    int posGam = 0;
                    for (int i=0; i<tempLines.size();i++){
                        if (fabs(tempLines.at(i)->GetGamma()) < gam){
                            posGam = i;
                            gam = fabs(tempLines.at(i)->GetGamma());
                    }
                }
                    tempLines.at(posGam)->deleteLater = true;
                    tempLines.removeAt(posGam);
            }

                for (int i=0; i<m_WakeLine.size(); i++){
                    if (m_WakeLine.at(i)->deleteLater){
                        if((m_WakeCounter == REVOLUTIONS && (m_currentAzimuthalPosition - m_WakeLine.at(i)->fromRevolution) / 360  > m_FullWakeRevs) || (m_WakeCounter == TIMESTEPS && (m_currentTimeStep - m_WakeLine.at(i)->fromTimestep) > m_FullWakeRevs) || (m_WakeCounter == TIME && (m_currentTime - m_WakeLine.at(i)->fromTime) > m_FullWakeRevs)){
                            m_WakeLine.at(i)->Delete();
                        }
                    }
                }


 }

}
