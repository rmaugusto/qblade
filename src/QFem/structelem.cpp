#include "structelem.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <typeinfo>
#include <assert.h>
#include <math.h>
const double PI  =3.141592653589793238462;
using Eigen::Matrix2d;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector4d;
using namespace ClipperLib;
using namespace std;
StructElem::StructElem(Node _NodeID ,std::vector<double> _xCoords,
                       std::vector<double> _yCoords,double _ChordAngle,
                       double _E,double _ChordLength,double ShellDensity, double _LocalXOffset, double _LocalYOffset)://shell density is the material density of the shell kg/m^3
    NodeID(_NodeID), ShellE(_E), Hollow(false), WithSpar(false), JPrime(0.0), ChordAngle(_ChordAngle),
    LocalYOffset(_LocalYOffset), LocalXOffset(_LocalXOffset), xCoords(_xCoords), yCoords(_yCoords), ChordLength( _ChordLength)
{

    //Transform(Scale) the to the full coordinates.
    Eigen::Transform<double,2,Eigen::Affine> t;
    t= Eigen::Scaling(ChordLength);

    //Transform (Translation between the full coordinates and the actual Node Location.
	QbladeNode2BladeDefTrans = Eigen::Translation2d(-LocalXOffset,-LocalYOffset);



    for(int i = 0; i < (int)_xCoords.size();i++)
    {
        Eigen::Vector2d TempCoords,NewCoords;
        TempCoords(0)=_xCoords.at(i);
        TempCoords(1)=_yCoords.at(i);
        NewCoords = QbladeNode2BladeDefTrans*t*TempCoords;
        xCoords.at(i)=NewCoords(0);
        yCoords.at(i)=NewCoords(1);
        //std::cout<<xCoords.at(i)<<", "<<yCoords.at(i)<<std::endl;
    }

    Outer.xCoords = xCoords;
    Outer.yCoords = yCoords;
    Outer.E = ShellE;
    Outer.rho = ShellDensity;

	//Outer.PrintCoords();
	//Outer.PrintCoords();

}

StructElem::StructElem()
{

}


void StructElem::CalcInnerRef()
{
	Inner.CalculateRef();
}

void StructElem::CalcOuterRef()
{
	Outer.CalculateRef();
}

void StructElem::CalcSparRef()
{
	Spar.CalculateRef();
}

