#include "BladeStructure.h"
#include "structelem.h"
#include "taperedelem.h"
#include "eqnmotion.h"
#include <stdlib.h>
#include "QDebug"
#include "../MainFrame.h"
#include <omp.h>
#include "../Serializer.h"
#include "../Graph/NewCurve.h"
#include "../Store.h"
#include "QFEMModule.h"
#include "../ColorManager.h"


BladeStructure* BladeStructure::newBySerialize() {
	BladeStructure* bladeStructure = new BladeStructure ();
	bladeStructure->serialize();
	return bladeStructure;
}

BladeStructure::BladeStructure()
    : StorableObject ("< no name >"),
      ShowAsGraphInterface (false)
{
    FillVariableList();
}

BladeStructure::BladeStructure(QString name, CBlade *rotor)
	: StorableObject (name, rotor)
{
	m_pen.setColor(g_colorManager.getLeastUsedColor(&g_bladeStructureStore));
	m_pen.setWidth(1);
	m_pen.setStyle(Qt::SolidLine);
	m_numElems = rotor->m_NSurfaces+1;
	m_rotor = rotor;
    Omega = 0;
    QFEMCompleted = false;

    AbsoluteShell = false;
    AbsoluteSpar = false;

    FillVariableList();


    sparColor = QColor(180,180,180);
    shellColor = QColor(220,220,220);

	m_numFoilPoints = 50;

	EIx.resize(m_numElems);
	EIy.resize(m_numElems);
	EI1.resize(m_numElems);
	EI2.resize(m_numElems);
	EA.resize(m_numElems);
	RhoA.resize(m_numElems);
	GJ.resize(m_numElems);
	StructuralTwist.resize(m_numElems);
	ShellThickness.resize(m_numElems);
	SparLocation.resize(m_numElems);
	SparThickness.resize(m_numElems);
	SparAngle.resize(m_numElems);




	for (int i=0;i<m_numElems;i++)
	{
        SparLocation[i] = 0.25;
        SparThickness[i] = 0.08;
		SparAngle[i] = 0;
        ShellThickness[i] = 0.02;
	}

	StructType = 0;
    SparEMod = 7.3e10;
    ShellEMod = 7.3e10;
    SparRho = 2900;
    ShellRho = 2900;

	ReadSectionCoordinates();

}

void BladeStructure::ReadSectionCoordinates()
{

    ChordLengths.clear();
    Radii.clear();
    ChordTwist.clear();
    LocalXOffset.clear();
    LocalYOffset.clear();
    PAxisX.clear();
    PAxisY.clear();
    PAxisZ.clear();


	for (int i=0;i<m_numElems;i++)
	{

		ChordLengths.append(m_rotor->m_TChord[i]);

		Radii.append(m_rotor->m_TRelPos[i]);

        ChordTwist.append(m_rotor->m_TTwist[i]);

		LocalXOffset.append(m_rotor->m_TFoilPAxisX[i]*m_rotor->m_TChord[i]);

		LocalYOffset.append(m_rotor->m_TFoilPAxisZ[i]*m_rotor->m_TChord[i]);

		PAxisX.append(m_rotor->m_TPAxisX[i]);

		PAxisY.append(m_rotor->m_TPAxisY[i]);

        PAxisZ.append(m_rotor->m_TOffsetZ[i]);

		////////now the coordinates for the airfoil are read from the foil data stored in the associated cblade object

		double x = 0, angle_incr = 0, angle = 0, totlength=0;

		///// create sinusoidal spacing in x for reading in of airfoil coordinates (for better leading edge resolution)
		for (int l=0;l<int(m_numFoilPoints/2);l++)
		{
			angle_incr = PI / (m_numFoilPoints/2);
			totlength+=sin(angle);
			angle+=angle_incr;
		}

		angle = 0;
        int end = 0;
		///// read in upper (suction) surface coordinates
		for (int l=0;l<int(m_numFoilPoints/2);l++)
		{
			xFoilCoords[i][l]=(1-x);
			yFoilCoords[i][l]=(-m_rotor->GetFoilFromStation(i)->GetUpperY(1-x));
			angle += angle_incr;
			x += sin(angle)/totlength;
            end = l;
		}
        end += 1;

		x=0;
		angle = 0;

		///// read in lower (pressure) surface coordinates
        for (int l=0;l<int(m_numFoilPoints/2)-2;l++)
		{
            angle += angle_incr;
            x += sin(angle)/totlength;
            xFoilCoords[i][end+l]=(x);
            yFoilCoords[i][end+l]=(-m_rotor->GetFoilFromStation(i)->GetLowerY(x));
		}





	}
}

