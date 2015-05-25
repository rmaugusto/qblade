#ifndef NOISEOPPOINT_H
#define NOISEOPPOINT_H

#include <QString>

/**
 * @brief The NoiseOpPoint class
 * This class is a simplified representation of OpPoint
 */
class NoiseOpPoint
{
public:
    NoiseOpPoint(double reynolds,double alpha) : m_Reynolds(reynolds),m_Alpha(alpha) {}
    NoiseOpPoint();
    ~NoiseOpPoint();

    double Reynolds() const;
    void setReynolds(double Reynolds);

    double Alpha() const;
    void setAlpha(double Alpha);
    void setAlphaDeg(double Alpha);


    double AlphaDeg();

    QString AirfoilName() const;
    void setAirfoilName(QString AirfoilName);

    QString PolarName() const;
    void setPolarName(QString PolarName);

private:
    double m_Reynolds;
    //Alpha in radians
    double m_Alpha;
    QString m_AirfoilName;
    QString m_PolarName;

};

#endif // NOISEOPPOINT_H
