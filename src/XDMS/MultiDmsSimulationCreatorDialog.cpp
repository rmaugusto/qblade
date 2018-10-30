#include "MultiDmsSimulationCreatorDialog.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>

#include "CDMSData.h"
#include "../Store.h"
#include "MultiDmsModule.h"
#include "../ColorManager.h"
#include "../ParameterGrid.h"

typedef Parameter::CDMSData P;


MultiDmsSimulationCreatorDialog::MultiDmsSimulationCreatorDialog(CDMSData *presetSimulation, MultiDmsModule *module) 
	: m_module(module),
	  m_editedSimulation(presetSimulation)
{
	setWindowTitle("Multi Parameter DMS Simulation");	
	
	m_leftContentVBox->addWidget(constructParameterBox("VAWT Multi Parameter Simulation"));
	m_leftContentVBox->addWidget(constructCorrectionsBox());
	m_leftContentVBox->addStretch();
	
	QGroupBox *groupBox = new QGroupBox ("Variables");
	m_rightContentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
		grid->addEdit(P::WindspeedFrom, NumberEditType, new NumberEdit (), "Windspeed From []:", 1, SPEED);
		grid->addEdit(P::WindspeedTo, NumberEditType, new NumberEdit (), "Windspeed To []:", 20, SPEED);
		grid->addEdit(P::WindspeedDelta, NumberEditType, new NumberEdit (), "Windspeed Delta []:", 0.5, SPEED);
		grid->addEdit(P::RotationalFrom, NumberEditType, new NumberEdit (), "Rotational Speed From [1/min]:", 200);
		grid->addEdit(P::RotationalTo, NumberEditType, new NumberEdit (), "Rotational Speed To [1/min]:", 500);
		grid->addEdit(P::RotationalDelta, NumberEditType, new NumberEdit (), "Rotational Speed Delta [1/min]:", 100);
		grid->addEdit(P::PitchFrom, NumberEditType, new NumberEdit (), "Pitch Angle From [deg]:", 0);
		grid->addEdit(P::PitchTo, NumberEditType, new NumberEdit (), "Pitch Angle To [deg]:", 10);
		grid->addEdit(P::PitchDelta, NumberEditType, new NumberEdit (), "Pitch Angle Delta [deg]:", 1);
	m_rightContentVBox->addStretch();
		
	setUnitContainingLabels();
	initView();
}

void MultiDmsSimulationCreatorDialog::initView() {
	if (m_editedSimulation) {
		loadObject(m_editedSimulation);
	} else {
		get<QLineEdit>(P::Name)->setText(g_cdmsdataStore.getNextName(
											 m_module->getShownSimulationParent()->getName() + " MultiSim"));
		loadValuesFromSettings();
	}
}

void MultiDmsSimulationCreatorDialog::loadValuesFromSettings() {
	SimulationCreatorDialog::loadValuesFromSettings();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");
	
	get<NumberEdit>(P::WindspeedFrom)->setValue(settings.value("windspeedFrom", 1).toDouble() * getUnitFactor(SPEED));
	get<NumberEdit>(P::WindspeedTo)->setValue(settings.value("windspeedTo", 20).toDouble() * getUnitFactor(SPEED));
	get<NumberEdit>(P::WindspeedDelta)->setValue(settings.value("windspeedDelta", 0.5).toDouble() * getUnitFactor(SPEED));
	get<NumberEdit>(P::RotationalFrom)->setValue(settings.value("rotFrom", 200).toDouble());
	get<NumberEdit>(P::RotationalTo)->setValue(settings.value("rotTo", 500).toDouble());
	get<NumberEdit>(P::RotationalDelta)->setValue(settings.value("rotDelta", 100).toDouble());
	get<NumberEdit>(P::PitchFrom)->setValue(settings.value("pitchFrom", 0).toDouble());
	get<NumberEdit>(P::PitchTo)->setValue(settings.value("pitchTo", 10).toDouble());
	get<NumberEdit>(P::PitchDelta)->setValue(settings.value("pitchDelta", 1).toDouble());
	
	settings.endGroup();
}

void MultiDmsSimulationCreatorDialog::saveValuesToSettings() {
	SimulationCreatorDialog::saveValuesToSettings();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");

	settings.setValue("windspeedFrom", get<NumberEdit>(P::WindspeedFrom)->getValue() / getUnitFactor(SPEED));
	settings.setValue("windspeedTo", get<NumberEdit>(P::WindspeedTo)->getValue() / getUnitFactor(SPEED));
	settings.setValue("windspeedDelta", get<NumberEdit>(P::WindspeedDelta)->getValue() / getUnitFactor(SPEED));
	settings.setValue("rotFrom", get<NumberEdit>(P::RotationalFrom)->getValue());
	settings.setValue("rotTo", get<NumberEdit>(P::RotationalTo)->getValue());
	settings.setValue("rotDelta", get<NumberEdit>(P::RotationalDelta)->getValue());
	settings.setValue("pitchFrom", get<NumberEdit>(P::PitchFrom)->getValue());
	settings.setValue("pitchTo", get<NumberEdit>(P::PitchTo)->getValue());
	settings.setValue("pitchDelta", get<NumberEdit>(P::PitchDelta)->getValue());
	
	settings.endGroup();
}

void MultiDmsSimulationCreatorDialog::onCreateButtonClicked() {
	// TODO check if wind from < wind to etc.
	
	CDMSData *newSimulation = new CDMSData (this);
	newSimulation->setSingleParent(m_module->getShownSimulationParent());
	newSimulation->m_WingName = newSimulation->getParent()->getName();

	if (g_cdmsdataStore.add(newSimulation)) {
		newSimulation->pen()->setColor(g_colorManager.getLeastUsedColor(&g_cdmsdataStore));
		m_module->setShownSimulation(newSimulation, true);
		saveValuesToSettings();
		accept();  // leave dialog only if adding was successful
	}
}

void MultiDmsSimulationCreatorDialog::onRestoreDefaultClicked() {
	this->loadObject(NULL);
	get<QLineEdit>(P::Name)->setText(g_cdmsdataStore.getNextName(
										 m_module->getShownSimulationParent()->getName() + " MultiSim"));
}
