#include "QFEMToolBar.h"

#include <QAction>
#include <QGroupBox>

//#include "../XGlobals.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"


QFEMToolBar::QFEMToolBar(QMainWindow* parent, QFEMModule* module)
{
    m_module = module;
//    HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
//    HideWidgetsAct->setCheckable(true);
//    HideWidgetsAct->setStatusTip(tr("Hide Widgets"));
//    //connect(HideWidgetsAct, SIGNAL(triggered()), this, SLOT(OnHideWidgets()));
//    addAction(HideWidgetsAct);
    HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
    HideWidgetsAct->setCheckable(true);
    HideWidgetsAct->setStatusTip(tr("Hide Widgets"));

    addAction(HideWidgetsAct);

    connect(HideWidgetsAct, SIGNAL(triggered()), m_module, SLOT(OnHideWidgets()));

    GLView = new QAction(QIcon(":/images/3dview.png"), tr("3D OpenGL View"), this);
    GLView->setCheckable(true);
    GLView->setStatusTip(tr("3D OpenGL View"));

    TwoDView = new QAction(QIcon(":/images/graph.png"), tr("Plot Results in a Graph"), this);
    TwoDView->setCheckable(true);
    TwoDView->setStatusTip(tr("Plot Results in a Graph"));

    connect (GLView, SIGNAL(triggered(bool)), m_module, SLOT(OnGLView()));
    connect (TwoDView, SIGNAL(triggered(bool)), m_module, SLOT(OnTwoDView()));


    addAction(GLView);
    addAction(TwoDView);
    addSeparator();

    setIconSize(QSize(36,36));

    QGroupBox *groupBox = new QGroupBox (tr("Rotor Blade"));
	m_rotorComboBox = new RotorComboBox (&g_rotorStore);
	m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_rotorComboBox->setMinimumWidth(170);

	connect (m_rotorComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeRotor()));

    QGridLayout *grid = new QGridLayout ();
	grid->addWidget(m_rotorComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

    groupBox = new QGroupBox (tr("Structural Model"));
	m_BladeStructureComboBox = new BladeStructureComboBox (&g_bladeStructureStore);
	m_BladeStructureComboBox->setParentBox(m_rotorComboBox);
	m_BladeStructureComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_BladeStructureComboBox->setMinimumWidth(170);
    connect (m_BladeStructureComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeBladeStructure()));
    grid = new QGridLayout ();
	grid->addWidget(m_BladeStructureComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

    groupBox = new QGroupBox (tr("Blade Static Loading"));
    m_BladeStructureLoadingComboBox = new BladeStructureLoadingComboBox(&g_bladestructureloadingStore);
    m_BladeStructureLoadingComboBox->setParentBox(m_BladeStructureComboBox);
    m_BladeStructureLoadingComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_BladeStructureLoadingComboBox->setMinimumWidth(170);
    connect (m_BladeStructureLoadingComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeLoading()));
    grid = new QGridLayout ();
    grid->addWidget(m_BladeStructureLoadingComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

	parent->addToolBar(this);
    hide();

}