BladeStructure::~BladeStructure() {

}

NewCurve* BladeStructure::newCurve(QString xAxis, QString yAxis, NewGraph::GraphType /*graphType*/) {
    const int xAxisIndex = m_availableVariables.indexOf(xAxis);
    const int yAxisIndex = m_availableVariables.indexOf(yAxis);

    if (xAxisIndex == -1 || yAxisIndex == -1) {
        return NULL;
    } else {
        NewCurve *curve = new NewCurve (this);
		curve->setAllPoints(m_results[xAxisIndex].data(),
							m_results[yAxisIndex].data(),
							m_results[0].size());  // numberOfRows is the same for all results
		return curve;
    }
}

void BladeStructure::Duplicate(BladeStructure *pStructure)
{
	//
	// Copies the blade structure data from an existing blade structure
	//

	m_numElems = pStructure->m_numElems;

	EIx.resize(m_numElems);
	EIy.resize(m_numElems);
	EI1.resize(m_numElems);
	EI2.resize(m_numElems);
	EA.resize(m_numElems);
	RhoA.resize(m_numElems);
	GJ.resize(m_numElems);
	StructuralTwist.resize(m_numElems);
	ShellThickness.resize(m_numElems);
	SparLocation.resize(m_numElems);
	SparThickness.resize(m_numElems);
	SparAngle.resize(m_numElems);



	for (int i=0;i<m_numElems;i++)
	{
		SparLocation[i] = pStructure->SparLocation[i];
		SparThickness[i] = pStructure->SparThickness[i];
		SparAngle[i] = pStructure->SparAngle[i];
		ShellThickness[i] = pStructure->ShellThickness[i];

	}

	StructType = pStructure->StructType;
	ShellEMod = pStructure->ShellEMod;
	SparEMod = pStructure->SparEMod;
	SparRho = pStructure->SparRho;
	ShellRho = pStructure->ShellRho;
	m_rotor = pStructure->m_rotor;
	m_numFoilPoints = pStructure->m_numFoilPoints;
    shellColor = pStructure->shellColor;
    sparColor = pStructure->sparColor;
    Omega = pStructure->Omega;

	ReadSectionCoordinates();

}



