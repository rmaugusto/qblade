#include "noisedock.h"

NoiseDock::NoiseDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, NoiseModule *module) :
    ScrolledDock(title, parent, flags)
{

    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);
    setFloating(false);
    setVisible(false);

    m_module = module;

    QGroupBox *groupBox = new QGroupBox ("LLT XX");
    m_contentVBox->addWidget(groupBox);

    QGridLayout *grid = new QGridLayout ();
    groupBox->setLayout(grid);

    QPushButton *m_startSimulationButton = new QPushButton (tr("Start Simulation"));
    connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT (OnClickX()));
    grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);

}

void NoiseDock::initView(){

}

void NoiseDock::OnClickX(){

    setFloating( !isFloating() );

}
