#include "NoiseCalculation.h"

#include <QDebug>

#include "NoiseParameter.h"
#include "../Serializer.h"
#include "NoiseOpPoint.h"
#include "NoiseException.h"


const double NoiseCalculation::AWeighting[] = {-44.7, -39.4, -34.6, -30.2, -26.2, -22.5, -19.1, -16.1,
											   -13.4, -10.9,  -8.6,  -6.6,  -4.8,  -3.2,  -1.9,  -0.8,
											     0.0,   0.6,   1.0,   1.2,   1.3,   1.2,   1.0,   0.5,
											    -0.1,  -1.1,  -2.5,  -4.3,  -6.6,-  9.3};
const double NoiseCalculation::BWeighting[] = {-20.4, -17.1, -14.2, -11.6,  -9.3,  -7.4,  -5.6,  -4.2,
											    -3.0,  -2.0,  -1.3,  -0.8,  -0.5,  -0.3,  -0.1,   0.0,
											     0.0,   0.0,   0.0,  -0.1,  -0.2,  -0.4,  -0.7,  -1.2,
											    -1.9,  -2.9,  -4.3,  -6.1,  -8.4, -11.1};
const double NoiseCalculation::CWeighting[] = { -4.4,  -3.0,  -2.0,  -1.3,  -0.8,  -0.5,  -0.3,  -0.2,
												-0.1,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
												 0.0,   0.0,  -0.1,  -0.2,  -0.3,  -0.5,  -0.8,  -1.3,
												-2.0,  -3.0,  -4.4,  -6.2,  -8.5, -11.2};
const QVector<double> NoiseCalculation::CENTRAL_BAND_FREQUENCY ({25, 31.5, 40, 50, 63, 80, 100, 125, 160,
																 200, 250, 315, 400, 500, 630, 800, 1000,
																 1250, 1600, 2000, 2500, 3150, 4000, 5000,
																 6300, 8000, 10000, 12500, 16000, 20000});

NoiseCalculation::NoiseCalculation() {
    m_parameter = NULL;
    m_CalcSeparatedFlow = false;
    m_CalcPressureSide = false;
	m_CalcSuctionSide = false;
}

void NoiseCalculation::serialize() {
	g_serializer.readOrWriteDoubleVector2D(&m_SPLadB);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLsdB);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLpdB);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLdB);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLdBAW);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLdBBW);
	g_serializer.readOrWriteDoubleVector2D(&m_SPLdBCW);
	
	g_serializer.readOrWriteDoubleVector1D(&m_OASPL);
	g_serializer.readOrWriteDoubleVector1D(&m_OASPLA);
	g_serializer.readOrWriteDoubleVector1D(&m_OASPLB);
	g_serializer.readOrWriteDoubleVector1D(&m_OASPLC);
	g_serializer.readOrWriteDoubleVector1D(&m_SPLALOG);
	g_serializer.readOrWriteDoubleVector1D(&m_SPLSLOG);
	g_serializer.readOrWriteDoubleVector1D(&m_SPLPLOG);
}

double NoiseCalculation::getDStarInterpolated(bool top,NoiseOpPoint * nop) {
    bool upDownFind = false;
    double chordUpStream = 0;
    double chordDownStream = 0;
    double dStarUpStream = 0;
    double dStarDownStream = 0;

    //For positive alpha use TopSide else BottomSide
    //int side = top ? 1 : 2;
    int side = top ? 2 : 1;
    int nside = top ? nop->getNSide2() : nop->getNSide1();

    double currentChord = 0;
    double currentDStar = 0;
    double previousChord = 0;
    double previousDStar = 0;

    //Find closest station assuming crescent order on chordStation
    for (int i = 2; i <= nside; ++i) {
        currentChord = nop->getXValue(i, side);
        currentDStar = nop->getDstrAt(i, side);
        previousChord = i == 0 ? currentChord : nop->getXValue(i-1, side);
        previousDStar = i == 0 ? currentDStar: nop->getDstrAt(i-1, side);

        //qDebug() << "i: " << i << " - " << ccur;

        if (currentChord > m_parameter->dStarChordStation) {
            chordUpStream = previousChord;
            chordDownStream = currentChord;
            dStarUpStream = previousDStar;
            dStarDownStream = currentDStar;

//			qDebug() << "Chord UpStream: " << chordUpStream;
//			qDebug() << "Chord DownStream: " << chordDownStream;
//			qDebug() << "D* UpStream: " << dStarUpStream;
//			qDebug() << "D* DownStream: " << dStarDownStream;
			
            upDownFind = true;
            break;
        }
    }

    if (!upDownFind) {
        qWarning() << "Can not find upstream and downstream. D* Interpolated will be zero ! D* ChordStation target: "
				   << m_parameter->dStarChordStation << " - Last found X ( "<<currentChord<<" ) D* ("
				   << currentDStar <<")";
        throw NoiseException(NoiseException::EXPT_DSTAR_NOT_FOUND, "There is no data to interpolate D* from, at the "
							 "specified chord station");
    }

    return ((dStarUpStream-dStarDownStream) * (m_parameter->dStarChordStation-chordDownStream) /
			(chordUpStream-chordDownStream)) + dStarDownStream;
}

