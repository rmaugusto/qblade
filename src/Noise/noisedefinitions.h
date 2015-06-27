#ifndef NOISEDEFINITIONS_H
#define NOISEDEFINITIONS_H

#include <vector>

namespace Noise{

    enum DeltaSource { XFoilCalculation, OriginalBPM, ImportData };
    enum TransitionType { FullyTurbulent, TransitionFlow };
    enum AirfoilSide { PressureSide, SuctionSide };

    const unsigned short IIVX = 302;	//300 = number of nodes along bl on one side of airfoil and wake
    const unsigned short IISX = 3;	//number of airfoil sides

    typedef double ArrChordStations[Noise::IIVX][Noise::IISX];
    typedef double TwoDArrDStars[Noise::IIVX][Noise::IISX];


    //1/3 octave band frequency
    const int FREQUENCY_TABLE_SIZE = 30;
    const double CENTRAL_BAND_FREQUENCY[FREQUENCY_TABLE_SIZE] = {25,31.5,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500,16000,20000};
    const double AWeighting[FREQUENCY_TABLE_SIZE] =  {-44.7,-39.4,-34.6,-30.2,-26.2,-22.5,-19.1,-16.1,-13.4,-10.9,-8.6,-6.6,-4.8,-3.2,-1.9,-0.8,0.0,0.6,1.0,1.2,1.3,1.2,1.0,0.5,-0.1,-1.1,-2.5,-4.3,-6.6,-9.3};
    const double BWeighting[FREQUENCY_TABLE_SIZE] =  {-20.4,-17.1,-14.2,-11.6,-9.3,-7.4,-5.6,-4.2,-3.0,-2.0,-1.3,-0.8,-0.5,-0.3,-0.1,0.0,0.0,0.0,0.0,-0.1,-0.2,-0.4,-0.7,-1.2,-1.9,-2.9,-4.3,-6.1,-8.4,-11.1};
    const double CWeighting[FREQUENCY_TABLE_SIZE] =  {-4.4,-3.0,-2.0,-1.3,-0.8,-0.5,-0.3,-0.2,-0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-0.1,-0.2,-0.3,-0.5,-0.8,-1.3,-2.0,-3.0,-4.4,-6.2,-8.5,-11.2};

    const double SWITCHING_ANGLE2 = 12.5;

    typedef std::vector< std::vector<double> > TwoDVector;
}


#endif // NOISEDEFINITIONS_H

