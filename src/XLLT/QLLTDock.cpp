/****************************************************************************

    QLLTDock Class
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

#include "QLLTDock.h"
#include <QMessageBox>
#include <QProgressBar>
#include "QLLTModule.h"
#include "QLLTToolBar.h"
#include "QLLTCreatorDialog.h"
#include "../Misc/LineButton.h"
#include "../Misc/LinePickerDlg.h"
#include "QLLTModule.h"
#include "QLLTSimulation.h"
#include "../src/StoreAssociatedComboBox.h"
#include "../src/GlobalFunctions.h"

QLLTDock::QLLTDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QLLTModule *module)
    : ScrolledDock (title, parent, flags)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);
    setVisible(false);

    m_module = module;
    m_shownLLTSimulation = NULL;

    QGroupBox *groupBox = new QGroupBox ("LLT Controls");
    m_contentVBox->addWidget(groupBox);
        QGridLayout *grid = new QGridLayout ();
        groupBox->setLayout(grid);
            m_startSimulationButton = new QPushButton (tr("Start Simulation"));
            connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT(onStartSimulationButtonClicked()));
            grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);
            m_renameButton = new QPushButton (tr("Rename"));
            connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
            grid->addWidget (m_renameButton, 1, 0);
            m_editCopyButton = new QPushButton (tr("Edit/Copy"));
            connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));
            grid->addWidget (m_editCopyButton, 1, 1);
            m_deleteButton = new QPushButton (tr("Delete"));
            connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
            grid->addWidget (m_deleteButton, 2, 0);
            m_newButton = new QPushButton (tr("New"));
            connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
            grid->addWidget (m_newButton, 2, 1);

            groupBox = new QGroupBox(tr("Run LLT on:"));
            m_contentVBox->addWidget(groupBox);
            grid = new QGridLayout ();
            groupBox->setLayout(grid);
            m_UseDevice = new QComboBox;
            grid->addWidget(m_UseDevice,0,0);
            m_UseDevice->addItem("CPU: Single Thread");
            m_UseDevice->addItem("CPU: OpenMP Multi Threading");
            m_UseDevice->setCurrentIndex(1);

            connect (m_UseDevice, SIGNAL(activated(int)), this, SLOT(onUseDeviceChanged(int)));

            m_GroupSizeLabel = new QLabel("#");
            m_GroupSizeLabel->hide();
            grid->addWidget(m_GroupSizeLabel,0,1);
            m_GroupSizeBox = new QSpinBox;
            grid->addWidget(m_GroupSizeBox,0,2);
            m_GroupSizeBox->setMaximumWidth(40);
            m_GroupSizeBox->setMinimumWidth(40);
            m_GroupSizeBox->setMinimum(1);
            m_GroupSizeBox->setMaximum(512);
            m_GroupSizeBox->setValue(32);
            m_GroupSizeBox->hide();


    groupBox = new QGroupBox(tr("Progress"));
    m_contentVBox->addWidget(groupBox);
    grid = new QGridLayout ();
            groupBox->setLayout(grid);
            m_progress = new QProgressBar;
            m_progress->setAlignment(Qt::AlignHCenter);
            grid->addWidget(m_progress,0,0);
            stopButton = new QPushButton(tr("STOP"));
            stopButton->setEnabled(false);
            grid->addWidget(stopButton,0,1);
            connect(stopButton, SIGNAL(clicked()), m_module, SLOT(onStopSimulation()));

            continueButton = new QPushButton(tr("CONTINUE"));
            continueButton->setEnabled(false);
            grid->addWidget(continueButton,0,1);
            connect(continueButton, SIGNAL(clicked()), this, SLOT(onContinueSimulationButtonClicked()));
            continueButton->setVisible(false);

            m_sectionBox = new QGroupBox(tr("Section to plot:"));

            grid = new QGridLayout ();
            m_sectionBox->setLayout(grid);
            QLabel *label = new QLabel(tr("Section in % of radius:"));
            grid->addWidget(label,0,0);
            m_sectionEdit = new QDoubleSpinBox;
            m_sectionEdit->setMinimum(0);
            m_sectionEdit->setMaximum(1);
            m_sectionEdit->setSingleStep(0.01);
            m_sectionEdit->setDecimals(3);
            grid->addWidget(m_sectionEdit,0,1);
            m_absoluteSection = new QLabel(tr("= x [m]"));
            grid->addWidget(m_absoluteSection,1,0);

            connect(m_sectionEdit,SIGNAL(valueChanged(double)),this,SLOT(onSectionChanged(double)));

    m_stylebox = new QGroupBox (tr("Styles"));

    grid = new QGridLayout ();
    m_stylebox->setLayout(grid);
            int gridRowCount = 0;
            label = new QLabel (tr("Style: "));
            grid->addWidget(label, gridRowCount, 0);
            m_simulationLineButton = new LineButton ();
            grid->addWidget(m_simulationLineButton, gridRowCount++, 1);
            QHBoxLayout *hBox = new QHBoxLayout ();
            grid->addLayout(hBox, gridRowCount++, 0, 1, 2);
            m_showCheckBox = new QCheckBox (tr("show"));
            hBox->addWidget(m_showCheckBox);
            m_showCurveCheckBox = new QCheckBox (tr("curve"));
            hBox->addWidget(m_showCurveCheckBox);
            m_showPointsCheckBox = new QCheckBox (tr("points"));
            hBox->addWidget(m_showPointsCheckBox);

            connect(m_simulationLineButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
            connect(m_showCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCheckBoxCanged()));
            connect(m_showCurveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCurveCheckBoxCanged()));
            connect(m_showPointsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowPointsCheckBoxCanged()));




    m_cutBox = new QGroupBox(tr("Velocity Cut Plane"));
    QVBoxLayout *vBox = new QVBoxLayout ();
    hBox = new QHBoxLayout;
    grid = new QGridLayout ();

    m_VAWTCutPlaneBox = new LLTCutPlaneComboBox(&g_QLLTCutPlaneStore);
    m_VAWTCutPlaneBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_VAWTCutPlaneBox->setMinimumWidth(170);
    m_VAWTCutPlaneBox->setParentBox(m_module->GetQLLTToolBar()->m_LLTVAWTSimulationComboBox);
    vBox->addWidget(m_VAWTCutPlaneBox);

    m_HAWTCutPlaneBox = new LLTCutPlaneComboBox(&g_QLLTCutPlaneStore);
    m_HAWTCutPlaneBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_HAWTCutPlaneBox->setMinimumWidth(170);
    m_HAWTCutPlaneBox->setParentBox(m_module->GetQLLTToolBar()->m_LLTHAWTSimulationComboBox);
    vBox->addWidget(m_HAWTCutPlaneBox);

    vBox->addLayout(hBox);
    vBox->addLayout(grid);

    m_cutBox->setLayout(vBox);
    label = new QLabel(tr("Plane Center"));
    hBox->addWidget(label);
    label = new QLabel(tr("X"));
    grid->addWidget(label,0,0);
    m_x_cut = new QDoubleSpinBox;
    grid->addWidget(m_x_cut,0,1);
    label = new QLabel(tr("Y"));
    grid->addWidget(label,1,0);
    m_y_cut = new QDoubleSpinBox;
    grid->addWidget(m_y_cut,1,1);
    label = new QLabel(tr("Z"));
    grid->addWidget(label,2,0);
    m_z_cut = new QDoubleSpinBox;
    grid->addWidget(m_z_cut,2,1);

    label = new QLabel(tr("Plane Rotation"));
    hBox->addWidget(label);
    label = new QLabel(tr("X"));
    grid->addWidget(label,0,2);
    m_x_rot = new QDoubleSpinBox;
    grid->addWidget(m_x_rot,0,3);
    label = new QLabel(tr("Y"));
    grid->addWidget(label,1,2);
    m_y_rot = new QDoubleSpinBox;
    grid->addWidget(m_y_rot,1,3);
    label = new QLabel(tr("Z"));
    grid->addWidget(label,2,2);
    m_z_rot = new QDoubleSpinBox;
    grid->addWidget(m_z_rot,2,3);

    label = new QLabel(tr("Length"));
    grid->addWidget(label,3,0);
    m_length = new QDoubleSpinBox;
    grid->addWidget(m_length,3,1);
    label = new QLabel(tr("Width"));
    grid->addWidget(label,3,2);
    m_width = new QDoubleSpinBox;
    grid->addWidget(m_width,3,3);

    label = new QLabel(tr("X Res"));
    grid->addWidget(label,4,0);
    m_X_res = new QSpinBox;
    grid->addWidget(m_X_res,4,1);
    label = new QLabel(tr("Y Res"));
    grid->addWidget(label,4,2);
    m_Y_res = new QSpinBox;
    grid->addWidget(m_Y_res,4,3);
    m_cutPlaneTime = new QLabel(tr("At Time: [-]"));
    hBox = new QHBoxLayout;
    label = new QLabel(" for all timesteps");
    m_allTimeSteps = new QCheckBox;
    hBox->addWidget(m_cutPlaneTime);
    hBox->addWidget(label);
    hBox->addWidget(m_allTimeSteps);
    vBox->addLayout(hBox);


    hBox = new QHBoxLayout;
    m_addPlane = new QPushButton(tr("New"));
    hBox->addWidget(m_addPlane);
    m_canceldeletePlane = new QPushButton(tr("Delete Single"));
    hBox->addWidget(m_canceldeletePlane);
    vBox->addLayout(hBox);
    hBox = new QHBoxLayout;
    m_deleteAllPlanes = new QPushButton(tr("Delete All"));
    hBox->addWidget(m_deleteAllPlanes);
    m_exportPlane = new QPushButton(tr("Export Single"));
    hBox->addWidget(m_exportPlane);
    vBox->addLayout(hBox);


    hBox = new QHBoxLayout;
    m_exportAllPlanes = new QPushButton(tr("Export All"));
    hBox->addWidget(m_exportAllPlanes);
    m_exportVelVolume = new QPushButton(tr("Export Vel Field"));
    hBox->addWidget(m_exportVelVolume);
    vBox->addLayout(hBox);


    m_x_cut->setDecimals(2);
    m_y_cut->setDecimals(2);
    m_z_cut->setDecimals(2);
    m_x_rot->setDecimals(2);
    m_y_rot->setDecimals(2);
    m_z_rot->setDecimals(2);

    m_x_rot->setMinimum(-180);
    m_y_rot->setMinimum(-180);
    m_z_rot->setMinimum(-180);
    m_x_rot->setMaximum(180);
    m_y_rot->setMaximum(180);
    m_z_rot->setMaximum(180);

    m_x_cut->setMinimum(-10e5);
    m_y_cut->setMinimum(-10e5);
    m_z_cut->setMinimum(-10e5);
    m_x_cut->setMaximum(10e5);
    m_y_cut->setMaximum(10e5);
    m_z_cut->setMaximum(10e5);

    m_x_cut->setSingleStep(1);
    m_y_cut->setSingleStep(1);
    m_z_cut->setSingleStep(1);
    m_x_rot->setSingleStep(5);
    m_y_rot->setSingleStep(5);
    m_z_rot->setSingleStep(5);

    m_X_res->setSingleStep(10);
    m_Y_res->setSingleStep(10);
    m_X_res->setMinimum(1);
    m_Y_res->setMinimum(1);
    m_X_res->setMaximum(1000);
    m_Y_res->setMaximum(1000);

    m_width->setSingleStep(1);
    m_length->setSingleStep(1);
    m_width->setMinimum(0);
    m_length->setMinimum(0);
    m_width->setMaximum(10e5);
    m_length->setMaximum(10e5);

    m_X_res->setEnabled(false);
    m_Y_res->setEnabled(false);
    m_x_rot->setEnabled(false);
    m_y_rot->setEnabled(false);
    m_z_rot->setEnabled(false);
    m_x_cut->setEnabled(false);
    m_y_cut->setEnabled(false);
    m_z_cut->setEnabled(false);
    m_width->setEnabled(false);
    m_length->setEnabled(false);
    m_allTimeSteps->setEnabled(false);

    m_X_res->setMaximumSize(60,30);
    m_Y_res->setMaximumSize(60,30);
    m_x_rot->setMaximumSize(60,30);
    m_y_rot->setMaximumSize(60,30);
    m_z_rot->setMaximumSize(60,30);
    m_x_cut->setMaximumSize(60,30);
    m_y_cut->setMaximumSize(60,30);
    m_z_cut->setMaximumSize(60,30);
    m_width->setMaximumSize(60,30);
    m_length->setMaximumSize(60,30);


    connect(m_addPlane, SIGNAL(clicked()), this, SLOT(OnCreateCutPlane()));
    connect(m_canceldeletePlane, SIGNAL(clicked()), m_module, SLOT(OnDeleteCutPlane()));
    connect(m_deleteAllPlanes, SIGNAL(clicked()), m_module, SLOT(OnDeleteAllCutPlanes()));
    connect(m_exportAllPlanes, SIGNAL(clicked()), this, SLOT(OnExportAllPlanes()));
    connect(m_exportPlane, SIGNAL(clicked()), this, SLOT(OnExportPlane()));
    connect(m_exportVelVolume, SIGNAL(clicked()), this, SLOT(OnExportVelField()));


    m_vizbox = new QGroupBox(tr("Turbine Visualization"));

    grid = new QGridLayout ();
    vBox = new QVBoxLayout;
    m_vizbox->setLayout(vBox);
    vBox->addLayout(grid);
        m_shed = new QPushButton(tr("Shed Vortices"));
        m_shed->setCheckable(true);
//        m_shed->setFixedWidth(checkButtonWidth);
        m_shed->setFlat(true);
        grid->addWidget(m_shed,0,0);

        m_trail = new QPushButton(tr("Trailing Vortices"));
        m_trail->setCheckable(true);
//        m_trail->setFixedWidth(checkButtonWidth);
        m_trail->setFlat(true);

        grid->addWidget(m_trail,0,1);

        m_nodes = new QPushButton(tr("Vortex Nodes"));
        m_nodes->setCheckable(true);
//        m_nodes->setFixedWidth(checkButtonWidth);
        m_nodes->setFlat(true);

        grid->addWidget(m_nodes,1,0);

        QPalette palette = m_nodes->palette();
        palette.setColor(QPalette::Button,QColor(Qt::white));


        m_singleblade = new QPushButton(tr("Single Blade Wake"));
        m_singleblade->setCheckable(true);
//        m_singleblade->setFixedWidth(checkButtonWidth);
        m_singleblade->setFlat(true);

        grid->addWidget(m_singleblade,1,1);

        m_showRotor = new QPushButton(tr("Rotor"));
        m_showRotor->setCheckable(true);
//        m_showRotor->setFixedWidth(checkButtonWidth);
        m_showRotor->setFlat(true);

        grid->addWidget(m_showRotor,2,0);

        m_showModel = new QPushButton(tr("Turbine Model"));
        m_showModel->setCheckable(true);
//        m_showModel->setFixedWidth(checkButtonWidth);
        m_showModel->setFlat(true);

        grid->addWidget(m_showModel,2,1);

        m_showCoordinates = new QPushButton(tr("Coordinate Systems"));
        m_showCoordinates->setCheckable(true);
//        m_showCoordinates->setFixedWidth(checkButtonWidth);
        m_showCoordinates->setFlat(true);

        grid->addWidget(m_showCoordinates,3,0);

        m_perspectiveView = new QPushButton(tr("Perspective View"));
        m_perspectiveView->setCheckable(true);
//        m_perspectiveView->setFixedWidth(checkButtonWidth);
        m_perspectiveView->setFlat(true);

        grid->addWidget(m_perspectiveView,3,1);

        m_showText = new QPushButton(tr("Show Text"));
        m_showText->setCheckable(true);
//        m_showText->setFixedWidth(checkButtonWidth);
        m_showText->setFlat(true);

        grid->addWidget(m_showText,4,0);

        m_showWindfield = new QPushButton(tr("Show WindField"));
        m_showWindfield->setCheckable(true);
//        m_showWindfield->setFixedWidth(checkButtonWidth);
        m_showWindfield->setFlat(true);

        grid->addWidget(m_showWindfield,4,1);

        m_colorAge = new QPushButton(tr("Color by Age"));
        m_colorAge->setCheckable(true);
//        m_colorAge->setFixedWidth(checkButtonWidth);
        m_colorAge->setFlat(true);

        grid->addWidget(m_colorAge,5,0);

        m_colorStrength = new QPushButton(tr("Color by Strength"));
        m_colorStrength->setCheckable(true);
//        m_colorStrength->setFixedWidth(checkButtonWidth);
        m_colorStrength->setFlat(true);

        grid->addWidget(m_colorStrength,5,1);

        m_linesize = new QDoubleSpinBox();
        m_linesize->setDecimals(1);
        m_linesize->setMinimum(0.5);
        m_linesize->setMaximum(10.0);
        m_linesize->setValue(0.5);
        m_linesize->setSingleStep(0.1);

        label = new QLabel(tr("Line Size"));
        QHBoxLayout *miniHBox = new QHBoxLayout ();
        miniHBox->addWidget(label);
        miniHBox->addWidget(m_linesize);
        grid->addLayout(miniHBox,6,0);

        m_pointsize = new QDoubleSpinBox();
        m_pointsize->setDecimals(1);
        m_pointsize->setSingleStep(0.1);
        m_pointsize->setMinimum(1.0);
        m_pointsize->setMaximum(99.0);
        m_pointsize->setValue(1.5);
        label = new QLabel(tr("Point Size"));
        miniHBox = new QHBoxLayout ();
        miniHBox->addWidget(label);
        miniHBox->addWidget(m_pointsize);
        grid->addLayout(miniHBox,6,1);



        cutVizBox = new QGroupBox(tr("Velocity Cut Plane Visualization"));

        vBox = new QVBoxLayout;
        cutVizBox->setLayout(vBox);
        grid = new QGridLayout;
        vBox->addLayout(grid);


        m_showCutPlanes = new QPushButton(tr("Show Cut Planes"));
        m_showCutPlanes->setCheckable(true);
//        m_showCutPlanes->setFixedWidth(checkButtonWidth);
        m_showCutPlanes->setFlat(true);

        grid->addWidget(m_showCutPlanes,0,0);

        m_showVelVectors = new QPushButton(tr("Show Vectors"));
        m_showVelVectors->setCheckable(true);
//        m_showVelVectors->setFixedWidth(checkButtonWidth);
        m_showVelVectors->setFlat(true);

        grid->addWidget(m_showVelVectors,0,1);



        miniHBox = new QHBoxLayout();
        label = new QLabel(tr("Colormap: "));
        miniHBox->addWidget(label);
        m_colorMapButton = new QButtonGroup(miniHBox);
        QRadioButton *radioButton = new QRadioButton ("R/B");
        m_colorMapButton->addButton(radioButton, 0);
        miniHBox->addWidget(radioButton);
        radioButton = new QRadioButton ("Color");
        m_colorMapButton->addButton(radioButton, 1);
        miniHBox->addWidget(radioButton);
        m_colorMapButton->button(0)->setChecked(true);

        QLabel *lab = new QLabel(tr("Range"));
        m_range = new QDoubleSpinBox;
        m_range->setSingleStep(0.01);
        m_range->setMinimum(0.01);
        m_range->setMaximum(1.0);
        m_range->setValue(0.8);
        miniHBox->addWidget(lab);
        miniHBox->addWidget(m_range);

        vBox->addLayout(miniHBox);

        grid = new QGridLayout();
        label = new QLabel(tr("Component: "));
        grid->addWidget(label,0,0);
        m_componentButton = new QButtonGroup(miniHBox);

        radioButton = new QRadioButton ("Total");
        m_componentButton->addButton(radioButton, 0);
        grid->addWidget(radioButton,1,0);

        radioButton = new QRadioButton ("X");
        m_componentButton->addButton(radioButton, 1);
        grid->addWidget(radioButton,0,1);

        radioButton = new QRadioButton ("Y");
        m_componentButton->addButton(radioButton, 2);
        grid->addWidget(radioButton,0,2);

        radioButton = new QRadioButton ("Z");
        m_componentButton->addButton(radioButton, 3);
        grid->addWidget(radioButton,0,3);

        radioButton = new QRadioButton ("Axial");
        m_componentButton->addButton(radioButton, 4);
        grid->addWidget(radioButton,1,1);

        radioButton = new QRadioButton ("Tang.");
        m_componentButton->addButton(radioButton, 5);
        grid->addWidget(radioButton,1,2);

        radioButton = new QRadioButton ("Radial");
        m_componentButton->addButton(radioButton, 6);
        grid->addWidget(radioButton,1,3);

        m_componentButton->button(0)->setChecked(true);
        vBox->addLayout(grid);

        m_showWindfield->setChecked(true);
        m_showVelVectors->setChecked(false);
        m_showCutPlanes->setChecked(true);
        m_nodes->setChecked(false);
        m_trail->setChecked(true);
        m_shed->setChecked(true);
        m_showRotor->setChecked(true);
        m_showCoordinates->setChecked(false);
        m_showModel->setChecked(true);
        m_singleblade->setChecked(false);
        m_perspectiveView->setChecked(false);
        m_showText->setChecked(true);
        m_colorStrength->setChecked(false);
        m_colorAge->setChecked(false);



        connect(m_shed, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_trail, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_nodes, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_colorAge, SIGNAL(toggled(bool)), this, SLOT(OnAge()));
        connect(m_colorStrength, SIGNAL(toggled(bool)), this, SLOT(OnStrength()));

        connect(m_linesize, SIGNAL(valueChanged(double)), m_module, SLOT(forceReRender()));
        connect(m_pointsize, SIGNAL(valueChanged(double)), m_module, SLOT(forceReRender()));


        connect(m_showRotor, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_showText, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));

        connect(m_showCoordinates, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_showModel, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));

        connect(m_singleblade, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_perspectiveView, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));

        connect(m_colorMapButton, SIGNAL(buttonToggled(int,bool)), m_module, SLOT(OnRenderCutPlanes()));
        connect(m_colorMapButton, SIGNAL(buttonToggled(int,bool)), m_module, SLOT(forceReRender()));
        connect(m_componentButton, SIGNAL(buttonToggled(int,bool)), m_module, SLOT(OnComponentChanged()));
        connect(m_componentButton, SIGNAL(buttonToggled(int,bool)), m_module, SLOT(OnRenderCutPlanes()));
        connect(m_componentButton, SIGNAL(buttonToggled(int,bool)), m_module, SLOT(forceReRender()));
        connect(m_range, SIGNAL(valueChanged(double)), m_module, SLOT(OnRenderCutPlanes()));
        connect(m_range, SIGNAL(valueChanged(double)), m_module, SLOT(forceReRender()));


        connect(m_showWindfield, SIGNAL(toggled(bool)), m_module, SLOT(forceReRender()));
        connect(m_showCutPlanes, SIGNAL(toggled(bool)), m_module, SLOT(OnRenderCutPlanes()));
        connect(m_showVelVectors, SIGNAL(toggled(bool)), m_module, SLOT(OnRenderCutPlanes()));




    m_contentVBox->addWidget(m_sectionBox);
    m_contentVBox->addWidget(m_stylebox);
        m_contentVBox->addWidget(m_vizbox);
        m_contentVBox->addWidget(m_cutBox);
        m_contentVBox->addWidget(cutVizBox);

    m_contentVBox->addStretch(10000000);

    addScrolledDock(Qt::LeftDockWidgetArea , parent);

    setShownLLTSimulation(NULL);


    connect(&g_QLLTCutPlaneStore, SIGNAL(objectListChanged(bool)), m_module, SLOT(OnSelChangeCutPlane()));


}

void QLLTDock::OnAge(){
    if(m_colorAge->isChecked()){
        m_colorStrength->setChecked(false);
    }
    m_module->forceReRender();
}

void QLLTDock::OnStrength(){
    if(m_colorStrength->isChecked()){
        m_colorAge->setChecked(false);
    }
    m_module->forceReRender();
}

void QLLTDock::onUseDeviceChanged(int i){
    qDebug() << "changed "<< i;
    if (i<2){ m_GroupSizeBox->hide();
        m_GroupSizeLabel->hide();}
    else{ m_GroupSizeBox->show();
        m_GroupSizeLabel->show();}
}

void QLLTDock::onSectionChanged(double section){
    if (m_module->GetQLLTSim()) {
        if (m_module->GetQLLTSim()->getIsVawt()){
            for (int i = 0; i < g_QLLTVAWTSimulationStore.size(); ++i) {
               g_QLLTVAWTSimulationStore.at(i)->setShownBladeSection(section);
            }
        } else {
            for (int i = 0; i < g_QLLTHAWTSimulationStore.size(); ++i) {
               g_QLLTHAWTSimulationStore.at(i)->setShownBladeSection(section);
            }
        }
        m_module->reloadTimeGraphs();
        if (m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*section <=
			m_module->GetQLLTSim()->getUsedBlade()->m_TPos[0]) {
			m_absoluteSection->setText("= "+QString().number(m_module->GetQLLTSim()->getUsedBlade()->m_TPos[0])+" [m]");
		} else {
			m_absoluteSection->setText("= " +
									   QString().number(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*section) +
									   " [m]");
		}
    } else {
        m_absoluteSection->setText("= x [m]");
    }
}

void QLLTDock::adjustShowCheckBox() {
    m_showCheckBox->setChecked(m_module->getShownQLLTSimulation()->isShownInGraph());
}

void QLLTDock::OnExportVelField(){

    NumberEdit *XC,*YC,*ZC, *XD, *YD, *ZD, *XR, *YR, *ZR, *CHUS;
    QLabel *XCL,*YCL,*ZCL, *XDL, *YDL, *ZDL, *XRL, *YRL, *ZRL, *CBox, *CHUL;
    QPushButton *accept, *reject;
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *hlayout = new QHBoxLayout;
    QGridLayout *grid = new QGridLayout;
    QDialog *dialog = new QDialog();
    QCheckBox *allsteps = new QCheckBox();

    dialog->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    allsteps->setChecked(false);

    XC = new NumberEdit;
    YC = new NumberEdit;
    ZC = new NumberEdit;
    XD = new NumberEdit;
    YD = new NumberEdit;
    ZD = new NumberEdit;
    XR = new NumberEdit;
    YR = new NumberEdit;
    ZR = new NumberEdit;
    CHUS = new NumberEdit;


    XCL = new QLabel(tr("X Center"));
    YCL = new QLabel(tr("Y Center"));
    ZCL = new QLabel(tr("Z Center"));
    XDL = new QLabel(tr("X Dim"));
    YDL = new QLabel(tr("Y Dim"));
    ZDL = new QLabel(tr("Z Dim"));
    XRL = new QLabel(tr("X Res"));
    YRL = new QLabel(tr("Y Res"));
    ZRL = new QLabel(tr("Z Res"));
    CBox = new QLabel(tr("All Timesteps"));
    CHUL = new QLabel(tr("Chunk Size"));


    accept = new QPushButton(tr("Export"));
    reject = new QPushButton(tr("Reject"));

    connect(accept,SIGNAL(clicked(bool)), dialog, SLOT(accept()));
    connect(reject,SIGNAL(clicked(bool)), dialog, SLOT(reject()));

    grid->addWidget(XC,0,1);
    grid->addWidget(YC,1,1);
    grid->addWidget(ZC,2,1);
    grid->addWidget(XD,0,3);
    grid->addWidget(YD,1,3);
    grid->addWidget(ZD,2,3);
    grid->addWidget(XR,0,5);
    grid->addWidget(YR,1,5);
    grid->addWidget(ZR,2,5);
    grid->addWidget(XCL,0,0);
    grid->addWidget(YCL,1,0);
    grid->addWidget(ZCL,2,0);
    grid->addWidget(XDL,0,2);
    grid->addWidget(YDL,1,2);
    grid->addWidget(ZDL,2,2);
    grid->addWidget(XRL,0,4);
    grid->addWidget(YRL,1,4);
    grid->addWidget(ZRL,2,4);

    XC->setValue(-m_module->GetQLLTSim()->getOverhang());
    ZC->setValue(m_module->GetQLLTSim()->getTowerHeight());
    XD->setValue(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*2.5);
    YD->setValue(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*2.5);
    ZD->setValue(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*2.5);
    XR->setValue(50);
    YR->setValue(50);
    ZR->setValue(50);
    XR->setAutomaticPrecision(0);
    YR->setAutomaticPrecision(0);
    ZR->setAutomaticPrecision(0);
    XR->setMinimum(2);
    YR->setMinimum(2);
    ZR->setMinimum(2);
    XR->setMaximum(1000);
    YR->setMaximum(1000);
    ZR->setMaximum(1000);
    CHUS->setAutomaticPrecision(0);
    CHUS->setMaximum(1e6);
    CHUS->setMinimum(1000);
    CHUS->setValue(5e5);



    if (m_module->GetQLLTSim()->getStoreWake()){
    grid->addWidget(CBox,3,0);
    grid->addWidget(allsteps,3,1);
    }
    grid->addWidget(CHUL,3,2);
    grid->addWidget(CHUS,3,3);

    layout->addLayout(grid);
    hlayout->addWidget(accept);
    hlayout->addWidget(reject);
    layout->addLayout(hlayout);

    dialog->setLayout(layout);

    if (dialog->exec() == QDialog::Accepted){

        //create filename

        QString SelectedFilter;
        QFileDialog::Options options;
        QStringList velFileNames, bladeFileNames;
        QStringList timesteps;

        QString fileName = QFileDialog::getSaveFileName(g_mainFrame, QString("Export Graph"), g_mainFrame->m_ExportLastDirName, tr("Text File (*.txt);;Comma Seperated Values (*.csv);;VTK format (*.vtu)"),&SelectedFilter, options);
        if (fileName.size()){
            QString ascName;
            int pos;
            pos = fileName.lastIndexOf("/");
            if(pos>0) g_mainFrame->m_ExportLastDirName = fileName.left(pos);
            pos = fileName.lastIndexOf(".");

            int Xres = XR->getValue();
            int Yres = YR->getValue();
            int Zres = ZR->getValue();

            //set loop size if exported @ all timesteps
            int num_loops;
            if (allsteps->isChecked()) num_loops = m_module->GetQLLTSim()->getResultsArray().at(0)->size();
            else num_loops = 1;

            //break down matrix size for better memory management
            int total_size = Xres*Yres*Zres;
            int XCHUNK, num_chunks;

            if (total_size > int(CHUS->getValue())) XCHUNK = floor(int(CHUS->getValue())/(Yres*Zres));
            else XCHUNK = Xres;

            if (XCHUNK == 0) XCHUNK = Yres * Zres;
            num_chunks = floor(total_size/(XCHUNK*Yres*Xres));

            QProgressDialog progress("","Cancel",0,(num_loops)*(num_chunks),this);
            progress.setModal(true);
            QLabel lab;
            progress.setLabel(&lab);

            for (int step = 0; step < num_loops; step++){

                lab.setText("Computing Velocities...");


                CVector ***positions, ***velocities;
                positions = new CVector**[Xres];
                velocities = new CVector**[Xres];

                //init arrays
                for (int i = 0; i < Xres; ++i) {
                    positions[i] = new CVector*[Yres];
                    velocities[i] = new CVector*[Yres];
                    for (int j = 0; j < Yres; ++j) {
                        positions[i][j] = new CVector[Zres] ();
                        velocities[i][j] = new CVector[Zres] ();
                    }
                }
                //compute positions
                for (int i=0; i<Xres;i++){
                    for (int j=0;j<Yres;j++){
                        for (int k=0; k<Zres; k++){
                            positions[i][j][k].Set(XC->getValue()-XD->getValue()/2+XD->getValue()/Xres*i, YC->getValue()-YD->getValue()/2+YD->getValue()/Yres*j, ZC->getValue()-ZD->getValue()/2+ZD->getValue()/Zres*k);
                        }
                    }
                }

                int timestep;
                if (!m_module->GetQLLTSim()->getStoreWake()) timestep = 0;
                else if (allsteps->isChecked()) timestep = step;
                else timestep = m_module->GetQLLTSim()->getShownTimeIndex();
                double time;
                if (!m_module->GetQLLTSim()->getStoreWake()) time = m_module->GetQLLTSim()->getResultsArray().at(0)->at(m_module->GetQLLTSim()->getResultsArray().at(0)->size()-1);
                else time = m_module->GetQLLTSim()->getResultsArray().at(0)->at(timestep);

                //break down computation in chunks here:
                //compute velocities

                m_module->GetQLLTSim()->InitMultiThreading();

                int XSTART = 0;
                int XEND = XCHUNK;

                for (int i=0;i<num_chunks;i++){
//                qDebug() <<endl<< num_chunks << XCHUNK << XSTART << XEND << Xres;
                m_module->GetQLLTSim()->onComputeVelVolume(positions, velocities, XSTART, XEND, Yres, Zres, timestep, time);
                XSTART=XEND;
                XEND+=XCHUNK;
                progress.setValue(step*num_chunks+i);
                if (progress.wasCanceled()) break;
                progress.update();
                progress.show();


                }
                //last partial chunk
                if (XSTART<Xres){
                    XEND = Xres;
                m_module->GetQLLTSim()->onComputeVelVolume(positions, velocities, XSTART, XEND, Yres, Zres, timestep, time);
                }

                if (progress.wasCanceled()) break;

                lab.setText("Writing Data to File...");
                progress.update();
                progress.show();

                if (allsteps->isChecked()){
                    ascName = fileName.left(pos) + QString().number(step) +"."+ fileName.at(pos+1)+ fileName.at(pos+2)+ fileName.at(pos+3);
                    WriteVelFieldToFile(ascName, velocities, positions,Xres,Yres,Zres,time);
                }
                else WriteVelFieldToFile(fileName, velocities, positions,Xres,Yres,Zres,time);

                if (fileName.lastIndexOf(".vtu") > 0){
                    ascName = fileName.left(pos) + QString().number(step) +"."+ fileName.at(pos+1)+ fileName.at(pos+2)+ fileName.at(pos+3);

                    if (allsteps->isChecked()){
                        velFileNames.append(ascName);
                        bladeFileNames.append(fileName.left(pos) + "_blade" + QString().number(step) +".vtu");
                        WriteBladeGeomToFile(fileName.left(pos) + "_blade" + QString().number(step) + ".vtu", timestep);
                        timesteps.append(QString().number(time,'f',6));
                    }
                    else{
                        velFileNames.append(fileName);
                        bladeFileNames.append(fileName.left(pos) + "_blade" + ".vtu");
                        WriteBladeGeomToFile(fileName.left(pos) + "_blade" + ".vtu", timestep);
                        timesteps.append("0");
                    }
                }

                //clean up
                for (int i = 0; i < Xres; ++i) {
                    for (int j = 0; j < Yres; ++j) {
                        delete [] positions[i][j];
                        delete [] velocities[i][j];
                    }
                    delete [] positions[i];
                    delete [] velocities[i];
                }
                delete [] positions;
                delete [] velocities;


            }

            //write timeseries XML file if needed
            if (fileName.lastIndexOf(".vtu") > 0 && timesteps.size()){
                QFile file (fileName.left(pos)+"_collection"+".pvd");

                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream stream (&file);
                    stream << "<?xml version=\"1.0\"?>"<<endl;
                    stream << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">"<<endl;
                    stream << "<Collection>"<<endl;
                    for (int i=0;i<timesteps.size();i++){
                        stream << "<DataSet timestep=\""<<timesteps.at(i)<<"\" group=\"\" part=\"0\" file=\""<<velFileNames.at(i)<<"\"/>"<<endl;
                        stream << "<DataSet timestep=\""<<timesteps.at(i)<<"\" group=\"\" part=\"1\" file=\""<<bladeFileNames.at(i)<<"\"/>"<<endl;
                    }
                    stream << "</Collection>"<<endl;
                    stream << "</VTKFile>" << endl;
                }
                file.close();
            }
        }
    }

    delete grid;
    delete hlayout;
    delete XC;
    delete YC;
    delete ZC;
    delete XD;
    delete YD;
    delete ZD;
    delete XR;
    delete YR;
    delete ZR;
    delete XCL;
    delete YCL;
    delete ZCL;
    delete XDL;
    delete YDL;
    delete ZDL;
    delete XRL;
    delete YRL;
    delete ZRL;
    delete CHUL;
    delete CHUS;
    delete CBox;
    delete allsteps;
    delete accept;
    delete reject;
    delete layout;
    delete dialog;




}

void QLLTDock::OnCreateCutPlane(){

    if (!m_module->GetQLLTSim()) return;
    if (!m_module->GetQLLTSim()->hasStoredData()) return;

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    disconnect(m_canceldeletePlane, SIGNAL(clicked()), m_module, SLOT(OnDeleteCutPlane()));
    connect(m_canceldeletePlane, SIGNAL(clicked()), this, SLOT(OnCancelCutPlane()));
    m_canceldeletePlane->setText(tr("Cancel"));

    m_HAWTCutPlaneBox->setEnabled(false);
    m_VAWTCutPlaneBox->setEnabled(false);

    QString name = m_module->GetQLLTSim()->getName() + " Plane";
    for (int i=0;i<g_QLLTCutPlaneStore.size();i++){
        if (name == g_QLLTCutPlaneStore.at(i)->getName()){
            name = makeNameWithHigherNumber(name);
            i = 0;
        }
    }
    QLLTCutPlane *plane = new QLLTCutPlane(name, m_module->GetQLLTSim());
    if (m_module->GetQLLTSim()->getStoreWake()){
    plane->m_time = m_module->GetQLLTSim()->getResultsArray().at(0)->at(m_module->GetQLLTSim()->getShownTimeIndex());
    }
    else plane->m_time = m_module->GetQLLTSim()->getResultsArray().at(0)->at(m_module->GetQLLTSim()->getResultsArray().at(0)->size()-1);

    QString strong;
    m_cutPlaneTime->setText("At Time: "+strong.number(plane->m_time,'f',3)+" [s]");

    m_addPlane->setText(tr("Compute"));
    m_addPlane->disconnect();
    connect(m_addPlane, SIGNAL(clicked()), this, SLOT(OnComputeCutPlane()));

    m_X_res->setEnabled(true);
    m_Y_res->setEnabled(true);
    m_x_rot->setEnabled(true);
    m_y_rot->setEnabled(true);
    m_z_rot->setEnabled(true);
    m_x_cut->setEnabled(true);
    m_y_cut->setEnabled(true);
    m_z_cut->setEnabled(true);
    m_width->setEnabled(true);
    m_length->setEnabled(true);
    m_allTimeSteps->setEnabled(true);
    m_allTimeSteps->setChecked(false);

    if (!m_module->GetCutPlane()){
    m_X_res->setValue(50);
    m_Y_res->setValue(50);
    m_width->setValue(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*2.5);
    m_length->setValue(m_module->GetQLLTSim()->getUsedBlade()->getRotorRadius()*2.5);
    m_x_rot->setValue(0);
    m_y_rot->setValue(0);
    m_z_rot->setValue(0);
    m_x_cut->setValue(-m_module->GetQLLTSim()->getOverhang());
    m_y_cut->setValue(0);
    m_z_cut->setValue(m_module->GetQLLTSim()->getTowerHeight());
    }

    connect(m_x_cut, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_y_cut, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_z_cut, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_x_rot, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_y_rot, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_z_rot, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_width, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_length, SIGNAL(valueChanged(double)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_X_res, SIGNAL(valueChanged(int)), m_module, SLOT(OnUpdateCutPlane()));
    connect(m_Y_res, SIGNAL(valueChanged(int)), m_module, SLOT(OnUpdateCutPlane()));

    m_module->SetCutPlane(plane);

    m_startSimulationButton->setEnabled(false);
    m_editCopyButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_renameButton->setEnabled(false);
    m_newButton->setEnabled(false);
    continueButton->setEnabled(false);

    m_deleteAllPlanes->setEnabled(false);
    m_exportAllPlanes->setEnabled(false);
    m_exportPlane->setEnabled(false);
    m_exportVelVolume->setEnabled(false);


    m_module->DisableButtons();

    m_module->OnUpdateCutPlane();


}

void QLLTDock::OnComputeCutPlane(){

    m_addPlane->setText(tr("New"));
    m_addPlane->disconnect();
    connect(m_addPlane, SIGNAL(clicked()), this, SLOT(OnCreateCutPlane()));

    m_HAWTCutPlaneBox->setEnabled(true);
    m_VAWTCutPlaneBox->setEnabled(true);

    m_X_res->setEnabled(false);
    m_Y_res->setEnabled(false);
    m_x_rot->setEnabled(false);
    m_y_rot->setEnabled(false);
    m_z_rot->setEnabled(false);
    m_x_cut->setEnabled(false);
    m_y_cut->setEnabled(false);
    m_z_cut->setEnabled(false);
    m_width->setEnabled(false);
    m_length->setEnabled(false);
    m_allTimeSteps->setEnabled(false);

    m_X_res->disconnect();
    m_Y_res->disconnect();
    m_width->disconnect();
    m_length->disconnect();
    m_x_rot->disconnect();
    m_y_rot->disconnect();
    m_z_rot->disconnect();
    m_x_cut->disconnect();
    m_y_cut->disconnect();
    m_z_cut->disconnect();

    int time;
    QLLTCutPlane *plane;

    if (m_allTimeSteps->isChecked() && m_module->GetQLLTSim()->getStoreWake()){
        QProgressDialog progress("Computing Cut Planes...","Cancel",0,m_module->GetQLLTSim()->getResultsArray().at(0)->size()-1,this);
        progress.setModal(true);
        disconnect(&g_QLLTCutPlaneStore, SIGNAL(objectListChanged(bool)), m_module, SLOT(OnSelChangeCutPlane()));

        plane = m_module->GetCutPlane();
        m_module->SetCutPlane(NULL);
        delete plane;
        for (int i=0;i<m_module->GetQLLTSim()->getResultsArray().at(0)->size();i++){

            QString name = m_module->GetQLLTSim()->getName() + " Plane";
            for (int j=0;j<g_QLLTCutPlaneStore.size();j++){
                if (name == g_QLLTCutPlaneStore.at(j)->getName()){
                    name = makeNameWithHigherNumber(name);
                    j = 0;
                }
            }
            plane = new QLLTCutPlane(name, m_module->GetQLLTSim());
            plane->m_X_res = m_X_res->value();
            plane->m_Y_res = m_Y_res->value();
            plane->m_width = m_width->value();
            plane->m_length = m_length->value();
            plane->m_X_rot = m_x_rot->value();
            plane->m_Y_rot = m_y_rot->value();
            plane->m_Z_rot = m_z_rot->value();
            plane->m_X = m_x_cut->value();
            plane->m_Y = m_y_cut->value();
            plane->m_Z = m_z_cut->value();
            plane->m_time = m_module->GetQLLTSim()->getResultsArray().at(0)->at(i);
            plane->m_Hub = m_module->GetQLLTSim()->m_savedCoordinateSystems.at(i).at(1);
            plane->m_Axis = m_module->GetQLLTSim()->m_savedCoordinateSystems.at(i).at(0);
            plane->Update();
            m_module->GetQLLTSim()->onComputeCutPlane(plane, i);
            g_QLLTCutPlaneStore.add(plane);

            if (progress.wasCanceled()) break;
            progress.setValue(i);
        }
        connect(&g_QLLTCutPlaneStore, SIGNAL(objectListChanged(bool)), m_module, SLOT(OnSelChangeCutPlane()));
    }
    else{
    if (!m_module->GetQLLTSim()->getStoreWake()) time = 0;
    else time = m_module->GetQLLTSim()->getShownTimeIndex();
    plane = m_module->GetCutPlane();
    plane->m_Hub = m_module->GetQLLTSim()->m_savedCoordinateSystems.at(0).at(1);
    plane->m_Axis = m_module->GetQLLTSim()->m_savedCoordinateSystems.at(0).at(0);
    m_module->GetQLLTSim()->onComputeCutPlane(plane, time);
    g_QLLTCutPlaneStore.add(plane);
    }

    if (m_module->GetQLLTSim()->getIsVawt()){
        m_VAWTCutPlaneBox->setCurrentObject(plane);
    }
    else{
        m_HAWTCutPlaneBox->setCurrentObject(plane);
    }

    m_startSimulationButton->setEnabled(m_module->GetQLLTSim());
    m_editCopyButton->setEnabled(m_module->GetQLLTSim());
    m_deleteButton->setEnabled(m_module->GetQLLTSim());
    m_renameButton->setEnabled(m_module->GetQLLTSim());
    m_newButton->setEnabled(true);
    m_deleteAllPlanes->setEnabled(true);
    m_exportAllPlanes->setEnabled(true);
    m_exportPlane->setEnabled(true);
    m_exportVelVolume->setEnabled(true);

    if (m_module->GetQLLTSim()){
    continueButton->setEnabled(m_module->GetQLLTSim()->getCanContinue());
    }

    m_module->EnableButtons();

    disconnect(m_canceldeletePlane, SIGNAL(clicked()), this, SLOT(OnCancelCutPlane()));
    connect(m_canceldeletePlane, SIGNAL(clicked()), m_module, SLOT(OnDeleteCutPlane()));
    m_canceldeletePlane->setText(tr("Delete Single"));

    m_module->OnRenderCutPlanes();
}

void QLLTDock::OnExportAllPlanes(){

    if (!m_module->GetCutPlane()) return;

    QString SelectedFilter;
    QStringList timesteps;
    QStringList velFileNames;
    QFileDialog::Options options;
    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    QString fileName = QFileDialog::getSaveFileName(g_mainFrame, QString("Export Graph"), g_mainFrame->m_ExportLastDirName, tr("Text File (*.txt);;Comma Seperated Values (*.csv);;VTK XML format (*.vtu)"),&SelectedFilter, options);

    LLTCutPlaneComboBox *box;
    if (m_module->GetQLLTSim()->getIsVawt()) box = m_VAWTCutPlaneBox;
    else box = m_HAWTCutPlaneBox;

    int pos;
    pos = fileName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_ExportLastDirName = fileName.left(pos);

    pos = fileName.lastIndexOf(".");

    if(fileName.lastIndexOf(".txt") > 0) for (int i=0;i<box->count();i++) box->getObjectAt(i)->exportPlane(fileName.left(pos)+QString().number(i)+".txt");

    else if(fileName.lastIndexOf(".csv") > 0) for (int i=0;i<box->count();i++) box->getObjectAt(i)->exportPlaneCSV(fileName.left(pos)+QString().number(i)+".csv");

    else if(fileName.lastIndexOf(".vtu") > 0){
        for (int i=0;i<box->count();i++){
            box->getObjectAt(i)->exportPlaneVTK(fileName.left(pos)+QString().number(i)+".vtu");
            velFileNames.append(fileName.left(pos)+QString().number(i)+".vtu");
            timesteps.append(QString().number(box->getObjectAt(i)->m_time,'f',6));
        }
        QFile file (fileName.left(pos)+"timeseries"+".pvd");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream (&file);
            stream << "<?xml version=\"1.0\"?>"<<endl;
            stream << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">"<<endl;
            stream << "<Collection>"<<endl;
            for (int i=0;i<box->count();i++){
                stream << "<velFileNames timestep=\""<<timesteps.at(i)<<"\" group=\"\" part=\"0\" file=\""<<velFileNames.at(i)<<"\"/>"<<endl;
            }
            stream << "</Collection>"<<endl;
            stream << "</VTKFile>" << endl;
        }
        file.close();
    }

}

void QLLTDock::WriteVelFieldToFile(QString fileName, CVector*** velocities, CVector*** positions, int XR, int YR, int ZR, double time){

    QDate date = QDate::currentDate();
    QTime curtime = QTime::currentTime();
    QFile file (fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file);

        if(fileName.lastIndexOf(".txt") > 0){
            stream << "Export File Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << curtime.toString("hh:mm:ss") << endl;
            stream << "Simulation Time " << time << " [s]" <<endl;
            stream << "Position Vector\t\tVelocity Vector " << endl;
            stream << "        X\t        Y\t        Z\t        X\t        Y\t        Z" << endl;

            for (int i=0;i<XR;i++){
                for (int j=0;j<YR;j++){
                    for (int k=0;k<ZR;k++){
                    stream << QString("%1\t%2\t%3\t%4\t%5\t%6").arg(positions[i][j][k].x, 9).arg(positions[i][j][k].y, 9).arg(positions[i][j][k].z, 9).arg(velocities[i][j][k].x, 9).arg(velocities[i][j][k].y, 9).arg(velocities[i][j][k].z, 9)<<endl;
                    }
                }
            }
        }
        else if(fileName.lastIndexOf(".csv") > 0){
            stream << "Export File Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << curtime.toString("hh:mm:ss") << endl;
            stream << "Simulation Time " << time << " [s]" <<endl;
            stream << "Position Vector\t\tVelocity Vector " << endl;
            stream << "X;Y;Z;X;Y;Z" << endl;

            for (int i=0;i<XR;i++){
                for (int j=0;j<YR;j++){
                    for (int k=0;k<ZR;k++){
                    stream << QString("%1;%2;%3;%4;%5;%6").arg(positions[i][j][k].x, 9).arg(positions[i][j][k].y, 9).arg(positions[i][j][k].z, 9).arg(velocities[i][j][k].x, 9).arg(velocities[i][j][k].y, 9).arg(velocities[i][j][k].z, 9).replace(".",",")<<endl;
                    }
                }
            }
        }
        else if(fileName.lastIndexOf(".vtu") > 0){

            stream << "<?xml version=\"1.0\"?>" <<endl;
            stream << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" <<endl;
            stream << "<UnstructuredGrid>" <<endl;
            stream << "<Piece NumberOfPoints=\""<<QString().number(XR*YR*ZR,'f',0)<<"\" NumberOfCells=\"" << QString().number((XR-1)*(YR-1)*(ZR-1),'f',0)<<"\">" <<endl;
            stream << "<Points>" <<endl;
            stream << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" <<endl;
            for (int i=0;i<XR;i++){
                for (int j=0;j<YR;j++){
                    for (int k=0;k<ZR;k++){
                        stream << QString("%1 %2 %3").arg(positions[i][j][k].x, 4).arg(positions[i][j][k].y, 4).arg(positions[i][j][k].z, 4)<<endl;
                    }
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "</Points>" <<endl;
            stream << "<Cells>" <<endl;
            stream << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" <<endl;
            for (int i=0;i<XR-1;i++){
                for (int j=0;j<YR-1;j++){
                    for (int k=0;k<ZR-1;k++){
                        stream <<QString().number(k+j*ZR+i*(ZR*YR),'f',0)<<" "<<QString().number(k+j*ZR+1+i*(ZR*YR),'f',0)<<" "<<QString().number(k+(j+1)*ZR+1+i*(ZR*YR),'f',0)<<" "<<QString().number(k+(j+1)*ZR+i*(ZR*YR),'f',0)<<" "<<QString().number(k+j*ZR+(i+1)*(ZR*YR),'f',0)<<" "<<QString().number(k+j*ZR+1+(i+1)*(ZR*YR),'f',0)<<" "<<QString().number(k+(j+1)*ZR+1+(i+1)*(ZR*YR),'f',0)<<" "<<QString().number(k+(j+1)*ZR+(i+1)*(ZR*YR),'f',0)<<endl;
                    }
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" <<endl;
            int offset = 8;
            for (int i=0;i<XR-1;i++){
                for (int j=0;j<YR-1;j++){
                    for (int k=0;k<ZR-1;k++){
                        stream <<QString().number(offset,'f',0)<<endl;
                        offset+=8;
                    }
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" <<endl;
            for (int i=0;i<XR-1;i++){
                for (int j=0;j<YR-1;j++){
                    for (int k=0;k<ZR-1;k++){
                        stream << "12"<<endl;
                    }
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "</Cells>" <<endl;
            stream << "<PointData Vectors=\"velocity\">" <<endl;
            stream << "<DataArray type=\"Float64\" Name=\"velocity\" NumberOfComponents=\"3\" format=\"ascii\">" <<endl;
            for (int i=0;i<XR;i++){
                for (int j=0;j<YR;j++){
                    for (int k=0;k<ZR;k++){
                        stream << QString("%1 %2 %3").arg(velocities[i][j][k].x,4).arg(velocities[i][j][k].y,4).arg(velocities[i][j][k].z,4)<<endl;
                    }
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "</PointData>" <<endl;
            stream << "</Piece>" <<endl;
            stream << "</UnstructuredGrid>" <<endl;
            stream << "</VTKFile>" <<endl;

        }
    }
    file.close();
}

void QLLTDock::WriteBladeGeomToFile(QString fileName, int timestep){

    QFile file (fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file);

        QList< DummyLine> list;

        qDebug() << endl << timestep;

        if (timestep == -1) timestep = m_module->GetQLLTSim()->getVizLines()->size()-1;

        qDebug() << endl << timestep << m_module->GetQLLTSim()->getVizLines()->size();


        for (int i=0; i< m_module->GetQLLTSim()->getVizLines()->at(timestep).size();i++) list.append(m_module->GetQLLTSim()->getVizLines()->at(timestep).at(i));

        int bladesize = list.size()/m_module->GetQLLTSim()->getfromBlade();

        qDebug() << endl << timestep << list.size();


            stream << "<?xml version=\"1.0\"?>" <<endl;
            stream << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" <<endl;
            stream << "<UnstructuredGrid>" <<endl;
            stream << "<Piece NumberOfPoints=\""<<QString().number(2*list.size(),'f',0)<<"\" NumberOfCells=\"" << QString().number(list.size()-m_module->GetQLLTSim()->getfromBlade(),'f',0)<<"\">" <<endl;
            stream << "<Points>" <<endl;
            stream << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" <<endl;
            for (int i=0;i<list.size();i++){
                stream << QString("%1 %2 %3").arg(list.at(i).Lx, 4).arg(list.at(i).Ly, 4).arg(list.at(i).Lz, 4)<<endl;
                stream << QString("%1 %2 %3").arg(list.at(i).Tx, 4).arg(list.at(i).Ty, 4).arg(list.at(i).Tz, 4)<<endl;
            }
            stream << "</DataArray>" <<endl;
            stream << "</Points>" <<endl;
            stream << "<Cells>" <<endl;
            stream << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" <<endl;
            for (int i=0;i<list.size();i++){
                if(!((i+1) % bladesize) == 0){
                stream <<QString().number(2*i,'f',0)<<" "<<QString().number(2*i+1,'f',0)<<" "<<QString().number(2*i+3,'f',0)<<" "<<QString().number(2*i+2,'f',0)<<endl;
                }
            }
            stream << "</DataArray>" <<endl;
            stream << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" <<endl;
            int offset = 4;
            for (int i=0;i<list.size()-m_module->GetQLLTSim()->getfromBlade();i++){
                        stream <<QString().number(offset,'f',0)<<endl;
                        offset+=4;
            }
            stream << "</DataArray>" <<endl;
            stream << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" <<endl;
            for (int i=0;i<list.size()-m_module->GetQLLTSim()->getfromBlade();i++){
                        stream << "9"<<endl;
            }
            stream << "</DataArray>" <<endl;
            stream << "</Cells>" <<endl;
            stream << "</Piece>" <<endl;
            stream << "</UnstructuredGrid>" <<endl;
            stream << "</VTKFile>" <<endl;

        }

    file.close();
}

void QLLTDock::OnExportPlane(){

    if (!m_module->GetCutPlane()) return;

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    QString fileName = QFileDialog::getSaveFileName(g_mainFrame, QString("Export Graph"), g_mainFrame->m_ExportLastDirName, tr("Text File (*.txt);;Comma Seperated Values (*.csv);;VTK format (*.vtu)"));

    int pos;
    pos = fileName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_ExportLastDirName = fileName.left(pos);

    if(fileName.indexOf(".txt") > 0)     m_module->GetCutPlane()->exportPlane(fileName);
    else if(fileName.indexOf(".csv") > 0)     m_module->GetCutPlane()->exportPlaneCSV(fileName);
    else if(fileName.indexOf(".vtu") > 0)     m_module->GetCutPlane()->exportPlaneVTK(fileName);
}


void QLLTDock::OnCancelCutPlane(){

    if (!m_module->GetCutPlane()) return;
    if (m_module->GetCutPlane()->is_computed) return;

    m_HAWTCutPlaneBox->setEnabled(true);
    m_VAWTCutPlaneBox->setEnabled(true);

    delete m_module->GetCutPlane();
    if (m_module->GetQLLTSim()->getIsVawt()) m_module->SetCutPlane(m_VAWTCutPlaneBox->currentObject());
    else m_module->SetCutPlane(m_HAWTCutPlaneBox->currentObject());

    m_X_res->setEnabled(false);
    m_Y_res->setEnabled(false);
    m_x_rot->setEnabled(false);
    m_y_rot->setEnabled(false);
    m_z_rot->setEnabled(false);
    m_x_cut->setEnabled(false);
    m_y_cut->setEnabled(false);
    m_z_cut->setEnabled(false);
    m_width->setEnabled(false);
    m_length->setEnabled(false);
    m_allTimeSteps->setEnabled(false);

    m_X_res->disconnect();
    m_Y_res->disconnect();
    m_width->disconnect();
    m_length->disconnect();
    m_x_rot->disconnect();
    m_y_rot->disconnect();
    m_z_rot->disconnect();
    m_x_cut->disconnect();
    m_y_cut->disconnect();
    m_z_cut->disconnect();


    m_addPlane->setText(tr("New"));
    disconnect(m_addPlane, SIGNAL(clicked()),0,0);
    connect(m_addPlane, SIGNAL(clicked()), this, SLOT(OnCreateCutPlane()));

    m_startSimulationButton->setEnabled(m_module->GetQLLTSim());
    m_editCopyButton->setEnabled(m_module->GetQLLTSim());
    m_deleteButton->setEnabled(m_module->GetQLLTSim());
    m_renameButton->setEnabled(m_module->GetQLLTSim());
    if (m_module->GetQLLTSim()){
    continueButton->setEnabled(m_module->GetQLLTSim()->getCanContinue());
    }
    m_newButton->setEnabled(true);
    m_deleteAllPlanes->setEnabled(true);
    m_exportAllPlanes->setEnabled(true);
    m_exportPlane->setEnabled(true);
    m_exportVelVolume->setEnabled(true);

    disconnect(m_canceldeletePlane, SIGNAL(clicked()), this, SLOT(OnCancelCutPlane()));
    connect(m_canceldeletePlane, SIGNAL(clicked()), m_module, SLOT(OnDeleteCutPlane()));
    m_canceldeletePlane->setText(tr("Delete Single"));

    m_module->EnableButtons();

    m_module->OnRenderCutPlanes();
}

void QLLTDock::updateProgress(int i){
    m_progress->setValue(i);
    QString cur;
    QTextStream(&cur) << i;
    QString end;
    QTextStream(&end) << m_progress->maximum();
    m_progress->setFormat("Timestep "+cur+" of "+end);
}

void QLLTDock::onStartSimulationButtonClicked(){
    if (!m_module->GetQLLTSim())
		return;
	
    m_module->OnDeleteAllCutPlanes();

    m_module->GetQLLTSim()->setCanContinue(false);

    m_module->ResetTimestepSlider();

    m_QLLTThread =  new QLLTThread ();
    m_QLLTThread->simulation = m_shownLLTSimulation;
    connect(m_QLLTThread, SIGNAL(started()), this, SLOT(onSimStarted()), Qt::QueuedConnection);
    connect(m_QLLTThread, SIGNAL(finished()), this, SLOT(onSimFinished()), Qt::QueuedConnection);
    m_QLLTThread->start();

}

void QLLTDock::onContinueSimulationButtonClicked(){
    if (!m_module->GetQLLTSim())
        return;

    m_QLLTThread =  new QLLTThread ();
    m_QLLTThread->simulation = m_shownLLTSimulation;
    connect(m_QLLTThread, SIGNAL(started()), this, SLOT(onSimStarted()), Qt::QueuedConnection);
    connect(m_QLLTThread, SIGNAL(finished()), this, SLOT(onSimFinished()), Qt::QueuedConnection);
    m_QLLTThread->start();
}

void QLLTDock::onSimStarted(){

    m_progress->setMinimum(0);
    m_progress->setMaximum(m_shownLLTSimulation->getNumberTimesteps());
    m_progress->setTextVisible(true);

    stopButton->setVisible(true);
    stopButton->setEnabled(true);
    continueButton->setVisible(false);

    m_startSimulationButton->setEnabled(false);
    m_editCopyButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_renameButton->setEnabled(false);
    m_newButton->setEnabled(false);
    m_UseDevice->setEnabled(false);

    m_canceldeletePlane->setEnabled(false);
    m_addPlane->setEnabled(false);
    m_exportAllPlanes->setEnabled(false);
    m_deleteAllPlanes->setEnabled(false);
    m_exportPlane->setEnabled(false);
    m_exportVelVolume->setEnabled(false);


    m_module->DisableButtons();
}

void QLLTDock::onSimFinished(){

    if (!m_module->GetQLLTSim()->getCanContinue() && m_module->GetQLLTSim()->hasStoredData()){
        m_progress->setValue(0);
        m_progress->setFormat(tr("Finished!"));
        stopButton->setEnabled(false);
        stopButton->setVisible(true);
        continueButton->setVisible(false);
        continueButton->setEnabled(false);
    }
    else{
        stopButton->setEnabled(false);
        stopButton->setVisible(false);
        continueButton->setVisible(true);
        continueButton->setEnabled(true);
    }

    m_canceldeletePlane->setEnabled(true);
    m_addPlane->setEnabled(true);
    m_exportAllPlanes->setEnabled(true);
    m_deleteAllPlanes->setEnabled(true);
    m_exportPlane->setEnabled(true);
    m_exportVelVolume->setEnabled(true);

    m_startSimulationButton->setText(tr("Restart Simulation"));
    m_startSimulationButton->setEnabled(true);
    m_UseDevice->setEnabled(true);
    m_editCopyButton->setEnabled(true);
    m_deleteButton->setEnabled(true);
    m_renameButton->setEnabled(true);
    m_newButton->setEnabled(true);

    m_module->EnableButtons();
    m_module->UpdateDockToolbar();

    m_QLLTThread->deleteLater();

    if (m_module->GetQLLTSim()->wasAborted()) OnReportAbort();

    OnReportConvergenceProblems();

}

void QLLTDock::OnReportAbort(){
    QMessageBox::critical(g_mainFrame, tr("Simulation Aborted"), QString(tr("NaN values occured during the simulation!\nCheck polars for discontinuities and the simulation conditions\nTry deactivating the UAM model")), QMessageBox::Ok);
}


void QLLTDock::OnReportConvergenceProblems(){
    if (m_module->GetQLLTSim()->getConvergenceProblems().size()){
        QDialog *dialog = new QDialog(this);

        dialog->setWindowTitle("WARNING, no convergence obtained for the following timesteps:");
        dialog->setModal(true);

        QVBoxLayout *vBox = new QVBoxLayout;
        dialog->setLayout(vBox);
        QHBoxLayout *hBox = new QHBoxLayout;
        QHBoxLayout *hBox2 = new QHBoxLayout;

        vBox->addLayout(hBox);

        QScrollArea *scroll = new QScrollArea;
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scroll->setWidgetResizable(true);
        vBox->addWidget(scroll);

        QLabel *label = new QLabel();
        QString strong;
        for (int i=0;i<m_module->GetQLLTSim()->getConvergenceProblems().size();i++){
            int blade = 0;
            strong.append("Timestep: ");
            strong.append(QString().number(m_module->GetQLLTSim()->getConvergenceProblems().at(i).at(0),'f',0));
            strong.append(" Time: ");
            strong.append(QString().number(m_module->GetQLLTSim()->getConvergenceProblems().at(i).at(1),'f',3));
            strong.append(" Radial Positions:");

            for (int j=2;j<m_module->GetQLLTSim()->getConvergenceProblems().at(i).size();j++){
                if (blade <= m_module->GetQLLTSim()->getConvergenceProblems().at(i).at(j)){
                    blade++;
                    strong.append("  Blade ");
                    strong.append(QString().number(blade,'f',0));
                    strong.append(":");
                }
                j++;
                strong.append("  ");
                strong.append(QString().number(m_module->GetQLLTSim()->getConvergenceProblems().at(i).at(j),'f',2));
            }
            strong.append("\n");
        }
        label->setText(strong);
//        label->setWordWrap(true);
        scroll->setWidget(label);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        vBox->addLayout(hBox2);

        QPushButton *button = new QPushButton ("Ok");
        connect(button, SIGNAL(clicked()), dialog, SLOT(accept()));
        QLabel *label2 = new QLabel(tr("Check polars for discontinuities; check simulation settings and relaxation"));
        hBox2->addWidget(label2, 0, Qt::AlignLeft);
        hBox2->addWidget(button, 0, Qt::AlignRight);


        dialog->exec();
        dialog->deleteLater();
    }
}

void QLLTDock::onReplayStopped(){

    if (!m_module->GetQLLTSim()) return;
    if (!m_module->GetQLLTSim()->getCanContinue() && m_module->GetQLLTSim()->hasStoredData()){
        continueButton->setEnabled(false);
    }
    else{
        continueButton->setEnabled(true);
    }

    m_canceldeletePlane->setEnabled(true);
    m_addPlane->setEnabled(true);
    m_exportAllPlanes->setEnabled(true);
    m_deleteAllPlanes->setEnabled(true);
    m_exportPlane->setEnabled(true);
    m_exportVelVolume->setEnabled(true);

    m_startSimulationButton->setEnabled(!m_module->GetQLLTSim()->isRunning());
    m_editCopyButton->setEnabled(!m_module->GetQLLTSim()->isRunning());
    m_deleteButton->setEnabled(!m_module->GetQLLTSim()->isRunning());
    m_renameButton->setEnabled(!m_module->GetQLLTSim()->isRunning());
    m_newButton->setEnabled(!m_module->GetQLLTSim()->isRunning());

    m_module->GetQLLTToolBar()->m_LLTHAWTSimulationComboBox->setEnabled(g_QLLTHAWTSimulationStore.size() && !m_module->GetQLLTSim()->isRunning());
    m_module->GetQLLTToolBar()->m_LLTVAWTSimulationComboBox->setEnabled(g_QLLTVAWTSimulationStore.size() && !m_module->GetQLLTSim()->isRunning());

}

void QLLTDock::onReplayStarted(){

    if (!m_module->GetQLLTSim()) return;

    continueButton->setEnabled(false);
    m_startSimulationButton->setEnabled(false);
    m_editCopyButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_renameButton->setEnabled(false);
    m_newButton->setEnabled(false);

    m_canceldeletePlane->setEnabled(false);
    m_addPlane->setEnabled(false);
    m_exportAllPlanes->setEnabled(false);
    m_deleteAllPlanes->setEnabled(false);
    m_exportPlane->setEnabled(false);
    m_exportVelVolume->setEnabled(false);

    m_module->GetQLLTToolBar()->m_LLTHAWTSimulationComboBox->setEnabled(false);
    m_module->GetQLLTToolBar()->m_LLTVAWTSimulationComboBox->setEnabled(false);

}


void QLLTDock::onNewButtonClicked() {
    /* check first, if all requirements are fullfilled */

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    QString errorMessage ("");
    if (!m_module->m_bisVAWT){
        if (g_rotorStore.size() == 0) {
            errorMessage.append(tr("\n - No HAWT Rotor in Database"));
        }
    }
    if (m_module->m_bisVAWT){
        if (g_verticalRotorStore.size() == 0) {
            errorMessage.append(tr("\n - No VAWT Rotor in Database"));
        }
    }

    if (errorMessage != "") {
        QMessageBox::critical(this, tr("Create New Simulation"), QString(tr("The following error(s) occured:\n") + errorMessage), QMessageBox::Ok);
        return;
    }

    QLLTCreatorDialog *creatorDialog = new QLLTCreatorDialog (NULL, m_module, m_module->m_bisVAWT);
    creatorDialog->exec();
    delete creatorDialog;
}

