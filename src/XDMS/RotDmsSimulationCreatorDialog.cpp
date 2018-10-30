#include "RotDmsSimulationCreatorDialog.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>

#include "../Misc/NumberEdit.h"
#include "DMSData.h"
#include "../Store.h"
#include "RotDmsModule.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../ColorManager.h"
#include "../ParameterGrid.h"

typedef Parameter::DMSData P;


RotDmsSimulationCreatorDialog::RotDmsSimulationCreatorDialog(DMSData *presetSimulation, RotDmsModule *module)
	: m_module(module),
	  m_editedSimulation(presetSimulation)
{
	setWindowTitle("Rotor DMS Simulation");

	m_leftContentVBox->addWidget(constructParameterBox("VAWT Rotor Simulation"));
	m_leftContentVBox->addWidget(constructCorrectionsBox());

	QGroupBox *groupBox = new QGroupBox ("Variables");
	m_rightContentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::TipSpeedFrom, NumberEditType, new NumberEdit (), "Tip Speed Ratio From:", 1);
			grid->addEdit(P::TipSpeedTo, NumberEditType, new NumberEdit (), "Tip Speed Ratio To:", 10);
			grid->addEdit(P::TipSpeedDelta, NumberEditType, new NumberEdit (), "Tip Speed Ratio Delta:", 0.5);
			grid->addEdit(P::Windspeed, NumberEditType, new NumberEdit (), "Windspeed []:", 7, SPEED);
	m_rightContentVBox->addStretch();

	setUnitContainingLabels();
	initView();
}

void RotDmsSimulationCreatorDialog::initView() {
	if (m_editedSimulation) {
		loadObject(m_editedSimulation);
	} else {
		get<QLineEdit>(P::Name)->setText(g_dmsdataStore.getNextName(
											 m_module->getShownSimulationParent()->getName() + " RotSim"));
		loadValuesFromSettings();
	}
}

void RotDmsSimulationCreatorDialog::loadValuesFromSettings() {
	SimulationCreatorDialog::loadValuesFromSettings();

	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");

	get<NumberEdit>(P::TipSpeedFrom)->setValue(settings.value("tipSpeedFrom", 1).toDouble());
	get<NumberEdit>(P::TipSpeedTo)->setValue(settings.value("tipSpeedTo", 10).toDouble());
	get<NumberEdit>(P::TipSpeedDelta)->setValue(settings.value("tipSpeedDelta", 0.5).toDouble());
	get<NumberEdit>(P::Windspeed)->setValue(settings.value("windspeed", 7).toDouble() * getUnitFactor(SPEED));

	settings.endGroup();
}

void RotDmsSimulationCreatorDialog::saveValuesToSettings() {
	SimulationCreatorDialog::saveValuesToSettings();

	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");

	settings.setValue("tipSpeedFrom", get<NumberEdit>(P::TipSpeedFrom)->getValue());
	settings.setValue("tipSpeedTo", get<NumberEdit>(P::TipSpeedTo)->getValue());
	settings.setValue("tipSpeedDelta", get<NumberEdit>(P::TipSpeedDelta)->getValue());
	settings.setValue("windspeed", get<NumberEdit>(P::Windspeed)->getValue() / getUnitFactor(SPEED));

	settings.endGroup();
}

void RotDmsSimulationCreatorDialog::onCreateButtonClicked() {
	// TODO check if ratio from < ratio to etc.

	DMSData *newSimulation = new DMSData (this);
	newSimulation->setSingleParent(m_module->getShownSimulationParent());
	newSimulation->m_WingName = newSimulation->getParent()->getName();

	if (g_dmsdataStore.add(newSimulation)) {
		newSimulation->pen()->setColor(g_colorManager.getLeastUsedColor(&g_dmsdataStore));
		m_module->setShownSimulation(newSimulation, true);
		saveValuesToSettings();
		accept();  // leave dialog only if adding was successful
	}
}

void RotDmsSimulationCreatorDialog::onRestoreDefaultClicked() {
	this->loadObject(NULL);
	get<QLineEdit>(P::Name)->setText(g_dmsdataStore.getNextName(
										 m_module->getShownSimulationParent()->getName() + " RotSim"));
}
