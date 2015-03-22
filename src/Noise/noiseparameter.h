#ifndef NOISEPARAMETER_H
#define NOISEPARAMETER_H

#include <QDebug>
#include <QtGlobal>
#include <list>
#include <noiseoppoint.h>

namespace Noise{

    enum DeltaSource { XFoilCalculation, OriginalBPM, ImportData };
    enum TransitionType { FullyTurbulent, TransitionFlow };

    const unsigned short IVX = 302;	//300 = number of nodes along bl on one side of airfoil and wake
    const unsigned short ISX = 3;	//number of airfoil sides

    typedef double ArrChordStations[Noise::IVX][Noise::ISX];
    typedef double TwoDArrDStars[Noise::IVX][Noise::ISX];

}

class NoiseParameter
{
public:



    NoiseParameter();

    /**
     * @brief ~NoiseParameter
     * When the object is destroyed, free the objects in memory
     */
    ~NoiseParameter();

    /**
     * @brief addOpPoint Add a new NoiseOpPoint to the list
     * @param reynolds
     * @param alpha
     */
    void addOpPoint(double reynolds, double alpha, double dStar);
    std::vector <NoiseOpPoint *> & OpPoints();

    /**
     * @brief DeltaSouce Delta* source
     * @return
     */
    Noise::DeltaSource DeltaSouce() ;
    /**
     * @brief setDeltaSouce Delta* source
     * @param DeltaSouce
     */
    void setDeltaSouce( Noise::DeltaSource &DeltaSouce);

    double WettedLength() ;
    void setWettedLength(double WettedLength);

    double DistanceObsever() ;
    void setDistanceObsever(double DistanceObsever);

    double DirectivityGreek() ;
    void setDirectivityGreek(double DirectivityGreek);

    double DirectivityPhi() ;
    void setDirectivityPhi(double DirectivityPhi);

    bool HighFreq() ;
    void setHighFreq(bool HighFreq);

    bool LowFreq() ;
    void setLowFreq(bool LowFreq);

    bool SuctionSide() ;
    void setSuctionSide(bool SuctionSide);

    bool PressureSide() ;
    void setPressureSide(bool PressureSide);

    bool SeparatedFlow() ;
    void setSeparatedFlow(bool SeparatedFlow);

    /**
     * @brief DStarChordStation D* at chord station
     * @return
     */
    double DStarChordStation() ;
    /**
     * @brief setDStarChordStation D* at chord station
     * @param DStarChordStation
     */
    void setDStarChordStation(double DStarChordStation);

    /**
     * @brief InterpolationLinear If use linear interpolation scheme
     * @return
     */
    bool InterpolationLinear() ;
    /**
     * @brief setInterpolationLinear If use linear interpolation scheme
     * @param InterpolationLinear
     */
    void setInterpolationLinear(bool InterpolationLinear);

    /**
     * @brief InterpolationNewtons If use newtons interpolation scheme
     * @return
     */
    bool InterpolationNewtons() ;
    /**
     * @brief setInterpolationNewtons If use newtons interpolation scheme
     * @param InterpolationNewtons
     */
    void setInterpolationNewtons(bool InterpolationNewtons);

    /**
     * @brief InterpolationLagranges If use Lagranges interpolation scheme
     * @return
     */
    bool InterpolationLagranges() ;
    /**
     * @brief setInterpolationLagranges If use Langranges interpolation scheme
     * @param InterpolationLagranges
     */
    void setInterpolationLagranges(bool InterpolationLagranges);

    /**
     * @brief InterpolationSpline If use Spline interpolation scheme
     * @return
     */
    bool InterpolationSpline() ;
    /**
     * @brief setInterpolationSpline If use Spline interpolation scheme
     * @param InterpolationSpline
     */
    void setInterpolationSpline(bool InterpolationSpline);

