#include "noisetoolbar.h"
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include <QMainWindow>


NoiseToolBar::NoiseToolBar(QMainWindow *parent, NoiseModule *module) :
    QToolBar(parent)
{
    m_module = module;

    GLView = new QAction(QIcon(":/images/3dview.png"), tr("3D OpenGL View NOISE"), this);
    GLView->setCheckable(true);
    GLView->setStatusTip(tr("3D OpenGL View NOISE"));

    TwoDView = new QAction(QIcon(":/images/graph.png"), tr("Plot Results in a Graph NOISE"), this);
    TwoDView->setCheckable(true);
    TwoDView->setStatusTip(tr("Plot Results in a Graph"));

    connect (GLView, SIGNAL(triggered(bool)), m_module, SLOT(OnGLView()));
    connect (TwoDView, SIGNAL(triggered(bool)), m_module, SLOT(OnTwoDView()));


    addAction(GLView);
    addAction(TwoDView);
    addSeparator();

//    QGroupBox *groupBox = new QGroupBox (tr("LLT Simulation"));
//    m_LLTSimulationComboBox = new LLTSimulationComboBox(&g_QLLTHAWTSimulationStore);
//    m_LLTSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//    m_LLTSimulationComboBox->setMinimumWidth(170);
//    connect (m_LLTSimulationComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeLLTSimulation()));
//    QGridLayout *grid = new QGridLayout ();
//    grid->addWidget(m_LLTSimulationComboBox, 0, 0);
//    groupBox->setLayout(grid);
//    addWidget(groupBox);

    parent->addToolBar(this);
    hide();


}

NoiseToolBar::~NoiseToolBar(){

}
