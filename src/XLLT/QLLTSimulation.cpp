#include "QLLTSimulation.h"
#include "../GLWidget.h"
#include <QDebug>
#include <QtOpenGL>
#include "../GlobalFunctions.h"
#include "../MainFrame.h"
#include "../Store.h"
#include "../Graph/NewCurve.h"



QLLTSimulation::QLLTSimulation(QString name,
                               bool bisVawt,
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
                               bool turbConvection
        )
    : StorableObject (name)
{

    for (int i=0; i<50;i++) m_pBladeList[i] = NULL;
    m_results.clear();


    m_pen.setColor(findColor (&g_QLLTHAWTSimulationStore));
    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);
    m_shownBladeSection = 1;
    m_shownTimeIndex = 0;

    m_bisVawt = bisVawt;
    m_ActualTimeStep = -1;

    m_module = g_QLLTModule;

    m_Blade = usedRotor;

    num_blades = m_Blade->m_blades;

    Pitch_Old = 0;

    FillVariableList(m_bisVawt);

    m_WindField = usedWindField;

    /// Main Settings for Op-Point
    m_YawAngle = rotorYaw;
    m_TeetherAngle = rotorTeether;
    m_InflowAngle = inflowAngle;
    m_PitchAngle = 0;
    m_Omega = rotorSpeed / 60.0 * 2 * PI;
    dT = timeStep;

    m_NumTimesteps = numTimeStep;
    m_ActualTime = 0;
    m_AngularDisc = aziDisc;
    m_bTurbConvection = turbConvection;

    /// Additional Modells / Tower Shadow / Prescribed Wake / Goundeffects

    m_bIncludeTower = bIncludeTower;
    m_HubHeight = hubHeight;
    m_bGround = bGround;
    m_bPrescribed = bPrescribed;

    /// Algorithm Parameters

    m_firstWakeRowLength = firstWakeRowLength;
    m_FullWakeRevs = fullWakeRevs;
    m_FineWakeRevs = fineWakeRevs;
    m_MaxWakeAge = maxWakeAge;
    m_VortexTimeOffset = vortexTimeOffset;
    m_Epsilon = epsilon;
    m_RelaxFactor = relaxFactor;
    m_MaxIterations = maxIter;
    m_InterpolationFactor = 1; // Wake Interpolation
    m_Thinfactor = thinFactor;
    m_MinGammaFrac = 0.1;
    m_CutOffRadius = cutOffRadius;
    m_bShed  = bShed;
    m_bTrail = true;
    m_bWakeRollUp = bWakeRollup;
    m_bWakeLine = true;

    m_TowerDrag = towerDrag;
    m_TowerRadius = towerRadius;
    m_TowerDistance = towerDist;


    ///Environmental Variables
    m_KinViscosity = kinVisc;
    m_TurbulentViscosity = turbVisc;
    m_Density = airDens;

    V_in.Set(m_WindField->getMeanWindSpeedAtHub(),0,0);
    V_in.RotZ(m_InflowAngle/180*PI);

    TSR = m_Omega *m_Blade->getRotorRadius() / V_in.VAbs();


    if (!m_bisVawt) createHAWTTurbineBlades(m_Blade);
    else createVAWTTurbineBlades(m_Blade);

    /// result / plotting variables
    m_bGLPlotWakeNodes = true;
    m_bGLPlotWakeLines = true;
    m_bGLPlotAllBlades = true;

}



QLLTSimulation::QLLTSimulation(CBlade *blade, WindField *windfield) : StorableObject (QString("test"))
{    

    m_pen.setWidth(1);
    m_pen.setStyle(Qt::SolidLine);

    for (int i=0; i<10;i++) m_pBladeList[i] = NULL;

    if (windfield) m_WindField = windfield;
    else m_WindField = NULL;

    Pitch_Old = 0;

    m_ActualTimeStep = -1;

    num_blades = blade->m_blades;

    qDebug() << m_BladeLine.size();

    qDebug() << blade->getName();

    m_bGlChanged = true;



    //////////////SIMULATION SETTINGS////////////////


    m_WindField = NULL; //Disables Windfield


    m_bisVawt = false;   //HAWT or VAWT Mode

    if (m_bisVawt) m_pen.setColor(findColor (&g_QLLTVAWTSimulationStore));
    if (!m_bisVawt) m_pen.setColor(findColor (&g_QLLTHAWTSimulationStore));
    FillVariableList(m_bisVawt);

    /// Main Settings for Op-Point
    TSR = 8;
    m_YawAngle = -30;
    m_TeetherAngle = 0;
    m_InflowAngle = 00;
    m_PitchAngle = 0;

    m_NumTimesteps = 500;
    m_ActualTime = 0;
    m_AngularDisc = 10;

    /// Additional Modells / Tower Shadow / Prescribed Wake / Goundeffects

    m_bIncludeTower = false;
    m_HubHeight = 65;
    m_bGround = false;
    m_bPrescribed = false;

    /// Algorithm Parameters

    m_firstWakeRowLength = 1.0;
    m_FullWakeRevs = 0.1;
    m_FineWakeRevs = 3;
    m_MaxWakeAge = 8;
    m_VortexTimeOffset = 0.5;
    m_Epsilon = 0.005;
    m_RelaxFactor = 0.6;
    m_MaxIterations = 70;
    m_InterpolationFactor = 1; // Wake Interpolation
    m_Thinfactor = 3;
    m_MinGammaFrac = 0.1;
    m_CutOffRadius = 1.1;
    m_bShed  = false;
    m_bTrail = true;
    m_bWakeRollUp = true;
    m_bWakeLine = true;

    m_TowerDrag = 0.2;
    m_TowerRadius = 0.2;
    m_TowerDistance = 0.5;


    ///Environmental Variables
    m_KinViscosity = 1.511e-5;
    m_TurbulentViscosity = 40;
    m_Density = 1.226;


    /// result / plotting variables
    m_bGLPlotWakeNodes = true;
    m_bGLPlotWakeLines = true;
    m_bGLPlotAllBlades = true;


    if (!m_bisVawt) createHAWTTurbineBlades(blade);
    else createVAWTTurbineBlades(blade);

    if (m_WindField) V_in.Set(m_WindField->getMeanWindSpeedAtHub(),0,0);
    else V_in.Set(15,0,0);

    qDebug() << "V_IN_VABS" << V_in.VAbs();


    if (!m_bisVawt) m_Omega = TSR / m_pBladeList[0]->getRotorRadius() * V_in.VAbs();
    else m_Omega = TSR / m_pBladeList[0]->m_TOffset[0] * V_in.VAbs();
    if (!m_bisVawt) dT = (2*PI*m_pBladeList[0]->getRotorRadius())/(TSR*V_in.VAbs())*m_AngularDisc/360;
    else dT = (2*PI*m_pBladeList[0]->m_TOffset[0])/(TSR*V_in.VAbs())*m_AngularDisc/360;


    dT = 0.01;

    m_Omega = 424.5/60.0 *2 *PI;

    dT = (2*PI)/(m_Omega)*m_AngularDisc/360;

    V_in.Set(15,0,0);
    V_in.RotZ(m_InflowAngle/180*PI);



}

QLLTSimulation::~QLLTSimulation() {
    for (int i=0; i<10;i++){
        if (m_pBladeList[i]){
            delete m_pBladeList[i];
            m_pBladeList[i] = NULL;
        }
    }

    for (int i=0; i<m_BladePanel.size();i++){
        delete m_BladePanel.at(i);
    }
    m_BladePanel.clear();

    for (int i=0; i<m_WakePanel.size();i++){
        delete m_WakePanel.at(i);
    }
    m_WakePanel.clear();

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

}


void QLLTSimulation::render(){


    if (g_mainFrame->getCurrentModule() == m_module && m_module->GLView){
    if (m_bGlChanged) GLDrawVelocityField();
    if (m_bGlChanged) GLCreateRotorMesh();
//    if (m_bGlChanged) GLDrawStreamlines();
    if (m_bGlChanged) GLCreateWakeMesh();

    m_bGlChanged = false;


    glCallList(WINGWAKEPANELS);
    glCallList(GLBODYMESHPANELS);
//    glCallList(VLMSTREAMLINES);
    glCallList(VELOCITYFIELD);
    glCallList(1234);

    }



}