    /**
     * @brief DStarScalingFactor D* scaling factor
     * @return
     */
    double DStarScalingFactor() ;
    /**
     * @brief setDStarScalingFactor D* scaling factor
     * @param DStarScalingFactor
     */
    void setDStarScalingFactor(double DStarScalingFactor);

    /**
     * @brief EddyConvectionMach Eddy Convection Mach number
     * @return
     */
    double EddyConvectionMach() ;
    /**
     * @brief setEddyConvectionMach Eddy Convection Mach number
     * @param EddyConvectionMach
     */
    void setEddyConvectionMach(double EddyConvectionMach);

    /**
     * @brief Aoa Manual angle for BPM model
     * @return
     */
    double Aoa() ;
    /**
     * @brief setAoa Manual angle for BPM model
     * @param Aoa
     */
    void setAoa(double Aoa);

    /**
     * @brief ChordBasedReynolds Chord based Reynolds number
     * @return
     */
    double ChordBasedReynolds() ;
    /**
     * @brief setChordBasedReynolds Chord based Reynolds number
     * @param ChordBasedReynolds
     */
    void setChordBasedReynolds(double ChordBasedReynolds);

    /**
     * @brief OriginalMach Original flow Mach Number
     * @return
     */
    double OriginalMach() ;
    /**
     * @brief setOriginalMach original flow Mach Number
     * @param OriginalMach
     */
    void setOriginalMach(double OriginalMach);

    /**
     * @brief Transition type of transition
     * @return
     */
    Noise::TransitionType Transition() ;
    /**
     * @brief setTransition type of transition
     * @param Transition
     */
    void setTransition( Noise::TransitionType &Transition);

    /**
     * @brief OriginalChordLength Original airfoil Chord length
     * @return
     */
    double OriginalChordLength() ;
    /**
     * @brief setOriginalChordLength Original flow velocity
     * @param OriginalChordLength
     */
    void setOriginalChordLength(double OriginalChordLength);

    /**
     * @brief OriginalVelocity Velocity
     * @return
     */
    double OriginalVelocity();
    /**
     * @brief setOriginalVelocity Velocity
     * @param OriginalVelocity
     */
    void setOriginalVelocity(double OriginalVelocity);

    /**
     * @brief ReynoldsBasedDisplacement Reynolds Number based on displacement thickness of the pressure side
     * @return
     */
    double ReynoldsBasedDisplacement() const;
    /**
     * @brief setReynoldsBasedDisplacement Reynolds Number based on displacement thickness of the pressure side
     * @param ReynoldsBasedDisplacement
     */
    void setReynoldsBasedDisplacement(double ReynoldsBasedDisplacement);

    Noise::ArrChordStations& ChordStations();
    Noise::TwoDArrDStars& DStars();


private:
    std::vector <NoiseOpPoint *> m_OpPoints;
    Noise::DeltaSource m_DeltaSouce;
    double m_WettedLength;
    double m_DistanceObsever;
    double m_DirectivityGreek;
    double m_DirectivityPhi;
    bool m_HighFreq;
    bool m_LowFreq;
    bool m_SuctionSide; //SPLs
    bool m_PressureSide; //SPLp
    bool m_SeparatedFlow; //SPLa

    //XFoil correlation
    double m_DStarChordStation;
    bool m_InterpolationLinear;
    bool m_InterpolationNewtons;
    bool m_InterpolationLagranges;
    bool m_InterpolationSpline;
    double m_DStarScalingFactor;
    double m_EddyConvectionMach;
    double m_ChordBasedReynolds;
    double m_OriginalMach;
    double m_OriginalChordLength;
    double m_OriginalVelocity;
    double m_ReynoldsBasedDisplacement;

    //Original BPM correlations
    double m_Aoa;
    Noise::TransitionType m_Transition;

    double m_ChordStations[Noise::IVX][Noise::ISX];
    double m_DStars[Noise::IVX][Noise::ISX];

};

#endif // NOISEPARAMETER_H
