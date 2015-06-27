#include "noisetoolbar.h"

#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include "noisesimulation.h"

#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QMainWindow>


NoiseToolBar::NoiseToolBar(QMainWindow *parent, NoiseModule *module) :
    QToolBar(parent)
{
    m_Module = module;

    BpmteViewAction = new QAction(QIcon(":/images/2d-noise.png"), tr("Plot BPMTE Results in a Graph"), this);
    BpmteViewAction->setCheckable(true);
    BpmteViewAction->setStatusTip(tr("Plot BPMTE Results in a Graph"));

    Q3dViewAction = new QAction(QIcon(":/images/3d-noise.png"), tr("Plot Q3D Results in a Graph - NOT IMPLEMENTED YET"), this);
    Q3dViewAction->setCheckable(true);
    Q3dViewAction->setStatusTip(tr("Plot Q3D Results in a Graph - NOT IMPLEMENTED YET"));
    Q3dViewAction->setEnabled(false);

    connect (Q3dViewAction, SIGNAL(triggered(bool)), m_Module, SLOT(OnQ3dView()));
    connect (BpmteViewAction, SIGNAL(triggered(bool)), m_Module, SLOT(OnBpmteView()));

    connect (Q3dViewAction, SIGNAL(triggered(bool)), this, SLOT(OnQ3dView()));
    connect (BpmteViewAction, SIGNAL(triggered(bool)), this, SLOT(OnBpmteView()));

    addAction(BpmteViewAction);
    addAction(Q3dViewAction);
//    addSeparator();

//    QGroupBox *groupBox = new QGroupBox (tr("Noise Simulation"));
//    m_NoiseSimulationComboBox = new NoiseSimulationComboBox(&g_NoiseSimulationStore);
//    m_NoiseSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//    m_NoiseSimulationComboBox->setMinimumWidth(170);
//    connect (m_NoiseSimulationComboBox, SIGNAL(valueChanged(int)), m_Module, SLOT(OnSelChangeNoiseSimulation()));
//    QGridLayout *grid = new QGridLayout ();
//    grid->addWidget(m_NoiseSimulationComboBox, 0, 0);
//    groupBox->setLayout(grid);
//    addWidget(groupBox);

    parent->addToolBar(this);
    hide();


}

NoiseToolBar::~NoiseToolBar(){

}

void NoiseToolBar::OnQ3dView()
{
    Q3dViewAction->setChecked(true);
    BpmteViewAction->setChecked(false);
}

void NoiseToolBar::OnBpmteView()
{
    Q3dViewAction->setChecked(false);
    BpmteViewAction->setChecked(true);
}
