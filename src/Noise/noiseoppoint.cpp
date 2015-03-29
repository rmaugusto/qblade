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

double NoiseOpPoint::AlphaDeg()
{
    return m_Alpha * 180 / M_PI;
}




