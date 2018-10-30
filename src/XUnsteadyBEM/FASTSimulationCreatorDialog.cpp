#include "FASTSimulationCreatorDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QTabWidget>
#include <QVBoxLayout>
#include "../StoreAssociatedComboBox.h"
#include "WindField.h"
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include "../XBEM/Blade.h"
#include "../XBEM/TData.h"
#include "../QFem/BladeStructure.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "../XBEM/360Polar.h"


FASTSimulationCreatorDialog::FASTSimulationCreatorDialog(FASTSimulation *presetSimulation, FASTModule *module) {
	m_editedFASTSimulation = presetSimulation;
	m_module = module;
	m_sectionTableWidget = NULL;
	
	setWindowTitle(tr("FAST Simulation"));
	
	QVBoxLayout *dialogVBox = new QVBoxLayout ();
	setLayout(dialogVBox);
		QTabWidget *tabWidget = new QTabWidget ();
		connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(createSectionTable(int)));
		dialogVBox->addWidget(tabWidget);
		QHBoxLayout *hBox = new QHBoxLayout ();
		dialogVBox->addLayout(hBox);
			hBox->addStretch();
			m_cancelButton = new QPushButton (tr("Cancel"));
			connect(m_cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));
			hBox->addWidget (m_cancelButton);
			m_createButton = new QPushButton (tr("Create"));
			m_createButton->setDefault(true);
			connect(m_createButton, SIGNAL(clicked(bool)), this, SLOT(onCreateButtonClicked()));
			hBox->addWidget (m_createButton);
		
	/* the parameter tab */
	QWidget *widget = new QWidget ();
	tabWidget->addTab(widget, "Parameters");
		hBox = new QHBoxLayout ();
		widget->setLayout(hBox);
			QVBoxLayout *vBox = new QVBoxLayout;
			hBox->addLayout(vBox);
                QGroupBox *groupBox = new QGroupBox (tr("FAST Simulation Parameters"));
				vBox->addWidget(groupBox);
					QGridLayout *grid = new QGridLayout ();
					groupBox->setLayout(grid);
						int gridRowCount = 0;
						QLabel *label = new QLabel (tr("Name of Simulation: "));
						grid->addWidget(label, gridRowCount, 0);
						m_nameEdit = new QLineEdit ();
						m_nameEdit->setMinimumWidth(175);
						grid->addWidget(m_nameEdit, gridRowCount++, 1);
                        label = new QLabel (tr("Rotor: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_rotorComboBox = new RotorComboBox (&g_rotorStore, false);
                        m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                        connect(m_rotorComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldOrRotorChanged()));
                        connect(m_rotorComboBox, SIGNAL(valueChanged(int)), this, SLOT(onRotorChanged()));
                        grid->addWidget(m_rotorComboBox, gridRowCount++, 1);
                        label = new QLabel (tr("Hub Height [m]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_hubHeightEdit = new NumberEdit ();
                        m_hubHeightEdit->setToolTip(tr("Rotor hub height [m]"));
                        grid->addWidget(m_hubHeightEdit, gridRowCount++, 1);
                        label = new QLabel (tr("Number of Blades: "));
                        grid->addWidget (label, gridRowCount, 0);
                        QHBoxLayout *miniHBox = new QHBoxLayout ();
                        grid->addLayout(miniHBox, gridRowCount++, 1);
                            miniHBox->addStretch();
                            m_twoOrThreeBladesGroup = new QButtonGroup(miniHBox);
                            QRadioButton *radioButton = new QRadioButton ("2");
                            m_twoOrThreeBladesGroup->addButton(radioButton, 0);
                            connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(onTwoOrThreeBladesChanged()));
                            miniHBox->addWidget(radioButton);
                            radioButton = new QRadioButton ("3");
                            m_twoOrThreeBladesGroup->addButton(radioButton, 1);
                            miniHBox->addWidget(radioButton);
						label = new QLabel (tr("Total run time [s]: "));
						grid->addWidget(label, gridRowCount, 0);
						m_totalRunTimeEdit = new NumberEdit ();
						grid->addWidget(m_totalRunTimeEdit, gridRowCount++, 1);
                        label = new QLabel (tr("FAST Time step [s]: "));
						grid->addWidget(label, gridRowCount, 0);
						m_timeStepEdit = new NumberEdit ();
						grid->addWidget(m_timeStepEdit, gridRowCount++, 1);
						label = new QLabel (tr("Rotor Speed [rpm]: "));
						grid->addWidget(label, gridRowCount, 0);
						m_rotorSpeedEdit = new NumberEdit ();
						m_rotorSpeedEdit->setToolTip(tr("Initial or fixed rotor speed [rpm]"));
						grid->addWidget(m_rotorSpeedEdit, gridRowCount++, 1);
						label = new QLabel (tr("Nacelle Yaw [deg]: "));
						grid->addWidget(label, gridRowCount, 0);
						m_nacelleYawEdit = new NumberEdit ();
						m_nacelleYawEdit->setToolTip(tr("Initial or fixed nacelle-yaw angle [degrees]"));
						grid->addWidget(m_nacelleYawEdit, gridRowCount++, 1);
				groupBox = new QGroupBox ("Environment");
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					groupBox->setLayout(grid);
						gridRowCount = 0;
						label = new QLabel (tr("Gravity [m/s^2]: "));
						grid->addWidget(label, gridRowCount, 0);
						m_gravityEdit = new NumberEdit ();
						grid->addWidget(m_gravityEdit, gridRowCount++, 1);
						label = new QLabel ("AirDens [k/m^3]: ");
						grid->addWidget(label, gridRowCount, 0);
						m_airDensEdit = new NumberEdit ();
						grid->addWidget(m_airDensEdit, gridRowCount++, 1);
						label = new QLabel ("KinVisc [m^2/s]: ");
						grid->addWidget(label, gridRowCount, 0);
						m_kinViscEdit = new NumberEdit (NumberEdit::Scientific);
						grid->addWidget(m_kinViscEdit, gridRowCount++, 1);


                        groupBox = new QGroupBox ("Windfield");
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;


                                label = new QLabel (tr("Windfield Source: "));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_windfieldLocationGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("QBlade Database");
                                    m_windfieldLocationGroup->addButton(radioButton, 0);
                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(onWindfieldLocationChanged()));
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("From File");
                                    m_windfieldLocationGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                    label = new QLabel (tr("From Database: "));
                    grid->addWidget(label, gridRowCount, 0);
                    m_windFieldComboBox = new WindFieldComboBox (&g_windFieldStore, false);
                    m_windFieldComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                    connect(m_windFieldComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldOrRotorChanged()));
                    grid->addWidget(m_windFieldComboBox, gridRowCount++, 1);


                    label = new QLabel (tr("From File: "));
                    grid->addWidget(label, gridRowCount, 0);
                    m_openWindfieldButton = new QPushButton("");
                    connect(m_openWindfieldButton, SIGNAL(pressed()), this, SLOT(onOpenWindfieldFile()));
                    grid->addWidget(m_openWindfieldButton, gridRowCount++, 1);
                    vBox->addStretch();



						
			vBox = new QVBoxLayout;
			hBox->addLayout(vBox);
                groupBox = new QGroupBox ("Aerodynamic Parameters");
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					groupBox->setLayout(grid);
						gridRowCount = 0;
                        label = new QLabel (tr("Aero Time step [s]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_aeroTimeStepEdit = new NumberEdit ();
                        grid->addWidget(m_aeroTimeStepEdit, gridRowCount++, 1);
						label = new QLabel ("StallMod: ");
						grid->addWidget(label, gridRowCount, 0);
						m_stallModComboBox = new QComboBox ();
						m_stallModComboBox->addItem("STEADY", FASTSimulation::STEADY);
                        m_stallModComboBox->addItem("BEDDOES", FASTSimulation::BEDDOES);
						grid->addWidget(m_stallModComboBox, gridRowCount++, 1);
						label = new QLabel ("UseCm: ");
						grid->addWidget(label, gridRowCount, 0);
						m_useCmComboBox = new QComboBox();
//						m_useCmComboBox->addItem("USE_CM", FASTSimulation::USE_CM);
						m_useCmComboBox->addItem("NO_CM", FASTSimulation::NO_CM);
						grid->addWidget(m_useCmComboBox, gridRowCount++, 1);
						label = new QLabel ("InfModel: ");
						grid->addWidget(label, gridRowCount, 0);
						m_infModelComboBox = new QComboBox ();
                        m_infModelComboBox->addItem("EQUIL", FASTSimulation::EQUIL);
						m_infModelComboBox->addItem("DYNIN", FASTSimulation::DYNIN);
						grid->addWidget(m_infModelComboBox, gridRowCount++, 1);
						label = new QLabel ("IndModel: ");
						grid->addWidget(label, gridRowCount, 0);
						m_indModelComboBox = new QComboBox ();
                        m_indModelComboBox->addItem("SWIRL", FASTSimulation::SWIRL);
                        m_indModelComboBox->addItem("WAKE", FASTSimulation::WAKE);
						m_indModelComboBox->addItem("NONE", FASTSimulation::NONE);
						grid->addWidget(m_indModelComboBox, gridRowCount++, 1);
						label = new QLabel ("TLModel: ");
						grid->addWidget(label, gridRowCount, 0);
						m_tlModelComboBox = new QComboBox ();
						m_tlModelComboBox->addItem("PRANDTL", FASTSimulation::PRANDTL);
						m_tlModelComboBox->addItem("GTECH", FASTSimulation::GTECH);
						m_tlModelComboBox->addItem("NONE", FASTSimulation::NONE);
						grid->addWidget(m_tlModelComboBox, gridRowCount++, 1);
						label = new QLabel ("HLModel: ");
						grid->addWidget(label, gridRowCount, 0);
						m_hlModelComboBox = new QComboBox ();
						m_hlModelComboBox->addItem("PRANDTL", FASTSimulation::PRANDTL);
						m_hlModelComboBox->addItem("NONE", FASTSimulation::NONE);
						grid->addWidget(m_hlModelComboBox, gridRowCount++, 1);
				groupBox = new QGroupBox ("Blade Structure");
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					groupBox->setLayout(grid);
						label = new QLabel (tr("Blade Structure: "));
						grid->addWidget(label, gridRowCount, 0);
						m_bladeStructureComboBox = new BladeStructureComboBox (&g_bladeStructureStore, false);
						m_bladeStructureComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
						m_bladeStructureComboBox->setParentBox(m_rotorComboBox);
						grid->addWidget(m_bladeStructureComboBox, gridRowCount++, 1);
						m_flapDof1CheckBox = new QCheckBox (tr("FlapDOF 1"));
						connect(m_flapDof1CheckBox, SIGNAL(stateChanged(int)), this, SLOT(onDOFChanged()));
						grid->addWidget(m_flapDof1CheckBox, gridRowCount++, 1);
						m_flapDof2CheckBox = new QCheckBox (tr("FlapDOF 2"));
						grid->addWidget(m_flapDof2CheckBox, gridRowCount++, 1);
						m_edgeDofCheckBox = new QCheckBox (tr("EdgeDOF"));
						grid->addWidget(m_edgeDofCheckBox, gridRowCount++, 1);
				vBox->addStretch();
				m_errorLabel = new QLabel ("");
				vBox->addWidget(m_errorLabel);

	/* the output section tab */
	widget = new QWidget ();
	tabWidget->addTab(widget, "Output Sections");
		grid = new QGridLayout ();
		widget->setLayout(grid);
			gridRowCount = 0;
			label = new QLabel (tr("Choose blade sections where you want to put virtual strain gages or produce AeroDyn output.<br>"
								   "Please notice that FAST uses another represantation of the blade than QBlade. Because FAST uses the "
								   "midpoint of a blade section as calculation node there is one node less."));
			label->setWordWrap(true);
			grid->addWidget(label, gridRowCount, 0);
			m_sectionTableScrollArea = new QScrollArea();
			grid->addWidget(m_sectionTableScrollArea);
				/* content is dynamically created in createSectionTable() */

	widget = new QWidget ();
	tabWidget->addTab(widget, "Output Parameters");
		vBox = new QVBoxLayout ();
		widget->setLayout(vBox);
			label = new QLabel (tr("Choose which parameters the FAST output should contain."));
			label->setWordWrap(true);
			vBox->addWidget(label);
				groupBox = new QGroupBox (tr("General"));
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					grid->setColumnStretch(2, 1);
					groupBox->setLayout(grid);
						label = new QLabel (tr("TStart [s]: "));
						grid->addWidget(label, 0, 0);
						m_tStartEdit = new NumberEdit ();
						grid->addWidget(m_tStartEdit, 0, 1);
						label = new QLabel (tr("DecFact: "));
						grid->addWidget(label, 1, 0);
						m_decFactEdit = new NumberEdit (NumberEdit::Standard, 0);
						m_decFactEdit->setMinimum(1);
						grid->addWidget(m_decFactEdit, 1, 1);
				groupBox = new QGroupBox (tr("Rotor Parameters"));
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					groupBox->setLayout(grid);
						QPushButton *button;
						int perColumn = 5;
						int row = 0, column = 0;
						for (int i = 0; i < FASTSimulation::numberOfFastRotorOutputParameters; ++i) {
							button = new QPushButton(FASTSimulation::fastRotorOutputParameters[i]);
                            button->setToolTip(FASTSimulation::fastRotorOutputParametersTooltips[i]);
							button->setCheckable(true);
							button->setAutoDefault(false);
							m_rotorParameterButtonList.append(button);
							grid->addWidget(button, row, column);
							
							column++;
							if (column == perColumn) {
								column -= perColumn;
								row++;
							}
						}			
				groupBox = new QGroupBox (tr("Blade Parameters"));
				vBox->addWidget(groupBox);
					grid = new QGridLayout ();
					groupBox->setLayout(grid);
						hBox = new QHBoxLayout ();
						grid->addLayout(hBox, 0, 0, 1, perColumn);
							label = new QLabel (tr("For which blades: "));
							hBox->addWidget(label);
							m_blade1Output = new QCheckBox (tr("Blade 1"));
							hBox->addWidget(m_blade1Output);
							m_blade2Output = new QCheckBox (tr("Blade 2"));
							hBox->addWidget(m_blade2Output);
							m_blade3Output = new QCheckBox (tr("Blade 3"));
							hBox->addWidget(m_blade3Output);
							hBox->addStretch();
						row = 1, column = 0;
						for (int i = 0; i < FASTSimulation::numberOfFastBladeOutputParameters; ++i) {
							button = new QPushButton(FASTSimulation::fastBladeOutputParameters[i]);
                            button->setToolTip(FASTSimulation::fastBladeOutputParametersTooltips[i]);
							button->setCheckable(true);
							button->setAutoDefault(false);
							m_bladeParameterButtonList.append(button);
							grid->addWidget(button, row, column);
							
							column++;
							if (column == perColumn) {
								column -= perColumn;
								row++;
							}
						}
				vBox->addStretch();

	initView();
}

void FASTSimulationCreatorDialog::initView() {
	if (m_editedFASTSimulation) {  // load parameters
		m_nameEdit->setText(m_editedFASTSimulation->getName());
		m_windFieldComboBox->setCurrentObject(m_editedFASTSimulation->getUsedWindField());
		m_totalRunTimeEdit->setValue(m_editedFASTSimulation->getTotalRunTime());
		m_timeStepEdit->setValue(m_editedFASTSimulation->getTimeStep());
        m_aeroTimeStepEdit->setValue(m_editedFASTSimulation->getAeroTimeStep());
		m_rotorComboBox->setCurrentObject(m_editedFASTSimulation->getUsedRotor());
		m_twoOrThreeBladesGroup->button((m_editedFASTSimulation->getNumberOfBlades() == 2 ? 0 : 1))->setChecked(true);
		m_rotorSpeedEdit->setValue(m_editedFASTSimulation->getRotorSpeed());
		m_nacelleYawEdit->setValue(m_editedFASTSimulation->getNacelleYaw());
		m_gravityEdit->setValue(m_editedFASTSimulation->getGravity());
		m_airDensEdit->setValue(m_editedFASTSimulation->getAirDens());
		m_kinViscEdit->setValue(m_editedFASTSimulation->getKinVisc());
		m_stallModComboBox->setCurrentIndex(m_stallModComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getStallMod()]));
		m_useCmComboBox->setCurrentIndex(m_useCmComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getUseCm()]));
		m_infModelComboBox->setCurrentIndex(m_infModelComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getInfModel()]));
		m_indModelComboBox->setCurrentIndex(m_indModelComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getIndModel()]));
		m_tlModelComboBox->setCurrentIndex(m_tlModelComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getTlModel()]));
		m_hlModelComboBox->setCurrentIndex(m_hlModelComboBox->findText(FASTSimulation::valueString[m_editedFASTSimulation->getHlModel()]));
		m_bladeStructureComboBox->setCurrentObject(m_editedFASTSimulation->getUsedBladeStructure());
		m_flapDof1CheckBox->setChecked(m_editedFASTSimulation->getUseFlapDof1());
		m_flapDof2CheckBox->setChecked(m_editedFASTSimulation->getUseFlapDof2());
        m_edgeDofCheckBox->setChecked(m_editedFASTSimulation->getUseEdgeDof());
        if (m_editedFASTSimulation->getHubHeight() == 0) m_hubHeightEdit->setValue(m_windFieldComboBox->currentObject()->getHubheight());
        else m_hubHeightEdit->setValue(m_editedFASTSimulation->getHubHeight());


        m_windfieldLocationGroup->button((m_editedFASTSimulation->getIsWindFromQBlade() ? 0 : 1))->setChecked(true);
        m_openWindfieldButton->setText(m_editedFASTSimulation->getWindFieldPath());
        if (m_windfieldLocationGroup->button(0)->isChecked()){
            m_openWindfieldButton->setText("Open File");
            m_WindfieldPathString = "";
        }
        else{
            m_openWindfieldButton->setText(m_editedFASTSimulation->getWindFieldPath());
            m_WindfieldPathString = m_editedFASTSimulation->getWindFieldPath();
            }
		
		createSectionTable(1);
		QVector<int> strainGages = m_editedFASTSimulation->getStrainGages();
		for (int i = 0; i < strainGages.size(); ++i) {
			m_strainGageCheckBoxList[strainGages[i]-1]->setChecked(true);
		}
		QBitArray aeroDynOutput = m_editedFASTSimulation->getAeroDynOutput();
		for (int i = 0; i < aeroDynOutput.size(); ++i) {
			m_aeroDynCheckBoxList[i]->setChecked(aeroDynOutput[i]);
		}
		m_tStartEdit->setValue(m_editedFASTSimulation->getTStart());
		m_decFactEdit->setValue(m_editedFASTSimulation->getDecFact());
		QVector<int> bladeOutput = m_editedFASTSimulation->getBladeOutput();
		for (int i = 0; i < bladeOutput.size(); ++i) {
			switch (bladeOutput[i]) {
				case 1: m_blade1Output->setChecked(true); break;
				case 2: m_blade2Output->setChecked(true); break;
				case 3: m_blade3Output->setChecked(true); break;
			}
		}
		QBitArray rotorParameters = m_editedFASTSimulation->getRotorParameters();
		for (int i = 0; i < rotorParameters.size(); ++i) {
			m_rotorParameterButtonList[i]->setChecked(rotorParameters[i]);
		}
		QBitArray bladeParameters = m_editedFASTSimulation->getBladeParameters();
		for (int i = 0; i < bladeParameters.size(); ++i) {
			m_bladeParameterButtonList[i]->setChecked(bladeParameters[i]);
		}	

	} else {  // set default parameters
        m_hubHeightEdit->setValue(m_windFieldComboBox->currentObject()->getHubheight());
        m_WindfieldPathString = "";
        m_windfieldLocationGroup->button(0)->setChecked(true);
        m_openWindfieldButton->setText("Open File");
        m_nameEdit->setText("FAST Simulation");
		m_twoOrThreeBladesGroup->button(1)->setChecked(true);
        m_totalRunTimeEdit->setValue(10);
		m_timeStepEdit->setValue(0.001);
        m_aeroTimeStepEdit->setValue(0.001);
        m_rotorSpeedEdit->setValue(12);
		m_gravityEdit->setValue(9.81);
		m_kinViscEdit->setValue(1.4661e-5);
		m_airDensEdit->setValue(1.225);
		m_flapDof1CheckBox->setChecked(true);
		m_flapDof2CheckBox->setChecked(true);
		m_edgeDofCheckBox->setChecked(true);
		m_decFactEdit->setValue(1);
        m_blade1Output->setChecked(true);
	}
	
	onWindFieldOrRotorChanged();
	onDOFChanged();
}

