#include "FASTSimulation.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "../XBEM/Blade.h"
#include "../XBEM/360Polar.h"
#include "../QFem/BladeStructure.h"
#include "WindField.h"
#include "../Store.h"
#include "FASTResult.h"
#include "FASTModule.h"
#include "../Graph/NewCurve.h"
#include "../Serializer.h"
#include "../ColorManager.h"
#include <iostream>

const QString FASTSimulation::valueString[] = {
	"BEDDOES", "STEADY", "USE_CM", "NO_CM", "DYNIN", "EQUIL", "NONE", "WAKE", "SWIRL", "PRANDTL", "GTECH"};
const QString FASTSimulation::fastRotorOutputParameters[16] = {
	"TotWindV", "WindVxi", "WindVyi", "WindVzi", "HorWindV", "HorWndDir", "VerWndDir", "RotPwr", "RotTorq", "RotThrust",
	"RotCp", "RotCq", "RotCt", "LSSTipPxa", "LSSTipVxa", "LSSTipAxa"};
const QString FASTSimulation::fastBladeOutputParameters[28] = {
	"TipDxc", "TipDyc", "TipDzc", "TipDxb", "TipDyb", "TipALxb", "TipALyb", "TipALzb", "TipRDxb", "TipRDyb", "TipClrnc",
	"Spn%1ALxb", "Spn%1ALyb", "Spn%1ALzb", "PtchPMzc", "RootFxc", "RootFyc", "RootFzc", "RootFxb", "RootFyb", "RootMxc",
	"RootMyc", "RootMzc", "RootMxb", "RootMyb", "Spn%1MLxb", "Spn%1MLyb", "Spn%1MLzb"};
const int FASTSimulation::numberOfFastRotorOutputParameters = 16;
const int FASTSimulation::numberOfFastBladeOutputParameters = 28;
const QString FASTSimulation::fastRotorOutputParametersTooltips[16] = {
	"Total hub-height wind speed magnitude [m/s]", //
	"Nominally downwind component of the hub-height wind velocity [m/s]",   //
	"Cross-wind component of the hub-height wind velocity [m/s]",   //
	"Vertical component of the hub-height wind velocity [m/s]",   //
	"Horizontal hub-height wind speed magnitude [m/s]",   //
	"Horizontal hub-height wind direction [deg]",   //
	"Vertical hub-height wind direction [deg]",   //
	"Rotor power [kW]",   //
	"Rotor torque [kNm]",   //
	"Rotor thrust [kN]",   //
	"Rotor power coefficient [-]",   //
	"Rotor torque coefficient [-]",   //
	"Rotor thrust coefficient [-]",   //
	"Rotor azimuth angle (position) [deg]",   //
	"Rotor azimuth angular speed [rpm]",   //
	"Rotor azimuth angular acceleration [deg/s^2]"   //
};
const QString FASTSimulation::fastBladeOutputParametersTooltips[28] = {
	"Blade out-of-plane tip deflection (relative to the pitch axis) [m]",  //
	"Blade in-plane tip deflection (relative to the pitch axis) [m]",  //
	"Blade axial tip deflection (relative to the pitch axis) [m]",  //
	"Blade flapwise tip deflection (relative to the pitch axis) [m]",  //
	"Blade edgewise tip deflection (relative to the pitch axis) [m]",  //
	"Blade local flapwise tip acceleration (absolute) [m/s^2]",  //
	"Blade local edgewise tip acceleration (absolute) [m/s^2]",  //
	"Blade local axial tip acceleration (absolute) [m/s^2]",  //
	"Blade roll (angular/rotational) tip deflection (relative to the undeflected position) [deg]",  //
	"Blade pitch (angular/rotational) tip deflection (relative to the undeflected position) [deg]",  //
	"Blade tip-to-tower clearance estimate [m]",  //
	"Blade local flapwise acceleration from virtual strain gage [m/s^2]",  //
	"Blade local edgewise acceleration from virtual strain gage [m/s^2]",  //
	"Blade local axial acceleration from virtual strain gage [m/s^2]",  //
	"Blade pitch angle (position) [deg]",  //
	"Blade out-of-plane shear force at the blade root [kN]",  //
	"Blade in-plane shear force at the blade root [kN]",  //
	"Blade axial force at the blade root [kN]",  //
	"Blade flapwise shear force at the blade root [kN]",  //
	"Blade edgewise shear force at the blade root [kN]",  //
	"Blade in-plane moment (i.e., the moment caused by in-plane forces) at the blade root [kNm]",  //
	"Blade out-of-plane moment (i.e., the moment caused by out-of-plane forces) at the blade root [kNm]",  //
	"Blade pitching moment at the blade root [kNm]",  //
	"Blade edgewise moment (i.e., the moment caused by edgewise forces) at the blade root [kNm]",  //
	"Blade flapwise moment (i.e., the moment caused by flapwise forces) at the blade root [kNm]",  //
	"Blade local edgewise moment from virtual strain gage [kNm]",  //
	"Blade local flapwise moment from virtual strain gage [kNm]",  //
	"Blade local pitching moment from virtual strain gage [kNm]",  //
};


void FASTSimulation::serialize() {
	StorableObject::serialize();
	ShowAsGraphInterface::serialize();

    if (g_serializer.getArchiveFormat() >= 100031){
        g_serializer.readOrWriteBool(&m_bWindFromQBlade);
        g_serializer.readOrWriteString(&m_WindFieldPathName);
        g_serializer.readOrWriteDouble(&m_hubHeight);
    }
	
	g_serializer.readOrWriteStorableObject(&m_usedWindField);
	g_serializer.readOrWriteStorableObject(&m_usedRotor);
	g_serializer.readOrWriteStorableObject(&m_usedBladeStructure);
	
	g_serializer.readOrWriteEnum(&m_stallMod);
	g_serializer.readOrWriteEnum(&m_useCm);
	g_serializer.readOrWriteEnum(&m_infModel);
	g_serializer.readOrWriteEnum(&m_indModel);
	g_serializer.readOrWriteEnum(&m_tlModel);
	g_serializer.readOrWriteEnum(&m_hlModel);
	g_serializer.readOrWriteDouble(&m_totalRunTime);
	g_serializer.readOrWriteDouble(&m_timeStep);
    g_serializer.readOrWriteDouble(&m_aeroTimeStep);
	g_serializer.readOrWriteDouble(&m_rotorSpeed);
	g_serializer.readOrWriteDouble(&m_nacelleYaw);
	g_serializer.readOrWriteDouble(&m_gravity);
	g_serializer.readOrWriteDouble(&m_airDens);
	g_serializer.readOrWriteDouble(&m_kinVisc);
	g_serializer.readOrWriteDouble(&m_tStart);
	g_serializer.readOrWriteInt(&m_numberOfBlades);
	g_serializer.readOrWriteInt(&m_decFact);
	g_serializer.readOrWriteBool(&m_useFlapDof1);
	g_serializer.readOrWriteBool(&m_useFlapDof2);
	g_serializer.readOrWriteBool(&m_useEdgeDof);
	g_serializer.readOrWriteIntVector1D(&m_strainGages);
	g_serializer.readOrWriteIntVector1D(&m_bladeOutput);
	g_serializer.readOrWriteBitArray(&m_aeroDynOutput);
	g_serializer.readOrWriteBitArray(&m_rotorParameters);
	g_serializer.readOrWriteBitArray(&m_bladeParameters);
	
	for (int loop = 0; loop < 2; ++loop) {
		QVector<FASTResult*> *resultVector;
		if (loop == 0) {
			resultVector = &m_fastResults;
		} else if (loop == 1) {
			resultVector = &m_aeroDynResults;
		}
		
		if (g_serializer.isReadMode()) {
			int n = g_serializer.readInt();
			resultVector->resize(n);
			for (int i = 0; i < n; ++i) {
				(*resultVector)[i] = new FASTResult ();
			}
		} else {
			g_serializer.writeInt(resultVector->size());
		}
		for (int i = 0; i < resultVector->size(); ++i) {
			g_serializer.readOrWriteString(&((*resultVector)[i]->defaultName));
			g_serializer.readOrWriteString(&((*resultVector)[i]->siUnit));
			g_serializer.readOrWriteInt(&((*resultVector)[i]->numberOfRows));
			g_serializer.readOrWriteFloatArray1D(&((*resultVector)[i]->data), (*resultVector)[i]->numberOfRows);
		}
	}
	
	g_serializer.readOrWriteStringList(&m_availableVariablesFastGraph);
	g_serializer.readOrWriteStringList(&m_availableVariablesBladeGraph);
	g_serializer.readOrWriteInt(&m_shownBladeSection);
	g_serializer.readOrWriteInt(&m_shownTimeIndex);
}

