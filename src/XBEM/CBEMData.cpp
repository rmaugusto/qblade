#include "CBEMData.h"
#include <QString>
#include <QList>
#include "QDebug"

#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"
#include "../MainFrame.h"


CBEMData* CBEMData::newBySerialize() {
	CBEMData* cBemData = new CBEMData ();
	cBemData->serialize();
	return cBemData;
}

CBEMData::CBEMData()
	: StorableObject("name")
{
    m_bShowPoints   =   false;
    m_bIsVisible    =   true;
    m_Style        =   0;
    m_Width        =   1;

    elements = 100;
    epsilon = 0.001;
    iterations = 1000;
    relax = 0.3;
    m_bTipLoss = false;
    m_bRootLoss = false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
    m_bNewTipLoss = false;
    m_bNewRootLoss = false;
    simulated = false;

    windtimes = 0;
    pitchtimes = 0;
    rottimes = 0;

    visc = 0.0000178;
    rho = 1.2;
}

CBEMData::~CBEMData()
{

DeleteArrays();

}

QStringList CBEMData::prepareMissingObjectMessage() {
	if (g_cbemdataStore.isEmpty()) {
		QStringList message = CBlade::prepareMissingObjectMessage(false);
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == BEM && g_mainFrame->m_iView == CHARSIMVIEW) {
				message = QStringList(">>> Click 'Define Simulation' to create a new Multi Parameter Simulation");
			} else {
				message = QStringList(">>> unknown hint");
			}
		}
		message.prepend("- No Multi Parameter BEM Simulation in Database");
		return message;
	} else {
		return QStringList();
	}
}

void CBEMData::DeleteArrays()
{
    if(pitchtimes == 0) return;

    for (int z = 0; z < windtimes; ++z) {
        for (int y = 0; y < rottimes; ++y) {
            delete [] m_P[z][y];
            delete [] m_S[z][y];
            delete [] m_V[z][y];
            delete [] m_Omega[z][y];
            delete [] m_Lambda[z][y];
            delete [] m_Cp[z][y];
            delete [] m_Ct[z][y];
            delete [] m_Cm[z][y];
            delete [] m_Pitch[z][y];
            delete [] m_Bending[z][y];
            delete [] m_Kp[z][y];
            delete [] m_one_over_Lambda[z][y];
            delete [] m_Torque[z][y];

        }
        delete [] m_P[z];
        delete [] m_S[z];
        delete [] m_V[z];
        delete [] m_Omega[z];
        delete [] m_Lambda[z];
        delete [] m_Cp[z];
        delete [] m_Ct[z];
        delete [] m_Cm[z];
        delete [] m_Pitch[z];
        delete [] m_Bending[z];
        delete [] m_Kp[z];
        delete [] m_one_over_Lambda[z];
        delete [] m_Torque[z];

    }

    delete [] m_P;
    delete [] m_S;
    delete [] m_V;
    delete [] m_Omega;
    delete [] m_Lambda;
    delete [] m_Cp;
    delete [] m_Ct;
    delete [] m_Cm;
    delete [] m_Pitch;
    delete [] m_Bending;
    delete [] m_Kp;
    delete [] m_one_over_Lambda;
    delete [] m_Torque;
}


void CBEMData::Compute(BData *pBData, CBlade *pWing, double lambda, double pitch, double windspeed)
{
    pBData->elements = elements;
    pBData->epsilon = epsilon;
    pBData->iterations = iterations;
    pBData->m_bTipLoss = m_bTipLoss;
    pBData->m_bRootLoss = m_bRootLoss;
    pBData->m_b3DCorrection = m_b3DCorrection;
    pBData->m_bInterpolation = m_bInterpolation;
    pBData->relax = relax;
    pBData->rho = rho;
    pBData->visc = visc;
    pBData->m_bNewRootLoss = m_bNewRootLoss;
    pBData->m_bNewTipLoss = m_bNewTipLoss;
    pBData->m_bCdReynolds = m_bCdReynolds;

    pBData->Init(pWing,lambda);

    pBData->OnBEM(pitch, pWing, windspeed);
}

