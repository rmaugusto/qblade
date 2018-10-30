/****************************************************************************

    QLLTCreatorDialog Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "QLLTCreatorDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextEdit>
#include "QLLTSimulation.h"
#include "../StoreAssociatedComboBox.h"
#include "../XUnsteadyBEM/WindField.h"
#include "../XBEM/Blade.h"
#include "../src/GlobalFunctions.h"
#include "../src/XBEM/BEM.h"


QLLTCreatorDialog::QLLTCreatorDialog(QLLTSimulation *presetSimulation, QLLTModule *module, bool isVawt) {
    m_editedLLTSimulation = presetSimulation;
    m_module = module;
    m_bisVawt = isVawt;
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




                QGroupBox *groupBox = new QGroupBox (tr("Name and Rotor"));
                vBox->addWidget(groupBox);
                    QGridLayout *grid = new QGridLayout ();
                    groupBox->setLayout(grid);
                    int gridRowCount = 0;

                        QLabel *label = new QLabel (tr("Name of Simulation: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_nameEdit = new QLineEdit ();
                        m_nameEdit->setMinimumWidth(EditWidth);
                        m_nameEdit->setMaximumWidth(EditWidth);
                        QHBoxLayout* miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_nameEdit);
                        grid->addLayout(miniHBox, gridRowCount++, 1);

                        label = new QLabel (tr("Store Replay: "));
                        grid->addWidget (label, gridRowCount, 0);
                        miniHBox = new QHBoxLayout ();
                        grid->addLayout(miniHBox, gridRowCount++, 1);
                            miniHBox->addStretch();
                            m_storeWakePositions = new QButtonGroup(miniHBox);
                            QRadioButton *radioButton = new QRadioButton ("On");
                            m_storeWakePositions->addButton(radioButton, 0);
                            miniHBox->addWidget(radioButton);
                            radioButton = new QRadioButton ("Off");
                            m_storeWakePositions->addButton(radioButton, 1);
                            miniHBox->addWidget(radioButton);

                        label = new QLabel (tr("Rotor: "));
                        grid->addWidget(label, gridRowCount, 0);
                        if (!m_bisVawt) m_rotorComboBox = new RotorComboBox (&g_rotorStore, false);
                        if (m_bisVawt) m_rotorComboBox = new RotorComboBox (&g_verticalRotorStore, false);
                        m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                        m_rotorComboBox->setMaximumWidth(EditWidth);
                        m_rotorComboBox->setMinimumWidth(EditWidth);
//                        connect(m_rotorComboBox, SIGNAL(valueChanged(int)), this, SLOT(onRotorChanged()));
                        miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_rotorComboBox);
                        grid->addLayout(miniHBox, gridRowCount++, 1);

                        label = new QLabel (tr("Type:"));
                        if (!m_bisVawt) grid->addWidget (label, gridRowCount, 0);
                        miniHBox = new QHBoxLayout ();
                        if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                            miniHBox->addStretch();
                            m_upWindGroup = new QButtonGroup(miniHBox);
                            radioButton = new QRadioButton ("UpWind");
                            m_upWindGroup->addButton(radioButton, 0);
                            miniHBox->addWidget(radioButton);
                            radioButton = new QRadioButton ("DownWind");
                            m_upWindGroup->addButton(radioButton, 1);
                            miniHBox->addWidget(radioButton);

                        if (!m_bisVawt) label = new QLabel (tr("Hub Height [m]:"));
                        if (m_bisVawt) label = new QLabel (tr("Ground Clearance [m]:"));

                        grid->addWidget(label, gridRowCount, 0);
                        m_HubHeight = new NumberEdit ();
                        if (!m_bisVawt) m_HubHeight->setToolTip(tr("Rotor hub height"));
                        if (m_bisVawt) m_HubHeight->setToolTip(tr("Rotor gound clearance"));
                        m_HubHeight->setMaximumWidth(EditWidth);
                        m_HubHeight->setMinimumWidth(EditWidth);
                        miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_HubHeight);
                        grid->addLayout(miniHBox, gridRowCount++, 1);

                        label = new QLabel (tr("Rotor Overhang [m]:"));
                        if (!m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                        m_RotorOverhang = new NumberEdit ();
                        m_RotorOverhang->setToolTip(tr("Distance between tower center and rotor hub"));
                        m_RotorOverhang->setMaximumWidth(EditWidth);
                        m_RotorOverhang->setMinimumWidth(EditWidth);
                        if (!m_bisVawt) {
                            miniHBox = new QHBoxLayout ();
                            miniHBox->addStretch();
                            miniHBox->addWidget(m_RotorOverhang);
                            grid->addLayout(miniHBox, gridRowCount++, 1);
                        }

                        groupBox = new QGroupBox (tr("Blade Discretization"));
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            QVBoxLayout *VBOX = new QVBoxLayout();
                            miniHBox = new QHBoxLayout ();

                            groupBox->setLayout(VBOX);
                            VBOX->addLayout(miniHBox);
                            label = new QLabel(tr("Panels: "));
                            miniHBox->addWidget(label);
                            m_numPanels = new QSpinBox;
                            m_numPanels->setMinimum(1);
                            m_numPanels->setMaximum(100);
                            m_numPanels->setSingleStep(1);
                            miniHBox->addWidget(m_numPanels);
                                    m_discTypeGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("From Table");
                                    m_discTypeGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Linear");
                                    m_discTypeGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Sinusoidal");
                                    m_discTypeGroup->addButton(radioButton, 2);
                                    miniHBox->addWidget(radioButton);

                                    miniHBox = new QHBoxLayout ();
                                    VBOX->addLayout(miniHBox);
                                    strutLab1 = new QLabel(tr("Strut Panels: "));
                                    miniHBox->addWidget(strutLab1);
                                    m_numStrutPanels = new QSpinBox;
                                    m_numStrutPanels->setMinimum(1);
                                    m_numStrutPanels->setMaximum(100);
                                    m_numStrutPanels->setSingleStep(1);
                                    miniHBox->addWidget(m_numStrutPanels);
                                    strutLab2 = new QLabel(tr("Linear"));
                                    miniHBox->addWidget(strutLab2);
                                    m_strutGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("Drag");
                                    m_strutGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Lift/Drag/Wake");
                                    m_strutGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);


                        connect(m_discTypeGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnDiscTypeChanged()));


                        groupBox = new QGroupBox (tr("Simulation Type"));
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            VBOX = new QVBoxLayout();
                            miniHBox = new QHBoxLayout ();

                            groupBox->setLayout(VBOX);
                            VBOX->addLayout(miniHBox);
                            VBOX->addLayout(grid);
                            gridRowCount = 0;

//                                    miniHBox->addStretch();
                                    m_WindInputGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("Simple Simulation");
                                    m_WindInputGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Simulation Input File");
                                    m_WindInputGroup->addButton(radioButton, 2);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("AeroDyn HH Input File");
                                    m_WindInputGroup->addButton(radioButton, 3);
                                    miniHBox->addWidget(radioButton);
                                    m_WindInputGroup->button(1)->setChecked(true);
                                    connect(m_WindInputGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnWindTypeChanged()));


                               miniHBox = new QHBoxLayout();
                               miniHBox->addStretch();
                               m_SimFileLabel = new QLabel (tr("- NONE -"));
                               m_AeroDynLabel = new QLabel (tr("- NONE -"));
                               miniHBox->addWidget(m_SimFileLabel);
                               miniHBox->addWidget(m_AeroDynLabel);
                               miniHBox->addStretch();
                               grid->addLayout(miniHBox, gridRowCount, 0);

                               m_OpenFile = new QPushButton (tr("Open File"));
                               m_OpenFile->setMaximumWidth(EditWidth/2-3);
                               m_OpenFile->setMinimumWidth(EditWidth/2-3);
                               connect(m_OpenFile, SIGNAL(clicked()), this, SLOT(OnImportWindFile()));
                               m_ViewFile = new QPushButton (tr("View File"));
                               m_ViewFile->setMaximumWidth(EditWidth/2-5);
                               m_ViewFile->setMinimumWidth(EditWidth/2-5);
                               connect(m_ViewFile, SIGNAL(clicked()), this, SLOT(OnViewWindFile()));

                               miniHBox = new QHBoxLayout();
                               miniHBox->addStretch();
                               miniHBox->addWidget(m_OpenFile);
                               miniHBox->addWidget(m_ViewFile);
                               grid->addLayout(miniHBox, gridRowCount++, 1);

                               m_SimFileLabel->setVisible(false);
                               m_OpenFile->setVisible(false);
                               m_AeroDynLabel->setVisible(false);
                               m_ViewFile->setVisible(false);

                               groupBox = new QGroupBox (tr("Wind Input Data"));
                               vBox->addWidget(groupBox);
                                   grid = new QGridLayout ();
                                   groupBox->setLayout(grid);
                                   gridRowCount = 0;

                                   label = new QLabel (tr("Use Turbulent Windfield: "));
                                   grid->addWidget (label, gridRowCount, 0);
                                   miniHBox = new QHBoxLayout ();
                                   grid->addLayout(miniHBox, gridRowCount++, 1);
                                       miniHBox->addStretch();
                                       m_useWindfieldGroup = new QButtonGroup(miniHBox);
                                       radioButton = new QRadioButton ("On");
                                       m_useWindfieldGroup->addButton(radioButton, 0);
    //                                   connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnIncludeTowerChanged()));
                                       miniHBox->addWidget(radioButton);
                                       radioButton = new QRadioButton ("Off");
                                       m_useWindfieldGroup->addButton(radioButton, 1);
                                       miniHBox->addWidget(radioButton);
                                   connect(m_useWindfieldGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnUseWindField()));


                                   m_WindfieldLabel = new QLabel (tr("Windfield: "));
                                   grid->addWidget(m_WindfieldLabel, gridRowCount, 0);
                                   m_windFieldComboBox = new WindFieldComboBox (&g_windFieldStore, false);
                                   m_windFieldComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                                   m_windFieldComboBox->setMaximumWidth(EditWidth);
                                   m_windFieldComboBox->setMinimumWidth(EditWidth);
                                   connect(m_windFieldComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldChanged()));
                                   miniHBox = new QHBoxLayout();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_windFieldComboBox);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);

                                   m_InflowLabel = new QLabel (tr("Inflow Speed [m/s]: "));
                                   grid->addWidget(m_InflowLabel, gridRowCount, 0);
                                   m_InflowSpeed = new NumberEdit();
                                   m_InflowSpeed->setMaximumWidth(EditWidth);
                                   m_InflowSpeed->setMinimumWidth(EditWidth);
                                   m_InflowSpeed->setMinimum(1e-5);
                                   m_InflowSpeed->setMaximum(1e5);
           //                        connect(m_windFieldComboBox, SIGNAL(valueChanged(int)), this, SLOT(onWindFieldOrRotorChanged()));
                                   miniHBox = new QHBoxLayout();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_InflowSpeed);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);
                                   connect(m_InflowSpeed, SIGNAL(editingFinished()), this, SLOT(onWindFieldChanged()));


                                   label = new QLabel (tr("Vertical Inflow Speed [m/s]: "));
                                   grid->addWidget(label, gridRowCount, 0);
                                   m_VertInflowSpeedEdit = new NumberEdit ();
                                   m_VertInflowSpeedEdit->setToolTip(tr("The vertical inflow speed"));
                                   m_VertInflowSpeedEdit->setMaximumWidth(EditWidth);
                                   m_VertInflowSpeedEdit->setMinimumWidth(EditWidth);
                                   miniHBox = new QHBoxLayout ();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_VertInflowSpeedEdit);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);
                                   connect(m_VertInflowSpeedEdit, SIGNAL(editingFinished()), this, SLOT(onWindFieldChanged()));

                                   label = new QLabel (tr("Horizontal Inflow Angle [deg]: "));
                                   grid->addWidget(label, gridRowCount, 0);
                                   m_horInflowAngleEdit = new NumberEdit ();
                                   m_horInflowAngleEdit->setToolTip(tr("The inflow angle of the wind on the rotor [deg]"));
                                   m_horInflowAngleEdit->setMaximumWidth(EditWidth);
                                   m_horInflowAngleEdit->setMinimumWidth(EditWidth);
                                   miniHBox = new QHBoxLayout ();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_horInflowAngleEdit);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);

                                   label = new QLabel (tr("Wind Shear Profile:"));
                                   grid->addWidget (label, gridRowCount, 0);
                                   miniHBox = new QHBoxLayout ();
                                   grid->addLayout(miniHBox, gridRowCount++, 1);
                                       miniHBox->addStretch();
                                       m_ShearGroup = new QButtonGroup(miniHBox);
                                       radioButton = new QRadioButton ("Power Law");
                                       m_ShearGroup->addButton(radioButton, 0);
                                       miniHBox->addWidget(radioButton);
                                       radioButton = new QRadioButton ("Logarithmic");
                                       m_ShearGroup->addButton(radioButton, 1);
                                       miniHBox->addWidget(radioButton);
                                       connect(m_ShearGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnShearChanged()));

                                   m_ShearLabel = new QLabel (tr("Power Law Shear exp [-]: "));
                                   grid->addWidget(m_ShearLabel, gridRowCount, 0);
                                   m_ShearCoefficientEdit = new NumberEdit ();
                                   m_ShearCoefficientEdit->setToolTip(tr("Exponent for vertical power law wind shear"));
                                   m_ShearCoefficientEdit->setMaximumWidth(EditWidth);
                                   m_ShearCoefficientEdit->setMinimumWidth(EditWidth);
                                   miniHBox = new QHBoxLayout ();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_ShearCoefficientEdit);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);

                                   label = new QLabel (tr("Reference Height [m]: "));
                                   grid->addWidget(label, gridRowCount, 0);
                                   m_refHeightEdit = new NumberEdit ();
                                   m_refHeightEdit->setToolTip(tr("Reference Height"));
                                   m_refHeightEdit->setMaximumWidth(EditWidth);
                                   m_refHeightEdit->setMinimumWidth(EditWidth);
                                   miniHBox = new QHBoxLayout ();
                                   miniHBox->addStretch();
                                   miniHBox->addWidget(m_refHeightEdit);
                                   grid->addLayout(miniHBox, gridRowCount++, 1);

                                groupBox = new QGroupBox (tr("Rotor Angles"));
                                vBox->addWidget(groupBox);
                                    grid = new QGridLayout ();
                                    groupBox->setLayout(grid);
                                        gridRowCount = 0;
                                label = new QLabel (tr("Rotor Yaw [deg]: "));
                                if (!m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                m_nacelleYawEdit = new NumberEdit ();
                                m_nacelleYawEdit->setToolTip(tr("Initial or fixed rotor-yaw angle [deg]"));
                                m_nacelleYawEdit->setMaximumWidth(EditWidth);
                                m_nacelleYawEdit->setMinimumWidth(EditWidth);
                                miniHBox = new QHBoxLayout ();
                                miniHBox->addStretch();
                                miniHBox->addWidget(m_nacelleYawEdit);
                                if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                label = new QLabel (tr("Rotorshaft Tilt [deg]: "));
                                if (!m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                m_shaftTilt = new NumberEdit ();
                                m_shaftTilt->setToolTip(tr("Fixed Rotorshaft tilt angle [deg]"));
                                m_shaftTilt->setMaximumWidth(EditWidth);
                                m_shaftTilt->setMinimumWidth(EditWidth);
                                miniHBox = new QHBoxLayout ();
                                miniHBox->addStretch();
                                miniHBox->addWidget(m_shaftTilt);
                                if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                label = new QLabel (tr("Rotor Cone Angle [deg]: "));
                                if (!m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                m_ConeAngle = new NumberEdit ();
                                m_ConeAngle->setToolTip(tr("Fixed rotor cone angle [deg]"));
                                m_ConeAngle->setMaximumWidth(EditWidth);
                                m_ConeAngle->setMinimumWidth(EditWidth);
                                miniHBox = new QHBoxLayout ();
                                miniHBox->addStretch();
                                miniHBox->addWidget(m_ConeAngle);
                                m_ConeAngle->setMinimum(-45);
                                m_ConeAngle->setMaximum(45);
                                if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                label = new QLabel (tr("Y-Roll Angle [deg]: "));
                                if (m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                m_YRollEdit = new NumberEdit ();
                                m_YRollEdit->setToolTip(tr("Roll angle around y axis [deg]"));
                                m_YRollEdit->setMaximumWidth(EditWidth);
                                m_YRollEdit->setMinimumWidth(EditWidth);
                                miniHBox = new QHBoxLayout ();
                                miniHBox->addStretch();
                                miniHBox->addWidget(m_YRollEdit);
                                if (m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                label = new QLabel (tr("X-Roll Angle [deg]: "));
                                if (m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                m_XRollEdit = new NumberEdit ();
                                m_XRollEdit->setToolTip(tr("Roll angle around x-axis [deg]"));
                                m_XRollEdit->setMaximumWidth(EditWidth);
                                m_XRollEdit->setMinimumWidth(EditWidth);
                                miniHBox = new QHBoxLayout ();
                                miniHBox->addStretch();
                                miniHBox->addWidget(m_XRollEdit);
                                if (m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);



                        vBox->addStretch();

                        vBox = new QVBoxLayout;
                        hBox->addLayout(vBox);

                        groupBox = new QGroupBox ("Starting Position");
                        vBox->addWidget(groupBox);
                            grid = new QGridLayout ();
                            groupBox->setLayout(grid);
                                gridRowCount = 0;
                        label = new QLabel (tr("Blade 1 initial azimuth [deg]:"));
                        grid->addWidget(label, gridRowCount, 0);
                        m_aziStart = new NumberEdit ();
                        m_aziStart->setToolTip(tr("Defines the starting position of the rotor"));
                        m_aziStart->setMaximumWidth(EditWidth);
                        m_aziStart->setMinimumWidth(EditWidth);
                        m_aziStart->setMinimum(0);
                        m_aziStart->setMaximum(360);
                        miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_aziStart);
                        grid->addLayout(miniHBox, gridRowCount++, 1);

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
                        m_TSREdit->setAutomaticPrecision(8);
                        m_TSREdit->setMinimum(0.0001);

                        miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_TSREdit);
                        grid->addLayout(miniHBox, gridRowCount++, 1);
                        connect(m_TSREdit, SIGNAL(editingFinished()), this, SLOT(OnTSRChanged()));
                        connect(m_TSREdit, SIGNAL(valueChanged(double)), this, SLOT(OnDesingChanged()));

                        label = new QLabel (tr("Rotor rotational speed [rpm]: "));
                        grid->addWidget(label, gridRowCount, 0);
                        m_rotorSpeedEdit = new NumberEdit ();
                        m_rotorSpeedEdit->setToolTip(tr("Initial or fixed rotor speed [rpm]"));
                        m_rotorSpeedEdit->setAutomaticPrecision(2);
                        m_rotorSpeedEdit->setMaximumWidth(EditWidth);
                        m_rotorSpeedEdit->setMinimumWidth(EditWidth);
                        m_rotorSpeedEdit->setAutomaticPrecision(8);
                        m_rotorSpeedEdit->setMinimum(0.001);

                        miniHBox = new QHBoxLayout ();
                        miniHBox->addStretch();
                        miniHBox->addWidget(m_rotorSpeedEdit);
                        grid->addLayout(miniHBox, gridRowCount++, 1);
                        connect(m_rotorSpeedEdit, SIGNAL(editingFinished()), this, SLOT(OnRotationalSpeedChanged()));


          groupBox = new QGroupBox (tr("Length of Simulation"));
          vBox->addWidget(groupBox);
              grid = new QGridLayout ();
              groupBox->setLayout(grid);
              gridRowCount = 0;

                 label = new QLabel (tr("Number of timesteps [-]: "));
                 grid->addWidget(label, gridRowCount, 0);
                 m_numberTimesteps = new NumberEdit ();
                 m_numberTimesteps->setMinimum(2);
                 m_numberTimesteps->setAutomaticPrecision(8);
                 m_numberTimesteps->setToolTip(tr("Number of timesteps that will be simulated"));
                 m_numberTimesteps->setMaximumWidth(EditWidth);
                 m_numberTimesteps->setMinimumWidth(EditWidth);
                 connect(m_numberTimesteps, SIGNAL(editingFinished()), this, SLOT(OnNumberTimestepsChanged()));
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_numberTimesteps);
                 grid->addLayout(miniHBox, gridRowCount++, 1);

                 label = new QLabel (tr("Number of rotor revolutions [-]: "));
                 grid->addWidget(label, gridRowCount, 0);
                 m_numberRevs = new NumberEdit ();
                 m_numberRevs->setToolTip(tr("Number of rotor revolutions that will be simulated"));
                 m_numberRevs->setAutomaticPrecision(8);
                 m_numberRevs->setMaximumWidth(EditWidth);
                 m_numberRevs->setMinimumWidth(EditWidth);
                 connect(m_numberRevs, SIGNAL(editingFinished()), this, SLOT(OnNumberRevolutionsChanged()));
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_numberRevs);
                 grid->addLayout(miniHBox, gridRowCount++, 1);


                 label = new QLabel (tr("Simulation Length [s]: "));
                 grid->addWidget(label, gridRowCount, 0);
                 m_totalRunTimeEdit = new NumberEdit ();
                 m_totalRunTimeEdit->setAutomaticPrecision(8);
                 m_totalRunTimeEdit->setMaximumWidth(EditWidth);
                 m_totalRunTimeEdit->setMinimumWidth(EditWidth);
                 connect(m_totalRunTimeEdit, SIGNAL(editingFinished()), this, SLOT(OnRunTimeChanged()));
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_totalRunTimeEdit);
                 grid->addLayout(miniHBox, gridRowCount++, 1);


                 groupBox = new QGroupBox (tr("Time Discretization"));
                 vBox->addWidget(groupBox);
                     grid = new QGridLayout ();
                     groupBox->setLayout(grid);
                         gridRowCount = 0;

                 label = new QLabel (tr("Azimuthal step size [deg]: "));
                 grid->addWidget(label, gridRowCount, 0);
                 m_aziStepEdit = new NumberEdit ();
                 m_aziStepEdit->setAutomaticPrecision(8);
                 m_aziStepEdit->setMaximumWidth(EditWidth);
                 m_aziStepEdit->setMinimumWidth(EditWidth);
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_aziStepEdit);
                 grid->addLayout(miniHBox, gridRowCount++, 1);
                 connect(m_aziStepEdit, SIGNAL(editingFinished()), this, SLOT(OnAziChanged()));


                 label = new QLabel (tr("Time step size [s]: "));
                 grid->addWidget(label, gridRowCount, 0);
                 m_timeStepEdit = new NumberEdit ();
                 m_timeStepEdit->setAutomaticPrecision(8);
                 m_timeStepEdit->setMaximumWidth(EditWidth);
                 m_timeStepEdit->setMinimumWidth(EditWidth);
                 m_timeStepEdit->setAutomaticPrecision(8);
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_timeStepEdit);
                 grid->addLayout(miniHBox, gridRowCount++, 1);
                 connect(m_timeStepEdit, SIGNAL(editingFinished()), this, SLOT(OnTimestepChanged()));


                 label = new QLabel (tr("Wake shed at every n-th timestep [-]:"));
                 grid->addWidget(label, gridRowCount, 0);
                 m_Nth_WakeStep = new NumberEdit ();
                 m_Nth_WakeStep->setToolTip(tr("New wake elements are only created every n-th timestep, induction is interpolated between timesteps"));
                 m_Nth_WakeStep->setMaximumWidth(EditWidth);
                 m_Nth_WakeStep->setMinimumWidth(EditWidth);
                 miniHBox = new QHBoxLayout ();
                 miniHBox->addStretch();
                 miniHBox->addWidget(m_Nth_WakeStep);
                 m_Nth_WakeStep->setMinimum(1);
                 m_Nth_WakeStep->setMaximum(10);
                 grid->addLayout(miniHBox, gridRowCount++, 1);


                 label = new QLabel (tr("Adaptive Timestepping:"));
                 grid->addWidget (label, gridRowCount, 0);
                 miniHBox = new QHBoxLayout ();
                 grid->addLayout(miniHBox, gridRowCount++, 1);
                     miniHBox->addStretch();
                     m_adaptiveGroup = new QButtonGroup(miniHBox);
                     radioButton = new QRadioButton ("On");
                     m_adaptiveGroup->addButton(radioButton, 0);
                     miniHBox->addWidget(radioButton);
                     radioButton = new QRadioButton ("Off");
                     m_adaptiveGroup->addButton(radioButton, 1);
                     miniHBox->addWidget(radioButton);
                     connect(m_adaptiveGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnWindTypeChanged()));




                       groupBox = new QGroupBox ("Modeling Parameters");
                       vBox->addWidget(groupBox);
                       grid = new QGridLayout ();
                       groupBox->setLayout(grid);
                       gridRowCount = 0;


                               label = new QLabel (tr("3D Himmelskamp Effect:"));
                               if (!m_bisVawt) grid->addWidget (label, gridRowCount, 0);
                               miniHBox = new QHBoxLayout ();
                               grid->addLayout(miniHBox, gridRowCount++, 1);
                                   miniHBox->addStretch();
                                   m_HimmelskampGroup = new QButtonGroup(miniHBox);
                                   radioButton = new QRadioButton ("On");
                                   m_HimmelskampGroup->addButton(radioButton, 0);
                                   if (!m_bisVawt) miniHBox->addWidget(radioButton);
                                   radioButton = new QRadioButton ("Off");
                                   m_HimmelskampGroup->addButton(radioButton, 1);
                                   if (!m_bisVawt) miniHBox->addWidget(radioButton);

                                label = new QLabel (tr("Include Ground Effect:"));
                                grid->addWidget (label, gridRowCount, 0);
                                miniHBox = new QHBoxLayout ();
                                grid->addLayout(miniHBox, gridRowCount++, 1);
                                    miniHBox->addStretch();
                                    m_GroundEffectGroup = new QButtonGroup(miniHBox);
                                    radioButton = new QRadioButton ("On");
                                    m_GroundEffectGroup->addButton(radioButton, 0);
                                    miniHBox->addWidget(radioButton);
                                    radioButton = new QRadioButton ("Off");
                                    m_GroundEffectGroup->addButton(radioButton, 1);
                                    miniHBox->addWidget(radioButton);

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

                                    label = new QLabel (tr("Tower Height [m]:"));
                                    if (m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                    m_TowerHeight = new NumberEdit ();
                                    m_TowerHeight->setToolTip(tr("Height of the tower"));
                                    m_TowerHeight->setMaximumWidth(EditWidth);
                                    m_TowerHeight->setMinimumWidth(EditWidth);
                                    miniHBox = new QHBoxLayout ();
                                    miniHBox->addStretch();
                                    miniHBox->addWidget(m_TowerHeight);
                                    if (m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);

                                    label = new QLabel (tr("Tower Top Radius [m]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerTopRadius = new NumberEdit ();
                                    m_TowerTopRadius->setToolTip(tr("Radius of the tower at nacelle height"));
                                    m_TowerTopRadius->setMaximumWidth(EditWidth);
                                    m_TowerTopRadius->setMinimumWidth(EditWidth);
                                    miniHBox = new QHBoxLayout ();
                                    miniHBox->addStretch();
                                    miniHBox->addWidget(m_TowerTopRadius);
                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                    label = new QLabel (tr("Tower Base Radius [m]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerBottomRadius = new NumberEdit ();
                                    m_TowerBottomRadius->setToolTip(tr("Radius of the tower at the ground"));
                                    m_TowerBottomRadius->setMaximumWidth(EditWidth);
                                    m_TowerBottomRadius->setMinimumWidth(EditWidth);
                                    miniHBox = new QHBoxLayout ();
                                    miniHBox->addStretch();
                                    miniHBox->addWidget(m_TowerBottomRadius);
                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                    label = new QLabel (tr("Tower drag coefficient [-]:"));
                                    grid->addWidget(label, gridRowCount, 0);
                                    m_TowerDragCoeff = new NumberEdit ();
                                    m_TowerDragCoeff->setToolTip(tr("Tower drag coefficient for tower wake computation"));
                                    m_TowerDragCoeff->setMaximumWidth(EditWidth);
                                    m_TowerDragCoeff->setMinimumWidth(EditWidth);
                                    miniHBox = new QHBoxLayout ();
                                    miniHBox->addStretch();
                                    miniHBox->addWidget(m_TowerDragCoeff);
                                    grid->addLayout(miniHBox, gridRowCount++, 1);


                                    vBox->addStretch();

                                    m_errorLabel = new QLabel ("");
                                    vBox->addWidget(m_errorLabel);

                                    /* the output section tab */
                                    widget = new QWidget ();
                                    tabWidget->addTab(widget, "Output, Algorithm and Wake Parameters");
                                            hBox = new QHBoxLayout ();
                                            widget->setLayout(hBox);

                                                            vBox = new QVBoxLayout;
                                                            hBox->addLayout(vBox);

                                                            groupBox = new QGroupBox ("Wake Convection");
                                                            vBox->addWidget(groupBox);
                                                                grid = new QGridLayout ();
                                                                groupBox->setLayout(grid);
                                                                    gridRowCount = 0;

                                                                    label = new QLabel (tr("Velocity Evaluation (Vortex):"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_EvalCenterGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("CENTER");
                                                                        m_EvalCenterGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("NODE");
                                                                        m_EvalCenterGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);

                                                                        label = new QLabel (tr("Velocity Integration:"));
                                                                        grid->addWidget (label, gridRowCount, 0);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                            miniHBox->addStretch();
                                                                            m_IntegrationSchemeGroup = new QButtonGroup(miniHBox);
                                                                            radioButton = new QRadioButton ("EF");
                                                                            m_IntegrationSchemeGroup->addButton(radioButton, 0);
                                                                            miniHBox->addWidget(radioButton);
                                                                            radioButton = new QRadioButton ("PC");
                                                                            m_IntegrationSchemeGroup->addButton(radioButton, 1);
                                                                            miniHBox->addWidget(radioButton);
                                                                            radioButton = new QRadioButton ("PC2B");
                                                                            m_IntegrationSchemeGroup->addButton(radioButton, 2);
                                                                            miniHBox->addWidget(radioButton);





                                                            groupBox = new QGroupBox ("Wake Modeling");
                                                            vBox->addWidget(groupBox);
                                                                grid = new QGridLayout ();
                                                                groupBox->setLayout(grid);
                                                                    gridRowCount = 0;

                                                                    label = new QLabel (tr("Wake Rollup:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_WakeRollupGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On");
                                                                        m_WakeRollupGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off");
                                                                        m_WakeRollupGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);

                                                                    label = new QLabel (tr("Include Trailing Vortices:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_TrailGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On");
                                                                        m_TrailGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off");
                                                                        m_TrailGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);

                                                                    label = new QLabel (tr("Include Shed Vortices:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_ShedGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On");
                                                                        m_ShedGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off");
                                                                        m_ShedGroup->addButton(radioButton, 1);
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

                                                                    label = new QLabel (tr("Count wake in:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_wakeCounterGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("Revolutions");
                                                                        m_wakeCounterGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Timesteps");
                                                                        m_wakeCounterGroup->addButton(radioButton, 2);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Time");
                                                                        m_wakeCounterGroup->addButton(radioButton, 3);
                                                                        miniHBox->addWidget(radioButton);
                                                                        connect(m_wakeCounterGroup,SIGNAL(buttonToggled(int,bool)), this, SLOT(OnWakeCountChanged()));


                                                                    m_maxWakeAgeLabel = new QLabel (tr("Max Wake Age in revolutions [-]:"));
                                                                    grid->addWidget(m_maxWakeAgeLabel, gridRowCount, 0);
                                                                    m_maxWakeAge = new NumberEdit ();
                                                                    m_maxWakeAge->setToolTip(tr("The number of rotor revolutions after which a wake vortex element (after released from the TE) is removed from the domain"));
                                                                    m_maxWakeAge->setMaximumWidth(EditWidth);
                                                                    m_maxWakeAge->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_maxWakeAge);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    m_fullWakeRevsLabel = new QLabel (tr("Full Wake in revolutions [-]:"));
                                                                    grid->addWidget(m_fullWakeRevsLabel, gridRowCount, 0);
                                                                    m_fullWakeRevs = new NumberEdit ();
                                                                    m_fullWakeRevs->setToolTip(tr("The number of rotor revolutions after which the wake is concentrated into the main vortices"));
                                                                    m_fullWakeRevs->setMaximumWidth(EditWidth);
                                                                    m_fullWakeRevs->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_fullWakeRevs);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    m_fineWakeRevsLabel = new QLabel (tr("Fine Wake in Revolutions [-]:"));
                                                                    if (!m_bisVawt) grid->addWidget(m_fineWakeRevsLabel, gridRowCount, 0);
                                                                    m_fineWakeRevs = new NumberEdit ();
                                                                    m_fineWakeRevs->setToolTip(tr("The number of Rotor revolutions after which the concentrated wake is discetized coarser"));
                                                                    m_fineWakeRevs->setMaximumWidth(EditWidth);
                                                                    m_fineWakeRevs->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_fineWakeRevs);
                                                                    if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Wake Thin Factor [-]:"));
                                                                    if (!m_bisVawt) grid->addWidget(label, gridRowCount, 0);
                                                                    m_thinFactor = new NumberEdit ();
                                                                    m_thinFactor->setToolTip(tr("The factor by which the wake is thinned out after the number of 'Fine Wake Revolutions'"));
                                                                    m_thinFactor->setMaximumWidth(EditWidth);
                                                                    m_thinFactor->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_thinFactor);
                                                                    m_thinFactor->setAutomaticPrecision(0);
                                                                    m_thinFactor->setMinimum(1);
                                                                    if (!m_bisVawt) grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    if (!m_bisVawt) label = new QLabel (tr("Min Gamma Factor [-]:"));
                                                                    else label = new QLabel (tr("Wake Reduction Factor [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_minGammaFrac = new NumberEdit ();
                                                                    m_minGammaFrac->setToolTip(tr("The fraction of the max Gamma that will be lumped during wake discretization"));
                                                                    m_minGammaFrac->setMaximumWidth(EditWidth);
                                                                    m_minGammaFrac->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_minGammaFrac);
                                                                     grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    label = new QLabel (tr("First Wake Row Length Fraction [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_firstWakeRowLength = new NumberEdit ();
                                                                    m_firstWakeRowLength->setToolTip(tr("The fraction to shorten the length of the first wake row"));
                                                                    m_firstWakeRowLength->setMaximumWidth(EditWidth);
                                                                    m_firstWakeRowLength->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_firstWakeRowLength);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    label = new QLabel (tr("Bound Vortex at % of Chord [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_BoundVortexPos = new NumberEdit ();
                                                                    m_BoundVortexPos->setToolTip(tr("The fraction to shorten the length of the first wake row"));
                                                                    m_BoundVortexPos->setMaximumWidth(EditWidth);
                                                                    m_BoundVortexPos->setMinimumWidth(EditWidth);
                                                                    m_BoundVortexPos->setMaximum(1.0);
                                                                    m_BoundVortexPos->setMinimum(0.0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_BoundVortexPos);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    label = new QLabel (tr("Calc AoA at % of Chord [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_AoAPos = new NumberEdit ();
                                                                    m_AoAPos->setToolTip(tr("The fraction to shorten the length of the first wake row"));
                                                                    m_AoAPos->setMaximumWidth(EditWidth);
                                                                    m_AoAPos->setMinimumWidth(EditWidth);
                                                                    m_AoAPos->setMaximum(1.0);
                                                                    m_AoAPos->setMinimum(0.0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_AoAPos);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    groupBox = new QGroupBox ("Vortex Modeling");
                                                                    vBox->addWidget(groupBox);
                                                                        grid = new QGridLayout ();
                                                                        groupBox->setLayout(grid);
                                                                            gridRowCount = 0;

                                                                    label = new QLabel (tr("Desingularization Type:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_desingGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("van Garrel ");
                                                                        m_desingGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Leishman ");
                                                                        m_desingGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);
                                                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnDesingChanged()));



                                                                    label = new QLabel (tr("Leishman z value [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_z_leishmanEdit = new NumberEdit ();
                                                                    m_z_leishmanEdit->setToolTip(tr("Derfines the z-exponent for the Leishman viscous parameter Kv"));
                                                                    m_z_leishmanEdit->setMaximumWidth(EditWidth);
                                                                    m_z_leishmanEdit->setMinimumWidth(EditWidth);
                                                                    m_z_leishmanEdit->setMinimum(1);
                                                                    m_z_leishmanEdit->setMaximum(2);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_z_leishmanEdit);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    label = new QLabel (tr("Vortex Time Offset [-]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_vortexTimeOffset = new NumberEdit ();
                                                                    m_vortexTimeOffset->setToolTip(tr("Vortex time offset to prevent zero core size during vortex emission"));
                                                                    m_vortexTimeOffset->setMaximumWidth(EditWidth);
                                                                    m_vortexTimeOffset->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_vortexTimeOffset);
                                                                    connect(m_vortexTimeOffset, SIGNAL(valueChanged(double)),this, SLOT(OnVortSizeChanged()));
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    label = new QLabel ("Turbulent Vortex Viscosity [-]:");
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_turbulentViscosity = new NumberEdit (NumberEdit::Scientific);
                                                                    m_turbulentViscosity->setMaximumWidth(EditWidth);
                                                                    m_turbulentViscosity->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_turbulentViscosity);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                    connect(m_turbulentViscosity, SIGNAL(valueChanged(double)),this, SLOT(OnVortSizeChanged()));

                                                                    label = new QLabel (tr("Include vortex strain:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_strainGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On ");
                                                                        m_strainGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off ");
                                                                        m_strainGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);
                                                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnStrainChanged()));

                                                                    label = new QLabel ("Maximum Vortex Stretching Factor [-]:");
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_maxStretch = new NumberEdit (NumberEdit::Scientific);
                                                                    m_maxStretch->setMaximumWidth(EditWidth);
                                                                    m_maxStretch->setMinimumWidth(EditWidth);
                                                                    m_maxStretch->setMinimum(2);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_maxStretch);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    label = new QLabel ("Initial Core Size [m]:");
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_vortSizeLabel = new QLabel();
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_vortSizeLabel);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    label = new QLabel ("Induced V at core [m/s]:");
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_vortInductionLabel = new QLabel();
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_vortInductionLabel);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    vBox->addStretch();


                                                                    vBox = new QVBoxLayout;
                                                                    hBox->addLayout(vBox);

                                                                    groupBox = new QGroupBox ("Unsteady Aerodynamics Model");
                                                                    vBox->addWidget(groupBox);
                                                                    grid = new QGridLayout ();
                                                                    groupBox->setLayout(grid);
                                                                        gridRowCount = 0;
                                                                    label = new QLabel (tr("Activate UA Model: "));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_DSGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On");
                                                                        m_DSGroup->addButton(radioButton, 0);
                                                                        connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnIncludeDSChanged()));
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off");
                                                                        m_DSGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);

                                                                        label = new QLabel (tr("Activate Vortex Lift: "));
                                                                        grid->addWidget (label, gridRowCount, 0);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                            miniHBox->addStretch();
                                                                            m_vortexLiftGroup = new QButtonGroup(miniHBox);
                                                                            radioButton = new QRadioButton ("On");
                                                                            m_vortexLiftGroup->addButton(radioButton, 0);
                                                                            connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnIncludeDSChanged()));
                                                                            miniHBox->addWidget(radioButton);
                                                                            radioButton = new QRadioButton ("Off");
                                                                            m_vortexLiftGroup->addButton(radioButton, 1);
                                                                            miniHBox->addWidget(radioButton);
                                                                            m_vortexLiftGroup->button(0)->setEnabled(false);
                                                                            m_vortexLiftGroup->button(1)->setEnabled(false);

                                                                        label = new QLabel (tr("Time constant Tf [-]:"));
                                                                        grid->addWidget(label, gridRowCount, 0);
                                                                        m_Tf = new NumberEdit ();
                                                                        m_Tf->setToolTip(tr("Boundary layer lag for Beddoes-Leishman dynamic stall model"));
                                                                        m_Tf->setMaximumWidth(EditWidth);
                                                                        m_Tf->setMinimumWidth(EditWidth);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        miniHBox->addStretch();
                                                                        miniHBox->addWidget(m_Tf);
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                        label = new QLabel (tr("Time constant Tp [-]:"));
                                                                        grid->addWidget(label, gridRowCount, 0);
                                                                        m_Tp = new NumberEdit ();
                                                                        m_Tp->setToolTip(tr("Peak pressure lag for Beddoes-Leishman dynamic stall model"));
                                                                        m_Tp->setMaximumWidth(EditWidth);
                                                                        m_Tp->setMinimumWidth(EditWidth);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        miniHBox->addStretch();
                                                                        miniHBox->addWidget(m_Tp);
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                        label = new QLabel (tr("Time constant Tv [-]:"));
                                                                        grid->addWidget(label, gridRowCount, 0);
                                                                        m_Tv = new NumberEdit ();
                                                                        m_Tv->setToolTip(tr("LE vortex decay for Beddoes-Leishman dynamic stall model"));
                                                                        m_Tv->setMaximumWidth(EditWidth);
                                                                        m_Tv->setMinimumWidth(EditWidth);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        miniHBox->addStretch();
                                                                        miniHBox->addWidget(m_Tv);
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                        label = new QLabel (tr("Time constant Tvl [-]:"));
                                                                        grid->addWidget(label, gridRowCount, 0);
                                                                        m_Tvl = new NumberEdit ();
                                                                        m_Tvl->setToolTip(tr("Vortex lift time constant for Beddoes-Leishman dynamic stall model"));
                                                                        m_Tvl->setMaximumWidth(EditWidth);
                                                                        m_Tvl->setMinimumWidth(EditWidth);
                                                                        miniHBox = new QHBoxLayout ();
                                                                        miniHBox->addStretch();
                                                                        miniHBox->addWidget(m_Tvl);
                                                                        grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    groupBox = new QGroupBox ("Special Testcases");
                                                                    vBox->addWidget(groupBox);
                                                                        grid = new QGridLayout ();
                                                                        groupBox->setLayout(grid);
                                                                            gridRowCount = 0;

                                                                            label = new QLabel (tr("Rotor Standstill:"));
                                                                            grid->addWidget (label, gridRowCount, 0);
                                                                            miniHBox = new QHBoxLayout ();
                                                                            grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                                miniHBox->addStretch();
                                                                                m_RotorStandstillGroup = new QButtonGroup(miniHBox);
                                                                                radioButton = new QRadioButton ("On");
                                                                                m_RotorStandstillGroup->addButton(radioButton, 0);
                                                                                miniHBox->addWidget(radioButton);
                                                                                radioButton = new QRadioButton ("Off");
                                                                                m_RotorStandstillGroup->addButton(radioButton, 1);
                                                                                miniHBox->addWidget(radioButton);
                                                                                connect(m_RotorStandstillGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnStandStillChanged()));

                                                                    label = new QLabel (tr("Simulate Startup:"));
                                                                    grid->addWidget (label, gridRowCount, 0);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                        miniHBox->addStretch();
                                                                        m_startupGroup = new QButtonGroup(miniHBox);
                                                                        radioButton = new QRadioButton ("On");
                                                                        m_startupGroup->addButton(radioButton, 0);
                                                                        miniHBox->addWidget(radioButton);
                                                                        radioButton = new QRadioButton ("Off");
                                                                        m_startupGroup->addButton(radioButton, 1);
                                                                        miniHBox->addWidget(radioButton);
                                                                    connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(OnStartupChanged()));

                                                                    label = new QLabel (tr("Moment of (rotational) Inertia [kg*m^2]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_momInertia = new NumberEdit ();
                                                                    m_momInertia->setToolTip(tr("Define a value for J"));
                                                                    m_momInertia->setMaximumWidth(EditWidth);
                                                                    m_momInertia->setMinimumWidth(EditWidth);
                                                                    m_momInertia->setMinimum(0.001);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_momInertia);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);

                                                                    label = new QLabel (tr("Initial rotor speed [rpm]:"));
                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                    m_initRot = new NumberEdit ();
                                                                    m_initRot->setToolTip(tr("Defines the initial rotor speed"));
                                                                    m_initRot->setMaximumWidth(EditWidth);
                                                                    m_initRot->setMinimumWidth(EditWidth);
                                                                    miniHBox = new QHBoxLayout ();
                                                                    miniHBox->addStretch();
                                                                    miniHBox->addWidget(m_initRot);
                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);


                                                                    groupBox = new QGroupBox ("Output Parameters");
                                                                    vBox->addWidget(groupBox);
                                                                        grid = new QGridLayout ();
                                                                        groupBox->setLayout(grid);
                                                                            gridRowCount = 0;
                                                                            label = new QLabel (tr("Starting time to store output [s]:"));
                                                                            grid->addWidget(label, gridRowCount, 0);
                                                                            m_TStart = new NumberEdit ();
                                                                            m_TStart->setToolTip(tr("Time from which output is stored:"));
                                                                            m_TStart->setMaximumWidth(EditWidth);
                                                                            m_TStart->setMinimumWidth(EditWidth);
                                                                            miniHBox = new QHBoxLayout ();
                                                                            miniHBox->addStretch();
                                                                            miniHBox->addWidget(m_TStart);
                                                                            grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                            label = new QLabel ("Output decimation factor:");
                    //                                                        grid->addWidget(label, gridRowCount, 0);
                                                                            m_DecFactor = new NumberEdit ();
                                                                            m_DecFactor->setToolTip(tr("Only every n'th timestep the ourput is stored"));
                                                                            m_DecFactor->setMaximumWidth(EditWidth);
                                                                            m_DecFactor->setMinimumWidth(EditWidth);
                                                                            m_DecFactor->setAutomaticPrecision(0);
                    //                                                        grid->addWidget(m_DecFactor, gridRowCount++, 1);


                                                                    groupBox = new QGroupBox ("Algorithm Parameters");
                                                                    vBox->addWidget(groupBox);
                                                                        grid = new QGridLayout ();
                                                                        groupBox->setLayout(grid);
                                                                            gridRowCount = 0;
                                                                            label = new QLabel (tr("Max. number of Iterations [-]:"));
                                                                            grid->addWidget(label, gridRowCount, 0);
                                                                            m_maxIterations = new NumberEdit ();
                                                                            m_maxIterations->setToolTip(tr("Maximum Number of Iterations:"));
                                                                            m_maxIterations->setMaximumWidth(EditWidth);
                                                                            m_maxIterations->setMinimumWidth(EditWidth);
                                                                            miniHBox = new QHBoxLayout ();
                                                                            miniHBox->addStretch();
                                                                            miniHBox->addWidget(m_maxIterations);
                                                                            grid->addLayout(miniHBox, gridRowCount++, 1);                                                        label = new QLabel ("Relaxation Factor [-]:");
                                                                            grid->addWidget(label, gridRowCount, 0);
                                                                            m_relaxationFactor = new NumberEdit ();
                                                                            m_relaxationFactor->setToolTip(tr("Relaxation Factor (between 0 and 1)"));
                                                                            m_relaxationFactor->setMaximumWidth(EditWidth);
                                                                            m_relaxationFactor->setMinimumWidth(EditWidth);
                                                                            miniHBox = new QHBoxLayout ();
                                                                            miniHBox->addStretch();
                                                                            miniHBox->addWidget(m_relaxationFactor);
                                                                            grid->addLayout(miniHBox, gridRowCount++, 1);                                                        label = new QLabel ("Epsilon [-]:");
                                                                            grid->addWidget(label, gridRowCount, 0);
                                                                            m_Epsilon = new NumberEdit ();
                                                                            m_Epsilon->setToolTip(tr("Epsilon for convergence of the LLT algorithm (convergence variable is the Circulation)"));
                                                                            m_Epsilon->setMaximumWidth(EditWidth);
                                                                            m_Epsilon->setMinimumWidth(EditWidth);
                                                                            miniHBox = new QHBoxLayout ();
                                                                            miniHBox->addStretch();
                                                                            miniHBox->addWidget(m_Epsilon);
                                                                            grid->addLayout(miniHBox, gridRowCount++, 1);

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
                                                                                    miniHBox = new QHBoxLayout ();
                                                                                    miniHBox->addStretch();
                                                                                    miniHBox->addWidget(m_airDensEdit);
                                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);                                                                label = new QLabel ("Kinematic Viscosity [m^2/s]: ");
                                                                                    grid->addWidget(label, gridRowCount, 0);
                                                                                    m_kinViscEdit = new NumberEdit (NumberEdit::Scientific);
                                                                                    m_kinViscEdit->setMaximumWidth(EditWidth);
                                                                                    m_kinViscEdit->setMinimumWidth(EditWidth);
                                                                                    miniHBox = new QHBoxLayout ();
                                                                                    miniHBox->addStretch();
                                                                                    miniHBox->addWidget(m_kinViscEdit);
                                                                                    connect(m_kinViscEdit, SIGNAL(valueChanged(double)),this, SLOT(OnVortSizeChanged()));

                                                                                    grid->addLayout(miniHBox, gridRowCount++, 1);
                                                                                vBox->addStretch();

    initView();
}