double NoiseCalculation::getDL() {
    //Dh, Low Freq. Directivity Factor
    if (m_parameter->lowFreq) {
        return (2 * (pow(sin(m_parameter->directivityGreek),2)) * (pow(sin(m_parameter->directivityPhi),2))) /
				pow((1+m_parameter->originalMach * cos(m_parameter->directivityGreek)),4);
    } else {
        return 1;
    }
}

double NoiseCalculation::getDH() {
    //Dh, High Freq. Directivity Factor
    if (m_parameter->highFreq) {
        return (2 * pow((sin((m_parameter->directivityGreek/2))),2) * pow((sin(m_parameter->directivityPhi)),2)) /
				((1+m_parameter->originalMach * cos(m_parameter->directivityGreek)) *
				 (pow((1+(m_parameter->originalMach-m_EddyMachNumber) * cos(m_parameter->directivityPhi)),2)));
    } else {
        return 1;
    }
}

double NoiseCalculation::getSt1() {
    return 0.02 * pow(m_parameter->originalMach, -0.6);
}

double NoiseCalculation::getSt2(NoiseOpPoint* nop) {
    const double st1 = getSt1();
    double st2;

    if (nop->getAlphaDegreeAbsolute() < 1.33) {
        st2 = st1;
    } else if (nop->getAlphaDegreeAbsolute() > 12.5) {
        st2 = st1*4.72;
    } else {
        st2 = st1 * pow(10,0.0054* pow((nop->getAlphaDegreeAbsolute()-1.33),2));
    }

    return st2;
}

double NoiseCalculation::getBPMThickness(NoiseOpPoint *nop, AirfoilSide as) {
    double dStarCF = 0;
    double dStarCT = 0;
    double dStar = 0;
    double bpm = 0;

    if (m_parameter->transition == NoiseParameter::FullyTurbulent) {
        if(nop->getReynolds() > 300000) {
            dStarCF = pow(10,(3.411-1.5397 * log10(nop->getReynolds())+0.1059 * pow(log10(nop->getReynolds()),2)));
        } else {
            dStarCF = 0.0601 * (pow(nop->getReynolds(),-0.114));
        }
        dStar = dStarCF * m_parameter->originalChordLength;
//		qDebug() << "BPM FullyTurbulent dStarCF: " << dStarCF;
	} else {
        dStarCT = pow(10,(3.0187-1.5397*log10(nop->getReynolds())+0.1059* pow(log10(nop->getReynolds()),2)));
//		qDebug() << "BPM TransitionFlow dStarCT: " << dStarCT;
		dStar = dStarCT * m_parameter->originalChordLength;
    }

    if (nop->getAlphaDegreeAbsolute() == 0) {
        bpm = dStar;
    } else {
        double corFactor = 0;

        if (as == PressureSide) {
            corFactor = pow(10,(-0.0432*nop->getAlphaDegreeAbsolute()+0.00113*pow(nop->getAlphaDegreeAbsolute(),2)));
            bpm = corFactor * dStar;
        } else {
            if (m_parameter->transition == NoiseParameter::FullyTurbulent) {
                if (nop->getAlphaDegreeAbsolute() < 5) {
                    corFactor = pow(10,(0.0679 * nop->getAlphaDegreeAbsolute()));
                } else if (nop->getAlphaDegreeAbsolute() >= 5 && nop->getAlphaDegreeAbsolute() <= 12.5) {
                    corFactor = 0.381*(pow(10,(0.1516*nop->getAlphaDegreeAbsolute())));
                } else {
                    corFactor = 14.296*(pow(10,(0.0258*nop->getAlphaDegreeAbsolute())));
                }
            } else {
                if (nop->getAlphaDegreeAbsolute() < 7.5) {
                    corFactor = pow(10,(0.0679*nop->getAlphaDegreeAbsolute()));
                } else if (nop->getAlphaDegreeAbsolute() >= 7.5 && nop->getAlphaDegreeAbsolute() <= 12.5) {
                    corFactor = 0.0162*(pow(10,(0.3066*nop->getAlphaDegreeAbsolute())));
                } else {
                    corFactor = 54.42*(pow(10,(0.0258*nop->getAlphaDegreeAbsolute())));
                }
            }
            bpm = corFactor * dStar;
        }
    }
//	qDebug() << "BPM D*: " << bpm;
	
    return bpm;
}

double NoiseCalculation::getK1(NoiseOpPoint* nop) {
    double k1;
	
    if (nop->getReynolds() < 247000) {
        k1 = -4.31 * log10(nop->getReynolds()) + 156.3;
    } else if (nop->getReynolds() > 800000) {
        k1 = 128.5;
    } else {
        k1 = -9 * log10(nop->getReynolds()) + 181.6;
    }

    return k1;
}

