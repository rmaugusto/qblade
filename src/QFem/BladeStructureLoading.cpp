#include "BladeStructureLoading.h"
#include "../Serializer.h"

#include "BladeStructure.h"
#include "structelem.h"
#include "taperedelem.h"
#include "eqnmotion.h"
#include "staticequation.h"
#include <stdlib.h>
#include "QDebug"
#include "../MainFrame.h"
#include <omp.h>
#include "../XBEM/BEM.h"
#include "../StoreAssociatedComboBox.h"
#include "../XBEM/TBEMData.h"
#include <functional>
#include "../Store.h"
#include "QFEMModule.h"
#include "QFEMDock.h"


BladeStructureLoading::BladeStructureLoading()
: StorableObject ("< no name >"){

}

BladeStructureLoading::BladeStructureLoading(QString name, BladeStructure *structure)
    : StorableObject (name, structure) {

    simulationFinished = false;
    m_structure = structure;

    for (int i=0;i<m_structure->m_numElems;i++){
    normalLoading.append(0);
    tangentialLoading.append(0);
    sectionWidth.append(0);
    }



}

BladeStructureLoading::~BladeStructureLoading() {

}

void BladeStructureLoading::restorePointers() {
    StorableObject::restorePointers();

	g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&m_structure));
}

QStringList BladeStructureLoading::prepareMissingObjectMessage() {
	if (g_bladestructureloadingStore.isEmpty() && g_QFEMModule->m_QFEMDock->m_Loading == NULL) {
		QStringList message = BladeStructure::prepareMissingObjectMessage();
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == QFEMMODULE && g_mainFrame->m_iView == QFEMLOADINGVIEW) {
				message = QStringList(">>> Click 'New' to create a new Structural Loading");
			} else {
				message = QStringList(">>> Create a new Static Blade Loading in the 'Static Blade Loading' Tab");
			}
		}
		message.prepend("- No Static Blade Loading in Database");
		return message;
	} else {
		return QStringList();
	}
}

void BladeStructureLoading::serialize() {
    StorableObject::serialize();

    g_serializer.readOrWriteStorableObject (&m_structure);

    g_serializer.readOrWriteDoubleVector1D (&normalLoading);
    g_serializer.readOrWriteDoubleVector1D (&tangentialLoading);
    g_serializer.readOrWriteDoubleVector1D (&sectionWidth);


    g_serializer.readOrWriteDoubleVector2D (&nodeTranslations);
    g_serializer.readOrWriteDoubleVector2D (&VMStresses);

    g_serializer.readOrWriteBool(&simulationFinished);
}

void BladeStructureLoading::Duplicate(BladeStructureLoading *pLoading)
{
    //
    // Copies the blade structure data from an existing blade structure
    //

    normalLoading.resize(pLoading->normalLoading.size());
    tangentialLoading.resize(pLoading->tangentialLoading.size());
    sectionWidth.resize(pLoading->sectionWidth.size());


    for (int i=0;i<pLoading->tangentialLoading.size();i++)
    {
        normalLoading[i] = pLoading->normalLoading[i];
        tangentialLoading[i] = pLoading->tangentialLoading[i];
        sectionWidth[i] = pLoading->sectionWidth[i];
    }

}

BladeStructureLoading* BladeStructureLoading::newBySerialize() {
    BladeStructureLoading* bladeStructureLoading = new BladeStructureLoading ();
    bladeStructureLoading->serialize();
    return bladeStructureLoading;
}

