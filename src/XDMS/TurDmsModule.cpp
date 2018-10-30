#include "TurDmsModule.h"

#include <QMenuBar>
#include <QSettings>

#include "TDMSData.h"
#include "TurDmsTurbineDock.h"
#include "TurDmsSimulationDock.h"
#include "TurDmsContextMenu.h"
#include "TurDmsToolBar.h"
#include "../TwoDWidget.h"
#include "../MainFrame.h"
#include "../Store.h"
#include "../TwoDGraphMenu.h"


TurDmsModule::TurDmsModule(QMainWindow *mainWindow, QToolBar *toolbar) {
	m_globalModuleIndentifier = TURDMSMODULE;
	m_shownWindspeedData = NULL;
	m_shownHeightIndex = -1;
	
	m_turbineDock = new TurDmsTurbineDock ("VAWT Turbine", mainWindow, 0, this);
	m_simulationDock = new TurDmsSimulationDock ("VAWT Simulation", mainWindow, 0, this);
	
	m_contextMenu = new TurDmsContextMenu (m_twoDWidget, this);
	setContextMenu(m_contextMenu);
	
	m_toolBar = new TurDmsToolBar (mainWindow, this);
	onWeibullChanged();	
	
	registrateAtToolbar(tr("Turbine DMS Simulation"), tr("Prepare and run a DMS turbine simulation"),
						":/images/vturb.png", toolbar);
	
	connect(&g_tdmsdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadAllGraphs()));
	connect(&g_verttdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadTDataGraphs()));
}

TurDmsModule::~TurDmsModule() {
	if (m_firstView == false) {
		delete m_graph[0];
		delete m_graph[1];
		delete m_graph[2];
		delete m_graph[3];
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		settings.setValue(QString("modules/TurDmsModule/graphArrangement"), getGraphArrangement());	
	}
}

void TurDmsModule::addMainMenuEntries() {
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
}

QList<NewCurve *> TurDmsModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											  NewGraph::GraphType /*graphTypeMulti*/) {
	QList<NewCurve*> curves;
	switch(graphType) {
	case NewGraph::TurbineRotor:
	case NewGraph::TurbineWeibull:
		g_tdmsdataStore.addAllCurves(&curves, xAxis, yAxis, graphType); break;
	case NewGraph::TurbineBlade:
	case NewGraph::TurbineAzimuthal:
		if (m_shownSimulation) {
			for (int i = 0; i < m_shownSimulation->m_DData.size(); ++i) {
				NewCurve *curve = m_shownSimulation->m_DData[i]->newCurve(xAxis, yAxis, graphType, m_shownHeightIndex);
				if (curve) {
					curves.append(curve);
				}
			}
		}
		break;
	case NewGraph::TurbineLegend: return curves;
	default: return curves;  // not allowed
	}
	return curves;
}

QStringList TurDmsModule::prepareMissingObjectMessage() {
	return TDMSData::prepareMissingObjectMessage();
}

QStringList TurDmsModule::getAvailableGraphVariables(bool xAxis) {
	NewGraph::GraphType graphType = m_graph[m_currentGraphIndex]->getGraphType();
	switch (graphType) {
	case NewGraph::TurbineRotor:
	case NewGraph::TurbineWeibull: return TDMSData::getAvailableVariables(graphType, xAxis);
	case NewGraph::TurbineBlade:
	case NewGraph::TurbineAzimuthal: return DData::getAvailableVariables(graphType);
	default: return QStringList ();
	}
}