void NoiseCalculation::preCalcA1(NoiseOpPoint* nop) {
    double m_A1ChordBasedReynolds;
    double m_A1Ao;
    double m_A1AMin;
    double m_A1AMax;
//    double m_A1FirstTerm;  // NM was not used...

//	m_A1FirstTerm = 10 * log10(pow(m_parameter->m_OriginalMach, 5) * m_parameter->m_WettedLength * getDL() *
//							   m_DStarFinalS / pow(m_parameter->m_DistanceObsever, 2));
	m_A1ChordBasedReynolds = nop->getReynolds() * 3;

    if(m_A1ChordBasedReynolds < 95200){
        m_A1Ao = 0.57;
    }else if(m_A1ChordBasedReynolds > 857000){
        m_A1Ao = 1.13;
    }else{
        m_A1Ao = (-0.000000000000957*(pow((m_A1ChordBasedReynolds-857000),2))+1.13);
    }

    if(m_A1Ao < 0.204){
        m_A1AMin = (sqrt(67.552-886.788*pow(m_A1Ao,2))-8.219);
    }else if(m_A1Ao > 0.244){
        m_A1AMin = (-142.795*pow(m_A1Ao,3)+103.656*pow(m_A1Ao,2)-57.757*m_A1Ao+6.006);
    }else{
        m_A1AMin = (-32.665*m_A1Ao+3.981);
    }

    if(m_A1Ao < 0.13){
        m_A1AMax = (sqrt(67.552-886.788*pow(m_A1Ao,2))-8.219);
    }else if(m_A1Ao > 0.321){
        m_A1AMax = (-4.669*pow(m_A1Ao,3)+3.491*pow(m_A1Ao,2)-16.699*m_A1Ao+1.149);
    }else{
        m_A1AMax = (-15.901*m_A1Ao+1.098);
    }

    m_A1Ar = (-20-m_A1AMin)/(m_A1AMax-m_A1AMin);

//	qDebug() << "A1 ChordBasedReynolds " << m_A1ChordBasedReynolds;
//	qDebug() << "A1 Ao " << m_A1Ao;
//	qDebug() << "A1 aMin " << m_A1AMin;
//	qDebug() << "A1 aMax " << m_A1AMax;
//	qDebug() << "A1 Ar " << m_A1Ar;
}

void NoiseCalculation::preCalcSPLa(NoiseOpPoint* nop) {
//    qDebug() << "---> SPLa CALCULATION";

    m_SplaFirstTerm=0;

    //If angle is smaller than the switching Angle
    //use dH and B else use dL and A1
    if (nop->getAlphaDegree() <= m_SwAlpha) {
//		qDebug() << "SPLa dH: " << getDH();
		m_SplaFirstTerm = 10 * log10(pow(m_parameter->originalMach, 5) * m_parameter->wettedLength * getDH() *
									 m_DStarFinalS / pow(m_parameter->distanceObsever, 2));

        if(nop->getReynolds() < 95200){
            m_SplaBo = 0.3;
        }else if(nop->getReynolds() > 857000){
            m_SplaBo = 0.56;
        }else{
            m_SplaBo = (-0.000000000000448*( pow((nop->getReynolds()-857000),2)+0.56));
        }

        if(m_SplaBo < 0.13){
            m_SplaBMin = (sqrt(16.888-886.788*pow(m_SplaBo,2))-4.109);
        }else if(m_SplaBo > 0.145){
            m_SplaBMin = (-817.81*pow(m_SplaBo,3)+335.21*pow(m_SplaBo,2)-135.024*m_SplaBo+10.619);
        }else{
            m_SplaBMin = (-83.607*m_SplaBo+8.138);
        }

        if(m_SplaBo < 0.1){
            m_SplaBMax = (sqrt(16.888-886.788*pow(m_SplaBo,2))-4.109);
        }else if(m_SplaBo > 0.187){
            m_SplaBMax = (-80.541*pow(m_SplaBo,3)+44.174*pow(m_SplaBo,2)-39.381*m_SplaBo+2.344);
        }else{
            m_SplaBMax = (-31.33*m_SplaBo+1.854);
        }

        m_SplaBr = (-20-m_SplaBMin)/(m_SplaBMax-m_SplaBMin);

//		qDebug() << "SPLA Bo " << m_SplaBo;
//		qDebug() << "SPLA bMin " << m_SplaBMin;
//		qDebug() << "SPLA bMax " << m_SplaBMax;
//		qDebug() << "SPLA Br " << m_SplaBr;
	} else {
//		qDebug() << "SPLa dL: " << getDL();
		m_SplaFirstTerm = 10 * log10(pow(m_parameter->originalMach, 5) * m_parameter->wettedLength *
									 getDL() * m_DStarFinalS / pow(m_parameter->distanceObsever, 2));
        m_ChordBasedReynolds = nop->getReynolds() * 3;

        if(m_ChordBasedReynolds < 95200){
            m_SplaAo = 0.57;
        }else if(m_ChordBasedReynolds > 857000){
            m_SplaAo = 1.13;
        }else{
            m_SplaAo = (-0.000000000000957*(pow((m_ChordBasedReynolds-857000),2))+1.13);
        }

        if(m_SplaAo < 0.204){
            m_SplaAMin = (sqrt(67.552-886.788*pow(m_SplaAo,2))-8.219);
        }else if(m_SplaAo > 0.244){
            m_SplaAMin = (-142.795*pow(m_SplaAo,3)+103.656*pow(m_SplaAo,2)-57.757*m_SplaAo+6.006);
        }else{
            m_SplaAMin = (-32.665*m_SplaAo+3.981);
        }

        if(m_SplaAo < 0.13){
            m_SplaAMax = (sqrt(67.552-886.788*pow(m_SplaAo,2))-8.219);
        }else if(m_SplaAo > 0.321){
            m_SplaAMax = (-4.669*pow(m_SplaAo,3)+3.491*pow(m_SplaAo,2)-16.699*m_SplaAo+1.149);
        }else{
            m_SplaAMax = (-15.901*m_SplaAo+1.098);
        }

        m_SplaAr = (-20-m_SplaAMin)/(m_SplaAMax-m_SplaAMin);

//		qDebug() << "SPLA ChordBasedReynolds " << m_ChordBasedReynolds;
//		qDebug() << "SPLA Ao " << m_SplaAo;
//		qDebug() << "SPLA aMin " << m_SplaAMin;
//		qDebug() << "SPLA aMax " << m_SplaAMax;
//		qDebug() << "SPLA Ar " << m_SplaAr;
	}
	
    m_SplaSt1 = getSt1();
    m_SplaSt2 = getSt2(nop);

    m_SplaGamma = 27.094 * m_parameter->originalMach + 3.32;
    m_SplaBeta = 72.65*m_parameter->originalMach+10.74;
    m_SplaBetaZero = -34.19*m_parameter->originalMach-13.82;

    m_SplaK1 = getK1(nop);

    m_SplaK2 = 0;
    if(nop->getAlphaDegreeAbsolute() < (m_SwAlpha1-m_SplaGamma)){
        m_SplaK2 = m_SplaK1 - 1000;
    }else if(nop->getAlphaDegreeAbsolute() > (m_SwAlpha1+m_SplaGamma)){
        m_SplaK2 = m_SplaK1 - 12;
    }else{
        m_SplaK2 = m_SplaK1 + (sqrt( pow(m_SplaBeta,2)-pow((m_SplaBeta/m_SplaGamma),2) * 
									 pow((nop->getAlphaDegreeAbsolute()-m_SwAlpha1),2) ) + m_SplaBetaZero);
    }

//	qDebug() << "SPLa firstTerm: " << m_SplaFirstTerm;
//	qDebug() << "SPLa st1: " << m_SplaSt1;
//	qDebug() << "SPLa st2: " << m_SplaSt2;
//	qDebug() << "SPLa gamma: " << m_SplaGamma;
//	qDebug() << "SPLa gamma_zero: " << m_SwAlpha1;
//	qDebug() << "SPLa beta: " << m_SplaBeta;
//	qDebug() << "SPLa betaZero: " << m_SplaBetaZero;
//	qDebug() << "SPLa reynolds: " << nop->Reynolds();
//	qDebug() << "SPLa k1: " << m_SplaK1;
//	qDebug() << "SPLa k2: " << m_SplaK2;
}

