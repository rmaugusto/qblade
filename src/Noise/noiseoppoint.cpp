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
}

NoiseOpPoint::~NoiseOpPoint()
{
    if(internalOpPoint && m_OpPoint){
        delete m_OpPoint;
    }

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

void NoiseOpPoint::calculateXSides()
{
        int i;

        for(int is=1; is<= 2; is++){
            for (int ibl=2; ibl<= m_OpPoint->getNbl()[is]; ibl++){
                i = m_OpPoint->getIpan()[ibl][is];
                xs[ibl][is] = m_OpPoint->getX()[i];
            }
        }

        m_nSide1 = m_OpPoint->getNbl()[2] + m_OpPoint->getIblte()[1] - m_OpPoint->getIblte()[2];
        m_nSide2 = m_OpPoint->getNbl()[2];

        for( int iblw=1; iblw <= m_OpPoint->getNbl()[2]-m_OpPoint->getIblte()[2]; iblw++)
            xs[m_OpPoint->getIblte()[1]+iblw][1] = xs[m_OpPoint->getIblte()[2]+iblw][2];

}
int NoiseOpPoint::getNSide1() const
{
    return m_nSide1;
}
int NoiseOpPoint::getNSide2() const
{
    return m_nSide2;
}

double *NoiseOpPoint::getX()
{
    return m_OpPoint->getX();
}

Noise::TwoDArrDStars &NoiseOpPoint::getDstr()
{
    return m_OpPoint->getDstr();
}
OpPoint *NoiseOpPoint::getOpPoint() const
{
    return m_OpPoint;
}