void FASTSimulationCreatorDialog::onWindFieldOrRotorChanged() {
	WindField *windField = m_windFieldComboBox->currentObject();
    CBlade *blade = m_rotorComboBox->currentObject();

	if (windField) {  // can be NULL during the call from initView()
		CBlade *rotor = m_rotorComboBox->currentObject();
        if (rotor && windField->getRotorRadius() < rotor->getRotorRadius() && !m_windfieldLocationGroup->button(1)->isChecked()) {
            m_errorLabel->setText(tr("<font color='red'>Windfield too small for Rotor!</font>"));
        }
        else {
			m_errorLabel->setText("");
		}

	}
    m_hubHeightEdit->setMinimum(blade->getRotorRadius()*1.1);
}

void FASTSimulationCreatorDialog::onWindfieldLocationChanged() {
    if (m_windfieldLocationGroup->button(0)->isChecked()){
        m_windFieldComboBox->setEnabled(true);
        m_openWindfieldButton->setEnabled(false);
    }
    else{
        m_windFieldComboBox->setEnabled(false);
        m_openWindfieldButton->setEnabled(true);
    }
}

void FASTSimulationCreatorDialog::onOpenWindfieldFile(){
    m_WindfieldPathString = QFileDialog::getOpenFileName(g_mainFrame, tr("Open Windfield"),
                                            g_mainFrame->m_LastDirName,
                                            tr("Windfield File File (*.wnd *.bts)"));

    if(!m_WindfieldPathString.length()) return;

    m_openWindfieldButton->setText(m_WindfieldPathString);

    int pos;
    pos = m_WindfieldPathString.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_LastDirName = m_WindfieldPathString.left(pos);

}