void NoiseCalculation::preCalcSPLs(NoiseOpPoint *nop) {
//	qDebug() << "---> SPLs CALCULATION";
    m_SplsFirstTerm = 0;
    m_SplsFirstTerm = 10 * log10(pow(m_parameter->originalMach,5) * m_parameter->wettedLength * getDH() *
								 m_DStarFinalS / pow(m_parameter->distanceObsever,2));

//	qDebug() << "SPLs dH: " << getDH();
//	qDebug() << "SPLs firstTerm: " << m_SplsFirstTerm;
	
    m_SplsSt1 = getSt1();
    m_SplsSt2 = getSt2(nop);
    m_SplsK1 = getK1(nop);
    m_SplsSt1Bar = (m_SplsSt1+m_SplsSt2)/2;
    m_SplsK13 = m_SplsK1-3;

//	qDebug() << "SPLs st1: " << m_SplsSt1;
//	qDebug() << "SPLs st2: " << m_SplsSt2;
//	qDebug() << "SPLs st1Bar: " << m_SplsSt1Bar;
//	qDebug() << "SPLs k1: " << m_SplsK1;
//	qDebug() << "SPLs k1-3: " << m_SplsK13;
}

void NoiseCalculation::preCalcSPLp(NoiseOpPoint *nop) {
//    qDebug() << "---> SPLp CALCULATION";
    m_SplpFirstTerm=0;
    m_SplpFirstTerm = 10 * log10(pow(m_parameter->originalMach,5) * m_parameter->wettedLength * getDH() *
								 m_DStarFinalP / pow(m_parameter->distanceObsever,2));

//    qDebug() << "SPLp dH: " << getDH();
//    qDebug() << "SPLp firstTerm: " << m_SplpFirstTerm;

    m_SplpSt1 = getSt1();
    m_SplpK1 = getK1(nop);
    m_SplpK13 = m_SplpK1-3;
    m_SplpDeltaK1 = 0;
    m_ReynoldsBasedDisplacement = 1.225 * m_parameter->originalVelocity * m_DStarFinalP / 0.0000178;

    if (m_ReynoldsBasedDisplacement > 5000) {
        m_SplpDeltaK1 = 0;
    } else {
        m_SplpDeltaK1 = nop->getAlphaDegreeAbsolute() * (1.43*log10(m_ReynoldsBasedDisplacement)-5.29);
    }
	
//	qDebug() << "Reynolds Based Displacement: " << m_ReynoldsBasedDisplacement;
//	qDebug() << "SPLp st1: " << m_SplpSt1;
//	qDebug() << "SPLp k1: " << m_SplpK1;
//	qDebug() << "SPLp k1-3: " << m_SplpK13;
//	qDebug() << "SPLp DeltaK1: " << m_SplpDeltaK1;
}

