#ifndef NOISEOPPOINT_H
#define NOISEOPPOINT_H

#include <QString>
//#include "../Objects/Polar.h"
#include "../Objects/OpPoint.h"

/**
 * @brief The NoiseOpPoint class
 * This class is a simplified representation of OpPoint
 */
class NoiseOpPoint
{
public:
    NoiseOpPoint(OpPoint * op) : m_OpPoint(op){}
    NoiseOpPoint(double reynolds,double alpha);
    NoiseOpPoint();
    ~NoiseOpPoint();

    double Reynolds();
    //void setReynolds(double Reynolds);

    double Alpha();
    //void setAlpha(double Alpha);
    //void setAlphaDeg(double Alpha);


    double AlphaDeg();

    QString AirfoilName();
    //void setAirfoilName(QString AirfoilName);

    QString PolarName();
    //void setPolarName(QString PolarName);

private:
    OpPoint * m_OpPoint;
    bool internalOpPoint=false;
};

Q_DECLARE_METATYPE(NoiseOpPoint*)

#endif // NOISEOPPOINT_H
