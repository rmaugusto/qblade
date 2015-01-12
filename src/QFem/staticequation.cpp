#include "staticequation.h"
#include<iostream>
#include "QDebug"
#include <Eigen/Dense>
#include <algorithm>
void StaticEquation::DeleteForceDOF(int DelDOF, MatrixXd &InputMat)
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
            //std::cout<<"ReplaceRigidForce "<<p<<std::endl;
            p++;
        }
        InputMat.conservativeResize(maxRows-1,1);
    }
    catch(const char* msg)
    {
        std::cerr << msg << std::endl<<
                     "DOF "<< DelDOF<<" Not Deleted!"<<std::endl;
    }

}

void StaticEquation::DeleteDOFs(std::vector<int> DOFList)
{
	EqnMotion::DeleteDOFs(DOFList);
	//std::cout<<"StaticEquation2"<<std::endl;
	if(!DOFList.empty()){
		std::sort(DOFList.begin(),DOFList.end());
		for (int y = 0; y < (int)DOFList.size(); y++)
		{
			DeleteForceDOF(DOFList.at(y)-y,ForceVector);
			DeleteForceDOF(DOFList.at(y)-y,DispVector);
		}
	}
}


void StaticEquation::ReplaceRigidForceDOF(int Insert)
{

    int maxRows = ForceVector.rows();
    int p = maxRows-1;
    ForceVector.conservativeResize(maxRows+1,1);
    DispVector.conservativeResize(maxRows+1,1);
    while (p>=Insert)
    {
        //This time add row only as the cols are rigid

        ForceVector.row(p+1) = ForceVector.row(p);
        DispVector.row(p+1) = DispVector.row(p);
        p--;
    }

    ForceVector(Insert,0) = 0;
    DispVector(Insert,0) =0;
}


void StaticEquation::ReplaceRigidDOFs(std::vector<int> DOFList)
{
	//Calls ReplaceRigidForceDOF over all Rigid DOFS
	int y;
	if (!DOFList.empty())
	{
		std::sort(DOFList.begin(),DOFList.end());
		for(y = 0; y < (int)DOFList.size(); y++)
		{
			ReplaceRigidForceDOF(DOFList.at(y));
		}
	}
}

void StaticEquation::InsertForcingTerm(ForcingTerm InputForce)
{
    //Localise the Forces into the vectors given the DOF
    ForceVector(InputForce.DOF,0) +=InputForce.Force;
}

void StaticEquation::StaticSolution()
{
    DispVector = StiffMat.colPivHouseholderQr().solve(ForceVector);
    double relative_error = (StiffMat*DispVector-ForceVector).norm() / ForceVector.norm(); // norm() is L2 norm
    std::cout << "The relative error is:\n" << relative_error << std::endl;


}


void StaticEquation::CreateDeformCoords()
{
    //Load the Deformations into the Load Deform Vector
    std::cout<<"Just Before Rez"<<std::endl;
    DeformedShape.reserve(ForceVector.rows()/6);
    for(int i = 0; i<ForceVector.rows()/6; i++){
        DeformationVector DeformedShapeTemp;
        DeformedShapeTemp.Coords(0)=DispVector(i*6,0);
        DeformedShapeTemp.Coords(1)=DispVector(i*6+1,0);
        DeformedShapeTemp.Coords(2)=DispVector(i*6+2,0);
        DeformedShapeTemp.xAxisAngle=DispVector(i*6+3,0);
        DeformedShapeTemp.yAxisAngle=DispVector(i*6+4,0);
        DeformedShapeTemp.zAxisAngle=DispVector(i*6+5,0);
        DeformedShape.push_back(DeformedShapeTemp);
    }
}