void NoiseCalculation::calcSPLa(double alpha, int posOpPoint, int posFreq) {
    double splDb = 0;

    //If angle is smaller than the switching Angle
    //use dH and B else use dL and A1
    if (alpha <= m_SwAlpha ) {
        double sts = CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_parameter->originalVelocity;
        double b = fabs(log10(sts/m_SplaSt2));
        double bMin = 0;
        double bMax = 0;
        double db = 0;

        if(b < 0.13){
            bMin = (sqrt(16.888-886.788*pow(b,2))-4.109);
        }else if(b > 0.145){
            bMin = (-817.81*pow(b,3)+335.21*pow(b,2)-135.024*b+10.619);
        }else{
            bMin = (-83.607*b+8.138);
        }

        if(b < 0.1){
            bMax = (sqrt(16.888-886.788*pow(b,2))-4.109);
        }else if(b > 0.187){

            bMax = (-80.541*pow(b,3)+44.174*pow(b,2)-39.381*b+2.344);
        }else{
            bMax = (-83.607*b+8.138);
        }
        db =bMin+ m_SplaBr *(bMax-bMin);
        splDb = m_SplaFirstTerm +db+m_SplaK2;

        m_SPLadB[posOpPoint][posFreq] = splDb;
        m_SPLadBAW[posOpPoint][posFreq] = splDb + AWeighting[posFreq];
        m_SPLadBBW[posOpPoint][posFreq] = splDb + BWeighting[posFreq];
        m_SPLadBCW[posOpPoint][posFreq] = splDb + CWeighting[posFreq];

//		qDebug() << "SPLa -> sts("<< sts <<")\t"<< "b("<< b <<")\t"<< "bMin("<< bMin <<")\t"<< "bMax("<< bMax
//				 << ")\t"<< "db("<< db <<")\t"<< "splDb("<< splDb <<")\t"<< "splDb-AW("
//				 << m_SPLadBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLadBBW[posOpPoint][posFreq] <<")\t"
//				 << "splDb-CW("<< m_SPLadBCW[posOpPoint][posFreq] <<")\t";
    } else {
        double sts = CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_parameter->originalVelocity;
        double a = fabs(log10(sts/m_SplaSt2));
        double aMin = 0;
        double aMax = 0;
        double a1 = 0;

        if(a<0.204){
            aMin = sqrt(67.552-886.788*pow(a,2));
        }else if(a > 0.244){
            aMin = -142.795*pow(a,3)+103.656*pow(a,2)-57.757*a+6.006;
        }else{
            aMin = -32.665*a+3.981;
        }

        if(a<0.13){
            aMax = (sqrt(67.552-886.788*pow(a,2))-8.219);
        }else if(a > 0.321){
            aMax = (-4.669*pow(a,3)+3.491*pow(a,2)-16.699*a+1.149);
        }else{
            aMax = (-15.901*a+1.098);
        }

        a1 =aMin+ m_SplaAr *(aMax-aMin);
        splDb = m_SplaFirstTerm +a1+m_SplaK2;

        m_SPLadB[posOpPoint][posFreq] = splDb;
        m_SPLadBAW[posOpPoint][posFreq] = splDb + AWeighting[posFreq];
        m_SPLadBBW[posOpPoint][posFreq] = splDb + BWeighting[posFreq];
        m_SPLadBCW[posOpPoint][posFreq] = splDb + CWeighting[posFreq];

//		qDebug() << "SPLa -> sts("<< sts <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax
//				 << ")\t"<< "a1("<< a1 <<")\t"<< "splDb("<< splDb <<")\t"<< "splDb-AW("
//				 << m_SPLadBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLadBBW[posOpPoint][posFreq] <<")\t"
//				 << "splDb-CW("<< m_SPLadBCW[posOpPoint][posFreq] <<")\t";
	}
}