bool BladeStructure::RunModalTest()
{
    QVector<StructElem> SectionStructures(m_numElems);
    int MaxDOF;

    // *******************Start Paralell Calculation Section**********************
    #pragma omp parallel default (none) shared (SectionStructures)
    {
    #pragma omp for

        for (int i = 0; i < m_numElems; i++)
        {

            if (! *m_cancelCalculation)
            {

            if (! *m_cancelCalculation) emit updateProgress();


            //Create a node for the structural properties to be attached to.
            Node TempNode;
            TempNode.Coords(0)=PAxisX.at(i);
            TempNode.Coords(1)=-PAxisZ.at(i);
            TempNode.Coords(2)=PAxisY.at(i);
            TempNode.IDnumber= i;

            for(int j=0; j<6;j++)//Give each node its identifier.
            {
                //Create and identifyer with the DOF
                TempNode.DOFNums.push_back(j+i*6);
            }

            std::vector<double> xCoords, yCoords;

            for(int j=0; j<m_numFoilPoints-2;j++)//push coordinates into arrays
            {
                xCoords.push_back(xFoilCoords[i][j]);
                yCoords.push_back(yFoilCoords[i][j]);

            }

            if (! *m_cancelCalculation)
            {
            StructElem TempStruct(TempNode,xCoords,yCoords,-ChordTwist.at(i)*3.14/180.0,ShellEMod,ChordLengths.at(i),ShellRho,LocalXOffset.at(i),LocalYOffset.at(i)); // Create a Foil
            TempStruct.CalcOuterRef();

            if (! *m_cancelCalculation) emit updateProgress();

            //With Coordinates X,Y, Elastic Mod, Chord Length, Density
            if(StructType < 2 && ! *m_cancelCalculation)
            {
                TempStruct.CreateInner(ShellThickness.at(i)); // Make it hollow
                if (TempStruct.Hollow) TempStruct.CalcInnerRef();
            }

            if (! *m_cancelCalculation) emit updateProgress();


            if(StructType == 0 && SparThickness.at(i) > 0.00001 && ! *m_cancelCalculation)
            {
                 if (TempStruct.Hollow) TempStruct.CreateSpar(SparLocation.at(i),SparThickness.at(i),SparAngle.at(i),SparEMod,SparRho);//Insert a spar
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
    TaperedElem TempElem(SectionStructures.at(i),SectionStructures.at(i+1),Omega);
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

    //-----------------------Solve the eigenvalue problem-----------------------------
    ModalEquations.GetEigSoln();

    //------------------Replace the previously deleted lines with zeros---------------
    ModalEquations.ReplaceRigidDOFs(FixedDOF);

    ModalEquations.FitPolynomials();

    //---------------stroring of results in the objects arrays fo later use and visualization purposes-----------

    for (int i=0;i<m_numElems;i++)
    {
        EIx[i]=SectionStructures.at(i).EIx;
        EIy[i]=SectionStructures.at(i).EIy;
        EI1[i]=SectionStructures.at(i).EI1;
        EI2[i]=SectionStructures.at(i).EI2;
        EA[i]=SectionStructures.at(i).EA;
        RhoA[i]=SectionStructures.at(i).RhoA;
        GJ[i]=SectionStructures.at(i).GJ;
        StructuralTwist[i]=SectionStructures.at(i).StructuralTwist/3.14*180.0;
    }

    addQVecToResults();

    QVector <double> coeffs;
    for (int i = 0; i < (int)ModalEquations.ModeContainer.size(); i++)
    {
        Mode TempMode = ModalEquations.ModeContainer.at(i);

        AscendingFrequencies.append(TempMode.Frequency/(2.0*3.14));


            if (int(TempMode.ModeType) == 0)
            {
                FlapwiseFrequencies.append(TempMode.Frequency/(2.0*3.14));

                coeffs.clear();
                for(int k=0; k<TempMode.Polynomial.rows();k++)
                {
                    if (k==0) coeffs.append(0); // placeholder for x^1 due to compatibility w older project files
                    coeffs.append(TempMode.Polynomial(k,0));
                }
                FlapwiseCoefficients.append(coeffs);
            }
            else if (int(TempMode.ModeType) == 1)
            {
                EdgewiseFrequencies.append(TempMode.Frequency/(2.0*3.14));

                coeffs.clear();
                for(int k=0; k<TempMode.Polynomial.rows();k++)
                {
                    if (k==0) coeffs.append(0); // placeholder for x^1 due to compatibility w older project files
                    coeffs.append(TempMode.Polynomial(k,0));
                }
                EdgewiseCoefficients.append(coeffs);
            }
            else if (int(TempMode.ModeType) == 2)
            {
                TorsionalFrequencies.append(TempMode.Frequency/(2.0*3.14));

                coeffs.clear();
                for(int k=0; k<TempMode.Polynomial.rows();k++)
                {
                    if (k==0) coeffs.append(0); // placeholder for x^1 due to compatibility w older project files
                    coeffs.append(TempMode.Polynomial(k,0));
                }
                TorsionalCoefficients.append(coeffs);

            }
            else if (int(TempMode.ModeType) == 3)
            {
                RadialFrequencies.append(TempMode.Frequency/(2.0*3.14));

                coeffs.clear();
                for(int k=0; k<TempMode.Polynomial.rows();k++)
                {
                    if (k==0) coeffs.append(0); // placeholder for x^1 due to compatibility w older project files
                    coeffs.append(TempMode.Polynomial(k,0));
                }
                RadialCoefficients.append(coeffs);

            }
            else if (int(TempMode.ModeType) == 4)
            {

                UnsortedFrequencies.append(TempMode.Frequency/(2.0*3.14));

                coeffs.clear();
                for(int k=0; k<TempMode.Polynomial.rows();k++)
                {
                    if (k==0) coeffs.append(0); // placeholder for x^1 due to compatibility w older project files
                    coeffs.append(TempMode.Polynomial(k,0));
                }
                UnsortedCoefficients.append(coeffs);
            }

    }


    for (int i = 0; i < (int)ModalEquations.ModeContainer.size(); i++)
    {
        Mode TempMode = ModalEquations.ModeContainer.at(i);
        QVector < QVector < double > > bladecoords;

        for (int j = 0; j < (int)TempMode.ModeShape.size(); j++)
        {
            DeformationVector TempNode = TempMode.ModeShape.at(j);
            QVector <double> coords;
            coords.append(TempNode.Coords(0));
            coords.append(TempNode.Coords(1));
            coords.append(TempNode.Coords(2));
            coords.append(TempNode.zAxisAngle);
            bladecoords.append(coords);
        }


            if (int(TempMode.ModeType) == 0)
            {
            FlapWiseNodeTranslations.append(bladecoords);
            }
            else if (int(TempMode.ModeType) == 1)
            {
            EdgeWiseNodeTranslations.append(bladecoords);
            }
            else if (int(TempMode.ModeType) == 2)
            {
            TorsionalTranslations.append(bladecoords);
            }
            else if (int(TempMode.ModeType) == 3)
            {
            RadialNodeTranslations.append(bladecoords);
            }
            else if (int(TempMode.ModeType) == 4)
            {
            UnsortedNodeTranslations.append(bladecoords);
            }

        bladecoords.clear();
    }

    ////finally the blade mass is computed////
    blademass = 0;
    int steps=50;

    double dist;

    for (int i=0;i<m_numElems-1;i++)
    {
        dist = Radii.at(i+1)-Radii.at(i);
        for (int j=0;j<steps;j++)
        {
            double frac = dist/steps;
            blademass += frac*(RhoA.at(i)*(steps-j)/steps+RhoA.at(i+1)*(j)/steps);
        }
    }

//    qDebug() << EdgeWiseNodeTranslations.size() << EdgeWiseNodeTranslations.at(1).size() << EdgeWiseNodeTranslations.at(1).at(1).size();
//    qDebug() << TorsionalTranslations.size() << TorsionalTranslations.at(1).size() << TorsionalTranslations.at(1).at(1).size();
//    qDebug() << FlapWiseNodeTranslations.size() << FlapWiseNodeTranslations.at(1).size() << FlapWiseNodeTranslations.at(1).at(1).size();
//    qDebug() << RadialNodeTranslations.size() << RadialNodeTranslations.at(1).size() << RadialNodeTranslations.at(1).at(1).size();
//    qDebug() << UnsortedNodeTranslations.size() << UnsortedNodeTranslations.at(1).size() << UnsortedNodeTranslations.at(1).at(1).size();



    QFEMCompleted = true;
    return true;
}

void BladeStructure::FillVariableList(BladeStructureLoading *loading){

    m_availableVariables.clear();
    m_availableVariables.append("Blade Radius [m]");
    m_availableVariables.append("EI_z (Flapwise Stiffness z-Axis [Nm^2])");
    m_availableVariables.append("EI_x (Edgewise Stiffness x-Axis [Nm^2])");
    m_availableVariables.append("EI_1 (Flapwise Stiffness Principal Axis [Nm^2])");
    m_availableVariables.append("EI_2 (Edgewise Stiffness Principal Axis [Nm^2])");
    m_availableVariables.append("EA (Longitudinal Stiffness y-Axis [N])");
    m_availableVariables.append("Rho/A (Density per Area [kg/m^2])");
    m_availableVariables.append("GJ (Torsional Stiffness [Nm^2])");
    m_availableVariables.append("Blade Structural Twist [deg]");
    m_availableVariables.append("Shell Thickness [m]");
    m_availableVariables.append("Spar Location [% chord]");
    m_availableVariables.append("Spar Thickness [m]");
    m_availableVariables.append("Spar Angle [deg]");
    m_availableVariables.append("Blade Chord [m]");
    m_availableVariables.append("Blade Twist [deg]");

    if (loading){
    m_availableVariables.append("Normal Loading at Sections [N]");
    m_availableVariables.append("Tangential Loading at Sections [N]");
    m_availableVariables.append("Normal Loading per Length [N/m]");
    m_availableVariables.append("Tangential Loading per Length [N/m]");
    m_availableVariables.append("Node Translations in Z [m]");
    m_availableVariables.append("Node Translations in X [m]");
    m_availableVariables.append("Max VM Stress at Section [Pa]");
    }

}

void BladeStructure::serialize() {
    StorableObject::serialize();
    ShowAsGraphInterface::serialize();

    g_serializer.readOrWriteStorableObject (&m_rotor);

    g_serializer.readOrWriteDouble (&ShellEMod);
    g_serializer.readOrWriteDouble (&ShellRho);
    g_serializer.readOrWriteDouble (&blademass);
    g_serializer.readOrWriteDouble (&SparEMod);
    g_serializer.readOrWriteDouble (&SparRho);
    g_serializer.readOrWriteDouble (&Omega);

    g_serializer.readOrWriteDoubleArray2D (reinterpret_cast<double*> (xFoilCoords), 100, 200);
    g_serializer.readOrWriteDoubleArray2D (reinterpret_cast<double*> (yFoilCoords), 100, 200);

    g_serializer.readOrWriteInt (&StructType);
    g_serializer.readOrWriteInt (&m_numElems);
    g_serializer.readOrWriteInt (&m_numFoilPoints);

    g_serializer.readOrWriteBool (&QFEMCompleted);
    g_serializer.readOrWriteBool (&AbsoluteShell);
    g_serializer.readOrWriteBool (&AbsoluteSpar);


    g_serializer.readOrWriteColor (&sparColor);
    g_serializer.readOrWriteColor (&shellColor);

    g_serializer.readOrWriteDoubleVector1D (&EIx);
    g_serializer.readOrWriteDoubleVector1D (&EIy);
    g_serializer.readOrWriteDoubleVector1D (&EI1);
    g_serializer.readOrWriteDoubleVector1D (&EI2);
    g_serializer.readOrWriteDoubleVector1D (&EA);
    g_serializer.readOrWriteDoubleVector1D (&RhoA);
    g_serializer.readOrWriteDoubleVector1D (&GJ);
    g_serializer.readOrWriteDoubleVector1D (&StructuralTwist);
    g_serializer.readOrWriteDoubleVector1D (&ShellThickness);
    g_serializer.readOrWriteDoubleVector1D (&SparLocation);
    g_serializer.readOrWriteDoubleVector1D (&SparThickness);
    g_serializer.readOrWriteDoubleVector1D (&SparAngle);
    g_serializer.readOrWriteDoubleVector1D (&FlapwiseFrequencies);
    g_serializer.readOrWriteDoubleVector1D (&EdgewiseFrequencies);
    g_serializer.readOrWriteDoubleVector1D (&TorsionalFrequencies);
    g_serializer.readOrWriteDoubleVector1D (&RadialFrequencies);
    g_serializer.readOrWriteDoubleVector1D (&UnsortedFrequencies);
    g_serializer.readOrWriteDoubleVector1D (&AscendingFrequencies);

    g_serializer.readOrWriteDoubleVector2D (&FlapwiseCoefficients);
    g_serializer.readOrWriteDoubleVector2D (&EdgewiseCoefficients);
    g_serializer.readOrWriteDoubleVector2D (&TorsionalCoefficients);
    g_serializer.readOrWriteDoubleVector2D (&RadialCoefficients);
    g_serializer.readOrWriteDoubleVector2D (&UnsortedCoefficients);

    g_serializer.readOrWriteDoubleVector3D (&FlapWiseNodeTranslations);
    g_serializer.readOrWriteDoubleVector3D (&EdgeWiseNodeTranslations);
    g_serializer.readOrWriteDoubleVector3D (&TorsionalTranslations);
    g_serializer.readOrWriteDoubleVector3D (&RadialNodeTranslations);
    g_serializer.readOrWriteDoubleVector3D (&UnsortedNodeTranslations);

    g_serializer.readOrWriteDoubleVector1D (&ChordLengths);
    g_serializer.readOrWriteDoubleVector1D (&Radii);
    g_serializer.readOrWriteDoubleVector1D (&ChordTwist);
    g_serializer.readOrWriteDoubleVector1D (&LocalXOffset);
    g_serializer.readOrWriteDoubleVector1D (&LocalYOffset);
    g_serializer.readOrWriteDoubleVector1D (&PAxisX);
    g_serializer.readOrWriteDoubleVector1D (&PAxisY);
    g_serializer.readOrWriteDoubleVector1D (&PAxisZ);


    addQVecToResults();

}

void BladeStructure::restorePointers() {
    StorableObject::restorePointers();

	g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&m_rotor));
}

QStringList BladeStructure::prepareMissingObjectMessage() {
	if (g_bladeStructureStore.isEmpty() && g_QFEMModule->m_structure == NULL) {
		QStringList message = CBlade::prepareMissingObjectMessage(false);
		if (message.isEmpty()) {
			if (g_mainFrame->m_iApp == QFEMMODULE && g_mainFrame->m_iView == QFEMSTRUCTVIEW) {
				message = QStringList(">>> Click 'New' to create a new Blade Structure");
			} else {
				message = QStringList(">>> Create a new Blade Structure in the QFEM Module");
			}
		}
		message.prepend("- No Blade Structure in Database");
		return message;
	} else {
		return QStringList();
	}
}

QStringList BladeStructure::getAvailableVariables(NewGraph::GraphType /*graphType*/) {
    return m_availableVariables;
}

void BladeStructure::addQVecToResults(BladeStructureLoading *loading)
{
    QVector<double> AbsShell, AbsSpar, zTranslation, xTranslation, maxVM, normalperlength, tangentialperlength;
    double max;

    for (int i=0; i<SparThickness.size();i++)
    {
        AbsSpar.append(SparThickness.at(i)*ChordLengths.at(i));
        AbsShell.append(ShellThickness.at(i)*ChordLengths.at(i));
    }

    for (int i=0; i<m_results.size(); i++) m_results[i].clear();

    m_results.clear();


    m_results.append(Radii);
    m_results.append(EIx);
    m_results.append(EIy);
    m_results.append(EI1);
    m_results.append(EI2);
    m_results.append(EA);
    m_results.append(RhoA);
    m_results.append(GJ);
    m_results.append(StructuralTwist);
    m_results.append(AbsShell);
    m_results.append(SparLocation);
    m_results.append(AbsSpar);
    m_results.append(SparAngle);
    m_results.append(ChordLengths);
    m_results.append(ChordTwist);


    if (loading){
    for (int i=0;i<loading->nodeTranslations.size();i++){
        zTranslation.append(loading->nodeTranslations.at(i).at(0));
        xTranslation.append(loading->nodeTranslations.at(i).at(1));
        max = 0;
        for (int j=0;j<loading->VMStresses.at(i).size();j++){
        if (max < loading->VMStresses.at(i).at(j)) max = loading->VMStresses.at(i).at(j);
        }
        maxVM.append(max);
    }
    for (int i=0;i<loading->normalLoading.size();i++){
        if (loading->sectionWidth.at(i) > 0){
        normalperlength.append(loading->normalLoading.at(i)/loading->sectionWidth.at(i));
        tangentialperlength.append(loading->tangentialLoading.at(i)/loading->sectionWidth.at(i));
        }
        else{
        normalperlength.append(0);
        tangentialperlength.append(0);
        }
    }



    m_results.append(loading->normalLoading);
    m_results.append(loading->tangentialLoading);
    m_results.append(normalperlength);
    m_results.append(tangentialperlength);
    m_results.append(zTranslation);
    m_results.append(xTranslation);
    m_results.append(maxVM);

    }
}
