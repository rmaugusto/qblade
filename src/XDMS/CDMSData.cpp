#include "CDMSData.h"
#include "../Objects/Wing.h"
#include <QString>
#include <QList>
//#include <qDebug>//debug JW

#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"


CDMSData *CDMSData::newBySerialize() {
	CDMSData* cDmsData = new CDMSData ();
	cDmsData->serialize();
	return cDmsData;
}

CDMSData::CDMSData()
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
	m_bAspectRatio = false;
	m_bVariable = false;
	m_bPowerLaw = false;
	m_bConstant = true;
	m_bLogarithmic = false;

    windtimes = 0;
    pitchtimes = 0;
    rottimes = 0;
	/*
    m_bRootLoss = false;
    m_b3DCorrection = false;
    m_bInterpolation = false;
    m_bNewTipLoss = false;
    m_bNewRootLoss = false;
	*/
    simulated = false;

    visc = 0.0000178;
    rho = 1.2;


}

CDMSData::~CDMSData()
{  

DeleteArrays();

}
void CDMSData::DeleteArrays()
{
    if (pitchtimes == 0) return;
    for (int z = 0; z < windtimes; ++z) {
        for (int y = 0; y < rottimes; ++y) {
            delete [] m_Lambda[z][y];
            delete [] m_one_over_Lambda[z][y];
            delete [] m_V[z][y];
            delete [] m_w[z][y];
            delete [] m_Pitch[z][y];
            delete [] m_Cp[z][y];
            delete [] m_Cp1[z][y];
            delete [] m_Cp2[z][y];
            delete [] m_Kp[z][y];
            delete [] m_Cm[z][y];
            delete [] m_Cm1[z][y];
            delete [] m_Cm2[z][y];
            delete [] m_P[z][y];
            delete [] m_Torque[z][y];

        }
        delete [] m_Lambda[z];
        delete [] m_one_over_Lambda[z];
        delete [] m_V[z];
        delete [] m_w[z];
        delete [] m_Pitch[z];
        delete [] m_Cp[z];
        delete [] m_Cp1[z];
        delete [] m_Cp2[z];
        delete [] m_Kp[z];
        delete [] m_Cm[z];
        delete [] m_Cm1[z];
        delete [] m_Cm2[z];
        delete [] m_P[z];
        delete [] m_Torque[z];

    }
    delete [] m_Lambda;
    delete [] m_one_over_Lambda;
    delete [] m_V;
    delete [] m_w;
    delete [] m_Pitch;
    delete [] m_Cp;
    delete [] m_Cp1;
    delete [] m_Cp2;
    delete [] m_Kp;
    delete [] m_Cm;
    delete [] m_Cm1;
    delete [] m_Cm2;
    delete [] m_P;
    delete [] m_Torque;
}

void CDMSData::initArrays(int wtimes, int rtimes, int ptimes)
{
    m_Lambda = new float**[wtimes];
    m_one_over_Lambda = new float**[wtimes];
    m_V = new float**[wtimes];
    m_w = new float**[wtimes];
    m_Pitch = new float**[wtimes];
    m_Cp = new float**[wtimes];
    m_Cp1 = new float**[wtimes];
    m_Cp2 = new float**[wtimes];
    m_Kp = new float**[wtimes];
    m_Cm = new float**[wtimes];
    m_Cm1 = new float**[wtimes];
    m_Cm2 = new float**[wtimes];
    m_P = new float**[wtimes];
    m_Torque = new float**[wtimes];


    for (int z = 0; z < wtimes; ++z) {
        m_Lambda[z] = new float*[rtimes];
        m_one_over_Lambda[z] = new float*[rtimes];
        m_V[z] = new float*[rtimes];
        m_w[z] = new float*[rtimes];
        m_Pitch[z] = new float*[rtimes];
        m_Cp[z] = new float*[rtimes];
        m_Cp1[z] = new float*[rtimes];
        m_Cp2[z] = new float*[rtimes];
        m_Kp[z] = new float*[rtimes];
        m_Cm[z] = new float*[rtimes];
        m_Cm1[z] = new float*[rtimes];
        m_Cm2[z] = new float*[rtimes];
        m_P[z] = new float*[rtimes];
        m_Torque[z] = new float*[rtimes];

        for (int y = 0; y < rtimes; ++y) {
            // empty paranthesis initialize the whole new array with 0
            m_Lambda[z][y] = new float[ptimes] ();
            m_one_over_Lambda[z][y] = new float[ptimes] ();
            m_V[z][y] = new float[ptimes] ();
            m_w[z][y] = new float[ptimes] ();
            m_Pitch[z][y] = new float[ptimes] ();
            m_Cp[z][y] = new float[ptimes] ();
            m_Cp1[z][y] = new float[ptimes] ();
            m_Cp2[z][y] = new float[ptimes] ();
            m_Kp[z][y] = new float[ptimes] ();
            m_Cm[z][y] = new float[ptimes] ();
            m_Cm1[z][y] = new float[ptimes] ();
            m_Cm2[z][y] = new float[ptimes] ();
            m_P[z][y] = new float[ptimes] ();
            m_Torque[z][y] = new float[ptimes] ();
        }
    }

}

