#ifndef ROTDMSMODULE_H
#define ROTDMSMODULE_H

#include "../SimulationModule.h"
#include "RotDmsSimulationDock.h"
#include "RotDmsToolBar.h"
class RotDmsContextMenu;
class DMSData;


class RotDmsModule : public SimulationModule<CBlade,DMSData>
{
	Q_OBJECT

public:
	RotDmsModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~RotDmsModule();

	virtual void addMainMenuEntries();
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList prepareMissingObjectMessage();
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	virtual int getHighlightIndex(NewGraph::GraphType graphTypeMulti);
	DData* getShownTipSpeedData () { return m_shownTipSpeedData; }
	int getShownHeightIndex () { return m_shownHeightIndex; }

private:
	void showAll();
	void hideAll();

	RotDmsToolBar* toolBar() { return m_toolBar; }
	RotDmsSimulationDock* simulationDock() { return m_simulationDock; }

	RotDmsToolBar *m_toolBar;
	RotDmsSimulationDock *m_simulationDock;
	RotDmsContextMenu *m_contextMenu;

	DData *m_shownTipSpeedData;
	int m_shownHeightIndex;

public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged();  // override from ModuleBase

	void setShownTipSpeedIndex (int index, bool forceReload = false);
	void setShownHeightIndex (int index);
	void setShownSimulation(DMSData *newSimulation, bool forceReload = false);
};

extern RotDmsModule *g_rotDmsModule;


#endif // ROTDMSMODULE_H