void FASTSimulation::restorePointers() {
	StorableObject::restorePointers();
	
	g_serializer.restorePointer(reinterpret_cast<StorableObject**>(&m_usedWindField));
	g_serializer.restorePointer(reinterpret_cast<StorableObject**>(&m_usedRotor));
	g_serializer.restorePointer(reinterpret_cast<StorableObject**>(&m_usedBladeStructure));
}

FASTSimulation* FASTSimulation::newBySerialize() {
	FASTSimulation* fastSimulation = new FASTSimulation ();
	fastSimulation->serialize();
	return fastSimulation;
}

FASTSimulation::FASTSimulation()
	: StorableObject ("< no name >"),
	  ShowAsGraphInterface (false)
{
	m_shownBladeSection = 1;
	m_shownTimeIndex = 0;
    m_WindFieldPathName = "Open File";
    m_bWindFromQBlade = true;
    m_hubHeight = 0;
}

FASTSimulation::FASTSimulation(QString name,
                               WindField *usedWindField,
                               double totalRunTime,
                               double timeStep,
                               CBlade *usedRotor,
                               int numberOfBlades,
                               double rotorSpeed,
                               double nacelleYaw,
                               double gravity,
                               double airDens,
                               double kinVisc,
                               FASTValue stallMod,
                               FASTValue useCm,
                               FASTValue infModel,
                               FASTValue indModel,
                               FASTValue tlModel,
                               FASTValue hlModel,
                               BladeStructure *usedBladeStructure,
                               bool useFlapDof1,
                               bool useFlapDof2,
                               bool useEdgeDof,
                               QVector<int> *strainGages,
                               QBitArray *aeroDynOutput,
                               double tStart,
                               int decFact,
                               QBitArray *rotorParameters,
                               QVector<int> *bladeOutput,
                               QBitArray *bladeParameters,
                               double aeroTimeStep,
                               QString windfieldFile,
                               bool bFromQBlade,
                               double hubheight)
	: StorableObject (name)
{
	m_pen.setColor(g_colorManager.getLeastUsedColor(&g_FASTSimulationStore));
	m_pen.setWidth(1);
	m_pen.setStyle(Qt::SolidLine);
	m_shownBladeSection = 1;
	m_shownTimeIndex = 0;
    m_hubHeight = hubheight;

    m_bWindFromQBlade = bFromQBlade;
    m_WindFieldPathName = windfieldFile;
	
	m_usedWindField = usedWindField;
	addParent(m_usedWindField);
	m_totalRunTime = totalRunTime;
	m_timeStep = timeStep;
	m_usedRotor = usedRotor;
	addParent(m_usedRotor);
	m_numberOfBlades = numberOfBlades;
	m_rotorSpeed = rotorSpeed;
	m_nacelleYaw = nacelleYaw;
	m_gravity = gravity;
	m_airDens = airDens;
	m_kinVisc = kinVisc;
	m_stallMod = stallMod;
	m_useCm = useCm;
	m_infModel = infModel;
	m_indModel = indModel;
	m_tlModel = tlModel;
	m_hlModel = hlModel;
	m_usedBladeStructure = usedBladeStructure;
	addParent(m_usedBladeStructure);
	m_useFlapDof1 = useFlapDof1;
	m_useFlapDof2 = useFlapDof2;
	m_useEdgeDof = useEdgeDof;
	m_strainGages = *strainGages;
	m_aeroDynOutput = *aeroDynOutput;
	m_tStart = tStart;
	m_decFact = decFact;
	m_rotorParameters = *rotorParameters;
	m_bladeOutput = *bladeOutput;
	m_bladeParameters = *bladeParameters;
    m_aeroTimeStep = aeroTimeStep;
}

FASTSimulation::~FASTSimulation() {
	for (int i = 0; i < m_fastResults.size(); ++i) {
		delete m_fastResults[i];
	}
	for (int i = 0; i < m_aeroDynResults.size(); ++i) {
		delete m_aeroDynResults[i];
	}
}

