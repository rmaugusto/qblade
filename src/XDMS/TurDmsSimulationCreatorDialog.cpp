#include "TurDmsSimulationCreatorDialog.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>

#include "../Misc/NumberEdit.h"
#include "TDMSData.h"
#include "../Store.h"
#include "TurDmsModule.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../ColorManager.h"
#include "../ParameterGrid.h"

typedef Parameter::TDMSData P;


TurDmsSimulationCreatorDialog::TurDmsSimulationCreatorDialog(TDMSData *presetSimulation, TurDmsModule *module) 
	: m_module(module),
	  m_editedSimulation(presetSimulation)
{
	setWindowTitle("Turbine DMS Simulation");	
	
	m_leftContentVBox->addWidget(constructParameterBox("VAWT Turbine Simulation"));
	m_leftContentVBox->addWidget(constructCorrectionsBox());
	
	QGroupBox *groupBox = new QGroupBox ("Variables");
	m_rightContentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::WindspeedFrom, NumberEditType, new NumberEdit (), "Windspeed From []:", 1, SPEED);
			grid->addEdit(P::WindspeedTo, NumberEditType, new NumberEdit (), "Windspeed To []:", 20, SPEED);
			grid->addEdit(P::WindspeedDelta, NumberEditType, new NumberEdit (), "Windspeed Delta []:", 0.5, SPEED);
	m_rightContentVBox->addStretch();
		
	setUnitContainingLabels();
	initView();
}

void TurDmsSimulationCreatorDialog::initView() {
	if (m_editedSimulation) {
		loadObject(m_editedSimulation);
	} else {
		get<QLineEdit>(P::Name)->setText(g_tdmsdataStore.getNextName(
											 m_module->getShownSimulationParent()->getName() + " TurSim"));
		loadValuesFromSettings();
	}
}

void TurDmsSimulationCreatorDialog::loadValuesFromSettings() {
	SimulationCreatorDialog::loadValuesFromSettings();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");
	
	get<NumberEdit>(P::WindspeedFrom)->setValue(settings.value("windspeedFrom", 1).toDouble() * getUnitFactor(SPEED));
	get<NumberEdit>(P::WindspeedTo)->setValue(settings.value("windspeedTo", 20).toDouble() * getUnitFactor(SPEED));
	get<NumberEdit>(P::WindspeedDelta)->setValue(settings.value("windspeedDelta", 0.5).toDouble() * getUnitFactor(SPEED));
	
	settings.endGroup();
}

void TurDmsSimulationCreatorDialog::saveValuesToSettings() {
	SimulationCreatorDialog::saveValuesToSettings();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");

	settings.setValue("windspeedFrom", get<NumberEdit>(P::WindspeedFrom)->getValue() / getUnitFactor(SPEED));
	settings.setValue("windspeedTo", get<NumberEdit>(P::WindspeedTo)->getValue() / getUnitFactor(SPEED));
	settings.setValue("windspeedDelta", get<NumberEdit>(P::WindspeedDelta)->getValue() / getUnitFactor(SPEED));
	
	settings.endGroup();
}

void TurDmsSimulationCreatorDialog::onCreateButtonClicked() {
	// TODO check if wind from < wind to etc.
	
	TDMSData *newSimulation = new TDMSData (this);
	newSimulation->setSingleParent(m_module->getShownSimulationParent());
	newSimulation->m_TurbineName = newSimulation->getParent()->getName();

	if (g_tdmsdataStore.add(newSimulation)) {
		newSimulation->pen()->setColor(g_colorManager.getLeastUsedColor(&g_tdmsdataStore));
		m_module->setShownSimulation(newSimulation, true);
		saveValuesToSettings();
		accept();  // leave dialog only if adding was successful
	}
}

void TurDmsSimulationCreatorDialog::onRestoreDefaultClicked() {
	this->loadObject(NULL);
	get<QLineEdit>(P::Name)->setText(g_tdmsdataStore.getNextName(
										 m_module->getShownSimulationParent()->getName() + " TurSim"));
}