// Takes the outer profile coordinates and Offsets by a thickness
//Note that large offsets will result in geometry deteriation
void StructElem::CreateInner(double Offset)
{
    Offset = Offset*ChordLength;
    Polygons OuterProfiles(1);
    Polygons InnerProfiles(1);

    // assemble the polgon ie Inner Profile
    int scale = 10e6;

    for(int i = 0; i < (int)xCoords.size();i++)
    {
         //cout<<(int) xCoords.at(0);
         double scaledX = xCoords.at(i)*scale;
         double scaledY = yCoords.at(i)*scale;
         //http://www.angusj.com/
         //Create outer "Profiles" - Its actually just one
         OuterProfiles[0].push_back(IntPoint((int) scaledX,(int) scaledY));
         // Run canned offset algor from //http://www.angusj.com/
         OffsetPolygons(OuterProfiles,InnerProfiles,-Offset*scale,jtRound,0.1,true);
    }

    if (InnerProfiles[0].size()<5)
    {
        Hollow = false;
        return;
    }

    //cout<<endl<<"Inner Profile Size "<<InnerProfiles[0].size()<<endl;
    //Built this way for debugging purposes. Can be optimized by just transferring the
    //whole vector.
    for(int i = 0; i < (int)InnerProfiles[0].size();i++)
    {
        vector<IntPoint> Current;
        Current = InnerProfiles[0];
        IntPoint Point = Current[i];
        double x = Point.X;
        double y = Point.Y;
        // Reload into the Inner Foil Object.
        Inner.xCoords.push_back((double)x/scale);
        Inner.yCoords.push_back((double)y/scale);
        //Inner.PrintCoords();
        //Outer.PrintCoords();
    }
    double MaxInner=0;
    double MaxOuter=0;
    double MinInner = 1e9;
    double MinOuter =1e9;
    for(int i = 0; i < (int)Inner.xCoords.size();i++)
    {
        if(Inner.xCoords.at(i)>MaxInner)
        {
            MaxInner =Inner.xCoords.at(i);
        }
        if(Inner.xCoords.at(i)<MinInner)
        {
            MinInner =Inner.xCoords.at(i);
        }
    }

    for(int i = 0; i < (int)Outer.xCoords.size();i++)
    {
        if(Outer.xCoords.at(i)>MaxOuter)
        {
            MaxOuter =Outer.xCoords.at(i);
        }
        if(Outer.xCoords.at(i)<MinOuter)
        {
            MinOuter =Outer.xCoords.at(i);
        }
    }
    //Tests that the Outer is actually bigger than the inner. Otherwise errors will come up
    try{
            if ((MaxOuter-MinOuter)<(MaxInner-MinInner))//Test Rubbish Geometry
            {
                throw "Geometry Error";
            }

    }
    catch (char const* msg)
    {
        std::cerr<<"Inner is larger than the Outer, Geometry input is incorrect."
                   <<"This can be fixed with a negative offset or redefining the "<<
                     "points definition to be the opposite direction "<<
                     "ie. change from clockwise to counter or vice versa"<<std::endl;
    }

    Inner.E=ShellE;
    Inner.rho = Outer.rho;
    Hollow = true;
}

void StructElem::CreateSpar(double Position,double Thickness,
                            double Angle,double _SparMaterialE
                            ,double SparDensity)//Positive angle gives clockwise rotation
{

    assert(Hollow);//Foil must be hollow to contain a spar.
    vector<double> SparXCoords,SparYCoords;
    //Adjust Values for chordlength.
	Position = Position*ChordLength -LocalXOffset;
    Thickness  =Thickness*ChordLength;

    //Create the spar coords declaring from the bottom left corner then
    //moving anti-clockwise. The spar at this stage is unclipped by the
    // foil and set about 0, 0
    //X Corners
    SparXCoords.push_back(-Thickness/2.0);
    SparXCoords.push_back(Thickness/2.0);
    SparXCoords.push_back(Thickness/2.0);
    SparXCoords.push_back(-Thickness/2.0);

    //Y Corners
    //Defined based on the assumption that the x Length will be the
    //Longest, as such the spar length is set that large so that
    //if 90 Degree angle is entered the spar is long enough.
    double xMax = *std::max_element(xCoords.begin(),xCoords.end());
    //double xMin = *std::min_element(xCoords.begin(),xCoords.end());
	double xMin = *std::min_element(xCoords.begin(),xCoords.end());
	xMax = xMax-xMin;

	SparYCoords.push_back(-xMax*1.1);
    SparYCoords.push_back(-xMax*1.1);
    SparYCoords.push_back(xMax*1.1);
    SparYCoords.push_back(xMax*1.1);
    //-------------------------------------------------------
    //Affine Rotation about the coord position and the x axis
    Eigen::Rotation2D<double> Rotation((Angle*2*PI)/360.0);
    Vector2d Point,Rotated;
    //Reload rotated coordinates.
    for(int i = 0; i < (int)SparXCoords.size();i++)
    {
        Point(0)=SparXCoords.at(i);
        Point(1)=SparYCoords.at(i);
        Rotated = Rotation*Point;
        SparXCoords.at(i)=Rotated(0)+Position;//Centre about the chordwise position.
		SparYCoords.at(i)=Rotated(1)-LocalYOffset;



    }
    //-------------------------------------------------------
    Polygons InnerFoilPoly(1),OutFoilPoly(1),SparPoly(1),solution;
    //Load up the polygons with the coords
    //Scaling factor used to recast doubles to ints.
    //This is nessesary for the clipper library.
    double Scale = 10.0e6;
    //Create polygon of the Inner Foil
    for(int i=Inner.xCoords.size()-1;i>=0;i--)
    {
        IntPoint NewPoint;
        //Doing the stupid rescaling thing because clipper
        //only handles ints
        double tempX = Scale*Inner.xCoords.at(i);
        double tempY =  Scale*Inner.yCoords.at(i);
        NewPoint.X =(int)tempX;
        NewPoint.Y =(int)tempY;
        InnerFoilPoly.at(0).push_back(NewPoint);
    }
    //Create Polygon of the Spar
    for(int i=SparXCoords.size()-1; i>=0;i--)
    {
        IntPoint NewPoint;
        double tempX = SparXCoords.at(i)*Scale;
        double tempY = SparYCoords.at(i)*Scale;
        NewPoint.X =(int) tempX;
        NewPoint.Y =(int) tempY;
        SparPoly.at(0).push_back(NewPoint);
    }
    //------------------------------------------------------
    //create a intersection binary operation meaning take the
    //area that is common to both polygons.
    Clipper c;
    c.AddPolygons(SparPoly,ptSubject);
    c.AddPolygons(InnerFoilPoly,ptClip);
    c.Execute(ctIntersection,solution,pftNonZero,pftNonZero);
    assert(solution.size()==1);// If not true then the solution has failed
    //Push solution back into the Spar Object.
    for (int i = 0; i < (int)solution.at(0).size();i++)
    {
        double tempX = (int) solution.at(0).at(i).X;
        double tempY = (int) solution.at(0).at(i).Y;
        Spar.xCoords.push_back(tempX/Scale);
        Spar.yCoords.push_back(tempY/Scale);
    }
    Spar.E= _SparMaterialE;
    Spar.rho = SparDensity;
    WithSpar = true;
//    Inner.PrintCoords();
//    Spar.PrintCoords();
}

