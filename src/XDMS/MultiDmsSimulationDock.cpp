#include "MultiDmsSimulationDock.h"

#include <QMessageBox>
#include <QGroupBox>
#include <QSettings>

#include "MultiDmsSimulationCreatorDialog.h"
#include "../Store.h"
#include "MultiDmsModule.h"
#include "../ParameterGrid.h"
#include "CDMSData.h"

typedef Parameter::CDMSData P;


MultiDmsSimulationDock::MultiDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags,
											   MultiDmsModule *module)
	: SimulationDock (title, parent, flags),
	  m_module(module)
{
	setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::RightDockWidgetArea);
	
	QGroupBox *groupBox = new QGroupBox ("Parameters");
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
			grid->addLabel(P::WindspeedFrom, true, "Windspeed From []:", SPEED);
			grid->addLabel(P::WindspeedTo, true, "Windspeed To []:", SPEED);
			grid->addLabel(P::WindspeedDelta, true, "Windspeed Delta []:", SPEED);
			grid->addLabel(P::RotationalFrom, true, "Rotational Speed From [1/min]:");
			grid->addLabel(P::RotationalTo, true, "Rotational Speed To [1/min]:");
			grid->addLabel(P::RotationalDelta, true, "Rotational Speed Delta [1/min]:");
			grid->addLabel(P::PitchFrom, true, "Pitch Angle From [deg]:");
			grid->addLabel(P::PitchTo, true, "Pitch Angle To [deg]:");
			grid->addLabel(P::PitchDelta, true, "Pitch Angle Delta [deg]:");
	
	
	m_contentVBox->addStretch();	
	
	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	m_showSelectCheckBox->setChecked(settings.value("modules/MultiDmsDock/showSelect", true).toBool());
	
	addScrolledDock (Qt::RightDockWidgetArea, parent);	
}

MultiDmsSimulationDock::~MultiDmsSimulationDock() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	settings.setValue("modules/MultiDmsDock/showSelect", m_showSelectCheckBox->isChecked());
}

SimulationModule<CBlade,CDMSData>* MultiDmsSimulationDock::module() {
	return m_module;
}

QList<NewGraph::GraphType> MultiDmsSimulationDock::affectedGraphTypes() {
	return QList<NewGraph::GraphType> ();  // TODO implement this!
}

void MultiDmsSimulationDock::setShownObject(CDMSData *newSimulation) {
	SimulationDock::setShownObject(newSimulation);
	
	loadObject(m_shownObject);
}

void MultiDmsSimulationDock::onUnitsChanged() {
	setUnitContainingLabels();
	setShownObject(m_shownObject);  // reload object
}

void MultiDmsSimulationDock::onEditCopyButtonClicked() {
	MultiDmsSimulationCreatorDialog *creatorDialog = new MultiDmsSimulationCreatorDialog (m_shownObject, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void MultiDmsSimulationDock::onRenameButtonClicked() {
	g_cdmsdataStore.rename(m_shownObject);
}

void MultiDmsSimulationDock::onDeleteButtonClicked() {
	g_cdmsdataStore.remove(m_shownObject);
}

void MultiDmsSimulationDock::onNewButtonClicked() {
	/* check first, if all requirements are fullfilled */
    if (g_verticalRotorStore.size() == 0) {
        QMessageBox::critical(this, "Create new Multi Parameter DMS Simulation",
							  "The following error(s) occured:\n\n - No VAWT Blade in database", QMessageBox::Ok);
        return;
    }

	MultiDmsSimulationCreatorDialog *creatorDialog = new MultiDmsSimulationCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}
