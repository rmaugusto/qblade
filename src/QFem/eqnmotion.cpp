#include "eqnmotion.h"
#include <iostream>
#include <assert.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/SVD>
#include <typeinfo>
#include <complex>
#include <map>
#include <QVector>
#include "mode.h"
#include <QDebug>

using namespace Eigen;
using Eigen::MatrixXd;

//Localise the indivdual element into the global matrix using the DOF provided in DOFs
void EqnMotion::Insert(MatrixXd ElemMatrix,std::vector<int> Connectors,MatrixXd &Mat)
{
    for(int i = 0;i < (int)Connectors.size();i++)
    {
        for(int j = 0; j < (int)Connectors.size();j++)
        {
            Mat(Connectors.at(i),Connectors.at(j))+=
                    +ElemMatrix(i,j);
        }
    }
}

void EqnMotion::AddTaperedElem(TaperedElem ElementIn)
{
    EqnMotion::Insert(ElementIn.K,ElementIn.Connectors,StiffMat);
    EqnMotion::Insert(ElementIn.MassMatrix,ElementIn.Connectors,MassMat);
    if(NodeVec.empty())NodeVec.push_back(ElementIn.Properties0.NodeID);
    NodeVec.push_back(ElementIn.Properties1.NodeID);
    if(PrincipleAxis1Vec.empty())PrincipleAxis1Vec.push_back(ElementIn.Properties0.PrincipleAxis1);
    PrincipleAxis1Vec.push_back(ElementIn.Properties1.PrincipleAxis1);
    if(PrincipleAxis2Vec.empty())PrincipleAxis2Vec.push_back(ElementIn.Properties0.PrincipleAxis2);
    PrincipleAxis2Vec.push_back(ElementIn.Properties1.PrincipleAxis2);
    if(AxialAxisVec.empty())AxialAxisVec.push_back(ElementIn.Properties0.AxialAxis);
    AxialAxisVec.push_back(ElementIn.Properties1.AxialAxis);
//    std::cout<<ElementIn.Properties0.NodeID.Coords<<std::endl;
}


void EqnMotion::DeleteDOF(int DelDOF, MatrixXd &InputMat)
{
    try
    {
        int p= DelDOF;
        int maxRows = InputMat.rows();
        if(DelDOF>=maxRows)
        {
            throw "Tried to delete DOF that doesn't exist!";
        }
        while (p<maxRows-1)
        {
            //Shift Delete Rows to the Back.
            InputMat.row(p) = InputMat.row(p+1);
            InputMat.col(p) = InputMat.col(p+1);
            p++;
        }
        InputMat.conservativeResize(maxRows-1,maxRows-1);
    }
    catch(const char* msg)
    {
        std::cerr << msg << std::endl<<
                     "DOF "<< DelDOF<<" Not Deleted!"<<std::endl;
    }

}

void EqnMotion::DeleteDOFs(std::vector<int> DOFList)
{
    int y;
    if (!DOFList.empty())
    {
        std::sort(DOFList.begin(),DOFList.end());
        for(y = 0; y < (int)DOFList.size();y++)
        {
            DeleteDOF(DOFList.at(y)-y,StiffMat);
            DeleteDOF(DOFList.at(y)-y,MassMat);
        }
    }

}

void EqnMotion::ReplaceRigidDOF(int Insert)
{


    int maxRows = EigVector.rows();
    int maxCols = EigVector.cols();
    int p = maxRows-1;
    EigVector.conservativeResize(maxRows+1,maxCols);
    while (p>=Insert)
    {
        //This time add row only as the cols are rigid
        EigVector.row(p+1) = EigVector.row(p);
        p--;
    }
    MatrixXcd a;
    a.setZero(1,EigVector.cols());
    EigVector.row(Insert) = a;
}