void QLLTSimulation::OnStartAnalysis(void){

    qDebug() << "start analysis";

    connect(this, SIGNAL(geomChanged()),  m_module, SLOT(UpdateView()));


    //Zeitmessung
    QElapsedTimer timer;
    qint64 nanoSec;
    qint64 sec;
    double min;
    timer.start();
    //Zeitmessung


    for (int i=0; i<m_WakePanel.size();i++){
        delete m_WakePanel.at(i);
    }
    m_WakePanel.clear();

    for (int i=0; i<m_WakeLine.size();i++){
        delete m_WakeLine.at(i);
    }
    m_WakeLine.clear();

    for (int i=0; i<m_WakeNode.size();i++){
        delete m_WakeNode.at(i);
    }
    m_WakeNode.clear();



    ///streamline visualization/////
//    int num_points = 50;

//    CVector point[num_points];


//    for (int l=0;l<num_points;l++)
//    {
//        point[l] = CVector(-0.2*m_pBladeList[0]->getRotorRadius(),0,-1*m_pBladeList[0]->getRotorRadius()+2*m_pBladeList[0]->getRotorRadius()/double(num_points)*double(l));

////        point[l] = CVector(-0.5*m_pBladeList[0]->getRotorRadius(),m_pBladeList[0]->getRotorRadius()*cos(2*PI/double(num_points)*double(l)),m_pBladeList[0]->getRotorRadius()*sin(2*PI/double(num_points)*double(l)));
//    }

//    for (int k=0;k<num_points;k++)
//    {
//        m_Streamline[0+k*m_NumTimesteps] = point[k];
//    }

    ////////////////////////////
    /// \brief CP
    ///
    ///
    QVector<double> wake1, wake2, wake3;
    double radius = m_pBladeList[0]->getRotorRadius();
    double x= -4;

    QVector <double> CN;
    QVector <double> PHI;


    QVector <double> HHSpeed;
    QVector <double> CPatV;
    QVector <double> V;
    QVector <double> Power;
    QVector <CVector> angvel;

    CP.clear();
    angPos.clear();
    CT.clear();
    AoA.clear();
    Time.clear();

    double POW = 0;










    ////create initial conditions/////
    /// \brief GuessInitialVortexDistribution

    if (!m_bisVawt) RotateHAWTRotorGeometry();  //rotor dreht
    else RotateVAWTRotorGeometry();

    GuessInitialVortexDistribution();           //Gamma-Verteilung wird wing zugewiesen

    GammaBoundFixedPointIteration();            //wing bekommt erste Zirkulation durch wing Einfluss

    if (!m_bWakeLine){
        AddFirstWakeRow();
    }
    else{
        AddFirstWakeLines();
    }

    KuttaCondition();                           //erste wakereihe bekommt wing zirkulation

//    GammaBoundFixedPointIteration();

    ////end create initial conditions///





    for (int i=0; i<m_NumTimesteps; i++){
        m_ActualTimeStep = i;
        m_ActualTime += dT;

        //                    m_PitchAngle = 45*sin(double(m_ActualTimeStep)/double(m_NumTimesteps)*6.0*PI);
        //                    m_YawAngle = 30*sin(double(m_ActualTimeStep)/double(m_NumTimesteps)*4.0*PI);

//        QElapsedTimer timer2;
//        timer2.start();

        qDebug() << "start calculation for timestep:" << m_ActualTimeStep;



        GammaBoundFixedPointIteration();

        ///// DEBUG OUTPUT ////
        double cp, Ct, Cn, phi, power;
        power = 0;
        CVector V_inPlane, V_outPlane;

        //            for (int k=0; k<m_BladePanel.size();k++)
        //            {
        //                int pos = k % (m_BladePanel.size()/num_blades);

        //                V_inPlane.Set(m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a3),0);
        //                V_outPlane.Set(0,0,m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a2));

        //                qDebug()<< "axial induction: " << 1-m_BladePanel[k]->m_V_total.x/V_in.x << "at station: " << m_pBladeList[0]->getFASTRadiusAt(pos) << "local crossflow angle: " << tan(V_outPlane.VAbs()/V_inPlane.VAbs())*180/PI << V_inPlane.VAbs() << V_outPlane.VAbs();
        //            }
        //            ////////////////////////////



        if(!m_bisVawt){

        for (int k=0;k<m_BladePanel.size();k++){
            int pos = k % (m_BladePanel.size()/num_blades);


            phi = m_BladePanel[k]->m_AoA+(m_pBladeList[0]->m_TTwist[pos]+m_pBladeList[0]->m_TTwist[pos+1])/2;
            Ct = m_BladePanel[k]->m_CL*sin(phi/180*PI)-m_BladePanel[k]->m_CD*cos(phi/180*PI);
            Cn = m_BladePanel[k]->m_CL*cos(phi/180*PI)+m_BladePanel[k]->m_CD*sin(phi/180*PI);
            V_inPlane.Set(m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a3),0);
            power += Ct * pow(V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * m_pBladeList[0]->getFASTRadiusAt(pos) * m_Omega *0.5*m_Density;
        }


        cp = power / (pow(m_pBladeList[0]->getRotorRadius(),2) * PI * 0.5*m_Density * pow(V_in.VAbs(),3));
        }
        else{
            for (int k=0;k<m_BladePanel.size();k++){
                int pos = k % (m_BladePanel.size()/num_blades);


                phi = m_BladePanel[k]->m_AoA+(m_pBladeList[0]->m_TTwist[pos]+m_pBladeList[0]->m_TTwist[pos+1])/2;
                Ct = m_BladePanel[k]->m_CL*sin(m_BladePanel[k]->m_AoA/180*PI)-m_BladePanel[k]->m_CD*cos(m_BladePanel[k]->m_AoA/180*PI);
                V_inPlane.Set(m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a1), m_BladePanel[k]->m_V_total.dot(m_BladePanel[k]->a3),0);
                power += Ct * pow(V_inPlane.VAbs(),2) * m_BladePanel[k]->Area * m_Omega * m_pBladeList[0]->m_TOffset[0];
            }


            cp = power / (2*m_pBladeList[0]->m_TOffset[0] * (m_pBladeList[0]->m_TPos[m_pBladeList[0]->m_NPanel]-m_pBladeList[0]->m_TPos[0])) / pow(V_in.VAbs(),3);

        }


        if (m_ActualTimeStep>=0){
        int pos = 0;

        V_inPlane.Set(m_BladePanel[pos]->m_V_total.dot(m_BladePanel[pos]->a1), m_BladePanel[pos]->m_V_total.dot(m_BladePanel[pos]->a3),0);

        phi = m_BladePanel[pos]->m_AoA+(m_pBladeList[0]->m_TTwist[pos]+m_pBladeList[0]->m_TTwist[pos+1])/2;
        PHI.append( phi);
        CN.append(0.5*m_Density*(m_BladePanel[pos]->m_CL*cos(phi/180*PI)+m_BladePanel[pos]->m_CD*sin(phi/180*PI))*pow(V_inPlane.VAbs(),2)*(m_pBladeList[0]->m_TChord[pos]+m_pBladeList[0]->m_TChord[pos+1])/2);
        CP.append(cp);
        angPos.append(m_BladePanel[pos]->angularPos);
        CT.append(m_BladePanel[pos]->m_CL*cos(m_BladePanel[pos]->m_AoA/180*PI)+m_BladePanel[pos]->m_CD*sin(m_BladePanel[pos]->m_AoA/180*PI));
        AoA.append(m_BladePanel[pos]->m_AoA);
        Time.append(m_ActualTime);
        HHSpeed.append(GetFreeStream(CVector(0,0,0)).VAbs());
        angvel.append((m_BladePanel[pos]->CtrlPt-m_BladePanel[pos]->Old_CtrlPt)/dT);
        }

        for (int o=0;o<m_pBladeList[0]->m_NPanel;o++){
            int pos = o;
            V_inPlane.Set(m_BladePanel[pos]->m_V_total.dot(m_BladePanel[pos]->a1), m_BladePanel[pos]->m_V_total.dot(m_BladePanel[pos]->a3),0);
            double phi = m_BladePanel[pos]->m_AoA+(m_pBladeList[0]->m_TTwist[pos]+m_pBladeList[0]->m_TTwist[pos+1])/2;

            qDebug() << 0.5*m_Density*(m_BladePanel[pos]->m_CL*cos(phi/180*PI)+m_BladePanel[pos]->m_CD*sin(phi/180*PI))*pow(V_inPlane.VAbs(),2)*(m_pBladeList[0]->m_TChord[pos]+m_pBladeList[0]->m_TChord[pos+1])/2 << (m_pBladeList[0]->m_TPos[o]+m_pBladeList[0]->m_TPos[o+1])/2;

        }

//        POW += power / 200;
//        if (m_ActualTimeStep % 100 == 0 && m_ActualTimeStep>=200){
//            CPatV.append(cp);
//            V.append(m_FullWakeRevs);
//            Power.append(power);
//            POW = 0;
////            V_in.x += 1;
//            m_FullWakeRevs -= 0.5;
////            if (m_ActualTimeStep == 200) V_in.Set(6,0,0);
////            if (m_ActualTimeStep == 400) V_in.Set(7,0,0);
////            if (m_ActualTimeStep == 600) V_in.Set(8,0,0);
////            if (m_ActualTimeStep == 800) V_in.Set(9,0,0);
////            if (m_ActualTimeStep == 1000)V_in.Set(10,0,0);

////            V_in.RotZ(-m_YawAngle/180*PI);

//        }



        qDebug() << "CP:::::::::::::::" << cp << " at a TSR of ::::::::::::::::" << TSR << "power:::::::"<<power << "Omega::::::::::::" << m_Omega << "WakeSize::::::" <<m_WakeLine.size()<< "NodeSize::::::" <<m_WakeNode.size();;


        if (!m_bisVawt) RotateHAWTRotorGeometry();  //rotor dreht
        else RotateVAWTRotorGeometry();

        if (m_bWakeLine) WakeConcentration();       //lumps vortex lines together

        ConvectWakeNodes();                         //euler integration of wakenodes

        if (!m_bWakeLine) AddNewWakeRow();          //create a new wake row between last wake row and rotor
        else AddNewWakeLines();

        KuttaCondition();                           //assign new circulation to newly created wakelines





        m_bGlChanged = true;

        emit geomChanged();


        AppendResults(); // append simulation results in m_results array






//            //streamline visualization/////

//            #pragma omp parallel default (none) shared(point, i)
//            {
//            #pragma omp for
//                        for (int k=0;k<50;k++)
//                        {
//                            CVector speed2;
//                            speed2 = BiotSavart_InductEval(point[k]);
//                            point[k] += (/*V_in+speed2+*/TowerInfluence(point[k],V_in+speed2))*dT;

//                            m_Streamline[i+1+k*m_NumTimesteps] = point[k];
//                        }
//                    }

//                        /////////////////


//            while (timer2.elapsed() < 50){
//            }

        int numdisk = 200;

//    if (m_ActualTimeStep > 500 && fabs(m_BladePanel.at(0)->angularPos) <= 1.5){
//        x=-4;
//        wake1.clear();
//        for (int j=0; j<numdisk;j++){
//            double x1 = 1.8;
//            double x2 = 4*radius;
//            double x3 = 6*radius;

//            CVector P1;
//            CVector P2;
//            CVector P3;
//            P1.Set(x,+1.8,0);
//            P1.RotZ(-m_YawAngle/180*PI);
//    //        P2.Set(x,y,0);
//    //        P3.Set(x,y,0);

//            CVector dir(1,0,0);
//            dir.RotZ(-m_YawAngle/180*PI);


//            wake1.append(CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)).dot(dir));
//    //        wake2.append(CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)).VAbs()/V_in.VAbs());
//    //        wake3.append(CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)).VAbs()/V_in.VAbs());
//            x+= 10.0/numdisk;


//        }

//        double lll= -4;

//        qDebug() <<"WAKE+1.4";
//        for (int j=0;j<wake1.size();j++){


//            qDebug() << lll << wake1.at(j) /*<< wake2.at(j) << wake3.at(j)*/;
//            lll+= 10.0/numdisk;

//        }

//        x=-4;
//        wake1.clear();
//        for (int j=0; j<numdisk;j++){
//            double x1 = 1.8;
//            double x2 = 4*radius;
//            double x3 = 6*radius;

//            CVector P1;
//            CVector P2;
//            CVector P3;
//            P1.Set(x,-1.8,0);
//            P1.RotZ(-m_YawAngle/180*PI);
//    //        P2.Set(x,y,0);
//    //        P3.Set(x,y,0);

//            CVector dir(1,0,0);
//            dir.RotZ(-m_YawAngle/180*PI);


//            wake1.append(CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)).dot(dir));
//    //        wake2.append(CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)).VAbs()/V_in.VAbs());
//    //        wake3.append(CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)).VAbs()/V_in.VAbs());

//            x+= 10.0/numdisk;

//        }

//        lll= -4;

//        qDebug() <<"WAKE-1.4";
//        for (int j=0;j<wake1.size();j++){


//            qDebug() << lll << wake1.at(j) /*<< wake2.at(j) << wake3.at(j)*/;
//            lll+= 10.0/numdisk;

//        }


//    }

//    if (m_ActualTimeStep > 500 && fabs(m_BladePanel.at(0)->angularPos - 60) <= 1.5){
//    x=-3.5;
//    wake1.clear();
//    for (int j=0; j<numdisk;j++){
//        double x1 = 1.8;
//        double x2 = 4*radius;
//        double x3 = 6*radius;

//        CVector P1;
//        CVector P2;
//        CVector P3;
//        P1.Set(0.15,x,0);
////        P2.Set(x,y,0);
////        P3.Set(x,y,0);

//        CVector dir(1,0,0);
//        dir.RotZ(-m_YawAngle/180*PI);


//        wake1.append(CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)).dot(dir));
////        wake2.append(CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)).VAbs()/V_in.VAbs());
////        wake3.append(CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)).VAbs()/V_in.VAbs());
//        x+= 10.0/numdisk;


//    }

//    double lll= -3.5;

//    qDebug() <<"WAKE+az60";
//    for (int j=0;j<wake1.size();j++){


//        qDebug() << lll << wake1.at(j) /*<< wake2.at(j) << wake3.at(j)*/;
//        lll+= 10.0/numdisk;

//    }
//}

//    if (m_ActualTimeStep > 500 && fabs(m_BladePanel.at(0)->angularPos - 100) <= 1.5){
//    x=-3.5;
//    wake1.clear();
//    for (int j=0; j<numdisk;j++){
//        double x1 = 1.8;
//        double x2 = 4*radius;
//        double x3 = 6*radius;

//        CVector P1;
//        CVector P2;
//        CVector P3;
//        P1.Set(0.15,x,0);
////        P2.Set(x,y,0);
////        P3.Set(x,y,0);

//        CVector dir(1,0,0);
//        dir.RotZ(-m_YawAngle/180*PI);


//        wake1.append(CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)).dot(dir));
////        wake2.append(CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)).VAbs()/V_in.VAbs());
////        wake3.append(CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)).VAbs()/V_in.VAbs());
//        x+= 10.0/numdisk;


//    }

//    double lll= -3.5;
//qDebug() <<"WAKE+az100";
//    for (int j=0;j<wake1.size();j++){


//        qDebug() << lll << wake1.at(j) /*<< wake2.at(j) << wake3.at(j)*/;
//        lll+= 10.0/numdisk;

//    }
//}

}





    for (int j=0;j<angPos.size();j++){
        qDebug() << angPos.at(j) << CP.at(j) << CT.at(j) << AoA.at(j) << Time.at(j) << HHSpeed.at(j) << CN.at(j)<< angvel.at(j).x << angvel.at(j).y << angvel.at(j).z << angvel[j].VAbs();
    }

    for (int j=0; j<CPatV.size();j++){
        qDebug() << V.at(j) << Power.at(j) << CPatV.at(j);
    }

    m_ActualTimeStep = -1;

    //Zeitmessung
    nanoSec = timer.nsecsElapsed();
    sec = nanoSec / 1000000000;
    min = double(sec/60.0);
    qDebug()<<"Process took: "<<min<<"minutes "<<"["<<sec<<" seconds"<<"]"<<endl;
    //Zeitmessung



}

void QLLTSimulation::AppendResults(){

    m_results.clear();
    m_results.append(Time);
    m_results.append(angPos);
    m_results.append(CP);
    m_results.append(AoA);
    m_results.append(CT);

    m_module->m_graph[0]->updateGraph();
    m_module->m_graph[1]->updateGraph();
    m_module->m_graph[2]->updateGraph();
    m_module->m_graph[3]->updateGraph();

}

CVector QLLTSimulation::GetFreeStream(CVector EvalPt){
    if (m_WindField){
        CVector V_free = m_WindField->getWindspeed(EvalPt,m_ActualTime-dT/2);
        V_free.RotZ(m_InflowAngle/180*PI);
        return V_free;
    }
    else return V_in;


}


CVector QLLTSimulation::TowerInfluence(CVector EvalPt, CVector V_ref){

    if (!m_bIncludeTower) return V_ref;

    double height = EvalPt.z;

    if (height > 0) return V_ref;


    EvalPt -= CVector(m_TowerDistance, 0,0);


    EvalPt.z = 0;


    CVector Vref2 = CVector(V_ref.x, V_ref.y, 0);


    double alpha = acos(CVector(1,0,0).dot(Vref2)/(Vref2).VAbs());
    if (Vref2.y < 0 && Vref2.x > 0) alpha = 2*PI-alpha;

    //now the coordinates need to be rotated

    double d = EvalPt.VAbs()/m_TowerRadius;

    CVector newCoord = EvalPt;
    newCoord.RotZ(-alpha);

    double x = newCoord.x/m_TowerRadius;
    double y = newCoord.y/m_TowerRadius;

    double utower = m_TowerDrag/sqrt(d)*pow(cos(PI/2*y/sqrt(d)),2);
    if (x<0 || fabs(y) > sqrt(d)) utower = 0;

    double u = 1-(pow((x+0.1),2)-pow(y,2))/pow((pow((x+0.1),2)+pow(y,2)),2)-utower;
    double v = 2*((x+0.1)*y)/pow((pow((x+0.1),2)+pow(y,2)),2);

    CVector res(u*V_ref.VAbs(),v*V_ref.VAbs(),0);

    res.RotZ(alpha);
    res.z = V_ref.z;

    return res;

}


void QLLTSimulation::GLCreateRotorMesh(){

    double max;

    max = -1000;

    if (glIsList(GLBODYMESHPANELS)) glDeleteLists(GLBODYMESHPANELS,1);
    glLineWidth(1.0);

    for (int p=0; p<m_BladePanel.size(); p++){
        if (m_BladePanel[p]->isHub && m_BladePanel[p]->isTip){
            m_BladePanel[p]->m_Gamma_Node_LA = m_BladePanel[p]->m_Gamma;
            m_BladePanel[p]->m_Gamma_Node_LB = m_BladePanel[p]->m_Gamma;
        }
        else if (m_BladePanel[p]->isHub){
            m_BladePanel[p]->m_Gamma_Node_LA = m_BladePanel[p]->m_Gamma;
            m_BladePanel[p]->m_Gamma_Node_LB = fabs(m_BladePanel[p]->m_Gamma-m_BladePanel[p+1]->m_Gamma);
        }
        else if (m_BladePanel[p]->isTip){
            m_BladePanel[p]->m_Gamma_Node_LB = m_BladePanel[p]->m_Gamma;
            m_BladePanel[p]->m_Gamma_Node_LA = fabs(m_BladePanel[p]->m_Gamma-m_BladePanel[p-1]->m_Gamma);
        }
        else{
            m_BladePanel[p]->m_Gamma_Node_LA = fabs(m_BladePanel[p]->m_Gamma-m_BladePanel[p-1]->m_Gamma);
            m_BladePanel[p]->m_Gamma_Node_LB = fabs(m_BladePanel[p]->m_Gamma-m_BladePanel[p+1]->m_Gamma);
        }
        if (fabs(m_BladePanel[p]->m_Gamma_Node_LA) > max) max = fabs(m_BladePanel[p]->m_Gamma_Node_LA);
        if (fabs(m_BladePanel[p]->m_Gamma_Node_LB) > max) max = fabs(m_BladePanel[p]->m_Gamma_Node_LB);
    }


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glNewList(GLBODYMESHPANELS, GL_COMPILE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);

    glLineWidth(1.0);
    for (int i=0; i<m_BladePanel.size();i++){


    //panel circulation color

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);

    hsv hs;
    hs.s = 1;
    hs.v = 1;

    if (max>0) {


    glBegin(GL_QUADS);

    hs.h = (1-m_BladePanel[i]->m_Gamma_Node_LA/max)*225;
    glColor4d(hsv2rgb(hs).r,hsv2rgb(hs).g,hsv2rgb(hs).b,1);

    glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA
    glVertex3d(m_BladePanel[i]->pTA->x, m_BladePanel[i]->pTA->y, m_BladePanel[i]->pTA->z); //TA

    hs.h = (1-m_BladePanel[i]->m_Gamma_Node_LB/max)*225;
    glColor4d(hsv2rgb(hs).r,hsv2rgb(hs).g,hsv2rgb(hs).b,1);

    glVertex3d(m_BladePanel[i]->pTB->x, m_BladePanel[i]->pTB->y, m_BladePanel[i]->pTB->z); //TB
    glVertex3d(m_BladePanel[i]->pLB->x, m_BladePanel[i]->pLB->y, m_BladePanel[i]->pLB->z); //LB

    glEnd();

    }
    }

    glEndList();


    glNewList(1234, GL_COMPILE);
    for (int i=0; i<m_BladePanel.size();i++){
        glColor4d(0,0,0,1);

    //panel contour
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (i % (m_BladePanel.size()/num_blades) == 0){
        glBegin(GL_LINE_STRIP);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2, -2);
        glVertex3d(m_BladePanel[i]->pLB->x, m_BladePanel[i]->pLB->y, m_BladePanel[i]->pLB->z); //LB
        glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA
        glVertex3d(m_BladePanel[i]->pTA->x, m_BladePanel[i]->pTA->y, m_BladePanel[i]->pTA->z); //TA
        glVertex3d(m_BladePanel[i]->pTB->x, m_BladePanel[i]->pTB->y, m_BladePanel[i]->pTB->z); //TB

        glEnd();
    }
    else if((i+1) %(m_BladePanel.size()/num_blades) == 0){

        glBegin(GL_LINE_STRIP);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2, -2);
        glVertex3d(m_BladePanel[i]->pTA->x, m_BladePanel[i]->pTA->y, m_BladePanel[i]->pTA->z); //TA
        glVertex3d(m_BladePanel[i]->pTB->x, m_BladePanel[i]->pTB->y, m_BladePanel[i]->pTB->z); //TB
        glVertex3d(m_BladePanel[i]->pLB->x, m_BladePanel[i]->pLB->y, m_BladePanel[i]->pLB->z); //LB
        glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA
        glEnd();

    }
    else{
    glBegin(GL_LINE_STRIP);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2, -2);

    glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA
