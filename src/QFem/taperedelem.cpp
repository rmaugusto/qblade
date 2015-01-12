#include "taperedelem.h"
#include <iostream>
#include <Eigen/Sparse>
#include "structelem.h"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <math.h>
using Eigen::Vector4d;
using Eigen::Vector3d;
using Eigen::Matrix3d;


TaperedElem::TaperedElem(StructElem _Properties0,StructElem _Properties1, double _omega):
    omega(_omega), Properties0(_Properties0), Properties1(_Properties1)
{

    Connectors=Properties0.NodeID.DOFNums;

    Connectors.insert(Connectors.end(),
                      Properties1.NodeID.DOFNums.begin(),Properties1.NodeID.DOFNums.end());
    //Dummy Variables
    //Density
    double rho2 = Properties1.RhoA;
    double rho1= Properties0.RhoA;
    double alpha = (Properties0.StructuralTwist+Properties1.StructuralTwist)/2; //Structural Twist in Radians

    //
    double EIy0 = Properties0.EI2;
    double EIyl = Properties1.EI2;
    double EIx0= Properties0.EI1;
    double EIxl= Properties1.EI1;
    double EA0 = Properties0.EA;
    double EAl= Properties1.EA;
    double GJ0=fabs(Properties0.GJ);
    double GJl=fabs(Properties1.GJ);


    Vector4d Difference;
    //---------------------From Fortran Code
    Difference.setZero();
    Difference = Properties1.ElasticAxisNode.Coords-Properties0.ElasticAxisNode.Coords;
    //Length between codes
    l = sqrt(pow(Difference(0),2)+pow(Difference(1),2)+pow(Difference(2),2));

    //Compute Slopes
    double dxdl = Difference(0)/l;//(cx(je) - cx(ie))/al;
    double dydl = Difference(1)/l;//(cy(je) - cy(ie))/al;
    double dzdl = Difference(2)/l;//(cz(je) - cz(ie))/al;
//    std::cout<<"dxdl "<<dxdl<<std::endl;
//    std::cout<<"dydl "<<dydl<<std::endl;
//    std::cout<<"dzdl "<<dzdl<<std::endl;
    //Compute elements of (3x3)transformation matrix
    //If element is vertical there is special handling
    //B is set using the matrix tool box;
    b.setZero();
    bool ivert = false;
    if (ivert == 1) {
    b(0,0) = 0.0;
    b(0,1) = -sin(alpha);
    b(0,2) = -cos(alpha);
    b(1,0) = 0.0;
    b(1,1) = cos(alpha);
    b(1,2) = -sin(alpha);
    b(2,0) = 1.0;
    b(2,1) = 0.0;
    b(2,2) = 0.0;
    }
    else
    {
    double d = sqrt(pow(dydl,2) + pow(dzdl,2));
    double a11 = (pow(dydl,2) + pow(dzdl,2))/d;
    double a12 = -(dxdl * dydl) /d;
    double a13 = -(dxdl * dzdl) /d;
    double a21 = 0.0;
    double a22 = dzdl/d;
    double a23 = -dydl/d;
    //cos(alpha) = cos(alpha);
    //sin(alpha) = sin(alpha);
    b(0,0) = a11 * cos(alpha) - a21 * sin(alpha);
    b(0,1) = a12 * cos(alpha) - a22 * sin(alpha);
    b(0,2) = a13 * cos(alpha) - a23 * sin(alpha);
    b(1,0) = a11 * sin(alpha) + a21 * cos(alpha);
    b(1,1) = a12 * sin(alpha) + a22 * cos(alpha);
    b(1,2) = a13 * sin(alpha) + a23 * cos(alpha);
    b(2,0) = dxdl;
    b(2,1) = dydl;
    b(2,2) = dzdl;
    }
    // Trash Filter
//    std::cout<<b<<std::endl;
    //Construct Giant B for 12x12 Transforms
    GiantB.resize(Row,Row);
    GiantB.setZero();
    for(int i=0;i<Row;i=i+3){
        GiantB.block(i,i,3,3) = b;
    }
    GiantBtr= GiantB.transpose();

    //Computer Parameters
    cy = Properties0.NodeID.Coords(1); // {Check these two lines
    cz = Properties0.NodeID.Coords(2); //      }
    double a0 = pow(omega,2)*(b(2,1) * cy + b(2,2) * cz);
    double b0 = pow(omega,2)*(pow(b(2,1),2) + pow(b(2,2),2));
    c0 = rho1 * a0;
    d0 = ((rho2 - rho1) * a0 + rho1 * b0 * l )/ l;
    e0 = (rho2 - rho1) * b0 / l;
    double a11 = pow(b(0,1),2) + pow(b(0,2),2);
    double a12 = b(0,1)*b(1,1) + b(0,2)*b(1,2);
    double a13 = b(0,1)*b(2,1) + b(0,2)*b(2,2);
    double a22 = pow(b(1,1),2) + pow(b(1,2),2);
    double a23 = b(1,1)*b(2,1) + b(1,2)*b(2,2);
    double a33 = pow(b(2,1),2) + pow(b(2,2),2);
//    double b1 = b(0,2)*b(1,1) - b(0,1)*b(1,2);
//    double b2 = b(0,2)*b(2,1) - b(0,1)*b(2,2);
//    double b3 = b(1,2)*b(2,1) - b(1,1)*b(2,2);



    ElasticStiff.resize(Row,Row);
    ElasticStiff.setZero();
    ElasticStiff(0,0)= 6.0/pow(l,3) * (EIy0 + EIyl);
    ElasticStiff(0,4)= 2.0/pow(l,2) * (2.0 * EIy0 + EIyl);
    ElasticStiff(0,6)= -6.0/pow(l,3) * (EIy0 + EIyl);
    ElasticStiff(0,10) = 2.0/pow(l,2) * (EIy0 + 2.0 * EIyl);
    ElasticStiff(1,1)= 6.0/pow(l,3) * (EIx0 + EIxl);
    ElasticStiff(1,3)= -2.0/pow(l,2) * (2.0* EIx0 + EIxl);
    ElasticStiff(1,7)= -6.0/pow(l,3) * (EIx0 + EIxl);
    ElasticStiff(1,9) = -2.0/pow(l,2) * (EIx0 + 2.0 * EIxl);
    ElasticStiff(2,2)= (EA0 + EAl)/(2.0 * l);
    ElasticStiff(2,8)= -(EA0 + EAl)/(2.0 * l);
    ElasticStiff(3,3)= 1.0/l * (3.0 * EIx0 + EIxl);
    ElasticStiff(3,7)= 2.0/pow(l,2) * (2.0 * EIx0 + EIxl);
    ElasticStiff(3,9) = 1.0/l * (EIx0 + EIxl);
    ElasticStiff(4,4)= 1.0/l * (3.0 * EIy0 + EIyl);
    ElasticStiff(4,6)= -2.0/pow(l,2) * (2.0 * EIy0 + EIyl);
    ElasticStiff(4,10) = 1.0/l * (EIy0 + EIyl);
    ElasticStiff(5,5)= (GJ0 + GJl)/(2.0 * l);
    ElasticStiff(5,11) = -(GJ0 + GJl)/(2.0 * l);
    ElasticStiff(6,6)= 6.0/pow(l,3) * (EIy0 + EIyl);
    ElasticStiff(6,10) = -2.0/pow(l,2) * (EIy0 + 2.0 * EIyl);
    ElasticStiff(7,7)= 6.0/pow(l,3) * (EIx0 + EIxl);
    ElasticStiff(7,9) = 2.0/pow(l,2) * (EIx0 + 2.0 * EIxl);
    ElasticStiff(8,8)= (EA0 + EAl)/(2.0 * l);
    ElasticStiff(9,9) = 1.0/l * (EIx0 + 3.0 * EIxl);
    ElasticStiff(10,10) = 1.0/l * (EIy0 + 3.0 * EIyl);
    ElasticStiff(11,11) = (GJ0 + GJl)/(2.0 * l);

    // Set symmetric condition
    ElasticStiff = ElasticStiff.selfadjointView<Eigen::Upper>();
    //std::cout<<"Elastic Stiff"<<std::endl<<ElasticStiff<<std::endl;

    //Spin Stiff - Taken and reformatted from CurveFEM(fortran-> c++)
    SpinStiff.resize(Row,Row);
    SpinStiff.setZero();
    SpinStiff(0,0)=rho1/420.0* 156.0*a11+(rho2-rho1)/840.0* 72.0*a11;
    SpinStiff(0,1)=rho1/420.0* 156.0*a12+(rho2-rho1)/840.0* 72.0*a12;
    SpinStiff(0,2)=rho1/420.0* 147.0*a13+(rho2-rho1)/840.0* 70.0*a13;
    SpinStiff(0,3)=rho1/420.0* -22.0*l*a12+(rho2-rho1)/840.0* -14.0*l*a12;
    SpinStiff(0,4)=rho1/420.0* 22.0*l*a11+(rho2-rho1)/840.0* 14.0*l*a11;
    SpinStiff(0,6)=rho1/420.0* 54.0*a11+(rho2-rho1)/840.0* 54.0*a11;
    SpinStiff(0,7)=rho1/420.0* 54.0*a12+(rho2-rho1)/840.0* 54.0*a12;
    SpinStiff(0,8)=rho1/420.0* 63.0*a13+(rho2-rho1)/840.0* 56.0*a13;
    SpinStiff(0,9)=rho1/420.0* 13.0*l*a12+(rho2-rho1)/840.0* 12.0*l*a12;
    SpinStiff(0,10)=rho1/420.0* -13.0*l*a11+(rho2-rho1)/840.0* -12.0*l*a11;
    SpinStiff(1,1)=rho1/420.0* 156.0*a22+(rho2-rho1)/840.0* 72.0*a22;
    SpinStiff(1,2)=rho1/420.0* 147.0*a23+(rho2-rho1)/840.0* 70.0*a23;
    SpinStiff(1,3)=rho1/420.0* -22.0*l*a22+(rho2-rho1)/840.0* -14.0*l*a22;
    SpinStiff(1,4)=rho1/420.0* 22.0*l*a12+(rho2-rho1)/840.0* 14.0*l*a12;
    SpinStiff(1,6)=rho1/420.0* 54.0*a12+(rho2-rho1)/840.0* 54.0*a12;
    SpinStiff(1,7)=rho1/420.0* 54.0*a22+(rho2-rho1)/840.0* 54.0*a22;
    SpinStiff(1,8)=rho1/420.0* 63.0*a23+(rho2-rho1)/840.0* 56.0*a23;
    SpinStiff(1,9)=rho1/420.0* 13.0*l*a22+(rho2-rho1)/840.0* 12.0*l*a22;
    SpinStiff(1,10)=rho1/420.0* -13.0*l*a12+(rho2-rho1)/840.0* -12.0*l*a12;
    SpinStiff(2,2)=rho1/420.0* 140.0*a33+(rho2-rho1)/840.0* 70.0*a33;
    SpinStiff(2,3)=rho1/420.0* -21.0*l*a23+(rho2-rho1)/840.0* -14.0*l*a23;
    SpinStiff(2,4)=rho1/420.0* 21.0*l*a13+(rho2-rho1)/840.0* 14.0*l*a13;
    SpinStiff(2,6)=rho1/420.0* 63.0*a13+(rho2-rho1)/840.0* 70.0*a13;
    SpinStiff(2,7)=rho1/420.0* 63.0*a23+(rho2-rho1)/840.0* 70.0*a23;
    SpinStiff(2,8)=rho1/420.0* 70.0*a33+(rho2-rho1)/840.0* 70.0*a33;
    SpinStiff(2,9)=rho1/420.0* 14.0*l*a23+(rho2-rho1)/840.0* 14.0*l*a23;
    SpinStiff(2,10)=rho1/420.0* -14.0*l*a13+(rho2-rho1)/840.0* -14.0*l*a13;
    SpinStiff(3,3)=rho1/420.0* 4.0*pow(l,2)*a22+(rho2-rho1)/840.0* 3.0*pow(l,2)*a22;
    SpinStiff(3,4)=rho1/420.0* -4.0*pow(l,2)*a12+(rho2-rho1)/840.0* -3.0*pow(l,2)*a12;
    SpinStiff(3,4)=rho1/420.0* -13.0*l*a12+(rho2-rho1)/840.0* -14.0*l*a12;
    SpinStiff(3,7)=rho1/420.0* -13.0*l*a22+(rho2-rho1)/840.0* -14.0*l*a22;
    SpinStiff(3,8)=rho1/420.0* -14.0*l*a23+(rho2-rho1)/840.0* -14.0*l*a23;
    SpinStiff(3,9)=rho1/420.0* -3.0*pow(l,2)*a22+(rho2-rho1)/840.0* -3.0*pow(l,2)*a22;
    SpinStiff(3,10)=rho1/420.0* 3.0*pow(l,2)*a12+(rho2-rho1)/840.0* 3.0*pow(l,2)*a12;
    SpinStiff(4,4)=rho1/420.0* 4.0*pow(l,2)*a11+(rho2-rho1)/840.0* 3.0*pow(l,2)*a11;
    SpinStiff(4,6)=rho1/420.0* 13.0*l*a11+(rho2-rho1)/840.0* 14.0*l*a11;
    SpinStiff(4,7)=rho1/420.0* 13.0*l*a12+(rho2-rho1)/840.0* 14.0*l*a12;
    SpinStiff(4,8)=rho1/420.0* 14.0*l*a13+(rho2-rho1)/840.0* 14.0*l*a13;
    SpinStiff(4,9)=rho1/420.0* 3.0*pow(l,2)*a12+(rho2-rho1)/840.0* 3.0*pow(l,2)*a12;
    SpinStiff(4,10)=rho1/420.0* -3.0*pow(l,2)*a11+(rho2-rho1)/840.0* -3.0*pow(l,2)*a11;
    SpinStiff(6,6)=rho1/420.0* 156.0*a11+(rho2-rho1)/840.0* 240.0*a11;
    SpinStiff(6,7)=rho1/420.0* 156.0*a12+(rho2-rho1)/840.0* 240.0*a12;
    SpinStiff(6,8)=rho1/420.0* 147.0*a13+(rho2-rho1)/840.0* 224.0*a13;
    SpinStiff(6,9)=rho1/420.0* 22.0*l*a12+(rho2-rho1)/840.0* 30.0*l*a12;
    SpinStiff(6,10)=rho1/420.0* -22.0*l*a11+(rho2-rho1)/840.0* -30.0*l*a11;
    SpinStiff(7,7)=rho1/420.0* 156.0*a22+(rho2-rho1)/840.0* 240.0*a22;
    SpinStiff(7,8)=rho1/420.0* 147.0*a23+(rho2-rho1)/840.0* 224.0*a23;
    SpinStiff(7,9)=rho1/420.0* 22.0*l*a22+(rho2-rho1)/840.0* 30.0*l*a22;
    SpinStiff(7,10)=rho1/420.0* -22.0*l*a12+(rho2-rho1)/840.0* -30.0*l*a12;
    SpinStiff(8,8)=rho1/420.0* 140.0*a33+(rho2-rho1)/840.0* 210.0*a33;
    SpinStiff(8,9)=rho1/420.0* 21.0*l*a23+(rho2-rho1)/840.0* 22.0*l*a23;
    SpinStiff(8,10)=rho1/420.0* -21.0*l*a13+(rho2-rho1)/840.0* -22.0*l*a13;
    SpinStiff(9,9)=rho1/420.0* 4.0*pow(l,2)*a22+(rho2-rho1)/840.0* 5.0*pow(l,2)*a22;
    SpinStiff(9,10)=rho1/420.0* -4.0*pow(l,2)*a12+(rho2-rho1)/840.0* -5.0*pow(l,2)*a12;
    SpinStiff(10,10)=rho1/420.0* 4.0*pow(l,2)*a11+(rho2-rho1)/840.0* 5.0*pow(l,2)*a11;
    SpinStiff = SpinStiff.selfadjointView<Eigen::Upper>();
    SpinStiff = SpinStiff*l*pow(omega,2);
    //Axial Reduction Matrix - Again from CurveFEM
    AxialRed.resize(Row,Row);
    AxialRed.setZero();
    AxialRed(0,0) = 36/(30*l);
    AxialRed(0,4) = 3*l/(30*l);
    AxialRed(0,6) = -36/(30*l);
    AxialRed(0,10) = 3*l/(30*l);
    AxialRed(1,1) = 36/(30*l);
    AxialRed(1,3) = -3*l/(30*l);
    AxialRed(1,7) = -36/(30*l);
    AxialRed(1,9) = -3*l/(30*l);
    AxialRed(3,3) = 4*pow(l,2) /(30*l);
    AxialRed(3,7) = 3*l/(30*l);
    AxialRed(3,9) = -pow(l,2) /(30*l);
    AxialRed(4,4) = 4*pow(l,2) /(30*l);
    AxialRed(4,6) = -3*l/(30*l);
    AxialRed(4,10) = -pow(l,2) /(30*l);
    AxialRed(6,6) = 36/(30*l);
    AxialRed(6,10) = -3*l/(30*l);
    AxialRed(7,7) = 36/(30*l);
    AxialRed(7,9) = 3*l/(30*l);
    AxialRed(9,9) = 4*pow(l,2) /(30*l);
    AxialRed(10,10) = 4*pow(l,2) /(30*l);

    AxialRed = AxialRed.selfadjointView<Eigen::Upper>();
    //Mass Matrix
    MassMatrix.resize(Row,Row);
    MassMatrix.setZero();
    MassMatrix(0,0)= l/35.0 * (10.0 * rho1 + 3.0 * rho2 );
    MassMatrix(0,4)= pow(l,2)/420.0 * (17.0 * rho1 + 5.0 * rho2 );
    MassMatrix(0,6)= 9.0 * l/140.0 * (rho1 + rho2 );
    MassMatrix(0,10)= -pow(l,2)/420.0 * (7.0 * rho1 + 6.0 * rho2 );
    MassMatrix(1,1)= l/35.0 * (10.0 * rho1 + 3.0 * rho2 );
    MassMatrix(1,3)= -pow(l,2)/420.0 * (17.0 * rho1 + 5.0 * rho2 );
    MassMatrix(1,7)= 9.0 * l/140.0 * (rho1 + rho2 );
    MassMatrix(1,9)= pow(l,2)/420.0 * (7.0 * rho1 + 6.0 * rho2 );
    MassMatrix(2,2)= l/12.0 * (3.0 * rho1 + rho2 );
    MassMatrix(2,8)= l/12.0 * (rho1 + rho2 );
    MassMatrix(3,3)= pow(l,3)/840.0 * (5.0 * rho1 + 3.0 * rho2 );
    MassMatrix(3,7)= -pow(l,2)/420.0 * (6.0 * rho1 + 7.0 * rho2 );
    MassMatrix(3,9)= -pow(l,3)/280.0 * (rho1 + rho2 );
    MassMatrix(4,4)= pow(l,3)/840.0 * (5.0 * rho1 + 3.0 * rho2 );
    MassMatrix(4,6)= pow(l,2)/420.0 * (6.0 * rho1 + 7.0 * rho2 );
    MassMatrix(4,10)= -pow(l,3)/280.0 * (rho1 + rho2 );
    MassMatrix(5,5)= l/12.0 * (3.0 * Properties0.JPrime + Properties1.JPrime );
    MassMatrix(5,11)= l/12.0 * (Properties0.JPrime + Properties1.JPrime );
    MassMatrix(6,6)= l/35.0 * (3.0 * rho1 + 10.0 * rho2 );
    MassMatrix(6,10)= -pow(l,2)/420.0 * (7.0 * rho1 + 15.0 * rho2 );
    MassMatrix(7,7)= l/35.0 * (3.0 * rho1 + 10.0 * rho2 );
    MassMatrix(7,9)= pow(l,2)/420.0 * (7.0 * rho1 + 15.0 * rho2 );
    MassMatrix(8,8)= l/4.0 * (rho1/3.0 + rho2 );
    MassMatrix(9,9)= pow(l,3)/840.0 * (3.0 * rho1 + 5.0 * rho2 );
    MassMatrix(10,10)= pow(l,3)/840.0 * (3.0 * rho1 + 5.0 * rho2 );
    MassMatrix(11,11)= l/4.0 * (Properties0.JPrime/3.0 + Properties1.JPrime );
    //std::cout<<"Single Mass Pre"<<std::endl<<MassMatrix<<std::endl;

    MassMatrix = GiantBtr*MassMatrix.selfadjointView<Eigen::Upper>()*GiantB;
    //std::cout<<"Single Mass"<<MassMatrix<<std::endl;
    //std::cout<<GiantBtr<<std::endl;
    // Other Matricies set to fake values.
    //SpinStiff = ElasticStiff;

    K.resize(1,1);//Set to one so it will throw an error if you don't initialize the axial stiffness


    //




}