void EqnMotion::ReplaceRigidDOFs(std::vector<int> DOFList)
{
    int y;
    if (!DOFList.empty())
    {
        std::sort(DOFList.begin(),DOFList.end());
        for(y = 0; y < (int)DOFList.size();y++)
        {
            ReplaceRigidDOF(DOFList.at(y));
        }
    }

}
void EqnMotion::TrashFilter(MatrixXd &InputMat){
    for(int i= 0;i<InputMat.rows();i++)
    {
        for(int j= 0;j<InputMat.rows();j++)
        {
            if(fabs(InputMat(i,j))<1e-12)
            {
                InputMat(i,j)=0;
            }
        }
    }
}
void EqnMotion::TrashFilter(MatrixXcd &InputMat){
    for(int i= 0;i<InputMat.cols();i++)
    {
        for(int j= 0;j<InputMat.rows();j++)
        {
            std::complex<double> Temp = InputMat(i,j);
            double Temp2 = Temp.real();
            if(fabs(Temp2)<1e-12)
            {

                InputMat(i,j)=0;
            }
        }
    }
}
//----------------------------------------------------------------------------------------
void EqnMotion::GetEigSoln()
{
    int cols = MassMat.cols();
    MatrixXd Full(cols,cols);
    Full = MassMat.inverse()* StiffMat;
    // Trash Filter
    //TrashFilter(Full);

    //Solve Eigenvalue problem
    //SelfAdjointEigenSolver<MatrixXd> eigensolver(Full);
    EigenSolver<MatrixXd> eigensolver(Full);
    try
    {
        if (eigensolver.info() != Success) {throw "EQN Matrix not solving";}
    }
    catch(char const* msg)
    {
        std::cerr<<msg<<std::endl;
		abort();
    }

    // Dump to output for debug
//    std::cout << "The eigenvalues of A are:\n"
//              << eigensolver.eigenvalues() << std::endl;
    //std::cout << "Here's a matrix whose columns are eigenvectors of A \n"
    //<< "corresponding to these eigenvalues:\n"
    //<< eigensolver.eigenvectors() << std::endl;

    //std::cout<<"Type is "<<typeid(eigensolver.eigenvectors()).name()<<std::endl;
    EigValues = eigensolver.eigenvalues();
    EigVector = eigensolver.eigenvectors();
    //TrashFilter(EigVector);
    try{

        // Check for rigid body modes
        std::complex<double> TestValue;
        for(int i = 0; i<EigValues.rows();i++)
        {
            TestValue = EigValues(i,0);
            if (TestValue.real()<0.001||TestValue.real()!=TestValue.real())//Tests for a trash frequency
            {
                throw "Rigid Body Modes";
            }
        }
    }
    catch (char const* msg)
    {
        std::cerr<<"Rigid Body Modes or Error Present in Solution"<<
              ". Consider Constraining Degrees of Freedom. Check Geometry Inputs"<<std::endl;
    }
}

//----------------------------------------------------------------------------------------
void EqnMotion::FitPolynomials(){
//    int DOF = EigVector.rows();
    int Modes = EigVector.cols();
    for(int i=0; i<Modes;i++)
    {
        FitPolynomial(i);
    }
    //Sort the modes by Freq
    SortModes(ModeContainer);
}