QStringList FASTSimulation::prepareMissingObjectMessage() {
	if (g_FASTSimulationStore.isEmpty()) {
		QStringList message1 = WindField::prepareMissingObjectMessage();
		QStringList message2 = BladeStructureLoading::prepareMissingObjectMessage();
		if (!message1.isEmpty() && !message2.isEmpty()) {
			message1.removeLast();  // remove the "create WindField" hint
		}
		QStringList message = message1 + message2;
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == FASTMODULE) {
				message = QStringList(">>> Click 'New' to create a new FAST Simulation");
			} else {
				message = QStringList(">>> Create a new FAST Simulation in the FAST Simulation Module");
			}
		}
		message.prepend("- No FAST Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

void FASTSimulation::writeAllFiles(QDir &simulationDirectory) {
	QFile file (QString(simulationDirectory.absolutePath() + QDir::separator() + "fast.fst"));
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream (&file);

		/* write the whole file */
		stream << "--------------------------------------------------------------------------------" << endl <<
				  "------- FAST INPUT FILE --------------------------------------------------------" << endl <<
				  m_objectName << " :: generated by QBlade" << endl <<
				  "Compatible with FAST v7.02.00." << endl <<
				  "---------------------- SIMULATION CONTROL --------------------------------------" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "Echo         - Echo input data to \"echo.out\" (flag)" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "ADAMSPrep    - ADAMS preprocessor mode {1: Run FAST, 2: use FAST as a preprocessor to create an ADAMS model, 3: do both} (switch)" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "AnalMode     - Analysis mode {1: Run a time-marching simulation, 2: create a periodic linearized model} (switch)" << endl <<
				  QString("%1        ").arg(m_usedRotor->getNumberOfBlades(), 4) <<
				  "NumBl        - Number of blades (-)" << endl <<
				  QString("%1      ").arg(m_totalRunTime, 6, 'f', 1) <<
				  "TMax         - Total run time (s)" << endl <<
                  QString("%1    ").arg(m_timeStep, 8, 'f', 5) <<
				  "DT           - Integration time step (s)" << endl <<
				  "---------------------- TURBINE CONTROL -----------------------------------------" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "YCMode       - Yaw control mode {0: none, 1: user-defined from routine UserYawCont, 2: user-defined from Simulink/Labview} (switch)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TYCOn        - Time to enable active yaw control (s) [unused when YCMode=0]" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "PCMode       - Pitch control mode {0: none, 1: user-defined from routine PitchCntrl, 2: user-defined from Simulink/Labview} (switch)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPCOn        - Time to enable active pitch control (s) [unused when PCMode=0]" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "VSContrl     - Variable-speed control mode {0: none, 1: simple VS, 2: user-defined from routine UserVSCont, 3: user-defined from Simulink/Labview} (switch)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "VS_RtGnSp    - Rated generator speed for simple variable-speed generator control (HSS side) (rpm) [used only when VSContrl=1]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "VS_RtTq      - Rated generator torque/constant generator torque in Region 3 for simple variable-speed generator control (HSS side) (N-m) [used only when VSContrl=1]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "VS_Rgn2K     - Generator torque constant in Region 2 for simple variable-speed generator control (HSS side) (N-m/rpm^2) [used only when VSContrl=1]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "VS_SlPc      - Rated generator slip percentage in Region 2 1/2 for simple variable-speed generator control (%) [used only when VSContrl=1]" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "GenModel     - Generator model {1: simple, 2: Thevenin, 3: user-defined from routine UserGen} (switch) [used only when VSContrl=0]" << endl <<
				  QString("%1").arg((true?"True":"False"), -12) <<
				  "GenTiStr     - Method to start the generator {T: timed using TimGenOn, F: generator speed using SpdGenOn} (flag)" << endl <<
				  QString("%1").arg((true?"True":"False"), -12) <<
				  "GenTiStp     - Method to stop the generator {T: timed using TimGenOf, F: when generator power = 0} (flag)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "SpdGenOn     - Generator speed to turn on the generator for a startup (HSS speed) (rpm) [used only when GenTiStr=False]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TimGenOn     - Time to turn on the generator for a startup (s) [used only when GenTiStr=True]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TimGenOf     - Time to turn off the generator (s) [used only when GenTiStp=True]" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "HSSBrMode    - HSS brake model {1: simple, 2: user-defined from routine UserHSSBr, 3: user-defined from Labview} (switch)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "THSSBrDp     - Time to initiate deployment of the HSS brake (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TiDynBrk     - Time to initiate deployment of the dynamic generator brake [CURRENTLY IGNORED] (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TTpBrDp(1)   - Time to initiate deployment of tip brake 1 (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TTpBrDp(2)   - Time to initiate deployment of tip brake 2 (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TTpBrDp(3)   - Time to initiate deployment of tip brake 3 (s) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TBDepISp(1)  - Deployment-initiation speed for the tip brake on blade 1 (rpm)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TBDepISp(2)  - Deployment-initiation speed for the tip brake on blade 2 (rpm)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TBDepISp(3)  - Deployment-initiation speed for the tip brake on blade 3 (rpm) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TYawManS     - Time to start override yaw maneuver and end standard yaw control (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TYawManE     - Time at which override yaw maneuver reaches final yaw angle (s)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "NacYawF      - Final yaw angle for yaw maneuvers (degrees)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManS(1)  - Time to start override pitch maneuver for blade 1 and end standard pitch control (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManS(2)  - Time to start override pitch maneuver for blade 2 and end standard pitch control (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManS(3)  - Time to start override pitch maneuver for blade 3 and end standard pitch control (s) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManE(1)  - Time at which override pitch maneuver for blade 1 reaches final pitch (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManE(2)  - Time at which override pitch maneuver for blade 2 reaches final pitch (s)" << endl <<
				  QString("%1      ").arg(9999.9, 6, 'f', 1) <<
				  "TPitManE(3)  - Time at which override pitch maneuver for blade 3 reaches final pitch (s) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitch(1)   - Blade 1 initial pitch (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitch(2)   - Blade 2 initial pitch (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitch(3)   - Blade 3 initial pitch (degrees) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitchF(1)  - Blade 1 final pitch for pitch maneuvers (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitchF(2)  - Blade 2 final pitch for pitch maneuvers (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "BlPitchF(3)  - Blade 3 final pitch for pitch maneuvers (degrees) [unused for 2 blades]" << endl <<
				  "---------------------- ENVIRONMENTAL CONDITIONS --------------------------------" << endl <<
				  QString("%1  ").arg(m_gravity, 10, 'f', 5) <<
				  "Gravity      - Gravitational acceleration (m/s^2)" << endl <<
				  "---------------------- FEATURE FLAGS -------------------------------------------" << endl <<
				  QString("%1").arg((m_useFlapDof1?"True":"False"), -12) <<
				  "FlapDOF1     - First flapwise blade mode DOF (flag)" << endl <<
				  QString("%1").arg((m_useFlapDof2?"True":"False"), -12) <<
				  "FlapDOF2     - Second flapwise blade mode DOF (flag)" << endl <<
				  QString("%1").arg((m_useEdgeDof?"True":"False"), -12) <<
				  "EdgeDOF      - First edgewise blade mode DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "TeetDOF      - Rotor-teeter DOF (flag) [unused for 3 blades]" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "DrTrDOF      - Drivetrain rotational-flexibility DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "GenDOF       - Generator DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "YawDOF       - Yaw DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "TwFADOF1     - First fore-aft tower bending-mode DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "TwFADOF2     - Second fore-aft tower bending-mode DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "TwSSDOF1     - First side-to-side tower bending-mode DOF (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "TwSSDOF2     - Second side-to-side tower bending-mode DOF (flag)" << endl <<
				  QString("%1").arg((true?"True":"False"), -12) <<
				  "CompAero     - Compute aerodynamic forces (flag)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "CompNoise    - Compute aerodynamic noise (flag)" << endl <<
				  "---------------------- INITIAL CONDITIONS --------------------------------------" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "OoPDefl      - Initial out-of-plane blade-tip displacement (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "IPDefl       - Initial in-plane blade-tip deflection (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TeetDefl     - Initial or fixed teeter angle (degrees) [unused for 3 blades]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "Azimuth      - Initial azimuth angle for blade 1 (degrees)" << endl <<
				  QString("%1    ").arg(m_rotorSpeed, 8, 'f', 3) <<
				  "RotSpeed     - Initial or fixed rotor speed (rpm)" << endl <<
				  QString("%1      ").arg(m_nacelleYaw, 6, 'f', 1) <<
				  "NacYaw       - Initial or fixed nacelle-yaw angle (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TTDspFA      - Initial fore-aft tower-top displacement (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TTDspSS      - Initial side-to-side tower-top displacement (meters)" << endl <<
				  "---------------------- TURBINE CONFIGURATION -----------------------------------" << endl <<
				  QString("%1    ").arg(m_usedRotor->getRotorRadius(), 8, 'f', 3) <<
				  "TipRad       - The distance from the rotor apex to the blade tip (meters)" << endl <<
                  QString("%1    ").arg(m_usedRotor->m_TPos[0], 8, 'f', 3) <<
				  "HubRad       - The distance from the rotor apex to the blade root (meters)" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "PSpnElN      - Number of the innermost blade element which is still part of the pitchable portion of the blade for partial-span pitch control [1 to BldNodes] [CURRENTLY IGNORED] (-)" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "UndSling     - Undersling length [distance from teeter pin to the rotor apex] (meters) [unused for 3 blades]" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "HubCM        - Distance from rotor apex to hub mass [positive downwind] (meters)" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "OverHang     - Distance from yaw axis to rotor apex [3 blades] or teeter pin [2 blades] (meters)" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "NacCMxn      - Downwind distance from the tower-top to the nacelle CM (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "NacCMyn      - Lateral  distance from the tower-top to the nacelle CM (meters)" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "NacCMzn      - Vertical distance from the tower-top to the nacelle CM (meters)" << endl <<
                  QString("%1    ").arg(m_hubHeight, 8, 'f', 3) <<
				  "TowerHt      - Height of tower above ground level [onshore] or MSL [offshore] (meters)" << endl <<
                  QString("%1    ").arg(0.0, 8, 'f', 3) <<
				  "Twr2Shft     - Vertical distance from the tower-top to the rotor shaft (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TwrRBHt      - Tower rigid base height (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "ShftTilt     - Rotor shaft tilt angle (degrees)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "Delta3       - Delta-3 angle for teetering rotors (degrees) [unused for 3 blades]" << endl <<
                  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "PreCone(1)   - Blade 1 cone angle (degrees)" << endl <<
                  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "PreCone(2)   - Blade 2 cone angle (degrees)" << endl <<
                  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "PreCone(3)   - Blade 3 cone angle (degrees) [unused for 2 blades]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "AzimB1Up     - Azimuth value to use for I/O when blade 1 points up (degrees)" << endl <<
				  "---------------------- MASS AND INERTIA ----------------------------------------" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "YawBrMass    - Yaw bearing mass (kg)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "NacMass      - Nacelle mass (kg)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "HubMass      - Hub mass (kg)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "TipMass(1)   - Tip-brake mass, blade 1 (kg)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "TipMass(2)   - Tip-brake mass, blade 2 (kg)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "TipMass(3)   - Tip-brake mass, blade 3 (kg) [unused for 2 blades]" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "NacYIner     - Nacelle inertia about yaw axis (kg m^2)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "GenIner      - Generator inertia about HSS (kg m^2)" << endl <<
                  QString("%1     ").arg(0.0, 7, 'f', 2) <<
				  "HubIner      - Hub inertia about rotor axis [3 blades] or teeter axis [2 blades] (kg m^2)" << endl <<
				  "---------------------- DRIVETRAIN ----------------------------------------------" << endl <<
				  QString("%1      ").arg(100.0, 6, 'f', 1) <<
				  "GBoxEff      - Gearbox efficiency (%)" << endl <<
				  QString("%1      ").arg(100.0, 6, 'f', 1) <<
				  "GenEff       - Generator efficiency [ignored by the Thevenin and user-defined generator models] (%)" << endl <<
				  QString("%1      ").arg(22.5, 6, 'f', 1) <<
				  "GBRatio      - Gearbox ratio (-)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "GBRevers     - Gearbox reversal {T: if rotor and generator rotate in opposite directions} (flag)" << endl <<
				  QString("%1      ").arg(6000.0, 6, 'f', 1) <<
				  "HSSBrTqF     - Fully deployed HSS-brake torque (N-m)" << endl <<
				  QString("%1      ").arg(0.5, 6, 'f', 1) <<
				  "HSSBrDT      - Time for HSS-brake to reach full deployment once initiated (sec) [used only when HSSBrMode=1]" << endl <<
				  QString("%1").arg("\"unused\"", -11) <<
				  " DynBrkFi     - File containing a mech-gen-torque vs HSS-speed curve for a dynamic brake [CURRENTLY IGNORED] (quoted string)" << endl <<
				  QString("%1  ").arg(50.0E6, 10, 'E', 1) <<
				  "DTTorSpr     - Drivetrain torsional spring (N-m/rad)" << endl <<
				  QString("%1  ").arg(1.0E6, 10, 'E', 1) <<
				  "DTTorDmp     - Drivetrain torsional damper (N-m/(rad/s))" << endl <<
				  "---------------------- SIMPLE INDUCTION GENERATOR ------------------------------" << endl <<
				  QString("%1   ").arg(1.5125, 9, 'f', 4) <<
				  "SIG_SlPc     - Rated generator slip percentage (%) [used only when VSContrl=0 and GenModel=1]" << endl <<
				  QString("%1      ").arg(1200.0, 6, 'f', 1) <<
				  "SIG_SySp     - Synchronous (zero-torque) generator speed (rpm) [used only when VSContrl=0 and GenModel=1]" << endl <<
				  QString("%1      ").arg(1367.9, 6, 'f', 1) <<
				  "SIG_RtTq     - Rated torque (N-m) [used only when VSContrl=0 and GenModel=1]" << endl <<
				  QString("%1      ").arg(2.0, 6, 'f', 1) <<
				  "SIG_PORt     - Pull-out ratio (Tpullout/Trated) (-) [used only when VSContrl=0 and GenModel=1]" << endl <<
				  "---------------------- THEVENIN-EQUIVALENT INDUCTION GENERATOR -----------------" << endl <<
				  QString("%1      ").arg(60.0, 6, 'f', 1) <<
				  "TEC_Freq     - Line frequency [50 or 60] (Hz) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1        ").arg(6, 4) <<
				  "TEC_NPol     - Number of poles [even integer > 0] (-) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1   ").arg(0.0185, 9, 'f', 4) <<
				  "TEC_SRes     - Stator resistance (ohms) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1    ").arg(0.017, 8, 'f', 3) <<
				  "TEC_RRes     - Rotor resistance (ohms) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1      ").arg(480.0, 6, 'f', 1) <<
				  "TEC_VLL      - Line-to-line RMS voltage (volts) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1   ").arg(0.0340, 9, 'f', 4) <<
				  "TEC_SLR      - Stator leakage reactance (ohms) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1   ").arg(0.0050, 9, 'f', 4) <<
				  "TEC_RLR      - Rotor leakage reactance (ohms) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  QString("%1   ").arg(0.7750, 9, 'f', 4) <<
				  "TEC_MR       - Magnetizing reactance (ohms) [used only when VSContrl=0 and GenModel=2]" << endl <<
				  "---------------------- PLATFORM ------------------------------------------------" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "PtfmModel    - Platform model {0: none, 1: onshore, 2: fixed bottom offshore, 3: floating offshore} (switch)" << endl <<
				  QString("%1").arg("\"unused\"", -11) <<
				  " PtfmFile     - Name of file containing platform properties (quoted string) [unused when PtfmModel=0]" << endl <<
				  "---------------------- TOWER ---------------------------------------------------" << endl <<
				  QString("%1        ").arg(21, 4) <<
				  "TwrNodes     - Number of tower nodes used for analysis (-)" << endl <<
				  QString("%1").arg("\"towerfile.dat\"", -11) <<
				  " TwrFile  - Name of file containing tower properties (quoted string)" << endl <<
				  "---------------------- NACELLE-YAW ---------------------------------------------" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "YawSpr       - Nacelle-yaw spring constant (N-m/rad)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "YawDamp      - Nacelle-yaw damping constant (N-m/(rad/s))" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "YawNeut      - Neutral yaw position--yaw spring force is zero at this yaw (degrees)" << endl <<
				  "---------------------- FURLING -------------------------------------------------" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "Furling      - Read in additional model properties for furling turbine (flag)" << endl <<
				  QString("%1").arg("\"unused\"", -11) <<
				  " FurlFile     - Name of file containing furling properties (quoted string) [unused when Furling=False]" << endl <<
				  "---------------------- ROTOR-TEETER --------------------------------------------" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "TeetMod      - Rotor-teeter spring/damper model {0: none, 1: standard, 2: user-defined from routine UserTeet} (switch) [unused for 3 blades]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TeetDmpP     - Rotor-teeter damper position (degrees) [used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1  ").arg(4.0e4, 10, 'e', 1) <<
				  "TeetDmp      - Rotor-teeter damping constant (N-m/(rad/s)) [[used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TeetCDmp     - Rotor-teeter rate-independent Coulomb-damping moment (N-m) [used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TeetSStP     - Rotor-teeter soft-stop position (degrees) [used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1      ").arg(180.0, 6, 'f', 1) <<
				  "TeetHStP     - Rotor-teeter hard-stop position (degrees) [used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "TeetSSSp     - Rotor-teeter soft-stop linear-spring constant (N-m/rad) [used only for 2 blades and when TeetMod=1]" << endl <<
				  QString("%1  ").arg(5.0e6, 10, 'e', 1) <<
				  "TeetHSSp     - Rotor-teeter hard-stop linear-spring constant (N-m/rad) [used only for 2 blades and when TeetMod=1]" << endl <<
				  "---------------------- TIP-BRAKE -----------------------------------------------" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TBDrConN     - Tip-brake drag constant during normal operation, Cd*Area (m^2)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TBDrConD     - Tip-brake drag constant during fully-deployed operation, Cd*Area (m^2)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "TpBrDT       - Time for tip-brake to reach full deployment once released (sec)" << endl <<
				  "---------------------- BLADE ---------------------------------------------------" << endl <<
                  QString("%1").arg("\"bladeFile.dat\"", -11) <<
				  " BldFile(1) - Name of file containing properties for blade 1 (quoted string)" << endl <<
                  QString("%1").arg("\"bladeFile.dat\"", -11) <<
				  " BldFile(2) - Name of file containing properties for blade 2 (quoted string)" << endl <<
                  QString("%1").arg("\"bladeFile.dat\"", -11) <<
				  " BldFile(3) - Name of file containing properties for blade 3 (quoted string) [unused for 2 blades]" << endl <<
				  "---------------------- AERODYN -------------------------------------------------" << endl <<
				  QString("%1").arg("\"aerodyn.ipt\"", -11) <<
				  " ADFile     - Name of file containing AeroDyn input parameters (quoted string)" << endl <<
				  "---------------------- NOISE ---------------------------------------------------" << endl <<
				  QString("%1").arg("\"unused\"", -11) <<
				  " NoiseFile    - Name of file containing aerodynamic noise input parameters (quoted string) [used only when CompNoise=True]" << endl <<
				  "---------------------- ADAMS ---------------------------------------------------" << endl <<
				  QString("%1").arg("\"AWT_ADAMS.dat\"", -11) <<
				  " ADAMSFile  - Name of file containing ADAMS-specific input parameters (quoted string) [unused when ADAMSPrep=1]" << endl <<
				  "---------------------- LINEARIZATION CONTROL -----------------------------------" << endl <<
				  QString("%1").arg("\"AWT_Linear.dat\"", -11) <<
				  " LinFile    - Name of file containing FAST linearization parameters (quoted string) [unused when AnalMode=1]" << endl <<
				  "---------------------- OUTPUT --------------------------------------------------" << endl <<
				  QString("%1").arg((true?"True":"False"), -12) <<
				  "SumPrint     - Print summary data to \"<RootName>.fsm\" (flag)" << endl <<
				  QString("%1        ").arg(1, 4) <<
				  "OutFileFmt   - Format for tabular (time-marching) output file(s) (1: text file [<RootName>.out], 2: binary file [<RootName>.outb], 3: both) (switch)" << endl <<
				  QString("%1").arg((true?"True":"False"), -12) <<
				  "TabDelim     - Use tab delimiters in text tabular output file? (flag)" << endl <<
				  QString("%1").arg("\"ES10.3E2\"", -11) <<
				  " OutFmt       - Format used for text tabular output (except time).  Resulting field should be 10 characters. (quoted string)  [not checked for validity!]" << endl <<
                  QString("%1      ").arg(m_tStart, 6, 'f', 1) <<
				  "TStart       - Time to begin tabular output (s)" << endl <<
                  QString("%1        ").arg(m_decFact, 4) <<
				  "DecFact      - Decimation factor for tabular output {1: output every time step} (-)" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "SttsTime     - Amount of time between screen status messages (sec)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "NcIMUxn      - Downwind distance from the tower-top to the nacelle IMU (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "NcIMUyn      - Lateral  distance from the tower-top to the nacelle IMU (meters)" << endl <<
				  QString("%1      ").arg(0.0, 6, 'f', 1) <<
				  "NcIMUzn      - Vertical distance from the tower-top to the nacelle IMU (meters)" << endl <<
				  QString("%1      ").arg(-0.2, 6, 'f', 1) <<
				  "ShftGagL     - Distance from rotor apex [3 blades] or teeter pin [2 blades] to shaft strain gages [positive for upwind rotors] (meters)" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "NTwGages     - Number of tower nodes that have strain gages for output [0 to 9] (-)" << endl <<
				  QString("%1        ").arg(0, 4) <<
				  "TwrGagNd     - List of tower nodes that have strain gages [1 to TwrNodes] (-) [unused if NTwGages=0]" << endl <<
				  QString("%1        ").arg(m_strainGages.size(), 4) <<
				  "NBlGages     - Number of blade nodes that have strain gages for output [0 to 9] (-)" << endl;
		for (int i = 0; i < m_strainGages.size(); ++i) {
			if (i != 0)
				stream << ',';
			stream << QString("%1").arg(m_strainGages[i]);
		}
		stream << " BldGagNd     - List of blade nodes that have strain gages [1 to BldNodes] (-) [unused if NBlGages=0]" << endl <<
				  "            OutList      - The next line(s) contains a list of output parameters.  See OutList.xlsx for a listing of available output channels, (-)" << endl;
		
		bool first = true;
		stream << "\"";
		for (int i = 0; i < numberOfFastRotorOutputParameters; ++i) {
			if (m_rotorParameters[i]) {
				if (first) {
					first = false;
				} else {
					stream << ", ";
				}
				stream << fastRotorOutputParameters[i];
			}
		}
		stream << "\"" << endl;
		
		first = true;
		stream << "\"";
		for (int i = 0; i < numberOfFastBladeOutputParameters; ++i) {
			if (m_bladeParameters[i]) {
				for (int bladeIndex = 0; bladeIndex < m_bladeOutput.size(); ++bladeIndex) {
					if (QString(fastBladeOutputParameters[i]).indexOf("%") == -1) {
						if (first) {
							first = false;
						} else {
							stream << ", ";
						}
						stream << QString(fastBladeOutputParameters[i] + "%1").arg(m_bladeOutput[bladeIndex]);
					} else {
						for (int gageIndex = 0; gageIndex < m_strainGages.size(); ++gageIndex) {
							if (first) {
								first = false;
							} else {
								stream << ", ";
							}
							stream << QString(fastBladeOutputParameters[i] + "%2").arg(gageIndex+1).arg(
																						m_bladeOutput[bladeIndex]);
						}
					}
				}
			}
		}
		stream << "\"" << endl;
		
		stream << "END of FAST input file (the word \"END\" must appear in the first 3 columns of this last line)." << endl <<
				  "--------------------------------------------------------------------------------" << endl;
		/* end of file */
		file.close();
	} else {
		throw QString(tr("Could not create file: ") + file.fileName());
	}
	
	QFile file2 (QString(simulationDirectory.absolutePath() + QDir::separator() + "aerodyn.ipt"));
	if (file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
		/* create the needed polar files */
		QVector<C360Polar*> exportedPolars;  // will contain the exported polars
		QStringList polarFiles;  // will contain the filenames of the exported polars
		
		C360Polar *polar360;
		for (int i = 0; i < m_usedRotor->getNumberOfNodes(); ++i) {
			polar360 = m_usedRotor->get360PolarAt(i);
			if (! exportedPolars.contains(polar360)) {
				exportedPolars.append(polar360);
				// TODO need a better method to find names for the 360Polar export files!
				polarFiles.append(QString(polar360->getName() + ".dat").replace(" ", "_").replace("/", "_"));
				
				QFile datFile (QString(simulationDirectory.absolutePath() + QDir::separator() + polarFiles.last()));
				if (datFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // option Text sets the correct line endings
					QTextStream datStream (&datFile);
                    polar360->ExportPolarNREL(datStream);
					datFile.close();
				} else {
					throw QString(tr("Could not create file: ") + datFile.fileName());
				}
			}
		}
		
		QTextStream stream (&file2);

        QString WindFieldName;
        if (m_bWindFromQBlade) WindFieldName = QString("\""+m_usedWindField->getName().replace(" ","_")+".bts\"");
        else WindFieldName = QString("\""+m_WindFieldPathName+"\"");

		/* write the whole file */
		stream << m_usedRotor->getName() << " :: generated by QBlade" << endl <<
				  QString("%1").arg("SI", -39) <<
				  "SysUnits - System of units for used for input and output [must be SI for FAST] (unquoted string)" << endl <<
				  QString("%1").arg(valueString[m_stallMod], -39) <<
				  "StallMod - Dynamic stall included [BEDDOES or STEADY] (unquoted string)" << endl <<
				  QString("%1").arg(valueString[m_useCm], -39) <<
				  "UseCm    - Use aerodynamic pitching moment model? [USE_CM or NO_CM] (unquoted string)" << endl <<
				  QString("%1").arg(valueString[m_infModel], -39) <<
				  "InfModel - Inflow model [DYNIN or EQUIL] (unquoted string)" << endl <<
				  QString("%1").arg(valueString[m_indModel], -39) <<
				  "IndModel - Induction-factor model [NONE or WAKE or SWIRL] (unquoted string)" << endl <<
				  QString("%1                               ").arg(0.005, 8, 'f', 3) <<
				  "AToler   - Induction-factor tolerance (convergence criteria) (-)" << endl <<
				  QString("%1").arg(valueString[m_tlModel], -39) <<
				  "TLModel  - Tip-loss model (EQUIL only) [PRANDtl, GTECH, or NONE] (unquoted string)" << endl <<
				  QString("%1").arg(valueString[m_hlModel], -39) <<
				  "HLModel  - Hub-loss model (EQUIL only) [PRANdtl or NONE] (unquoted string)" << endl <<
                  QString("%1").arg(WindFieldName, -38) <<
				  " WindFile - Name of file containing wind data (quoted string)" << endl <<
				  QString("%1                               ").arg(m_usedWindField->getHubheight(), 8, 'f', 3) <<
				  "HH       - Wind reference (hub) height [TowerHt+Twr2Shft+OverHang*SIN(ShftTilt)] (m)" << endl <<
				  QString("%1                                 ").arg(0.0, 6, 'f', 1) <<
				  "TwrShad  - Tower-shadow velocity deficit (-)" << endl <<
                  QString("%1                                 ").arg(1.0, 6, 'f', 1) <<
				  "ShadHWid - Tower-shadow half width (m)" << endl <<
				  QString("%1                               ").arg(0.0, 8, 'f', 3) <<
				  "T_Shad_Refpt - Tower-shadow reference point (m)" << endl <<
				  QString("%1                               ").arg(m_airDens, 8, 'f', 3) <<
				  "AirDens  - Air density (kg/m^3)" << endl <<
				  QString("%1                          ").arg(m_kinVisc, 13, 'e', 4) <<
				  "KinVisc  - Kinematic air viscosity (m^2/sec)" << endl <<
                  QString("%1                               ").arg(m_aeroTimeStep, 8, 'f', 5) <<
				  "DTAero   - Time interval for aerodynamic calculations (sec)" << endl <<
				  QString("%1                                   ").arg(polarFiles.size(), 4) <<
				  "NumFoil  - Number of airfoil files (-)" << endl;
		for (int i = 0; i < polarFiles.size(); ++i) {
			if (i == 0)
				stream << QString("%1").arg("\"" + polarFiles[i] + "\"", -38) <<
						  " FoilNm   - Names of the airfoil files [NumFoil lines] (quoted strings)" << endl;
			else
				stream << QString("%1").arg("\"" + polarFiles[i] + "\"") << endl;
		}
		stream << QString("%1                                   ").arg(m_usedRotor->m_NPanel, 4) <<
				  "BldNodes - Number of blade nodes used for analysis (-)" << endl <<
				  "RNodes   AeroTwst DRNodes Chord NFoil PrnElm" << endl;
		double position, twist, dr, chord;
		int airfoilNumber;
		for (int i = 0; i < m_usedRotor->getNumberOfNodes()-1; ++i) {  // leave out the last QBlade node
			position = m_usedRotor->getFASTRadiusAt(i);
            twist = m_usedRotor->m_TTwist[i+1] / 2 + m_usedRotor->m_TTwist[i] / 2;
			dr = m_usedRotor->m_TPos[i+1] - m_usedRotor->m_TPos[i];
            chord = m_usedRotor->m_TChord[i+1] / 2 + m_usedRotor->m_TChord[i] / 2;
			airfoilNumber = exportedPolars.indexOf(m_usedRotor->get360PolarAt(i)) + 1;  // index starts at 0 but AeroDyn at 1
			stream << QString("%1").arg(position, 8, 'f', 5) <<
					  QString("%1").arg(twist, 6, 'f', 2) <<
					  QString("%1").arg(dr, 10, 'f', 4) <<
					  QString("%1").arg(chord, 7, 'f', 3) <<
					  QString("%1").arg(airfoilNumber, 4);
			if (m_aeroDynOutput[i]) {
				stream << QString("%1").arg("PRINT", 10) << endl;
			} else {
				stream << QString("%1").arg("NOPRINT", 10) << endl;
			}					  
		}
		/* end of file */
        file2.close();
	} else {
		throw QString(tr("Could not create file: ") + file2.fileName());
	}
	
    if (m_bWindFromQBlade){
        QFile file3 (QString(simulationDirectory.absolutePath() + QDir::separator() + m_usedWindField->getName().replace(" ","_")+".bts"));
        if (file3.open(QIODevice::WriteOnly)) {
            QDataStream btsStream (&file3);
            m_usedWindField->exportToBinary(btsStream);
            file3.close();
        } else {
            throw QString(tr("Could not create file: ") + file3.fileName());
        }
    }
	
	QFile file4 (QString(simulationDirectory.absolutePath() + QDir::separator() + "towerfile.dat"));
	if (file4.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream (&file4);

		/* write the whole file */
		stream << "--------------------------------------------------------------------------------" << endl <<
				  "---------------------- FAST TOWER FILE -----------------------------------------" << endl <<
				  "A default towerfile" << " :: generated by QBlade" << endl <<
				  "---------------------- TOWER PARAMETERS ----------------------------------------" << endl <<
				  "   1        NTwInpSt    - Number of input stations to specify tower geometry" << endl <<
				  "False       CalcTMode   - Calculate tower mode shapes internally {T: ignore mode shapes from below, F: use mode shapes from below} [CURRENTLY IGNORED] (flag)" << endl <<
				  "   5.0      TwrFADmp(1) - Tower 1st fore-aft mode structural damping ratio (%)" << endl <<
				  "   9.58     TwrFADmp(2) - Tower 2nd fore-aft mode structural damping ratio (%)" << endl <<
				  "   5.0      TwrSSDmp(1) - Tower 1st side-to-side mode structural damping ratio (%)" << endl <<
				  "   9.62     TwrSSDmp(2) - Tower 2nd side-to-side mode structural damping ratio (%)" << endl <<
				  "---------------------- TOWER ADJUSTMUNT FACTORS --------------------------------" << endl <<
				  "   1.0      FAStTunr(1) - Tower fore-aft modal stiffness tuner, 1st mode (-)" << endl <<
				  "   1.0      FAStTunr(2) - Tower fore-aft modal stiffness tuner, 2nd mode (-)" << endl <<
				  "   1.0      SSStTunr(1) - Tower side-to-side stiffness tuner, 1st mode (-)" << endl <<
				  "   1.0      SSStTunr(2) - Tower side-to-side stiffness tuner, 2nd mode (-)" << endl <<
				  "   1.0      AdjTwMa     - Factor to adjust tower mass density (-)" << endl <<
				  "   1.0      AdjFASt     - Factor to adjust tower fore-aft stiffness (-)" << endl <<
				  "   1.0      AdjSSSt     - Factor to adjust tower side-to-side stiffness (-)" << endl <<
				  "---------------------- DISTRIBUTED TOWER PROPERTIES ----------------------------" << endl <<
				  "HtFract      TMassDen     TwFAStif     TwSSStif     TwGJStif     TwEAStif      TwFAIner     TwSSIner    TwFAcgOf     TwSScgOf" << endl <<
				  "    (-)        (kg/m)       (Nm^2)       (Nm^2)       (Nm^2)          (N)        (kg m)       (kg m)         (m)          (m)" << endl <<
				  "  0.000        879.16     1.564E10     1.564E10       3.0E10       2.02E9           0.0          0.0         0.0          0.0" << endl <<
				  "---------------------- TOWER FORE-AFT MODE SHAPES ------------------------------" << endl <<
				  "   1.6400   TwFAM1Sh(2) - Mode 1, coefficient of x^2 term" << endl <<
				  "  -0.6510   TwFAM1Sh(3) -       , coefficient of x^3 term" << endl <<
				  "  -0.0460   TwFAM1Sh(4) -       , coefficient of x^4 term" << endl <<
				  "   0.0520   TwFAM1Sh(5) -       , coefficient of x^5 term" << endl <<
				  "   0.0050   TwFAM1Sh(6) -       , coefficient of x^6 term" << endl <<
				  " -17.9490   TwFAM2Sh(2) - Mode 2, coefficient of x^2 term" << endl <<
				  "  22.8840   TwFAM2Sh(3) -       , coefficient of x^3 term" << endl <<
				  "  14.1730   TwFAM2Sh(4) -       , coefficient of x^4 term" << endl <<
				  " -27.1580   TwFAM2Sh(5) -       , coefficient of x^5 term" << endl <<
				  "   9.0500   TwFAM2Sh(6) -       , coefficient of x^6 term" << endl <<
				  "---------------------- TOWER SIDE-TO-SIDE MODE SHAPES --------------------------" << endl <<
				  "   1.6400   TwSSM1Sh(2) - Mode 1, coefficient of x^2 term" << endl <<
				  "  -0.6510   TwSSM1Sh(3) -       , coefficient of x^3 term" << endl <<
				  "  -0.0460   TwSSM1Sh(4) -       , coefficient of x^4 term" << endl <<
				  "   0.0520   TwSSM1Sh(5) -       , coefficient of x^5 term" << endl <<
				  "   0.0050   TwSSM1Sh(6) -       , coefficient of x^6 term" << endl <<
				  " -17.9490   TwSSM2Sh(2) - Mode 2, coefficient of x^2 term" << endl <<
				  "  22.8840   TwSSM2Sh(3) -       , coefficient of x^3 term" << endl <<
				  "  14.1730   TwSSM2Sh(4) -       , coefficient of x^4 term" << endl <<
				  " -27.1580   TwSSM2Sh(5) -       , coefficient of x^5 term" << endl <<
				  "   9.0500   TwSSM2Sh(6) -       , coefficient of x^6 term" << endl;
		/* end of file */
		file.close();		
	} else {
		throw QString(tr("Could not create file: ") + file4.fileName());
	}

    QFile file5 (QString(simulationDirectory.absolutePath() + QDir::separator() + "bladeFile.dat"));
    if (file5.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file5);

		/* write the whole file */
		stream << "--------------------------------------------------------------------------------" << endl <<
				  "---------------------- FAST INDIVIDUAL BLADE FILE ------------------------------" << endl <<
				  m_usedRotor->getName() << " :: generated by QBlade" << endl <<
				  "---------------------- BLADE PARAMETERS ----------------------------------------" << endl <<
				  QString("%1        ").arg(m_usedBladeStructure->m_numElems, 4) <<
				  "NBlInpSt    - Number of blade input stations (-)" << endl <<
				  QString("%1").arg((false?"True":"False"), -12) <<
				  "CalcBMode   - Calculate blade mode shapes internally {T: ignore mode shapes from below, F: use mode shapes from below} [CURRENTLY IGNORED] (flag)" << endl <<
				  QString("%1      ").arg(3.0, 6, 'f', 1) <<
				  "BldFlDmp(1) - Blade flap mode #1 structural damping in percent of critical (%)" << endl <<
				  QString("%1      ").arg(3.0, 6, 'f', 1) <<
				  "BldFlDmp(2) - Blade flap mode #2 structural damping in percent of critical (%)" << endl <<
				  QString("%1      ").arg(3.0, 6, 'f', 1) <<
				  "BldEdDmp(1) - Blade edge mode #1 structural damping in percent of critical (%)" << endl <<
				  "---------------------- BLADE ADJUSTMENT FACTORS --------------------------------" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "FlStTunr(1) - Blade flapwise modal stiffness tuner, 1st mode (-)" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "FlStTunr(2) - Blade flapwise modal stiffness tuner, 2nd mode (-)" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "AdjBlMs     - Factor to adjust blade mass density (-)" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "AdjFlSt     - Factor to adjust blade flap stiffness (-)" << endl <<
				  QString("%1      ").arg(1.0, 6, 'f', 1) <<
				  "AdjEdSt     - Factor to adjust blade edge stiffness (-)" << endl <<
				  "---------------------- DISTRIBUTED BLADE PROPERTIES ----------------------------" << endl <<
				  "BlFract  AeroCent  StrcTwst  BMassDen    FlpStff    EdgStff    GJStff    EAStff   Alpha   FlpIner   EdgIner   PrecrvRef   PreswpRef   FlpcgOf   EdgcgOf   FlpEAOf   EdgEAOf" << endl <<
				  "    (-)       (-)     (deg)    (kg/m)     (Nm^2)     (Nm^2)    (Nm^2)       (N)     (-)    (kg m)    (kg m)         (m)         (m)       (m)       (m)       (m)       (m)" << endl;
		for (int i = 0; i < m_usedBladeStructure->m_numElems; ++i) {
			stream << QString("%1").arg((m_usedRotor->m_TPos[i]-m_usedRotor->m_HubRadius)/(m_usedRotor->getRotorRadius()-m_usedRotor->m_HubRadius), 7, 'f', 3) <<
					  QString("%1").arg(0.250, 10, 'f', 3) <<
					  QString("%1").arg(m_usedBladeStructure->StructuralTwist.at(i), 10, 'f', 3) <<
					  QString("%1").arg(m_usedBladeStructure->RhoA[i], 10, 'E', 2) <<
                      QString("%1").arg(m_usedBladeStructure->EI1[i], 10, 'E', 2) <<
                      QString("%1").arg(m_usedBladeStructure->EI2[i], 10, 'E', 2) <<
					  QString("%1").arg(m_usedBladeStructure->GJ[i], 10, 'E', 2) <<
					  QString("%1").arg(m_usedBladeStructure->EA[i], 10, 'E', 2) <<
                      QString("%1").arg(0.0, 8, 'E', 1) <<
                      QString("%1").arg(0.0, 10, 'E', 2) <<
                      QString("%1").arg(0.0, 10, 'E', 2) <<
					  QString("%1").arg(0.0, 12, 'f', 1) <<
					  QString("%1").arg(0.0, 12, 'f', 1) <<
					  QString("%1").arg(0.0, 10, 'f', 1) <<
					  QString("%1").arg(0.0, 10, 'f', 1) <<
					  QString("%1").arg(0.0, 10, 'f', 1) <<
					  QString("%1").arg(0.0, 10, 'f', 1) << endl;
		}

        stream << "---------------------- BLADE MODE SHAPES ---------------------------------------" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[0][1], 9, 'f', 4) <<
				  "BldFl1Sh(2) - Flap mode 1, coeff of x^2" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[0][2], 9, 'f', 4) <<
				  "BldFl1Sh(3) -            , coeff of x^3" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[0][3], 9, 'f', 4) <<
				  "BldFl1Sh(4) -            , coeff of x^4" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[0][4], 9, 'f', 4) <<
				  "BldFl1Sh(5) -            , coeff of x^5" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[0][5], 9, 'f', 4) <<
				  "BldFl1Sh(6) -            , coeff of x^6" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[1][1], 9, 'f', 4) <<
				  "BldFl2Sh(2) - Flap mode 2, coeff of x^2" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[1][2], 9, 'f', 4) <<
				  "BldFl2Sh(3) -            , coeff of x^3" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[1][3], 9, 'f', 4) <<
				  "BldFl2Sh(4) -            , coeff of x^4" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[1][4], 9, 'f', 4) <<
				  "BldFl2Sh(5) -            , coeff of x^5" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->FlapwiseCoefficients[1][5], 9, 'f', 4) <<
				  "BldFl2Sh(6) -            , coeff of x^6" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->EdgewiseCoefficients[0][1], 9, 'f', 4) <<
				  "BldEdgSh(2) - Edge mode 1, coeff of x^2" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->EdgewiseCoefficients[0][2], 9, 'f', 4) <<
				  "BldEdgSh(3) -            , coeff of x^3" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->EdgewiseCoefficients[0][3], 9, 'f', 4) <<
				  "BldEdgSh(4) -            , coeff of x^4" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->EdgewiseCoefficients[0][4], 9, 'f', 4) <<
				  "BldEdgSh(5) -            , coeff of x^5" << endl <<
                  QString("%1   ").arg(m_usedBladeStructure->EdgewiseCoefficients[0][5], 9, 'f', 4) <<
				  "BldEdgSh(6) -            , coeff of x^6" << endl;
		/* end of file */
		file5.close();
	} else {
		throw QString(tr("Could not create file: ") + file5.fileName());
	}
}