void CDMSData::Compute(DData *pDData, CBlade *pWing, double lambda, double pitch, double windspeed)
{

	pDData->elements = elements;
	pDData->epsilon = epsilon;
	pDData->iterations = iterations;
	pDData->relax = relax;
	pDData->rho = rho;
	pDData->visc = visc;
	pDData->m_bTipLoss = m_bTipLoss;
	pDData->m_bAspectRatio = m_bAspectRatio;
	pDData->m_bVariable = m_bVariable;
	pDData->bPowerLaw = m_bPowerLaw;
	pDData->bConstant = m_bConstant;
	pDData->bLogarithmic = m_bLogarithmic;
	pDData->exponent = exponent;
	pDData->roughness = roughness;

	pDData->Toff = 0;
	pDData->windspeed = windspeed;

	pDData->Init(pWing,lambda,pitch);
	pDData->OnDMS();


}


void CDMSData::Serialize(QDataStream &ar, bool bIsStoring, int /*ArchiveFormat*/)
{
    int i,j,k,w,r,p;
    float f;

    if (bIsStoring)
    {
        w = (int) windtimes;
        r = (int) rottimes;
        p = (int) pitchtimes;

        ar << (float)windtimes;
        ar << (float)pitchtimes;
        ar << (float)rottimes;
        ar << (float)windstart;
        ar << (float)windend;
        ar << (float)winddelta;
        ar << (float)rotstart;
        ar << (float)rotend;
        ar << (float)rotdelta;
        ar << (float)pitchstart;
        ar << (float)pitchend;
        ar << (float)pitchdelta;

        ar << (int) m_Style;
        ar << (int) m_Width;
        ar << (float) elements;
        ar << (float) rho;
        ar << (float) epsilon;
        ar << (float) iterations;
        ar << (float) relax;
        ar << (float) visc;
		ar << (float) exponent;
		ar << (float) roughness;

		if (simulated) ar << 1; else ar<<0;
		if (m_bTipLoss) ar << 1; else ar<<0;
		if (m_bAspectRatio) ar << 1; else ar<<0;;
		if (m_bVariable) ar << 1; else ar<<0;
		if (m_bConstant) ar << 1; else ar<<0;
		if (m_bPowerLaw) ar << 1; else ar<<0;
		if (m_bLogarithmic) ar << 1; else ar<<0;

        WriteCOLORREF(ar,m_Color);
        WriteCString(ar, m_WingName);
        WriteCString(ar, m_SimName);
        ar << (int) w << (int) r << (int) p;
        for (i=0;i<w;i++)
        {
            for (j=0;j<r;j++)
            {
                for (k=0;k<p;k++)
                {
					ar << (float) m_P[i][j][k] << (float) m_V[i][j][k] << (float) m_w[i][j][k] << (float) m_Lambda[i][j][k] << (float) m_Cp[i][j][k] << (float) m_Cp1[i][j][k] << (float) m_Cp2[i][j][k] << (float) m_Cm[i][j][k] << (float) m_Cm1[i][j][k] << (float) m_Cm2[i][j][k] << (float) m_Pitch[i][j][k] << (float) m_Kp[i][j][k] << (float) m_one_over_Lambda[i][j][k] << (float) m_Torque[i][j][k];
                }
            }
        }

    }
    else
    {
        ar >> f;
        windtimes = f;
        ar >> f;
        pitchtimes = f;
        ar >> f;
        rottimes = f;
        ar >> f;
        windstart = f;
        ar >> f;
        windend = f;
        ar >> f;
        winddelta = f;
        ar >> f;
        rotstart = f;
        ar >> f;
        rotend = f;
        ar >> f;
        rotdelta = f;
        ar >> f;
        pitchstart = f;
        ar >> f;
        pitchend = f;
        ar >> f;
        pitchdelta = f;



        ar >> j;
        m_Style = j;
        ar >> j;
        m_Width = j;
        ar >> f;
        elements = f;
        ar >> f;
        rho = f;
        ar >> f;
        epsilon = f;
        ar >> f;
        iterations = f;
        ar >> f;
        relax = f;
        ar >> f;
        visc = f;
		ar >> f;
		exponent = f;
		ar >> f;
		roughness = f;

        ar >> f;
		if (f) simulated = true; else simulated = false;
		ar >> f;
		if (f) m_bTipLoss = true; else m_bTipLoss = false;
		ar >> f;
		if (f) m_bAspectRatio = true; else m_bAspectRatio = false;
		ar >> f;
		if (f) m_bVariable = true; else m_bVariable = false;
		ar >> f;
		if (f) m_bConstant = true; else m_bConstant = false;
		ar >> f;
		if (f) m_bPowerLaw = true; else m_bPowerLaw = false;
		ar >> f;
		if (f) m_bLogarithmic = true; else m_bLogarithmic = false;

        ReadCOLORREF(ar,m_Color);
        ReadCString(ar,m_WingName);
//		setParentName(m_WingName);  // NM REPLACE
		setSingleParent(g_verticalRotorStore.getObjectByNameOnly(m_WingName));  // only for backwards compatibility
        ReadCString(ar,m_SimName);
		setName(m_SimName);

        ar >> w;
        ar >> r;
        ar >> p;

        initArrays(w,r,p);

        for (i=0;i<w;i++)
        {
            for (j=0;j<r;j++)
            {
                for (k=0;k<p;k++)
				{
                    ar >> f;
					m_P[i][j][k] = f;
					ar >> f;
                    m_V[i][j][k] = f;
                    ar >> f;
					m_w[i][j][k] = f;
                    ar >> f;
                    m_Lambda[i][j][k] = f;
					ar >> f;
					m_Cp[i][j][k] = f;
					ar >> f;
					m_Cp1[i][j][k] = f;
					ar >> f;
					m_Cp2[i][j][k] = f;
					ar >> f;
					m_Cm[i][j][k] = f;
					ar >> f;
					m_Cm1[i][j][k] = f;
					ar >> f;
					m_Cm2[i][j][k] = f;
					ar >> f;
                    m_Pitch[i][j][k] = f;
					ar >> f;
                    m_Kp[i][j][k] = f;
                    ar >> f;
                    m_one_over_Lambda[i][j][k] = f;
                    ar >> f;
					m_Torque[i][j][k] = f;
                }
            }

        }
	}
}

