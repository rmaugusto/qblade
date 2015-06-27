#include "noiseoppoint.h"
#include <math.h>

NoiseOpPoint::NoiseOpPoint(double reynolds,double alpha){
    internalOpPoint = true;
    m_OpPoint = new OpPoint();
    m_OpPoint->setReynolds(reynolds);
    m_OpPoint->setAlpha(alpha);
}

NoiseOpPoint::NoiseOpPoint()
{
    if(internalOpPoint && m_OpPoint){
        delete m_OpPoint;
    }
}

NoiseOpPoint::~NoiseOpPoint()
{

}
double NoiseOpPoint::Reynolds()
{
    return m_OpPoint->getReynolds();
}

double NoiseOpPoint::Alpha()
{
    return m_OpPoint->getAlpha() * 180 / M_PI;
}


//void NoiseOpPoint::setAlphaDeg(double Alpha)
//{
//    m_Alpha = Alpha / 180 * M_PI;
//}

double NoiseOpPoint::AlphaDeg()
{
    return m_OpPoint->getAlpha();
}
QString NoiseOpPoint::AirfoilName()
{
    return m_OpPoint->getParent()->getParent()->getName();
}

QString NoiseOpPoint::PolarName()
{
    return m_OpPoint->getParent()->getName();
}