void FASTSimulation::readOutputFile(QString fileToRead) {
	/**** FAST ****/
	QFile file (fileToRead);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // read FAST output file
		int unusedLines = 6;
		int lineCount = 0;
		QTextStream stream (&file);
		while (!stream.atEnd()) {  // count lines
			stream.readLine();
			lineCount++;
		}
		int dataRows = lineCount - unusedLines - 2;
		
		file.seek(0);
		for (int i = 0; i < unusedLines; ++i) {
			stream.readLine();
		}
		QStringList names = stream.readLine().split("\t");
		QStringList units = stream.readLine().split("\t");
		for (int i = 0; i < names.size(); ++i) {
			m_fastResults.append(new FASTResult(names[i], units[i].mid(1, units[i].length()-2), dataRows));
			m_availableVariablesFastGraph.append(names[i]);
		}
		for (int row = 0; row < dataRows; ++row) {
			for (int column = 0; column < names.size(); ++column) {
				stream >> m_fastResults[column]->data[row];
			}
		}
	} else {
		throw QString(tr("Could not read file: ") + file.fileName());           
	}
	
	/**** AeroDyn ****/
	QFile file2 (fileToRead.replace(fileToRead.length()-3, 3, "elm"));
	if (file2.exists()) {  // if no AeroDyn output was demanded, this file does not exist
		if (file2.open(QIODevice::ReadOnly | QIODevice::Text)) {  // read AeroDyn output file
			int unusedLines = 1;
			int dataRows = 0;
			QTextStream stream (&file2);
			for (int i = 0; i < unusedLines + 2; ++i) {
				stream.readLine();
			}
			
			float time, lastTime = -1;
			while (!stream.atEnd()) {  // count lines
				stream >> time;
				stream.readLine();
				if (time != lastTime) {  // lines may be repeated if timesteps for aeroDyn and FAST are not the same
					dataRows++;
				}
				lastTime = time;
			}
			
			file2.seek(0);
			for (int i = 0; i < unusedLines; ++i) {
				stream.readLine();
			}
			QStringList names = stream.readLine().split("\t");
			QStringList units = stream.readLine().split("\t");
			m_aeroDynResults.append(new FASTResult(names[0], units[0].mid(1, units[0].length()-2), dataRows));  // time
			m_aeroDynResults.append(new FASTResult(names[1], units[1].mid(1, units[1].length()-2), dataRows));  // VX
			m_availableVariablesFastGraph.append("VX (at Tip)");
			m_aeroDynResults.append(new FASTResult(names[2], units[2].mid(1, units[2].length()-2), dataRows));  // VY
			m_availableVariablesFastGraph.append("VY (at Tip)");
			m_aeroDynResults.append(new FASTResult(names[3], units[3].mid(1, units[3].length()-2), dataRows));  // VZ
			m_availableVariablesFastGraph.append("VZ (at Tip)");
			m_availableVariablesBladeGraph.append("Radius");
			m_availableVariablesBladeGraph.append("Radius (relative)");
			QString name;
			for (int i = 4; i < names.size(); ++i) {  // leave out time and the three V
				m_aeroDynResults.append(new FASTResult(names[i], units[i].mid(1, units[i].length()-2), dataRows));
				name = names[i];
				name.chop(2);
				if (! m_availableVariablesFastGraph.contains(QString(name + " (at section)"))) {
					m_availableVariablesFastGraph.append(QString(name + " (at section)"));
					m_availableVariablesBladeGraph.append(name);
				}
			}
			int row = 0;
			while (row < dataRows) {
				stream >> m_aeroDynResults[0]->data[row];
				if (row == 0 || m_aeroDynResults[0]->data[row] != m_aeroDynResults[0]->data[row-1]) {  // only the non-repeating
					for (int column = 1; column < names.size(); ++column) {  // leave out column 0, the time
						stream >> m_aeroDynResults[column]->data[row];
					}
					row++;
				} else {
					stream.readLine();
				}
			}
		} else {
			throw QString(tr("Could not read file: ") + file2.fileName());           
		}
	}
}

