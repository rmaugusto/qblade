#ifndef MULTIDMSMODULE_H
#define MULTIDMSMODULE_H

#include "../SimulationModule.h"
#include "MultiDmsSimulationDock.h"
#include "MultiDmsToolBar.h"
class MultiDmsContextMenu;
class CBlade;
class CDMSData;


class MultiDmsModule : public SimulationModule<CBlade,CDMSData>
{
	Q_OBJECT
	
public:
	MultiDmsModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~MultiDmsModule();
	
	virtual void addMainMenuEntries();
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList prepareMissingObjectMessage();
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	virtual int getHighlightIndex(NewGraph::GraphType graphTypeMulti);
	
private:
	void initView();
	
	void showAll();
	void hideAll();
	
	MultiDmsToolBar* toolBar() { return m_toolBar; }
	MultiDmsSimulationDock* simulationDock() { return m_simulationDock; }
	
	MultiDmsToolBar *m_toolBar;
	MultiDmsSimulationDock *m_simulationDock;
	MultiDmsContextMenu *m_contextMenu;
	
	int m_shownWindspeedIndex, m_shownRotationalIndex, m_shownPitchIndex;
	
public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged();  // override from ModuleBase
	
	void setShownWindspeedIndex (int index);
	void setShownRotationalIndex (int index);
	void setShownPitchIndex (int index);
	void setShownSimulation(CDMSData *newSimulation, bool forceReload = false);
};

extern MultiDmsModule *g_multiDmsModule;


#endif // MULTIDMSMODULE_H