//    glVertex3d(m_BladePanel[i]->pTA->x, m_BladePanel[i]->pTA->y, m_BladePanel[i]->pTA->z); //TA
//    glVertex3d(m_BladePanel[i]->pTB->x, m_BladePanel[i]->pTB->y, m_BladePanel[i]->pTB->z); //TB
    glVertex3d(m_BladePanel[i]->pLB->x, m_BladePanel[i]->pLB->y, m_BladePanel[i]->pLB->z); //LB
//    glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA

    glEnd();

    glBegin(GL_LINE_STRIP);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2, -2);

//    glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA
    glVertex3d(m_BladePanel[i]->pTA->x, m_BladePanel[i]->pTA->y, m_BladePanel[i]->pTA->z); //TA
    glVertex3d(m_BladePanel[i]->pTB->x, m_BladePanel[i]->pTB->y, m_BladePanel[i]->pTB->z); //TB
//    glVertex3d(m_BladePanel[i]->pLB->x, m_BladePanel[i]->pLB->y, m_BladePanel[i]->pLB->z); //LB
//    glVertex3d(m_BladePanel[i]->pLA->x, m_BladePanel[i]->pLA->y, m_BladePanel[i]->pLA->z); //LA

    glEnd();
    }

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glColor4d(0,0,0,1);
//    glPointSize(5);
//    glBegin(GL_POINTS);
//    glVertex3d(m_BladePanel[i]->CtrlPt.x, m_BladePanel[i]->CtrlPt.y, m_BladePanel[i]->CtrlPt.z); //LA

//    glEnd();

//    glBegin(GL_POINTS);

//    int M = 11;
//    CVector EvalPt;
//    CVector MidL = CVector(m_BladePanel[i]->pLA + m_BladePanel[i]->pLB)/2;
//    CVector MidT = CVector(m_BladePanel[i]->pTA + m_BladePanel[i]->pTB)/2;
//    CVector chordline = MidL-MidT;
//    double chord = chordline.VAbs();
//    for (int k=1; k <= M; k++){
//        EvalPt = MidT-m_BladePanel[i]->a1*chord/2*(1-cos(PI*(k-1)/(M-1)));
//        glVertex3d(EvalPt.x, EvalPt.y, EvalPt.z); //LB
//    }
//    glEnd();
    }
    glEndList();

}

void QLLTSimulation::GLDrawVelocityField(){

    if (m_ActualTimeStep < m_NumTimesteps-1 || m_ActualTimeStep == -1) return;


    qDebug() << "GLCreateVelocityField";
    CVector P1, P2, P3, P4;



    if (glIsList(VELOCITYFIELD)) glDeleteLists(WINGWAKEPANELS,1);


    glNewList(VELOCITYFIELD,GL_COMPILE);
    {
//        glDisable(GL_DEPTH_TEST);

        //Panele einfärben
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );
        glClearColor(1.0,1.0,1.0,0.0);

        glPolygonOffset(1.0, 1.0);
        glLineWidth(1.0);

        hsv hs;
        hs.s = 1;
        hs.v = 1;

        double lengthX, lengthZ, startX;
        int numX = 200;
        int numZ = 100;

        double max = 0;
        for (int i=0; i<m_WakeLine.size();i++){
            if (m_WakeLine.at(i)->pL->x > max) max = m_WakeLine.at(i)->pL->x;
            if (m_WakeLine.at(i)->pT->x > max) max = m_WakeLine.at(i)->pT->x;
        }

        double A = 180.0;
        double opacity = 0.8;
        lengthX = max * 1.2;
        startX = -0.1*max;
        lengthZ = m_pBladeList[0]->getRotorRadius()*2*1.5;

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        {

            for (int i=0;i<numX-1;i++){
//#pragma omp parallel default (none) shared(numZ, numX, lengthZ, lengthX, startX, i)
//{
//#pragma omp for
                for (int j=0;j<numZ-1;j++){

                    CVector P1, P2, P3, P4;
                    hsv hs;
                    hs.s = 1;
                    hs.v = 1;
                    CVector VV;

                    P1.Set(startX+i*lengthX/numX,0,-lengthZ/2+lengthZ/numZ*j);
                    P2.Set(startX+(i+1)*lengthX/numX,0,-lengthZ/2+lengthZ/numZ*j);
                    P3.Set(startX+(i+1)*lengthX/numX,0,-lengthZ/2+lengthZ/numZ*(j+1));
                    P4.Set(startX+i*lengthX/numX,0,-lengthZ/2+lengthZ/numZ*(j+1));


                    VV = TowerInfluence(P1,CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P1.x,P1.y,P1.z);

                    VV = TowerInfluence(P2,CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P2.x,P2.y,P2.z);

                    VV = TowerInfluence(P3,CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P3.x,P3.y,P3.z);

                    VV = TowerInfluence(P4,CVector(BiotSavart_InductEval(P4)+GetFreeStream(P4)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P4.x,P4.y,P4.z);
                }
            }
//        }
        }
        glEnd();

//        glBegin(GL_QUADS);
//        {
////        #pragma omp parallel default (none) shared(numZ, numX, lengthZ, lengthX, startX)
////        {
////        #pragma omp for
//            for (int i=0;i<numX-1;i++){
//                for (int j=0;j<numZ-1;j++){

//                    CVector P1, P2, P3, P4;
//                    hsv hs;
//                    hs.s = 1;
//                    hs.v = 1;

//                    CVector VV;

//                    P1.Set(startX+i*lengthX/numX,-lengthZ/2+lengthZ/numZ*j,0);
//                    P2.Set(startX+(i+1)*lengthX/numX,-lengthZ/2+lengthZ/numZ*j,0);
//                    P3.Set(startX+(i+1)*lengthX/numX,-lengthZ/2+lengthZ/numZ*(j+1),0);
//                    P4.Set(startX+i*lengthX/numX,-lengthZ/2+lengthZ/numZ*(j+1),0);

//                    VV = TowerInfluence(P1,CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)));
//                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
//                    if (hs.h < 0) hs.h += 360;
//                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
//                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
//                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
//                    glVertex3d(P1.x,P1.y,P1.z);

//                    VV = TowerInfluence(P2,CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)));
//                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
//                    if (hs.h < 0) hs.h += 360;
//                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
//                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
//                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
//                    glVertex3d(P2.x,P2.y,P2.z);

//                    VV = TowerInfluence(P3,CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)));
//                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
//                    if (hs.h < 0) hs.h += 360;
//                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
//                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
//                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
//                    glVertex3d(P3.x,P3.y,P3.z);

//                    VV = TowerInfluence(P4,CVector(BiotSavart_InductEval(P4)+GetFreeStream(P4)));
//                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
//                    if (hs.h < 0) hs.h += 360;
//                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
//                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
//                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
//                    glVertex3d(P4.x,P4.y,P4.z);
//                }
//            }
//        }
////        }
//        glEnd();

        int numplanes = 0;
        double xpos = lengthX/(numplanes-1);
        for (int k=1;k<numplanes-1;k++){

if (k==1){
        glBegin(GL_QUADS);
        {
//#pragma omp parallel default (none) shared(numZ, numX, lengthZ, lengthX, startX)
//{
//#pragma omp for
            for (int i=0;i<numZ-1;i++){

                for (int j=0;j<numZ-1;j++){

                    CVector P1, P2, P3, P4;
                    hsv hs;
                    hs.s = 1;
                    hs.v = 1;

                    CVector VV;


                    P1.Set(xpos,-lengthZ/2+lengthZ/numZ*j,-lengthZ/2+i*lengthZ/numZ);
                    P2.Set(xpos,-lengthZ/2+lengthZ/numZ*j,-lengthZ/2+(i+1)*lengthZ/numZ);
                    P3.Set(xpos,-lengthZ/2+lengthZ/numZ*(j+1),-lengthZ/2+(i+1)*lengthZ/numZ);
                    P4.Set(xpos,-lengthZ/2+lengthZ/numZ*(j+1),-lengthZ/2+i*lengthZ/numZ);

                    VV = TowerInfluence(P1,CVector(BiotSavart_InductEval(P1)+GetFreeStream(P1)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P1.x,P1.y,P1.z);

                    VV = TowerInfluence(P2,CVector(BiotSavart_InductEval(P2)+GetFreeStream(P2)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P2.x,P2.y,P2.z);

                    VV = TowerInfluence(P3,CVector(BiotSavart_InductEval(P3)+GetFreeStream(P3)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P3.x,P3.y,P3.z);

                    VV = TowerInfluence(P4,CVector(BiotSavart_InductEval(P4)+GetFreeStream(P4)));
                    hs.h = A - (VV.VAbs()/V_in.VAbs())*180;
                    if (hs.h < 0) hs.h += 360;
                    glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.5);
                    if (VV.VAbs()/V_in.VAbs()>1) glColor4d(1,1-(VV.VAbs()/V_in.VAbs()-1),1-(VV.VAbs()/V_in.VAbs()-1),0.5);
                    else glColor4d(VV.VAbs()/V_in.VAbs(),VV.VAbs()/V_in.VAbs(),1,opacity);
                    glVertex3d(P4.x,P4.y,P4.z);
                }
            }
            }
//        }
        glEnd();
            }

        xpos+= lengthX/(numplanes-1);

        }


    }
    glEndList();
}


void QLLTSimulation::GLDrawStreamlines()
{

//    int num = 50;
//    int t = m_ActualTimeStep;

//    if (m_ActualTimeStep == m_NumTimesteps-1 || m_ActualTimeStep == m_NumTimesteps-2 || m_ActualTimeStep < 0) return;

//    if (glIsList(VLMSTREAMLINES)) glDeleteLists(VLMSTREAMLINES,1);

//    glNewList(VLMSTREAMLINES, GL_COMPILE);
//    {

//            glEnable (GL_LINE_STIPPLE);
//            glPolygonMode(GL_FRONT,GL_LINE);

//            glLineWidth(2);
//            glLineStipple (1, 0xFFFF);

//            glColor3d(1, 0, 0);

//            for (int k=0;k<num;k++)
//            {
//                    for (int p=0; p<=t; p++)
//                    {
//                        CVector A,B;
//                        double length, color;
//                        A.Set(m_Streamline[p+k*m_NumTimesteps].x,m_Streamline[p+k*m_NumTimesteps].y,m_Streamline[p+k*m_NumTimesteps].z);
//                        B.Set(m_Streamline[p+1+k*m_NumTimesteps].x,m_Streamline[p+1+k*m_NumTimesteps].y,m_Streamline[p+1+k*m_NumTimesteps].z);
//                        length = CVector(A-B).VAbs();
//                        length/=dT;
//                        color = length/V_in.VAbs();


//                        hsv hs;

//                        hs.h = fabs((1-color))*225;
//                        hs.s = 1;
//                        hs.v = 1;
//                        glColor3d(hsv2rgb(hs).r,hsv2rgb(hs).g,hsv2rgb(hs).b);



//                            glBegin(GL_LINES);
//                            {
//                                glVertex3d(m_Streamline[p+k*m_NumTimesteps].x,m_Streamline[p+k*m_NumTimesteps].y,m_Streamline[p+k*m_NumTimesteps].z);
//                                glVertex3d(m_Streamline[p+1+k*m_NumTimesteps].x,m_Streamline[p+1+k*m_NumTimesteps].y,m_Streamline[p+1+k*m_NumTimesteps].z);
//                            }
//                            glEnd();

//                    }
//            }

//            glDisable (GL_LINE_STIPPLE);
//    }
//    glEndList();

}


void QLLTSimulation::GLCreateWakeMesh(){

//    qDebug() << "GLCreateWakeMesh";

    if (glIsList(WINGWAKEPANELS)) glDeleteLists(WINGWAKEPANELS,1);


    glNewList(WINGWAKEPANELS,GL_COMPILE);
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);


        //Panele einfärben
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );
        glClearColor(1.0,1.0,1.0,0.0);

        glPolygonOffset(1.0, 1.0);
        glLineWidth(1.0);

        hsv hs;



        //Linien zeichnen
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor3d(0,0,0);

        double max=0;
        for (int p=0; p<m_WakePanel.size(); p++){
            m_WakePanel[p]->m_Gamma_Node_LB=fabs(m_WakePanel[p]->m_Gamma_Node_LB);
            m_WakePanel[p]->m_Gamma_Node_TB=fabs(m_WakePanel[p]->m_Gamma_Node_TB);
            m_WakePanel[p]->m_Gamma_Node_TA=fabs(m_WakePanel[p]->m_Gamma_Node_TA);
            m_WakePanel[p]->m_Gamma_Node_LA=fabs(m_WakePanel[p]->m_Gamma_Node_LA);
        }

        for (int p=0; p<m_BladePanel.size(); p++){
            if (fabs(m_BladePanel[p]->m_Gamma_Node_LA) > max) max = fabs(m_BladePanel[p]->m_Gamma_Node_LA);
            if (fabs(m_BladePanel[p]->m_Gamma_Node_LB) > max) max = fabs(m_BladePanel[p]->m_Gamma_Node_LB);
        }


//        max = 2;
//        qDebug() << "MAX" << max;

        if (!m_bWakeLine){
        for (int p=0; p<m_WakePanel.size(); p++)
        {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        glBegin(GL_QUADS);
                        {

                        hs.h = (1-m_WakePanel[p]->m_Gamma_Node_LA/max)*225;
                        hs.s = 1;
                        hs.v = 1;


                        glNormal3d(m_WakePanel[p]->Normal.x, m_WakePanel[p]->Normal.y, m_WakePanel[p]->Normal.z);

                        glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,m_WakePanel[p]->m_Gamma_Node_LA/max);
                        glVertex3d(m_WakePanel[p]->pLA->x, m_WakePanel[p]->pLA->y, m_WakePanel[p]->pLA->z);

                        hs.h = (1-m_WakePanel[p]->m_Gamma_Node_LB/max)*225;
                        glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,m_WakePanel[p]->m_Gamma_Node_LB/max);
                        glVertex3d(m_WakePanel[p]->pLB->x, m_WakePanel[p]->pLB->y, m_WakePanel[p]->pLB->z);

                        hs.h = (1-m_WakePanel[p]->m_Gamma_Node_TB/max)*225;
                        glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,m_WakePanel[p]->m_Gamma_Node_TB/max);
                        glVertex3d(m_WakePanel[p]->pTB->x, m_WakePanel[p]->pTB->y, m_WakePanel[p]->pTB->z);

                        hs.h = (1-m_WakePanel[p]->m_Gamma_Node_TA/max)*225;
                        glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,m_WakePanel[p]->m_Gamma_Node_TA/max);
                        glVertex3d(m_WakePanel[p]->pTA->x, m_WakePanel[p]->pTA->y, m_WakePanel[p]->pTA->z);
                    }
                    glEnd();

