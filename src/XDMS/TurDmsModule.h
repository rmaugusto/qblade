#ifndef TURDMSMODULE_H
#define TURDMSMODULE_H

#include "../SimulationModule.h"
#include "TurDmsSimulationDock.h"  // needed for covariance
#include "TurDmsToolBar.h"
class TurDmsTurbineDock;
class TurDmsContextMenu;
class TData;
class DData;
class TDMSData;


class TurDmsModule : public SimulationModule<TData,TDMSData>
{
	Q_OBJECT
	
public:
	TurDmsModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~TurDmsModule();
	
	virtual void addMainMenuEntries();
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList prepareMissingObjectMessage();
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	
	DData* getShownWindspeedData () { return m_shownWindspeedData; }
	int getShownHeightIndex () { return m_shownHeightIndex; }
	void getWeibullParameters (double *k, double *kMoreLess, double *a, double *aMoreLess);
		
private:
	void initView();
	
	void showAll();
	void hideAll();
	
	TurDmsToolBar* toolBar() { return m_toolBar; }
	TurDmsSimulationDock* simulationDock() { return m_simulationDock; }
	
	TurDmsToolBar *m_toolBar;
	TurDmsSimulationDock *m_simulationDock;
	TurDmsTurbineDock *m_turbineDock;
	TurDmsContextMenu *m_contextMenu;
	
	DData *m_shownWindspeedData;
	int m_shownHeightIndex;
	
public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged();  // override from ModuleBase
	
	void setShownSimulationParent(TData *newParent, bool forceReload = false);
	void setShownSimulation(TDMSData *newSimulation, bool forceReload = false);
	void setShownWindspeedData (int index, bool forceReload = false);
	void setShownHeightIndex (int index, bool forceReload = false);
	void onWeibullChanged ();
	void onHideDocks (bool hide);
	
	void reloadTDataGraphs () { reloadForGraphType(NewGraph::TurbineAzimuthal);
								reloadForGraphType(NewGraph::TurbineWeibull); }
	void reloadAllGraphs () { reloadAllGraphCurves(); }
};

extern TurDmsModule *g_turDmsModule;


#endif // TURDMSMODULE_H