void StructElem::TotalStruct()
{
    //Creating the summation of all of the integrals listed in Hansen Page 100.
    // The Inner is subtracted as it is a hole.
    Total.EA = Outer.EA-Inner.EA+Spar.EA;
    Total.EDXYR=Outer.EDXYR-Inner.EDXYR+Spar.EDXYR;
    Total.EIXR=Outer.EIXR-Inner.EIXR+Spar.EIXR;
    Total.EIYR=Outer.EIYR-Inner.EIYR+Spar.EIYR;
    Total.ESXR=Outer.ESXR-Inner.ESXR+Spar.ESXR;
    Total.ESYR=Outer.ESYR-Inner.ESYR+Spar.ESYR;
    Total.rhoA = Outer.rhoA-Inner.rhoA+Spar.rhoA;
    Total.CenY = (Outer.CenY-Inner.CenY+Spar.CenY);
    Total.CenX = (Outer.CenX-Inner.CenX+Spar.CenX);
    Total.Area = (Outer.Area+ Inner.Area)/2;//+ Spar.Area);
}

void StructElem::LocaliseAboutElasticAxis()
{
    //Before this function is called the values
    //are set around reference coordinates.
    //These functions readjust the values to the
    //elastic axis and calculate its location
    TotalStruct();
    Xe = Total.ESYR/Total.EA;
    Ye = Total.ESXR/Total.EA;
    ElasticAxisNode =  NodeID;
    Area = Total.Area;


    //Transform (Translation between the full coordinates and the actual Node Location.
    QbladeNode2ElasticTrans = Eigen::Translation3d(Xe,Ye,0);
    ElasticAxisNode.Coords = QbladeNode2ElasticTrans*NodeID.Coords;
//    std::cout<<"Elastic Coords"<<std::endl;
//    ElasticAxisNode.PrintCoords();

    EIx = Total.EIXR-pow(Ye,2)*Total.EA;
    EIy = Total.EIYR-pow(Xe,2)*Total.EA;

    EDXY = Total.EDXYR-Xe*Ye*Total.EA;
    //Test for boundary case where EIx == EIy and sets alpha=0
    if (fabs((EIx-EIy)/EIx)>0.03){
    alpha = 0.5*atan((2*EDXY)/(EIy-EIx));
    EI1 = EIx-EDXY*tan(alpha);
    EI2 = EIy+EDXY*tan(alpha);
    }
    else
    {
//        std::cout<<"circle"<<std::endl;
        EI1 = EIx;
        EI2 = EIy;
        alpha = 0;
    }
	StructuralTwist = alpha - ChordAngle;

    StructuralTwistMatrix = Eigen::Rotation2Dd(-StructuralTwist);
    Vector3d Temp, Rotated;
    Temp.setZero();
    Temp(0)=1;

    Rotated= StructuralTwistMatrix*Temp;
    PrincipleAxis1.Coords(0)= Rotated(0);
    PrincipleAxis1.Coords(1)= Rotated(1);
    Temp.setZero();
    Temp(1)=1;
    Rotated= StructuralTwistMatrix*Temp;
    PrincipleAxis2.Coords(0)= Rotated(0);
    PrincipleAxis2.Coords(1)= Rotated(1);
    AxialAxis.Coords(2)=1;
//    std::cout<<"Unit Vectors"<<std::endl;
//    PrincipleAxis1.PrintCoords();
//    PrincipleAxis2.PrintCoords();

//    std::cout<<EDXY<<std::endl;
//    std::cout<<"EIx = "<<EIx<<" EIy = "<<EIy<<std::endl;
    try{
            if (EI1<0||EI2<0)//Test Rubbish Geometry
            {
                throw "Geometry Error";
            }

    }
    catch (char const* msg)
    {
        std::cerr<<"Negative Stiffnesses, probably caused by bad geometry input"<<std::endl;
    }
//    std::cout<<"Alpha(deg)="<<alpha*180.0/PI<<std::endl;
//    std::cout<<"Xe = "<< Xe<<std::endl<<"Ye = "<<Ye<<std::endl;
    GJ = (EIx+EIy)/(2.0*(1.0+0.33)); //Assuming poissons ratio to be 0.3

    RhoA = Total.rhoA;
    EA = Total.EA;
    CenY = Total.CenY/RhoA;
    CenX = Total.CenX/RhoA;
    CalcJPrime();

}