void FASTSimulationCreatorDialog::onRotorChanged() {
	if (m_sectionTableWidget) {  // delete the old widget with grid in section tab
		m_strainGageCheckBoxList.clear();
		m_aeroDynCheckBoxList.clear();
		delete m_sectionTableWidget;
		m_sectionTableWidget = NULL;
	}
}

void FASTSimulationCreatorDialog::onTwoOrThreeBladesChanged() {
	m_blade3Output->setEnabled(m_twoOrThreeBladesGroup->button(1)->isChecked());
}

void FASTSimulationCreatorDialog::onDOFChanged() {
	m_flapDof2CheckBox->setEnabled(m_flapDof1CheckBox->isChecked());
}

void FASTSimulationCreatorDialog::onCreateButtonClicked() {
	/* check first, if all requirements are fullfilled */
	QString errorMessage ("");
	if (m_nameEdit->text() == "") {
		errorMessage.append(tr("\n - No name given"));
	}
	if (m_windFieldComboBox->currentIndex() == -1) {
		errorMessage.append(tr("\n - No windfield selected"));
	} else if (m_errorLabel->text() != "") {
		errorMessage.append(tr("\n - Windfield is too small"));		
	}
	if (m_rotorComboBox->currentIndex() == -1) {
		errorMessage.append(tr("\n - No rotor selected"));
	}
	if (m_bladeStructureComboBox->currentIndex() == -1) {
		errorMessage.append((tr("\n - No Structur Model selected")));
	}
    if (m_WindfieldPathString == "" && m_windfieldLocationGroup->button(1)->isChecked()){
        errorMessage.append((tr("\n - No Windfield File selected")));
    }
	if (errorMessage != "") {
		QMessageBox::critical(this, tr("Create Simulation"), QString(tr("The following error(s) occured:\n")																+ errorMessage), QMessageBox::Ok);
		return;
	}


	/* prepare some parameters */
	QVector<int> strainGages, bladeOutput;
	QBitArray aeroDynOutput (m_rotorComboBox->currentObject()->getNumberOfNodes()-1);
	QBitArray rotorParameters (m_rotorParameterButtonList.size());
	QBitArray bladeParameters (m_bladeParameterButtonList.size());
	for (int i = 0; i < m_strainGageCheckBoxList.size(); ++i) {
		if (m_strainGageCheckBoxList[i]->isChecked()) {
			strainGages.append(i+1);  // FAST starts to count at 1
		}
		aeroDynOutput[i] = m_aeroDynCheckBoxList[i]->isChecked();
	}
	if (m_blade1Output->isChecked())
		bladeOutput.append(1);
	if (m_blade2Output->isChecked())
		bladeOutput.append(2);
	if (m_blade3Output->isChecked() && m_blade3Output->isEnabled())
		bladeOutput.append(3);
	for (int i = 0; i < m_rotorParameterButtonList.size(); ++i) {
		rotorParameters[i] = m_rotorParameterButtonList[i]->isChecked();
	}
	for (int i = 0; i < m_bladeParameterButtonList.size(); ++i) {
		bladeParameters[i] = m_bladeParameterButtonList[i]->isChecked();		
	}
	
	typedef FASTSimulation::FASTValue FVal;  // only a shortcut for here
	FASTSimulation *simulation =
			new FASTSimulation (m_nameEdit->text(),
								m_windFieldComboBox->currentObject(),
								m_totalRunTimeEdit->getValue(),
								m_timeStepEdit->getValue(),
								m_rotorComboBox->currentObject(),
								(m_twoOrThreeBladesGroup->checkedId() == 0 ? 2 : 3),
								m_rotorSpeedEdit->getValue(),
								m_nacelleYawEdit->getValue(),
								m_gravityEdit->getValue(),
								m_airDensEdit->getValue(),
								m_kinViscEdit->getValue(),
								static_cast<FVal> (m_stallModComboBox->itemData(m_stallModComboBox->currentIndex()).toInt()),
								static_cast<FVal> (m_useCmComboBox->itemData(m_useCmComboBox->currentIndex()).toInt()),
								static_cast<FVal> (m_infModelComboBox->itemData(m_infModelComboBox->currentIndex()).toInt()),
								static_cast<FVal> (m_indModelComboBox->itemData(m_indModelComboBox->currentIndex()).toInt()),
								static_cast<FVal> (m_tlModelComboBox->itemData(m_tlModelComboBox->currentIndex()).toInt()),
								static_cast<FVal> (m_hlModelComboBox->itemData(m_hlModelComboBox->currentIndex()).toInt()),
								m_bladeStructureComboBox->currentObject(),
								m_flapDof1CheckBox->isChecked(),
								(m_flapDof1CheckBox->isChecked() && m_flapDof2CheckBox->isChecked()),
								m_edgeDofCheckBox->isChecked(),
								&strainGages,
							    &aeroDynOutput,
								m_tStartEdit->getValue(),
								m_decFactEdit->getValue(),
							    &rotorParameters,
							    &bladeOutput,
                                &bladeParameters,
                                m_aeroTimeStepEdit->getValue(),
                                m_WindfieldPathString,
                                m_windfieldLocationGroup->button(0)->isChecked(),
                                m_hubHeightEdit->getValue());
	if (g_FASTSimulationStore.add(simulation)) {
		m_module->setShownFASTSimulation(simulation);
		accept();  // leave dialog only if adding was successful
	}
}

