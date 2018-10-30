#include "NoiseToolBar.h"

#include <QAction>
#include <QGroupBox>
#include <QBoxLayout>
#include <QComboBox>
#include <QMainWindow>

#include "../Store.h"
#include "NoiseModule.h"
#include "NoiseSimulation.h"
#include "../StoreAssociatedComboBox.h"
#include "../Objects/OpPoint.h"


NoiseToolBar::NoiseToolBar(QMainWindow *parent, NoiseModule *module) {
	setObjectName("NoiseToolBar");

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));

    m_module = module;
	
	QAction *hideDocksAction = new QAction(QIcon(":/images/hidden.png"), "Hide Docks", this);
	hideDocksAction->setCheckable(true);
	hideDocksAction->setStatusTip("Hide Docks");
	connect(hideDocksAction, SIGNAL(toggled(bool)), m_module, SLOT(onHideDocks(bool)));
	addAction(hideDocksAction);
	
	QGroupBox *groupBox = new QGroupBox ("Noise Simulation");
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_simulationComboBox = new NoiseSimulationComboBox (&g_noiseSimulationStore);
			m_simulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_simulationComboBox->setMinimumWidth(170);
			vBox->addWidget(m_simulationComboBox);
	groupBox = new QGroupBox ("Operational Point");
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_opPointComboBox = new QComboBox ();
			m_opPointComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			vBox->addWidget(m_opPointComboBox);
			
	connect (m_simulationComboBox, SIGNAL(valueChanged(NoiseSimulation*)),
			 m_module, SLOT(setShownSimulation(NoiseSimulation*)));
	connect (m_opPointComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(onNeedUpdate()));
	
	parent->addToolBar(this);
	hide();
	
	setShownSimulation(NULL);
}

void NoiseToolBar::setShownSimulation(NoiseSimulation *newSimulation) {
	QString oldOpPoint = m_opPointComboBox->currentText();
	m_simulationComboBox->setCurrentObject(newSimulation);
	
	m_opPointComboBox->clear();
	if (newSimulation) {
		foreach (OpPoint* opPoint, newSimulation->getAnalyzedOpPoints()) {
			m_opPointComboBox->addItem(QString("%1").arg(opPoint->getName().toDouble(), 6, 'f', 2, QChar(' ')) +
									   " - " + opPoint->getParent()->getName() + " - " +
									   opPoint->getParent()->getParent()->getName());
		}
		m_opPointComboBox->setCurrentText(oldOpPoint);
	}
	m_opPointComboBox->setEnabled(m_opPointComboBox->currentIndex() != -1);
}

int NoiseToolBar::getShownOpPointIndex() {
	return m_opPointComboBox->currentIndex();
}