void NoiseCalculation::calcSPLs(int posOpPoint,int posFreq) {
    double sts = CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_parameter->originalVelocity;
    double a = fabs(log10(sts/m_SplsSt1Bar));
    double aMin = 0;
    double aMax = 0;
    double a1 = 0;
    double splDb = 0;

    //If angle is bigger than the switching Angle
    //or suction side is mandatory
    if(m_CalcSuctionSide){

        if(a<0.204){
            aMin = sqrt(67.552-886.788*pow(a,2)-8.219);
        }else if(a > 0.244){
            aMin = -142.795*pow(a,3)+103.656*pow(a,2)-57.757*a+6.006;
        }else{
            aMin = -32.665*a+3.981;
        }

        if(a<0.13){
            aMax = (sqrt(67.552-886.788*pow(a,2))-8.219);
        }else if(a > 0.321){
            aMax = (-4.669*pow(a,3)+3.491*pow(a,2)-16.699*a+1.149);
        }else{
            aMax = (-15.901*a+1.098);
        }

        a1 =aMin+ m_A1Ar *(aMax-aMin);
        splDb = m_SplsFirstTerm +a1+m_SplsK13;

    } else {
		splDb = std::numeric_limits<long>::lowest();
    }

    m_SPLsdB[posOpPoint][posFreq] = splDb;
    m_SPLsdBAW[posOpPoint][posFreq] = splDb + AWeighting[posFreq];
    m_SPLsdBBW[posOpPoint][posFreq] = splDb + BWeighting[posFreq];
    m_SPLsdBCW[posOpPoint][posFreq] = splDb + CWeighting[posFreq];

//	qDebug() << "SPLs -> sts("<< sts <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax <<")\t"
//			 << "a1("<< a1 <<")\t" << "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLsdBAW[posOpPoint][posFreq]
//			 << ")\t"<< "splDb-BW("<< m_SPLsdBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("
//			 << m_SPLsdBCW[posOpPoint][posFreq] <<")\t";
}

void NoiseCalculation::calcSPLp(int posOpPoint,int posFreq) {
    double stp = CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalP /m_parameter->originalVelocity;
    double a = fabs(log10(stp/m_SplpSt1));
    double aMin = 0;
    double aMax = 0;
    double a1 = 0;
    double splDb = 0;

    //If angle is bigger than the switching Angle
    //or pressure side is mandatory
    if(m_CalcPressureSide){

        if(a<0.204){
            aMin = sqrt(67.552-886.788*pow(a,2)-8.219);
        }else if(a > 0.244){
            aMin = -142.795*pow(a,3)+103.656*pow(a,2)-57.757*a+6.006;
        }else{
            aMin = -32.665*a+3.981;
        }

        if(a<0.13){
            aMax = (sqrt(67.552-886.788*pow(a,2))-8.219);
        }else if(a > 0.321){
            aMax = (-4.669*pow(a,3)+3.491*pow(a,2)-16.699*a+1.149);
        }else{
            aMax = (-15.901*a+1.098);
        }

        a1 =aMin+ m_A1Ar *(aMax-aMin);
        splDb = m_SplpFirstTerm +a1+m_SplpK13+m_SplpDeltaK1;

	} else {
		splDb = std::numeric_limits<long>::lowest();
    }

    m_SPLpdB[posOpPoint][posFreq] = splDb;
    m_SPLpdBAW[posOpPoint][posFreq] = splDb + AWeighting[posFreq];
    m_SPLpdBBW[posOpPoint][posFreq] = splDb + BWeighting[posFreq];
    m_SPLpdBCW[posOpPoint][posFreq] = splDb + CWeighting[posFreq];

//	qDebug() << "SPLp -> stp("<< stp <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax <<")\t"
//			 << "a1("<< a1 <<")\t" << "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLpdBAW[posOpPoint][posFreq]
//			 << ")\t"<< "splDb-BW("<< m_SPLpdBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("
//			 << m_SPLpdBCW[posOpPoint][posFreq] <<")\t";
}

