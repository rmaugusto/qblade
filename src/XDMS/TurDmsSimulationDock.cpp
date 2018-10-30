#include "TurDmsSimulationDock.h"

#include <QMessageBox>
#include <QGroupBox>
#include <QSettings>

#include "TurDmsSimulationCreatorDialog.h"
#include "../Store.h"
#include "TurDmsModule.h"
#include "../ParameterGrid.h"
#include "../MainFrame.h"

typedef Parameter::TDMSData P;


TurDmsSimulationDock::TurDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags,
										   TurDmsModule *module)
	: SimulationDock (title, parent, flags),
	  m_module(module)
{
	setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::RightDockWidgetArea);
	
	QGroupBox *groupBox = new QGroupBox ("Power Yield");
	m_contentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P> (this);
		groupBox->setLayout(grid);
			grid->addLabel(P::AnnualYield, true, "Annual Yield []:", POWER);
	groupBox = new QGroupBox ("Parameters");
	m_contentVBox->addWidget(groupBox);
		grid = new ParameterGrid<P> (this);
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
			grid->addLabel(P::WindspeedFrom, true, "Windspeed From []:", SPEED);
			grid->addLabel(P::WindspeedTo, true, "Windspeed To []:", SPEED);
			grid->addLabel(P::WindspeedDelta, true, "Windspeed Delta []:", SPEED);
	
	
	m_contentVBox->addStretch();	
	
	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	m_showSelectCheckBox->setChecked(settings.value("modules/TurDmsDock/showSelect", true).toBool());

	addScrolledDock (Qt::RightDockWidgetArea, parent);	
}

TurDmsSimulationDock::~TurDmsSimulationDock() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	settings.setValue("modules/TurDmsDock/showSelect", m_showSelectCheckBox->isChecked());
}

SimulationModule<TData,TDMSData>* TurDmsSimulationDock::module() {
	return m_module;
}

QList<NewGraph::GraphType> TurDmsSimulationDock::affectedGraphTypes() {
	QList<NewGraph::GraphType> list;
	list.append(NewGraph::TurbineRotor);
	list.append(NewGraph::TurbineWeibull);
	list.append(NewGraph::TurbineLegend);
	return list;
}

void TurDmsSimulationDock::setShownObject(TDMSData *newSimulation) {
	SimulationDock::setShownObject(newSimulation);
	
	loadObject(m_shownObject);
}

void TurDmsSimulationDock::onUnitsChanged() {
	setUnitContainingLabels();
	QLabel *label = getUnitLabel(P::AnnualYield);  // add h to the unit
	label->setText(label->text().replace("]", "h]"));
	
	setShownObject(m_shownObject);  // reload object
}

void TurDmsSimulationDock::onEditCopyButtonClicked() {
	TurDmsSimulationCreatorDialog *creatorDialog = new TurDmsSimulationCreatorDialog (m_shownObject, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void TurDmsSimulationDock::onRenameButtonClicked() {
	g_tdmsdataStore.rename(m_shownObject);
}

void TurDmsSimulationDock::onDeleteButtonClicked() {
	g_tdmsdataStore.remove(m_shownObject);
}

void TurDmsSimulationDock::onNewButtonClicked() {
	/* check first, if all requirements are fullfilled */
    if (g_verttdataStore.size() == 0) {
        QMessageBox::critical(this, "Create new Turbine DMS Simulation",
							  "The following error(s) occured:\n\n - No VAWT Turbine in database", QMessageBox::Ok);
        return;
    }

	TurDmsSimulationCreatorDialog *creatorDialog = new TurDmsSimulationCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}