void EqnMotion::FitPolynomial(int mode)
{
    int DOF = EigVector.rows();
//    double MagDOF = DOF/6;
    bool DoesItWork;
    std::vector<double> dx,dy;
    double xCount = 0.0;
    double yCount = 0.0;
    double zCount = 0.0;
    double TorsionCount = 0.0;
    double xCount2 = 0.0;
    double yCount2 = 0.0;
    double zCount2 = 0.0;
    std::vector<double> FlapDot,EdgeDot,RadialDot;
    MatrixXd ConMatrix;
    Mode NewMode;
    NewMode.Frequency = sqrt(EigValues(mode,0).real());
    //iterate through the dof of each mode
//    std::cout<<"------------------------------------------------------------------------------"<<std::endl;
    for(int i = 0; i<DOF/6;i++)
            {
                //Create the magnitude of the three translation dof per node
//        double TempMag = pow(EigVector(i*6,mode).real(),1)+
//                        pow(EigVector(i*6+1,mode).real(),1)+
//                        pow(EigVector(i*6+2,mode).real(),1);

        double TempMag2 =pow(pow(EigVector(i*6,mode).real(),2)+
                        pow(EigVector(i*6+1,mode).real(),2)+
                        pow(EigVector(i*6+2,mode).real(),2),0.5);

             UnitVector CurrentModeVec;
            if( TempMag2 ==0)
            {
                CurrentModeVec.Coords(0)=0;
                CurrentModeVec.Coords(1)=0;
                CurrentModeVec.Coords(2)=0;
            }
            else {
                CurrentModeVec.Coords(0)=EigVector(i*6,mode).real()/TempMag2;
                CurrentModeVec.Coords(1)=EigVector(i*6+1,mode).real()/TempMag2;
                CurrentModeVec.Coords(2)=EigVector(i*6+2,mode).real()/TempMag2;
            }
                UnitVector NodePrinc1 = PrincipleAxis1Vec.at(i);
                UnitVector NodePrinc2 = PrincipleAxis2Vec.at(i);
                UnitVector NodeAxis2 = AxialAxisVec.at(i);
                //Dot Product between the elastic axes and the actual deformation direction
                EdgeDot.push_back((NodePrinc1.Coords.dot(CurrentModeVec.Coords)));
                FlapDot.push_back((NodePrinc2.Coords.dot(CurrentModeVec.Coords)));
                RadialDot.push_back((NodeAxis2.Coords.dot(CurrentModeVec.Coords)));

                xCount2 += TempMag2*fabs(NodePrinc1.Coords.dot(CurrentModeVec.Coords));
                yCount2 += TempMag2*fabs(NodePrinc2.Coords.dot(CurrentModeVec.Coords));
                zCount2 += TempMag2*fabs(NodeAxis2.Coords.dot(CurrentModeVec.Coords));

                //std::cout<<"Current Mode Vec"<<CurrentModeVec.Coords<<std::endl;
//                std::cout<<"x dot ="<<NodePrinc1.Coords.dot(CurrentModeVec.Coords)<<
//                           " y dot = "<<NodePrinc2.Coords.dot(CurrentModeVec.Coords)<<
//                           " z dot = "<<NodeAxis2.Coords.dot(CurrentModeVec.Coords)<<std::endl;

                //keep track of which dof is dominating - Obsolete Used for Screen Print out only
                xCount += fabs(EigVector(i*6,mode).real());
                yCount += fabs(EigVector(i*6+1,mode).real());
                zCount += fabs(EigVector(i*6+2,mode).real());
                TorsionCount+= fabs(EigVector(i*6+5,mode).real());
                //Push into the vectors used to create the polyfit
                dy.push_back(TempMag2);
                Node TempNode = NodeVec.at(i);//
                //insert Normalised x
                Node LastNode = NodeVec.at(NodeVec.size()-1);
                double NormalisedX = TempNode.Coords(2)/LastNode.Coords(2);
                dx.push_back(NormalisedX);

//                std::cout<< i<< "   x"<< EigVector(i*6,mode).real()<<
//                           "; y ="<<EigVector(i*6+1,mode).real()<<
//                           "; z = "<<EigVector(i*6+2,mode).real()<<
//                           "xAlp ="<< EigVector(i*6+3,mode).real()<<
//                           "; yAlp ="<<EigVector(i*6+4,mode).real()<<
//                           "; zAlp = "<<EigVector(i*6+5,mode).real()<<std::endl;

                //Load the Deformations into the Load Deform Vector
                DeformationVector NewDeform;
                NewDeform.Coords(0)=EigVector(i*6,mode).real();
                NewDeform.Coords(1)=EigVector(i*6+1,mode).real();
                NewDeform.Coords(2)=EigVector(i*6+2,mode).real();
                NewDeform.zAxisAngle=EigVector(i*6+5,mode).real();
                NewMode.ModeShape.push_back(NewDeform);
            }


    // sort into the correct mode shape group
    if(xCount2>1.2*yCount2&&xCount2>1.2*zCount&&10.0*xCount2>TorsionCount){
        NewMode.ModeType=Edge;
    }
    else if(yCount2>1.2*xCount2&&yCount2>zCount2&&10.0*yCount2>TorsionCount)
    {
        NewMode.ModeType = Flap;

    }
    else if(zCount2>1.2*xCount2&&zCount2>1.2*yCount2&&zCount2>TorsionCount)
    {
        NewMode.ModeType =Radial;
    }
    else if(TorsionCount>xCount2&&TorsionCount>yCount2&&TorsionCount>zCount2)
    {
        NewMode.ModeType = Torsion;
    }
    else
    {
        NewMode.ModeType = UnSorted;
    }
    //Project the Mode onto the Dominant direction with positive and negative.

            for(int i = 0; i<DOF/6;i++)
            {
//                std::cout<<"Flap Dot" <<FlapDot.at(i)<<std::endl;
                switch(NewMode.ModeType){
                    case 0://Flap
                    {
                        if(dy.at(i)!=0)dy.at(i)=dy.at(i)*FlapDot.at(i)/fabs(FlapDot.at(i));
                        break;
                    }
                    case 1://Edge
                    {
                        if(dy.at(i)!=0)dy.at(i)=dy.at(i)*EdgeDot.at(i)/fabs(EdgeDot.at(i));
                        break;
                    }
                    case 2://Radial
                    {
                        if(dy.at(i)!=0)dy.at(i)=dy.at(i)*RadialDot.at(i)/fabs(RadialDot.at(i));

                        break;
                    }
                    case 3://Torsional
                    {
                        break;
                    }
                    case 4://UnSorted
                    {
                        break;
                    }
                }
            }
//            std::cout<<"Influence Summations"<<std::endl;
//            std::cout<<"x ="<< xCount<<"; y ="<<yCount<<"; z = "<<zCount<<"; Torsion = "<<TorsionCount<<std::endl;
//            std::cout<<"Ideal Edge2 ="<< xCount2<<"; Ideal Flap 2 ="<<yCount2<<"; z = "<<zCount<<"; Torsion = "<<TorsionCount<<std::endl;

    DoesItWork = true;
    assert(DoesItWork);

    MatrixXd xPolyVals(dx.size(),5);
    MatrixXd yPolyVals(dy.size(),1);

    for (unsigned int i = 0 ; i< dx.size(); i++)
    {
        //Create the x^2 + x^3.... equations
        xPolyVals(i,0) = pow(dx.at(i),2);
        xPolyVals(i,1) = pow(dx.at(i),3);
        xPolyVals(i,2) = pow(dx.at(i),4);
        xPolyVals(i,3) = pow(dx.at(i),5);
        xPolyVals(i,4) = pow(dx.at(i),6);
        // Create the dy matrix
        yPolyVals(i,0) = dy.at(i);
    }

    ConMatrix = xPolyVals.jacobiSvd(ComputeThinU|ComputeThinV).solve(yPolyVals);

    // Normalize mode shapes to a tip deflection of one
    double summation = ConMatrix.sum();
    ConMatrix /=summation;

    NewMode.Polynomial = ConMatrix;
    //assert(UngroupedFreqs.size()==0);//Otherwise your modes probably aren't sorting into the proper category because the blades are too twisted
    //    NewMode.PrintMode();
    ModeContainer.push_back(NewMode);

}

