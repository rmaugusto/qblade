#include "noisecalculation.h"
#include "noiseparameter.h"

NoiseCalculation::NoiseCalculation()
{

    m_NoiseParameter = 0;

}

NoiseCalculation::~NoiseCalculation()
{
    if(m_NoiseParameter){
        delete m_NoiseParameter;
        m_NoiseParameter = 0;
    }
}

NoiseParameter *NoiseCalculation::NoiseParam() const
{
    return m_NoiseParameter;
}

void NoiseCalculation::setNoiseParam(NoiseParameter *np)
{
    m_NoiseParameter = np;
}

double NoiseCalculation::getDStarInterpolated(bool top)
{

    bool upDownFind = false;
    double chordUpStream = 0;
    double chordDownStream = 0;
    double dStarUpStream = 0;
    double dStarDownStream = 0;

    //For positive alpha use TopSide else BottomSide
    int side = top ? 1 : 2;

    //Find closest station assuming crescent order on chordStation
    for(int i =0;i<Noise::IIVX;i++){

        //Current chord
        double ccur = this->NoiseParam()->ChordStations()[i][side];
        //Current D*
        double cd = this->NoiseParam()->DStars()[i][side];

        //prev chord
        double prev_ccur = i==0?ccur : this->NoiseParam()->ChordStations()[i-1][side];
        //prev chord
        double prev_d = i==0?cd: this->NoiseParam()->DStars()[i-1][side];

        //qDebug() << "i: " << i << " - " << ccur;

        if(ccur > this->NoiseParam()->DStarChordStation()){
            chordUpStream = ccur;
            chordDownStream = prev_ccur;
            dStarUpStream = cd;
            dStarDownStream = prev_d;

            qDebug() << "Chord UpStream: " << chordUpStream;
            qDebug() << "Chord DownStream: " << chordDownStream;
            qDebug() << "D* UpStream: " << dStarUpStream;
            qDebug() << "D* DownStream: " << dStarDownStream;

            upDownFind = true;
            break;
        }
    }

    if(!upDownFind){
        qWarning("Can not find upstream and downstream. D* Interpolated will be zero !");
        return 0;
    }

    return ((dStarUpStream-dStarDownStream)*(this->NoiseParam()->DStarChordStation()-chordDownStream)/(chordUpStream-chordDownStream))+dStarDownStream;

}

double NoiseCalculation::getDL(){

    //Dh, Low Freq. Directivity Factor
    if(m_NoiseParameter->LowFreq()){
        return (2*(pow(sin(m_NoiseParameter->DirectivityGreek()),2))*(pow(sin(m_NoiseParameter->DirectivityPhi()),2)))/pow((1+m_NoiseParameter->OriginalMach()*cos(m_NoiseParameter->DirectivityGreek())),4);
    }else{
        return 1;
    }

}

double NoiseCalculation::getDH()
{    
    //Dh, High Freq. Directivity Factor
    if(m_NoiseParameter->HighFreq()){
        return (2*pow((sin((m_NoiseParameter->DirectivityGreek()/2))),2)*pow((sin(m_NoiseParameter->DirectivityPhi())),2))/((1+m_NoiseParameter->OriginalMach()*cos(m_NoiseParameter->DirectivityGreek()))*(pow((1+(m_NoiseParameter->OriginalMach()-m_EddyMachNumber)*cos(m_NoiseParameter->DirectivityPhi())),2)));
    }else{
        return 1;
    }


}

double NoiseCalculation::getSt1()
{
    return 0.02*pow(m_NoiseParameter->OriginalMach(),-0.6);
}

double NoiseCalculation::getSt2(NoiseOpPoint* nop)
{
    double st1 = getSt1();
    double st2 = 0;

    if( nop->AlphaDeg() < 1.33 ){
        st2 = st1;
    }else if( nop->AlphaDeg() > 12.5 ){
        st2 = st1*4.72;
    }else{
        st2 = st1 * pow(10,0.0054* pow((nop->AlphaDeg()-1.33),2));
    }

    return st2;

}

