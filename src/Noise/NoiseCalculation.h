#ifndef NOISECALCULATION_H
#define NOISECALCULATION_H

#include <QVector>

class NoiseParameter;
class NoiseOpPoint;


/* This class performes the calculations required by the noise simulation. Its purpose is to reduce the amount of
 * variables in the NoiseSimulation class and keep its namespace clean.
 * WARNING: When serializing this class only the publicly accsessible members are taken into account to avoid the
 * overhead of storing intermediate results. Therefore, after loading a calculation the calculate() method must be
 * run to restore the intermediate results.
 * */

class NoiseCalculation
{
public:
	enum AirfoilSide {PressureSide, SuctionSide};
	
	typedef QVector< QVector<double> > TwoDVector;
	
	static constexpr double SWITCHING_ANGLE2 = 12.5;
	static constexpr int FREQUENCY_TABLE_SIZE = 30;
	static const double AWeighting[FREQUENCY_TABLE_SIZE];  // 1/3 octave band frequency
	static const double BWeighting[FREQUENCY_TABLE_SIZE];
	static const double CWeighting[FREQUENCY_TABLE_SIZE];
	static const QVector<double> CENTRAL_BAND_FREQUENCY;
	
	NoiseCalculation();
	void serialize ();
	
	void setNoiseParam (NoiseParameter *parameter) { m_parameter = parameter; }
	void calculate();  // can throw NoiseException
	
	// NM the arrays containing the graph data
	TwoDVector SPLadB() const { return m_SPLadB; }
	TwoDVector SPLsdB() const { return m_SPLsdB; }
	TwoDVector SPLpdB() const { return m_SPLpdB; }
	TwoDVector SPLdB() const { return m_SPLdB; }
	TwoDVector SPLdBAW() const { return m_SPLdBAW; }
	TwoDVector SPLdBBW() const { return m_SPLdBBW; }
	TwoDVector SPLdBCW() const { return m_SPLdBCW; }
	
	// NM apparently needed for export as .txt only
	QVector<double> OASPL() const { return m_OASPL; }
	QVector<double> OASPLA() const { return m_OASPLA; }
	QVector<double> OASPLB() const { return m_OASPLB; }
	QVector<double> OASPLC() const { return m_OASPLC; }
	QVector<double> SPLALOG() const { return m_SPLALOG; }
	QVector<double> SPLSLOG() const { return m_SPLSLOG; }
	QVector<double> SPLPLOG() const { return m_SPLPLOG; }
	
private:
    void setupVectors();

	// calculation sub-functions
	double getK1(NoiseOpPoint* nop);
    double getDStarInterpolated(bool top, NoiseOpPoint *nop);  // can throw NoiseException
    double getDH();
    double getDL();
    double getSt1();
    double getSt2(NoiseOpPoint *nop);
    double getBPMThickness(NoiseOpPoint *nop, AirfoilSide as);
    void preCalcA1(NoiseOpPoint* nop);
    void preCalcSPLa(NoiseOpPoint* nop);
    void preCalcSPLs(NoiseOpPoint* nop);
    void preCalcSPLp(NoiseOpPoint* nop);
    void calcSPLa(double alpha,int posOpPoint,int posFreq);
    void calcSPLs(int posOpPoint,int posFreq);
    void calcSPLp(int posOpPoint,int posFreq);
		
	NoiseParameter *m_parameter;
	
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

    double m_A1Ar;

    QVector<double> m_OASPL;
    QVector<double> m_OASPLA;
    QVector<double> m_OASPLB;
    QVector<double> m_OASPLC;
    QVector<double> m_SPLALOG;
    QVector<double> m_SPLSLOG;
    QVector<double> m_SPLPLOG;

    //For SPLa
    TwoDVector m_SPLadB; //Store db of SPL alpha
    TwoDVector m_SPLadBAW; //Store db of SPL alpha + A-Weighting
    TwoDVector m_SPLadBBW; //Store db of SPL alpha + A-Weighting
    TwoDVector m_SPLadBCW; //Store db of SPL alpha + A-Weighting
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
    TwoDVector m_SPLsdB; //Store db of SPLs
    TwoDVector m_SPLsdBAW; //Store db of SPLs + A-Weighting
    TwoDVector m_SPLsdBBW; //Store db of SPLs + B-Weighting
    TwoDVector m_SPLsdBCW; //Store db of SPLs + C-Weighting
    double m_SplsFirstTerm;
    double m_SplsSt1;
    double m_SplsSt2;
    double m_SplsK1;
    double m_SplsSt1Bar;
    double m_SplsK13;

    //For SPLp
    TwoDVector m_SPLpdB; //Store db of SPLp
    TwoDVector m_SPLpdBAW; //Store db of SPLp + A-Weighting
    TwoDVector m_SPLpdBBW; //Store db of SPLp + B-Weighting
    TwoDVector m_SPLpdBCW; //Store db of SPLp + C-Weighting
    double m_SplpFirstTerm;
    double m_SplpSt1;
    double m_SplpK1;
    double m_SplpK13;
    double m_SplpDeltaK1;
    double m_ReynoldsBasedDisplacement;

    //For SPL
    TwoDVector m_SPLdB; //Store db of SPL
    TwoDVector m_SPLdBAW; //Store db of SPL + A-Weighting
    TwoDVector m_SPLdBBW; //Store db of SPL + B-Weighting
    TwoDVector m_SPLdBCW; //Store db of SPL + C-Weighting
};

#endif // NOISECALCULATION_H