void QLLTCreatorDialog::initView() {
    if (m_editedLLTSimulation) {  // load parameters
        m_nameEdit->setText(m_editedLLTSimulation->getName());
        m_WindInputGroup->button(m_editedLLTSimulation->getWindInputType())->setChecked(true);
        m_timeStepEdit->setValue(m_editedLLTSimulation->getTimeStep());
        m_InflowSpeed->setValue(m_editedLLTSimulation->getInflowSpeed());
        if (m_editedLLTSimulation->getUsedWindField()){
            m_windFieldComboBox->setCurrentObject(m_editedLLTSimulation->getUsedWindField());
            m_useWindfieldGroup->button(0)->setChecked(true);
        }
        else m_useWindfieldGroup->button(1)->setChecked(true);
        if (m_editedLLTSimulation->isStandStill()){
        if (m_bisVawt)  m_rotorSpeedEdit->setValue(3/m_editedLLTSimulation->getUsedBlade()->m_MaxRadius/2/2/PI*getMeanWindSpeed()*60);
        if (!m_bisVawt) m_rotorSpeedEdit->setValue(7/m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/2/PI*getMeanWindSpeed()*60);
        if (m_bisVawt)  m_aziStepEdit->setValue(5);
        if (!m_bisVawt) m_aziStepEdit->setValue(10);
        }
        else{
        m_rotorSpeedEdit->setValue(m_editedLLTSimulation->getRotorSpeed());
        m_aziStepEdit->setValue(m_editedLLTSimulation->getAziDisc());
        }
        OnRotationalSpeedChanged(); //init the lineedits

        if (m_editedLLTSimulation->get_z_Leishman() == -1) m_desingGroup->button(0)->setChecked(true);
        else {
            m_desingGroup->button(1)->setChecked(true);
            m_z_leishmanEdit->setEnabled(false);
        }
        m_shaftTilt->setValue(m_editedLLTSimulation->getShaftTiltAngle());
        m_nacelleYawEdit->setValue(m_editedLLTSimulation->getYawAngle());
        m_horInflowAngleEdit->setValue(m_editedLLTSimulation->getHorInflowAngle());
        m_VertInflowSpeedEdit->setValue(m_editedLLTSimulation->getvertinflowSpeed());
        m_airDensEdit->setValue(m_editedLLTSimulation->getDensity());
        m_kinViscEdit->setValue(m_editedLLTSimulation->getKinVisc());
        m_turbulentViscosity->setValue(m_editedLLTSimulation->getTurbVisc());
        m_maxStretch->setValue(m_editedLLTSimulation->getStretch());
        m_maxWakeAge->setValue(m_editedLLTSimulation->getMaxWakeRevolutions());
        m_firstWakeRowLength->setValue(m_editedLLTSimulation->getFirstWakeRowLength());
        m_fullWakeRevs->setValue(m_editedLLTSimulation->getFullWakeRevolutions());
        m_fineWakeRevs->setValue(m_editedLLTSimulation->getFineWakeRevolutions());
        m_thinFactor->setValue(m_editedLLTSimulation->getThinFactor());
        m_vortexTimeOffset->setValue(m_editedLLTSimulation->getVortexTimeOffset());
        m_Epsilon->setValue(m_editedLLTSimulation->getEpsilon());
        m_relaxationFactor->setValue(m_editedLLTSimulation->getRelaxFactor());
        m_maxIterations->setValue(m_editedLLTSimulation->getMaxIterations());
        m_aziStart->setValue(m_editedLLTSimulation->getAziStart());
        m_BoundVortexPos->setValue(m_editedLLTSimulation->getBoundVortexPos());
        m_AoAPos->setValue(m_editedLLTSimulation->getAoAPos());
        m_strainGroup->button(0)->setChecked(m_editedLLTSimulation->getIncludeStrain());
        m_strainGroup->button(1)->setChecked(!m_editedLLTSimulation->getIncludeStrain());
        if (m_editedLLTSimulation->get_z_Leishman() == -1){
            m_z_leishmanEdit->setValue(2);
            m_z_leishmanEdit->setEnabled(false);
        }
        else{
            m_z_leishmanEdit->setValue(m_editedLLTSimulation->get_z_Leishman());
            m_z_leishmanEdit->setEnabled(true);
        }
        m_TowerTopRadius->setValue(m_editedLLTSimulation->getTowerTopRadius());
        m_TowerBottomRadius->setValue(m_editedLLTSimulation->getTowerBottomRadius());
        m_TowerDragCoeff->setValue(m_editedLLTSimulation->getTowerDrag());
        m_RotorOverhang->setValue(m_editedLLTSimulation->getOverhang());
        m_HubHeight->setValue(m_editedLLTSimulation->getHubHeight());
        m_rotorComboBox->setCurrentObject(m_editedLLTSimulation->getUsedBlade());
        m_TowerShadowGroup->button((m_editedLLTSimulation->getIncludeTower() ? 0 : 1))->setChecked(true);
        m_GroundEffectGroup->button((m_editedLLTSimulation->getIncludeGround() ? 0 : 1))->setChecked(true);
        m_HimmelskampGroup->button((m_editedLLTSimulation->getIncludeHimmelskamp() ? 0 : 1))->setChecked(true);
        m_ShedGroup->button((m_editedLLTSimulation->getShed() ? 0 : 1))->setChecked(true);
        m_TrailGroup->button((m_editedLLTSimulation->getTrail() ? 0 : 1))->setChecked(true);
        m_WakeRollupGroup->button((m_editedLLTSimulation->getWakeRollup() ? 0 : 1))->setChecked(true);
        m_TurbConvectionGroup->button((m_editedLLTSimulation->getTurbConvection() ? 0 : 1))->setChecked(true);
        m_storeWakePositions->button((m_editedLLTSimulation->getStoreWake() ? 0 : 1))->setChecked(true);
        m_totalRunTimeEdit->setValue(m_editedLLTSimulation->getTimeStep()*m_editedLLTSimulation->getNumberTimesteps());
        m_XRollEdit->setValue(m_editedLLTSimulation->getXRoll());
        m_YRollEdit->setValue(m_editedLLTSimulation->getYRoll());
        m_TowerHeight->setValue(m_editedLLTSimulation->getTowerHeight());
        m_TStart->setValue(m_editedLLTSimulation->getTStart());
        m_DecFactor->setValue(m_editedLLTSimulation->getDecFactor());
        m_RotorStandstillGroup->button((m_editedLLTSimulation->isStandStill() ? 0 : 1))->setChecked(true);
        m_vortexLiftGroup->button((m_editedLLTSimulation->getisVortexLift() ? 0 : 1))->setChecked(true);
        m_Nth_WakeStep->setValue(m_editedLLTSimulation->getWakeCoarsening());
        m_wakeCounterGroup->button(m_editedLLTSimulation->getWakeCounter())->setChecked(true);


        m_ShearCoefficientEdit->setValue(m_editedLLTSimulation->getShearCoeff());
        m_minGammaFrac->setValue(m_editedLLTSimulation->getGammaFrac());
        m_ViewFile->setEnabled(false);

        if (m_editedLLTSimulation->getisPC2B()) m_IntegrationSchemeGroup->button(2)->setChecked(true);
        else if(m_editedLLTSimulation->getisPC())  m_IntegrationSchemeGroup->button(1)->setChecked(true);
        else m_IntegrationSchemeGroup->button(0)->setChecked(true);

        m_EvalCenterGroup->button((m_editedLLTSimulation->getEvalPoint() ? 0 : 1))->setChecked(true);
        m_numPanels->setValue(m_editedLLTSimulation->getNumPanels());
        m_discTypeGroup->button(m_editedLLTSimulation->getDiscType())->setChecked(true);
        m_ConeAngle->setValue(m_editedLLTSimulation->getConeAngle());
        m_upWindGroup->button((m_editedLLTSimulation->getisUpWind() ? 0 : 1))->setChecked(true);
        m_startupGroup->button((m_editedLLTSimulation->getisStartup() ? 0 : 1))->setChecked(true);
        m_adaptiveGroup->button((m_editedLLTSimulation->getAdaptive() ? 0 : 1))->setChecked(true);

        m_initRot->setValue(m_editedLLTSimulation->getInitRot());
        m_momInertia->setValue(m_editedLLTSimulation->getMomInertia());
        m_numStrutPanels->setValue(m_editedLLTSimulation->getNumStrutPanels());
        m_strutGroup->button(0)->setChecked(!m_editedLLTSimulation->getCircStrut());
        m_strutGroup->button(1)->setChecked(m_editedLLTSimulation->getCircStrut());


        m_DSGroup->button((m_editedLLTSimulation->getIncludeDS() ? 0 : 1))->setChecked(true);
        m_Tf->setValue(m_editedLLTSimulation->getTf());
        m_Tp->setValue(m_editedLLTSimulation->getTp());
        m_Tv->setValue(m_editedLLTSimulation->getTv());
        m_Tvl->setValue(m_editedLLTSimulation->getTvl());
        m_Tp->setEnabled(m_editedLLTSimulation->getIncludeDS());
        m_Tf->setEnabled(m_editedLLTSimulation->getIncludeDS());
        m_Tv->setEnabled(m_editedLLTSimulation->getIncludeDS());
        m_Tvl->setEnabled(m_editedLLTSimulation->getIncludeDS());


        if (m_editedLLTSimulation->getisStartup()){
            m_momInertia->setEnabled(true);
            m_initRot->setEnabled(true);
        }
        else{
            m_momInertia->setEnabled(false);
            m_initRot->setEnabled(false);
        }


        if (m_editedLLTSimulation->getWindInputType() == CONSTANT){
            if (!m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/m_editedLLTSimulation->getInflowSpeed()/60);
            if (m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->m_MaxRadius/m_editedLLTSimulation->getInflowSpeed()/60);
        }

        else if (m_editedLLTSimulation->getWindInputType() == SIMFILE){
            if (!m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/(10)/60);
            if (m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->m_MaxRadius/(10)/60);
            m_SimFileLabel->setText(m_editedLLTSimulation->getFileName());
            m_ViewFile->setEnabled(true);
        }
        else if (m_editedLLTSimulation->getWindInputType() == AERODYN){
            if (!m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/(10)/60);
            if (m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->m_MaxRadius/(10)/60);
            m_AeroDynLabel->setText(m_editedLLTSimulation->getFileName());
            m_ViewFile->setEnabled(true);
        }
        if (m_useWindfieldGroup->button(0)->isChecked()){
            if (!m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->getRotorRadius()/m_editedLLTSimulation->getUsedWindField()->getMeanWindSpeedAtHub()/60);
            if (m_bisVawt) m_TSREdit->setValue(m_editedLLTSimulation->getRotorSpeed()*2*PI*m_editedLLTSimulation->getUsedBlade()->m_MaxRadius/m_editedLLTSimulation->getUsedWindField()->getMeanWindSpeedAtHub()/60);
        }
        m_ShearGroup->button(1)->setChecked(!m_editedLLTSimulation->getIsPowerLaw());
        m_ShearGroup->button(0)->setChecked(m_editedLLTSimulation->getIsPowerLaw());
        m_numberTimesteps->setValue(m_editedLLTSimulation->getRunTime()/m_editedLLTSimulation->getTimeStep());
        m_numberRevs->setValue(m_numberTimesteps->getValue()*m_aziStepEdit->getValue()/360);
        m_TowerTopRadius->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_TowerBottomRadius->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_TowerDragCoeff->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_TowerHeight->setEnabled(m_editedLLTSimulation->getIncludeTower());
        m_SimParameters = m_editedLLTSimulation->getSimParameters();
        m_refHeightEdit->setValue(m_editedLLTSimulation->getRefHeight());
        m_FileHeader = m_editedLLTSimulation->getFileHeader();
        if (!g_windFieldStore.size()) m_useWindfieldGroup->button(0)->setEnabled(false);
        OnWindTypeChanged();
        if (m_editedLLTSimulation->getAdaptive()) {
            m_aziStepEdit->setValue(m_editedLLTSimulation->getAziDisc());
            OnAziChanged();}

        strutLab1->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
        strutLab2->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
        m_strutGroup->button(0)->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
        m_strutGroup->button(1)->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
        m_numStrutPanels->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());

    } else {  // set default parameters
        QString newname("New LLT Simulation");
        if (m_bisVawt){
            for (int i=0;i<g_QLLTVAWTSimulationStore.size();i++){
                if (newname == g_QLLTVAWTSimulationStore.at(i)->getName()){
                    newname = makeNameWithHigherNumber(newname);
                    i = 0;
                }
            }}
        else{
            for (int i=0;i<g_QLLTHAWTSimulationStore.size();i++){
                if (newname == g_QLLTHAWTSimulationStore.at(i)->getName()){
                    newname = makeNameWithHigherNumber(newname);
                    i = 0;
                }
        }}
        m_Nth_WakeStep->setValue(1);
        m_desingGroup->button(0)->setChecked(true);
        m_z_leishmanEdit->setEnabled(false);
        m_nameEdit->setText(newname);
        if (!m_bisVawt) m_aziStepEdit->setValue(10);
        if (m_bisVawt) m_aziStepEdit->setValue(5);
        if (!m_bisVawt) m_TSREdit->setValue(7);
        if (m_bisVawt) m_TSREdit->setValue(3);
        m_shaftTilt->setValue(0);
        m_nacelleYawEdit->setValue(0);
        m_YRollEdit->setValue(0);
        m_XRollEdit->setValue(0);
        m_horInflowAngleEdit->setValue(0);
        m_refHeightEdit->setValue(30);
        m_VertInflowSpeedEdit->setValue(0);
        m_airDensEdit->setValue(1.225);
        m_kinViscEdit->setValue(1.6468e-5);
        m_turbulentViscosity->setValue(40);
        m_maxStretch->setValue(10e5);
        m_maxWakeAge->setValue(8);
        m_firstWakeRowLength->setValue(0.3);
        if (!m_bisVawt) m_fullWakeRevs->setValue(0.5);
        else m_fullWakeRevs->setValue(0.0);
        m_fineWakeRevs->setValue(4);
        m_thinFactor->setValue(2);
        m_vortexTimeOffset->setValue(10);
        m_ShearGroup->button(0)->setChecked(true);
        m_strainGroup->button(0)->setChecked(true);
        m_Epsilon->setValue(0.0005);
        m_relaxationFactor->setValue(0.4);
        m_maxIterations->setValue(100);
        m_z_leishmanEdit->setValue(2.0);
        m_TowerTopRadius->setValue(1.0);
        m_TowerBottomRadius->setValue(3.0);
        m_TowerDragCoeff->setValue(0.5);
        m_BoundVortexPos->setValue(0.25);
        m_wakeCounterGroup->button(REVOLUTIONS)->setChecked(true);
        m_AoAPos->setValue(0.25);
        m_RotorOverhang->setValue(m_rotorComboBox->currentObject()->getRotorRadius()/6);
        m_HubHeight->setValue(m_rotorComboBox->currentObject()->getRotorRadius()*2);
        m_TowerHeight->setValue(m_rotorComboBox->currentObject()->getRotorRadius()*2);
        m_numberRevs->setValue(10);
        m_TowerShadowGroup->button(1)->setChecked(true);
        m_GroundEffectGroup->button(1)->setChecked(true);
        m_HimmelskampGroup->button(1)->setChecked(true);
        m_HimmelskampGroup->button(1)->setChecked(true);
        m_ShedGroup->button(0)->setChecked(true);
        m_TrailGroup->button(0)->setChecked(true);
        m_WakeRollupGroup->button(0)->setChecked(true);
        m_TurbConvectionGroup->button(1)->setChecked(true);
        m_storeWakePositions->button(1)->setChecked(true);
        m_useWindfieldGroup->button(1)->setChecked(true);
        m_IntegrationSchemeGroup->button(0)->setChecked(true);
        m_EvalCenterGroup->button(0)->setChecked(true);
        m_InflowSpeed->setValue(12);
        m_TStart->setValue(0);
        m_DecFactor->setValue(1);
        m_ConeAngle->setValue(0);
        m_RotorStandstillGroup->button(1)->setChecked(true);
        m_numberTimesteps->setValue(m_numberRevs->getValue()*360/m_aziStepEdit->getValue());
        if (m_bisVawt) m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->m_MaxRadius/2/PI*getMeanWindSpeed()*60);
        else m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->getRotorRadius()/2/PI*getMeanWindSpeed()*60);
        if (!g_windFieldStore.size()) m_useWindfieldGroup->button(0)->setEnabled(false);
        m_timeStepEdit->setValue(m_aziStepEdit->getValue()/360*60/m_rotorSpeedEdit->getValue());
        m_totalRunTimeEdit->setValue(m_timeStepEdit->getValue()*m_numberTimesteps->getValue());
        m_ShearCoefficientEdit->setValue(0);
        if (!m_bisVawt) m_minGammaFrac->setValue(0.4);
        else  m_minGammaFrac->setValue(0.3);
        m_numPanels->setValue(15);
        m_discTypeGroup->button(0)->setChecked(true);
        m_upWindGroup->button(0)->setChecked(true);
        m_TowerTopRadius->setEnabled(false);
        m_TowerBottomRadius->setEnabled(false);
        m_TowerDragCoeff->setEnabled(false);
        m_TowerHeight->setEnabled(false);
        m_ViewFile->setEnabled(false);
        if (m_bisVawt)m_aziStart->setValue(270);
        else m_aziStart->setValue(0);
        m_momInertia->setEnabled(false);
        m_initRot->setEnabled(false);
        m_momInertia->setValue(10);
        m_initRot->setValue(1);
        m_startupGroup->button(1)->setChecked(true);
        m_DSGroup->button(1)->setChecked(true);
        m_Tp->setEnabled(false);
        m_Tf->setEnabled(false);
        m_Tv->setEnabled(false);
        m_Tvl->setEnabled(false);
        m_Tp->setValue(1.7);
        m_Tf->setValue(3);
        m_Tv->setValue(2);
        m_Tvl->setValue(11);
        m_vortexLiftGroup->button(1)->setChecked(true);
        m_adaptiveGroup->button(1)->setChecked(true);
        m_adaptiveGroup->button(0)->setEnabled(false);
        m_adaptiveGroup->button(0)->setChecked(false);
        m_numStrutPanels->setValue(5);
        m_strutGroup->button(0)->setChecked(true);
        onWindFieldChanged();
        onRotorChanged();
    }

    OnShearChanged();
    OnVortSizeChanged();
    OnStrainChanged();

    connect(m_rotorComboBox, SIGNAL(valueChanged(int)), this, SLOT(onRotorChanged()));


}