//                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//                    glColor4d(0, 0, 0,1);
//                    glLineWidth(0.2);
//                    glBegin(GL_QUADS);
//                    {
//                        glNormal3d(m_WakePanel[p]->Normal.x, m_WakePanel[p]->Normal.y, m_WakePanel[p]->Normal.z);


//                        glVertex3d(m_WakePanel[p]->pLA->x, m_WakePanel[p]->pLA->y, m_WakePanel[p]->pLA->z);


//                        glVertex3d(m_WakePanel[p]->pLB->x, m_WakePanel[p]->pLB->y, m_WakePanel[p]->pLB->z);


//                        glVertex3d(m_WakePanel[p]->pTB->x, m_WakePanel[p]->pTB->y, m_WakePanel[p]->pTB->z);


//                        glVertex3d(m_WakePanel[p]->pTA->x, m_WakePanel[p]->pTA->y, m_WakePanel[p]->pTA->z);

//                    }
//                    glEnd();



        }
        }
        else{
            for (int p=0; p<m_WakeLine.size(); p++)
            {
            if (m_WakeLine.at(p)->fromStation < m_BladeLine.size()/num_blades || m_bGLPlotAllBlades){

                if (m_WakeLine[p]->isTrailing) glColor4d(0, 0, 0,1);
                if (m_WakeLine[p]->isShed) glColor4d(1, 0, 0,1);
                glLineWidth(0.5);
                if (m_bGLPlotWakeLines && (m_ActualTimeStep - m_WakeLine[p]->fromTimestep)*m_AngularDisc/360 < m_FineWakeRevs+m_FullWakeRevs){
                glBegin(GL_LINES);
                {
                    glVertex3d(m_WakeLine[p]->pL->x, m_WakeLine[p]->pL->y, m_WakeLine[p]->pL->z);

                    glVertex3d(m_WakeLine[p]->pT->x, m_WakeLine[p]->pT->y, m_WakeLine[p]->pT->z);
                }
                glEnd();
                }
                if (floor(m_WakeLine.at(p)->fromStation / (m_BladeLine.size()/num_blades)) == 0) glColor4d(0, 0, 0,1);
                if (floor(m_WakeLine.at(p)->fromStation / (m_BladeLine.size()/num_blades)) == 1) glColor4d(1, 0, 0,1);
                if (floor(m_WakeLine.at(p)->fromStation / (m_BladeLine.size()/num_blades)) == 2) glColor4d(0, 0, 1,1);

                glPointSize(2.0);
                if (m_bGLPlotWakeNodes){
                glBegin(GL_POINTS);
                {

                    glVertex3d(m_WakeLine[p]->pL->x, m_WakeLine[p]->pL->y, m_WakeLine[p]->pL->z);

                    glVertex3d(m_WakeLine[p]->pT->x, m_WakeLine[p]->pT->y, m_WakeLine[p]->pT->z);

                }
                glEnd();
                }
            }
        }
        }
    }
    glEndList();

}

void QLLTSimulation::RotateGeom(){

    double deltaYaw = m_YawAngle - Yaw_Old;


    HRotorAxis.Set(1,0,0);
    HRotorAxis.RotateY(CVector(0,0,0),m_TeetherAngle);
    HRotorAxis.RotateZ(CVector(0,0,0),m_YawAngle);

    VRotorAxis.Set(0,0,1);
    VRotorAxis.RotateY(CVector(0,0,0),m_TeetherAngle);

    Yaw_Old = m_YawAngle;

    CVector axis;
    if (!m_bisVawt) axis = HRotorAxis;
    if ( m_bisVawt) axis = VRotorAxis;




    for (int j=0;j<m_BladeNode.size();j++)
    {
        m_BladeNode[j]->RotateZ(CVector(0,0,0),deltaYaw);
    }

    for (int i=0;i<num_blades;i++)
    {
        for(int j=0;j<m_pBladeList[i]->m_NPanel;j++)
        {
            m_pBladeList[i]->m_Surface[j].RotateZ(axis,deltaYaw);
        }
    }

    for (int j=0;j<m_BladeNode.size();j++)
    {
        m_BladeNode[j]->RotateN(axis,180/PI*m_Omega*dT);
    }

    for (int i=0;i<num_blades;i++)
    {
        for(int j=0;j<m_pBladeList[i]->m_NPanel;j++)
        {
            m_pBladeList[i]->m_Surface[j].RotateN(axis,180/PI*m_Omega*dT);
        }
    }

}


void QLLTSimulation::RotateHAWTRotorGeometry()
{
    // rotate the rotor geometry

    CVector WLA,WLB,WTA,WTB,LATB,TALB;

    RotateGeom();

    PitchBlades();

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

        // this computes the angular position of the panel and stores it in the panel class
        m_BladePanel[i]->angularPos += m_Omega*dT/PI*180;
        m_BladePanel[i]->angularPos += 360 / num_blades * m_BladePanel[i]->blade_number;
        if (m_BladePanel[i]->angularPos > 360 || m_BladePanel[i]->angularPos == 360) m_BladePanel[i]->angularPos -= 360;

    }
}

void QLLTSimulation::PitchBlades(){

    CVector pitchAxis, TA, TB, LA, LB, PA, PB;

    double Deltam_PitchAngle = m_PitchAngle - Pitch_Old;

    Pitch_Old = m_PitchAngle;

    int blade = 0;

    for (int j=0;j<m_BladeNode.size();j++)
    {
        if ((j!=0) && (j % (m_BladeNode.size()/num_blades)) == 0) blade += 1;

        LA = m_pBladeList[blade]->m_Surface[0].m_LA;
        LB = m_pBladeList[blade]->m_Surface[m_pBladeList[blade]->m_NPanel-1].m_LB;
        TA = m_pBladeList[blade]->m_Surface[0].m_TA;
        TB = m_pBladeList[blade]->m_Surface[m_pBladeList[blade]->m_NPanel-1].m_TB;

        PA = LA+(TA-LA)*m_pBladeList[blade]->m_TFoilPAxisX[0];
        PB = LB+(TB-LB)*m_pBladeList[blade]->m_TFoilPAxisX[m_pBladeList[blade]->m_NPanel];
        pitchAxis = PA-PB;
        m_BladeNode[j]->Rotate(PA,pitchAxis,Deltam_PitchAngle);
    }

}

void QLLTSimulation::RotateVAWTRotorGeometry()
{
    ////// rotate the rotor geometry

    CVector WLA,WLB,WTA,WTB,LATB,TALB;

    RotateGeom();

    PitchBlades();

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

        // this computes the angular position of the panel and stores it in the panel class
        m_BladePanel[i]->angularPos += m_Omega*dT/PI*180;
        m_BladePanel[i]->angularPos += 360 / num_blades * m_BladePanel[i]->blade_number;
        if (m_BladePanel[i]->angularPos > 360 || m_BladePanel[i]->angularPos == 360) m_BladePanel[i]->angularPos -= 360;


    }

    qDebug() <<"VAWT AngularVel_HAWT"<< m_ActualTimeStep << m_BladePanel[0]->angularPos << m_BladePanel[0]->m_V_total.VAbs() << m_BladePanel[0]->m_AoA;

}


void QLLTSimulation::AddNewWakeRow(){
//create the new shed wake line
//nach jedem Funk aufruf erhöht sich m_nXWakePanels um 1
///////////////////////////////


    VortexNode LATB,TALB;
    VortexNode * TA,*TB,*LA,*LB;
    VortexNode *n0,*n1;
    int c = m_WakePanel.size()-m_BladePanel.size(); //m_nWakeColumn == m_BladePanel.size() !// m_nXWakePanels started bei 1

    for (int i=0;i<m_BladePanel.size();i++)
    {
        CPanel *panel = new CPanel;

        LA = new VortexNode;
        LB = new VortexNode;

        *LA = *m_BladePanel[i]->pTA;
        *LB = *m_BladePanel[i]->pTB;

        n0 = IsWakeNode(*LA);
        n1 = IsWakeNode(*LB);

        if (i % (m_BladePanel.size()/num_blades) == 0) panel->isHub = true;
        if ((i+1) % (m_BladePanel.size()/num_blades) == 0) panel->isTip = true;

        TA = m_WakePanel[c]->pLA;
        panel->pTA = m_WakePanel[c]->pLA;
        TB = m_WakePanel[c]->pLB;
        panel->pTB = m_WakePanel[c]->pLB;

        c++;

        LATB = *TB - *LA;
        TALB = *LB - *TA;

        panel->Normal = LATB * TALB;
        panel->m_bIsWakePanel = true;
        panel->Area =  panel->Normal.VAbs()/2.0;
        panel->Normal.Normalize();
        panel->SetFrame(*LA,*LB, *TA, *TB);
        panel->m_iSym     = -1;
        panel->m_bIsLeftPanel  = false;


        if(n0) {
            panel->pLA = n0; //LA Wake Panel = TA Wing Panel
            delete LA;
        }
        else {
            panel->pLA = LA;
            m_WakeNode.push_back(LA);
        }

        if(n1) {
            panel->pLB = n1; //LB Wake Panel = TB Wing Panel
            delete LB;
        }
        else {
            panel->pLB = LB;
            m_WakeNode.push_back(LB);
        }

        CVector WLA,WLB,WTA,WTB;
        WLA.Copy(*panel->pLA);
        WLB.Copy(*panel->pLB);
        WTA.Copy(*panel->pTA);
        WTB.Copy(*panel->pTB);
        panel->SetFrame(WLA, WLB, WTA, WTB);
        m_WakePanel.push_back(panel);
    }
}

void QLLTSimulation::AddNewWakeLines(){
//create the new shed wake line
//nach jedem Funk aufruf erhöht sich m_nXWakePanels um 1
///////////////////////////////

    VortexNode *L, *T;
    VortexLine *line;


    if (m_bShed){
        int pos = 0;
        for (int i=m_WakeNode.size()-m_BladeLine.size();i<m_WakeNode.size();i++){

            if (((pos+1) % (m_BladeLine.size()/num_blades)) != 0){
                VortexLine *line = new VortexLine;
                line->fromStation = m_BladeLine[pos]->fromStation;
                line->fromTimestep = m_ActualTimeStep;
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
                line->Update(dT);
                m_WakeLine.push_back(line);
            }

            pos++;

        }
    }


    for (int i=0;i<m_BladeLine.size();i++)
    {
        //search for the corresponding downstream node
        for (int j=0;j<m_WakeLine.size();j++){
            if (m_bTrail && m_WakeLine.at(j)->isTrailing && (m_WakeLine.at(j)->fromTimestep == m_ActualTimeStep-1) && (m_WakeLine.at(j)->fromStation == m_BladeLine[i]->fromStation)){
                 T = m_WakeLine.at(j)->pL;
            }
        }


        //create new leading node
        L = new VortexNode;
        *L = *m_BladeLine[i]->pT;
        L->fromTimestep = m_ActualTimeStep;
        L->fromStation  = m_BladeLine[i]->fromStation;
        m_WakeNode.push_back(L);

        //add trailing vorticity
        if (m_bTrail){
            line = new VortexLine;
            line->pL = L;
            line->pT = T;
            line->fromStation = m_BladeLine[i]->fromStation;
            line->isHub = m_BladeLine[i]->isHub;
            line->isTip = m_BladeLine[i]->isTip;
            line->fromTimestep = m_ActualTimeStep;
            line->m_KinViscosity = m_KinViscosity;
            line->m_TurbulentViscosity = m_TurbulentViscosity;
            line->m_VortexTimeOffset = m_VortexTimeOffset;
            line->isTrailing = true;
            line->m_Lines = &m_WakeLine;
            line->m_Nodes = &m_WakeNode;
            line->Update(dT);
            //connect node to line vortex
            T->attachLine(line);
            L->attachLine(line);
            m_WakeLine.push_back(line);
        }




    }


}