void StructElem::CalcJPrime()// Calculate the J Prime values referencing the newly found centroid.
{
    Outer.CalculateJRef(CenX,CenY);
    if(Hollow)Inner.CalculateJRef(CenX,CenY);
    if(WithSpar)Spar.CalculateJRef(CenX,CenY);
    Total.JPrime = Outer.JPrime-Inner.JPrime+Spar.JPrime;
    JPrime = Total.JPrime;
}
void StructElem::PrintProperties()
{
    std::cout<<"EA ="<<EA<<std::endl;
    std::cout<<"EI1 ="<<EI1<<std::endl;
    std::cout<<"EI2 ="<<EI2<<std::endl;
    std::cout<<"EIx ="<<EIx<<std::endl;
    std::cout<<"EIy ="<<EIy<<std::endl;
    std::cout<<"GJ ="<<GJ<<std::endl;
    std::cout<<"RhoA ="<<RhoA<<std::endl;
    std::cout<<"JPrime ="<<JPrime<<std::endl;
    std::cout<<"Area ="<<Area<<std::endl;
	std::cout<<"Structural Twist ="<<StructuralTwist*180.0/PI<<std::endl;
    std::cout<<"Point of Elasticity(compared to Reference)"<< Xe<<","<<Ye<<std::endl;
}

