#include "FASTModule.h"

#include <QDebug>
#include <QMenuBar>
#include <QSettings>

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
#include "../TwoDGraphMenu.h"


FASTModule::FASTModule(QMainWindow *mainWindow, QToolBar *toolbar) {
	m_globalModuleIndentifier = FASTMODULE;
	m_shownFASTSimulation = NULL;

    m_FASTMenu = new FASTMenu (mainWindow, this);
    mainWindow->menuBar()->addMenu(m_FASTMenu);
	
	m_FASTToolBar = new FASTSimulationToolBar (mainWindow, this);
	m_FASTDock = new FASTDock ("FAST Simulation", mainWindow, 0, this);
	registrateAtToolbar(tr("FAST Simulation"), tr("Prepare and run a FAST simulation"),
						":/images/ubem.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);

	m_fastContextMenu = new FastTwoDContextMenu (m_twoDWidget, this);
	setContextMenu(m_fastContextMenu);
	
	connect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadFastGraphs()));
}

void FASTModule::addMainMenuEntries() {
    g_mainFrame->menuBar()->addMenu(g_mainFrame->BEMViewMenu);
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
	g_mainFrame->menuBar()->addMenu(m_FASTMenu);
}

QStringList FASTModule::prepareMissingObjectMessage() {
	return FASTSimulation::prepareMissingObjectMessage();
}

FASTModule::~FASTModule() {
	if (m_firstView == false) {
		delete m_graph[0];
		delete m_graph[1];
		delete m_graph[2];
		delete m_graph[3];
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		settings.setValue(QString("modules/FASTModule/graphArrangement"), getGraphArrangement());	
	}
}

QList<NewCurve *> FASTModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											NewGraph::GraphType /*graphTypeMulti*/) {
	QList<NewCurve*> curves;
	g_FASTSimulationStore.addAllCurves(&curves, xAxis, yAxis, graphType);
	return curves;
}

QStringList FASTModule::getAvailableGraphVariables(bool /*xAxis*/) {
	if (m_shownFASTSimulation) {
		return m_shownFASTSimulation->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
	} else {
		return QStringList();
	}
}

QPair<ShowAsGraphInterface*,int> FASTModule::getHighlightDot(NewGraph::GraphType graphType) {
	switch (graphType) {
	case NewGraph::FastSimulation:
		if (m_shownFASTSimulation != NULL && m_shownFASTSimulation->hasAeroDynResults()) {
			return QPair<ShowAsGraphInterface*,int> (m_shownFASTSimulation, m_shownFASTSimulation->getShownTimeIndex());
		}
	default:
		return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
}

void FASTModule::initView() {
	if (m_firstView) {
		m_firstView = false;
		
		m_graph[0] = new NewGraph ("FastGraphOne", this, {NewGraph::FastSimulation, "", "", false, false});
		m_graph[1] = new NewGraph ("FastGraphTwo", this, {NewGraph::FastSimulation, "", "", false, false});
		m_graph[2] = new NewGraph ("FastGraphThree", this, {NewGraph::FastSimulation, "", "", false, false});
		m_graph[3] = new NewGraph ("FastGraphFour", this, {NewGraph::FastSimulation, "", "", false, false});
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
							(settings.value("modules/FASTModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));
	}
}

void FASTModule::showAll() {
	g_FASTSimulationStore.showAllCurves(true);
	reloadAllGraphCurves();
	m_FASTDock->adjustShowCheckBox();
}

void FASTModule::hideAll() {
	g_FASTSimulationStore.showAllCurves(false, m_shownFASTSimulation);
	reloadAllGraphCurves();
	m_FASTDock->adjustShowCheckBox();
}

void FASTModule::onActivationActionTriggered () {
	ModuleBase::onActivationActionTriggered();
	showModule();
	g_mainFrame->switchToTwoDWidget();
	m_FASTDock->show();
	m_FASTToolBar->show();
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


FASTModule *g_fastModule;