void QLLTCreatorDialog::onWindFieldChanged() {
    if (m_windFieldComboBox->currentObject() && m_useWindfieldGroup->button(0)->isChecked()) {  // can be NULL during the call from initView()
        CBlade *rotor = m_rotorComboBox->currentObject();
        if (rotor && m_windFieldComboBox->currentObject()->getRotorRadius() < rotor->getRotorRadius() && !m_bisVawt) {
            m_errorLabel->setText(tr("<font color='red'>Windfield too small to fit the rotor cross-section!</font>"));
        }
        else if (rotor && m_bisVawt && (m_windFieldComboBox->currentObject()->getRotorRadius() < rotor->getRotorRadius()/2 ||  m_windFieldComboBox->currentObject()->getRotorRadius() < rotor->m_MaxRadius)){

        } else {
            m_errorLabel->setText("");
        }
    }
    else m_errorLabel->setText("");
    if (GetWindInputType()==SIMFILE) OnTimestepChanged();
    else OnRotationalSpeedChanged();
    OnVortSizeChanged();
}

void QLLTCreatorDialog::onRotorChanged(){
    if (!m_rotorComboBox->currentObject()) return;
    m_RotorOverhang->setValue(m_rotorComboBox->currentObject()->getRotorRadius()/6);
    m_HubHeight->setValue(m_rotorComboBox->currentObject()->getRotorRadius()*2);
    m_TowerHeight->setValue(m_rotorComboBox->currentObject()->getRotorRadius()*2);

    strutLab1->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
    strutLab2->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
    m_strutGroup->button(0)->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
    m_strutGroup->button(1)->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());
    m_numStrutPanels->setVisible(m_rotorComboBox->currentObject()->m_StrutList.size());


    OnTSRChanged();
}

