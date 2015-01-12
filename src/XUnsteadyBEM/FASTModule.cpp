#include "FASTModule.h"

#include <QDebug>
#include <QMenuBar>

#include "../Params.h"
#include "FASTSimulationToolBar.h"
#include "FASTDock.h"
#include "../Store.h"
#include "../TwoDWidget.h"
#include "../Graph/QGraph.h"
#include "../Graph/NewGraph.h"
#include "FASTSimulation.h"
#include "FastTwoDContextMenu.h"
#include "FASTMenu.h"


FASTModule::FASTModule(QMainWindow *mainWindow, QToolBar *toolbar) {
	m_globalModuleIndentifier = FASTMODULE;
	m_shownFASTSimulation = NULL;
    m_DockWidth = 305;

    m_FASTMenu = new FASTMenu (mainWindow, this);
    mainWindow->menuBar()->addMenu(m_FASTMenu);
	
//	m_graph[0] = new NewGraph ("FastGraphOne", NewGraph::FastSimulation, this);
//	m_graph[1] = new NewGraph ("FastGraphTwo", NewGraph::FastSimulation, this);
//	m_graph[2] = new NewGraph ("FastGraphThree", NewGraph::FastSimulation, this);
//	m_graph[3] = new NewGraph ("FastGraphFour", NewGraph::FastSimulation, this);
	
	m_FASTToolBar = new FASTSimulationToolBar (mainWindow, this);	
	m_FASTDock = new FASTDock ("FAST Simulation", mainWindow, 0, this);
	registrateAtToolbar(tr("FAST Simulation"), tr("Prepare and run a FAST simulation"),
						":/images/ubem.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);

	
	m_fastContextMenu = new FastTwoDContextMenu (m_twoDWidget, this);
	setContextMenu(m_fastContextMenu);
}

void FASTModule::addMainMenuEntries() {
    g_mainFrame->menuBar()->addMenu(g_mainFrame->BEMViewMenu);
    g_mainFrame->menuBar()->addMenu(m_FASTMenu);
}

FASTModule::~FASTModule() {
	if (m_firstView == false) {
		delete m_graph[0];
		delete m_graph[1];
		delete m_graph[2];
		delete m_graph[3];
	}
}

QStringList FASTModule::getAvailableGraphVariables() {
	if (m_shownFASTSimulation) {
		return m_shownFASTSimulation->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
	} else {
		return QStringList();
	}
}

void FASTModule::initView() {
	if (m_firstView) {
		m_firstView = false;
		m_graph[0] = new NewGraph ("FastGraphOne", NewGraph::FastSimulation, this);
		m_graph[1] = new NewGraph ("FastGraphTwo", NewGraph::FastSimulation, this);
		m_graph[2] = new NewGraph ("FastGraphThree", NewGraph::FastSimulation, this);
		m_graph[3] = new NewGraph ("FastGraphFour", NewGraph::FastSimulation, this);
		
		m_FASTDock->initView();
	}
}

void FASTModule::showAll() {
	for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
		g_FASTSimulationStore.at(i)->setShownInGraph(true);
    }
	reportGraphChange();
	m_FASTDock->adjustShowCheckBox();
}

void FASTModule::hideAll() {
	for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
		g_FASTSimulationStore.at(i)->setShownInGraph(false);
    }
	reportGraphChange();
	m_FASTDock->adjustShowCheckBox();
}

void FASTModule::setContextMenuGraphType(NewGraph::GraphType graphType) {
	m_fastContextMenu->setCurrentGraphType(graphType);
}

void FASTModule::onActivationActionTriggered () {
	ModuleBase::onActivationActionTriggered();
	TwoDModule::showModule();
	g_mainFrame->switchToTwoDWidget();
	m_FASTDock->show();
	m_FASTToolBar->show();
    m_FASTDock->setMinimumWidth(m_DockWidth);
    m_FASTDock->setMaximumWidth(m_DockWidth);
}

void FASTModule::onModuleChanged () {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		TwoDModule::hideModule();
		m_FASTDock->hide();
		m_FASTToolBar->hide();
	}
}

void FASTModule::setShownFASTSimulation(FASTSimulation *newShownFASTSimulation, bool forceReload) {
	if (forceReload || m_shownFASTSimulation != newShownFASTSimulation) {
		m_shownFASTSimulation = newShownFASTSimulation;
		m_FASTDock->setShownFASTSimulation(m_shownFASTSimulation);
		m_FASTToolBar->useFASTSimulation(m_shownFASTSimulation);
	}
}

void FASTModule::reloadAllGraphCurves() {
	m_graph[0]->reloadCurves();
	m_graph[1]->reloadCurves();
	m_graph[2]->reloadCurves();
	m_graph[3]->reloadCurves();
}

void FASTModule::onShownFASTSimulationChanged(const QString newName) {  // NM TODO ComboBox should provide valueChanged (StorableObject*)
	setShownFASTSimulation (g_FASTSimulationStore.getObjectByNameOnly(newName));  // this can be replaces then
}
