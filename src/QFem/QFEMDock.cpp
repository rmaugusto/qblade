/****************************************************************************

    QFEMDock Class
        Copyright (C) 2014 David Marten david.marten@tu-berlin.de

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

#include "QFEMDock.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QProgressDialog>

#include "QFEMModule.h"
#include "../Misc/LineButton.h"
#include "../Misc/ColorButton.h"
#include "../Misc/NumberEdit.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include "QFEMToolBar.h"
#include "../XBEM/BData.h"
#include "../XBEM/TBEMData.h"
#include "../Globals.h"
#include "../Misc/FloatEditDelegate.h"
#include "StructDelegate.h"
#include "../GlobalFunctions.h"
#include "../Misc/LinePickerDlg.h"


extern bool ObjectIsEdited;

QFEMDock::QFEMDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QFEMModule *module)
	: ScrolledDock (title, parent, flags)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);

    m_LoadingShown = false;
	m_iSection = 0;
	m_module = module;
	m_pStructModel = NULL;
    m_pLoadingModel = NULL;
    structureWidget = new QStackedWidget;
    viewWidget = new QGroupBox(tr("3D View Control"));
    CurvesBox = new QGroupBox (tr("Curve Styles Control"));

    ModesWidget = new QGroupBox(tr("Mode View Control"));

    m_Loading = NULL;

    ModesTypesLayout = new QHBoxLayout;

    ModesWidget->setLayout(ModesTypesLayout);

    m_contentVBox->addWidget(viewWidget);
    m_contentVBox->addWidget(CurvesBox);
    m_contentVBox->addWidget(structureWidget);
    setVisible(false);



	/////////////////fill the material arrays with predefined structural properties//////////////
	MaterialsList.append(tr("User defined"));
    RhoList.append(2900);
    EModList.append(7.3e10);

    MaterialsList.append(tr("Triax (Exemplary)"));
    RhoList.append(1850);
    EModList.append(2.0e10);

    MaterialsList.append(tr("Double Bias (Exemplary)"));
    RhoList.append(1750);
    EModList.append(1.2e10);

    MaterialsList.append(tr("Foam (Exemplary)"));
    RhoList.append(200);
    EModList.append(0.0256e10);

    MaterialsList.append(tr("Polyurethane 20GF 65D"));
    RhoList.append(1360);
    EModList.append(0.172e10);

    MaterialsList.append(tr("6000 Series Aluminium"));
    RhoList.append(2740);
    EModList.append(7e10);

	MaterialsList.append(tr("7000 Series Aluminium"));
	RhoList.append(2900);
	EModList.append(7.3e10);

	MaterialsList.append(tr("1040 Carbon Steel"));
	RhoList.append(7845);
	EModList.append(20e10);




    //---------------------3D View Layout ---------------------------//

    QGridLayout *ThreeDView = new QGridLayout;

    m_pctrlPerspective = new QCheckBox(tr("Perspective View"));
    m_pctrlSurfaces = new QCheckBox(tr("Bottom Surface"));
    m_pctrlTopSurface = new QCheckBox(tr("Top Surface"));
    m_pctrlOutline = new QCheckBox(tr("Outlines"));
    m_pctrlAxes = new QCheckBox(tr("Coordinates"));
    m_pctrlRotor = new QCheckBox(tr("Show Rotor"));
    m_pctrlInternal = new QCheckBox(tr("Internal Structure"));

    QPushButton *m_pctrlLightDlg = new QPushButton(tr("GL settings"));
    QPushButton *m_pctrlResetView = new QPushButton(tr("Fit to Screen"));

    connect(m_pctrlSurfaces, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlTopSurface, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlOutline, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlPerspective, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlAxes, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlRotor, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));
    connect(m_pctrlInternal, SIGNAL(clicked()), m_module, SLOT(UpdateGeomRerenderGL()));


    connect(m_pctrlLightDlg, SIGNAL(clicked()), m_module, SLOT(OnLightDlg()));
    connect(m_pctrlResetView, SIGNAL(clicked()), m_module, SLOT(OnCenterScene()));
    connect(m_pctrlRotor, SIGNAL(clicked()), m_module, SLOT(OnCenterScene()));

    m_pctrlSurfaces->setChecked(true);
    m_pctrlTopSurface->setChecked(false);
    m_pctrlOutline->setChecked(true);
    m_pctrlPerspective->setChecked(false);
    m_pctrlAxes->setChecked(false);
    m_pctrlRotor->setChecked(false);
    m_pctrlInternal->setChecked(true);

    ThreeDView->addWidget(m_pctrlResetView,1,1);
    ThreeDView->addWidget(m_pctrlRotor,1,2);
    ThreeDView->addWidget(m_pctrlSurfaces,1,4);
    ThreeDView->addWidget(m_pctrlTopSurface,1,3);
    ThreeDView->addWidget(m_pctrlInternal,1,5);
    ThreeDView->addWidget(m_pctrlLightDlg,2,1);
    ThreeDView->addWidget(m_pctrlPerspective,2,2);
    ThreeDView->addWidget(m_pctrlAxes,2,3);
    ThreeDView->addWidget(m_pctrlOutline,2,4);

//    ThreeDView->addWidget(m_pctrlPositions,2,4);
//    ThreeDView->addWidget(m_pctrlFoilNames,2,5);

    QVBoxLayout *ViewLayout = new QVBoxLayout;
    ViewLayout->addLayout(ThreeDView);

    viewWidget->setLayout(ViewLayout);
//------------------2DViewLayout------------------------//

        QGridLayout *grid = new QGridLayout ();
        CurvesBox->setLayout(grid);
            int gridRowCount = 0;
            QLabel *label = new QLabel (tr("Style: "));
            grid->addWidget(label, gridRowCount, 0);

			m_simulationLineButton = new LineButton ();
            connect(m_simulationLineButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
            grid->addWidget(m_simulationLineButton, gridRowCount++, 1);
            QHBoxLayout *hBox = new QHBoxLayout ();
            grid->addLayout(hBox, gridRowCount++, 0, 1, 2);
                m_showCheckBox = new QCheckBox (tr("show"));
                connect(m_showCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCheckBoxCanged()));
                hBox->addWidget(m_showCheckBox);
                m_showCurveCheckBox = new QCheckBox (tr("curve"));
                connect(m_showCurveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCurveCheckBoxCanged()));
                hBox->addWidget(m_showCurveCheckBox);
                m_showPointsCheckBox = new QCheckBox (tr("points"));
                connect(m_showPointsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowPointsCheckBoxCanged()));
                hBox->addWidget(m_showPointsCheckBox);
                grid->setColumnStretch(2,100);

//-------------------- vsualize modes-------------------------//
    grid = new QGridLayout;

    QGroupBox *groupBox = new QGroupBox (tr("Mode Type"));
    m_pctrlModeTypeSelecta = new QComboBox;
    m_pctrlModeTypeSelecta->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_pctrlModeTypeSelecta->setMinimumWidth(120);
    m_pctrlModeTypeSelecta->setEnabled(true);
    grid = new QGridLayout ();
    grid->addWidget(m_pctrlModeTypeSelecta, 0, 0);
    groupBox->setLayout(grid);

    connect (m_pctrlModeTypeSelecta, SIGNAL(currentIndexChanged(int)), m_module, SLOT(OnSelChangeModeType(int)));
    ModesTypesLayout->addWidget(groupBox);



    groupBox = new QGroupBox (tr("Mode Number"));
    m_pctrlModeNumberSelecta = new QComboBox;
    m_pctrlModeNumberSelecta->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_pctrlModeNumberSelecta->setMinimumWidth(120);
    m_pctrlModeNumberSelecta->setEnabled(true);
    connect (m_pctrlModeNumberSelecta, SIGNAL(currentIndexChanged(int)), m_module, SLOT(OnSelChangeModeNumber(int)));
    grid = new QGridLayout ();
    grid->addWidget(m_pctrlModeNumberSelecta, 0, 0);
    groupBox->setLayout(grid);

    ModesTypesLayout->addWidget(groupBox);

    groupBox = new QGroupBox (tr("Shape Parameter"));
    m_modeSlider = new QSlider(Qt::Horizontal);
//    m_modeSlider->setMinimumWidth(150);
//    m_modeSlider->setMaximumWidth(120);
    m_modeSlider->setEnabled(true);
    m_modeSlider->setMinimum(-100);
    m_modeSlider->setMaximum(100);
    connect (m_modeSlider, SIGNAL(valueChanged(int)), m_module, SLOT(DeformBlade()));
    connect (m_modeSlider, SIGNAL(sliderReleased()), m_module, SLOT(SliderReleased()));
    connect (m_modeSlider, SIGNAL(sliderPressed()), m_module, SLOT(SliderPressed()));

    grid = new QGridLayout ();
    grid->addWidget(m_modeSlider, 0, 0);
    groupBox->setLayout(grid);


    ModesTypesLayout->addWidget(groupBox);




    //--------------------Structure Show Layout--------------//

    QHBoxLayout *EditNewLayout = new QHBoxLayout;
    QVBoxLayout *TopLayout = new QVBoxLayout;
    QVBoxLayout *StructureLayout = new QVBoxLayout;

	m_pctrlNameLabel = new QLabel;


    TopLayout->addWidget(m_pctrlNameLabel);
//    TopLayout->addLayout(ModesTypesLayout);
//    TopLayout->addWidget(m_modeSlider);




    label = new QLabel;

    QHBoxLayout *structureomegalayout = new QHBoxLayout;


    m_pctrlStructureLabel = new QLabel;
    m_pctrlOmegaLabel = new QLabel;

    structureomegalayout->addWidget(m_pctrlStructureLabel);
    structureomegalayout->addWidget(m_pctrlOmegaLabel);


    TopLayout->addLayout(structureomegalayout);


    QHBoxLayout *tophbox = new QHBoxLayout;
    QGroupBox *box = new QGroupBox(tr("Shell Material"));
    QHBoxLayout *hbox = new QHBoxLayout;
    QVBoxLayout *vbox = new QVBoxLayout;
    label = new QLabel(tr("E"));
    m_pctrlShellELabel = new QLabel;
    hbox->addWidget(label);
    hbox->addWidget(m_pctrlShellELabel);
	label = new QLabel(tr("Pa"));
	hbox->addWidget(label);
    vbox->addLayout(hbox);
    label = new QLabel(tr("Rho"));
    m_pctrlShellRhoLabel = new QLabel;
    hbox = new QHBoxLayout;
    hbox->addWidget(label);
    hbox->addWidget(m_pctrlShellRhoLabel);
	label = new QLabel(tr("kg/m^3"));
	hbox->addWidget(label);
    vbox->addLayout(hbox);
    box->setLayout(vbox);
    tophbox->addWidget(box);


    box = new QGroupBox(tr("Internal Material"));
    hbox = new QHBoxLayout;
    vbox = new QVBoxLayout;
    label = new QLabel(tr("E"));
    m_pctrlIntELabel = new QLabel;
    hbox->addWidget(label);
    hbox->addWidget(m_pctrlIntELabel);
	label = new QLabel(tr("Pa"));
	hbox->addWidget(label);
    vbox->addLayout(hbox);
    label = new QLabel(tr("Rho"));
    m_pctrlIntRhoLabel = new QLabel;
    hbox = new QHBoxLayout;
    hbox->addWidget(label);
    hbox->addWidget(m_pctrlIntRhoLabel);
	label = new QLabel(tr("kg/m^3"));
	hbox->addWidget(label);
    vbox->addLayout(hbox);
    box->setLayout(vbox);
    tophbox->addWidget(box);

    TopLayout->addLayout(tophbox);

    m_pctrlStructureTableView = new QTableView;

    m_pctrlStructureTableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_pctrlStructureTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    m_pctrlStructureTableView->setFixedHeight(380);
//    m_pctrlStructureTableView->setFixedWidth(520);

    m_pctrlEdit = new QPushButton(tr("Edit"));
    m_pctrlNew = new QPushButton(tr("New"));
    m_pctrlDelete = new QPushButton(tr("Delete"));


    EditNewLayout->addWidget(m_pctrlNew);
    connect(m_pctrlNew, SIGNAL(clicked()), this, SLOT (OnNew()));
    EditNewLayout->addWidget(m_pctrlEdit);
    connect(m_pctrlEdit, SIGNAL(clicked()), this, SLOT (OnEdit()));
    EditNewLayout->addWidget(m_pctrlDelete);
    connect(m_pctrlDelete, SIGNAL(clicked()), this, SLOT (OnDelete()));



    StructureLayout->addWidget(ModesWidget);
    StructureLayout->addLayout(TopLayout);
    StructureLayout->addWidget(m_pctrlStructureTableView);
    StructureLayout->addLayout(EditNewLayout);

    QGroupBox *StructureDataBox = new QGroupBox(tr("Structural Data"));
    StructureDataBox->setLayout(StructureLayout);

    //structureWidget->addWidget(ViewControl);

    m_tabwidget = new QTabWidget;



    m_tabwidget->addTab(StructureDataBox, "Structural Blade Design / Modal Analysis");
    m_tabwidget->setTabIcon(0,QIcon(":/images/struct.png"));
    m_tabwidget->setIconSize(QSize(36,36));

    //--------------Loading Show Layout-----------//

    m_LoadingTableView = new QTableView;

    m_LoadingTableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_LoadingTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    m_LoadingTableView->setFixedHeight(550);
//    m_LoadingTableView->setFixedWidth(520);

    m_editLoading = new QPushButton(tr("Edit"));
    m_newLoading = new QPushButton(tr("New"));
    m_deleteLoading = new QPushButton(tr("Delete"));

    QHBoxLayout *EditNewLoadingLayout = new QHBoxLayout;
    EditNewLoadingLayout->addWidget(m_newLoading);
    connect(m_newLoading, SIGNAL(clicked()), this, SLOT (OnNewLoading()));

    EditNewLoadingLayout->addWidget(m_editLoading);
    connect(m_editLoading, SIGNAL(clicked()), this, SLOT (OnEditLoading()));

    EditNewLoadingLayout->addWidget(m_deleteLoading);
    connect(m_deleteLoading, SIGNAL(clicked()), this, SLOT (OnDeleteLoading()));


    m_pctrlLoadingNameLabel = new QLabel;

    QVBoxLayout *LoadingShowLayout = new QVBoxLayout;
    LoadingShowLayout->addWidget(m_pctrlLoadingNameLabel);
    LoadingShowLayout->addWidget(m_LoadingTableView);
    LoadingShowLayout->addLayout(EditNewLoadingLayout);

    QGroupBox *LoadingDataBox = new QGroupBox(tr("Loading Data"));
    LoadingDataBox->setLayout(LoadingShowLayout);

    m_tabwidget->addTab(LoadingDataBox, "Static Blade Loading / Deflection");
    m_tabwidget->setTabIcon(1,QIcon(":/images/stress.png"));


    structureWidget->addWidget(m_tabwidget);

    connect(m_tabwidget, SIGNAL(currentChanged(int)), this, SLOT (OnTabChanged(int)));


    //-----------Structure Edit Layout-------------//
    QHBoxLayout *CancelSaveLayout = new QHBoxLayout;
    QVBoxLayout *TopEditLayout = new QVBoxLayout;
    QVBoxLayout *StructureEditLayout = new QVBoxLayout;

    m_pctrlStructureCombobox = new QComboBox;
	m_pctrlStructureCombobox->addItem(tr("Hollow with Spar"));
	m_pctrlStructureCombobox->addItem(tr("Hollow - No Spar"));
	m_pctrlStructureCombobox->addItem(tr("Solid - No Spar"));
//    m_pctrlStructureCombobox->setMaximumWidth(180);

	connect(m_pctrlStructureCombobox,  SIGNAL(currentIndexChanged(int)), m_module, SLOT(OnStructTypeChanged()));

    m_pctrlAlignSparMaxThickness = new QPushButton(tr("Align Spar at max Thickness"));

    connect(m_pctrlAlignSparMaxThickness,  SIGNAL(clicked()), m_module, SLOT(OnAlignSparAtMaxThickness()));


	m_pctrlInnerMaterial = new QComboBox;
//	m_pctrlInnerMaterial->setMaximumWidth(180);
	for (int i=0;i<MaterialsList.size();i++)
	{
		m_pctrlInnerMaterial->addItem(MaterialsList.at(i));
	}
	connect(m_pctrlInnerMaterial,  SIGNAL(currentIndexChanged(int)), m_module, SLOT(OnSelChangeInnerMaterial(int)));


	m_pctrlShellMaterial = new QComboBox;
//	m_pctrlShellMaterial->setMaximumWidth(180);
	for (int i=0;i<MaterialsList.size();i++)
	{
		m_pctrlShellMaterial->addItem(MaterialsList.at(i));
	}
	connect(m_pctrlShellMaterial,  SIGNAL(currentIndexChanged(int)), m_module, SLOT(OnSelChangeShellMaterial(int)));


    m_pctrlAllSections = new QPushButton(tr("Copy selected Section to ALL"));
//    m_pctrlAllSections->setMaximumWidth(180);
    connect(m_pctrlAllSections, SIGNAL(clicked()), this, SLOT (OnAllSections()));


    m_pctrlshellColor    = new ColorButton;
    m_pctrlsparColor    = new ColorButton;
	m_pctrlShellELineEdit = new NumberEdit(NumberEdit::Scientific);
	m_pctrlIntELineEdit = new NumberEdit(NumberEdit::Scientific);
	m_pctrlShellRhoLineEdit = new NumberEdit;
	m_pctrlIntRhoLineEdit = new NumberEdit;

    connect(m_pctrlshellColor, SIGNAL(clicked()),m_module, SLOT(OnShellColor()));
    connect(m_pctrlsparColor, SIGNAL(clicked()),m_module, SLOT(OnSparColor()));


    QHBoxLayout *NameOmegaLabelLayout = new QHBoxLayout;
    QLabel *structName = new QLabel(tr("Name of Structure"));
    QLabel *omegaLabel = new QLabel(tr("Rot. speed at design point [1/min]"));
    NameOmegaLabelLayout->addWidget(structName);
    NameOmegaLabelLayout->addWidget(omegaLabel);


    QHBoxLayout *NameOmegaLayout = new QHBoxLayout;
    m_pctrlDesignOmega = new NumberEdit;
	m_pctrlNameEdit = new QLineEdit;
    NameOmegaLayout->addWidget(m_pctrlNameEdit);
    NameOmegaLayout->addWidget(m_pctrlDesignOmega);


    TopEditLayout->addLayout(NameOmegaLabelLayout);
    TopEditLayout->addLayout(NameOmegaLayout);


    grid = new QGridLayout;
    label = new QLabel(tr("Internal Structure"));
    grid->addWidget(label,1,1);
    grid->addWidget(m_pctrlStructureCombobox,2,1);
    grid->addWidget(m_pctrlAlignSparMaxThickness,2,2);
    grid->addWidget(m_pctrlAllSections,2,3);
    TopEditLayout->addLayout(grid);


    AbsoluteShell = new QCheckBox(tr("Absolute Shell Thickness"));
    AbsoluteSpar = new QCheckBox(tr("Absolute Spar Thickness"));
    connect(AbsoluteShell, SIGNAL(clicked()), m_module, SLOT (OnChangeCoordinates()));
    connect(AbsoluteSpar, SIGNAL(clicked()), m_module, SLOT (OnChangeCoordinates()));

    tophbox = new QHBoxLayout;
    box = new QGroupBox(tr("Shell Material"));
    grid = new QGridLayout;
    label = new QLabel(tr("E"));
    grid->addWidget(m_pctrlshellColor,1,1);
	grid->addWidget(m_pctrlShellMaterial,1,2);
	grid->addWidget(label,2,1);
	grid->addWidget(m_pctrlShellELineEdit,2,2);
	label = new QLabel(tr("Pa"));
	grid->addWidget(label,2,3);
    label = new QLabel(tr("Rho"));
	grid->addWidget(label,3,1);
	grid->addWidget(m_pctrlShellRhoLineEdit,3,2);
	label = new QLabel(tr("kg/m^3"));
	grid->addWidget(label,3,3);
    grid->addWidget(AbsoluteShell,4,2);
    box->setLayout(grid);
    tophbox->addWidget(box);

    box = new QGroupBox(tr("Internal Material"));
    grid = new QGridLayout;
    label = new QLabel(tr("E"));
    grid->addWidget(m_pctrlsparColor,1,3);
	grid->addWidget(m_pctrlInnerMaterial,1,2);
	grid->addWidget(label,2,1);
	grid->addWidget(m_pctrlIntELineEdit,2,2);
	label = new QLabel(tr("Pa"));
	grid->addWidget(label,2,3);
    label = new QLabel(tr("Rho"));
	grid->addWidget(label,3,1);
	grid->addWidget(m_pctrlIntRhoLineEdit,3,2);
	label = new QLabel(tr("kg/m^3"));
	grid->addWidget(label,3,3);
    grid->addWidget(AbsoluteSpar,4,2);
    box->setLayout(grid);
    tophbox->addWidget(box);

    TopEditLayout->addLayout(tophbox);


    m_pctrlSave = new QPushButton(tr("Save"));
//    m_pctrlRename = new QPushButton(tr("Rename"));
    m_pctrlCancel = new QPushButton(tr("Cancel"));

    connect(m_pctrlSave, SIGNAL(clicked()), this, SLOT (OnSave()));
    CancelSaveLayout->addWidget(m_pctrlSave);
//    connect(m_pctrlRename, SIGNAL(clicked()), this, SLOT (OnRename()));
//    CancelSaveLayout->addWidget(m_pctrlRename);
    connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT (OnCancel()));
    CancelSaveLayout->addWidget(m_pctrlCancel);


    m_pctrlStructureTable = new QTableView(this);
//    m_pctrlStructureTable->setWindowTitle(QObject::tr("Structure definition"));
//    m_pctrlStructureTable->setFixedWidth(520);
    m_pctrlStructureTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pctrlStructureTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pctrlStructureTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                              QAbstractItemView::DoubleClicked |
                                                                              QAbstractItemView::SelectedClicked |
                                                                              QAbstractItemView::EditKeyPressed);


    StructureEditLayout->addLayout(TopEditLayout);
    StructureEditLayout->addWidget(m_pctrlStructureTable);
    StructureEditLayout->addLayout(CancelSaveLayout);

    QGroupBox *StructureEditBox = new QGroupBox(tr("Structural Data"));
    StructureEditBox->setLayout(StructureEditLayout);

    structureWidget->addWidget(StructureEditBox);

    //--------------LoadingEditLayout-----------//

    QHBoxLayout *ImportLoadingLayout = new QHBoxLayout;
    QHBoxLayout *LoadingCancelSaveLayout = new QHBoxLayout;
    QVBoxLayout *LoadingEditLayout= new QVBoxLayout;

    QLabel *info = new QLabel(tr("Import Loading from Turbine BEM Simulation at a choosen Windspeed"));

    groupBox = new QGroupBox (tr("Turbine"));
    m_TDataBox = new TDataComboBox (&g_tdataStore);
    m_TDataBox->setParentBox(m_module->m_QFEMToolBar->m_rotorComboBox);
    m_TDataBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_TDataBox->setMinimumWidth(170);
//    connect (m_BladeStructureComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeBladeStructure()));
    grid = new QGridLayout ();
    grid->addWidget(m_TDataBox, 0, 0);
    groupBox->setLayout(grid);
    ImportLoadingLayout->addWidget(groupBox);

    groupBox = new QGroupBox (tr("Simulation"));
    m_TBEMDataBox = new TBEMDataComboBox (&g_tbemdataStore);
    m_TBEMDataBox->setParentBox(m_TDataBox);
    m_TBEMDataBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_TBEMDataBox->setMinimumWidth(170);
    connect (m_TBEMDataBox, SIGNAL(valueChanged(int)), this, SLOT(OnFillWindspeeds()));
    grid = new QGridLayout ();
    grid->addWidget(m_TBEMDataBox, 0, 0);
    groupBox->setLayout(grid);
    ImportLoadingLayout->addWidget(groupBox);

    groupBox = new QGroupBox (tr("Windspeed"));
    m_WindspeedBox = new QComboBox;
    grid = new QGridLayout ();
    grid->addWidget(m_WindspeedBox, 0, 0);
    groupBox->setLayout(grid);
    ImportLoadingLayout->addWidget(groupBox);

    m_importLoading = new QPushButton(tr("Click to Import Loads"));
    connect (m_importLoading, SIGNAL(clicked()), this, SLOT(OnImportLoading()));




    m_saveLoading = new QPushButton(tr("Save"));
    m_cancelLoading = new QPushButton(tr("Cancel"));

    LoadingCancelSaveLayout->addWidget(m_saveLoading);
    connect(m_saveLoading, SIGNAL(clicked()), this, SLOT (OnSaveLoading()));
    LoadingCancelSaveLayout->addWidget(m_cancelLoading);
    connect(m_cancelLoading, SIGNAL(clicked()), this, SLOT (OnCancelLoading()));



    m_LoadingTable = new QTableView(this);
//    m_pctrlStructureTable->setWindowTitle(QObject::tr("Structure definition"));
//    m_LoadingTable->setFixedWidth(520);
    m_LoadingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_LoadingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_LoadingTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                              QAbstractItemView::DoubleClicked |
                                                                              QAbstractItemView::SelectedClicked |
                                                                              QAbstractItemView::EditKeyPressed);
    m_pctrlLoadingNameEdit = new QLineEdit;
    LoadingEditLayout->addWidget(m_pctrlLoadingNameEdit);
    LoadingEditLayout->addWidget(info);
    LoadingEditLayout->addLayout(ImportLoadingLayout);
    LoadingEditLayout->addWidget(m_importLoading);
    LoadingEditLayout->addWidget(m_LoadingTable);
    LoadingEditLayout->addLayout(LoadingCancelSaveLayout);

    QGroupBox *LoadingEditBox = new QGroupBox(tr("Loading Data"));
    LoadingEditBox->setLayout(LoadingEditLayout);

    structureWidget->addWidget(LoadingEditBox);

	addScrolledDock(Qt::LeftDockWidgetArea , parent);

    CheckButtons();

    SetShownBladeStructure(NULL);

    connect(this,SIGNAL(resized()),this,SLOT(OnResize()));

}

void QFEMDock::OnTabChanged (int i) {
    if (i == 1) {
        m_LoadingShown = true;
        m_module->m_internalChecked = m_module->m_QFEMDock->m_pctrlInternal->isChecked();
        m_module->m_QFEMDock->m_pctrlInternal->setChecked(false);
        m_module->m_QFEMDock->m_pctrlInternal->hide();

        if (!(g_mainFrame->m_iView == QFEMTWODVIEW)) g_mainFrame->m_iView = QFEMLOADINGVIEW;
        m_Loading = m_module->m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject();
        InitLoadingTable();
    } else {
        m_LoadingShown = false;
        m_module->m_QFEMDock->m_pctrlInternal->setChecked(m_module->m_internalChecked);
        m_module->m_QFEMDock->m_pctrlInternal->show();

        if (!(g_mainFrame->m_iView == QFEMTWODVIEW)) g_mainFrame->m_iView = QFEMSTRUCTVIEW;
        m_module->m_structure = m_module->m_QFEMToolBar->m_BladeStructureComboBox->currentObject();
        InitStructureTable();
    }
    m_module->DeformBlade();
    if (!(g_mainFrame->m_iView == QFEMTWODVIEW)) m_module->OnGLView();
}

void QFEMDock::OnImportLoading(){

    double normal;
    double tangential;
    double deltas, width;



    if (m_WindspeedBox->count()){

        m_Loading->tangentialLoading.clear();
        m_Loading->normalLoading.clear();
        m_Loading->sectionWidth.clear();

        BData *bdata = m_TBEMDataBox->currentObject()->GetBData().at(m_WindspeedBox->currentIndex());
        bdata->Init(m_module->m_rotor,0);
        for (int k=0;k<m_module->m_rotor->getNumberOfNodes();k++){
            normal = 0;
            tangential = 0;
            deltas = 0;
            width = 0;
            for (int j=0;j<bdata->elements;j++){

                if ( k==0 ){
                    if (bdata->m_pos.at(j)<(m_module->m_rotor->m_TPos[k]+m_module->m_rotor->m_TPos[k+1])/2){
                        tangential+=bdata->deltas.at(j)*bdata->m_p_tangential.at(j);
                        normal+=bdata->deltas.at(j)*bdata->m_p_normal.at(j);
                        deltas += bdata->deltas.at(j);
                        width = (m_module->m_rotor->m_TPos[k+1]-m_module->m_rotor->m_TPos[k])/2;

                    }
                }
                else if (k==m_module->m_rotor->m_NSurfaces){
                    if (bdata->m_pos.at(j)>=(m_module->m_rotor->m_TPos[k-1]+m_module->m_rotor->m_TPos[k])/2){
                        tangential+=bdata->deltas.at(j)*bdata->m_p_tangential.at(j);
                        normal+=bdata->deltas.at(j)*bdata->m_p_normal.at(j);
                        deltas += bdata->deltas.at(j);
                        width = (m_module->m_rotor->m_TPos[k]-m_module->m_rotor->m_TPos[k-1])/2;


                    }
                    }

                else {
                if (bdata->m_pos.at(j)<=(m_module->m_rotor->m_TPos[k]+m_module->m_rotor->m_TPos[k+1])/2 && bdata->m_pos.at(j)>(m_module->m_rotor->m_TPos[k-1]+m_module->m_rotor->m_TPos[k])/2){
                    tangential+=bdata->deltas.at(j)*bdata->m_p_tangential.at(j);
                    normal+=bdata->deltas.at(j)*bdata->m_p_normal.at(j);
                    deltas += bdata->deltas.at(j);
                    width = (m_module->m_rotor->m_TPos[k]-m_module->m_rotor->m_TPos[k-1])/2 +(m_module->m_rotor->m_TPos[k+1]-m_module->m_rotor->m_TPos[k])/2;

                }
                }


            }
            if (deltas == 0){
            m_Loading->tangentialLoading.append(0);
            m_Loading->normalLoading.append(0);
            }
            else{
            m_Loading->tangentialLoading.append(tangential/deltas*width);
            m_Loading->normalLoading.append(normal/deltas*width);
            }
            m_Loading->sectionWidth.append(width);


        }

        FillLoadingDataTable();

    }

}

void QFEMDock::OnFillWindspeeds(){

    m_WindspeedBox->clear();
    if (m_TBEMDataBox->currentObject()){
        TBEMData *tbem = m_TBEMDataBox->currentObject();
        for (int i=0; i< tbem->GetBData().size();i++){

            m_WindspeedBox->addItem(tbem->GetBData().at(i)->GetWindspeed());
        }
    }

    if (m_WindspeedBox->count()) m_WindspeedBox->setEnabled(true);
    else m_WindspeedBox->setEnabled(false);
}

void QFEMDock::OnResize(){

    m_pctrlStructureTableView->setMaximumWidth(0.9*this->width());
    m_pctrlStructureTableView->setMinimumWidth(0.9*this->width());

    if (m_module->m_structure){
        if (m_module->m_structure->StructType == 0)
        {
            int unitwidth = (int)((double)m_pctrlStructureTableView->width()/4.0);
            m_pctrlStructureTableView->setColumnWidth(0,unitwidth);
            m_pctrlStructureTableView->setColumnWidth(1,unitwidth);
            m_pctrlStructureTableView->setColumnWidth(2,unitwidth);
            m_pctrlStructureTableView->setColumnWidth(3,unitwidth);
            m_pctrlStructureLabel->setText("Internal Structure: Hollow with Spar");
        }
        if (m_module->m_structure->StructType == 1)
        {
            int unitwidth = (int)((double)m_pctrlStructureTableView->width()/4.0);
            m_pctrlStructureTableView->setColumnWidth(0,4*unitwidth);
            m_pctrlStructureTableView->setColumnWidth(1,0);
            m_pctrlStructureTableView->setColumnWidth(2,0);
            m_pctrlStructureTableView->setColumnWidth(3,0);
            m_pctrlStructureLabel->setText("Internal Structure: Hollow - No Spar");
        }
        if (m_module->m_structure->StructType == 2)
        {
            m_pctrlStructureTableView->setColumnWidth(0,0);
            m_pctrlStructureTableView->setColumnWidth(1,0);
            m_pctrlStructureTableView->setColumnWidth(2,0);
            m_pctrlStructureTableView->setColumnWidth(3,0);
            m_pctrlStructureLabel->setText("Internal Structure: Solid - No Spar");
        }
    }

    m_LoadingTable->setMaximumWidth(0.9*this->width());
    m_LoadingTable->setMinimumWidth(0.9*this->width());
    int unitwidth = (int)((double)m_LoadingTable->width()/3.0);
    m_LoadingTable->setColumnWidth(0,unitwidth);
    m_LoadingTable->setColumnWidth(1,unitwidth);
    m_LoadingTable->setColumnWidth(2,unitwidth);


    m_pctrlStructureTable->setMaximumWidth(0.9*this->width());
    m_pctrlStructureTable->setMinimumWidth(0.9*this->width());
    if(m_module->m_structure){
        if (m_module->m_structure->StructType == 0)
        {
            int unitwidth = (int)((double)m_pctrlStructureTable->width()/4.0);
            m_pctrlStructureTable->setColumnWidth(0,unitwidth);
            m_pctrlStructureTable->setColumnWidth(1,unitwidth);
            m_pctrlStructureTable->setColumnWidth(2,unitwidth);
            m_pctrlStructureTable->setColumnWidth(3,unitwidth);
        }
        if (m_module->m_structure->StructType == 1)
        {
            int unitwidth = (int)((double)m_pctrlStructureTable->width()/4.0);
            m_pctrlStructureTable->setColumnWidth(0,4*unitwidth);
            m_pctrlStructureTable->setColumnWidth(1,0);
            m_pctrlStructureTable->setColumnWidth(2,0);
            m_pctrlStructureTable->setColumnWidth(3,0);
        }
        if (m_module->m_structure->StructType == 2)
        {
        m_pctrlStructureTable->setColumnWidth(0,0);
        m_pctrlStructureTable->setColumnWidth(1,0);
        m_pctrlStructureTable->setColumnWidth(2,0);
        m_pctrlStructureTable->setColumnWidth(3,0);
        }
    }

    m_LoadingTableView->setMaximumWidth(0.9*this->width());
    m_LoadingTableView->setMinimumWidth(0.9*this->width());
    unitwidth = (int)((double)m_LoadingTableView->width()/3.0);
    m_LoadingTableView->setColumnWidth(0,unitwidth);
    m_LoadingTableView->setColumnWidth(1,unitwidth);
    m_LoadingTableView->setColumnWidth(2,unitwidth);
}

void QFEMDock::InitStructureTable()
{
	if (m_module->m_structure)
	{
	if (m_pStructModel) delete m_pStructModel;

	QString str;

	m_pctrlShellELabel->setText(str.number(m_module->m_structure->ShellEMod));
	m_pctrlIntELabel->setText(str.number(m_module->m_structure->SparEMod));
	m_pctrlShellRhoLabel->setText(str.number(m_module->m_structure->ShellRho));
	m_pctrlIntRhoLabel->setText(str.number(m_module->m_structure->SparRho));
    m_pctrlNameLabel->setText(m_module->m_structure->getName());
    m_pctrlOmegaLabel->setText("Design rot. speed: "+str.number(m_module->m_structure->Omega)+" [1/min]");

	m_pStructModel = new QStandardItemModel;
	m_pStructModel->setRowCount(100);//temporary
	m_pStructModel->setColumnCount(4);

    GetLengthUnit(str, g_mainFrame->m_LengthUnit);

    if (m_module->m_structure->AbsoluteShell) m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(")+str+")");
    else m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(%)"));

    if (m_module->m_structure->AbsoluteSpar) m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(")+str+")");
    else m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(%)"));

	m_pStructModel->setHeaderData(2, Qt::Horizontal, tr("Spar Position"));
	m_pStructModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Spar Angle"));

	m_pctrlStructureTableView->setModel(m_pStructModel);



    OnResize();


	m_iSection = -1;
	m_module->FillDataTable();
	m_module->ComputeGeometry();
	m_module->m_bResetglGeom = true;
    m_module->m_needToRerender = true;

    m_module->shellColor = m_module->m_structure->shellColor;
    m_module->sparColor = m_module->m_structure->sparColor;


    if (m_module->m_structure->QFEMCompleted)
    {
        m_module->QFEMCompleted = false;

        m_pctrlModeTypeSelecta->clear();
        m_pctrlModeTypeSelecta->addItem("Flapwise");
        m_pctrlModeTypeSelecta->addItem("Edgewise");
        m_pctrlModeTypeSelecta->addItem("Torsional");
        m_pctrlModeTypeSelecta->addItem("Longitudinal");
        m_pctrlModeTypeSelecta->addItem("Unsorted");

        m_pctrlModeNumberSelecta->setEnabled(true);
        m_pctrlModeTypeSelecta->setEnabled(true);
        m_modeSlider->setEnabled(true);

        m_module->QFEMCompleted = true;

    }
	}
	else
	{
		if (m_pStructModel) delete m_pStructModel;
		m_pStructModel = new QStandardItemModel;
		m_pStructModel->setRowCount(0);
		m_pStructModel->setColumnCount(0);
		m_pctrlStructureTableView->setModel(m_pStructModel);
		m_pctrlShellELabel->setText("");
		m_pctrlIntELabel->setText("");
		m_pctrlShellRhoLabel->setText("");
		m_pctrlIntRhoLabel->setText("");
        m_pctrlNameLabel->setText("-empty-");
        m_pctrlOmegaLabel->setText("");

        m_pctrlModeNumberSelecta->setEnabled(false);
        m_pctrlModeTypeSelecta->setEnabled(false);
        m_modeSlider->setEnabled(false);
        m_module->QFEMCompleted = false;
        m_module->m_bResetglGeom = true;
        m_module->m_needToRerender = true;
        m_module->reportGLChange();

        m_pctrlModeTypeSelecta->clear();
	}

}

bool QFEMDock::InitLoadingDialog(BladeStructureLoading *pLoading){

    m_iSection = 0;

    m_Loading = pLoading;

    m_pctrlLoadingNameEdit->setText(pLoading->getName());


    m_pLoadingModel = new QStandardItemModel;
    m_pLoadingModel->setRowCount(100);//temporary
    m_pLoadingModel->setColumnCount(3);

    QString str, str2;

    GetForceUnit(str, g_mainFrame->m_ForceUnit);
    GetLengthUnit(str2, g_mainFrame->m_LengthUnit);


    m_pLoadingModel->setHeaderData(0, Qt::Horizontal, tr("Radial Position(")+str2+")");

    m_pLoadingModel->setHeaderData(1, Qt::Horizontal, tr("Normal Loading(")+str+")");

    m_pLoadingModel->setHeaderData(2, Qt::Horizontal, tr("Tangential Loading")+str+")");

    m_LoadingTable->setModel(m_pLoadingModel);

    OnResize();

    QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pLoadingModel);
    m_LoadingTable->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), m_module, SLOT(OnItemClicked(QModelIndex)));



    m_pFloatDelegate = new FloatEditDelegate(this);
    m_pFloatDelegate->QFEM = true;
    m_LoadingTable->setItemDelegate(m_pFloatDelegate);
    connect(m_pFloatDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnLoadingCellChanged()));

    int  *precision = new int[3];
    precision[0] = 2;
    precision[1] = 2;
    precision[2] = 2;

    m_pFloatDelegate->SetPrecision(precision);


    FillLoadingDataTable();
    m_LoadingTable->selectRow(m_iSection);
    m_module->SetCurrentSection(m_iSection);

    m_module->reportGLChange();

    return true;

}

void QFEMDock::FillLoadingDataTable()
{
        if(!m_Loading) return;
        int i;
        m_pLoadingModel->setRowCount(m_Loading->normalLoading.size());

        for(i=0; i<m_Loading->normalLoading.size(); i++)
        {
                FillLoadingTableRow(i);
        }


}

void QFEMDock::FillLoadingTableRow(int row){

    QModelIndex ind;

    ind = m_pLoadingModel->index(row, 0, QModelIndex());
    m_pLoadingModel->setData(ind, m_Loading->m_structure->Radii[row]);

    ind = m_pLoadingModel->index(row, 1, QModelIndex());
    m_pLoadingModel->setData(ind, m_Loading->normalLoading[row]);

    ind = m_pLoadingModel->index(row, 2, QModelIndex());
    m_pLoadingModel->setData(ind, m_Loading->tangentialLoading[row]);

}

void QFEMDock::OnLoadingCellChanged(){

    ReadLoadingParams();
//    ComputeGeometry();
    m_module->reportGLChange();
}


void QFEMDock::ReadLoadingSectionData(int sel){

    if (sel >= m_pLoadingModel->rowCount())
        return;

    double d;
    bool bOK;
    QString strong;
    QStandardItem *pItem;

    pItem = m_pLoadingModel->item(sel,1);
    strong = pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_Loading->normalLoading[sel] = d;

    pItem = m_pLoadingModel->item(sel,2);
    strong = pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_Loading->tangentialLoading[sel] = d;

}


void QFEMDock::ReadLoadingParams(){

    if (!m_Loading) return;

    for (int i=0; i< m_pLoadingModel->rowCount();  i++) {
        ReadLoadingSectionData(i);
    }


}

void QFEMDock::CheckButtons()
{
    m_pctrlNew->setEnabled(g_rotorStore.size());
    m_pctrlEdit->setEnabled(m_module->m_structure);
    m_pctrlDelete->setEnabled(m_module->m_structure);

    m_newLoading->setEnabled(m_module->m_structure);
    m_editLoading->setEnabled(m_Loading);
    m_deleteLoading->setEnabled(m_Loading);


}



void QFEMDock::InitLoadingTable () {
	if (m_Loading) {
		if (m_pLoadingModel)
			delete m_pLoadingModel;
		
		m_pctrlLoadingNameLabel->setText(m_Loading->getName());
		
		m_pLoadingModel = new QStandardItemModel;
		m_pLoadingModel->setRowCount(100);//temporary
		m_pLoadingModel->setColumnCount(3);
		
		QString str, str2;
		
		GetForceUnit(str, g_mainFrame->m_ForceUnit);
		GetLengthUnit(str2, g_mainFrame->m_LengthUnit);
		
		m_pLoadingModel->setHeaderData(0, Qt::Horizontal, tr("Radial Position(")+str2+")");
		m_pLoadingModel->setHeaderData(1, Qt::Horizontal, tr("Normal Loading(")+str+")");
		m_pLoadingModel->setHeaderData(2, Qt::Horizontal, tr("Tangential Loading")+str+")");
		m_LoadingTableView->setModel(m_pLoadingModel);
		
        OnResize();

		m_iSection = -1;
		FillLoadingDataTable();
		m_module->ComputeGeometry();
		m_module->m_bResetglGeom = true;
		m_module->m_needToRerender = true;
		m_module->reportGLChange();
	} else {
		m_pctrlLoadingNameLabel->setText("-empty-");
		if (m_pLoadingModel) delete m_pLoadingModel;
		m_pLoadingModel = new QStandardItemModel;
		m_pLoadingModel->setRowCount(0);
		m_pLoadingModel->setColumnCount(0);
		m_LoadingTableView->setModel(m_pLoadingModel);
	}
}

bool QFEMDock::InitStructureDialog(BladeStructure *pStructure)
{
		m_module->m_bStructEdited = true;

        m_iSection = 0;

		if (m_pStructModel) delete m_pStructModel;

		m_module->m_structure = pStructure;

		m_pctrlSave->setEnabled(true);

		if(!m_module->m_structure) return false;

		m_module->DisableAll();

        m_pctrlNameEdit->setText(pStructure->getName());
        m_pctrlDesignOmega->setValue(m_module->m_structure->Omega);

		m_module->ComputeGeometry();

        AbsoluteShell->setChecked(m_module->m_structure->AbsoluteShell);
        AbsoluteShell->setChecked(m_module->m_structure->AbsoluteSpar);


		m_pStructModel = new QStandardItemModel;
		m_pStructModel->setRowCount(100);//temporary
		m_pStructModel->setColumnCount(4);

        QString str;

        GetLengthUnit(str, g_mainFrame->m_LengthUnit);

        if (m_module->m_structure->AbsoluteShell) m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(")+str+")");
        else m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(%)"));

        if (m_module->m_structure->AbsoluteSpar) m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(")+str+")");
        else m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(%)"));

		m_pStructModel->setHeaderData(2, Qt::Horizontal, tr("Spar Position"));
		m_pStructModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Spar Angle"));

		m_pctrlStructureTable->setModel(m_pStructModel);

        OnResize();

		QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pStructModel);
		m_pctrlStructureTable->setSelectionModel(selectionModel);
		connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), m_module, SLOT(OnItemClicked(QModelIndex)));



		m_pBladeDelegate = new StructDelegate(this);
		m_pctrlStructureTable->setItemDelegate(m_pBladeDelegate);
		connect(m_pBladeDelegate,  SIGNAL(closeEditor(QWidget *)), m_module, SLOT(OnCellChanged()));

		int  *precision = new int[3];
        precision[0] = 5;
        precision[1] = 5;
        precision[2] = 5;
        precision[3] = 5;


		m_pBladeDelegate->SetPointers(precision,&pStructure->m_numElems);

		m_pBladeDelegate->itemmodel = m_pStructModel;

		m_pctrlStructureCombobox->setCurrentIndex(m_module->m_structure->StructType);

        m_module->shellColor = pStructure->shellColor;
        m_module->sparColor = pStructure->sparColor;

        m_pctrlshellColor->SetColor(pStructure->shellColor);
        m_pctrlsparColor->SetColor(pStructure->sparColor);

		m_module->FillDataTable();
		m_pctrlStructureTable->selectRow(m_iSection);
		m_module->SetCurrentSection(m_iSection);
		m_module->m_bResetglGeom = true;
        m_module->m_needToRerender = true;
        m_module->m_newSectionHighlight = true;


		m_module->reportGLChange();

		return true;
}
void QFEMDock::OnNewLoading(){

    if (m_module->m_structure)
    {
    OnFillWindspeeds();

    ObjectIsEdited = true;

    structureWidget->setCurrentIndex(2);

    BladeStructureLoading *pLoading = new BladeStructureLoading("New Loading",m_module->m_structure);

    QString newname(m_module->m_structure->getName()+" Loading Data");
    for (int i=0;i<g_bladestructureloadingStore.size();i++){
        if (newname == g_bladestructureloadingStore.at(i)->getName()){
            newname = makeNameWithHigherNumber(newname);
            i = 0;
        }
    }

    pLoading->setName(newname);


    m_module->DisableAll();

    InitLoadingDialog(pLoading);

    }

}

void QFEMDock::OnEditLoading(){

    if (!m_Loading) return;

        OnFillWindspeeds();

        ObjectIsEdited = true;

        BladeStructureLoading *pLoading = new BladeStructureLoading(m_Loading->getName(), m_module->m_structure);

        pLoading->Duplicate(m_Loading);

        InitLoadingDialog(pLoading);

        structureWidget->setCurrentIndex(2);

}

void QFEMDock::OnDeleteLoading(){

    if (!m_Loading) return;

    QMessageBox msgBox;
    msgBox.setText(tr("Do you really want to Delete this Loading Table?"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
    if (msgBox.clickedButton() == cancelButton)
    {
    return;
    }


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {
        g_bladestructureloadingStore.remove(m_Loading);
    }

}

void QFEMDock::OnSaveLoading(){

    m_cancelCalculation = false;

    m_Loading->m_cancelCalculation = &m_cancelCalculation;

    m_progressStep = 0;
    m_progressStepShown = 0;
    m_progressDialog = new QProgressDialog ("", "Cancel", 0, 3*m_Loading->m_structure->m_numElems);
    m_progressDialog->setModal(true);
    m_progressDialog->setValue(0);
    m_progressDialog->show();
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(onProgressDialogCanceled()));
    connect(m_Loading, SIGNAL(updateProgress()), this, SLOT(onWindFieldProgress()));


    ReadLoadingParams();

    if (!m_Loading->RunModalTest()) return;

    m_Loading->setName(m_pctrlLoadingNameEdit->text());

    if (!g_bladestructureloadingStore.add(m_Loading)) m_Loading = NULL;

    if (m_Loading) m_module->m_QFEMToolBar->m_BladeStructureLoadingComboBox->setCurrentObject(m_Loading);
    else{
        m_module->m_QFEMToolBar->m_BladeStructureLoadingComboBox->setCurrentIndex(0);
        m_Loading = m_module->m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject();
    }

    InitLoadingTable();

    structureWidget->setCurrentIndex(0);
    m_module->EnableAll();
    m_module->DeformBlade();

    ObjectIsEdited = false;

    m_module->OnGLView();
}

void QFEMDock::OnCancelLoading(){
    QMessageBox msgBox;
    msgBox.setText(tr("If you cancel this loading data will be lost!"));
    msgBox.setInformativeText(tr("You want to proceed?"));
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    QPushButton *backButton = msgBox.addButton(tr("Back to Design"), QMessageBox::ActionRole);
    msgBox.exec();
    if (msgBox.clickedButton() == backButton)
    {
    return;
    }
    ObjectIsEdited = false;

    delete m_Loading;
    m_Loading = m_module->m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject();

    structureWidget->setCurrentIndex(0);

    m_module->EnableAll();


}

void QFEMDock::OnNew() {
	if (m_module->m_rotor) {
		ObjectIsEdited = true;
		structureWidget->setCurrentIndex(1);
		BladeStructure *pStructure = new BladeStructure("New Structure",m_module->m_rotor);
		
		QString newname(m_module->m_rotor->getName()+" Structural Model");
		for (int i=0;i<g_bladeStructureStore.size();i++){
			if (newname == g_bladeStructureStore.at(i)->getName()){
				newname = makeNameWithHigherNumber(newname);
                i = 0;
			}
		}
		
		pStructure->setName(newname);
		
		m_pctrlShellMaterial->setCurrentIndex(0);
		m_pctrlInnerMaterial->setCurrentIndex(0);
		m_pctrlShellELineEdit->setValue(pStructure->ShellEMod);
		m_pctrlShellRhoLineEdit->setValue(pStructure->ShellRho);
		m_pctrlIntELineEdit->setValue(pStructure->SparEMod);
		m_pctrlIntRhoLineEdit->setValue(pStructure->SparRho);
		
		InitStructureDialog(pStructure);
	}
}


void QFEMDock::OnEdit()
{

	if (m_module->m_structure)

	{
        ObjectIsEdited = true;
		BladeStructure *pStructure = new BladeStructure(m_module->m_structure->getName(), m_module->m_rotor);

		pStructure->Duplicate(m_module->m_structure);

		m_pctrlShellMaterial->setCurrentIndex(0);
		m_pctrlInnerMaterial->setCurrentIndex(0);
		m_pctrlShellELineEdit->setValue(pStructure->ShellEMod);
		m_pctrlShellRhoLineEdit->setValue(pStructure->ShellRho);
		m_pctrlIntELineEdit->setValue(pStructure->SparEMod);
		m_pctrlIntRhoLineEdit->setValue(pStructure->SparRho);

        InitStructureDialog(pStructure);

		structureWidget->setCurrentIndex(1);

//        InitStructureDialog(m_module->m_structure);
	}

}


void QFEMDock::OnDelete()
{
    if (!m_module->m_structure) return;

    QMessageBox msgBox;
    msgBox.setText(tr("Do you really want to Delete this Blade Structure?"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
    if (msgBox.clickedButton() == cancelButton)
    {
    return;
    }


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {
        g_bladeStructureStore.remove(m_module->m_structure);
    }
}

void QFEMDock::onProgressDialogCanceled() {
    m_cancelCalculation = true;
}

void QFEMDock::onWindFieldProgress() {
    m_progressStep++;

//    if (m_progressStepShown >= m_progressStep -30) {  // stackoverflow can occure if the difference is too large
        m_progressDialog->setValue(m_progressStep);
//    }

    QString curstation;
    curstation.sprintf("%.0f",double(int(m_progressStep/3)));
    QString text = "Compute structural properties for station " + curstation;
    m_progressDialog->setLabelText(text);

    m_progressStepShown++;
}

void QFEMDock::OnSave()
{

    m_cancelCalculation = false;

    m_module->m_structure->m_cancelCalculation = &m_cancelCalculation;

    m_progressStep = 0;
    m_progressStepShown = 0;
    m_progressDialog = new QProgressDialog ("", "Cancel", 0, 3*m_module->m_structure->m_numElems);
//    m_progressDialog->setCancelButton(NULL);
    m_progressDialog->setModal(true);
    m_progressDialog->setValue(0);
    m_progressDialog->show();
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(onProgressDialogCanceled()));
    connect(m_module->m_structure, SIGNAL(updateProgress()), this, SLOT(onWindFieldProgress()));

	m_module->ReadParams();

	m_module->m_structure->setName(m_pctrlNameEdit->text());

    m_module->m_structure->ShellRho = m_pctrlShellRhoLineEdit->getValue();
    m_module->m_structure->ShellEMod = m_pctrlShellELineEdit->getValue();
    m_module->m_structure->SparRho = m_pctrlIntRhoLineEdit->getValue();
    m_module->m_structure->SparEMod = m_pctrlIntELineEdit->getValue();


    m_module->m_structure->shellColor = m_module->shellColor;
    m_module->m_structure->sparColor = m_module->sparColor;
    m_module->m_structure->Omega = m_pctrlDesignOmega->getValue();


    if (!m_module->m_structure->RunModalTest()) return;

	if (!g_bladeStructureStore.add(m_module->m_structure))
	{
		m_module->m_structure = NULL;
	}

    if (m_module->m_structure) m_module->m_QFEMToolBar->m_BladeStructureComboBox->setCurrentObject(m_module->m_structure);
    else{
        m_module->m_QFEMToolBar->m_BladeStructureComboBox->setCurrentIndex(0);
        m_module->m_structure = m_module->m_QFEMToolBar->m_BladeStructureComboBox->currentObject();
    }

    m_modeSlider->setValue(0);

    InitStructureTable();

    ObjectIsEdited = false;

	m_module->m_bStructEdited = false;

	structureWidget->setCurrentIndex(0);

	m_module->EnableAll();

    m_module->DeformBlade();

	m_pctrlInnerMaterial->setCurrentIndex(0);
	m_pctrlShellMaterial->setCurrentIndex(0);


}


void QFEMDock::OnRename()
{

}

void QFEMDock::OnCancel()
{
	QMessageBox msgBox;
	msgBox.setText(tr("If you cancel the structural design will be lost!"));
	msgBox.setInformativeText(tr("You want to proceed?"));
	msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
	QPushButton *backButton = msgBox.addButton(tr("Back to Design"), QMessageBox::ActionRole);
	msgBox.exec();
	if (msgBox.clickedButton() == backButton)
	{
	return;
	}

    ObjectIsEdited = false;

	m_module->m_bStructEdited = false;

	delete m_module->m_structure;

    structureWidget->setCurrentIndex(0);

    m_module->m_structure = m_module->m_QFEMToolBar->m_BladeStructureComboBox->currentObject();

    InitStructureTable();

	m_module->EnableAll();

}

void QFEMDock::OnAllSections()
{
    for (int i=0;i<m_module->m_structure->m_numElems; i++)
	{
        if (m_module->m_structure->AbsoluteShell) m_module->m_structure->ShellThickness[i] = m_module->m_structure->ShellThickness[m_iSection] * m_module->m_structure->ChordLengths[m_iSection] / m_module->m_structure->ChordLengths[i];
        else m_module->m_structure->ShellThickness[i] = m_module->m_structure->ShellThickness[m_iSection];

        if (m_module->m_structure->AbsoluteSpar) m_module->m_structure->SparThickness[i] = m_module->m_structure->SparThickness[m_iSection] * m_module->m_structure->ChordLengths[m_iSection] / m_module->m_structure->ChordLengths[i];
        else m_module->m_structure->SparThickness[i] = m_module->m_structure->SparThickness[m_iSection];

        m_module->m_structure->SparLocation[i] = m_module->m_structure->SparLocation[m_iSection];

        m_module->m_structure->SparAngle[i] = m_module->m_structure->SparAngle[m_iSection];
	}
    m_module->FillDataTable();
    m_module->ReadParams();
    m_module->m_needToRerender = true;
    m_module->reportGLChange();

}

void QFEMDock::onLineButtonClicked() {
	LinePickerDlg linePicker;
	linePicker.InitDialog(m_simulationLineButton->GetStyle(), m_simulationLineButton->GetWidth(), m_simulationLineButton->GetColor());
	if(linePicker.exec() == QDialog::Accepted) 	{
		m_simulationLineButton->SetStyle(linePicker.GetStyle());
		m_simulationLineButton->SetWidth(linePicker.GetWidth());
		m_simulationLineButton->SetColor(linePicker.GetColor());
		
		m_module->m_structure->setPen(m_simulationLineButton->getPen());
	}
}

void QFEMDock::onShowCheckBoxCanged () {
    m_module->m_structure->setShownInGraph(m_showCheckBox->isChecked());
	m_module->reloadAllGraphCurves();
}

void QFEMDock::onShowPointsCheckBoxCanged () {
    m_module->m_structure->setDrawPoints(m_showPointsCheckBox->isChecked());
    m_module->update();
}

void QFEMDock::onShowCurveCheckBoxCanged () {
    m_module->m_structure->setDrawCurve(m_showCurveCheckBox->isChecked());
    m_module->update();
}

void QFEMDock::SetShownBladeStructure(BladeStructure *structure){
	if (structure) {
		m_simulationLineButton->setPen(structure->getPen());
		m_showCheckBox->setChecked(structure->isShownInGraph());
		m_showCurveCheckBox->setChecked(structure->isDrawCurve());
		m_showPointsCheckBox->setChecked(structure->isDrawPoints());
	} else {
		QPen pen;
		pen.setColor(QColor("lightgrey"));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		m_simulationLineButton->setPen(QPen());
	}
	
	m_showCheckBox->setEnabled(structure);
	m_showCurveCheckBox->setEnabled(structure);
	m_showPointsCheckBox->setEnabled(structure);
	m_simulationLineButton->setEnabled(structure);
}