double QLLTCreatorDialog::getMeanWindSpeed(){
    if (m_useWindfieldGroup->button(0)->isChecked()) return m_windFieldComboBox->currentObject()->getMeanWindSpeedAtHub();
    else return pow(pow(m_InflowSpeed->getValue(),2)+pow(m_VertInflowSpeedEdit->getValue(),2),0.5);
}

QString QLLTCreatorDialog::GetFileName(){
    if (GetWindInputType() == SIMFILE) return m_SimFileLabel->text();
    else if (GetWindInputType() == AERODYN) return m_AeroDynLabel->text();
    else return "- NONE -";
}


void QLLTCreatorDialog::onCreateButtonClicked() {
    /* check first, if all requirements are fullfilled */
    QString errorMessage ("");
    if (m_nameEdit->text() == "") {
        errorMessage.append(tr("\n - No name given"));
    }
    if (m_HubHeight->getValue() <= m_rotorComboBox->currentObject()->getRotorRadius() && !m_bisVawt) {
        errorMessage.append(tr("\n - Hub height needs to be larger than rotor radius"));
    }
    if (m_HubHeight->getValue() < 0 && m_bisVawt) {
        errorMessage.append(tr("\n - Height of rotor center needs to be larger than the rotor half height"));
    }
    if (m_errorLabel->text() != "") {
        errorMessage.append(tr("\n - Windfield is too small"));
    }
    if (GetWindInputType() == SIMFILE && !m_ViewFile->isEnabled()){
        errorMessage.append(tr("\n - No QBlade Simulation File Opened"));
    }
    if (GetWindInputType() == AERODYN && !m_ViewFile->isEnabled()){
        errorMessage.append(tr("\n - No AeroDyn Hub Height Wind File Opened"));
    }

    if (m_rotorComboBox->currentObject()->m_bisSinglePolar && m_DSGroup->button(0)->isChecked()){
        bool allgood = true;
    for (int i=0; i < m_rotorComboBox->currentObject()->m_Airfoils.size();i++){
        if (!g_qbem->Get360Polar(m_rotorComboBox->currentObject()->m_Airfoils.at(i),m_rotorComboBox->currentObject()->m_Polar.at(i))->m_bisDecomposed) allgood = false;
    }
    if (!allgood) errorMessage.append(tr("\n - Not all polars are decomposed for the UAM. Decompose polars in the 360 extrapolation module!"));
    if (!allgood) errorMessage.append(tr("\n - Unsteady aerodynamics model must be turned off..."));
    }
    if (!m_rotorComboBox->currentObject()->m_bisSinglePolar && m_DSGroup->button(0)->isChecked()){
        bool allgood = true;
    for (int i=0; i < m_rotorComboBox->currentObject()->m_MultiPolars.size();i++){
        for (int j=0; j < m_rotorComboBox->currentObject()->m_MultiPolars.at(i).size();j++){

        if (!g_qbem->Get360Polar(m_rotorComboBox->currentObject()->m_PolarAssociatedFoils.at(i),m_rotorComboBox->currentObject()->m_MultiPolars.at(i).at(j))->m_bisDecomposed) allgood = false;
        }
    }
    if (!allgood) errorMessage.append(tr("\n - Not all polars are decomposed for the UAM. Decompose polars in the 360 extrapolation module!"));
    if (!allgood) errorMessage.append(tr("\n - Unsteady aerodynamics model must be turned off..."));
    }

    if (errorMessage != "") {
        QMessageBox::critical(this, tr("Create Simulation"), QString(tr("The following error(s) occured:\n")
                                                                            + errorMessage), QMessageBox::Ok);
        return;
    }

    double m_leishman;
    if (m_desingGroup->button(1)->isChecked()) m_leishman = m_z_leishmanEdit->getValue();
    else m_leishman = -1;

    QLLTSimulation *simulation =
            new QLLTSimulation(m_nameEdit->text(), m_bisVawt,
                               GetUsedWindField(),
                               m_timeStepEdit->getValue(),
                               m_numberTimesteps->getValue(),
                               m_aziStepEdit->getValue(),
                               m_rotorComboBox->currentObject(),
                               m_rotorSpeedEdit->getValue(),
                               m_nacelleYawEdit->getValue(),
                               m_shaftTilt->getValue(),
                               m_YRollEdit->getValue(),
                               m_XRollEdit->getValue(),
                               m_horInflowAngleEdit->getValue(),
                               m_VertInflowSpeedEdit->getValue(),
                               m_turbulentViscosity->getValue(),
                               m_airDensEdit->getValue(),
                               m_kinViscEdit->getValue(),
                               m_firstWakeRowLength->getValue(),
                               m_minGammaFrac->getValue(),
                               m_fullWakeRevs->getValue(),
                               m_fineWakeRevs->getValue(),
                               m_maxWakeAge->getValue(),
                               m_vortexTimeOffset->getValue(),
                               m_Epsilon->getValue(),
                               m_relaxationFactor->getValue(),
                               m_maxIterations->getValue(),
                               m_thinFactor->getValue(),
                               m_leishman,
                               m_TowerShadowGroup->button(0)->isChecked(),
                               m_TowerDragCoeff->getValue(),
                               m_TowerTopRadius->getValue(),
                               m_TowerBottomRadius->getValue(),
                               m_RotorOverhang->getValue(),
                               m_GroundEffectGroup->button(0)->isChecked(),
                               m_HubHeight->getValue(),
                               m_TowerHeight->getValue(),
                               m_WakeRollupGroup->button(0)->isChecked(),
                               m_ShedGroup->button(0)->isChecked(),
                               m_TrailGroup->button(0)->isChecked(),
                               m_TurbConvectionGroup->button(0)->isChecked(),
                               m_storeWakePositions->button(0)->isChecked(),
                               m_InflowSpeed->getValue(),
                               m_SimParameters,
                               m_FileHeader,
                               GetFileName(),
                               GetWindInputType(),
                               m_TStart->getValue(),
                               m_DecFactor->getValue(),
                               m_RotorStandstillGroup->button(0)->isChecked(),
                               m_ShearCoefficientEdit->getValue(),
                               false,
                               m_IntegrationSchemeGroup->button(1)->isChecked(),
                               m_IntegrationSchemeGroup->button(2)->isChecked(),
                               GetDiscType(),
                               m_numPanels->value(),
                               m_ConeAngle->getValue(),
                               m_upWindGroup->button(0)->isChecked(),
                               m_HimmelskampGroup->button(0)->isChecked(),
                               m_aziStart->getValue(),
                               m_momInertia->getValue(),
                               m_initRot->getValue(),
                               m_startupGroup->button(0)->isChecked(),
                               m_DSGroup->button(0)->isChecked(),
                               m_Tf->getValue(),
                               m_Tp->getValue(),
                               m_Tv->getValue(),
                               m_Tvl->getValue(),
                               m_BoundVortexPos->getValue(),
                               m_AoAPos->getValue(),
                               m_adaptiveGroup->button(0)->isChecked(),
                               m_EvalCenterGroup->button(0)->isChecked(),
                               m_vortexLiftGroup->button(0)->isChecked(),
                               m_maxStretch->getValue(),
                               m_Nth_WakeStep->getValue(),
                               wakeCounter,
                               m_ShearGroup->button(0)->isChecked(),
                               m_strainGroup->button(0)->isChecked(),
                               m_numStrutPanels->value(),
                               m_strutGroup->button(1)->isChecked(),
                               m_refHeightEdit->getValue());

    if (!m_bisVawt){
        if (g_QLLTHAWTSimulationStore.add(simulation)) {
            m_module->setShownLLTSimulation(simulation, true);
            accept();  // leave dialog only if adding was successful
        }
    }
    else{
        if (g_QLLTVAWTSimulationStore.add(simulation)) {
            m_module->setShownLLTSimulation(simulation, true);
            accept();  // leave dialog only if adding was successful
        }
    }
}

