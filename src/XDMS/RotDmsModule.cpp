#include "RotDmsModule.h"

#include <QMenuBar>
#include <QSettings>

#include "../MainFrame.h"
#include "DMSData.h"
#include "RotDmsContextMenu.h"
#include "../TwoDWidget.h"
#include "../Store.h"
#include "../TwoDGraphMenu.h"


RotDmsModule::RotDmsModule(QMainWindow *mainWindow, QToolBar *toolbar)
	: m_shownTipSpeedData(NULL),
	  m_shownHeightIndex(-1)
{
	m_globalModuleIndentifier = ROTDMSMODULE;

	m_graph[0] = new NewGraph ("RotDmsGraphOne", this, {NewGraph::TurbineRotor, "", "", false, false});
	m_graph[1] = new NewGraph ("RotDmsGraphTwo", this, {NewGraph::TurbineBlade, "", "", false, false});
	m_graph[2] = new NewGraph ("RotDmsGraphThree", this, {NewGraph::RotorLegend, "", "", false, false});
	m_graph[3] = new NewGraph ("RotDmsGraphFour", this, {NewGraph::TurbineAzimuthal, "", "", false, false});

	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
						(settings.value("modules/RotDmsModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));

	m_simulationDock = new RotDmsSimulationDock ("VAWT Simulation", mainWindow, 0, this);

	m_contextMenu = new RotDmsContextMenu (m_twoDWidget, this);
	setContextMenu(m_contextMenu);

	m_toolBar = new RotDmsToolBar (mainWindow, this);

	registrateAtToolbar("Rotor DMS Simulation", "Simulate a Rotor", ":/images/vrotor.png", toolbar);
}

RotDmsModule::~RotDmsModule() {
	delete m_graph[0];
	delete m_graph[1];
	delete m_graph[2];
	delete m_graph[3];

	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
	settings.setValue(QString("modules/RotDmsModule/graphArrangement"), getGraphArrangement());
}

void RotDmsModule::addMainMenuEntries() {
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
}

QList<NewCurve *> RotDmsModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											  NewGraph::GraphType graphTypeMulti) {
	QList<NewCurve*> curves;
	switch(graphType) {
	case NewGraph::TurbineRotor:
		g_dmsdataStore.addAllCurves(&curves, xAxis, yAxis, graphType); break;
	case NewGraph::TurbineBlade:
	case NewGraph::TurbineAzimuthal:
		if (m_shownTipSpeedData) {
			switch (graphTypeMulti) {
			case NewGraph::None:
				if (m_shownTipSpeedData) {
					NewCurve *curve = m_shownTipSpeedData->newCurve(xAxis, yAxis, graphType, m_shownHeightIndex);
					if (curve) {
						curves.append(curve);
					}
				}
				break;
			case NewGraph::RotorTipSpeed:
				if (m_shownSimulation) {
					for (int i = 0; i < m_shownSimulation->m_DData.size(); ++i) {
						NewCurve *curve = m_shownSimulation->m_DData[i]->newCurve(xAxis, yAxis, graphType,
																				  m_shownHeightIndex);
						if (curve) {
							curves.append(curve);
						}
					}
				}
				break;
			case NewGraph::RotorAllSimulations:
				for (int simIndex = 0; simIndex < g_dmsdataStore.size(); ++simIndex) {
					DMSData *simulation = g_dmsdataStore.at(simIndex);
					if (simulation->isShownInGraph()) {
						for (int dataIndex = 0; dataIndex < simulation->m_DData.size(); ++dataIndex) {
							DData *data = simulation->m_DData[dataIndex];
							if (graphType == NewGraph::TurbineBlade) {
								if (m_shownTipSpeedData->lambdaglobal == data->lambdaglobal) {
									NewCurve *curve = data->newCurve(xAxis, yAxis, graphType, m_shownHeightIndex);
									if (curve) {
										curves.append(curve);
									}
								}
							} else if (graphType == NewGraph::TurbineAzimuthal) {
								// NM TODO prepare the curves with the same height values; crashing in the original module
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case NewGraph::RotorLegend: return curves;
	default: return curves;  // not allowed
	}
	return curves;
}

QStringList RotDmsModule::prepareMissingObjectMessage() {
	return DMSData::prepareMissingObjectMessage();
}

QStringList RotDmsModule::getAvailableGraphVariables(bool /*xAxis*/) {
	NewGraph::GraphType graphType = m_graph[m_currentGraphIndex]->getGraphType();
	switch (graphType) {
	case NewGraph::TurbineRotor: return DMSData::getAvailableVariables(graphType);
	case NewGraph::TurbineBlade:
	case NewGraph::TurbineAzimuthal: return DData::getAvailableVariables(graphType);
	default: return QStringList ();
	}
}

QPair<ShowAsGraphInterface*, int> RotDmsModule::getHighlightDot(NewGraph::GraphType graphType) {
	if (!m_simulationDock->isSelectShown()) {
		return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}

	switch (graphType) {
	case NewGraph::TurbineRotor: return QPair<ShowAsGraphInterface*,int> (m_shownSimulation, m_toolBar->getTipSpeedIndex());
	default: return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
}

int RotDmsModule::getHighlightIndex(NewGraph::GraphType graphTypeMulti) {
	switch (graphTypeMulti) {
	case NewGraph::None: return -1;
	case NewGraph::RotorTipSpeed: return m_toolBar->getTipSpeedIndex();
	case NewGraph::RotorAllSimulations: return -1;
	default: return -1;
	}
}

void RotDmsModule::showAll() {
	g_dmsdataStore.showAllCurves(true);
	m_simulationDock->adjustShowCheckBox();
}

void RotDmsModule::hideAll() {
	g_dmsdataStore.showAllCurves(false, m_shownSimulation);
	m_simulationDock->adjustShowCheckBox();
}

void RotDmsModule::onActivationActionTriggered () {
	ModuleBase::onActivationActionTriggered();
	showModule();
	g_mainFrame->switchToTwoDWidget();
	m_simulationDock->show();
	m_toolBar->show();
}

void RotDmsModule::onModuleChanged () {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		hideModule();
		m_simulationDock->hide();
		m_toolBar->hide();
	}
}

void RotDmsModule::setShownTipSpeedIndex(int index, bool forceReload) {
	/* find the new DData */
	DData *newData = NULL;
	if (index != -1) {
		newData = m_shownSimulation->m_DData[index];
	}

	/* set new value */
	if (forceReload || m_shownTipSpeedData != newData) {
		if (m_shownTipSpeedData) {
			m_shownTipSpeedData->pen()->setWidth(1);
		}

		m_shownTipSpeedData = newData;

		if (m_shownTipSpeedData) {
			m_shownTipSpeedData->pen()->setWidth(3);
		}

		reloadForGraphType(NewGraph::TurbineBlade);
		reloadForGraphType(NewGraph::TurbineAzimuthal);
	}
}

void RotDmsModule::setShownHeightIndex(int index) {
	m_shownHeightIndex = index;
	reloadForGraphType(NewGraph::TurbineAzimuthal);
}

void RotDmsModule::setShownSimulation(DMSData *newSimulation, bool forceReload) {
	if (forceReload || m_shownSimulation != newSimulation) {
		SimulationModule::setShownSimulation(newSimulation, forceReload);
		reloadForGraphType(NewGraph::TurbineBlade);
		reloadForGraphType(NewGraph::TurbineAzimuthal);
	}
}

RotDmsModule *g_rotDmsModule;
