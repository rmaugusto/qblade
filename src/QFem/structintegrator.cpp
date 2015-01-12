#include "structintegrator.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <typeinfo>
#include "node.h"
using Eigen::Matrix2d;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector4d;


StructIntegrator::StructIntegrator(std::vector<double> _xCoords,
                                   std::vector<double> _yCoords,
                                   double _E, double _rho):
 ESXR(0.0),
 ESYR(0.0),
 EA(0.0),
 EIXR(0.0),
 EIYR(0.0),
 EDXYR(0.0),
 E(_E),
 rhoA(0.0),
 rho(_rho),
 JPrime(0.0),
 xCoords(_xCoords),
 yCoords(_yCoords)
{
    resolution = 300;
}

StructIntegrator::StructIntegrator():
 ESXR(0.0), ESYR(0.0),
 EA(0.0),
 EIXR(0.0),
 EIYR(0.0),
 EDXYR(0.0),
 E(0.0),
 CenY(0.0),CenX(0.0),
 rhoA(0.0),
 rho(0.0),
 JPrime(0.0)
 {
    resolution = 300;
 }

void StructIntegrator::CalculateRef()
{

    assert(E!=0);//Makes sure they are both set

    //Integration Grid Resolution
    int Res = resolution;
    double yMin = *std::min_element(yCoords.begin(),yCoords.end());
    double yMax = *std::max_element(yCoords.begin(),yCoords.end());
    double xMin = *std::min_element(xCoords.begin(),xCoords.end());
    double xMax = *std::max_element(xCoords.begin(),xCoords.end());
    double yRes = std::fabs(yMax-yMin)/Res;
    double xRes = std::fabs(xMax-xMin)/Res;

    for(double i = xMin;i<xMax;i=i+xRes)
    {
        for(double j = yMin;j<yMax;j=j+yRes)
        {
           if(pointInPolygon(xCoords,yCoords,i,j))
           {
               //std::cout<<i<<", "<<j<<std::endl;
               //Formulas derived from Hansen in Aerodynamics of wind turbines
               //Pages 110.
               //add values for the first integration
               //Three integration Terms required to extract the section properties.
               //    • Moment of stiffness inertia about the axis XR: [EIXR] = ∫A EYR2d A.
               //    • Moment of stiffness inertia about the axis YR: [EIYR] = ∫A EXR2d A.
               //    • Moment of centrifugal stiffness: [EDXYR] = ∫A EXRYRd A
               ESXR+=xRes*yRes*j; //EQN 1 in notes
               ESYR+=xRes*yRes*i; // EQN 2 in notes
               EA+= yRes*xRes;//EQN 4
               EIXR += ((xRes)*(pow((j+yRes/2),3)-pow((j-yRes/2),3)))/3;//eqn 6
               EIYR += ((yRes)*(pow((i+xRes/2),3)-pow((i-xRes/2),3)))/3;
               EDXYR += (pow((i+xRes/2),2)-pow((i-xRes/2),2))*
                       (pow((j+yRes/2),2)-pow((j-yRes/2),2))/4;
               CenY +=yRes*xRes*j;
               CenX +=yRes*xRes*i;
           }
        }
    }
    CenY*=rho;
    CenX*=rho;
    ESXR *=E;
    ESYR *=E;
    rhoA = EA*rho;
    Area = EA;
    EA *=E;
    EIXR *=E;
    EIYR *=E;
    EDXYR *=E;
    //std::cout<<"EA"<<EA;

}

void StructIntegrator::CalculateJRef(double RefX, double RefY)
{

    //Calculates the values required to get the structural properties of the foil
    // Works by iterating over the a rectangle over the max size of the foil
    //calculates only for points inside the foil/or arbitrary structural shape.
    assert(E!=0);//Makes sure they are both set

    //Integration Grid Resolution
    int Res = resolution;
    double yMin = *std::min_element(yCoords.begin(),yCoords.end());
    double yMax = *std::max_element(yCoords.begin(),yCoords.end());
    double xMin = *std::min_element(xCoords.begin(),xCoords.end());
    double xMax = *std::max_element(xCoords.begin(),xCoords.end());
    double yRes = std::fabs(yMax-yMin)/Res;
    double xRes = std::fabs(xMax-xMin)/Res;

    for(double i = xMin;i<xMax;i=i+xRes)
    {
        for(double j = yMin;j<yMax;j=j+yRes)
        {
           if(pointInPolygon(xCoords,yCoords,i,j))
           {
               JPrime+=(xRes*yRes)*(pow((i-RefX),2)+pow((j-RefY),2));
           }
        }
    }
    JPrime*=rho;
}

bool StructIntegrator::pointInPolygon(std::vector<double> polyX,std::vector<double> polyY, double x,double y) {


  int polySides = polyY.size();
  int   i, j=polySides-1 ;
  bool  oddNodes=false      ;

  for (i=0; i<polySides; i++) {
    if (((polyY.at(i)< y && polyY.at(j)>=y)
    ||   (polyY.at(j)< y && polyY.at(i)>=y))
    &&  (polyX.at(i)<=x || polyX.at(j)<=x)) {
      oddNodes^=(polyX.at(i)+(y-polyY.at(i))/(polyY.at(j)-polyY.at(i))*(polyX.at(j)-polyX.at(i))<x); }
    j=i; }

  return oddNodes;

}

void StructIntegrator::PrintCoords()
{
    std::cout<<std::endl;
    for(int i = 0; i < (int)xCoords.size();i++)
    {
        std::cout<<xCoords.at(i)<<", "<<yCoords.at(i)<<std::endl;
    }
}

void StructIntegrator::PrintValues()
{
    std::cout<<"ESXR = "<<ESXR<<std::endl;
    std::cout<< "ESYR = "<<ESYR<<std::endl;
    std::cout<< "EA = "<<EA<<std::endl;
    std::cout<< "EIXR = "<<EIXR<<std::endl;
    std::cout<< "EIYR = "<<EIYR<<std::endl;
    std::cout<< "EDXYR = "<<EDXYR<<std::endl;
    std::cout<< "E = "<<E<<std::endl;
    std::cout<< "rhoA = "<<rhoA<<std::endl;
}


void StructIntegrator::PrintGlobalCoords(Node NodeID,double ChordAngle)
{
    Eigen::Transform<double,2,Eigen::Affine> RotateMat;
    RotateMat = Eigen::Rotation2Dd(ChordAngle);
    Eigen::Transform<double,3,Eigen::Affine> TranslateMat;
    TranslateMat = Eigen::Translation3d(NodeID.Coords(0),NodeID.Coords(1),NodeID.Coords(2));
    Vector3d Temp, Rotated;
    Vector4d Rotated3DMat, TranslatedPoint;
    Rotated3DMat.setZero();
    Rotated3DMat(3)=1;
    Temp.setZero();
    Temp(2)=1;
    //Rotated the 2D coordinates
    std::cout<<"A=["<<std::endl;
    for (int i = 0; i < (int)xCoords.size();i++)
    {
        Temp(0) = xCoords.at(i);
        Temp(1) = yCoords.at(i);
        Rotated = RotateMat*Temp;
        Rotated3DMat(0) = Rotated(0);
        Rotated3DMat(1) = Rotated(1);
        TranslatedPoint = TranslateMat*Rotated3DMat;
        GlobalCoords.push_back(TranslatedPoint);
        std::cout<<TranslatedPoint(0)<<','<<TranslatedPoint(1)<<','<<TranslatedPoint(2)<<std::endl;
    }
    std::cout<<"]"<<std::endl;

}

