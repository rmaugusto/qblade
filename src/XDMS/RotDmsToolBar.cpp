#include "RotDmsToolBar.h"

#include <QAction>
#include <QGroupBox>
#include <QVBoxLayout>

#include "RotDmsModule.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"


RotDmsToolBar::RotDmsToolBar (QMainWindow *parent, RotDmsModule *module)
	: SimulationToolBar()
{
	m_module = module;
	constructSimulationToolBar("Rotor", &g_verticalRotorStore, "Rotor Simulation", &g_dmsdataStore);
	setObjectName("RotDmsToolBar");

	QGroupBox *groupBox = new QGroupBox ("Tip Speed Ratio");
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout;
		groupBox->setLayout(vBox);
			m_tipSpeedComboBox = new QComboBox;
			m_tipSpeedComboBox->setEnabled(false);
			connect (m_tipSpeedComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(setShownTipSpeedIndex(int)));
			vBox->addWidget(m_tipSpeedComboBox);
	groupBox = new QGroupBox ("Height Position");
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_heightComboBox = new QComboBox;
			m_heightComboBox->setEnabled(false);
			connect (m_heightComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(setShownHeightIndex(int)));
			vBox->addWidget(m_heightComboBox);


	// must be here because Qt won't connect valueChanged(Parent*) because qmake can't resolve the template
	connect (m_parentComboBox, SIGNAL(valueChanged(CBlade*)), m_module, SLOT(setShownSimulationParent(CBlade*)));
	connect (m_simulationComboBox, SIGNAL(valueChanged(DMSData*)), m_module, SLOT(setShownSimulation(DMSData*)));

	parent->addToolBar(this);
	hide();
}

SimulationModule<CBlade, DMSData> *RotDmsToolBar::module() {
	return m_module;
}

void RotDmsToolBar::setShownSimulation(DMSData *newSimulation) {
	m_simulationComboBox->setCurrentObject(newSimulation);

	/* handle tip speed ratio and height boxes */
	const QString oldTipSpeed = m_tipSpeedComboBox->currentText();
	const QString oldHeight = m_heightComboBox->currentText();
	m_tipSpeedComboBox->clear();
	m_heightComboBox->clear();
	m_tipSpeedComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	m_heightComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	if (newSimulation && newSimulation->hasResults()) {
		for (int i = 0; i < newSimulation->m_DData.size(); ++i) {
			const QString itemText = newSimulation->m_DData[i]->lambdaglobal;
			m_tipSpeedComboBox->addItem(itemText);
			if (itemText == oldTipSpeed) {
				m_tipSpeedComboBox->setCurrentIndex(i);
			}
		}
		for (int i = 0; i < m_module->getShownTipSpeedData()->m_zeta.size(); ++i) {  // fill height box
			const QString height = QString::number((m_module->getShownTipSpeedData()->m_zeta.at(i) + 1) / 2, 'f', 3);
			m_heightComboBox->addItem(height);
			if (height == oldHeight) {
				m_heightComboBox->setCurrentIndex(i);
			}
		}
	}
}

int RotDmsToolBar::getTipSpeedIndex() {
	return m_tipSpeedComboBox->currentIndex();
}