void NoiseCalculation::calculate() {
    setupVectors();

	QList<NoiseOpPoint*> noiseOpPoints = m_parameter->prepareNoiseOpPointList();
    for (int posOpPoint = 0; posOpPoint < noiseOpPoints.size(); ++posOpPoint) {
		NoiseOpPoint *nop = noiseOpPoints[posOpPoint];

		m_OASPL[posOpPoint] = 0;
        m_OASPLA[posOpPoint] = 0;
        m_OASPLB[posOpPoint] = 0;
        m_OASPLC[posOpPoint] = 0;
        m_SPLALOG[posOpPoint] = 0;
        m_SPLSLOG[posOpPoint] = 0;
        m_SPLPLOG[posOpPoint] = 0;

//        qDebug() << "======================== OpPoint ========================";
//        qDebug() << "Alpha deg: " << nop->AlphaDeg();
//        qDebug() << "Reynolds: " << nop->Reynolds();

        bool dStarOrder = false;

        //When angle is negative D* search must be inverted
        if(nop->getAlphaDegree() < 0){
            dStarOrder = true;
        }


        if (m_parameter->opPointSource == NoiseParameter::OnePolar ||
			m_parameter->opPointSource == NoiseParameter::MultiplePolars)
		{

            //For XFoil model
            m_DStarInterpolatedS = getDStarInterpolated(dStarOrder,nop);
            m_DStarInterpolatedP = getDStarInterpolated(!dStarOrder,nop);

            m_DStarFinalS = m_DStarInterpolatedS * m_parameter->originalChordLength * m_parameter->dStarScalingFactor;
            m_DStarFinalP = m_DStarInterpolatedP * m_parameter->originalChordLength * m_parameter->dStarScalingFactor;
        } else if (m_parameter->opPointSource == NoiseParameter::OriginalBpm) {

            m_DStarInterpolatedS = 0;
            m_DStarInterpolatedP = 0;

            //For BPM model
			
            m_DStarFinalS = getBPMThickness(nop, SuctionSide) * m_parameter->dStarScalingFactor;
            m_DStarFinalP = getBPMThickness(nop, PressureSide) * m_parameter->dStarScalingFactor;
        }

        m_EddyMachNumber = m_parameter->originalMach * m_parameter->eddyConvectionMach;

//        qDebug() << "Linear DStar interpolated Top/Suction: " << m_DStarInterpolatedS;
//        qDebug() << "Final DStar Top/Suction: " << m_DStarFinalS;
//        qDebug() << "Linear DStar interpolated Bottom/Pressure: " << m_DStarInterpolatedP;
//        qDebug() << "Final DStar Bottom/Pressure: " << m_DStarFinalP;
//        qDebug() << "Mach Number: " << m_NoiseParameter->m_OriginalMach;
//        qDebug() << "Velocity: " << m_NoiseParameter->m_OriginalVelocity;
//        qDebug() << "Eddy Mach Number: " << m_EddyMachNumber;

        m_SwAlpha1 = 23.43 * m_parameter->originalMach + 4.651;
        m_SwAlpha = fmin(m_SwAlpha1, SWITCHING_ANGLE2);

        if( nop->getAlphaDegree() <= m_SwAlpha){
            m_AlphaBigSw = false;
        }else{
            m_AlphaBigSw = true;
        }

//        qDebug() << "SwAngle1: " << m_SwAlpha1;
//        qDebug() << "SwAngle calculated: " << m_SwAlpha;


        m_CalcSeparatedFlow = false;
        m_CalcPressureSide = false;
        m_CalcSuctionSide = false;

        if(m_AlphaBigSw && m_parameter->separatedFlow){
//            qDebug() << "Only separated flow source will be calculated";
            m_CalcSeparatedFlow = true;
            m_CalcPressureSide = false;
            m_CalcSuctionSide = false;
        }else{
            if(m_parameter->separatedFlow){
//                qDebug() << "Separated flow source will be calculated";
                m_CalcSeparatedFlow = true;
            }

            if(m_parameter->pressureSide){
//                qDebug() << "Pressure side source will be calculated";
                m_CalcPressureSide = true;
            }

            if(m_parameter->suctionSide){
//                qDebug() << "Suction side source will be calculated";
                m_CalcSuctionSide = true;
            }
        }

        preCalcA1(nop);

        if(m_CalcSeparatedFlow){
            preCalcSPLa(nop);
        }

        //If angle is bigger than the switching Angle
        //or suction side is mandatory
        if(m_CalcSuctionSide){
            preCalcSPLs(nop);
        }

        //If angle is bigger than the switching Angle
        //or pressure side is mandatory
        if(m_CalcPressureSide){
            preCalcSPLp(nop);
        }

        //For each frequency
        //for (unsigned int posFreq = 0; posFreq < 3; ++posFreq) {
        for (unsigned int posFreq = 0; posFreq < FREQUENCY_TABLE_SIZE; ++posFreq) {
//			qDebug() << "==== Band Frequency ====";
//			qDebug() << "Freq: [" << (posFreq+1) << "] " << Noise::CENTRAL_BAND_FREQUENCY[posFreq] ;
			
            if (m_CalcSeparatedFlow) {
                calcSPLa(nop->getAlphaDegree(),posOpPoint,posFreq);
            }

            //If angle is bigger than the switching Angle
            //or suction side is mandatory
//			if(m_CalcSuctionSide){
                calcSPLs(posOpPoint,posFreq);
//			}

            //If angle is bigger than the switching Angle
            //or pressure side is mandatory
//            if(m_CalcPressureSide){
                calcSPLp(posOpPoint,posFreq);
//            }

            double splDbConsolidated = 0.0;

            if(m_CalcSeparatedFlow)
                splDbConsolidated += pow(10,(m_SPLadB[posOpPoint][posFreq]/10));

            if(m_CalcPressureSide)
                splDbConsolidated += pow(10,(m_SPLsdB[posOpPoint][posFreq]/10));

            if(m_CalcSuctionSide)
                splDbConsolidated += pow(10,(m_SPLpdB[posOpPoint][posFreq]/10));

            m_SPLdB[posOpPoint][posFreq] = 10*log10( splDbConsolidated );
            //m_SPLdB[posOpPoint][posFreq] = 10*log10(pow(10,(m_SPLadB[posOpPoint][posFreq]/10))+pow(10,(m_SPLsdB[posOpPoint][posFreq]/10))+pow(10,(m_SPLpdB[posOpPoint][posFreq]/10)));
            m_SPLdBAW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq] + AWeighting[posFreq];
            m_SPLdBBW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq] + BWeighting[posFreq];
            m_SPLdBCW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq] + CWeighting[posFreq];