void CDMSData::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteString (&m_SimName);

    g_serializer.readOrWriteInt (&windtimes);
    g_serializer.readOrWriteInt (&pitchtimes);
    g_serializer.readOrWriteInt (&rottimes);

    g_serializer.readOrWriteFloatArray3D (&m_Lambda, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_one_over_Lambda, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_V, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_w, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Pitch, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cp, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cp1, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cp2, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Kp, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cm, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cm1, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_Cm2, windtimes,rottimes,pitchtimes);
    g_serializer.readOrWriteFloatArray3D (&m_P, windtimes,rottimes,pitchtimes);
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
	
	g_serializer.readOrWriteDouble (&rho);
	g_serializer.readOrWriteDouble (&elements);
	g_serializer.readOrWriteDouble (&epsilon);
	g_serializer.readOrWriteDouble (&iterations);
	g_serializer.readOrWriteDouble (&relax);
	g_serializer.readOrWriteDouble (&visc);
	g_serializer.readOrWriteBool (&m_bPowerLaw);
	g_serializer.readOrWriteBool (&m_bConstant);
	g_serializer.readOrWriteBool (&m_bLogarithmic);
	g_serializer.readOrWriteDouble (&exponent);
	g_serializer.readOrWriteDouble (&roughness);
	g_serializer.readOrWriteBool (&m_bTipLoss);
	g_serializer.readOrWriteBool (&m_bAspectRatio);
	g_serializer.readOrWriteBool (&m_bVariable);
}
