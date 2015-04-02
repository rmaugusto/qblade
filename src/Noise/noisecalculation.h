#ifndef NOISECALCULATION_H
#define NOISECALCULATION_H

#include <cmath>
#include <list>
#include <limits>
#include "noiseoppoint.h"
#include "noiseparameter.h"

class NoiseParameter;

namespace Noise {

    //1/3 octave band frequency
    const int FREQUENCY_TABLE_SIZE = 30;
    const double CENTRAL_BAND_FREQUENCY[FREQUENCY_TABLE_SIZE] = {25,31.5,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500,16000,20000};
    const double AWeighting[FREQUENCY_TABLE_SIZE] =  {-44.7,-39.4,-34.6,-30.2,-26.2,-22.5,-19.1,-16.1,-13.4,-10.9,-8.6,-6.6,-4.8,-3.2,-1.9,-0.8,0.0,0.6,1.0,1.2,1.3,1.2,1.0,0.5,-0.1,-1.1,-2.5,-4.3,-6.6,-9.3};
    const double BWeighting[FREQUENCY_TABLE_SIZE] =  {-20.4,-17.1,-14.2,-11.6,-9.3,-7.4,-5.6,-4.2,-3.0,-2.0,-1.3,-0.8,-0.5,-0.3,-0.1,0.0,0.0,0.0,0.0,-0.1,-0.2,-0.4,-0.7,-1.2,-1.9,-2.9,-4.3,-6.1,-8.4,-11.1};
    const double CWeighting[FREQUENCY_TABLE_SIZE] =  {-4.4,-3.0,-2.0,-1.3,-0.8,-0.5,-0.3,-0.2,-0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-0.1,-0.2,-0.3,-0.5,-0.8,-1.3,-2.0,-3.0,-4.4,-6.2,-8.5,-11.2};

    const double SWITCHING_ANGLE2 = 12.5;

    typedef std::vector< std::vector<double> > TwoDVector;

}


class NoiseCalculation
{
public:

    NoiseCalculation();
    ~NoiseCalculation();

    /**
     * @brief calculate
     */
    void calculate();

    /**
     * @brief setupVectors Allocates space according to parameters
     */
    void setupVectors();

    NoiseParameter *NoiseParam() const;
    void setNoiseParam(NoiseParameter *np);

    double getK1(NoiseOpPoint* nop);

    Noise::TwoDVector SPLdB() const;
    Noise::TwoDVector SPLdBAW() const;
    Noise::TwoDVector SPLdBBW() const;
    Noise::TwoDVector SPLdBCW() const;
    Noise::TwoDVector SPLpdB() const;
    Noise::TwoDVector SPLsdB() const;
    Noise::TwoDVector SPLadB() const;

    double* OASPL();
    double* OASPLA();
    double* OASPLB();
    double* OASPLC();

private:
    NoiseParameter * m_NoiseParameter;

    //For general
    double m_DStarInterpolatedS;
    double m_DStarInterpolatedP;
    double m_DStarFinalS;
    double m_DStarFinalP;
    double m_EddyMachNumber;
    double m_SwAlpha1;
    double m_SwAlpha;
    bool m_AlphaBigSw;
    double m_OASPL[Noise::FREQUENCY_TABLE_SIZE];
    double m_OASPLA[Noise::FREQUENCY_TABLE_SIZE];
    double m_OASPLB[Noise::FREQUENCY_TABLE_SIZE];
    double m_OASPLC[Noise::FREQUENCY_TABLE_SIZE];

    //For SPLa
    Noise::TwoDVector m_SPLadB; //Store db of SPL alpha
    Noise::TwoDVector m_SPLadBAW; //Store db of SPL alpha + A-Weighting
    Noise::TwoDVector m_SPLadBBW; //Store db of SPL alpha + A-Weighting
    Noise::TwoDVector m_SPLadBCW; //Store db of SPL alpha + A-Weighting
    double m_SplaFirstTerm;
    double m_SplaSt1;
    double m_SplaSt2;
    double m_SplaGamma;
    double m_SplaBeta;
    double m_SplaBetaZero;
    double m_SplaK1;
    double m_SplaK2;
    double m_SplaBr;
    double m_SplaBMax;
    double m_SplaBMin;
    double m_SplaBo;
    double m_SplaAr;
    double m_SplaAMax;
    double m_SplaAMin;
    double m_SplaAo;
    double m_ChordBasedReynolds;

    //For SPLs
    Noise::TwoDVector m_SPLsdB; //Store db of SPLs
    Noise::TwoDVector m_SPLsdBAW; //Store db of SPLs + A-Weighting
    Noise::TwoDVector m_SPLsdBBW; //Store db of SPLs + B-Weighting
    Noise::TwoDVector m_SPLsdBCW; //Store db of SPLs + C-Weighting
    double m_SplsFirstTerm;
    double m_SplsSt1;
    double m_SplsSt2;
    double m_SplsK1;
    double m_SplsSt1Bar;
    double m_SplsK13;

    //For SPLp
    Noise::TwoDVector m_SPLpdB; //Store db of SPLp
    Noise::TwoDVector m_SPLpdBAW; //Store db of SPLp + A-Weighting
    Noise::TwoDVector m_SPLpdBBW; //Store db of SPLp + B-Weighting
    Noise::TwoDVector m_SPLpdBCW; //Store db of SPLp + C-Weighting
    double m_SplpFirstTerm;
    double m_SplpSt1;
    double m_SplpK1;
    double m_SplpK13;
    double m_SplpDeltaK1;
    double m_ReynoldsBasedDisplacement;

    //For SPL
    Noise::TwoDVector m_SPLdB; //Store db of SPL
    Noise::TwoDVector m_SPLdBAW; //Store db of SPL + A-Weighting
    Noise::TwoDVector m_SPLdBBW; //Store db of SPL + B-Weighting
    Noise::TwoDVector m_SPLdBCW; //Store db of SPL + C-Weighting

    double getDStarInterpolated(bool top);
    double getDH();
    double getDL();
    double getSt1();
    double getSt2(NoiseOpPoint *nop);
    double getBPMThickness(NoiseOpPoint *nop, Noise::AirfoilSide as);

    void preCalcSPLa(NoiseOpPoint* nop);
    void preCalcSPLs(NoiseOpPoint* nop);
    void preCalcSPLp(NoiseOpPoint* nop);

    void calcSPLa(double alpha,int posOpPoint,int posFreq);
    void calcSPLs(int posOpPoint,int posFreq);
    void calcSPLp(int posOpPoint,int posFreq);



};

#endif // NOISECALCULATION_H