bool BladeStructureLoading::RunModalTest()
{
    QVector<StructElem> SectionStructures(m_structure->m_numElems);
    int MaxDOF;


    // *******************Start Paralell Calculation Section**********************
    #pragma omp parallel default (none) shared (SectionStructures)
    {
    #pragma omp for

        for (int i=0;i < m_structure->m_numElems ; i++)
        {
            if (! *m_cancelCalculation)
            {

            if (! *m_cancelCalculation) emit updateProgress();




            //Create a node for the structural properties to be attached to.
            Node TempNode;
            TempNode.Coords(0)=m_structure->PAxisX.at(i);
            TempNode.Coords(1)=-m_structure->PAxisZ.at(i);
            TempNode.Coords(2)=m_structure->PAxisY.at(i);
            TempNode.IDnumber= i;

            for(int j=0; j<6;j++)//Give each node its identifier.
            {
                //Create and identifyer with the DOF
                TempNode.DOFNums.push_back(j+i*6);
            }

            std::vector<double> xCoords, yCoords;

            for(int j=0; j<m_structure->m_numFoilPoints-2;j++)//push coordinates into arrays
            {
                xCoords.push_back(m_structure->xFoilCoords[i][j]);
                yCoords.push_back(m_structure->yFoilCoords[i][j]);

            }

            if (! *m_cancelCalculation)
            {

            StructElem TempStruct(TempNode,xCoords,yCoords,m_structure->ChordTwist.at(i),m_structure->ShellEMod,m_structure->ChordLengths.at(i),m_structure->ShellRho,m_structure->LocalXOffset.at(i),m_structure->LocalYOffset.at(i)); // Create a Foil
            TempStruct.CalcOuterRef();

            if (! *m_cancelCalculation) emit updateProgress();

            //With Coordinates X,Y, Elastic Mod, Chord Length, Density
            if(m_structure->StructType < 2 && ! *m_cancelCalculation)
            {
                TempStruct.CreateInner(m_structure->ShellThickness.at(i)); // Make it hollow
                if (TempStruct.Hollow) TempStruct.CalcInnerRef();
            }

            if (! *m_cancelCalculation) emit updateProgress();


            if(m_structure->StructType == 0 && ! *m_cancelCalculation)
            {
                 if (TempStruct.Hollow) TempStruct.CreateSpar(m_structure->SparLocation.at(i),m_structure->SparThickness.at(i),m_structure->SparAngle.at(i),m_structure->SparEMod,m_structure->SparRho);//Insert a spar
                 if (TempStruct.Hollow) TempStruct.CalcSparRef();
            }


            //Located at 0.4 of the chord, 0.15*chordlength thick,at zero degrees,Elastic mod, Density


            TempStruct.LocaliseAboutElasticAxis();//move the I values from reference axis to the elastic axis
            SectionStructures[i]=(TempStruct);
            }
        }
        }

    }
    // *****************End Paralell Calculation Section*****************

    if (*m_cancelCalculation) return false;


    MaxDOF=SectionStructures.size()*6;//Plus one because it is used to declare the matrix size.

    //-----------------------Create the local Element matricies--------------------------
    QVector<TaperedElem> Elements;
    for(int i = 0; i < (int)SectionStructures.size()-1;i++)
    {
    TaperedElem TempElem(SectionStructures.at(i),SectionStructures.at(i+1),m_structure->Omega);
    Elements.push_back(TempElem);
    }
    //Needs to be done in two sections so we can create the axial stiffness matricies.
    for(int i=0; i < (int)SectionStructures.size()-1;i++)
    {
        Elements[i].InitAxialStiff(Elements,i);
    }
    //---------------------------------------------------------------------------------//
    //Create the equations of motions by taking local stiffness matrix and globalising //
    EqnMotion ModalEquations(MaxDOF);
    for(int i = 0; i < (int)Elements.size();i++)
    {
        ModalEquations.AddTaperedElem(Elements.at(i));
    }

    //-------------------------Constrain Degrees of Freedom ---------------------------
    // Now constrain degrees of freedom.
    std::vector<int> FixedDOF;
    //Fake DOF Constraints to remove rigid Body modes
    FixedDOF.push_back(0);
    FixedDOF.push_back(1);
    FixedDOF.push_back(2);
    FixedDOF.push_back(3);
    FixedDOF.push_back(4);
    FixedDOF.push_back(5);

    //Delete the lines from the equations
    ModalEquations.DeleteDOFs(FixedDOF);// Add Constraint



    //---------------stroring of results in the objects arrays fo later use and visualization purposes-----------

    //STATIC SOLVER CODE TEMP HERE
    //START OF STATIC EQUATIONS

        StaticEquation TipDeflectionTest(MaxDOF);



        //Integrate forces over the blade and place onto nodes
        for(int j =0; j<int(SectionStructures.size());j++)
        {

            //Place forcing terms into the system of equations
            ForcingTerm TangTerm(6*(j),tangentialLoading.at(j));
            ForcingTerm NormTerm(6*j+1,normalLoading.at(j));
            TipDeflectionTest.InsertForcingTerm(TangTerm);
            TipDeflectionTest.InsertForcingTerm(NormTerm);
        }

        for (int n = 0; n < (int)Elements.size(); n++)
        {
            TipDeflectionTest.AddTaperedElem(Elements.at(n));
        }

        std::vector<int> DeleteList;

        DeleteList.push_back(0);
        DeleteList.push_back(1);
        DeleteList.push_back(2);
        DeleteList.push_back(3);
        DeleteList.push_back(4);
        DeleteList.push_back(5);

        TipDeflectionTest.DeleteDOFs(DeleteList);

        TipDeflectionTest.StaticSolution();

        TipDeflectionTest.ReplaceRigidDOFs(DeleteList);

        TipDeflectionTest.CreateDeformCoords();



        for (int j = 0; j<int(TipDeflectionTest.DeformedShape.size());j++)
        {
            DeformationVector TempDeform = TipDeflectionTest.DeformedShape.at(j);
            TempDeform.PrintCoords();
        }

        TipDeflectionTest.BackStrains();
        TipDeflectionTest.BackStresses(Elements);





        QVector < double > disp;
        for (int ii=0;ii < m_structure->m_numElems ; ii++)
        {
        disp.clear();
        disp.append(TipDeflectionTest.DispVector(ii*6,0));
        disp.append(TipDeflectionTest.DispVector(ii*6+1,0));
        disp.append(TipDeflectionTest.DispVector(ii*6+2,0));
        disp.append(TipDeflectionTest.DispVector(ii*6+5,0));
        nodeTranslations.append(disp);
        }

        // the von mises stresses are interpolated from the element center to the element edges here
        QVector < double > VM;
        for (int ii=0;ii < m_structure->m_numElems ; ii++)
        {
            VM.clear();
            for (int jj=0;jj<TipDeflectionTest.VMStresses.at(0).size();jj++)
            {



                if (ii == 0)
                {
                // now check if the whole blade has just one element (prevent a crash here)
                if (2 < m_structure->m_numElems){
                double stressi1 = TipDeflectionTest.VMStresses.at(ii+1).at(jj);
                double posi1 = (m_structure->Radii.at(ii+1)+m_structure->Radii.at(ii+2))/2.0;
                double posi = (m_structure->Radii.at(ii)+m_structure->Radii.at(ii+1))/2.0;
                double stressi = TipDeflectionTest.VMStresses.at(ii).at(jj);
                VM.append(stressi1-posi1*(stressi1-stressi)/(posi1-posi));
                }
                else{
                double stressi = TipDeflectionTest.VMStresses.at(ii).at(jj);
                VM.append(stressi);
                }

                }

                else if (ii<m_structure->m_numElems-1)
                {
                double posim1 = (m_structure->Radii.at(ii-1)+m_structure->Radii.at(ii))/2.0;
                double stressim1 = TipDeflectionTest.VMStresses.at(ii-1).at(jj);
                double posi = (m_structure->Radii.at(ii)+m_structure->Radii.at(ii+1))/2.0;
                double stressi = TipDeflectionTest.VMStresses.at(ii).at(jj);

                VM.append(stressim1+(stressi-stressim1)/(posi-posim1)*(m_structure->Radii.at(ii)-posim1));
                }

                else
                {
                VM.append(0);
                }

            }
            VMStresses.append(VM);
        }

        m_structure->addQVecToResults(this);
        m_structure->FillVariableList(this);

        simulationFinished = true;

    return true;

}