void QLLTDock::setShownLLTSimulation(QLLTSimulation *newSimulation) {
    /* set all labels to correct values */
    m_shownLLTSimulation = newSimulation;
    m_showCheckBox->setEnabled(m_shownLLTSimulation);
    m_showCurveCheckBox->setEnabled(m_shownLLTSimulation);
    m_showPointsCheckBox->setEnabled(m_shownLLTSimulation);
    m_simulationLineButton->setEnabled(m_shownLLTSimulation);
    m_startSimulationButton->setEnabled(m_shownLLTSimulation);
    m_deleteButton->setEnabled(m_shownLLTSimulation);
    m_editCopyButton->setEnabled(m_shownLLTSimulation);
    m_renameButton->setEnabled(m_shownLLTSimulation);

    if (m_shownLLTSimulation){
    m_canceldeletePlane->setEnabled(m_shownLLTSimulation->hasStoredData());
    m_addPlane->setEnabled(m_shownLLTSimulation->hasStoredData());
    m_exportAllPlanes->setEnabled(m_shownLLTSimulation->hasStoredData());
    m_exportPlane->setEnabled(m_shownLLTSimulation->hasStoredData());
    m_exportVelVolume->setEnabled(m_shownLLTSimulation->hasStoredData());
    m_deleteAllPlanes->setEnabled(m_shownLLTSimulation->hasStoredData());
    }
    else{
        m_canceldeletePlane->setEnabled(m_shownLLTSimulation);
        m_addPlane->setEnabled(m_shownLLTSimulation);
        m_exportAllPlanes->setEnabled(m_shownLLTSimulation);
        m_exportPlane->setEnabled(m_shownLLTSimulation);
        m_exportVelVolume->setEnabled(m_shownLLTSimulation);


        m_deleteAllPlanes->setEnabled(m_shownLLTSimulation);
    }

    if (m_shownLLTSimulation){
        m_sectionEdit->setValue(m_shownLLTSimulation->getShownBladeSection());
        if (m_shownLLTSimulation->hasStoredData() && m_shownLLTSimulation->getCanContinue()){
            m_startSimulationButton->setText(tr("Restart Simulation"));
            stopButton->setVisible(false);
            stopButton->setEnabled(false);
            continueButton->setVisible(true);
            continueButton->setEnabled(true);
            m_progress->setTextVisible(true);
            m_progress->setMinimum(0);
            m_progress->setMaximum(m_shownLLTSimulation->getNumberTimesteps());
            m_progress->setValue(m_shownLLTSimulation->getCurrentTimeStep());
            QString cur;
            QTextStream(&cur) << m_shownLLTSimulation->getCurrentTimeStep();
            QString end;
            QTextStream(&end) << m_progress->maximum();
            m_progress->setFormat("Timestep "+cur+" of "+end);
        }
        else if (m_shownLLTSimulation->hasStoredData()){
            m_startSimulationButton->setText(tr("Restart Simulation"));
            stopButton->setVisible(true);
            stopButton->setEnabled(false);
            continueButton->setVisible(false);
            continueButton->setEnabled(false);
            m_progress->setTextVisible(true);
            m_progress->setMinimum(0);
            m_progress->setMaximum(m_shownLLTSimulation->getNumberTimesteps());
            m_progress->setValue(m_shownLLTSimulation->getCurrentTimeStep());
            QString cur;
            QTextStream(&cur) << m_shownLLTSimulation->getCurrentTimeStep();
            QString end;
            QTextStream(&end) << m_progress->maximum();
            m_progress->setFormat("Timestep "+cur+" of "+end);      
        }
        else{
            m_startSimulationButton->setText(tr("Start Simulation"));
            stopButton->setVisible(true);
            stopButton->setEnabled(false);
            continueButton->setVisible(false);
            continueButton->setEnabled(false);
            m_progress->setTextVisible(true);
            m_progress->setMinimum(0);
            m_progress->setMaximum(m_shownLLTSimulation->getNumberTimesteps());
            m_progress->setValue(0);
            QString cur;
            QTextStream(&cur) << 0;
            QString end;
            QTextStream(&end) << m_progress->maximum();
            m_progress->setFormat("Timestep "+cur+" of "+end);
        }
        m_simulationLineButton->setPen(m_shownLLTSimulation->getPen());
        m_showCheckBox->setChecked(m_shownLLTSimulation->isShownInGraph());
        m_showPointsCheckBox->setChecked(m_shownLLTSimulation->isDrawPoints());
        m_showCurveCheckBox->setChecked(m_shownLLTSimulation->isDrawCurve());
        if (m_shownLLTSimulation->isRunning()){
            m_startSimulationButton->setEnabled(false);
            m_deleteButton->setEnabled(false);
            m_editCopyButton->setEnabled(false);
            m_renameButton->setEnabled(false);
        }
    }
    else{
        m_progress->setTextVisible(false);
        m_progress->setMinimum(0);
        m_progress->setMaximum(1);
        m_progress->setValue(0);
    }
    onSectionChanged(m_sectionEdit->value());
}

