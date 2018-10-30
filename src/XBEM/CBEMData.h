#ifndef CBEMDATA_H
#define CBEMDATA_H

#include <QList>
#include <QString>
#include <QColor>
#include "BData.h"

class CBEMData : public StorableObject
{
	friend class QBEM;
    friend class MainFrame;

public:
	static CBEMData* newBySerialize ();
    CBEMData();
    virtual ~CBEMData();
	static QStringList prepareMissingObjectMessage();

public:
    void Compute(BData *pBData, CBlade *pWing, double lambda, double pitch, double windspeed);
	void serialize ();  // override from StorableObject
    void initArrays(int wtimes, int rtimes, int ptimes);
    void DeleteArrays();


private:
    QString m_WingName;
    QString m_SimName;

//    double m_P [100][100][100];                     //power
//    double m_S [100][100][100];                     //thrust
//    double m_V [100][100][100];                     //wind speed
//    double m_Omega [100][100][100];                 //rotational speed
//    double m_Lambda [100][100][100];                //tip-speed ratio
//    double m_Cp [100][100][100];                    //power coefficient
//    double m_Ct [100][100][100];                    //thrust coefficient
//    double m_Cm [100][100][100];                    //moment coefficient
//    double m_Pitch [100][100][100];                 //pitch angle
//    double m_Bending [100][100][100];               //bending moment
//    double m_Kp [100][100][100];                    //dimensionless
//    double m_one_over_Lambda [100][100][100];       //1/tip speed ratio
//    double m_Torque [100][100][100];                //rotor torque

    float*** m_P;
    float*** m_S;
    float*** m_V;
    float*** m_Omega;
    float*** m_Lambda;
    float*** m_Cp;
    float*** m_Ct;
    float*** m_Cm;
    float*** m_Pitch;
    float*** m_Bending;
    float*** m_Kp;
    float*** m_one_over_Lambda;
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
    double elements;
    double epsilon;
    double iterations;
    double relax;
    bool m_bTipLoss;
    bool m_bRootLoss;
    bool m_b3DCorrection;
    bool m_bInterpolation;
    bool m_bNewTipLoss;
    bool m_bCdReynolds;
    bool m_bNewRootLoss;
    double rho;
    double visc;
};

#endif // CBEMDATA_H