void QLLTCreatorDialog::onCancelButtonClicked() {
    reject();
}

void QLLTCreatorDialog::OnShearChanged(){
    if (m_ShearGroup->button(0)->isChecked()) m_ShearLabel->setText(tr("Power Law Shear Exp [-]: "));
    if (m_ShearGroup->button(1)->isChecked()) m_ShearLabel->setText(tr("Roughness Length [m]:"));
}

void QLLTCreatorDialog::OnRunTimeChanged (){

    m_numberRevs->setValue(m_totalRunTimeEdit->getValue()/m_timeStepEdit->getValue()/360*m_aziStepEdit->getValue());
    m_numberTimesteps->setValue(m_totalRunTimeEdit->getValue()/m_timeStepEdit->getValue());

}

int QLLTCreatorDialog::GetWindInputType (){
    if (m_WindInputGroup->button(CONSTANT)->isChecked()) return CONSTANT;
    else if (m_WindInputGroup->button(SIMFILE)->isChecked()) return SIMFILE;
    else return AERODYN;
}

int QLLTCreatorDialog::GetDiscType(){
    if (m_discTypeGroup->button(0)->isChecked()) return 0;
    else if (m_discTypeGroup->button(1)->isChecked()) return 1;
    else return 2;
}

void QLLTCreatorDialog::OnNumberTimestepsChanged (){

    m_numberRevs->setValue(m_numberTimesteps->getValue()/360*m_aziStepEdit->getValue());
    m_totalRunTimeEdit->setValue(m_numberTimesteps->getValue()*m_timeStepEdit->getValue());


}

