#include "QLLTCreatorDialog.h"

#include <QGridLayout>
#include <QLabel>
#include "../XGlobals.h"
#include <QButtonGroup>
#include <QTabWidget>
#include <QVBoxLayout>
#include "../StoreAssociatedComboBox.h"
#include "../XUnsteadyBEM/WindField.h"
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include "../XBEM/Blade.h"
#include <QDebug>
#include <QMessageBox>


QLLTCreatorDialog::QLLTCreatorDialog(QLLTSimulation *presetSimulation, QLLTModule *module) {
    m_editedLLTSimulation = presetSimulation;
    m_module = module;

    int EditWidth = 200;

    setWindowTitle(tr("LLT Simulation"));

    QVBoxLayout *dialogVBox = new QVBoxLayout ();
    setLayout(dialogVBox);
    QTabWidget *tabWidget = new QTabWidget ();
//    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(createSectionTable(int)));
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
    tabWidget->addTab(widget, "Operational Point");
        hBox = new QHBoxLayout ();
        widget->setLayout(hBox);
            QVBoxLayout *vBox = new QVBoxLayout;
            hBox->addLayout(vBox);
                QGroupBox *groupBox = new QGroupBox (tr("Rotor and Windfield"));
                vBox->addWidget(groupBox);
                    QGridLayout *grid = new QGridLayout ();
                    groupBox->setLayout(grid);
                        int gridRowCount = 0;
                        QLabel *label = new QLabel (tr("Name of Simulation: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_nameEdit = new QLineEdit ();
                        m_nameEdit->setMinimumWidth(EditWidth);
                        m_nameEdit->setMaximumWidth(EditWidth);
                        grid->addWidget(m_nameEdit, gridRowCount++, 1);
                        label = new QLabel (tr("Rotor: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_rotorComboBox = new RotorComboBox (&g_rotorStore, false);
                        m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                        m_rotorComboBox->setMaximumWidth(EditWidth);
                        m_rotorComboBox->setMinimumWidth(EditWidth);
                        connect(m_rotorComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldOrRotorChanged()));
                        grid->addWidget(m_rotorComboBox, gridRowCount++, 1);
                        label = new QLabel (tr("Windfield: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_windFieldComboBox = new WindFieldComboBox (&g_windFieldStore, false);
                        m_windFieldComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                        m_windFieldComboBox->setMaximumWidth(EditWidth);
                        m_windFieldComboBox->setMinimumWidth(EditWidth);
                        connect(m_windFieldComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldOrRotorChanged()));
                        grid->addWidget(m_windFieldComboBox, gridRowCount++, 1);
                        groupBox = new QGroupBox (tr("Length of Simulation"));
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;

                        label = new QLabel (tr("Number of timesteps [-]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_numberTimesteps = new NumberEdit ();
                        m_numberTimesteps->setAutomaticPrecision(0);
                        m_numberTimesteps->setToolTip(tr("Number of timesteps that will be simulated"));
                        m_numberTimesteps->setMaximumWidth(EditWidth);
                        m_numberTimesteps->setMinimumWidth(EditWidth);
                        connect(m_numberTimesteps, SIGNAL(editingFinished()), this, SLOT(OnNumberTimestepsChanged()));
                        grid->addWidget(m_numberTimesteps, gridRowCount++, 1);

                        label = new QLabel (tr("Number of rotor revolutions [-]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_numberRevs = new NumberEdit ();
                        m_numberRevs->setToolTip(tr("Number of rotor revolutions that will be simulated"));
                        m_numberRevs->setAutomaticPrecision(2);
                        m_numberRevs->setMaximumWidth(EditWidth);
                        m_numberRevs->setMinimumWidth(EditWidth);
                        connect(m_numberRevs, SIGNAL(editingFinished()), this, SLOT(OnNumberRevolutionsChanged()));
                        grid->addWidget(m_numberRevs, gridRowCount++, 1);


                        label = new QLabel (tr("Total simulated time [s]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_totalRunTimeEdit = new NumberEdit ();
                        m_totalRunTimeEdit->setAutomaticPrecision(2);
                        m_totalRunTimeEdit->setMaximumWidth(EditWidth);
                        m_totalRunTimeEdit->setMinimumWidth(EditWidth);
                        connect(m_totalRunTimeEdit, SIGNAL(editingFinished()), this, SLOT(OnRunTimeChanged()));
                        grid->addWidget(m_totalRunTimeEdit, gridRowCount++, 1);


                        groupBox = new QGroupBox (tr("Time Discretization"));
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;

                        label = new QLabel (tr("Azimuthal step size [deg]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_aziStepEdit = new NumberEdit ();
                        m_aziStepEdit->setAutomaticPrecision(2);
                        m_aziStepEdit->setMaximumWidth(EditWidth);
                        m_aziStepEdit->setMinimumWidth(EditWidth);
                        grid->addWidget(m_aziStepEdit, gridRowCount++, 1);
                        connect(m_aziStepEdit, SIGNAL(editingFinished()), this, SLOT(OnAziChanged()));

                        label = new QLabel (tr("Time step size [s]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_timeStepEdit = new NumberEdit ();
                        m_timeStepEdit->setAutomaticPrecision(5);
                        m_timeStepEdit->setMaximumWidth(EditWidth);
                        m_timeStepEdit->setMinimumWidth(EditWidth);
                        grid->addWidget(m_timeStepEdit, gridRowCount++, 1);
                        connect(m_timeStepEdit, SIGNAL(editingFinished()), this, SLOT(OnTimestepChanged()));


                        groupBox = new QGroupBox (tr("Operational Point"));
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;

                        label = new QLabel (tr("Tip Speed Ratio [-]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_TSREdit = new NumberEdit ();
                        m_TSREdit->setToolTip(tr("Rotor Tip Speed Ratio"));
                        m_TSREdit->setAutomaticPrecision(2);
                        m_TSREdit->setMaximumWidth(EditWidth);
                        m_TSREdit->setMinimumWidth(EditWidth);
                        grid->addWidget(m_TSREdit, gridRowCount++, 1);
                        connect(m_TSREdit, SIGNAL(editingFinished()), this, SLOT(OnTSRChanged()));

                        label = new QLabel (tr("Rotor rotational speed [rpm]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_rotorSpeedEdit = new NumberEdit ();
                        m_rotorSpeedEdit->setToolTip(tr("Initial or fixed rotor speed [rpm]"));
                        m_rotorSpeedEdit->setAutomaticPrecision(2);
                        m_rotorSpeedEdit->setMaximumWidth(EditWidth);
                        m_rotorSpeedEdit->setMinimumWidth(EditWidth);
                        grid->addWidget(m_rotorSpeedEdit, gridRowCount++, 1);
                        connect(m_rotorSpeedEdit, SIGNAL(editingFinished()), this, SLOT(OnRotationalSpeedChanged()));


                        vBox = new QVBoxLayout;
                        hBox->addLayout(vBox);
                            groupBox = new QGroupBox (tr("Inflow Conditions"));
                            vBox->addWidget(groupBox);
                                grid = new QGridLayout ();
                                groupBox->setLayout(grid);
                                    gridRowCount = 0;
                            label = new QLabel (tr("Rotor Yaw [deg]: "));
                            grid->addWidget(label, gridRowCount, 0);
                            m_nacelleYawEdit = new NumberEdit ();
                            m_nacelleYawEdit->setToolTip(tr("Initial or fixed rotor-yaw angle [deg]"));
                            m_nacelleYawEdit->setMaximumWidth(EditWidth);
                            m_nacelleYawEdit->setMinimumWidth(EditWidth);
                            grid->addWidget(m_nacelleYawEdit, gridRowCount++, 1);
                            label = new QLabel (tr("Nacelle Teether [deg]: "));
                            grid->addWidget(label, gridRowCount, 0);
                            m_nacelleTeetherEdit = new NumberEdit ();
                            m_nacelleTeetherEdit->setToolTip(tr("Initial or fixed rotor-teether angle [deg]"));
                            m_nacelleTeetherEdit->setMaximumWidth(EditWidth);
                            m_nacelleTeetherEdit->setMinimumWidth(EditWidth);
                            grid->addWidget(m_nacelleTeetherEdit, gridRowCount++, 1);
                            label = new QLabel (tr("WindInflow Angle [deg]: "));
                            grid->addWidget(label, gridRowCount, 0);
                            m_infloweAngleEdit = new NumberEdit ();
                            m_infloweAngleEdit->setToolTip(tr("The inflow angle of the wind on the rotor [deg]"));
                            m_infloweAngleEdit->setMaximumWidth(EditWidth);
                            m_infloweAngleEdit->setMinimumWidth(EditWidth);
                            grid->addWidget(m_infloweAngleEdit, gridRowCount++, 1);
                            groupBox = new QGroupBox ("Modeling Parameters");
                            vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;

                                label = new QLabel (tr("Wake Rollup:"));
                                grid->addWidget (label, gridRowCount, 0);
                                QHBoxLayout *miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_WakeRollupGroup = new QButtonGroup(miniHBox);
                                    QRadioButton *radioButton = new QRadioButton ("On");
                                    m_WakeRollupGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_WakeRollupGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                label = new QLabel (tr("Include Shed Vortices:"));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_ShedTrailGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_ShedTrailGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_ShedTrailGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                label = new QLabel (tr("Turbulent Wake Convection:"));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_TurbConvectionGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_TurbConvectionGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_TurbConvectionGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                label = new QLabel (tr("Prescribed Wake:"));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_PrescribedGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_PrescribedGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_PrescribedGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                label = new QLabel (tr("Include Ground Effect:"));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_GroundEffectGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_GroundEffectGroup->addButton(radioButton, 0);
                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnIncludeGroundChanged()));
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_GroundEffectGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                    label = new QLabel (tr("Hub Height [m]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_HubHeight = new NumberEdit ();
                                    m_HubHeight->setToolTip(tr("Rotor hub height"));
                                    m_HubHeight->setMaximumWidth(EditWidth);
                                    m_HubHeight->setMinimumWidth(EditWidth);
                                    grid->addWidget(m_HubHeight, gridRowCount++, 1);
                                label = new QLabel (tr("Include Tower Shadow: "));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_TowerShadowGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_TowerShadowGroup->addButton(radioButton, 0);
                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnIncludeTowerChanged()));
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_TowerShadowGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

                                    label = new QLabel (tr("Tower Radius [m]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerRadius = new NumberEdit ();
                                    m_TowerRadius->setToolTip(tr("Radius of the tower"));
                                    m_TowerRadius->setMaximumWidth(EditWidth);
                                    m_TowerRadius->setMinimumWidth(EditWidth);
                                    grid->addWidget(m_TowerRadius, gridRowCount++, 1);
                                    label = new QLabel (tr("Distance from tower to rotor [m]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerDistance = new NumberEdit ();
                                    m_TowerDistance->setToolTip(tr("Distance between tower center and rotor hub"));
                                    m_TowerDistance->setMaximumWidth(EditWidth);
                                    m_TowerDistance->setMinimumWidth(EditWidth);
                                    grid->addWidget(m_TowerDistance, gridRowCount++, 1);
                                    label = new QLabel (tr("Tower drag coefficient:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerDragCoeff = new NumberEdit ();
                                    m_TowerDragCoeff->setToolTip(tr("Tower drag coefficient for tower wake computation"));
                                    m_TowerDragCoeff->setMaximumWidth(EditWidth);
                                    m_TowerDragCoeff->setMinimumWidth(EditWidth);
                                    grid->addWidget(m_TowerDragCoeff, gridRowCount++, 1);
                            vBox->addStretch();

                                    m_errorLabel = new QLabel ("");
                                    vBox->addWidget(m_errorLabel);

                                    /* the output section tab */
                                    widget = new QWidget ();
                                    tabWidget->addTab(widget, "Algorithm and Wake Parameters");
                                            hBox = new QHBoxLayout ();
                                            widget->setLayout(hBox);
                                                vBox = new QVBoxLayout;
                                                hBox->addLayout(vBox);
                                                groupBox = new QGroupBox ("Algorithm Parameters");
                                                vBox->addWidget(groupBox);
                                                    grid = new QGridLayout ();
                                                    groupBox->setLayout(grid);
                                                        gridRowCount = 0;
                                                        label = new QLabel (tr("Max. number of Iterations:"));
                                                        grid->addWidget(label, gridRowCount, 0);
                                                        m_maxIterations = new NumberEdit ();
                                                        m_maxIterations->setToolTip(tr("Maximum Number of Iterations:"));
                                                        m_maxIterations->setMaximumWidth(EditWidth);
                                                        m_maxIterations->setMinimumWidth(EditWidth);
                                                        grid->addWidget(m_maxIterations, gridRowCount++, 1);
                                                        label = new QLabel ("Relaxation Factor:");
                                                        grid->addWidget(label, gridRowCount, 0);
                                                        m_relaxationFactor = new NumberEdit ();
                                                        m_relaxationFactor->setToolTip(tr("Relaxation Factor (between 0 and 1)"));
                                                        m_relaxationFactor->setMaximumWidth(EditWidth);
                                                        m_relaxationFactor->setMinimumWidth(EditWidth);
                                                        grid->addWidget(m_relaxationFactor, gridRowCount++, 1);
                                                        label = new QLabel ("Epsilon:");
                                                        grid->addWidget(label, gridRowCount, 0);
                                                        m_Epsilon = new NumberEdit ();
                                                        m_Epsilon->setToolTip(tr("Epsilon for convergence of the LLT algorithm (convergence variable is the Circulation)"));
                                                        m_Epsilon->setMaximumWidth(EditWidth);
                                                        m_Epsilon->setMinimumWidth(EditWidth);
                                                        grid->addWidget(m_Epsilon, gridRowCount++, 1);


                                                        groupBox = new QGroupBox ("Environment");
                                                        vBox->addWidget(groupBox);
                                                            grid = new QGridLayout ();
                                                            groupBox->setLayout(grid);
                                                                gridRowCount = 0;
                                                                label = new QLabel ("Air Density [k/m^3]: ");
                                                                grid->addWidget(label, gridRowCount, 0);
                                                                m_airDensEdit = new NumberEdit ();
                                                                m_airDensEdit->setMaximumWidth(EditWidth);
                                                                m_airDensEdit->setMinimumWidth(EditWidth);
                                                                grid->addWidget(m_airDensEdit, gridRowCount++, 1);
                                                                label = new QLabel ("Kinematic Viscosity [m^2/s]: ");
                                                                grid->addWidget(label, gridRowCount, 0);
                                                                m_kinViscEdit = new NumberEdit (NumberEdit::Scientific);
                                                                m_kinViscEdit->setMaximumWidth(EditWidth);
                                                                m_kinViscEdit->setMinimumWidth(EditWidth);
                                                                grid->addWidget(m_kinViscEdit, gridRowCount++, 1);

                                                            vBox->addStretch();

                                                            vBox = new QVBoxLayout;
                                                            hBox->addLayout(vBox);
                                                            groupBox = new QGroupBox ("Wake and Vortex Parameters");
                                                            vBox->addWidget(groupBox);
                                                                grid = new QGridLayout ();
                                                                groupBox->setLayout(grid);
                                                                    gridRowCount = 0;
                                                                    label = new QLabel (tr("Maximum Wake Age:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_maxWakeAge = new NumberEdit ();
                                                                    m_maxWakeAge->setToolTip(tr("The number of rotor revolutions after which a wake vortex element (after released from the TE) is removed from the domain"));
                                                                    m_maxWakeAge->setMaximumWidth(EditWidth);
                                                                    m_maxWakeAge->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_maxWakeAge, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Full Wake Revolutions:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_fullWakeRevs = new NumberEdit ();
                                                                    m_fullWakeRevs->setToolTip(tr("The number of rotor revolutions after which the wake is concentrated into the main vortices"));
                                                                    m_fullWakeRevs->setMaximumWidth(EditWidth);
                                                                    m_fullWakeRevs->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_fullWakeRevs, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Fine Wake Revolutions:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_fineWakeRevs = new NumberEdit ();
                                                                    m_fineWakeRevs->setToolTip(tr("The number of Rotor revolutions after which the concentrated wake is discetized coarser"));
                                                                    m_fineWakeRevs->setMaximumWidth(EditWidth);
                                                                    m_fineWakeRevs->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_fineWakeRevs, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Wake Thin Factor:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_thinFactor = new NumberEdit ();
                                                                    m_thinFactor->setToolTip(tr("The factor by which the wake is thinned out after the number of 'Fine Wake Revolutions'"));
                                                                    m_thinFactor->setMaximumWidth(EditWidth);
                                                                    m_thinFactor->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_thinFactor, gridRowCount++, 1);
                                                                    label = new QLabel (tr("First Wake Row Length Fraction:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_firstWakeRowLength = new NumberEdit ();
                                                                    m_firstWakeRowLength->setToolTip(tr("The fraction to shorten the length of the first wake row"));
                                                                    m_firstWakeRowLength->setMaximumWidth(EditWidth);
                                                                    m_firstWakeRowLength->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_firstWakeRowLength, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Bound Vortex Cut-Off Radius:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_cutOff = new NumberEdit ();
                                                                    m_cutOff->setToolTip(tr("The cut-off radius for the bound (blade) vortices"));
                                                                    m_cutOff->setMaximumWidth(EditWidth);
                                                                    m_cutOff->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_cutOff, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Vortex Time Offset:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_vortexTimeOffset = new NumberEdit ();
                                                                    m_vortexTimeOffset->setToolTip(tr("Vortex time offset to prevent zero core size during vortex emission"));
                                                                    m_vortexTimeOffset->setMaximumWidth(EditWidth);
                                                                    m_vortexTimeOffset->setMinimumWidth(EditWidth);
                                                                    grid->addWidget(m_vortexTimeOffset, gridRowCount++, 1);
                                                                    label = new QLabel ("Turbulent Vortex Viscosity:");
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_turbulentViscosity = new NumberEdit (NumberEdit::Scientific);
                                                                    m_turbulentViscosity->setMaximumWidth(EditWidth);
                                                                    m_turbulentViscosity->setMinimumWidth(EditWidth);


                                                                    grid->addWidget(m_turbulentViscosity, gridRowCount++, 1);

                                                                    vBox->addStretch();







    initView();
}

void QLLTCreatorDialog::initView() {
    if (m_editedLLTSimulation) {  // load parameters
        m_nameEdit->setText(m_editedLLTSimulation->getName());
        m_timeStepEdit->setValue(m_editedLLTSimulation->getTimeStep());
        m_rotorSpeedEdit->setValue(m_editedLLTSimulation->getRotorSpeed());
        m_nacelleTeetherEdit->setValue(m_editedLLTSimulation->getTeetherAngle());
        m_nacelleYawEdit->setValue(m_editedLLTSimulation->getYawAngle());
        m_infloweAngleEdit->setValue(m_editedLLTSimulation->getInflowAngle());
        m_airDensEdit->setValue(m_editedLLTSimulation->getDensity());
        m_kinViscEdit->setValue(m_editedLLTSimulation->getKinVisc());
        m_turbulentViscosity->setValue(m_editedLLTSimulation->getTurbVisc());
        m_maxWakeAge->setValue(m_editedLLTSimulation->getMaxWakeRevolutions());
        m_firstWakeRowLength->setValue(m_editedLLTSimulation->getFirstWakeRowLength());
        m_fullWakeRevs->setValue(m_editedLLTSimulation->getFullWakeRevolutions());
        m_fineWakeRevs->setValue(m_editedLLTSimulation->getFineWakeRevolutions());
        m_thinFactor->setValue(m_editedLLTSimulation->getThinFactor());
        m_vortexTimeOffset->setValue(m_editedLLTSimulation->getVortexTimeOffset());
        m_Epsilon->setValue(m_editedLLTSimulation->getEpsilon());
        m_relaxationFactor->setValue(m_editedLLTSimulation->getRelaxFactor());
        m_maxIterations->setValue(m_editedLLTSimulation->getMaxIterations());
        m_cutOff->setValue(m_editedLLTSimulation->getCutOffRadius());
        m_TowerRadius->setValue(m_editedLLTSimulation->getTowerRadius());
        m_TowerDragCoeff->setValue(m_editedLLTSimulation->getTowerDrag());
        m_TowerDistance->setValue(m_editedLLTSimulation->getTowerDistance());
        m_HubHeight->setValue(m_editedLLTSimulation->getHubHeight());
        m_windFieldComboBox->setCurrentObject(m_editedLLTSimulation->getUsedWindField());
        m_rotorComboBox->setCurrentObject(m_editedLLTSimulation->getUsedBlade());
        m_TowerShadowGroup->button((m_editedLLTSimulation->getIncludeTower() ? 0 : 1))->setChecked(true);
        m_GroundEffectGroup->button((m_editedLLTSimulation->getIncludeGround() ? 0 : 1))->setChecked(true);
        m_ShedTrailGroup->button((m_editedLLTSimulation->getShed() ? 0 : 1))->setChecked(true);
        m_WakeRollupGroup->button((m_editedLLTSimulation->getWakeRollup() ? 0 : 1))->setChecked(true);
        m_PrescribedGroup->button((m_editedLLTSimulation->getPrescribed() ? 0 : 1))->setChecked(true);
        m_TurbConvectionGroup->button((m_editedLLTSimulation->getTurbConvection() ? 0 : 1))->setChecked(true);
        m_totalRunTimeEdit->setValue(m_editedLLTSimulation->getTimeStep()*m_editedLLTSimulation->getNumberTimesteps());
        m_aziStepEdit->setValue(m_editedLLTSimulation->getAziDisc());
        m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/m_editedLLTSimulation->getUsedWindField()->getMeanWindSpeedAtHub()/60);
        m_numberTimesteps->setValue(m_editedLLTSimulation->getRunTime()/m_editedLLTSimulation->getTimeStep());
        m_numberRevs->setValue(m_numberTimesteps->getValue()*m_aziStepEdit->getValue()/360);


        m_TowerRadius->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_TowerDragCoeff->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_TowerDistance->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_HubHeight->setEnabled(m_editedLLTSimulation->getIncludeGround());


    }else  {  // set default parameters
        m_nameEdit->setText("New LLT Simulation");
        m_aziStepEdit->setValue(10);
        m_TSREdit->setValue(7);
        m_nacelleTeetherEdit->setValue(0);
        m_nacelleYawEdit->setValue(0);
        m_infloweAngleEdit->setValue(0);
        m_airDensEdit->setValue(1.225);
        m_kinViscEdit->setValue(1.4661e-5);
        m_turbulentViscosity->setValue(40);
        m_maxWakeAge->setValue(8);
        m_firstWakeRowLength->setValue(1.0);
        m_fullWakeRevs->setValue(0.5);
        m_fineWakeRevs->setValue(4);
        m_thinFactor->setValue(2);
        m_vortexTimeOffset->setValue(0.5);
        m_Epsilon->setValue(0.001);
        m_relaxationFactor->setValue(0.6);
        m_maxIterations->setValue(100);
        m_cutOff->setValue(1.0);
        m_TowerRadius->setValue(1.0);
        m_TowerDragCoeff->setValue(0.5);
        m_TowerDistance->setValue(2.0);
        m_HubHeight->setValue(90);
        m_numberRevs->setValue(10);
        m_TowerShadowGroup->button(1)->setChecked(true);
        m_GroundEffectGroup->button(1)->setChecked(true);
        m_ShedTrailGroup->button(0)->setChecked(true);
        m_WakeRollupGroup->button(0)->setChecked(true);
        m_PrescribedGroup->button(1)->setChecked(true);
        m_TurbConvectionGroup->button(1)->setChecked(true);

        m_numberTimesteps->setValue(m_numberRevs->getValue()*360/m_aziStepEdit->getValue());
        m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->getRotorRadius()/2/PI*m_windFieldComboBox->currentObject()->getMeanWindSpeedAtHub()*60);
        m_timeStepEdit->setValue(m_aziStepEdit->getValue()/360*60/m_rotorSpeedEdit->getValue());

        m_totalRunTimeEdit->setValue(m_timeStepEdit->getValue()*m_numberTimesteps->getValue());


        m_TowerRadius->setEnabled(false);
        m_TowerDragCoeff->setEnabled(false);
        m_TowerDistance->setEnabled(false);
        m_HubHeight->setEnabled(false);
    }

    onWindFieldOrRotorChanged();
}

void QLLTCreatorDialog::onWindFieldOrRotorChanged() {
    WindField *windField = m_windFieldComboBox->currentObject();
    if (windField) {  // can be NULL during the call from initView()
        CBlade *rotor = m_rotorComboBox->currentObject();
        if (rotor && windField->getRotorRadius() < rotor->getRotorRadius()) {
            m_errorLabel->setText(tr("<font color='red'>Windfield to small for Rotor!</font>"));
        } else {
            m_errorLabel->setText("");
        }
        if (rotor){
            m_totalRunTimeEdit->setMaximum(windField->getSimulationTime()-2*rotor->getRotorRadius()/windField->getMeanWindSpeedAtHub());
        }
    }
    OnTSRChanged();
    m_HubHeight->setValue(m_windFieldComboBox->currentObject()->getHubheight());

}


void QLLTCreatorDialog::onCreateButtonClicked() {
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
    if (errorMessage != "") {
        QMessageBox::critical(this, tr("Create Simulation"), QString(tr("The following error(s) occured:\n")
                                                                            + errorMessage), QMessageBox::Ok);
        return;
    }

    QLLTSimulation *simulation =
            new QLLTSimulation(m_nameEdit->text(), false,
                               m_windFieldComboBox->currentObject(),
                               m_timeStepEdit->getValue(),
                               m_numberTimesteps->getValue(),
                               m_aziStepEdit->getValue(),
                               m_rotorComboBox->currentObject(),
                               m_rotorSpeedEdit->getValue(),
                               m_nacelleYawEdit->getValue(),
                               m_nacelleTeetherEdit->getValue(),
                               m_infloweAngleEdit->getValue(),
                               m_turbulentViscosity->getValue(),
                               m_airDensEdit->getValue(),
                               m_kinViscEdit->getValue(),
                               m_firstWakeRowLength->getValue(),
                               m_fullWakeRevs->getValue(),
                               m_fineWakeRevs->getValue(),
                               m_maxWakeAge->getValue(),
                               m_vortexTimeOffset->getValue(),
                               m_Epsilon->getValue(),
                               m_relaxationFactor->getValue(),
                               m_maxIterations->getValue(),
                               m_thinFactor->getValue(),
                               m_cutOff->getValue(),
                               m_TowerShadowGroup->button(0)->isChecked(),
                               m_TowerDragCoeff->getValue(),
                               m_TowerRadius->getValue(),
                               m_TowerDistance->getValue(),
                               m_GroundEffectGroup->button(0)->isChecked(),
                               m_HubHeight->getValue(),
                               m_WakeRollupGroup->button(0)->isChecked(),
                               m_ShedTrailGroup->button(0)->isChecked(),
                               m_PrescribedGroup->button(0)->isChecked(),
                               m_TurbConvectionGroup->button(0)->isChecked());

    if (g_QLLTHAWTSimulationStore.add(simulation)) {
        m_module->setShownLLTSimulation(simulation, true);
        accept();  // leave dialog only if adding was successful
    }
}

void QLLTCreatorDialog::onCancelButtonClicked() {
    reject();
}

void QLLTCreatorDialog::OnRunTimeChanged (){

    m_numberRevs->setValue(m_totalRunTimeEdit->getValue()/m_timeStepEdit->getValue()/360*m_aziStepEdit->getValue());
    m_numberTimesteps->setValue(m_totalRunTimeEdit->getValue()/m_timeStepEdit->getValue());

}

void QLLTCreatorDialog::OnNumberTimestepsChanged (){

    m_numberRevs->setValue(m_numberTimesteps->getValue()/360*m_aziStepEdit->getValue());
    m_totalRunTimeEdit->setValue(m_numberTimesteps->getValue()*m_timeStepEdit->getValue());

}

void QLLTCreatorDialog::OnNumberRevolutionsChanged (){

    m_totalRunTimeEdit->setValue(m_numberRevs->getValue()*360/m_aziStepEdit->getValue()*m_timeStepEdit->getValue());
    m_numberTimesteps->setValue(m_numberRevs->getValue()*360/m_aziStepEdit->getValue());

}

void QLLTCreatorDialog::OnTSRChanged (){

    m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->getRotorRadius()/2/PI*m_windFieldComboBox->currentObject()->getMeanWindSpeedAtHub()*60);
    OnAziChanged ();

}

void QLLTCreatorDialog::OnRotationalSpeedChanged (){

    m_TSREdit->setValue(m_rotorSpeedEdit->getValue()*m_rotorComboBox->currentObject()->getRotorRadius()*2*PI/m_windFieldComboBox->currentObject()->getMeanWindSpeedAtHub()/60);
    OnAziChanged ();
}

void QLLTCreatorDialog::OnAziChanged (){

    m_timeStepEdit->setValue(m_aziStepEdit->getValue()/360*60/m_rotorSpeedEdit->getValue());
    OnNumberTimestepsChanged ();
}

void QLLTCreatorDialog::OnTimestepChanged (){

    m_aziStepEdit->setValue(m_timeStepEdit->getValue()*360/60*m_rotorSpeedEdit->getValue());
    OnNumberTimestepsChanged ();
}

void QLLTCreatorDialog::OnIncludeTowerChanged (){

    m_TowerRadius->setEnabled(m_TowerShadowGroup->button(0)->isChecked());
    m_TowerDragCoeff->setEnabled(m_TowerShadowGroup->button(0)->isChecked());
    m_TowerDistance->setEnabled(m_TowerShadowGroup->button(0)->isChecked());

}

void QLLTCreatorDialog::OnIncludeGroundChanged (){

    m_HubHeight->setEnabled(m_GroundEffectGroup->button(0)->isChecked());

}