void StaticEquation::BackStrains()
{
    for(int i = 0; i<ForceVector.rows()/6-1; i++)
    {
        Node DiffNode,FirstNode, SecondNode;
        DeformationVector FirstDefNode, SecondDefNode;
        FirstDefNode = DeformedShape.at(i);
        SecondDefNode = DeformedShape.at(i+1);
        DeformationVector ElementDisp = FirstDefNode.Subtract(SecondDefNode);
        FirstNode = NodeVec.at(i);
        SecondNode = NodeVec.at(i+1);
        DiffNode = FirstNode.Subtract(SecondNode);
        double Mag = sqrt(pow(DiffNode.Coords(0),2)+pow(DiffNode.Coords(1),2)+pow(DiffNode.Coords(2),2));
        ElementDisp.Coords = ElementDisp.Coords/Mag;
        ElementDisp.xAxisAngle = ElementDisp.xAxisAngle/Mag;
        ElementDisp.yAxisAngle = ElementDisp.yAxisAngle/Mag;
        ElementDisp.zAxisAngle = ElementDisp.zAxisAngle/Mag;
        StrainVectors.push_back(ElementDisp);
        qDebug()<<ElementDisp.Coords(0)<<","<<ElementDisp.Coords(1)<<","<<ElementDisp.Coords(2)<<","<<ElementDisp.xAxisAngle<<","<<ElementDisp.yAxisAngle<<","<<ElementDisp.zAxisAngle;
    }
}


void StaticEquation::BackStresses(QVector<TaperedElem> InputElems)
{
    for(int i = 0; i < (int)StrainVectors.size(); i++)
    {
        DeformationVector ElemStrain = StrainVectors.at(i);
        TaperedElem ElemProps = InputElems.at(i);
        std::vector<double> xCoords=ElemProps.Properties1.xCoords;
        std::vector<double> yCoords=ElemProps.Properties1.yCoords;
        std::vector<double> VMVect;
        double E = ElemProps.Properties1.Outer.E;
        double G = ElemProps.Properties1.Outer.E/(2*(1+0.33));
//        qDebug()<<"xyVM=[";
        VMStation.clear();
        for(int j = 0; j < (int)xCoords.size(); j++)
        {
            // Rod Element Stresses
            double Rod = ElemStrain.Coords(0,0)*ElemProps.Properties0.EA;
            // X Beam Stresses
            double xBeam = (xCoords.at(j)-ElemProps.Properties1.Xe)*ElemStrain.yAxisAngle*E;
            // Y Beam Stresses
            double yBeam = (yCoords.at(j)-ElemProps.Properties1.Ye)*ElemStrain.xAxisAngle*E;
            //Shaft Stresses

            double ShaftStress = G*sqrt(pow(xCoords.at(j)-ElemProps.Properties1.Xe,2)+pow(yCoords.at(j)-ElemProps.Properties1.Ye,2))*ElemStrain.zAxisAngle;//r*G*gamma
            //VonMises
            double VM = sqrt(0.5*(pow(xBeam-yBeam,2)+pow(xBeam-Rod,2)+pow(Rod-yBeam,2)+6*pow(ShaftStress,2)));
//            qDebug()<<"Radial = "<<Rod/VM<<" XBeam = "<<xBeam/VM<<" yBeam = "<<yBeam/VM<<" Torsion = "<<ShaftStress/VM;
            VMVect.push_back(VM);
            VMStation.append(VM);
//            qDebug()<<xCoords.at(j)-ElemProps.Properties1.Xe<<","<<yCoords.at(j)-ElemProps.Properties1.Ye<<","<<VM;
        }
        VMStresses.append(VMStation);

//        qDebug()<<"]";
//        qDebug()<< "scatter(xyVM(:,1),xyVM(:,2),xyVM(:,3)*3e-5,xyVM(:,3),'fill');axis equal";
        double maxVM, minVM;
        maxVM = *std::max_element(VMVect.begin(),VMVect.end());
        minVM = *std::min_element(VMVect.begin(),VMVect.end());
        qDebug()<<"maxVM"<<maxVM<<"minVM"<<minVM;
    }

}