void QLLTCreatorDialog::OnIncludeDSChanged (){

    m_Tf->setEnabled(m_DSGroup->button(0)->isChecked());
    m_Tp->setEnabled(m_DSGroup->button(0)->isChecked());
    m_Tv->setEnabled(m_DSGroup->button(0)->isChecked());
    m_Tvl->setEnabled(m_DSGroup->button(0)->isChecked());
    m_vortexLiftGroup->button(0)->setEnabled(false);
    m_vortexLiftGroup->button(1)->setEnabled(false);
}

void QLLTCreatorDialog::OnVortSizeChanged() {

    if (m_desingGroup->button(1)->isChecked()) m_vortSizeLabel->setText("-");
    else{
        double a = 1.25643; // a factor from literature
        double size =  sqrt(4*a*m_turbulentViscosity->getValue()*m_kinViscEdit->getValue()*(m_vortexTimeOffset->getValue()));
        m_vortSizeLabel->setText(QString().number(size));

        double wind;

        if (GetUsedWindField()) wind = GetUsedWindField()->getMeanWindSpeed();
        else if (m_WindInputGroup->button(CONSTANT)->isChecked()) wind = m_InflowSpeed->getValue();
        else wind = 12.00;

        CBlade *blade = m_rotorComboBox->currentObject();
        double chord = blade->m_TChord[int(blade->m_NPanel)];
        double vel = sqrt(pow(m_TSREdit->getValue()*wind,2)+pow(wind,2));
        double ind = 0.5*chord*vel*2.0/2/PI/size;
        m_vortInductionLabel->setText("~ "+QString().number(ind));
    }

}