//            qDebug() << "SPLdb(" << m_SPLdB[posOpPoint][posFreq] << ") " << "SPLdbAW(" << m_SPLdBAW[posOpPoint][posFreq] << ") " << "SPLdbBW(" << m_SPLdBBW[posOpPoint][posFreq] << ") " << "SPLdbCW(" << m_SPLdBCW[posOpPoint][posFreq] << ") ";


            m_OASPL[posOpPoint] += pow(10,(m_SPLdB[posOpPoint][posFreq]/10));
            m_OASPLA[posOpPoint] += pow(10,(m_SPLdBAW[posOpPoint][posFreq]/10));
            m_OASPLB[posOpPoint] += pow(10,(m_SPLdBBW[posOpPoint][posFreq]/10));
            m_OASPLC[posOpPoint] += pow(10,(m_SPLdBCW[posOpPoint][posFreq]/10));

            m_SPLALOG[posOpPoint] += pow(10,(m_SPLadB[posOpPoint][posFreq]/10));
            m_SPLSLOG[posOpPoint] += pow(10,(m_SPLsdB[posOpPoint][posFreq]/10));
            m_SPLPLOG[posOpPoint] += pow(10,(m_SPLpdB[posOpPoint][posFreq]/10));
        }

        m_OASPL[posOpPoint] = 10*log10(m_OASPL[posOpPoint]);
        m_OASPLA[posOpPoint] = 10*log10(m_OASPLA[posOpPoint]);
        m_OASPLB[posOpPoint] = 10*log10(m_OASPLB[posOpPoint]);
        m_OASPLC[posOpPoint] = 10*log10(m_OASPLC[posOpPoint]);

        m_SPLALOG[posOpPoint] = 10*log10(m_SPLALOG[posOpPoint]);
        m_SPLSLOG[posOpPoint] = 10*log10(m_SPLSLOG[posOpPoint]);
        m_SPLPLOG[posOpPoint] = 10*log10(m_SPLPLOG[posOpPoint]);
    }
	qDeleteAll(noiseOpPoints);
	
//	// transform negative values to zero  // NM this apparently wasn't intended...
//	for (TwoDVector *vector2d : {&m_SPLadB, &m_SPLsdB, &m_SPLpdB}) {
//		for (int i = 0; i < vector2d->size(); ++i) {
//			for (int j = 0; j < (*vector2d)[i].size(); ++j) {
//				(*vector2d)[i][j] = std::max(0.0, (*vector2d)[i][j]);
//			}
//		}
//	}
}

void NoiseCalculation::setupVectors() {
    // Clear all results is case of simulation edition
    m_SPLdB.clear();
    m_SPLdBAW.clear();
    m_SPLdBBW.clear();
    m_SPLdBCW.clear();
    m_SPLpdB.clear();
    m_SPLpdBAW.clear();
    m_SPLpdBBW.clear();
    m_SPLpdBCW.clear();
    m_SPLsdB.clear();
    m_SPLsdBAW.clear();
    m_SPLsdBBW.clear();
    m_SPLsdBCW.clear();
    m_SPLadB.clear();
    m_SPLadBAW.clear();
    m_SPLadBBW.clear();
    m_SPLadBCW.clear();

    m_OASPL.clear();
    m_OASPLA.clear();
    m_OASPLB.clear();
    m_OASPLC.clear();
    m_SPLALOG.clear();
    m_SPLSLOG.clear();
    m_SPLPLOG.clear();

    // Resize vectors acording to OpPoints total
    unsigned int size = 0;
	if (m_parameter->opPointSource == NoiseParameter::OnePolar ||
		m_parameter->opPointSource == NoiseParameter::MultiplePolars)
	{
		size = m_parameter->analyzedOpPoints.size();
	} else if (m_parameter->opPointSource == NoiseParameter::OriginalBpm) {
		size = 1;
	}

	m_SPLdB.resize   (size);
    m_SPLdBAW.resize (size);
    m_SPLdBBW.resize (size);
    m_SPLdBCW.resize (size);
    m_SPLpdB.resize  (size);
    m_SPLpdBAW.resize(size);
    m_SPLpdBBW.resize(size);
    m_SPLpdBCW.resize(size);
    m_SPLsdB.resize  (size);
    m_SPLsdBAW.resize(size);
    m_SPLsdBBW.resize(size);
    m_SPLsdBCW.resize(size);
    m_SPLadB.resize  (size);
    m_SPLadBAW.resize(size);
    m_SPLadBBW.resize(size);
    m_SPLadBCW.resize(size);

    m_OASPL.resize  (size);
    m_OASPLA.resize (size);
    m_OASPLB.resize (size);
    m_OASPLC.resize (size);
    m_SPLALOG.resize(size);
    m_SPLSLOG.resize(size);
    m_SPLPLOG.resize(size);

    // Resize for each OpPoint the frequency table
    for (unsigned int i = 0; i < size; ++i) {
        m_SPLdB[i].resize   (FREQUENCY_TABLE_SIZE);
        m_SPLdBAW[i].resize (FREQUENCY_TABLE_SIZE);
        m_SPLdBBW[i].resize (FREQUENCY_TABLE_SIZE);
        m_SPLdBCW[i].resize (FREQUENCY_TABLE_SIZE);
        m_SPLpdB[i].resize  (FREQUENCY_TABLE_SIZE);
        m_SPLpdBAW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLpdBBW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLpdBCW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLsdB[i].resize  (FREQUENCY_TABLE_SIZE);
        m_SPLsdBAW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLsdBBW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLsdBCW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLadB[i].resize  (FREQUENCY_TABLE_SIZE);
        m_SPLadBAW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLadBBW[i].resize(FREQUENCY_TABLE_SIZE);
        m_SPLadBCW[i].resize(FREQUENCY_TABLE_SIZE);
    }
}