TaperedElem::TaperedElem(){

}


void TaperedElem::InitAxialStiff(QVector<TaperedElem> ElemArray, int CurrentPos)
{
    double f1 = 0.0;



        //Add up contribution from outer elements, except for last element
    if(CurrentPos != (int)ElemArray.size()){

            for(int j = CurrentPos+1; j < (int)ElemArray.size();j++){
                f1 += pow(omega,2) * ElemArray.at(j).l/2.0 * (
                         (b(2,1) * ElemArray.at(j).cy + b(2,2) * ElemArray.at(j).cz ) *
                        (ElemArray.at(j).Properties0.RhoA + ElemArray.at(j).Properties1.RhoA)
                        + ElemArray.at(j).l * (b(2,1) * ElemArray.at(j).b(2,1) + b(2,2) * ElemArray.at(j).b(2,2))
                        * (ElemArray.at(j).Properties0.RhoA + 2.0 * ElemArray.at(j).Properties1.RhoA)/3.0   );
            }
    }


//        ! Add contribution of current element to outer elements (if any)
        f1 += pow(omega,2) * l/2.0 * ((b(2,1) * cy + b(2,2) * cz ) * (Properties0.RhoA + Properties1.RhoA)
                + l * (pow(b(2,1),2) + pow(b(2,2),2))
                * (Properties0.RhoA + 2.0 * Properties1.RhoA)/3.0);

        AxialStiff.resize(Row,Row);
        AxialStiff.setZero();
        AxialStiff(0,0) = f1/(30.0*l) * 36.0 -c0/60.0 * 36.0 -d0*l/420.0  * 72.0 -e0* pow(l,2)/2520.0 * 180.0;
        AxialStiff(0,4) = f1/(30.0*l) * 3.0*l -c0/60.0 * 6.0*l -d0*l/420.0  * 15.0*l -e0* pow(l,2)/2520.0 * 42.0*l;
        AxialStiff(0,6) = f1/(30.0*l) * -36.0 -c0/60.0 * -36.0 -d0*l/420.0  * -72.0 -e0* pow(l,2)/2520.0 * -180.0;
        AxialStiff(0,10) = f1/(30.0*l) * 3.0*l -c0/60.0 * 0.0*l -d0*l/420.0  * -6.0*l -e0* pow(l,2)/2520.0 * -30.0*l;
        AxialStiff(1,1) = f1/(30.0*l) * 36.0 -c0/60.0 * 36.0 -d0*l/420.0  * 72.0 -e0* pow(l,2)/2520.0 * 180.0;
        AxialStiff(1,3) = f1/(30.0*l) * -3.0*l -c0/60.0 * -6.0*l -d0*l/420.0  * -15.0*l -e0* pow(l,2)/2520.0 * -42.0*l;
        AxialStiff(1,7) = f1/(30.0*l) * -36.0 -c0/60.0 * -36.0 -d0*l/420.0  * -72.0 -e0* pow(l,2)/2520.0 * -180.0;
        AxialStiff(1,9) = f1/(30.0*l) * -3.0*l -c0/60.0 * 0.0*l -d0*l/420.0  * 6.0*l -e0* pow(l,2)/2520.0 * 30.0*l;
        AxialStiff(3,3) = f1/(30.0*l) * 4.0* pow(l,2) -c0/60.0 * 2.0* pow(l,2) -d0*l/420.0  * 4.0* pow(l,2) -e0* pow(l,2)/2520.0 * 11.0* pow(l,2);
        AxialStiff(3,7) = f1/(30.0*l) * 3.0*l -c0/60.0 * 6.0*l -d0*l/420.0  * 15.0*l -e0* pow(l,2)/2520.0 * 42.0*l;
        AxialStiff(3,9) = f1/(30.0*l) * - pow(l,2) -c0/60.0 * - pow(l,2) -d0*l/420.0  * -3.0* pow(l,2) -e0* pow(l,2)/2520.0 * -11.0* pow(l,2);
        AxialStiff(4,4) = f1/(30.0*l) * 4.0* pow(l,2) -c0/60.0 * 2.0* pow(l,2) -d0*l/420.0  * 4.0* pow(l,2) -e0* pow(l,2)/2520.0 * 11.0* pow(l,2);
        AxialStiff(4,6) = f1/(30.0*l) * -3.0*l -c0/60.0 * -6.0*l -d0*l/420.0  * -15.0*l -e0* pow(l,2)/2520.0 * -42.0*l ;
        AxialStiff(4,10) = f1/(30.0*l) * - pow(l,2) -c0/60.0 * - pow(l,2) -d0*l/420.0  * -3.0* pow(l,2) -e0* pow(l,2)/2520.0 * -11.0* pow(l,2) ;
        AxialStiff(6,6) = f1/(30.0*l) * 36.0 -c0/60.0 * 36.0 -d0*l/420.0  * 72.0 -e0* pow(l,2)/2520.0 * 180.0;
        AxialStiff(6,10) = f1/(30.0*l) * -3.0*l -c0/60.0 * 0.0*l -d0*l/420.0  * 6.0*l -e0* pow(l,2)/2520.0 * 30.0*l ;
        AxialStiff(7,7) = f1/(30.0*l) * 36.0 -c0/60.0 * 36.0 -d0*l/420.0  * 72.0 -e0* pow(l,2)/2520.0 * 180.0;
        AxialStiff(7,9) = f1/(30.0*l) * 3.0*l -c0/60.0 * 0.0*l -d0*l/420.0  * -6.0*l -e0* pow(l,2)/2520.0 * -30.0*l ;
        AxialStiff(9,9) = f1/(30.0*l) * 4.0* pow(l,2) -c0/60.0 * 6.0* pow(l,2) -d0*l/420.0  * 18.0* pow(l,2) -e0* pow(l,2)/2520.0 * 65.0* pow(l,2) ;
        AxialStiff(10,10) = f1/(30.0*l) * 4.0* pow(l,2) -c0/60.0 * 6.0* pow(l,2) -d0*l/420.0  * 18.0* pow(l,2) -e0* pow(l,2)/2520.0 * 65.0* pow(l,2);

        AxialStiff = AxialStiff.selfadjointView<Eigen::Upper>();
        //std::cout<<AxialStiff<<std::endl;
        K.setZero();
        //Assemble full stiffness matrix
        K =GiantBtr*ElasticStiff*GiantB+ GiantBtr*AxialStiff*GiantB+ GiantBtr*AxialRed*GiantB-GiantBtr*SpinStiff*GiantB;

}
