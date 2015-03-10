#include "noisedock.h"

#include "noisesimulation.h"


NoiseDock::NoiseDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, NoiseModule *module) :
    ScrolledDock(title, parent, flags)
{

    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);
    setVisible(false);

    m_module = module;

    QGroupBox *groupBox = new QGroupBox ("LLT XX");
    m_contentVBox->addWidget(groupBox);

    QGridLayout *grid = new QGridLayout ();
    groupBox->setLayout(grid);

    QPushButton *m_startSimulationButton = new QPushButton (tr("Start Simulation"));
    connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT (OnClickX()));
    grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);

    addScrolledDock(Qt::LeftDockWidgetArea , parent);


}

void NoiseDock::initView(){

}

void NoiseDock::OnClickX(){

NoiseSimulationDialog * d = new NoiseSimulationDialog();
d->exec();


//    NoiseSimulation * ns = new NoiseSimulation();
//    g_NoiseSimulationStore.add(ns);

//    QLLTSimulation *simulation =
//            new QLLTSimulation();
//    g_QLLTHAWTSimulationStore.add(simulation);

}

NoiseDock::~NoiseDock(){

}
