#ifndef NOISECALCULATION_H
#define NOISECALCULATION_H

#include <cmath>
#include <list>
#include <limits>
#include "noiseexception.h"
#include "noiseoppoint.h"
#include "noiseparameter.h"
#include "noisedefinitions.h"

class NoiseParameter;
class NoiseOpPoint;


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


    std::vector<double> OASPL();
    std::vector<double> OASPLA();
    std::vector<double> OASPLB();
    std::vector<double> OASPLC();

    std::vector<double> SPLALOG() const;
    std::vector<double> SPLSLOG() const;
    std::vector<double> SPLPLOG() const;

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

    bool m_CalcSeparatedFlow;
    bool m_CalcSuctionSide;
    bool m_CalcPressureSide;

    std::vector<double> m_OASPL;
    std::vector<double> m_OASPLA;
    std::vector<double> m_OASPLB;
    std::vector<double> m_OASPLC;
    std::vector<double> m_SPLALOG;
    std::vector<double> m_SPLSLOG;
    std::vector<double> m_SPLPLOG;

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

    double getDStarInterpolated(bool top, NoiseOpPoint *nop);
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