double NoiseCalculation::getBPMThickness(NoiseOpPoint * nop, Noise::AirfoilSide as)
{

    double dStarCF = 0;
    double dStarCT = 0;
    double dStar = 0;
    double bpm = 0;


    if(m_NoiseParameter->Transition() == Noise::FullyTurbulent){

        if(nop->Reynolds() > 300000){
            dStarCF = pow(10,(3.411-1.5397*log10(nop->Reynolds())+0.1059* pow(log10(nop->Reynolds()),2)));
        }else{
            dStarCF = 0.0601*(pow(nop->Reynolds(),-0.114));
        }
        dStar = dStarCF * m_NoiseParameter->OriginalChordLength();

    }else{

        dStarCT = pow(10,(3.0187-1.5397*log10(nop->Reynolds())+0.1059* pow(log10(nop->Reynolds()),2)));
        dStar = dStarCT * m_NoiseParameter->OriginalChordLength();

    }

    if(nop->AlphaDeg() == 0){

        bpm = dStar;

    }else{
        double corFactor = 0;

        if(as==Noise::PressureSide){
            corFactor = pow(10,(-0.0432*nop->AlphaDeg()+0.00113*pow(nop->AlphaDeg(),2)));
            bpm = corFactor * dStar;
        }else{

            if(m_NoiseParameter->Transition() == Noise::FullyTurbulent){

                if(nop->AlphaDeg() < 5){
                    corFactor = pow(10,(0.0679 * nop->AlphaDeg()));
                }else if(nop->AlphaDeg() >= 5 && nop->AlphaDeg() <= 12.5){
                    corFactor = 0.381*(pow(10,(0.1516*nop->AlphaDeg())));
                }else{
                    corFactor = 14.296*(pow(10,(0.0258*nop->AlphaDeg())));
                }

            }else{

                if(nop->AlphaDeg() < 7.5){
                    corFactor = pow(10,(0.0679*nop->AlphaDeg()));
                }else if(nop->AlphaDeg() >= 7.5 && nop->AlphaDeg() <= 12.5){
                    corFactor = 0.0162*(pow(10,(0.3066*nop->AlphaDeg())));
                }else{
                    corFactor = 54.42*(pow(10,(0.0258*nop->AlphaDeg())));
                }

            }

            bpm = corFactor * dStar;

        }

    }


    return bpm;

}

double NoiseCalculation::getK1(NoiseOpPoint* nop)
{
    double k1 = 0;
    if(nop->Reynolds()<247000){
        k1 = -4.31*log10(nop->Reynolds())+156.3;
    }else if(nop->Reynolds()>800000){
        k1 = 128.5;
    }else{
        k1 = -9*log10(nop->Reynolds())+181.6;
    }

    return k1;
}
Noise::TwoDVector NoiseCalculation::SPLdB() const
{
    return m_SPLdB;
}

Noise::TwoDVector NoiseCalculation::SPLdBAW() const
{
    return m_SPLdBAW;
}

Noise::TwoDVector NoiseCalculation::SPLdBBW() const
{
    return m_SPLdBBW;
}



Noise::TwoDVector NoiseCalculation::SPLdBCW() const
{
    return m_SPLdBCW;
}

Noise::TwoDVector NoiseCalculation::SPLpdB() const
{
    return m_SPLpdB;
}

Noise::TwoDVector NoiseCalculation::SPLadB() const
{
    return m_SPLadB;
}
double* NoiseCalculation::OASPL()
{
    return m_OASPL;
}

Noise::TwoDVector NoiseCalculation::SPLsdB() const
{
    return m_SPLsdB;
}

