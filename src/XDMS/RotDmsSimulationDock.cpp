#include "RotDmsSimulationDock.h"

#include <QMessageBox>
#include <QGroupBox>
#include <QSettings>

#include "RotDmsSimulationCreatorDialog.h"
#include "RotDmsModule.h"
#include "../Store.h"
#include "../ParameterGrid.h"

typedef Parameter::DMSData P;


RotDmsSimulationDock::RotDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags,
										   RotDmsModule *module)
	: SimulationDock (title, parent, flags),
	  m_module(module)
{
	setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::RightDockWidgetArea);

	QGroupBox *groupBox = new QGroupBox ("Parameters");  // TODO move this up in hierarchy
	m_contentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P> (this);
		groupBox->setLayout(grid);
			grid->addLabel(P::Rho, true, "Rho [kg/m^3]:");
			grid->addLabel(P::Viscosity, true, "Viscosity [kg/(m*s)]:");
			grid->addLabel(P::Discretize, true, "Number of Elements:");
			grid->addLabel(P::MaxIterations, true, "Max Iterations:");
			grid->addLabel(P::MaxEpsilon, true, "Max Epsilon:");
			grid->addLabel(P::RelaxFactor, true, "Relax. Factor:");
	groupBox = new QGroupBox ("Corrections");
	m_contentVBox->addWidget(groupBox);
		grid = new ParameterGrid<P> (this);
		groupBox->setLayout(grid);
			grid->addLabel(P::TipLoss, false, "Tip Loss:");
			grid->addLabel(P::VariableInduction, false, "Variable Induction Factors:");
	groupBox = new QGroupBox ("Variables");
	m_contentVBox->addWidget(groupBox);
		grid = new ParameterGrid<P> (this);
		groupBox->setLayout(grid);
			grid->addLabel(P::TipSpeedFrom, true, "Tip Speed From:");
			grid->addLabel(P::TipSpeedTo, true, "Tip Speed To:");
			grid->addLabel(P::TipSpeedDelta, true, "Tip Speed Delta:");
			grid->addLabel(P::Windspeed, true, "Windspeed []:", SPEED);


	m_contentVBox->addStretch();

	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();

	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	m_showSelectCheckBox->setChecked(settings.value("modules/RotDmsDock/showSelect", true).toBool());

	addScrolledDock (Qt::RightDockWidgetArea, parent);
}

RotDmsSimulationDock::~RotDmsSimulationDock() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	settings.setValue("modules/RotDmsDock/showSelect", m_showSelectCheckBox->isChecked());
}

SimulationModule<CBlade,DMSData>* RotDmsSimulationDock::module() {
	return m_module;
}

QList<NewGraph::GraphType> RotDmsSimulationDock::affectedGraphTypes() {
	QList<NewGraph::GraphType> list;
	list.append(NewGraph::TurbineRotor);
	list.append(NewGraph::TurbineBlade);
	list.append(NewGraph::TurbineAzimuthal);
	list.append(NewGraph::RotorLegend);
	return list;
}

void RotDmsSimulationDock::setShownObject(DMSData *newSimulation) {
	SimulationDock::setShownObject(newSimulation);

	loadObject(m_shownObject);
}

void RotDmsSimulationDock::onUnitsChanged() {
	setUnitContainingLabels();
	setShownObject(m_shownObject);  // reload object
}

void RotDmsSimulationDock::onEditCopyButtonClicked() {
	RotDmsSimulationCreatorDialog *creatorDialog = new RotDmsSimulationCreatorDialog (m_shownObject, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void RotDmsSimulationDock::onRenameButtonClicked() {
	g_dmsdataStore.rename(m_shownObject);
}

void RotDmsSimulationDock::onDeleteButtonClicked() {
	g_dmsdataStore.remove(m_shownObject);
}

void RotDmsSimulationDock::onNewButtonClicked() {
	/* check first, if all requirements are fullfilled */
	if (g_verticalRotorStore.size() == 0) {
		QMessageBox::critical(this, "Create new Rotor DMS Simulation",
							  "The following error(s) occured:\n\n - No VAWT Blade in database", QMessageBox::Ok);
		return;
	}

	RotDmsSimulationCreatorDialog *creatorDialog = new RotDmsSimulationCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}