NewCurve* FASTSimulation::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType graphType) {
	NewCurve *curve;
	
	switch (graphType) {
	case NewGraph::FastSimulation:
	{
        const bool rotorMatches = true; /*TODO DAVID TEST (g_fastModule->getShownFASTSimulation() &&
                                   g_fastModule->getShownFASTSimulation()->getUsedRotor() == m_usedRotor);*/
		int xAxisIndex = m_availableVariablesFastGraph.indexOf(xAxis);
		int yAxisIndex = m_availableVariablesFastGraph.indexOf(yAxis);
		const bool xAxisIsAeroDyn = xAxisIndex >= m_fastResults.size();
		const bool yAxisIsAeroDyn = yAxisIndex >= m_fastResults.size();
		
		if (xAxisIsAeroDyn) {
			if (xAxisIndex - m_fastResults.size() <= 2) {  // == 0: VX; == 1: VY; == 2: VZ
				xAxisIndex = xAxisIndex - m_fastResults.size() + 1;  // AeroDyn time is in results but not in availableVaribles
			} else if (rotorMatches) {
				xAxisIndex = -1;
				for (int i = 4; i < m_aeroDynResults.size(); ++i) {
					if (m_aeroDynResults.at(i)->defaultName.contains(xAxis.split(' ').at(0)) && m_aeroDynResults.at(i)->defaultName.endsWith(QString("%1").arg(m_shownBladeSection, 2, 10, QChar('0')))) {
						xAxisIndex = i;
						break;
					}
				}
			}
		}
			
		if (yAxisIsAeroDyn) {
			if (yAxisIndex - m_fastResults.size() <= 2) {  // == 0: VX; == 1: VY; == 2: VZ
				yAxisIndex = yAxisIndex - m_fastResults.size() + 1;  // AeroDyn time is in results but not in availableVaribles
			} else if (rotorMatches) {
				yAxisIndex = -1;
				for (int i = 4; i < m_aeroDynResults.size(); ++i) {
					if (m_aeroDynResults.at(i)->defaultName.contains(yAxis.split(' ').at(0)) && m_aeroDynResults.at(i)->defaultName.endsWith(QString("%1").arg(m_shownBladeSection, 2, 10, QChar('0')))) {
						yAxisIndex = i;
						break;
					}
				}
			}
		}
		
		if (xAxisIndex == -1 || yAxisIndex == -1) {
			curve = NULL;
        } else if (!rotorMatches && ((xAxisIsAeroDyn && xAxisIndex > 3) || (yAxisIsAeroDyn && yAxisIndex > 3))) {
			curve = NULL;
		} else if (xAxisIsAeroDyn && yAxisIsAeroDyn) {  // both aeroDyn
			curve = new NewCurve (this);
	//		qDebug() << m_aeroDynResults[xAxisIndex]->defaultName << m_aeroDynResults[yAxisIndex]->defaultName;
			curve->setAllPoints(m_aeroDynResults[xAxisIndex]->data,
								m_aeroDynResults[yAxisIndex]->data,
								m_aeroDynResults[0]->numberOfRows);  // numberOfRows is the same for all aero results
		} else if (!xAxisIsAeroDyn && !yAxisIsAeroDyn) {  // both are FAST
			curve = new NewCurve (this);
			curve->setAllPoints(m_fastResults[xAxisIndex]->data,
								m_fastResults[yAxisIndex]->data,
								m_fastResults[0]->numberOfRows);
		} else {  // mix
	//		qDebug() << m_fastResults[xAxisIndex]->defaultName << m_aeroDynResults[yAxisIndex]->defaultName;
			const float *xData = (xAxisIsAeroDyn ? m_aeroDynResults[xAxisIndex]->data : m_fastResults[xAxisIndex]->data);
			const float *yData = (yAxisIsAeroDyn ? m_aeroDynResults[yAxisIndex]->data : m_fastResults[yAxisIndex]->data);
			int aeroStep = 0;
			curve = new NewCurve (this);
			for (int i = 0; i < m_fastResults[0]->numberOfRows; ++i) {
				if (m_aeroDynResults[0]->data[aeroStep+1] < m_fastResults[0]->data[i]) {
					aeroStep++;
				}
				curve->addPoint(xData[xAxisIsAeroDyn?aeroStep:i], yData[xAxisIsAeroDyn?i:aeroStep]);
			}
		}
		break;
	}
	case NewGraph::BladeGraph:
	{
		const int xAxisIndex = m_availableVariablesBladeGraph.indexOf(xAxis);
		const int yAxisIndex = m_availableVariablesBladeGraph.indexOf(yAxis);
		
		if (xAxisIndex == -1 || yAxisIndex == -1) {
			curve = NULL;
		} else {
			QVector<int> aeroDynResultSections;
			QVector<float> radius, radiusRelative;
			for (int i = 0; i < m_aeroDynOutput.size(); ++i) {
				if (m_aeroDynOutput[i]) {
					aeroDynResultSections.append(i);
					radius.append(m_usedRotor->getFASTRadiusAt(i));
					radiusRelative.append(m_usedRotor->getFASTRadiusAt(i) / m_usedRotor->getRotorRadius());
				}
			}
			
			QVector<int> xAxisResultIndizes, yAxisResultIndizes;
			for (int i = 0; i < m_aeroDynResults.size(); ++i) {
				if (m_aeroDynResults[i]->defaultName.contains(xAxis)) {
					xAxisResultIndizes.append(i);
				}
				if (m_aeroDynResults[i]->defaultName.contains(yAxis)) {
					yAxisResultIndizes.append(i);
				}
			}
			
			/* performance shouldn't be an issue, because there are only a few points */
			float xValue, yValue;
			curve = new NewCurve (this);
			for (int i = 0; i < aeroDynResultSections.size(); ++i) {
				switch (xAxisIndex) {
				case 0: xValue = radius[i]; break;
				case 1: xValue = radiusRelative[i]; break;
				default: xValue = m_aeroDynResults[xAxisResultIndizes[i]]->data[m_shownTimeIndex];
				}
				switch (yAxisIndex) {
				case 0: yValue = radius[i]; break;
				case 1: yValue = radiusRelative[i]; break;
				default: yValue = m_aeroDynResults[yAxisResultIndizes[i]]->data[m_shownTimeIndex];
				}
				
				curve->addPoint(xValue, yValue);
			}
		}
		break;
	}
	default:
		curve = NULL;
	}
	
	return curve;
}

