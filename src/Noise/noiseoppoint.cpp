#include "noiseoppoint.h"
#include <math.h>

NoiseOpPoint::NoiseOpPoint()
{

}

NoiseOpPoint::~NoiseOpPoint()
{

}
double NoiseOpPoint::Reynolds() const
{
    return m_Reynolds;
}

void NoiseOpPoint::setReynolds(double Reynolds)
{
    m_Reynolds = Reynolds;
}
double NoiseOpPoint::Alpha() const
{
    return m_Alpha;
}

void NoiseOpPoint::setAlpha(double Alpha)
{
    m_Alpha = Alpha;
}

void NoiseOpPoint::setAlphaDeg(double Alpha)
{
    m_Alpha = Alpha / 180 * M_PI;
}

double NoiseOpPoint::AlphaDeg()
{
    return m_Alpha * 180 / M_PI;
}
QString NoiseOpPoint::AirfoilName() const
{
    return m_AirfoilName;
}

void NoiseOpPoint::setAirfoilName(QString AirfoilName)
{
    m_AirfoilName = AirfoilName;
}
QString NoiseOpPoint::PolarName() const
{
    return m_PolarName;
}

void NoiseOpPoint::setPolarName(QString PolarName)
{
    m_PolarName = PolarName;
}