void EqnMotion::SortModes(QVector<Mode> &ModeCon)
{
    //Sort using map
    typedef std::map<double, Mode> PairType;
    PairType Pairs;
    //Load into map with default sorting
    for(int i = 0; i < (int)ModeCon.size();i++)
    {
        Mode TempMode = ModeCon.at(i);
    Pairs[TempMode.Frequency] = ModeCon.at(i);
    }

    //Load back into the object containers


    ModeCon.clear();
    for(PairType::iterator i = Pairs.begin();i!=Pairs.end();i++)
    {    
       ModeCon.push_back(i->second);
    }

}

//using namespace Eigen;
/**
 * An SVD based implementation of the Moore-Penrose pseudo-inverse
 */
MatrixXd EqnMotion::pinv(MatrixXd& m, double epsilon) {
    //    	   ei_assert(m_isInitialized && "SVD is not initialized.");
    //    	   double  pinvtoler=1.e-6; // choose your tolerance widely!
    //    	   SingularValuesType m_sigma_inv=m_sigma;
    //    	   for ( long i=0; i<m_workMatrix.cols(); ++i) {
    //    	      if ( m_sigma(i) > pinvtoler )
    //    	         m_sigma_inv(i)=1.0/m_sigma(i);
    //    	     else m_sigma_inv(i)=0;
    //    	   }
    //    	   pinvmat= (m_matV*m_sigma_inv.asDiagonal()*m_matU.transpose());
    typedef JacobiSVD<MatrixXd> SVD;
    SVD svd(m, ComputeFullU | ComputeFullV);
    typedef SVD::SingularValuesType SingularValuesType;
    const SingularValuesType singVals = svd.singularValues();
    SingularValuesType invSingVals = singVals;

    for(int i=0; i<singVals.rows(); i++) {
        if(singVals(i) <= epsilon) {
            invSingVals(i) = 0.0; // FIXED can not be safely inverted
        }
        else {
            invSingVals(i) = 1.0 / invSingVals(i);
        }
    }


    return MatrixXd(svd.matrixV() *
            invSingVals.asDiagonal() *
            svd.matrixU().transpose());
}
//void EqnMotion::CreateDeformationNode(std::vector<DeformationVector> & DeformVect):
//{

//}

//Obs
//----------------------------------------------------------------------------------------
//void EqnMotion::FitPolynomial()
//{

//    int DOF = EigVector.rows();
//    int Modes = EigVector.cols();

//    //Set up Coefficient Matrix
//    MatrixXd DOFMatrix;
//    DOFMatrix.resize(DOF/6,DOF/6);
//    DOFMatrix.setZero();
//    for(int j=0; j<DOF/6;j++)
//    {
//        for(int i = 0; i<DOF/6;i++)
//        {
//          DOFMatrix(j,i) = pow(j/((double)((DOF/6)-1)),i);
//        }
//    }

//    MatrixXd InvDOF;
//    InvDOF = DOFMatrix.inverse();
//    std::cout<<DOFMatrix<<std::endl;

