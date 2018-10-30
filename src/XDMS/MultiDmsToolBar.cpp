#include "MultiDmsToolBar.h"

#include <QMainWindow>
#include <QAction>
#include <QGroupBox>
#include <QVBoxLayout>

#include "MultiDmsModule.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"


MultiDmsToolBar::MultiDmsToolBar (QMainWindow *parent, MultiDmsModule *module) 
	: SimulationToolBar()
{
	m_module = module;
	constructSimulationToolBar("Rotor", &g_verticalRotorStore, "Multi Parameter Simulation", &g_cdmsdataStore);
	setObjectName("MultiDmsToolBar");

	QGroupBox *groupBox = new QGroupBox ("Windspeed [m/s]");
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_windspeedComboBox = new QComboBox ();
			m_windspeedComboBox->setEnabled(false);
			connect (m_windspeedComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(setShownWindspeedIndex(int)));
			vBox->addWidget(m_windspeedComboBox);
	groupBox = new QGroupBox ("Rotational Speed [1/min]");
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_rotationalComboBox = new QComboBox ();
			m_rotationalComboBox->setEnabled(false);
			connect (m_rotationalComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(setShownRotationalIndex(int)));
			vBox->addWidget(m_rotationalComboBox);
	groupBox = new QGroupBox ("Pitch [deg]");
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_pitchComboBox = new QComboBox ();
			m_pitchComboBox->setEnabled(false);
			connect (m_pitchComboBox, SIGNAL(currentIndexChanged(int)), m_module, SLOT(setShownPitchIndex(int)));
			vBox->addWidget(m_pitchComboBox);
		
	
	// must be here because Qt won't connect valueChanged(Parent*) because qmake can't resolve the template
	connect (m_parentComboBox, SIGNAL(valueChanged(CBlade*)), m_module, SLOT(setShownSimulationParent(CBlade*)));
	connect (m_simulationComboBox, SIGNAL(valueChanged(CDMSData*)), m_module, SLOT(setShownSimulation(CDMSData*)));
			
	parent->addToolBar(this);
	hide();
}

SimulationModule<CBlade, CDMSData> *MultiDmsToolBar::module() {
	return m_module;
}

void MultiDmsToolBar::setShownSimulation(CDMSData *newSimulation) {
	m_simulationComboBox->setCurrentObject(newSimulation);
	
	/* handle windspeed, rotational speed and pitch */
	const QString oldWindspeed = m_windspeedComboBox->currentText();
	const QString oldRotational = m_rotationalComboBox->currentText();
	const QString oldPitch = m_pitchComboBox->currentText();
	m_windspeedComboBox->clear();
	m_rotationalComboBox->clear();
	m_pitchComboBox->clear();
	m_windspeedComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	m_rotationalComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	m_pitchComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	if (newSimulation && newSimulation->hasResults()) {
		for (int i = 0; i < newSimulation->windtimes; ++i) {
			const QString itemText = QString::number(newSimulation->windstart + newSimulation->winddelta * i, 'f', 2);
			m_windspeedComboBox->addItem(itemText);
			if (itemText == oldWindspeed) {
				m_windspeedComboBox->setCurrentIndex(i);
			}
		}
		for (int i = 0; i < newSimulation->rottimes; ++i) {
			const QString itemText = QString::number(newSimulation->rotstart + newSimulation->rotdelta * i, 'f', 2);
			m_rotationalComboBox->addItem(itemText);
			if (itemText == oldRotational) {
				m_rotationalComboBox->setCurrentIndex(i);
			}
		}
		for (int i = 0; i < newSimulation->pitchtimes; ++i) {
			const QString itemText = QString::number(newSimulation->pitchstart + newSimulation->pitchdelta * i, 'f', 2);
			m_pitchComboBox->addItem(itemText);
			if (itemText == oldPitch) {
				m_pitchComboBox->setCurrentIndex(i);
			}
		}
	}
}
