#ifndef PARAMS_H
#define PARAMS_H

//General
#define PI             3.14159265358979
#define MAXRECENTFILES         8
#define SETTINGSFORMAT    100623
#define VERSIONNUMBER    100055


// Applications
#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3

///////////////////////new code DM//////////////////
#define BLADEVIEW        33
#define BEM              9
#define BEMSIMVIEW        32
#define POLARVIEW       31
#define TURBINEVIEW       34
#define CHARSIMVIEW          35
#define SECTIONHIGHLIGHT 1420
#define INNERGEOM 1429
///////////////////////end new code DM////////////////

///////////////////////new code JW//////////////////
#define DMS            10
///////////////////////end new code JW////////////////

/////////////// new NM ///////////////
#define WINDFIELDMODULE 4299
#define FASTMODULE 4300
#define TURDMSMODULE 4301
#define MULTIDMSMODULE 4302
#define NOISEMODULE 4303
#define ROTDMSMODULE 4304
/////////// end new NM ///////////////
///////////new DM/////////
#define QFEMMODULE 4400
#define QLLTMODULE 4404
#define QFEMSTRUCTVIEW 4401
#define QFEMLOADINGVIEW 4403
#define QFEMTWODVIEW 4404

#define VMSTRESSES 4555
#define GLCUTPLANESETUP 4556
#define GLCUTPLANES 4557


#define CONSTANT    1
#define SIMFILE     2
#define AERODYN     3

#define REVOLUTIONS    1
#define TIMESTEPS      2
#define TIME           3

//3D analysis parameters
#define MAXSPANSECTIONS   200 // max number of sections on a half wing
#define MAXSPANSTATIONS   250 // max number of stations for LLT or VLM analysis
#define MAXCHORDPANELS	   50 // max number of panels in the chordwise direction
#define VLMMAXMATSIZE    5000 // max number of VLM panels over the whole plane
#define VLMHALF          2500

//Direct Design
#define MAXSTACKPOS        50 // max number of undo pictures on the stack in direct design
#define SPLINECONTROLSIZE  50 // maximum number of control points

//XFoil Direct Parameters
#define IQX  302	//300 = number of surface panel nodes + 6
#define IQX2 151	//IQX/2 added arcds
#define IWX   50	// number of wake panel nodes
#define IPX    6	//6 number of qspec[s] distributions
#define ISX    3	//number of airfoil sides
#define IBX  604	//600 number of buffer airfoil nodes = 2*IQX
#define IZX  350	//350 = number of panel nodes [airfoil + wake]
#define IVX  302	//300 = number of nodes along bl on one side of airfoil and wake

//XFoil INVERSE parameters
#define ICX 257 // number of circle-plane points for complex mapping   ( 2^n  + 1 )
#define IMX 64  // number of complex mapping coefficients  Cn
#define IMX4 16 // = IMX/4 added arcds

//Misc
#define VLMSTREAMLINES          1220
#define WINGSURFACES            1300
#define TOPSURFACES             1310
#define BOTTOMSURFACES          1311
#define WINGOUTLINE             1304
#define WING2OUTLINE            1305
#define GLWINGWAKEPANELS          1383
#define GLBODYMESHPANELS        1402
#define GLBLADEOUTLINES         1411
#define GLTURBINECOORDINATES      1412
#define GLWINDFIELD               1415


typedef enum{FIXEDSPEEDPOLAR = 1, FIXEDLIFTPOLAR = 2, RUBBERCHORDPOLAR = 3, FIXEDAOAPOLAR = 4, STABILITYPOLAR = 7} enumPolarType;  // NM TODO seems like only FIXEDAOAPOLAR is used, the rest is just lots of garbage

#endif // PARAMS_H