void QLLTCreatorDialog::OnNumberRevolutionsChanged (){

    m_totalRunTimeEdit->setValue(m_numberRevs->getValue()*360.0/m_aziStepEdit->getValue()*m_timeStepEdit->getValue());
    m_numberTimesteps->setValue(int(m_numberRevs->getValue()*360.0/m_aziStepEdit->getValue()));

}

void QLLTCreatorDialog::OnStrainChanged(){

    m_maxStretch->setEnabled(m_strainGroup->button(0)->isChecked());

}

void QLLTCreatorDialog::OnTSRChanged (){
    if (!m_bisVawt) m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->getRotorRadius()/2/PI*getMeanWindSpeed()*60);
    if (m_bisVawt) m_rotorSpeedEdit->setValue(m_TSREdit->getValue()/m_rotorComboBox->currentObject()->m_MaxRadius/2/PI*getMeanWindSpeed()*60);
    OnAziChanged ();
}

void QLLTCreatorDialog::OnRotationalSpeedChanged (){
    if (GetWindInputType() != SIMFILE){
    if (!m_bisVawt) m_TSREdit->setValue(m_rotorSpeedEdit->getValue()*m_rotorComboBox->currentObject()->getRotorRadius()*2*PI/getMeanWindSpeed()/60);
    if (m_bisVawt) m_TSREdit->setValue(m_rotorSpeedEdit->getValue()*m_rotorComboBox->currentObject()->m_MaxRadius*2*PI/getMeanWindSpeed()/60);
    OnAziChanged ();
    }
}

void QLLTCreatorDialog::OnAziChanged (){
    if (GetWindInputType() != SIMFILE){
    m_timeStepEdit->setValue(m_aziStepEdit->getValue()/360.0*60.0/m_rotorSpeedEdit->getValue());
    OnNumberRevolutionsChanged();
     }
}

void QLLTCreatorDialog::OnTimestepChanged (){
    m_aziStepEdit->setValue(m_timeStepEdit->getValue()*360/60*m_rotorSpeedEdit->getValue());
    OnNumberTimestepsChanged ();
}

void QLLTCreatorDialog::OnIncludeTowerChanged (){

    m_TowerTopRadius->setEnabled(m_TowerShadowGroup->button(0)->isChecked());
    m_TowerBottomRadius->setEnabled(m_TowerShadowGroup->button(0)->isChecked());
    m_TowerDragCoeff->setEnabled(m_TowerShadowGroup->button(0)->isChecked());
    m_TowerHeight->setEnabled(m_TowerShadowGroup->button(0)->isChecked());

}

void QLLTCreatorDialog::OnDiscTypeChanged (){
    m_numPanels->setEnabled(!m_discTypeGroup->button(0)->isChecked());
}

void QLLTCreatorDialog::OnDesingChanged() {
    if (m_desingGroup->button(1)->isChecked()) m_z_leishmanEdit->setEnabled(true);
    else m_z_leishmanEdit->setEnabled(false);
}

void QLLTCreatorDialog::OnStartupChanged() {

    OnWindTypeChanged();

}

void QLLTCreatorDialog::OnWakeCountChanged(){
    if (m_wakeCounterGroup->button(REVOLUTIONS)->isChecked()){
        m_fullWakeRevsLabel->setText(tr("Full Wake length in revolutions [-]:"));
        m_fineWakeRevsLabel->setText(tr("Fine Wake length in revolutions [-]:"));
        m_maxWakeAgeLabel ->setText(tr("Max Wake length in revolutions [-]:"));
        wakeCounter = REVOLUTIONS;
    }
    else if (m_wakeCounterGroup->button(TIMESTEPS)->isChecked()){
        m_fullWakeRevsLabel->setText(tr("Full Wake length in timesteps [-]:"));
        m_fineWakeRevsLabel->setText(tr("Fine Wake length in timesteps [-]:"));
        m_maxWakeAgeLabel ->setText(tr("Max Wake length in timesteps [-]:"));
        wakeCounter = TIMESTEPS;
    }
    else if (m_wakeCounterGroup->button(TIME)->isChecked()){
        m_fullWakeRevsLabel->setText(tr("Full Wake length in time [s]:"));
        m_fineWakeRevsLabel->setText(tr("Fine Wake length in time [s]:"));
        m_maxWakeAgeLabel ->setText(tr("Max Wake length in time [s]:"));
        wakeCounter = TIME;
    }
}