void StructElem::PrintGlobalCoords()
{
//    Eigen::Transform<double,2,Eigen::Affine> RotateMat;
//    RotateMat = Eigen::Rotation2Dd(ChordAngle);
//    Eigen::Transform<double,3,Eigen::Affine> TranslateMat;
//    TranslateMat = Eigen::Translation3d(NodeID.Coords(0),NodeID.Coords(1),NodeID.Coords(2));
//    Vector3d Temp, Rotated;
//    Vector4d Rotated3DMat, TranslatedPoint;
//    Rotated3DMat.setZero();
//    Rotated3DMat(3)=1;
//    Temp.setZero();
//    Temp(2)=1;
//    //Rotated the 2D coordinates
//    for (int i = 0;i<xCoords.size();i++)
//    {
//        Temp(0) = xCoords.at(i);
//        Temp(1) = yCoords.at(i);
//        Rotated = RotateMat*Temp;
//        Rotated3DMat(0) = Rotated(0);
//        Rotated3DMat(1) = Rotated(1);
//        TranslatedPoint = TranslateMat*Rotated3DMat;
//        GlobalCoords.push_back(TranslatedPoint);
//        std::cout<<TranslatedPoint(0)<<','<<TranslatedPoint(1)<<','<<TranslatedPoint(2)<<std::endl;
//    }
    Outer.PrintGlobalCoords(NodeID,ChordAngle);
    std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[0 0 1]); axis equal; hold on;"<<std::endl;
    if(Hollow)
    {
        Inner.PrintGlobalCoords(NodeID,ChordAngle);
        std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[1 1 1]); axis equal; hold on;"<<std::endl;
    }
    if(WithSpar){
        Spar.PrintGlobalCoords(NodeID,ChordAngle);
        std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[0 0 0]); axis equal; hold on;"<<std::endl;
    }
}


void StructElem::PrintGlobalCoords(DeformationVector DeformedNode, double Amplification)
{
//    Eigen::Transform<double,2,Eigen::Affine> RotateMat;
//    RotateMat = Eigen::Rotation2Dd(ChordAngle);
//    Eigen::Transform<double,3,Eigen::Affine> TranslateMat;
//    TranslateMat = Eigen::Translation3d(NodeID.Coords(0),NodeID.Coords(1),NodeID.Coords(2));
//    Vector3d Temp, Rotated;
//    Vector4d Rotated3DMat, TranslatedPoint;
//    Rotated3DMat.setZero();
//    Rotated3DMat(3)=1;
//    Temp.setZero();
//    Temp(2)=1;
//    //Rotated the 2D coordinates
//    for (int i = 0;i<xCoords.size();i++)
//    {
//        Temp(0) = xCoords.at(i);
//        Temp(1) = yCoords.at(i);
//        Rotated = RotateMat*Temp;
//        Rotated3DMat(0) = Rotated(0);
//        Rotated3DMat(1) = Rotated(1);
//        TranslatedPoint = TranslateMat*Rotated3DMat;
//        GlobalCoords.push_back(TranslatedPoint);
//        std::cout<<TranslatedPoint(0)<<','<<TranslatedPoint(1)<<','<<TranslatedPoint(2)<<std::endl;
//    }
	DeformedNode.Coords = Amplification*DeformedNode.Coords;
	DeformedNode.zAxisAngle = Amplification*DeformedNode.zAxisAngle;
    DeformedNode.Coords = DeformedNode.Coords + NodeID.Coords;
    double totalAngle = DeformedNode.zAxisAngle+ChordAngle;
    Outer.PrintGlobalCoords(DeformedNode,totalAngle);
    std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[0 0 1]); axis equal; hold on;"<<std::endl;
    if(Hollow){
        Inner.PrintGlobalCoords(DeformedNode,totalAngle);
        std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[1 1 1]); axis equal; hold on;"<<std::endl;
    }
    if(WithSpar)
    {
        Spar.PrintGlobalCoords(DeformedNode,totalAngle);
        std::cout<<"fill3(A(:,1),A(:,2),A(:,3),[0 0 0]); axis equal; hold on;"<<std::endl;
    }
}
