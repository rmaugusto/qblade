#include "TurDmsToolBar.h"

#include <QMainWindow>
#include <QAction>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDoubleSpinBox>

#include "TurDmsModule.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include "TDMSData.h"


TurDmsToolBar::TurDmsToolBar (QMainWindow *parent, TurDmsModule *module) 
	: SimulationToolBar()
{
	m_module = module;
	constructSimulationToolBar("Turbine", &g_verttdataStore, "Turbine Simulation", &g_tdmsdataStore);
	setObjectName("TurDmsToolBar");

	QGroupBox *groupBox = new QGroupBox ("Windspeed [m/s]");
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_windspeedComboBox = new QComboBox ();
			m_windspeedComboBox->setFixedWidth(100);
			connect (m_windspeedComboBox, SIGNAL(currentIndexChanged(int)),
					 m_module, SLOT(setShownWindspeedData(int)));
			vBox->addWidget(m_windspeedComboBox);
	groupBox = new QGroupBox ("Height Position");
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_heightPositionComboBox = new QComboBox ();
			m_heightPositionComboBox->setFixedWidth(100);
			connect (m_heightPositionComboBox, SIGNAL(currentIndexChanged(int)),
					 m_module, SLOT(setShownHeightIndex(int)));
			vBox->addWidget(m_heightPositionComboBox);
	groupBox = new QGroupBox ("Weibull Settings");
	addWidget(groupBox);
		QHBoxLayout *hBox = new QHBoxLayout ();
		groupBox->setLayout(hBox);
			QLabel *label = new QLabel("k =");
			hBox->addWidget(label);
			m_kEdit = new QDoubleSpinBox();
			m_kEdit->setMinimum(0.05);
			m_kEdit->setSingleStep(0.05);
			m_kEdit->setValue(2);
			connect(m_kEdit, SIGNAL(valueChanged(double)), m_module, SLOT(onWeibullChanged()));
			hBox->addWidget(m_kEdit);
			label = new QLabel("+-");
			hBox->addWidget(label);
			m_kMoreLessEdit = new QDoubleSpinBox();
			m_kMoreLessEdit->setMinimum(0.05);
			m_kMoreLessEdit->setSingleStep(0.05);
			m_kMoreLessEdit->setValue(3);
			connect(m_kMoreLessEdit, SIGNAL(valueChanged(double)), m_module, SLOT(onWeibullChanged()));
			hBox->addWidget(m_kMoreLessEdit);
			label = new QLabel("A =");
			hBox->addWidget(label);
			m_aEdit = new QDoubleSpinBox();
			m_aEdit->setMinimum(0.05);
			m_aEdit->setSingleStep(0.05);
			m_aEdit->setValue(9);
			connect(m_aEdit, SIGNAL(valueChanged(double)), m_module, SLOT(onWeibullChanged()));
			hBox->addWidget(m_aEdit);
			label = new QLabel("+-");
			hBox->addWidget(label);
			m_aMoreLessEdit = new QDoubleSpinBox();
			m_aMoreLessEdit->setMinimum(0.05);
			m_aMoreLessEdit->setSingleStep(0.05);
			m_aMoreLessEdit->setValue(3);
			connect(m_aMoreLessEdit, SIGNAL(valueChanged(double)), m_module, SLOT(onWeibullChanged()));
			hBox->addWidget(m_aMoreLessEdit);
	
	
	// must be here because Qt won't connect valueChanged(Parent*) because qmake can't resolve the template
	connect (m_parentComboBox, SIGNAL(valueChanged(TData*)), m_module, SLOT(setShownSimulationParent(TData*)));
	connect (m_simulationComboBox, SIGNAL(valueChanged(TDMSData*)), m_module, SLOT(setShownSimulation(TDMSData*)));
	connect(&g_tdmsdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(onSimulationStoreChange()));
			
	m_windspeedComboBox->setEnabled(false);
	m_heightPositionComboBox->setEnabled(false);
	onSimulationStoreChange();
			
	parent->addToolBar(this);
	hide();
}

void TurDmsToolBar::getWeibullParameters(double *k, double *kMoreLess, double *a, double *aMoreLess) {
	*k = m_kEdit->value();
	*kMoreLess = m_kMoreLessEdit->value();
	*a = m_aEdit->value();
	*aMoreLess = m_aMoreLessEdit->value();
}

SimulationModule<TData, TDMSData> *TurDmsToolBar::module() {
	return m_module;
}

void TurDmsToolBar::onSimulationStoreChange() {
	const bool simulationInStore = !g_tdmsdataStore.isEmpty();
	m_kEdit->setEnabled(simulationInStore);
	m_kMoreLessEdit->setEnabled(simulationInStore);
	m_aEdit->setEnabled(simulationInStore);
	m_aMoreLessEdit->setEnabled(simulationInStore);
}

void TurDmsToolBar::setShownSimulation(TDMSData *newSimulation) {
	m_simulationComboBox->setCurrentObject(newSimulation);
	
	/* handle windspeed and height */
	const QString oldWindspeed = m_windspeedComboBox->currentText();
	const QString oldHeight = m_heightPositionComboBox->currentText();
	m_windspeedComboBox->clear();
	m_heightPositionComboBox->clear();
	m_windspeedComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	m_heightPositionComboBox->setEnabled(newSimulation && newSimulation->hasResults());
	if (newSimulation && newSimulation->hasResults()) {
		for (int i = 0; i < newSimulation->m_DData.size(); ++i) {  // fill windspeed box
			m_windspeedComboBox->addItem(newSimulation->m_DData[i]->windspeedStr);
			if (newSimulation->m_DData[i]->windspeedStr == oldWindspeed) {
				m_windspeedComboBox->setCurrentIndex(i);
			}
		}
		for (int i = 0; i < m_module->getShownWindspeedData()->m_zeta.size(); ++i) {  // fill height box
			const QString height = QString::number((m_module->getShownWindspeedData()->m_zeta.at(i) + 1) / 2, 'f', 3);
			m_heightPositionComboBox->addItem(height);
			if (height == oldHeight) {
				m_heightPositionComboBox->setCurrentIndex(i);
			}
		}
	}
}

int TurDmsToolBar::getWindspeedIndex() {
	return m_windspeedComboBox->currentIndex();
}