QStringList FASTSimulation::getAvailableVariables(NewGraph::GraphType graphType) {
	switch (graphType) {
	case NewGraph::FastSimulation:
		return m_availableVariablesFastGraph;
	case NewGraph::BladeGraph:
		return m_availableVariablesBladeGraph;
	default:
		return QStringList();
	}
}

void FASTSimulation::setShownTime(float shownTime) {

    ////TODO NICO WHAT IS PAINTED IF SHOWNTIME OUT OF RANGE???
	if (hasAeroDynResults()) {
		int timeIndex = (shownTime - m_tStart) / m_aeroTimeStep;
		
		if (m_aeroDynResults[0]->data[timeIndex] == shownTime) {
			m_shownTimeIndex = timeIndex;
			return;
		}
		
		while (m_aeroDynResults[0]->data[timeIndex] < shownTime && timeIndex < m_aeroDynResults[0]->numberOfRows-1) {
			timeIndex++;
			if (m_aeroDynResults[0]->data[timeIndex] == shownTime) {
				m_shownTimeIndex = timeIndex;
				return;					
			} else if (m_aeroDynResults[0]->data[timeIndex] > shownTime) {
				return;  // searched from lower to higher value but didn't find a matching one
			}
		}
		
		while (m_aeroDynResults[0]->data[timeIndex] > shownTime && timeIndex > 0) {
			timeIndex--;
			if (m_aeroDynResults[0]->data[timeIndex] == shownTime) {
				m_shownTimeIndex = timeIndex;
				return;					
			} else if (m_aeroDynResults[0]->data[timeIndex] < shownTime) {
				return;  // searched from higher to lower value but didn't find a matching one
			}
		}		
	}
}
