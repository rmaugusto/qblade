#ifndef CDMSDATA_H
#define CDMSDATA_H

#include <QList>
#include <QString>
#include <QColor>
#include "DData.h"

class CDMSData : public StorableObject
{
	friend class QBEM;
	friend class QDMS;
    friend class MainFrame;

public:
	static CDMSData* newBySerialize ();
	CDMSData();
	virtual ~CDMSData();

public:
	void Compute(DData *pDData, CBlade *pWing, double lambda, double pitch, double windspeed);
    void Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat);
	void serialize();  // override from StorableObject
    void initArrays(int wtimes, int rtimes, int ptimes);
    void DeleteArrays();



private:
    QString m_WingName;
    QString m_SimName;

//	double m_Lambda [100][100][100];                //tip-speed ratio
//	double m_one_over_Lambda [100][100][100];       //1/tip speed ratio

//    double m_V [100][100][100];                     //wind speed
//	double m_w [100][100][100];						//rotational speed
//	double m_Pitch [100][100][100];                 //pitch angle

//	double m_Cp [100][100][100];                    //power coefficient
//	double m_Cp1 [100][100][100];                   //power coefficient
//	double m_Cp2 [100][100][100];                   //power coefficient
//	double m_Kp [100][100][100];                    //dimensionless

//	double m_Cm [100][100][100];                    //torque coefficient
//	double m_Cm1 [100][100][100];                   //torque coefficient
//	double m_Cm2 [100][100][100];                   //torque coefficient

//	double m_P [100][100][100];                     //power
//    double m_Torque [100][100][100];                //rotor torque

    float*** m_Lambda;
    float*** m_one_over_Lambda;
    float*** m_V;
    float*** m_w;
    float*** m_Pitch;
    float*** m_Cp;
    float*** m_Cp1;
    float*** m_Cp2;
    float*** m_Kp;
    float*** m_Cm;
    float*** m_Cm1;
    float*** m_Cm2;
    float*** m_P;
    float*** m_Torque;

    bool simulated;

    int windtimes;
    int pitchtimes;
    int rottimes;

    double windstart;
    double windend;
    double winddelta;

    double rotstart;
    double rotend;
    double rotdelta;

    double pitchstart;
    double pitchend;
    double pitchdelta;

    bool m_bShowPoints;
    bool m_bIsVisible;
    int m_Style;
    int m_Width;
    QColor m_Color;

	//simulation parameters//
	double rho;
	double elements;
	double epsilon;
	double iterations;
	double relax;
	double visc;
	bool m_bPowerLaw;
	bool m_bConstant;
	bool m_bLogarithmic;
	double exponent;
	double roughness;
	bool m_bTipLoss;
	bool m_bAspectRatio;
	bool m_bVariable;
};

#endif // CDMSDATA_H
