#include "MultiDmsModule.h"

#include <QMenuBar>
#include <QSettings>

#include "CDMSData.h"
#include "MultiDmsContextMenu.h"
#include "../TwoDWidget.h"
#include "../Store.h"
#include "../TwoDGraphMenu.h"


MultiDmsModule::MultiDmsModule(QMainWindow *mainWindow, QToolBar *toolbar) 
	: m_shownWindspeedIndex(-1),
	  m_shownRotationalIndex(-1),
	  m_shownPitchIndex(-1)
{
	m_globalModuleIndentifier = MULTIDMSMODULE;
		  
	m_simulationDock = new MultiDmsSimulationDock ("VAWT Simulation", mainWindow, 0, this);
	
	m_contextMenu = new MultiDmsContextMenu (m_twoDWidget, this);
	setContextMenu(m_contextMenu);
	
	m_toolBar = new MultiDmsToolBar (mainWindow, this);
	
	registrateAtToolbar("Multi Parameter DMS Simulation", "Vary Simulations over Pitch, Wind and Rotational Speed",
						":/images/char.png", toolbar);
}

MultiDmsModule::~MultiDmsModule() {
	if (m_firstView == false) {
		delete m_graph[0];
		delete m_graph[1];
		delete m_graph[2];
		delete m_graph[3];
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		settings.setValue(QString("modules/MultiDmsModule/graphArrangement"), getGraphArrangement());	
	}
}

void MultiDmsModule::addMainMenuEntries() {
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
}

QList<NewCurve *> MultiDmsModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
												NewGraph::GraphType graphTypeMulti) {
	QList<NewCurve*> curves;
	if (m_shownSimulation && m_shownSimulation->hasResults()) {
		int wind = (graphType == NewGraph::MultiWindspeed ? -1 : m_shownWindspeedIndex);
		int rot = (graphType == NewGraph::MultiRotational ? -1 : m_shownRotationalIndex);
		int pitch = (graphType == NewGraph::MultiPitch ? -1 : m_shownPitchIndex);
		if (wind + rot == -2 || wind + pitch == -2 || rot + pitch == -2) {
			return curves;  // the comboBoxes are not yet set and give -1 for currentIndex
		}
		
		if (graphTypeMulti == NewGraph::None) {
			NewCurve* curve = m_shownSimulation->newCurve(xAxis, yAxis, wind, rot, pitch);
			if (curve) {
				curves.append(curve);
			}				
		} else {
			int number, *iter;
			switch (graphTypeMulti) {
			case NewGraph::MultiWindspeed:
				number = m_shownSimulation->windtimes;
				iter = &wind;
				break;
			case NewGraph::MultiRotational:
				number = m_shownSimulation->rottimes;
				iter = &rot;
				break;
			case NewGraph::MultiPitch:
				number = m_shownSimulation->pitchtimes;
				iter = &pitch;
				break;
			default: return curves;  // not allowed
			}
			
			for (*iter = 0; *iter < number; ++(*iter)) {
				NewCurve* curve = m_shownSimulation->newCurve(xAxis, yAxis, wind, rot, pitch);
				if (curve) {
					curves.append(curve);
				}
			}
		}
	}
	return curves;
}

QStringList MultiDmsModule::prepareMissingObjectMessage() {
	return CDMSData::prepareMissingObjectMessage();
}

QStringList MultiDmsModule::getAvailableGraphVariables(bool /*xAxis*/) {
	return CDMSData::getAvailableVariables();
}

QPair<ShowAsGraphInterface *, int> MultiDmsModule::getHighlightDot(NewGraph::GraphType graphType) {
	if (!m_simulationDock->isSelectShown()) {
		return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
	
	switch (graphType) {
	case NewGraph::MultiWindspeed: return QPair<ShowAsGraphInterface*,int> (m_shownSimulation, m_shownWindspeedIndex);
	case NewGraph::MultiRotational: return QPair<ShowAsGraphInterface*,int> (m_shownSimulation, m_shownRotationalIndex);
	case NewGraph::MultiPitch: return QPair<ShowAsGraphInterface*,int> (m_shownSimulation, m_shownPitchIndex);
	default: return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
}

int MultiDmsModule::getHighlightIndex(NewGraph::GraphType graphTypeMulti) {
	switch (graphTypeMulti) {
	case NewGraph::MultiWindspeed: return m_shownWindspeedIndex;
	case NewGraph::MultiRotational: return m_shownRotationalIndex;
	case NewGraph::MultiPitch: return m_shownPitchIndex;
	default: return -1;
	}
}

void MultiDmsModule::initView() {
	if (m_firstView) {
		m_firstView = false;
		m_graph[0] = new NewGraph ("MultiDmsGraphOne", this, {NewGraph::MultiWindspeed, "", "", false, false});
		m_graph[1] = new NewGraph ("MultiDmsGraphTwo", this, {NewGraph::MultiWindspeed, "", "", false, false});
		m_graph[2] = new NewGraph ("MultiDmsGraphThree", this, {NewGraph::MultiRotational, "", "", false, false});
		m_graph[3] = new NewGraph ("MultiDmsGraphFour", this, {NewGraph::MultiPitch, "", "", false, false});
		setGraphArrangement(TwoDWidgetInterface::Quad);
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
							(settings.value("modules/MultiDmsModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));
	}
}

void MultiDmsModule::showAll() {
	g_cdmsdataStore.showAllCurves(true);
	m_simulationDock->adjustShowCheckBox();
}

void MultiDmsModule::hideAll() {
	g_cdmsdataStore.showAllCurves(false, m_shownSimulation);
	m_simulationDock->adjustShowCheckBox();
}

void MultiDmsModule::onActivationActionTriggered () {
	ModuleBase::onActivationActionTriggered();
	showModule();
	g_mainFrame->switchToTwoDWidget();
	m_simulationDock->show();
	m_toolBar->show();
}

void MultiDmsModule::onModuleChanged () {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		hideModule();
		m_simulationDock->hide();
		m_toolBar->hide();
	}
}

void MultiDmsModule::setShownWindspeedIndex(int index) {
	m_shownWindspeedIndex = index;
	reloadForGraphType(NewGraph::MultiRotational);
	reloadForGraphType(NewGraph::MultiPitch);
}

void MultiDmsModule::setShownRotationalIndex(int index) {
	m_shownRotationalIndex = index;
	reloadForGraphType(NewGraph::MultiWindspeed);
	reloadForGraphType(NewGraph::MultiPitch);
}

void MultiDmsModule::setShownPitchIndex(int index) {
	m_shownPitchIndex = index;
	reloadForGraphType(NewGraph::MultiWindspeed);
	reloadForGraphType(NewGraph::MultiRotational);
}

void MultiDmsModule::setShownSimulation(CDMSData *newSimulation, bool forceReload) {
	if (forceReload || m_shownSimulation != newSimulation) {
		SimulationModule::setShownSimulation(newSimulation, forceReload);
		reloadAllGraphCurves();
	}
}

MultiDmsModule *g_multiDmsModule;