QPair<ShowAsGraphInterface *, int> TurDmsModule::getHighlightDot(NewGraph::GraphType graphType) {
	if (!m_simulationDock->isSelectShown()) {
		return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
	
	switch (graphType) {
	case NewGraph::TurbineRotor: return QPair<ShowAsGraphInterface*,int> (m_shownSimulation, m_toolBar->getWindspeedIndex());
	default: return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
}

void TurDmsModule::getWeibullParameters(double *k, double *kMoreLess, double *a, double *aMoreLess) {
	m_toolBar->getWeibullParameters(k, kMoreLess, a, aMoreLess);
}

void TurDmsModule::initView() {
	if (m_firstView) {
		m_firstView = false;
		m_graph[0] = new NewGraph ("TurDmsGraphOne", this, {NewGraph::TurbineRotor, "", "", false, false});
		m_graph[1] = new NewGraph ("TurDmsGraphTwo", this, {NewGraph::TurbineBlade, "", "", false, false});
		m_graph[2] = new NewGraph ("TurDmsGraphThree", this, {NewGraph::TurbineLegend, "", "", false, false});
		m_graph[3] = new NewGraph ("TurDmsGraphFour", this, {NewGraph::TurbineAzimuthal, "", "", false, false});
		setGraphArrangement(TwoDWidgetInterface::Quad);
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
							(settings.value("modules/TurDmsModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));
	}
}

void TurDmsModule::showAll() {
	g_tdmsdataStore.showAllCurves(true);
	reloadForGraphType(NewGraph::TurbineRotor);
	reloadForGraphType(NewGraph::TurbineWeibull);
	m_simulationDock->adjustShowCheckBox();
}

void TurDmsModule::hideAll() {
	g_tdmsdataStore.showAllCurves(false, m_shownSimulation);
	reloadForGraphType(NewGraph::TurbineRotor);
	reloadForGraphType(NewGraph::TurbineWeibull);
	m_simulationDock->adjustShowCheckBox();
}

void TurDmsModule::onActivationActionTriggered () {
	ModuleBase::onActivationActionTriggered();
	showModule();
	g_mainFrame->switchToTwoDWidget();
	m_turbineDock->show();
	m_simulationDock->show();
	m_toolBar->show();
}

void TurDmsModule::onModuleChanged () {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		hideModule();
		m_turbineDock->hide();
		m_simulationDock->hide();
		m_toolBar->hide();
	}
}

void TurDmsModule::setShownSimulationParent(TData *newParent, bool forceReload) {
	if (forceReload || m_shownSimulationParent != newParent) {
		SimulationModule::setShownSimulationParent(newParent, forceReload);
		m_turbineDock->setShownObject(newParent);
	}
}

void TurDmsModule::setShownSimulation(TDMSData *newSimulation, bool forceReload) {
	if (forceReload || m_shownSimulation != newSimulation) {
		SimulationModule::setShownSimulation(newSimulation, forceReload);
		reloadForGraphType(NewGraph::TurbineBlade);
	}
}

void TurDmsModule::setShownWindspeedData(int index, bool forceReload) {
	/* find the new DData */
	DData *newData = NULL;
	if (index != -1) {
		newData = m_shownSimulation->m_DData[index];
	}
	
	/* set new value */
	if (forceReload || m_shownWindspeedData != newData) {
		if (m_shownWindspeedData) {
			m_shownWindspeedData->pen()->setWidth(1);
		}
		
		m_shownWindspeedData = newData;
		
		if (m_shownWindspeedData) {
			m_shownWindspeedData->pen()->setWidth(3);
		}
		
		update();
	}
}

void TurDmsModule::setShownHeightIndex(int index, bool forceReload) {
	if (forceReload || m_shownHeightIndex != index) {
		m_shownHeightIndex = index;
		reloadForGraphType(NewGraph::TurbineAzimuthal);
	}
}

void TurDmsModule::onWeibullChanged() {
	double a, aMoreLess, k, kMoreLess;
	m_toolBar->getWeibullParameters(&k, &kMoreLess, &a, &aMoreLess);
	
	// create A and k evaluation range
	TDMSData::aWeibull.clear();
	TDMSData::kWeibull.clear();
	for (int i = 0; i < 101; ++i) {
		TDMSData::aWeibull.append(a + aMoreLess / 50.0 * (i-50));
		TDMSData::kWeibull.append(k + kMoreLess / 50.0 * (i-50));
	}
	
	for (int i = 0; i < g_tdmsdataStore.size(); ++i) {  // update all simulations
		g_tdmsdataStore.at(i)->calculateWeibull();
	}
	m_simulationDock->setShownObject(m_shownSimulation);  // reload
	
	reloadForGraphType(NewGraph::TurbineWeibull);
}

void TurDmsModule::onHideDocks(bool hide) {
	SimulationModule::onHideDocks(hide);
	
	m_turbineDock->setVisible(!hide);
}

TurDmsModule *g_turDmsModule;