void FASTSimulationCreatorDialog::onCancelButtonClicked() {
	reject();
}

void FASTSimulationCreatorDialog::createSectionTable(int newTabIndex) {
	m_numberOfStrainGages = 0;
	
	if (newTabIndex == 1 && ! m_sectionTableWidget && m_rotorComboBox->currentObject()) {
		m_sectionTableWidget = new QWidget();
		m_sectionTableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);	
		m_sectionTableScrollArea->setWidget(m_sectionTableWidget);
			QGridLayout *grid = new QGridLayout ();
			grid->setSizeConstraint(QLayout::SetMinimumSize);
			m_sectionTableWidget->setLayout(grid);
				QLabel *label = new QLabel("<center>Strain<br>Gage<br>(max. 5)</center>");
				grid->addWidget(label, 0, 0, 1, 1);
				label = new QLabel("<center>AeroDyn<br>Output<br>(any)</center>");
				grid->addWidget(label, 0, 1, 1, 1);
				QPushButton *button = new QPushButton (tr("set all"));
//				button->setFixedWidth(48);
				button->setCheckable(true);
				connect(button, SIGNAL(toggled(bool)), this, SLOT(onSetAllClicked(bool)));
				grid->addWidget(button, 1, 1, 1, 1, Qt::AlignCenter);
				CBlade *rotor = m_rotorComboBox->currentObject();
				QCheckBox *checkBox;
				QFont font ("Monospace");
				int i;
				for (i = 0; i < rotor->getNumberOfNodes()-1; ++i) {
					label = new QLabel (QString("%1 - %2m - %3").arg(i+1, 2).arg(rotor->getFASTRadiusAt(i), 6, 'f', 2).arg(rotor->get360PolarAt(i)->getName()));
					label->setFont(font);
					grid->addWidget(label, i+2, 2, 1, 1);
					checkBox = new QCheckBox ();
					connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(onStrainGageCheckBoxToggled(bool)));
					m_strainGageCheckBoxList.append(checkBox);
					grid->addWidget(checkBox, i+2, 0, 1, 1, Qt::AlignCenter);
					checkBox = new QCheckBox ();
					m_aeroDynCheckBoxList.append(checkBox);
					grid->addWidget(checkBox, i+2, 1, 1, 1, Qt::AlignCenter);
				}	
	}
}

void FASTSimulationCreatorDialog::onStrainGageCheckBoxToggled(bool checked) {
	if (checked) {
		m_numberOfStrainGages++;
		if (m_numberOfStrainGages == 5) {
			for (int i = 0; i < m_strainGageCheckBoxList.size(); ++i) {
				m_strainGageCheckBoxList.at(i)->setEnabled(m_strainGageCheckBoxList.at(i)->isChecked());
			}			
		}
	} else {
		m_numberOfStrainGages--;
		if (m_numberOfStrainGages == 4) {
			for (int i = 0; i < m_strainGageCheckBoxList.size(); ++i) {
				m_strainGageCheckBoxList.at(i)->setEnabled(true);
			}			
		}
	}
}

void FASTSimulationCreatorDialog::onSetAllClicked(bool checked) {
	for (int i = 0; i < m_aeroDynCheckBoxList.size(); ++i) {
		m_aeroDynCheckBoxList.at(i)->setChecked(checked);
	}
}