void QLLTSimulation::WakeConcentration(){

    // here the wake is lumped into concentrated vortices after the specified number of "m_FullWakeRevs"
    int newlines = 0;
    if (m_bShed)  newlines+=m_BladeLine.size()-num_blades;
    if (m_bTrail) newlines+=m_BladeLine.size();
    QVector<VortexLine *> lines;
    QVector<VortexLine *> newLines;
    std::vector<std::vector <VortexNode *> > nodes;
    std::vector<VortexNode *> newVNodes;
    double Gamma_Max = 0;
    double age;
    int fromTimeStep;

    for (int i=m_WakeLine.size()-newlines-1;i>=0;i--){
        if ((m_ActualTimeStep - m_WakeLine.at(i)->fromTimestep) * m_AngularDisc / 360  > m_MaxWakeAge) m_WakeLine.at(i)->Delete();
    }


    int num_bladelines = m_BladeLine.size() / num_blades;

    // here the vortexlines that have to be converted into lumped vortices are searched and appended into the lines array; the shed vorticity is removed

    for (int blade = 0;blade < num_blades; blade++){
        int linestart = blade * num_bladelines;
        int lineend = (blade+1) * num_bladelines;
            lines.clear();
            for (int i=m_WakeLine.size()-newlines-1;i>=0;i--){
                if ((m_ActualTimeStep - m_WakeLine.at(i)->fromTimestep) * m_AngularDisc / 360.0 >= m_FullWakeRevs){
                    if (m_WakeLine.at(i)->isShed) {
                        m_WakeLine.at(i)->Delete();
                    }
                    else if (!m_WakeLine.at(i)->isConcentrated && m_WakeLine.at(i)->fromStation >= linestart &&  m_WakeLine.at(i)->fromStation < lineend){
                        lines.append(m_WakeLine.at(i));
                    }

                }

            }

            if (lines.size()) fromTimeStep = lines.at(0)->fromTimestep;


            for (int n=0;n< lines.size();n++){
                if (fabs(lines.at(n)->GetGamma()) > Gamma_Max) Gamma_Max = fabs(lines.at(n)->GetGamma());
            }


            for (int n=lines.size()-1; n>=0;n--){
                if (n==0) age = lines.at(n)->age;
                if (fabs(lines.at(n)->GetGamma() / Gamma_Max) < m_MinGammaFrac){
                    lines.at(n)->Delete();
                    lines.removeAt(n);
                }
            }

            double Gamma = 0;
            CVector Vec = CVector(0,0,0);
            CVector VecT = CVector(0,0,0);

            QVector<double>  VGamma;
            QVector<CVector> VVec, VVecT;


            Gamma = 0;
            Vec = CVector(0,0,0);
            VecT = CVector(0,0,0);

            // here the zero crossings of the gamma derivative are found, for every zero crossing a vortex is created later that rotates in the appropriate direction

            for (int l = 0; l<lines.size()-1;l++){

            //gamma stores the total vortex strength of the vortex that will be created
            //vec and vect are used to compute a weighted position for the shedding of the vortex



                    Gamma += lines.at(l)->GetGamma();
                    Vec += *lines.at(l)->pL*fabs(lines.at(l)->GetGamma());
                    VecT += *lines.at(l)->pT*fabs(lines.at(l)->GetGamma());

                    if (lines.at(l)->GetGamma() * lines.at(l+1)->GetGamma() <= 0 ){
                        VGamma.append(Gamma);
                        VVec.append(Vec);
                        VVecT.append(VecT);

                        Gamma = 0;
                        Vec = CVector(0,0,0);
                        VecT = CVector(0,0,0);
                    }



                if (l==lines.size()-2){
                        Gamma += lines.at(l+1)->GetGamma();
                        Vec += *lines.at(l+1)->pL*fabs(lines.at(l+1)->GetGamma());
                        VecT += *lines.at(l+1)->pT*fabs(lines.at(l+1)->GetGamma());


                        VGamma.append(Gamma);
                        VVec.append(Vec);
                        VVecT.append(VecT);
                }


            }

//            QVector<int> vortpositions;
//            vortpositions.clear();

//            for (int m=0;m<VGamma.size();m++){

//                double min = 10e9;
//                int vortpos = -1;
//                CVector Vec = VVec.at(m);
//                Vec = Vec / fabs(VGamma.at(m));

//                for (int l=0;l<lines.size();l++){
//                    if (CVector(*lines.at(l)->pL-Vec).VAbs() < min){
//                        vortpos = l;
//                        min = CVector(*lines.at(l)->pL-Vec).VAbs();
//                    }

//                }
//                qDebug()<<"VORTPOS"<<vortpos;
//                vortpositions.append(vortpos);

//            }

//            for (int m=0;m<VGamma.size();m++){
//                lines.at(vortpositions.at(m))->isConcentrated = true;
//                lines.at(vortpositions.at(m))->SetGamma(VGamma.at(m));
//            }




            for (int m=VGamma.size()-1;m>=0;m--){
//                qDebug() << "Gamma" << VGamma.at(m);

            }

            int fromStation = 0;
            if (lines.size()){
                fromStation = lines.at(0)->fromStation;
            }


            for (int m=lines.size()-1;m>=0;m--){
//                qDebug() << "Lines" << lines.at(m)->GetGamma();

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
                        line->fromStation = fromStation;
                        line->fromTimestep = fromTimeStep;


                        bool found = false;
                        CVector Vec = VVec.at(z);
                        CVector VecT = VVecT.at(z);
                        Vec = Vec / fabs(VGamma.at(z));
                        VecT = VecT / fabs(VGamma.at(z));


                        if (TempConnectionNode.size()){
                            for (int k=0; k<TempConnectionNode.at(blade).size();k++){
                                if (TempConnectionNode.at(blade).size() == VGamma.size()){
                                    found = true;
                                    TempConnectionNode.at(blade).at(z)->attachLine(line);
                                    line->pT = TempConnectionNode.at(blade).at(z);
                                }
                            }
                        }

                        if (!found){
                            VortexNode *node = new VortexNode;
                            *node = VecT;
                            node->attachLine(line);
                            line->pT = node;
                            m_WakeNode.insert(m_WakeNode.begin(),node);
                        }


                        VortexNode *node = new VortexNode;
                        *node = Vec;
                        node->attachLine(line);
                        line->pL = node;
                        m_WakeNode.insert(m_WakeNode.begin(),node);
                        newVNodes.push_back(node);

                        line->Update(dT);
                        newLines.append(line);
            }

            nodes.push_back(newVNodes);


    }

    TempConnectionNode.clear();
    for (int j=0;j<num_blades;j++)
        if (nodes.size()){
                TempConnectionNode.push_back(nodes.at(j));
        }

    for (int i=0; i< newLines.size();i++){
        m_WakeLine.insert(m_WakeLine.begin(),newLines.at(i));
    }

 if (newLines.size()){

//now thin out wake lines
    if (!(m_ActualTimeStep % m_Thinfactor == 0)){
        VortexLine *vLine1;
        for (int i=m_WakeLine.size()-1; i>=0;i--){
            if (m_WakeLine.at(i)->fromTimestep == fromTimeStep-1-int(m_FineWakeRevs/m_AngularDisc*360.0)){

              if (m_WakeLine.at(i)->pL->attachedLines.size()==2 && m_WakeLine.at(i)->pT->attachedLines.size()==2){
                  if (m_WakeLine.at(i)->pT->attachedLines[0] == m_WakeLine.at(i))  vLine1 = (VortexLine *) m_WakeLine.at(i)->pT->attachedLines[1];
                  else vLine1 = (VortexLine *) m_WakeLine.at(i)->pT->attachedLines[0];

                  m_WakeLine.at(i)->pT->detachLine(vLine1);
                  m_WakeLine.at(i)->pL->attachLine(vLine1);
                  vLine1->pL = m_WakeLine.at(i)->pL;

                  m_WakeLine.at(i)->Delete();

              }
            }
        }
    }

}


}




void QLLTSimulation::ConvectWakeNodes(){

    //Update of all nodes in the wake due to new position calculated with m_V_induced and dT

    //temporary storage of wake nodes
    TempWakeNode.clear();


    for (int i=0;i<m_WakeNode.size();i++){
        VortexNode C;
        C = *m_WakeNode.at(i);
        C.wasConvected = false;
        TempWakeNode.push_back(C);
    }

    if (m_InterpolationFactor == 1 || m_bWakeRollUp == false){
    // normal convection of wake panels except the newly created row
        #pragma omp parallel default (none)
        {
            #pragma omp for
            for (int i=0;i<m_WakeNode.size()-m_BladeLine.size();i++){
                if(m_bWakeRollUp == true){
                    if (m_bTurbConvection) TempWakeNode[i] += GetFreeStream((*m_WakeNode[i])+BiotSavart_InductEval(*m_WakeNode[i], m_bPrescribed))*dT;
                    else {
                        CVector MeanInflow(m_WindField->getMeanWindSpeedAtHub(),0,0);
                        MeanInflow.RotZ(m_InflowAngle/180*PI);
                        TempWakeNode[i] += (MeanInflow+BiotSavart_InductEval(*m_WakeNode[i], m_bPrescribed))*dT;
                    }
                }
                else{
                     if (m_bTurbConvection) TempWakeNode[i] += GetFreeStream(*m_WakeNode[i])*dT;
                     else{
                         CVector MeanInflow(m_WindField->getMeanWindSpeedAtHub(),0,0);
                         MeanInflow.RotZ(m_InflowAngle/180*PI);
                         TempWakeNode[i] += MeanInflow*dT;
                     }
                }
            }
        }
    }
    else InterpolateWakeSpeeds();



 //convect new wake row at trailing edge -> the proximity to the trailing edge is a user variable: m_firstwakerowlength
    if(!m_bisVawt){
        #pragma omp parallel default (none)
        {
        #pragma omp for
                for (int l=m_WakeNode.size()-m_BladeLine.size();l<m_WakeNode.size();l++) // only the new row of nodes is treated here
                {
                        CVector TWN(TempWakeNode[l].x,TempWakeNode[l].y,TempWakeNode[l].z);
                        TWN.Rotate(HRotorAxis,m_Omega*dT*(1.0-m_firstWakeRowLength)*180/PI);
                        TempWakeNode[l] = TWN;
                        if (m_bWakeRollUp) TempWakeNode[l] += (V_in + BiotSavart_InductEval(TempWakeNode[l], m_bPrescribed))*dT*m_firstWakeRowLength;
                        else TempWakeNode[l] += (V_in)*dT*m_firstWakeRowLength;
                }
        }
    }
    else{
        #pragma omp parallel default (none)
        {
        #pragma omp for
                for (int l=m_WakeNode.size()-m_BladeLine.size();l<m_WakeNode.size();l++) // only the new row of nodes is treated here
                {
                        CVector TWN(TempWakeNode[l].x,TempWakeNode[l].y,TempWakeNode[l].z);
                        TWN.Rotate(VRotorAxis,m_Omega*dT*(1-m_firstWakeRowLength)*180/PI);
                        TempWakeNode[l] = TWN;
                        if (m_bWakeRollUp) TempWakeNode[l] += (V_in + BiotSavart_InductEval(TempWakeNode[l], m_bPrescribed))*dT*m_firstWakeRowLength;
                        else TempWakeNode[l] += (V_in)*dT*m_firstWakeRowLength;
                }
        }
    }


   //copy back from temporary storage
   for (int i=0;i<m_WakeNode.size();i++){
       *m_WakeNode[i] = TempWakeNode.at(i);
   }

    // Re-create the wake panels

    CVector WLA,WLB,WTA,WTB,LATB,TALB;

    if (!m_bWakeLine){
    for (int i=0; i<m_WakePanel.size(); i++)
    {
        WLA.Copy(*m_WakePanel[i]->pLA);
        WLB.Copy(*m_WakePanel[i]->pLB);
        WTA.Copy(*m_WakePanel[i]->pTA);
        WTB.Copy(*m_WakePanel[i]->pTB);

        LATB = WTB - WLA;
        TALB = WLB - WTA;

        m_WakePanel[i]->Normal = LATB * TALB;
        m_WakePanel[i]->Area =  m_WakePanel[i]->Normal.VAbs()/2.0;
        m_WakePanel[i]->Normal.Normalize();
        m_WakePanel[i]->SetFrame(WLA, WLB, WTA, WTB);
    }
    }
    else{
        for (int i=0; i<m_WakeLine.size(); i++)
        {
        m_WakeLine[i]->Update(dT);
        }
    }

}

void QLLTSimulation::InterpolateWakeSpeeds(){

    // linear velocity interpolation
    #pragma omp parallel default (none)
    {
        #pragma omp for
        for (int i=0;i<m_WakeNode.size()-m_BladeLine.size();i++){
            if(m_bWakeRollUp == true && m_WakeNode[i]->fromTimestep % m_InterpolationFactor == 0){
                TempWakeNode[i].V_induced = (BiotSavart_InductEval(*m_WakeNode[i], m_bPrescribed));
                TempWakeNode[i] += (TempWakeNode[i].V_induced+GetFreeStream(*m_WakeNode[i]))*dT;
                TempWakeNode[i].wasConvected = true;
            }
        }
    }

#pragma omp parallel default (none)
{
#pragma omp for
    for (int i=0;i<m_WakeNode.size()-m_BladeLine.size();i++){
        bool up = false;
        bool down = false;
        int jj;
        int kk;
        if (!TempWakeNode[i].wasConvected){
        for (int j=0;j<m_WakeNode.size()-m_BladeLine.size();j++){
            if (m_WakeNode[j]->fromTimestep - m_WakeNode[i]->fromTimestep <= m_InterpolationFactor-1 && m_WakeNode[j]->fromTimestep - m_WakeNode[i]->fromTimestep >= 0 && TempWakeNode[j].wasConvected && m_WakeNode[j]->fromStation == m_WakeNode[i]->fromStation){
                up = true;
                jj = j;
                break;
            }
        }
        for (int k=0;k<m_WakeNode.size()-m_BladeLine.size();k++){
            if (m_WakeNode[i]->fromTimestep - m_WakeNode[k]->fromTimestep <= m_InterpolationFactor-1 && m_WakeNode[i]->fromTimestep - m_WakeNode[k]->fromTimestep >= 0 && TempWakeNode[k].wasConvected && m_WakeNode[k]->fromStation == m_WakeNode[i]->fromStation){
                down = true;
                kk = k;
                break;
            }
        }

        //interpolation
        if (up && down){
//            qDebug() << i << CVector(TempWakeNode[jj].V_induced*(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs()) + TempWakeNode[kk].V_induced*(CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs())).VAbs()<<TempWakeNode[jj].V_induced.VAbs() << TempWakeNode[kk].V_induced.VAbs() << (CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs()) << (CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs()) << jj << kk;
            TempWakeNode[i] += (TempWakeNode[jj].V_induced*(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs()) + TempWakeNode[kk].V_induced*(CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs())/(CVector(*m_WakeNode[jj]-*m_WakeNode[i]).VAbs()+CVector(*m_WakeNode[kk]-*m_WakeNode[i]).VAbs())+V_in)*dT;
        }
        //no interpolation
        else {
            TempWakeNode[i] += (GetFreeStream(*m_WakeNode[i])+BiotSavart_InductEval(*m_WakeNode[i], m_bPrescribed))*dT;
        }
        }
    }

}
}


void QLLTSimulation::KuttaCondition()
{
    //1)hier wird jeder neuen Wake-Reihe eine im loop neu berechnete Gamma-Verteilung vom Wing zugewiesen
    //2)die initiale Wirbellänge der vier Wirbelringelemente in jedem neuen Wake Panel berechnet
    //
    //Input: Wing: m_Gamma
    //Output: Wake: m_Gamma

    if (!m_bWakeLine){

    int IDP = 0;// IDP startet bei 0 bis m_BladePanel.size()

    for(int ID = (m_WakePanel.size()-m_BladePanel.size());ID < m_WakePanel.size();ID++){

        m_WakePanel[ID]->m_Gamma = m_BladePanel[IDP]->m_Gamma;

        //each vortex element should only be used once during biot savart induction computation, thats why the Gamma delta is computed and stored here at the corresponding panel vortex lines
        if (m_WakePanel[ID]->isTip){
            // at the tip
            m_WakePanel[ID]->m_GammaLBTB = m_BladePanel[IDP]->m_Gamma;
        }
        // somewhere in between, delta is computed
        else m_WakePanel[ID]->m_GammaLBTB = m_BladePanel[IDP]->m_Gamma-m_BladePanel[IDP+1]->m_Gamma;

        if (m_bShed){
        if (m_WakePanel.size()==m_BladePanel.size() ){
            //this is the first shed wake row
            m_WakePanel[ID]->m_GammaTBTA = m_BladePanel[IDP]->m_Gamma;
        }
        else{
            // somewhere in between delta is computed
            m_WakePanel[ID]->m_GammaTBTA = m_BladePanel[IDP]->m_Gamma-m_WakePanel[ID-m_BladePanel.size()]->m_Gamma;
        }
        }
        else m_WakePanel[ID]->m_GammaTBTA = 0;

        // the LALB vortex is always 0 since the panel circulation is equivalent to the blade circulation in the first wake column, in later wake columns only the TBTA line is used
        m_WakePanel[ID]->m_GammaLALB = 0;

        // the TALA line vortex is only needed if the panel is at the hub of a blade, if not only the LBTB line vortex of each panel is used
        if (m_WakePanel[ID]->isHub) m_WakePanel[ID]->m_GammaTALA = m_BladePanel[IDP]->m_Gamma;
        else m_WakePanel[ID]->m_GammaTALA = 0;

        IDP++;
    }
    }
    else{
        int newlines = 0;
        int IDP = 0;
        if (m_bTrail){
            newlines += m_BladeLine.size();
            for(int ID = (m_WakeLine.size()-newlines);ID < m_WakeLine.size();ID++){
               m_WakeLine[ID]->Gamma = m_BladeLine[IDP]->Gamma;
               IDP++;
            }
        }
        if (m_bShed){
            IDP = 0;
            newlines += m_BladeLine.size()-num_blades;
            for(int ID = (m_WakeLine.size()-newlines);ID < m_WakeLine.size()-(newlines-m_BladeLine.size()+num_blades);ID++){
               if (m_ActualTimeStep==0) m_WakeLine[ID]->Gamma = -m_BladePanel[IDP]->m_Gamma;
               else m_WakeLine[ID]->Gamma = m_BladePanel[IDP]->m_Gamma_lastTimestep-m_BladePanel[IDP]->m_Gamma;
               IDP++;
            }
        }


    }

    InterpolateDeltaGammaOnWakeNodes();

}