void NoiseCalculation::preCalcSPLa(NoiseOpPoint* nop)
{
    qDebug() << "---> SPLa CALCULATION";

    m_SplaFirstTerm=0;

    //If angle is smaller than the switching Angle
    //use dH and B else use dL and A1
    if( nop->AlphaDeg() <= m_SwAlpha ){
        qDebug() << "SPLa No need to calculate SPLs and SPLp";
        qDebug() << "SPLa dH: " << getDH();
        m_SplaFirstTerm = 10*log10((pow(m_NoiseParameter->OriginalMach(),5)*m_NoiseParameter->WettedLength()*getDH()*m_DStarFinalS/pow(m_NoiseParameter->DistanceObsever(),2)));

        if(nop->Reynolds() < 95200){
            m_SplaBo = 0.3;
        }else if(nop->Reynolds() > 857000){
            m_SplaBo = 0.56;
        }else{
            m_SplaBo = (-0.000000000000448*( pow((nop->Reynolds()-857000),2)+0.56));
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
        m_AlphaBigSw = false;

        qDebug() << "SPLA Bo " << m_SplaBo;
        qDebug() << "SPLA bMin " << m_SplaBMin;
        qDebug() << "SPLA bMax " << m_SplaBMax;
        qDebug() << "SPLA Br " << m_SplaBr;


    }else{
        qDebug() << "SPLa Need to calculate SPLs and SPLp";
        qDebug() << "SPLa dL: " << getDL();
        m_SplaFirstTerm = 10*log10((pow(m_NoiseParameter->OriginalMach(),5)*m_NoiseParameter->WettedLength()*getDL()*m_DStarFinalS/pow(m_NoiseParameter->DistanceObsever(),2)));
        m_ChordBasedReynolds = nop->Reynolds() * 3;

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


        qDebug() << "SPLA ChordBasedReynolds " << m_ChordBasedReynolds;
        qDebug() << "SPLA Ao " << m_SplaAo;
        qDebug() << "SPLA aMin " << m_SplaAMin;
        qDebug() << "SPLA aMax " << m_SplaAMax;
        qDebug() << "SPLA Ar " << m_SplaAr;


        m_AlphaBigSw = true;
    }


    m_SplaSt1 = getSt1();
    m_SplaSt2 = getSt2(nop);


    m_SplaGamma = 27.094 * m_NoiseParameter->OriginalMach() + 3.32;
    m_SplaBeta = 72.65*m_NoiseParameter->OriginalMach()+10.74;
    m_SplaBetaZero = -34.19*m_NoiseParameter->OriginalMach()-13.82;

    m_SplaK1 = getK1(nop);

    m_SplaK2 = 0;
    if(nop->AlphaDeg() < (m_SwAlpha1-m_SplaGamma)){
        m_SplaK2 = m_SplaK1 - 1000;
    }else if(nop->AlphaDeg() > (m_SwAlpha1+m_SplaGamma)){
        m_SplaK2 = m_SplaK1 - 12;
    }else{
        m_SplaK2 = m_SplaK1 + (sqrt( pow(m_SplaBeta,2)-pow((m_SplaBeta/m_SplaGamma),2)* pow((nop->AlphaDeg()-m_SwAlpha1),2)  )+m_SplaBetaZero);
    }


    qDebug() << "SPLa firstTerm: " << m_SplaFirstTerm;
    qDebug() << "SPLa st1: " << m_SplaSt1;
    qDebug() << "SPLa st2: " << m_SplaSt2;
    qDebug() << "SPLa gamma: " << m_SplaGamma;
    qDebug() << "SPLa gamma_zero: " << m_SwAlpha1;
    qDebug() << "SPLa beta: " << m_SplaBeta;
    qDebug() << "SPLa betaZero: " << m_SplaBetaZero;
    qDebug() << "SPLa reynolds: " << nop->Reynolds();
    qDebug() << "SPLa k1: " << m_SplaK1;
    qDebug() << "SPLa k2: " << m_SplaK2;


}

void NoiseCalculation::preCalcSPLs(NoiseOpPoint *nop)
{

    qDebug() << "---> SPLs CALCULATION";
    m_SplsFirstTerm=0;
    m_SplsFirstTerm = 10*log10((pow(m_NoiseParameter->OriginalMach(),5)*m_NoiseParameter->WettedLength()*getDH()*m_DStarFinalS/pow(m_NoiseParameter->DistanceObsever(),2)));

    qDebug() << "SPLs dH: " << getDH();
    qDebug() << "SPLs firstTerm: " << m_SplsFirstTerm;

    m_SplsSt1 = getSt1();
    m_SplsSt2 = getSt2(nop);
    m_SplsK1 = getK1(nop);
    m_SplsSt1Bar = (m_SplsSt1+m_SplsSt2)/2;
    m_SplsK13 = m_SplsK1-3;

    qDebug() << "SPLs st1: " << m_SplsSt1;
    qDebug() << "SPLs st2: " << m_SplsSt2;
    qDebug() << "SPLs st1Bar: " << m_SplsSt1Bar;
    qDebug() << "SPLs k1: " << m_SplsK1;
    qDebug() << "SPLs k1-3: " << m_SplsK13;

}

void NoiseCalculation::preCalcSPLp(NoiseOpPoint *nop)
{
    qDebug() << "---> SPLp CALCULATION";
    m_SplpFirstTerm=0;
    m_SplpFirstTerm = 10*log10((pow(m_NoiseParameter->OriginalMach(),5)*m_NoiseParameter->WettedLength()*getDH()*m_DStarFinalP/pow(m_NoiseParameter->DistanceObsever(),2)));

    qDebug() << "SPLp dH: " << getDH();
    qDebug() << "SPLp firstTerm: " << m_SplpFirstTerm;

    m_SplpSt1 = getSt1();
    m_SplpK1 = getK1(nop);
    m_SplpK13 = m_SplpK1-3;
    m_SplpDeltaK1 = 0;
    m_ReynoldsBasedDisplacement = 1.225 * m_NoiseParameter->OriginalVelocity() * m_DStarFinalP / 0.0000178;

    if(m_ReynoldsBasedDisplacement > 5000){
        m_SplpDeltaK1 = 0;
    }else{
        m_SplpDeltaK1 = ( nop->AlphaDeg() *(1.43*log10(m_ReynoldsBasedDisplacement)-5.29));
    }

    qDebug() << "Reynolds Based Displacement: " << m_ReynoldsBasedDisplacement;
    qDebug() << "SPLp st1: " << m_SplpSt1;
    qDebug() << "SPLp k1: " << m_SplpK1;
    qDebug() << "SPLp k1-3: " << m_SplpK13;
    qDebug() << "SPLp DeltaK1: " << m_SplpDeltaK1;

}

void NoiseCalculation::calcSPLa(double alpha, int posOpPoint, int posFreq)
{

    double splDb = 0;

    //If angle is smaller than the switching Angle
    //use dH and B else use dL and A1
    if( alpha <= m_SwAlpha ){

        double sts = Noise::CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_NoiseParameter->OriginalVelocity();
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
        m_SPLadBAW[posOpPoint][posFreq] = splDb+Noise::AWeighting[posFreq];
        m_SPLadBBW[posOpPoint][posFreq] = splDb+Noise::BWeighting[posFreq];
        m_SPLadBCW[posOpPoint][posFreq] = splDb+Noise::CWeighting[posFreq];

        qDebug() << "SPLa -> sts("<< sts <<")\t"<< "b("<< b <<")\t"<< "bMin("<< bMin <<")\t"<< "bMax("<< bMax <<")\t"<< "db("<< db <<")\t"<< "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLadBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLadBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("<< m_SPLadBCW[posOpPoint][posFreq] <<")\t";

    }else{

        double sts = Noise::CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_NoiseParameter->OriginalVelocity();
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

        a1 =aMin+ m_SplaBr *(aMax-aMin);
        splDb = m_SplaFirstTerm +a1+m_SplaK2;

        m_SPLadB[posOpPoint][posFreq] = splDb;
        m_SPLadBAW[posOpPoint][posFreq] = splDb+Noise::AWeighting[posFreq];
        m_SPLadBBW[posOpPoint][posFreq] = splDb+Noise::BWeighting[posFreq];
        m_SPLadBCW[posOpPoint][posFreq] = splDb+Noise::CWeighting[posFreq];

        qDebug() << "SPLa -> sts("<< sts <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax <<")\t"<< "a1("<< a1 <<")\t"<< "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLadBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLadBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("<< m_SPLadBCW[posOpPoint][posFreq] <<")\t";
    }



}

void NoiseCalculation::calcSPLs(int posOpPoint,int posFreq)
{

    double sts = Noise::CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalS/m_NoiseParameter->OriginalVelocity();
    double a = fabs(log10(sts/m_SplsSt1Bar));
    double aMin = 0;
    double aMax = 0;
    double a1 = 0;
    double splDb = 0;

    //If angle is bigger than the switching Angle
    //or suction side is mandatory
    if(!m_AlphaBigSw || m_NoiseParameter->SuctionSide()){

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

        a1 =aMin+ m_SplaBr *(aMax-aMin);
        splDb = m_SplsFirstTerm +a1+m_SplsK13;

    }else{
        //use biggest negative number
        splDb = -std::numeric_limits<long>::max();
    }

    m_SPLsdB[posOpPoint][posFreq] = splDb;
    m_SPLsdBAW[posOpPoint][posFreq] = splDb+Noise::AWeighting[posFreq];
    m_SPLsdBBW[posOpPoint][posFreq] = splDb+Noise::BWeighting[posFreq];
    m_SPLsdBCW[posOpPoint][posFreq] = splDb+Noise::CWeighting[posFreq];

    qDebug() << "SPLs -> sts("<< sts <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax <<")\t"<< "a1("<< a1 <<")\t" << "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLsdBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLsdBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("<< m_SPLsdBCW[posOpPoint][posFreq] <<")\t";

}

void NoiseCalculation::calcSPLp(int posOpPoint,int posFreq)
{

    double stp = Noise::CENTRAL_BAND_FREQUENCY[posFreq]*m_DStarFinalP /m_NoiseParameter->OriginalVelocity();
    double a = fabs(log10(stp/m_SplpSt1));
    double aMin = 0;
    double aMax = 0;
    double a1 = 0;
    double splDb = 0;

    //If angle is bigger than the switching Angle
    //or pressure side is mandatory
    if(!m_AlphaBigSw || m_NoiseParameter->PressureSide() ){

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

        a1 =aMin+ m_SplaBr *(aMax-aMin);
        splDb = m_SplpFirstTerm +a1+m_SplpK13+m_SplpDeltaK1;

    }else{
        //Use biggest negative number
        splDb = -std::numeric_limits<long>::max();
    }

    m_SPLpdB[posOpPoint][posFreq] = splDb;
    m_SPLpdBAW[posOpPoint][posFreq] = splDb+Noise::AWeighting[posFreq];
    m_SPLpdBBW[posOpPoint][posFreq] = splDb+Noise::BWeighting[posFreq];
    m_SPLpdBCW[posOpPoint][posFreq] = splDb+Noise::CWeighting[posFreq];

    qDebug() << "SPLp -> stp("<< stp <<")\t"<< "a("<< a <<")\t"<< "aMin("<< aMin <<")\t"<< "aMax("<< aMax <<")\t"<< "a1("<< a1 <<")\t" << "splDb("<< splDb <<")\t"<< "splDb-AW("<< m_SPLpdBAW[posOpPoint][posFreq] <<")\t"<< "splDb-BW("<< m_SPLpdBBW[posOpPoint][posFreq] <<")\t"<< "splDb-CW("<< m_SPLpdBCW[posOpPoint][posFreq] <<")\t";


}

void NoiseCalculation::calculate()
{
    if(!m_NoiseParameter){
        qWarning("Parameter not defined !");
        return;
    }

    setupVectors();

    //For each OpPoint
    for (unsigned int posOpPoint = 0; posOpPoint < m_NoiseParameter->OpPoints().size(); ++posOpPoint) {

        NoiseOpPoint * nop = m_NoiseParameter->OpPoints()[posOpPoint];

        m_OASPL[posOpPoint] = 0;

        qDebug() << "======================== OpPoint ========================";
        qDebug() << "Alpha deg: " << nop->AlphaDeg();
        qDebug() << "Reynolds: " << nop->Reynolds();

        m_DStarInterpolatedS = getDStarInterpolated(true);
        m_DStarInterpolatedP = getDStarInterpolated(false);

        if( m_NoiseParameter->DeltaSouce() == Noise::XFoilCalculation){
            //For XFoil model
            m_DStarFinalS = m_DStarInterpolatedS * m_NoiseParameter->OriginalChordLength() * m_NoiseParameter->DStarScalingFactor();
            m_DStarFinalP = m_DStarInterpolatedP * m_NoiseParameter->OriginalChordLength() * m_NoiseParameter->DStarScalingFactor();
        }else if( m_NoiseParameter->DeltaSouce() == Noise::OriginalBPM){
            //For BPM model
            m_DStarFinalS = getBPMThickness(nop,Noise::SuctionSide) * m_NoiseParameter->DStarScalingFactor();
            m_DStarFinalP = getBPMThickness(nop,Noise::PressureSide) * m_NoiseParameter->DStarScalingFactor();
        }else{
            //NOT SUPPORTED YET
        }

        m_EddyMachNumber = m_NoiseParameter->OriginalMach() * m_NoiseParameter->EddyConvectionMach();

        qDebug() << "Linear DStar interpolated Top/Suction: " << m_DStarInterpolatedS;
        qDebug() << "Final DStar Top/Suction: " << m_DStarFinalS;
        qDebug() << "Linear DStar interpolated Bottom/Pressure: " << m_DStarInterpolatedP;
        qDebug() << "Final DStar Bottom/Pressure: " << m_DStarFinalP;
        qDebug() << "Mach Number: " << m_NoiseParameter->OriginalMach();
        qDebug() << "Velocity: " << m_NoiseParameter->OriginalVelocity();
        qDebug() << "Eddy Mach Number: " << m_EddyMachNumber;

        m_SwAlpha1 = 23.43 * m_NoiseParameter->OriginalMach() + 4.651;
        m_SwAlpha = fmin(m_SwAlpha1,Noise::SWITCHING_ANGLE2);

        qDebug() << "SwAngle1: " << m_SwAlpha1;
        qDebug() << "SwAngle calculated: " << m_SwAlpha;

        if(m_NoiseParameter->SeparatedFlow()){
            preCalcSPLa(nop);
        }

        //If angle is bigger than the switching Angle
        //or suction side is mandatory
        if(!m_AlphaBigSw || m_NoiseParameter->SuctionSide()){
            preCalcSPLs(nop);
        }

        //If angle is bigger than the switching Angle
        //or pressure side is mandatory
        if(!m_AlphaBigSw || m_NoiseParameter->PressureSide()){
            preCalcSPLp(nop);
        }


        //For each frequency
        //for (unsigned int posFreq = 0; posFreq < 3; ++posFreq) {
        for (unsigned int posFreq = 0; posFreq < Noise::FREQUENCY_TABLE_SIZE; ++posFreq) {

            qDebug() << "==== Band Frequency ====";
            qDebug() << "Freq: [" << (posFreq+1) << "] " << Noise::CENTRAL_BAND_FREQUENCY[posFreq] ;

            if(m_NoiseParameter->SeparatedFlow()){
                calcSPLa(nop->AlphaDeg(),posOpPoint,posFreq);
            }

            //If angle is bigger than the switching Angle
            //or suction side is mandatory
            if(!m_AlphaBigSw || m_NoiseParameter->SuctionSide()){
                calcSPLs(posOpPoint,posFreq);
            }

            //If angle is bigger than the switching Angle
            //or pressure side is mandatory
            if(!m_AlphaBigSw || m_NoiseParameter->PressureSide()){
                calcSPLp(posOpPoint,posFreq);
            }

            m_SPLdB[posOpPoint][posFreq] = 10*log10(pow(10,(m_SPLadB[posOpPoint][posFreq]/10))+pow(10,(m_SPLsdB[posOpPoint][posFreq]/10))+pow(10,(m_SPLpdB[posOpPoint][posFreq]/10)));
            m_SPLdBAW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq]+Noise::AWeighting[posFreq];
            m_SPLdBBW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq]+Noise::BWeighting[posFreq];
            m_SPLdBCW[posOpPoint][posFreq] =m_SPLdB[posOpPoint][posFreq]+Noise::CWeighting[posFreq];

            qDebug() << "SPLdb(" << m_SPLdB[posOpPoint][posFreq] << ") " << "SPLdbAW(" << m_SPLdBAW[posOpPoint][posFreq] << ") " << "SPLdbBW(" << m_SPLdBBW[posOpPoint][posFreq] << ") " << "SPLdbCW(" << m_SPLdBCW[posOpPoint][posFreq] << ") ";


            m_OASPL[posOpPoint] += pow(10,(m_SPLdB[posOpPoint][posFreq]/10));

        }

        m_OASPL[posOpPoint] = 10*log10(m_OASPL[posOpPoint]);
    }

}

void NoiseCalculation::setupVectors()
{

    //Resize vectors acording to OpPoints total
    m_SPLdB.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLdBAW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLdBBW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLdBCW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLpdB.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLpdBAW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLpdBBW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLpdBCW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLsdB.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLsdBAW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLsdBBW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLsdBCW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLadB.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLadBAW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLadBBW.resize( m_NoiseParameter->OpPoints().size() );
    m_SPLadBCW.resize( m_NoiseParameter->OpPoints().size() );

    //Resize for each OpPoint the frequency table
    for (unsigned int i = 0; i < m_NoiseParameter->OpPoints().size(); ++i) {
        m_SPLdB[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLdBAW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLdBBW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLdBCW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLpdB[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLpdBAW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLpdBBW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLpdBCW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLsdB[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLsdBAW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLsdBBW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLsdBCW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLadB[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLadBAW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLadBBW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
        m_SPLadBCW[i].resize( Noise::FREQUENCY_TABLE_SIZE );
    }

}