void CBEMData::initArrays(int wtimes, int rtimes, int ptimes)
{
    m_P = new float**[wtimes];
    m_S = new float**[wtimes];
    m_V = new float**[wtimes];
    m_Omega = new float**[wtimes];
    m_Lambda = new float**[wtimes];
    m_Cp = new float**[wtimes];
    m_Ct = new float**[wtimes];
    m_Cm = new float**[wtimes];
    m_Pitch = new float**[wtimes];
    m_Bending = new float**[wtimes];
    m_Kp = new float**[wtimes];
    m_one_over_Lambda = new float**[wtimes];
    m_Torque = new float**[wtimes];

    for (int z = 0; z < wtimes; ++z) {
        m_P[z] = new float*[rtimes];
        m_S[z] = new float*[rtimes];
        m_V[z] = new float*[rtimes];
        m_Omega[z] = new float*[rtimes];
        m_Lambda[z] = new float*[rtimes];
        m_Cp[z] = new float*[rtimes];
        m_Ct[z] = new float*[rtimes];
        m_Cm[z] = new float*[rtimes];
        m_Pitch[z] = new float*[rtimes];
        m_Bending[z] = new float*[rtimes];
        m_Kp[z] = new float*[rtimes];
        m_one_over_Lambda[z] = new float*[rtimes];
        m_Torque[z] = new float*[rtimes];

        for (int y = 0; y < rtimes; ++y) {
            // empty paranthesis initialize the whole new array with 0
            m_P[z][y] = new float[ptimes] ();
            m_S[z][y] = new float[ptimes] ();
            m_V[z][y] = new float[ptimes] ();
            m_Omega[z][y] = new float[ptimes] ();
            m_Lambda[z][y] = new float[ptimes] ();
            m_Cp[z][y] = new float[ptimes] ();
            m_Ct[z][y] = new float[ptimes] ();
            m_Cm[z][y] = new float[ptimes] ();
            m_Pitch[z][y] = new float[ptimes] ();
            m_Bending[z][y] = new float[ptimes] ();
            m_Kp[z][y] = new float[ptimes] ();
            m_one_over_Lambda[z][y] = new float[ptimes] ();
            m_Torque[z][y] = new float[ptimes] ();
        }
    }

}

void CBEMData::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_SimName);

    g_serializer.readOrWriteInt (&windtimes);
    g_serializer.readOrWriteInt (&pitchtimes);
    g_serializer.readOrWriteInt (&rottimes);
	    

    g_serializer.readOrWriteFloatArray3D (&m_P, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_S, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_V, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Omega, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Lambda, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cp, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Ct, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cm, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Pitch, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Bending, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Kp, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_one_over_Lambda, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Torque, windtimes,rottimes,pitchtimes);
	
	g_serializer.readOrWriteBool (&simulated);
	
	g_serializer.readOrWriteDouble (&windstart);
	g_serializer.readOrWriteDouble (&windend);
	g_serializer.readOrWriteDouble (&winddelta);
	g_serializer.readOrWriteDouble (&rotstart);
	g_serializer.readOrWriteDouble (&rotend);
	g_serializer.readOrWriteDouble (&rotdelta);
	g_serializer.readOrWriteDouble (&pitchstart);
	g_serializer.readOrWriteDouble (&pitchend);
	g_serializer.readOrWriteDouble (&pitchdelta);
	
	g_serializer.readOrWriteBool (&m_bShowPoints);
	g_serializer.readOrWriteBool (&m_bIsVisible);
	g_serializer.readOrWriteInt (&m_Style);
	g_serializer.readOrWriteInt (&m_Width);
	g_serializer.readOrWriteColor (&m_Color);
	
	g_serializer.readOrWriteDouble (&elements);
	g_serializer.readOrWriteDouble (&epsilon);
	g_serializer.readOrWriteDouble (&iterations);
	g_serializer.readOrWriteDouble (&relax);
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bRootLoss);
	g_serializer.readOrWriteBool (&m_b3DCorrection);
	g_serializer.readOrWriteBool (&m_bInterpolation);
	g_serializer.readOrWriteBool (&m_bNewTipLoss);
	g_serializer.readOrWriteBool (&m_bCdReynolds);
	g_serializer.readOrWriteBool (&m_bNewRootLoss);
	g_serializer.readOrWriteDouble (&rho);
	g_serializer.readOrWriteDouble (&visc);
}