void QLLTCreatorDialog::OnWindTypeChanged (){

    if (m_WindInputGroup->button(SIMFILE)->isChecked()){
        m_OpenFile->setVisible(true);
        m_ViewFile->setVisible(true);
        m_SimFileLabel->setVisible(true);
        m_AeroDynLabel->setVisible(false);
        m_ViewFile->setEnabled(m_SimFileLabel->text() != "- NONE -");
        OnTimestepChanged();
    }
    else if (m_WindInputGroup->button(AERODYN)->isChecked()){
        m_OpenFile->setVisible(true);
        m_ViewFile->setVisible(true);
        m_SimFileLabel->setVisible(false);
        m_AeroDynLabel->setVisible(true);
        m_ViewFile->setEnabled(m_AeroDynLabel->text() != "- NONE -");
        OnRotationalSpeedChanged();
    }
    else if (m_WindInputGroup->button(CONSTANT)->isChecked()){
        m_OpenFile->setVisible(false);
        m_ViewFile->setVisible(false);
        m_SimFileLabel->setVisible(false);
        m_AeroDynLabel->setVisible(false);
        OnRotationalSpeedChanged();
    }

    if (m_startupGroup->button(0)->isChecked()){
        m_momInertia->setEnabled(true);
        m_initRot->setEnabled(true);
    }
    else{
        m_momInertia->setEnabled(false);
        m_initRot->setEnabled(false);
    }

    m_numberRevs->setEnabled(!m_WindInputGroup->button(SIMFILE)->isChecked() && !m_startupGroup->button(0)->isChecked());
    m_aziStepEdit->setEnabled((!m_WindInputGroup->button(SIMFILE)->isChecked() && !m_startupGroup->button(0)->isChecked()) || m_adaptiveGroup->button(0)->isChecked());
    m_horInflowAngleEdit->setEnabled(!m_WindInputGroup->button(SIMFILE)->isChecked()&&!m_WindInputGroup->button(AERODYN)->isChecked());
    m_TSREdit->setEnabled(!m_WindInputGroup->button(SIMFILE)->isChecked()&&!m_WindInputGroup->button(AERODYN)->isChecked() && !m_startupGroup->button(0)->isChecked());
    m_rotorSpeedEdit->setEnabled(!m_WindInputGroup->button(SIMFILE)->isChecked() && !m_startupGroup->button(0)->isChecked());
    m_nacelleYawEdit->setEnabled(!m_WindInputGroup->button(SIMFILE)->isChecked());
    m_VertInflowSpeedEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearCoefficientEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_refHeightEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearGroup->button(0)->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearGroup->button(1)->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_InflowSpeed->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_totalRunTimeEdit->setEnabled(!(m_adaptiveGroup->button(0)->isChecked() && m_WindInputGroup->button(SIMFILE)->isChecked()) && !(m_adaptiveGroup->button(0)->isChecked() && m_startupGroup->button(0)->isChecked()));
    m_timeStepEdit->setEnabled(!(m_adaptiveGroup->button(0)->isChecked() && m_WindInputGroup->button(SIMFILE)->isChecked()) && !(m_adaptiveGroup->button(0)->isChecked() && m_startupGroup->button(0)->isChecked()));

    m_RotorStandstillGroup->button(0)->setEnabled(!m_startupGroup->button(0)->isChecked());
    m_RotorStandstillGroup->button(1)->setEnabled(!m_startupGroup->button(0)->isChecked());
    m_adaptiveGroup->button(0)->setEnabled(m_WindInputGroup->button(SIMFILE)->isChecked() || m_startupGroup->button(0)->isChecked());
    m_adaptiveGroup->button(1)->setEnabled(m_WindInputGroup->button(SIMFILE)->isChecked() || m_startupGroup->button(0)->isChecked());
    m_adaptiveGroup->button(1)->setChecked(!m_WindInputGroup->button(SIMFILE)->isChecked() && !m_startupGroup->button(0)->isChecked());

    if (m_adaptiveGroup->button(0)->isChecked()) m_Nth_WakeStep->setValue(1);
    m_Nth_WakeStep->setEnabled(!m_adaptiveGroup->button(0)->isChecked());

    onWindFieldChanged();
}

void QLLTCreatorDialog::OnStandStillChanged(){
    if (m_RotorStandstillGroup->button(1)->isChecked()){
        m_rotorSpeedEdit->setEnabled(true);
        m_TSREdit->setEnabled(true);
        m_numberRevs->setEnabled(true);
        m_aziStepEdit->setEnabled(true);
        m_startupGroup->button(0)->setEnabled(true);
        m_startupGroup->button(1)->setEnabled(true);
    }
    else if (m_RotorStandstillGroup->button(0)->isChecked()){
        m_rotorSpeedEdit->setEnabled(false);
        m_TSREdit->setEnabled(false);
        m_numberRevs->setEnabled(false);
        m_aziStepEdit->setEnabled(false);
        m_startupGroup->button(0)->setEnabled(false);
        m_startupGroup->button(1)->setEnabled(false);
    }
}

void QLLTCreatorDialog::OnUseWindField(){
    if (m_useWindfieldGroup->button(0)->isChecked()) m_windFieldComboBox->setEnabled(true);
    else m_windFieldComboBox->setEnabled(false);
    m_VertInflowSpeedEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearCoefficientEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_refHeightEdit->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_InflowSpeed->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearGroup->button(0)->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
    m_ShearGroup->button(1)->setEnabled(m_WindInputGroup->button(CONSTANT)->isChecked() && m_useWindfieldGroup->button(1)->isChecked());
}

WindField* QLLTCreatorDialog::GetUsedWindField(){
    if (m_useWindfieldGroup->button(0)->isChecked()) return m_windFieldComboBox->currentObject();
    else return NULL;
}



void QLLTCreatorDialog::OnViewWindFile(){

    if (GetWindInputType() == AERODYN && m_AeroDynLabel->text() == "- NONE -") return;
    if (GetWindInputType() == SIMFILE && m_SimFileLabel->text() == "- NONE -") return;

    if (!m_SimParameters.size()) return;

    QStringList varNames;
     if (GetWindInputType() == AERODYN){
         varNames.append("TIME");
         varNames.append("WIND SPEED");
         varNames.append("WIND DIR");
         varNames.append("VERT SPEED");
         varNames.append("HORIZ SHEAR");
         varNames.append("VERT SHEAR");
         varNames.append("LINV SHEAR");
         varNames.append("GUST SPEED");
     }
     else if (GetWindInputType() == SIMFILE){
         varNames.append("TIME");
         varNames.append("ROT SPEED");
         varNames.append("WIND SPEED");
         varNames.append("VERT SPEED");
         varNames.append("WIND DIR");
         varNames.append("YAW ANGLE");
         varNames.append("PLAT ROLL");
         varNames.append("PLAT PITCH");
         varNames.append("PLAT YAW");
         varNames.append("PLAT SURGE");
         varNames.append("PLAT SWAY");
         varNames.append("PLAT HEAVE");
         for (int i=0;i<m_rotorComboBox->currentObject()->m_blades;i++){
         varNames.append("BLA "+QString().number(i+1,'f',0)+" PITCH");
         }
     }

    QVBoxLayout *vBox = new QVBoxLayout;
    QTextEdit *textEdit = new QTextEdit;
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QHBoxLayout *hBox = new QHBoxLayout ();
    QString strong;


    QDialog *viewFile = new QDialog(g_mainFrame);
    viewFile->setMinimumWidth(750);
    viewFile->setMinimumHeight(450);
    viewFile->setLayout(vBox);

    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vBox->addWidget(textEdit);
    vBox->addLayout(hBox);
        hBox->addStretch();
        hBox->addWidget(closeButton);
    connect (closeButton,SIGNAL(clicked()), viewFile,SLOT(close()));

    for (int i=0;i<m_FileHeader.size();i++){
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(m_FileHeader.at(i)+"\n");
    }

    int size;
    if (m_SimParameters[0].size()<=varNames.size()) size = m_SimParameters[0].size();
    else size = varNames.size();

    for (int j=0;j<size;j++){
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(varNames.at(j)+"\t");
    }
    textEdit->insertPlainText("\n");

    for (int i=0;i<m_SimParameters.size();i++){
        for (int j=0;j<size;j++){
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(strong.number(m_SimParameters.at(i).at(j),'f',2)+"\t");
        }
        textEdit->insertPlainText("\n");
    }


    viewFile->exec();

//    delete textEdit;
//    delete vBox;
//    delete closeButton;
//    delete hBox;
//    delete viewFile;

}

void QLLTCreatorDialog::OnImportWindFile(){

        QRegExp any_number = QRegExp("^\\-?\\+?[0-9]+(\\.[0-9]+)?(e?E?\\-?\\+?[0-9]+)?$"); //regexp for a positive or negative floating number...
        QString PathName, strong, header;
        QList<double> datarow;
        bool isAeroDyn = false;
        bool isSimFile = false;
        m_SimParameters.clear();
        m_FileHeader.clear();
        m_AeroDynLabel->setText("- NONE -");
        m_SimFileLabel->setText("- NONE -");

        if (GetWindInputType() == SIMFILE){
        PathName = QFileDialog::getOpenFileName(g_mainFrame, tr("Open File"),
                                                g_mainFrame->m_LastDirName,
                                                tr("QBlade Simulation File (*.sim)"));
        isSimFile = true;
        }
        else if (GetWindInputType() == AERODYN){
        PathName = QFileDialog::getOpenFileName(g_mainFrame, tr("Open File"),
                                                g_mainFrame->m_LastDirName,
                                                tr("AeroDyn Hub Height Wind File (*.wnd)"));
        isAeroDyn = true;
        }
        else return;

        if(!PathName.length())		return ;
        int pos = PathName.lastIndexOf("/");
        if(pos>0) g_mainFrame->m_LastDirName = PathName.left(pos);

        QFile XFile(PathName);
        if (!XFile.open(QIODevice::ReadOnly))
        {
            QString strange = tr("Could not read the file\n")+PathName;
            QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
            return;
        }

        QTextStream in(&XFile);

        bool valid = true;
        int columns = 0;

        while(!in.atEnd())
        {
            valid = true;
            header = in.readLine();
            strong = header.simplified();
            strong = UnifyString(strong);

            QStringList list = strong.split(QRegExp(" "),QString::SkipEmptyParts);

            for (int i=0; i<list.size();i++){
                if (!any_number.exactMatch(list.at(i)) && (i<3)) valid = false;
            }

            if (!valid) m_FileHeader.append(header);

            int maxSimRowSize = 12+m_rotorComboBox->currentObject()->m_blades;

            if (list.size()>=2 && valid){
                datarow.clear();
                for (int j=0;j<list.size();j++){
                    if (any_number.exactMatch(list.at(j))) datarow.append(list.at(j).toDouble());
                }
                if ((datarow.size() > 8 && isAeroDyn) || (datarow.size() > maxSimRowSize && isSimFile)){
                    if (isAeroDyn) while (datarow.size()>8) datarow.removeLast();
                    if (isSimFile) while (datarow.size()>13) datarow.removeLast();
                }
                columns = datarow.size();
                m_SimParameters.append(datarow);
                }
            }

            if(m_SimParameters.size() < 2){
                QString strange = tr("Data in file is corrupt or does contain wrong data and cannot be interpreted\n")+PathName;
                QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                return;
            }

            int size = m_SimParameters.at(0).size();
            bool isCorrect = true;

            if (isAeroDyn){
                for (int i=0;i<m_SimParameters.size();i++){
                if (m_SimParameters.at(i).at(0)<0) isCorrect = false;
                if (i > 0) if(m_SimParameters.at(i).at(0)<=m_SimParameters.at(i-1).at(0)) isCorrect = false;
                if (size<2) isCorrect = false;
                if (size != columns) isCorrect = false;
                }
            }
            else if(isSimFile){
                for (int i=0;i<m_SimParameters.size();i++){
                if (m_SimParameters.at(i).at(0)<0) isCorrect = false;
                if (i > 0) if(m_SimParameters.at(i).at(0)<=m_SimParameters.at(i-1).at(0)) isCorrect = false;
                if (size<3) isCorrect = false;
                if (size != columns) isCorrect = false;
                }
            }

            if(!isCorrect){
                QString strange = tr("Data in file is corrupt or does contain wrong data and cannot be interpreted\n")+PathName;
                QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                return;
            }
            if(pos>0 && GetWindInputType() == SIMFILE) m_SimFileLabel->setText(PathName.right(PathName.length()-pos-1));
            if(pos>0 && GetWindInputType() == AERODYN) m_AeroDynLabel->setText(PathName.right(PathName.length()-pos-1));

            m_ViewFile->setEnabled(true);
}