//    //Reduce deformation into single magnitude ignoring angles
//    MatrixXd MagMatrix,ConMatrix;
//    MagMatrix.resize(DOF/6,Modes);
//    ConMatrix.resize(DOF/6,Modes);
//    ConMatrix.setZero();
//    MagMatrix.setZero();
//    //Counters used to compare the components of the eigenvector
//    double xCount,yCount,zCount,TorsionCount;
//    xCount = 0;
//    yCount = 0;
//    zCount = 0;
//    TorsionCount=0;
//    //iterate through the modes
//    for(int j=0; j<Modes;j++)
//    {
//        //iterate through the dof of each mode
//        for(int i = 0; i<DOF/6;i++)
//        {
//            //Create the magnitude of the three translation dof per node
//            double TempMag =
//                    pow(EigVector(i*6,j).real(),2)+
//                    pow(EigVector(i*6+1,j).real(),2)+
//                    pow(EigVector(i*6+2,j).real(),2);
//            //keep track of which dof is dominating
//            xCount += fabs(EigVector(i*6,j).real());
//            yCount += fabs(EigVector(i*6+1,j).real());
//            zCount += fabs(EigVector(i*6+2,j).real());
//            TorsionCount+= fabs(EigVector(i*6+5,j).real());

//            std::cout<<"x ="<< EigVector(i*6,j).real()<<
//                       "; y ="<<EigVector(i*6+1,j).real()<<
//                       "; z = "<<EigVector(i*6+2,j).real()<<
//                       "xAlp ="<< EigVector(i*6+3,j).real()<<
//                       "; yAlp ="<<EigVector(i*6+4,j).real()<<
//                       "; zAlp = "<<EigVector(i*6+5,j).real()<<std::endl;
//            TempMag= pow(TempMag,0.5);
//            MagMatrix(i,j) = TempMag;
//        }

//        //Solve polynomial matrix equation
//        ConMatrix.col(j) = InvDOF*MagMatrix.col(j);
//        //Normalize polynomial so teh tip deflection is one
//        double sumValue = ConMatrix.col(j).sum();
//        ConMatrix.col(j)/=sumValue;
//        //Sort Modes into Categories and then print.
//        ModeSorter(ConMatrix,xCount, yCount,zCount, TorsionCount, true,j);
//        //Reset the counter when moving onto the next mode
//        xCount = 0;
//        yCount = 0;
//        zCount = 0;
//        TorsionCount=0;
//    }

//}

//void EqnMotion::ModeSorter(MatrixXd ConMatrix, double xCount, double yCount, double zCount, double TorsionCount, bool Verbose, int j)
//{

//    if(Verbose) std::cout<<"Summation"<<"x ="<< xCount<<"; y ="<<yCount<<"; z = "<<zCount<< "Torsion = "<<TorsionCount<<std::endl;
//    //Load into the edge, flap or axial vectors
//    if(xCount>yCount&&xCount>zCount&&xCount>TorsionCount){
//        if(Verbose) std::cout<<"x Dominated/EdgeWise"<<std::endl;
//        EdgeModes.push_back(ConMatrix.col(j));
//        EdgeFreqs.push_back(EigValues(j,0).real());
//    }
//    if(yCount>xCount&&yCount>zCount&&yCount>TorsionCount)
//    {
//        if(Verbose) std::cout<<"y Dominated/FlapWise"<<std::endl;
//        FlapModes.push_back(ConMatrix.col(j));
//        FlapFreqs.push_back(EigValues(j,0).real());

//    }
//    if(zCount>xCount&&zCount>yCount&&zCount>TorsionCount)
//    {
//        if(Verbose) std::cout<<"z Dominated/Radial"<<std::endl;
//        AxialModes.push_back(ConMatrix.col(j));
//        AxialFreqs.push_back(EigValues(j,0).real());
//    }
//    if(TorsionCount>xCount&&TorsionCount>yCount&&TorsionCount>zCount)
//    {
//        if(Verbose) std::cout<<"Torsion Mode"<<std::endl;
//        TorsionModes.push_back(ConMatrix.col(j));
//        TorsionFreqs.push_back(EigValues(j,0).real());
//    }

//    std::cout<<"Frequency "<<EigValues(j,0).real()<<" Rad/s"<<std::endl;
//    MatrixXd Print;
//    Print = ConMatrix.col(j);
//    std::cout<<Print(0,0);
//    for(int k=1; k<Print.rows();k++)
//    {
//        std::cout<<"+("<<Print(k,0)<<"*x^"<<k<<")";
//    }
//    std::cout<<std::endl;

//    std::cout<<"------------------------------------------------------------------------------"<<std::endl;
//}