void QLLTSimulation::InterpolateDeltaGammaOnWakeNodes(){

    //Here the Gamma Values are interpolated onto the nodes for later use during wake visualization

    if (m_WakePanel.size() - m_BladePanel.size()<0) return;

    for(int p = m_WakePanel.size() - m_BladePanel.size();p < m_WakePanel.size();p++){

    if (p >= (m_WakePanel.size()-m_BladePanel.size())){
        if (m_WakePanel[p]->isHub){
        m_WakePanel[p]->m_Gamma_Node_LA = m_WakePanel[p]->m_GammaTALA*2/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_LA = (m_WakePanel[p-1]->m_GammaLBTB*2)/4;
    }
    else{
        if (m_WakePanel[p]->isHub){
        m_WakePanel[p]->m_Gamma_Node_LA = (m_WakePanel[p]->m_GammaTALA+m_WakePanel[p+m_BladePanel.size()]->m_GammaTALA+m_WakePanel[p+m_BladePanel.size()]->m_GammaTBTA)/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_LA = (m_WakePanel[p-1]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()]->m_GammaTBTA+m_WakePanel[p+m_BladePanel.size()-1]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()-1]->m_GammaTBTA)/4;
    }

    if (p >= (m_WakePanel.size()-m_BladePanel.size())){
        if (m_WakePanel[p]->isTip){
        m_WakePanel[p]->m_Gamma_Node_LB = m_WakePanel[p]->m_GammaLBTB*2/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_LB = (m_WakePanel[p]->m_GammaLBTB*2)/4;
    }
    else{
        if (m_WakePanel[p]->isTip){
        m_WakePanel[p]->m_Gamma_Node_LB = (m_WakePanel[p]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()]->m_GammaTBTA)/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_LB = (m_WakePanel[p]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()]->m_GammaLBTB+m_WakePanel[p+m_BladePanel.size()]->m_GammaTBTA+m_WakePanel[p+m_BladePanel.size()+1]->m_GammaTBTA)/4;
    }
    if (p < (m_BladePanel.size())){
        if (m_WakePanel[p]->isTip){
        m_WakePanel[p]->m_Gamma_Node_TB = (m_WakePanel[p]->m_GammaLBTB+m_WakePanel[p]->m_GammaTBTA)/2;
        }
        else m_WakePanel[p]->m_Gamma_Node_TB = (m_WakePanel[p]->m_GammaLBTB+m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p+1]->m_GammaTBTA)/3;
    }
    else{
        if (m_WakePanel[p]->isTip){
        m_WakePanel[p]->m_Gamma_Node_TB = (m_WakePanel[p]->m_GammaLBTB+m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p-m_BladePanel.size()]->m_GammaLBTB)/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_TB = (m_WakePanel[p]->m_GammaLBTB-m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p+1]->m_GammaTBTA+m_WakePanel[p-m_BladePanel.size()]->m_GammaLBTB)/4;
    }

    if (p < m_BladePanel.size()){
        if (m_WakePanel[p]->isHub){
        m_WakePanel[p]->m_Gamma_Node_TA = (m_WakePanel[p]->m_GammaTALA+m_WakePanel[p]->m_GammaTBTA)/2;
        }
        else m_WakePanel[p]->m_Gamma_Node_TA = (m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p-1]->m_GammaLBTB+m_WakePanel[p-1]->m_GammaTBTA)/3;
    }
    else{
        if (m_WakePanel[p]->isHub){
        m_WakePanel[p]->m_Gamma_Node_TA = (m_WakePanel[p]->m_GammaTALA+m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p-m_BladePanel.size()]->m_GammaTALA)/3;
        }
        else m_WakePanel[p]->m_Gamma_Node_TA = (m_WakePanel[p]->m_GammaTBTA+m_WakePanel[p-1]->m_GammaLBTB-m_WakePanel[p-1]->m_GammaTBTA+m_WakePanel[p-m_BladePanel.size()-1]->m_GammaLBTB)/4;

    }


    }
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

