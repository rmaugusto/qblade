#include "noiseparameter.h"

NoiseParameter::NoiseParameter()
{

    m_WettedLength=1;
    m_DistanceObsever=1.22;
    m_OriginalVelocity=71.3;
    m_OriginalChordLength=1;
    m_OriginalMach=0.21;
    m_DStarChordStation=0.98;
    m_DStarScalingFactor=1;
    m_EddyConvectionMach=0.8;
    m_DirectivityGreek = 1.570797;
    m_DirectivityPhi = 1.570797;
    m_HighFreq = true;
    m_LowFreq = true;
    m_InterpolationLinear =true;
    m_InterpolationLagranges=false;
    m_InterpolationNewtons=false;
    m_InterpolationSpline=false;
    m_SeparatedFlow=true;
    m_SuctionSide=true;
    m_PressureSide=true;
    m_DeltaSouce = Noise::XFoilCalculation;

    //reset all value
    memset(m_ChordStations, 0, sizeof(m_ChordStations[0][0]) * Noise::IIVX * Noise::IISX);
    memset(m_DStars, 0, sizeof(m_DStars[0][0]) * Noise::IIVX * Noise::IISX);


}

NoiseParameter::~NoiseParameter()
{

    //Remove from list and free memory
    while(m_OpPoints.size()>0){
        delete m_OpPoints.front();
        m_OpPoints.erase( m_OpPoints.begin() );
    }

}

void NoiseParameter::addOpPoint(double reynolds, double alpha)
{
    m_OpPoints.push_back(new NoiseOpPoint(reynolds,alpha));
}

std::vector<NoiseOpPoint *> &NoiseParameter::OpPoints()
{
    return m_OpPoints;
}


Noise::DeltaSource NoiseParameter::DeltaSouce()
{
    return m_DeltaSouce;
}

void NoiseParameter::setDeltaSouce( Noise::DeltaSource DeltaSouce)
{
    m_DeltaSouce = DeltaSouce;
}
double NoiseParameter::WettedLength()
{
    return m_WettedLength;
}

void NoiseParameter::setWettedLength(double WettedLength)
{
    m_WettedLength = WettedLength;
}
double NoiseParameter::DistanceObsever()
{
    return m_DistanceObsever;
}

void NoiseParameter::setDistanceObsever(double DistanceObsever)
{
    m_DistanceObsever = DistanceObsever;
}
double NoiseParameter::DirectivityGreek()
{
    return m_DirectivityGreek;
}

void NoiseParameter::setDirectivityGreek(double DirectivityGreek)
{
    m_DirectivityGreek = DirectivityGreek;
}

double NoiseParameter::DirectivityGreekDeg()
{
    return m_DirectivityGreek * (180 / M_PI);
}

void NoiseParameter::setDirectivityGreekDeg(double DirectivityGreek)
{
    m_DirectivityGreek =  DirectivityGreek * (M_PI / 180);
}
double NoiseParameter::DirectivityPhi()
{
    return m_DirectivityPhi;
}

void NoiseParameter::setDirectivityPhi(double DirectivityPhi)
{
    m_DirectivityPhi = DirectivityPhi;
}

double NoiseParameter::DirectivityPhiDeg()
{
    return m_DirectivityPhi * (180 / M_PI);
}

void NoiseParameter::setDirectivityPhiDeg(double DirectivityPhi)
{
    m_DirectivityPhi =  DirectivityPhi * (M_PI / 180);
}
bool NoiseParameter::HighFreq()
{
    return m_HighFreq;
}

void NoiseParameter::setHighFreq(bool HighFreq)
{
    m_HighFreq = HighFreq;
}
bool NoiseParameter::LowFreq()
{
    return m_LowFreq;
}

void NoiseParameter::setLowFreq(bool LowFreq)
{
    m_LowFreq = LowFreq;
}
bool NoiseParameter::SuctionSide()
{
    return m_SuctionSide;
}

void NoiseParameter::setSuctionSide(bool SuctionSide)
{
    m_SuctionSide = SuctionSide;
}
bool NoiseParameter::PressureSide()
{
    return m_PressureSide;
}

void NoiseParameter::setPressureSide(bool PressureSide)
{
    m_PressureSide = PressureSide;
}
bool NoiseParameter::SeparatedFlow()
{
    return m_SeparatedFlow;
}

void NoiseParameter::setSeparatedFlow(bool SeparatedFlow)
{
    m_SeparatedFlow = SeparatedFlow;
}
double NoiseParameter::DStarChordStation()
{
    return m_DStarChordStation;
}

void NoiseParameter::setDStarChordStation(double DStarChordStation)
{
    m_DStarChordStation = DStarChordStation;
}
bool NoiseParameter::InterpolationLinear()
{
    return m_InterpolationLinear;
}

void NoiseParameter::setInterpolationLinear(bool InterpolationLinear)
{
    m_InterpolationLinear = InterpolationLinear;
}
bool NoiseParameter::InterpolationNewtons()
{
    return m_InterpolationNewtons;
}

void NoiseParameter::setInterpolationNewtons(bool InterpolationNewtons)
{
    m_InterpolationNewtons = InterpolationNewtons;
}
bool NoiseParameter::InterpolationLagranges()
{
    return m_InterpolationLagranges;
}

void NoiseParameter::setInterpolationLagranges(bool InterpolationLagranges)
{
    m_InterpolationLagranges = InterpolationLagranges;
}
bool NoiseParameter::InterpolationSpline()
{
    return m_InterpolationSpline;
}

void NoiseParameter::setInterpolationSpline(bool InterpolationSpline)
{
    m_InterpolationSpline = InterpolationSpline;
}
double NoiseParameter::DStarScalingFactor()
{
    return m_DStarScalingFactor;
}

void NoiseParameter::setDStarScalingFactor(double DStarScalingFactor)
{
    m_DStarScalingFactor = DStarScalingFactor;
}
double NoiseParameter::EddyConvectionMach()
{
    return m_EddyConvectionMach;
}

void NoiseParameter::setEddyConvectionMach(double EddyConvectionMach)
{
    m_EddyConvectionMach = EddyConvectionMach;
}
double NoiseParameter::Aoa()
{
    return m_Aoa;
}

void NoiseParameter::setAoa(double Aoa)
{
    m_Aoa = Aoa;
}

void NoiseParameter::setOriginalMach(double OriginalMach)
{
    m_OriginalMach = OriginalMach;
}
double NoiseParameter::OriginalMach()
{
    return m_OriginalMach;
}
Noise::TransitionType NoiseParameter::Transition()
{
    return m_Transition;
}

void NoiseParameter::setTransition(Noise::TransitionType Transition)
{
    m_Transition = Transition;
}
double NoiseParameter::OriginalChordLength()
{
    return m_OriginalChordLength;
}

void NoiseParameter::setOriginalChordLength(double OriginalChordLength)
{
    m_OriginalChordLength = OriginalChordLength;
}
double NoiseParameter::OriginalVelocity()
{
    return m_OriginalVelocity;
}

void NoiseParameter::setOriginalVelocity(double OriginalVelocity)
{
    m_OriginalVelocity = OriginalVelocity;
}

Noise::ArrChordStations& NoiseParameter::ChordStations()
{
    return m_ChordStations;
}

Noise::TwoDArrDStars& NoiseParameter::DStars()
{
    return m_DStars;
}