void QLLTDock::onRenameButtonClicked() {

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    if (!m_module->GetQLLTSim()->getIsVawt()) g_QLLTHAWTSimulationStore.rename(m_shownLLTSimulation);
    else g_QLLTVAWTSimulationStore.rename(m_shownLLTSimulation);
}

void QLLTDock::onEditCopyButtonClicked() {

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    QLLTCreatorDialog *creatorDialog = new QLLTCreatorDialog (m_shownLLTSimulation, m_module, m_module->m_bisVAWT);
    creatorDialog->exec();
    delete creatorDialog;
}

void QLLTDock::onDeleteButtonClicked() {

    m_module->OnStopReplay();
    m_module->GetQLLTToolBar()->onStopReplay();

    if (!m_module->GetQLLTSim()->getIsVawt()) g_QLLTHAWTSimulationStore.remove(m_shownLLTSimulation);
    else g_QLLTVAWTSimulationStore.remove(m_shownLLTSimulation);
}

void QLLTDock::onLineButtonClicked() {
    LinePickerDlg linePicker;
    linePicker.InitDialog(m_simulationLineButton->GetStyle(), m_simulationLineButton->GetWidth(), m_simulationLineButton->GetColor());
    if(linePicker.exec() == QDialog::Accepted) 	{
        m_simulationLineButton->SetStyle(linePicker.GetStyle());
        m_simulationLineButton->SetWidth(linePicker.GetWidth());
        m_simulationLineButton->SetColor(linePicker.GetColor());

        m_shownLLTSimulation->setPen(m_simulationLineButton->getPen());
    }
}

void QLLTDock::onShowCheckBoxCanged () {
    m_shownLLTSimulation->setShownInGraph(m_showCheckBox->isChecked());
	m_module->reloadAllGraphCurves();
}

void QLLTDock::onShowPointsCheckBoxCanged () {
    m_shownLLTSimulation->setDrawPoints(m_showPointsCheckBox->isChecked());
    m_module->update();
}

void QLLTDock::onShowCurveCheckBoxCanged () {
    m_shownLLTSimulation->setDrawCurve(m_showCurveCheckBox->isChecked());
    m_module->update();
}

void QLLTDock::OnTwoDView(){
    m_vizbox->hide();
    m_cutBox->hide();
    cutVizBox->hide();
    m_stylebox->show();
    m_sectionBox->show();
}

void QLLTDock::OnGLView(){
    m_stylebox->hide();
    m_sectionBox->hide();
    m_vizbox->show();
    m_cutBox->show();
    cutVizBox->show();
}

void QLLTDock::OnHAWTView(){
    m_VAWTCutPlaneBox->hide();
    m_HAWTCutPlaneBox->show();
}

void QLLTDock::OnVAWTView(){
    m_HAWTCutPlaneBox->hide();
    m_VAWTCutPlaneBox->show();
}