void QLLTSimulation::AddFirstWakeRow()
{
    VortexNode *LA, *LB, *TA,*TB;//wake panel's corner points
    VortexNode *n0, *n1,*n2, *n3; // wake nodes LA,LB,TA,TB
    VortexNode LATB, TALB;
    QVector<VortexNode*> leadingNodes, trailingNodes; //nodes are first stores here and then sorted into the node array (trailing first, then leading)

    for (int ID=0;ID<m_BladePanel.size();ID++)
    {


    TA = new VortexNode;
    TB = new VortexNode;
    LA = new VortexNode;
    LB = new VortexNode;

    *TA = *m_BladePanel[ID]->pTA;
    *TB = *m_BladePanel[ID]->pTB;

    if(m_bisVawt){
    TA->Rotate(VRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    TB->Rotate(VRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    }
    if(!m_bisVawt){
    TA->Rotate(HRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    TB->Rotate(HRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    }


    CPanel *panel = new CPanel;

    if (ID % (m_BladePanel.size()/num_blades) == 0) panel->isHub = true;
    else if ((ID+1) % (m_BladePanel.size()/num_blades) == 0) panel->isTip = true;

        *LA = *TA;
        *LB = *TB;

        *TA += V_in*m_firstWakeRowLength*dT;
        *TB += V_in*m_firstWakeRowLength*dT;

        LATB = *TB - *LA;
        TALB = *LB - *TA;

        panel->Normal = LATB * TALB;
        panel->m_bIsWakePanel = true;
        panel->Area =  panel->Normal.VAbs()/2.0;
        panel->Normal.Normalize();
        panel->SetFrame(*LA,*LB, *TA, *TB);
        panel->m_iSym = -1;
        panel->m_bIsLeftPanel  = false;
        panel->m_Gamma=m_BladePanel[ID]->m_Gamma;

        // LA,LB,TA,TB
        n0 = IsWakeNode(*LA);
        n1 = IsWakeNode(*TA);
        n2 = IsWakeNode(*LB);
        n3 = IsWakeNode(*TB);

        if(n0) {
            panel->pLA = n0; // LA Wake Panel Node hat jetzt ID von TA Wing Panel Node
            delete LA;
        }
        else {
            panel->pLA = LA;
            leadingNodes.push_back(LA);
            m_WakeNode.push_back(LA);
        }

        if(n1) {
            panel->pTA = n1;
            delete TA;
        }
        else {
            panel->pTA = TA;
            trailingNodes.push_back(TA);
            m_WakeNode.push_back(TA);

        }

        if(n2) {
            panel->pLB = n2;
            delete LB;
        }
        else {
            panel->pLB = LB;
            leadingNodes.push_back(LB);
            m_WakeNode.push_back(LB);

        }

        if(n3) {
            panel->pTB = n3;
            delete TB;
        }
        else {
            panel->pTB = TB;
            trailingNodes.push_back(TB);
            m_WakeNode.push_back(TB);

        }

        CVector WLA,WLB,WTA,WTB;
        WLA.Copy(*panel->pLA);
        WLB.Copy(*panel->pLB);
        WTA.Copy(*panel->pTA);
        WTB.Copy(*panel->pTB);
        panel->SetFrame(WLA, WLB, WTA, WTB);
        m_WakePanel.push_back(panel);

    }

    m_WakeNode.clear();


    for (int i=0;i<trailingNodes.size();i++){
        m_WakeNode.push_back(trailingNodes.at(i));
    }

    for (int i=0;i<leadingNodes.size();i++){
        m_WakeNode.push_back(leadingNodes.at(i));
    }



}

void QLLTSimulation::AddFirstWakeLines()
{

    VortexNode *L, *T;
    QVector <VortexNode*> leadingNodes, trailingNodes;

    for (int i =0;i<m_BladeLine.size();i++){

    // create new nodes
    T = new VortexNode;
    L = new VortexNode;
    *T = *m_BladeLine[i]->pT;
    *L = *T;
    if (m_bisVawt) T->Rotate(VRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    if (!m_bisVawt) T->Rotate(HRotorAxis,m_Omega*dT*(m_firstWakeRowLength)*180/PI);
    *T += V_in*dT*m_firstWakeRowLength;

    T->fromTimestep = -1;
    T->fromStation = m_BladeLine[i]->fromStation;
    trailingNodes.push_back(T);
    L->fromTimestep = 0;
    L->fromStation = m_BladeLine[i]->fromStation;
    leadingNodes.push_back(L);
    }

    if (m_bShed){
        for (int i=0;i<trailingNodes.size();i++){

            if (((i+1) % (m_BladeLine.size()/num_blades) != 0)){
                VortexLine *line = new VortexLine;
                line->fromStation = m_BladeLine[i]->fromStation;
                line->isHub = false;
                line->isTip = false;
                line->fromTimestep = m_ActualTimeStep;
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
                line->Update(dT);

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
            line->fromStation = m_BladeLine[i]->fromStation;
            line->isHub = m_BladeLine[i]->isHub;
            line->isTip = m_BladeLine[i]->isTip;
            line->fromTimestep = m_ActualTimeStep;
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
            line->Update(dT);

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


void QLLTSimulation::GuessInitialVortexDistribution()
{
    //Anfangsverteilung von Gamma annehmen
    //Wing
    for (int i=0;i<m_BladePanel.size();i++)
    {
        m_BladePanel[i]->m_Gamma = 0;

        m_BladePanel[i]->m_Gamma_old = m_BladePanel[i]->m_Gamma;

    }
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

    for(int i = 0; i< m_BladePanel.size();i++){
        m_BladePanel[i]->m_Gamma_lastTimestep = m_BladePanel[i]->m_Gamma;
    }

    while ((m_bAllConverged == false) && (iterations < m_MaxIterations)){

        //1)
        CalcBladePanelVelocities();

        //2)
        CalcAoA();

        //3)
        ReadPanelLiftCoef();

        //4)
        CalcNewGamma();

        //5)
        GammaConvergenceCheck();

    iterations++;
    }

        AssignGammaToWingLines();

//    //test of correction at blade tip//

//    for (int j=0;j<m_BladePanel.size()/num_blades;j++){
//    int M = 11;
//    double dGamma = 0;
//    CVector MidL = CVector(m_BladeNode[m_BladePanel[j].m_iLA] + m_BladeNode[m_BladePanel[j].m_iLB])/2;
//    CVector MidT = CVector(m_BladeNode[m_BladePanel[j].m_iTA] + m_BladeNode[m_BladePanel[j].m_iTB])/2;
//    CVector chordline = MidL-MidT;
//    double chord = chordline.VAbs();
//    CVector MidLeft = CVector(m_BladeNode[m_BladePanel[j].m_iLA]);
//    CVector MidRight = CVector(m_BladeNode[m_BladePanel[j].m_iLB]);
//    double width = CVector(MidLeft-MidRight).VAbs();
//    CVector EvalPt;
//    CVector VQuarterChord = BiotSavart_InductEval(m_BladePanel[j].CtrlPt,false,0,true)*m_BladePanel[j].a3;
//    double dTheta = PI/(M-1);

//    for (int k=1; k <= M; k++){
//        EvalPt = MidT-m_BladePanel[j].a1*chord/2*(1-cos(PI*(k-1)/(M-1)));
//        dGamma += 2 * dTheta / m_BladePanel[j].m_V_total.VAbs() / cos(m_BladePanel[j].m_AoA/180*PI) * (CVector(BiotSavart_InductEval(EvalPt,false,0,true)*m_BladePanel[j].a3).VAbs()-VQuarterChord.VAbs()) * (cos(PI*(k-1)/(M-1))-1);
//        qDebug() << (CVector(EvalPt-MidT).VAbs())<<CVector(BiotSavart_InductEval(EvalPt,false,0,true)*m_BladePanel[j].a3).VAbs()<<VQuarterChord.VAbs()<<CVector(BiotSavart_InductEval(EvalPt,false,0,true)*m_BladePanel[j].a3).VAbs();

//    }

//    qDebug() << "Gamma Correction at Station: " << m_Blade->getFASTRadiusAt(j)<< " is:" << dGamma << dTheta << chord << width;

//    }

//    //////////////////////////////////

    qDebug() << "TIMESTEP"<< m_ActualTimeStep<<"iterations" << iterations;

    for(int i = 0; i< m_BladePanel.size();i++){

        int pos = i % (m_BladePanel.size()/num_blades);


        qDebug() <<"PANEL:"<<i<< "radial position"<<m_pBladeList[0]->getFASTRadiusAt(pos)<<"AoA"<<m_BladePanel[i]->m_AoA<<"m_BladePanel[i]->m_Gamma"<<m_BladePanel[i]->m_Gamma;
}
    qDebug()<<"--------------------------------------------------";

    iterations =0; //reset iterations

}

void QLLTSimulation::AssignGammaToWingLines(){

    int pos = 0;
    for(int i = 0; i < m_BladePanel.size();i++){
        if (m_BladePanel[i]->isHub && m_BladePanel[i]->isTip){
            m_BladeLine[pos]->Gamma = -m_BladePanel[i]->m_Gamma;
            pos++;
            m_BladeLine[pos]->Gamma = m_BladePanel[i]->m_Gamma;
            pos++;
        }
        else if (m_BladePanel[i]->isHub){

            m_BladeLine[pos]->Gamma = -m_BladePanel[i]->m_Gamma;
            pos++;

            m_BladeLine[pos]->Gamma = m_BladePanel[i]->m_Gamma - m_BladePanel[i+1]->m_Gamma;
            pos++;
        }
        else if (m_BladePanel[i]->isTip){
            m_BladeLine[pos]->Gamma = m_BladePanel[i]->m_Gamma;
            pos++;
        }
        else{
            m_BladeLine[pos]->Gamma = m_BladePanel[i]->m_Gamma - m_BladePanel[i+1]->m_Gamma;
            pos++;
        }
    }

//    for (int i=0;i<m_BladeLine.size();i++){
//        qDebug() << "BLADELINEGAMMA:::::"<<m_BladeLine[i]->Gamma << i << m_BladeLine[i]->isHub << m_BladeLine[i]->isTip;
//    }

}


void QLLTSimulation::GammaConvergenceCheck(){
    //compares Gamma_old and Gamma at all stations
    //if somewhere the convergence condition is violated set m_bAllConverged = false

    m_bAllConverged = true;

    for(int i = 0; i< m_BladePanel.size();i++){
        m_BladePanel[i]->m_Gamma_new = m_BladePanel[i]->m_Gamma_old + m_RelaxFactor*(m_BladePanel[i]->m_Gamma - m_BladePanel[i]->m_Gamma_old);


        m_BladePanel[i]->m_DeltaGamma = (m_BladePanel[i]->m_Gamma - m_BladePanel[i]->m_Gamma_old);

        if(m_BladePanel[i]->m_DeltaGamma > m_Epsilon){
            m_bAllConverged = false;
        }
                m_BladePanel[i]->m_Gamma_old = m_BladePanel[i]->m_Gamma_new; // assign new gamma_old if not converged at panel
                m_BladePanel[i]->m_Gamma = m_BladePanel[i]->m_Gamma_new; //assign new gamma if not converged on panel //ist bei actualtimestep==0 && iterations==0  Gamma_old
    }
}



void QLLTSimulation::CalcNewGamma()
{
    // calculate with equation (van Garrel S. 11 (18)) Gamma _cal
    for (int i=0; i<m_BladePanel.size();i++){

        double Vcp_a1_skal = m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1);
        double Vcp_a3_skal = m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a3);

        CVector LALB = m_BladePanel[i]->VortPtB - m_BladePanel[i]->VortPtA;

        double Vcp_dl_cross_a1_skal = CVector(m_BladePanel[i]->m_V_total*LALB).dot(m_BladePanel[i]->a1);
        double Vcp_dl_cross_a3_skal = CVector(m_BladePanel[i]->m_V_total*LALB).dot(m_BladePanel[i]->a3);

        //Calculate new gamma
        m_BladePanel[i]->m_Gamma = m_BladePanel[i]->m_CL*0.5*m_BladePanel[i]->GetArea()*(pow(Vcp_a1_skal,2)+pow(Vcp_a3_skal,2)) / sqrt(pow(Vcp_dl_cross_a1_skal,2)+pow(Vcp_dl_cross_a3_skal,2));
    }
}

void QLLTSimulation::ReadPanelLiftCoef()
{
    int IDP;
    int actual_blade = 1;
    int one_blade_size = m_BladePanel.size()/num_blades;

    C360Polar *m_pPolar1;

    for(int i=0;i<m_BladePanel.size();i++){

        if((i>0)&&(i%one_blade_size == 0)) actual_blade +=1;
        IDP = i - (actual_blade-1)*m_BladePanel.size()/num_blades;

        m_pPolar1 = m_pBladeList[0]->get360PolarAt(IDP);

//        qDebug() << "PANEL" << i << "polar1" << m_Blade->get360PolarAt(IDP)->getName() << "polar2" << m_Blade->get360PolarAt(IDP+1)->getName();
        for(int j=1;j<m_pPolar1->m_Alpha.size();j++){
            if(m_pPolar1->m_Alpha[j-1] == m_BladePanel[i]->m_AoA){
                m_BladePanel[i]->m_CL = 0.5*m_pPolar1->m_Cl[j];
                m_BladePanel[i]->m_CD = 0.5*m_pPolar1->m_Cd[j];
                break;
            }
            if((m_pPolar1->m_Alpha[j-1] < m_BladePanel[i]->m_AoA) && (m_pPolar1->m_Alpha[j] > m_BladePanel[i]->m_AoA)){

                m_BladePanel[i]->m_CL = 0.5*(m_pPolar1->m_Cl[j-1]+(m_pPolar1->m_Cl[j]-m_pPolar1->m_Cl[j-1])*(m_BladePanel[i]->m_AoA-m_pPolar1->m_Alpha[j-1])/(m_pPolar1->m_Alpha[j]-m_pPolar1->m_Alpha[j-1]));
                m_BladePanel[i]->m_CD = 0.5*(m_pPolar1->m_Cd[j-1]+(m_pPolar1->m_Cd[j]-m_pPolar1->m_Cd[j-1])*(m_BladePanel[i]->m_AoA-m_pPolar1->m_Alpha[j-1])/(m_pPolar1->m_Alpha[j]-m_pPolar1->m_Alpha[j-1]));

                break;
            }
        }
    }

    actual_blade = 1;


    for(int i=0;i<m_BladePanel.size();i++){

        if((i>0)&&(i%one_blade_size == 0)) actual_blade +=1;
        IDP = i - (actual_blade-1)*m_BladePanel.size()/num_blades;

        m_pPolar1 = m_pBladeList[0]->get360PolarAt(IDP+1);

        for(int j=1;j<m_pPolar1->m_Alpha.size();j++){
            if(m_pPolar1->m_Alpha[j-1] == m_BladePanel[i]->m_AoA){
                m_BladePanel[i]->m_CL += 0.5*m_pPolar1->m_Cl[j];
                m_BladePanel[i]->m_CD += 0.5*m_pPolar1->m_Cd[j];
                break;
            }
            if((m_pPolar1->m_Alpha[j-1] < m_BladePanel[i]->m_AoA) && (m_pPolar1->m_Alpha[j] > m_BladePanel[i]->m_AoA)){

                m_BladePanel[i]->m_CL += 0.5*(m_pPolar1->m_Cl[j-1]+(m_pPolar1->m_Cl[j]-m_pPolar1->m_Cl[j-1])*(m_BladePanel[i]->m_AoA-m_pPolar1->m_Alpha[j-1])/(m_pPolar1->m_Alpha[j]-m_pPolar1->m_Alpha[j-1]));
                m_BladePanel[i]->m_CD += 0.5*(m_pPolar1->m_Cd[j-1]+(m_pPolar1->m_Cd[j]-m_pPolar1->m_Cd[j-1])*(m_BladePanel[i]->m_AoA-m_pPolar1->m_Alpha[j-1])/(m_pPolar1->m_Alpha[j]-m_pPolar1->m_Alpha[j-1]));
                break;
            }
        }
    }


}

void QLLTSimulation::CalcAoA()
{
//berechnet Anstellwinkel für wing-panele
//Input: Wing: m_V_total
//Output: Wing: m_AoA

    for (int i=0; i<m_BladePanel.size();i++){
        m_BladePanel[i]->m_AoA = atan((m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a3))/(m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1)))*180/PI;
        if (m_BladePanel[i]->m_V_total.dot(m_BladePanel[i]->a1) < 0 ) m_BladePanel[i]->m_AoA += 180;
    }
}


void QLLTSimulation::CalcBladePanelVelocities(){
//updates induction in wing and wake Panels of all panels in simulation in a given poin on panel
//Input: Gamma_alt, V_in
//Output: Wing-Panel: m_V_induced, m_V_total
//        Wake-Panel: m_V_induced

//////
//wing update : m_V_induced, m_V_total -> für neue m_alpha -> m_CL -> m_Gamma berechnung
//////



    for(int ID=0;ID<m_BladePanel.size();ID++){
            m_BladePanel[ID]->m_V_induced.Set(0,0,0);
            m_BladePanel[ID]->m_V_total.Set(0,0,0);

        //calcs m_V_induced for every WingPanel in WingPanel Control Point
            m_BladePanel[ID]->m_V_induced = BiotSavart_InductEval(m_BladePanel[ID]->CtrlPt, false, true, ID);
            if (iterations > 0) m_BladePanel[ID]->m_V_induced += m_StoreWakeInfluence[ID];

        //m_V_total for new Gamma calc

        m_BladePanel[ID]->m_V_total =  TowerInfluence(m_BladePanel[ID]->CtrlPt,GetFreeStream(m_BladePanel[ID]->CtrlPt)+m_BladePanel[ID]->m_V_induced)-(m_BladePanel[ID]->CtrlPt-m_BladePanel[ID]->Old_CtrlPt)/dT;

    }

}

CVector QLLTSimulation::BiotSavart_InductEval(CVector EvalPt, bool prescribed, bool indWing, int tempID)
{
//berechnet m_V_induced in beliebigen Übergabe-Punkt
//Input: EvalPt
//Output: m_VGamma_total

    CVector m_VGamma_total;

    VortexNode R1; //EvalPt-P1
    VortexNode R2; //EvalPt-P2
    VortexNode R3; //EvalPt-P3
    VortexNode R4; //EvalPt-P4

    m_VGamma_total.Set(0,0,0); // total induced Velocity of all Panels in Simulation

/////////////
//Wing Panels
/////////////


    for(int ID=0;ID<m_BladePanel.size();ID++){

        //calc vector P1234 -> EvalPt on WingPanels
        R1 = EvalPt - m_BladePanel[ID]->VortPtA; //EvalPt-P1
        R2 = EvalPt - m_BladePanel[ID]->VortPtB; //EvalPt-P2
        R3 = EvalPt - *m_BladePanel[ID]->pTB; //EvalPt-P3
        R4 = EvalPt - *m_BladePanel[ID]->pTA; //EvalPt-P4

        /////////////////////////////////////
        //calc induction of wing Panels
        /////////////////////////////////////

        //Induction Vortexelement 1-2 on WingPanels
        double R1R2 = R1.VAbs()*R2.VAbs();
        double R1R2_skal = R1.dot(R2);
        double R1_plus_R2 = R1.VAbs() + R2.VAbs();
        double correction1 = pow((m_CutOffRadius*m_BladePanel[ID]->LALB),2);
        CVector R1R2_cross = R1*R2;
        if (R1R2+R1R2_skal!=0) m_VGamma_total += R1R2_cross*m_BladePanel[ID]->m_Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction1) ; //+ m_global_BiotS_correction); //

        //Induction Vortexelement 2-3 on WingPanels
        double R2R3 = R2.VAbs()*R3.VAbs();
        double R2R3_skal = R2.dot(R3);
        double R2_plus_R3 = R2.VAbs() + R3.VAbs();
        double correction2 = pow((m_CutOffRadius*m_BladePanel[ID]->LBTB),2);
        CVector R2R3_cross = R2*R3;
        if (R2R3+R2R3_skal!=0) m_VGamma_total += R2R3_cross*m_BladePanel[ID]->m_Gamma*R2_plus_R3/(4*PI*R2R3*(R2R3+R2R3_skal)+correction2) ; //+ m_global_BiotS_correction); //

        if (m_bShed){
        if (m_WakePanel.size() == 0 && m_WakeLine.size() == 0){
        //Induction Vortexelement 3-4 on WingPanels
        double R3R4 = R3.VAbs()*R4.VAbs();
        double R3R4_skal = R3.dot(R4);
        double R3_plus_R4 = R3.VAbs() + R4.VAbs();
        double correction3 = pow((m_CutOffRadius*m_BladePanel[ID]->TBTA),2);
        CVector R3R4_cross = R3*R4;
        if (R3R4+R3R4_skal!=0) m_VGamma_total += R3R4_cross*m_BladePanel[ID]->m_Gamma*R3_plus_R4/(4*PI*R3R4*(R3R4+R3R4_skal)+correction3) ; //+ m_global_BiotS_correction); //
        }
        }

        //Induction Vortexelement 4-1 on WingPanels
        double R4R1 = R4.VAbs()*R1.VAbs();
        double R4R1_skal = R4.dot(R1);
        double R4_plus_R1 = R4.VAbs() + R1.VAbs();
        double correction4 = pow((m_CutOffRadius*m_BladePanel[ID]->TALA),2);
        CVector R4R1_cross = R4*R1;
        if (R4R1+R4R1_skal!=0) m_VGamma_total += R4R1_cross*m_BladePanel[ID]->m_Gamma*R4_plus_R1/(4*PI*R4R1*(R4R1+R4R1_skal)+correction4) ; //+ m_global_BiotS_correction); //

    }

    if (m_bGround){
    for(int ID=0;ID<m_BladePanel.size();ID++){

        //calc vector P1234 -> EvalPt on WingPanels
        R1 = EvalPt - (m_BladePanel[ID]->VortPtA - CVector(0,0,m_BladePanel[ID]->VortPtA.z+m_HubHeight)*2); //EvalPt-P1
        R2 = EvalPt - (m_BladePanel[ID]->VortPtB - CVector(0,0,m_BladePanel[ID]->VortPtB.z+m_HubHeight)*2); //EvalPt-P2
        R3 = EvalPt - (*m_BladePanel[ID]->pTB - CVector(0,0,m_BladePanel[ID]->pTB->z+m_HubHeight)*2); //EvalPt-P3
        R4 = EvalPt - (*m_BladePanel[ID]->pTA - CVector(0,0,m_BladePanel[ID]->pTA->z+m_HubHeight)*2); //EvalPt-P4

        /////////////////////////////////////
        //calc induction of wing Panels
        /////////////////////////////////////

        //Induction Vortexelement 1-2 on WingPanels
        double R1R2 = R1.VAbs()*R2.VAbs();
        double R1R2_skal = R1.dot(R2);
        double R1_plus_R2 = R1.VAbs() + R2.VAbs();
        double correction1 = pow((m_CutOffRadius*m_BladePanel[ID]->LALB),2);
        CVector R1R2_cross = R1*R2;
        if (R1R2+R1R2_skal!=0) m_VGamma_total += R1R2_cross*(-m_BladePanel[ID]->m_Gamma)*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction1) ; //+ m_global_BiotS_correction); //

        //Induction Vortexelement 2-3 on WingPanels
        double R2R3 = R2.VAbs()*R3.VAbs();
        double R2R3_skal = R2.dot(R3);
        double R2_plus_R3 = R2.VAbs() + R3.VAbs();
        double correction2 = pow((m_CutOffRadius*m_BladePanel[ID]->LBTB),2);
        CVector R2R3_cross = R2*R3;
        if (R2R3+R2R3_skal!=0) m_VGamma_total += R2R3_cross*(-m_BladePanel[ID]->m_Gamma)*R2_plus_R3/(4*PI*R2R3*(R2R3+R2R3_skal)+correction2) ; //+ m_global_BiotS_correction); //

        if (m_bShed){
        if (m_WakePanel.size() == 0 && m_WakeLine.size() == 0){
        //Induction Vortexelement 3-4 on WingPanels
        double R3R4 = R3.VAbs()*R4.VAbs();
        double R3R4_skal = R3.dot(R4);
        double R3_plus_R4 = R3.VAbs() + R4.VAbs();
        double correction3 = pow((m_CutOffRadius*m_BladePanel[ID]->TBTA),2);
        CVector R3R4_cross = R3*R4;
        if (R3R4+R3R4_skal!=0) m_VGamma_total += R3R4_cross*(-m_BladePanel[ID]->m_Gamma)*R3_plus_R4/(4*PI*R3R4*(R3R4+R3R4_skal)+correction3) ; //+ m_global_BiotS_correction); //
        }
        }

        //Induction Vortexelement 4-1 on WingPanels
        double R4R1 = R4.VAbs()*R1.VAbs();
        double R4R1_skal = R4.dot(R1);
        double R4_plus_R1 = R4.VAbs() + R1.VAbs();
        double correction4 = pow((m_CutOffRadius*m_BladePanel[ID]->TALA),2);
        CVector R4R1_cross = R4*R1;
        if (R4R1+R4R1_skal!=0) m_VGamma_total += R4R1_cross*(-m_BladePanel[ID]->m_Gamma)*R4_plus_R1/(4*PI*R4R1*(R4R1+R4R1_skal)+correction4) ; //+ m_global_BiotS_correction); //

    }
    }

/////////////
//Wake Panels
/////////////

    // if wake induction is computed for the wing panels this happens only during the first iteration, the induction is then stored in the m_StoreWakeInfluence array for the timestep
    if (indWing && iterations > 0) return m_VGamma_total;


    if (indWing){
        m_StoreWakeInfluence[tempID].Set(0,0,0);
    }

if (!m_bWakeLine){

    for(int ID=0;ID<m_WakePanel.size();ID++){

//        double K = 10;

        //calc vector P1/P2/P3/P4 -> EvalPt on WakePanels
        R1 = EvalPt - *m_WakePanel[ID]->pLA; //EvalPt-P1
        R2 = EvalPt - *m_WakePanel[ID]->pLB; //EvalPt-P2
        R3 = EvalPt - *m_WakePanel[ID]->pTB; //EvalPt-P3
        R4 = EvalPt - *m_WakePanel[ID]->pTA; //EvalPt-P4

        /////////////////////////////////////
        //calc induction of wake Panels
        /////////////////////////////////////

        //Induction Vortexelement 2-3 on WakePanels
        double R2R3 = R2.VAbs()*R3.VAbs();
        double R2R3_skal = R2.dot(R3);
        double R2_plus_R3 = R2.VAbs() + R3.VAbs();
        double correction2 = pow((m_CutOffRadius*m_WakePanel[ID]->LBTB),2);
        CVector R2R3_cross = R2*R3;
        m_VGamma_total += R2R3_cross*m_WakePanel[ID]->m_GammaLBTB*R2_plus_R3/(4*PI*R2R3*(R2R3+R2R3_skal)+correction2) ; //
        if (indWing) m_StoreWakeInfluence[tempID] += R2R3_cross*m_WakePanel[ID]->m_GammaLBTB*R2_plus_R3/(4*PI*R2R3*(R2R3+R2R3_skal)+correction2) ;


        if (m_bShed){
        //Induction Vortexelement 3-4 on WakePanels
        double R3R4 = R3.VAbs()*R4.VAbs();
        double R3R4_skal = R3.dot(R4);
        double R3_plus_R4 = R3.VAbs() + R4.VAbs();
        double correction3 = pow((m_CutOffRadius*m_WakePanel[ID]->TBTA),2);
        CVector R3R4_cross = R3*R4;
        m_VGamma_total += R3R4_cross*m_WakePanel[ID]->m_GammaTBTA*R3_plus_R4/(4*PI*R3R4*(R3R4+R3R4_skal)+correction3) ; //
        if (indWing) m_StoreWakeInfluence[tempID] += R3R4_cross*m_WakePanel[ID]->m_GammaTBTA*R3_plus_R4/(4*PI*R3R4*(R3R4+R3R4_skal)+correction3) ;
        }

        //Induction Vortexelement 4-1 on WakePanels
        if (m_WakePanel[ID]->isHub){
        double R4R1 = R4.VAbs()*R1.VAbs();
        double R4R1_skal = R4.dot(R1);
        double R4_plus_R1 = R4.VAbs() + R1.VAbs();
        double correction4 = pow((m_CutOffRadius*m_WakePanel[ID]->TALA),2);
        CVector R4R1_cross = R4*R1;
        m_VGamma_total += R4R1_cross*m_WakePanel[ID]->m_Gamma*R4_plus_R1/(4*PI*R4R1*(R4R1+R4R1_skal)+correction4) ; //
        if (indWing) m_StoreWakeInfluence[tempID] +=  R4R1_cross*m_WakePanel[ID]->m_Gamma*R4_plus_R1/(4*PI*R4R1*(R4R1+R4R1_skal)+correction4) ;
        }

    }//end for m_WakePanel.size()

}
else{

    CVector gamma_cont;
        for(int ID=0;ID<m_WakeLine.size();ID++){

        //calc vector P1/P2/P3/P4 -> EvalPt on WakePanels
        R1 = EvalPt - *m_WakeLine[ID]->pL; //EvalPt-P1
        R2 = EvalPt - *m_WakeLine[ID]->pT; //EvalPt-P2

        /////////////////////////////////////
        //calc induction of wake Lines
        /////////////////////////////////////

        //Induction Vortexelement 2-3 on WakePanels
        double R1R2 = R1.VAbs()*R2.VAbs();
        double R1R2_skal = R1.dot(R2);
        double R1_plus_R2 = R1.VAbs() + R2.VAbs();
    //    double correction2 = pow((m_CutOffRadius*m_WakeLine[ID]->GetLength()),2);
        double correction2 = pow((m_WakeLine[ID]->coreSize),2);

        CVector R1R2_cross = R1*R2;

        //computation of biot savart correction Kv//
    //    double Kv, h;
    //    h = R1R2_cross.VAbs()/m_WakeLine[ID]->GetLength();
    //    Kv = pow(h,nfactor)/pow((pow(m_WakeLine[ID]->coreSize,2*nfactor)+pow(h,2*nfactor)),1/nfactor);
    //    qDebug() << "KV" <<R1R2_cross.VAbs()<<R1R2<<R1R2_skal<<R1_plus_R2<< Kv << h << m_WakeLine[ID]->coreSize << pow((pow(m_WakeLine[ID]->coreSize,2*nfactor)+pow(h,2*nfactor)),1/nfactor) << m_WakeLine[ID]->GetLength();

    //    m_VGamma_total += R1R2_cross*m_WakeLine[ID]->Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction2) ; //
        gamma_cont =  R1R2_cross/**Kv*/*m_WakeLine[ID]->Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction2/*correction2*/);
        m_VGamma_total += gamma_cont;
        if (indWing) m_StoreWakeInfluence[tempID] += gamma_cont ;

        }

        if (m_bGround){
            for(int ID=0;ID<m_WakeLine.size();ID++){

            //calc vector P1/P2/P3/P4 -> EvalPt on WakePanels
            R1 = EvalPt - (*m_WakeLine[ID]->pL - CVector(0,0,m_WakeLine[ID]->pL->z+m_HubHeight)*2); //EvalPt-P1
            R2 = EvalPt - (*m_WakeLine[ID]->pT - CVector(0,0,m_WakeLine[ID]->pT->z+m_HubHeight)*2); //EvalPt-P2

            /////////////////////////////////////
            //calc induction of wake Lines
            /////////////////////////////////////

            //Induction Vortexelement 2-3 on WakePanels
            double R1R2 = R1.VAbs()*R2.VAbs();
            double R1R2_skal = R1.dot(R2);
            double R1_plus_R2 = R1.VAbs() + R2.VAbs();
        //    double correction2 = pow((m_CutOffRadius*m_WakeLine[ID]->GetLength()),2);
            double correction2 = pow((m_WakeLine[ID]->coreSize),2);

            CVector R1R2_cross = R1*R2;

            //computation of biot savart correction Kv//
        //    double Kv, h;
        //    h = R1R2_cross.VAbs()/m_WakeLine[ID]->GetLength();
        //    Kv = pow(h,nfactor)/pow((pow(m_WakeLine[ID]->coreSize,2*nfactor)+pow(h,2*nfactor)),1/nfactor);
        //    qDebug() << "KV" <<R1R2_cross.VAbs()<<R1R2<<R1R2_skal<<R1_plus_R2<< Kv << h << m_WakeLine[ID]->coreSize << pow((pow(m_WakeLine[ID]->coreSize,2*nfactor)+pow(h,2*nfactor)),1/nfactor) << m_WakeLine[ID]->GetLength();

        //    m_VGamma_total += R1R2_cross*m_WakeLine[ID]->Gamma*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction2) ; //
            gamma_cont =  R1R2_cross/**Kv*/*(-m_WakeLine[ID]->Gamma)*R1_plus_R2/(4*PI*R1R2*(R1R2+R1R2_skal)+correction2/*correction2*/);
            m_VGamma_total += gamma_cont;
            if (indWing) m_StoreWakeInfluence[tempID] += gamma_cont ;

            }
        }

}

if (prescribed){
CVector WindDir;
WindDir.Set(1,0,0);
WindDir.RotZ(m_InflowAngle/180.0*PI);
double mag = WindDir.dot(m_VGamma_total);
m_VGamma_total = WindDir*mag;
}

return m_VGamma_total; //total induced Velocity
}




void QLLTSimulation::createHAWTTurbineBlades(CBlade *blade){
    qDebug() << "create turbine";

    m_bGlChanged = true;

        CVector O(0,0,0);

        if (!blade) return;

        for (int i=0;i<num_blades;i++)
        {
            m_pBladeList[i] = new CBlade;

            m_pBladeList[i]->Duplicate(blade);

            m_pBladeList[i]->CreateSurfaces();


            for(int j=0;j<m_pBladeList[i]->m_NPanel;j++)
            {
            m_pBladeList[i]->m_Surface[j].SetSidePoints(NULL,0.0,0.0);
            m_pBladeList[i]->m_Surface[j].RotateZ(O, 360/num_blades*i-90); // Panels werden je nach anzahl gleichmäßig um z-Achse angeordnet
            m_pBladeList[i]->m_Surface[j].RotateY(O, 90); // Panel um 90° um y-Achse rotieren
            m_pBladeList[i]->m_Surface[j].RotateY(O, m_TeetherAngle); // Panel um Teether Angle um y-Achse rotieren
            m_pBladeList[i]->m_Surface[j].RotateZ(O, m_YawAngle); // Panel Yaw Angle um z-Achse rotieren
            }

            for (int k=0; k<m_pBladeList[i]->m_NSurfaces; k++)
            {
                    m_pBladeList[i]->m_Surface[k].SetSidePoints(NULL,0.0,0.0);
            }
        }
        InitializePanels();

        PitchBlades();

        Yaw_Old = m_YawAngle;
}

void QLLTSimulation::createVAWTTurbineBlades(CBlade *blade){
        qDebug() << "create turbine";

        m_bGlChanged = true;

        CVector O(0,0,0);

        CVector translation;

        translation.Set(0,-(blade->m_TPos[0]+(blade->m_TPos[blade->m_NPanel]-blade->m_TPos[0])/2),0);

        if (!blade) return;

        for (int i=0;i<num_blades;i++)
        {
            m_pBladeList[i] = new CBlade;

            m_pBladeList[i]->Duplicate(blade);

            m_pBladeList[i]->CreateSurfaces(true);

                for(int j=0;j<m_pBladeList[i]->m_NPanel;j++)
                {
                    m_pBladeList[i]->m_Surface[j].SetSidePoints(NULL,0.0,0.0);
                    m_pBladeList[i]->m_Surface[j].Translate(translation);
                    m_pBladeList[i]->m_Surface[j].RotateX(O, 90);
                    m_pBladeList[i]->m_Surface[j].RotateZ(O, 360.0/num_blades*i-90); // Panels werden je nach anzahl gleichmäßig um Rotor-Achse angeordnet
                    m_pBladeList[i]->m_Surface[j].RotateY(O, m_TeetherAngle);
                }

            for (int k=0; k<m_pBladeList[i]->m_NSurfaces; k++)
            {
            m_pBladeList[i]->m_Surface[k].SetSidePoints(NULL,0.0,0.0);
            }
        }

        InitializePanels();

        PitchBlades();

}

void QLLTSimulation::InitializePanels()
{
        int j;

        // all set to create the panels

        for(int iw=0; iw<num_blades; iw++)
        {
                if(m_pBladeList[iw])
                {
                        for(j=0; j<m_pBladeList[iw]->m_NSurfaces;j++)
                        {
//                                m_pBladeList[iw]->m_Surface[j].ResetFlap();
                                CreateElements(&m_pBladeList[iw]->m_Surface[j]);
                        }

                }
        }

        int blade = 0;
        for (int i=0;i<m_BladePanel.size();i++){

            m_BladePanel[i]->blade_number = blade;
            if (i % (m_BladePanel.size()/num_blades) == 0) m_BladePanel[i]->isHub = true;
            if ((i+1) % (m_BladePanel.size()/num_blades) == 0){
                m_BladePanel[i]->isTip = true;
                blade += 1;
            }
        }

        // initialize rotor lines for line vortices
        int pos = 0;
        for (int i=0;i<m_BladePanel.size();i++){
            if (m_BladePanel[i]->isHub){
                VortexLine *line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLA;
                line->pT = m_BladePanel[i]->pTA;
                line->isHub = true;
                line->fromStation = pos;
                pos++;

                line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLB;
                line->pT = m_BladePanel[i]->pTB;
                line->fromStation = pos;
                pos++;
            }
            else{
                VortexLine *line = new VortexLine;
                m_BladeLine.push_back(line);
                line->pL = m_BladePanel[i]->pLB;
                line->pT = m_BladePanel[i]->pTB;
                if (m_BladePanel[i]->isTip) line->isTip = true;
                line->fromStation = pos;
                pos++;
            }
        }

}

VortexNode *QLLTSimulation::IsNode(CVector &Pt)
{
        //
        // returns the index of a node if found, else returns NULL
        //
        int in;
        // explore in reverse order, since we have better chance of
        // finding the node close to the last point when creating elements
        //
        for (in=m_BladeNode.size()-1; in>=0; in--)
        {
                if(Pt.IsSame(*m_BladeNode[in])) return m_BladeNode[in];
        }
        return NULL;
}

void QLLTSimulation::CreateElements(CSurface *pSurface)
{
        // Creates the panel elements that will be used by the lifting line method
        // The panels are created from left to right on a surface
        VortexNode *n0, *n1, *n2, *n3;

        CPanel *panel;
        panel = new CPanel;

                        n0 = IsNode(pSurface->m_LA);
                        n1 = IsNode(pSurface->m_TA);
                        n2 = IsNode(pSurface->m_LB);
                        n3 = IsNode(pSurface->m_TB);


                        panel->m_bIsTrailing = true;
                        panel->m_bIsLeading  = true;
                        panel->m_bIsWakePanel   = false;

                        if(n0)
                        {
                                panel->pLA = n0;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = pSurface->m_LA;
                                panel->pLA = vec;
                                m_BladeNode.push_back(vec);
                        }

                        if(n1)
                        {
                                panel->pTA = n1;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = pSurface->m_TA;
                                panel->pTA = vec;
                                m_BladeNode.push_back(vec);
                        }

                        if(n2)
                        {
                                panel->pLB = n2;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = pSurface->m_LB;
                                panel->pLB = vec;
                                m_BladeNode.push_back(vec);
                        }

                        if(n3)
                        {
                                panel->pTB = n3;
                        }
                        else {
                                VortexNode *vec = new VortexNode;
                                *vec = pSurface->m_TB;
                                panel->pTB = vec;
                                m_BladeNode.push_back(vec);
                        }

                        panel->m_iElement = m_BladePanel.size();
                        panel->m_iSym  = -1;

                        panel->SetFrame(pSurface->m_LA, pSurface->m_LB, pSurface->m_TA, pSurface->m_TB);

                        m_BladePanel.push_back(panel);

}

void QLLTSimulation::serialize() {
    StorableObject::serialize();
    ShowAsGraphInterface::serialize();
}

void QLLTSimulation::restorePointers() {
    StorableObject::restorePointers();

}

void QLLTSimulation::FillVariableList(bool isVawt){

    if (!m_bisVawt){
    m_availableVariables.clear();
    m_availableVariables.append("Simulation Time [s]");
    m_availableVariables.append("Azimuthal Angle [deg]");
    m_availableVariables.append("Power Coefficient Cp");
    m_availableVariables.append("Angle of Attack");
    m_availableVariables.append("Tangential Force Coeffienct C_T");
    }

}

QLLTSimulation::QLLTSimulation()
    : StorableObject ("< no name >"),
      ShowAsGraphInterface (false)
{

}

NewCurve* QLLTSimulation::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType) {

    const int xAxisIndex = m_availableVariables.indexOf(xAxis);
    const int yAxisIndex = m_availableVariables.indexOf(yAxis);

    if (xAxisIndex == -1 || yAxisIndex == -1) {
        return NULL;
    } else if (m_results.size()){
        NewCurve *curve = new NewCurve (this);
        curve->setAllPoints(m_results[xAxisIndex].data(),
                            m_results[yAxisIndex].data(),
                            m_results[0].size());  // numberOfRows is the same for all results
        return curve;
    }

}

QStringList QLLTSimulation::getAvailableVariables(NewGraph::GraphType graphType) {
    switch (graphType) {
    case NewGraph::QLLTSimulation:
        return m_availableVariables;
    default:
        return QStringList();
    }
}

QLLTSimulation* QLLTSimulation::newBySerialize() {
    QLLTSimulation* fastSimulation = new QLLTSimulation ();
    fastSimulation->serialize();
    return fastSimulation;
}

