#ifndef NOISEMODULE_H
#define NOISEMODULE_H

#include "../Module.h"
class NoiseDock;
class NoiseToolBar;
class NoiseSimulation;
class NoiseContextMenu;
class NoiseMenu;


class NoiseModule : public ModuleBase, public TwoDModule
{
	Q_OBJECT
	
public:
	NoiseModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~NoiseModule();
	
	void addMainMenuEntries();
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	int getHighlightIndex(NewGraph::GraphType graphTypeMulti);
	QStringList prepareMissingObjectMessage();
	bool isColorByOpPoint();
	
private:
	void showAll();
	void hideAll();
	
	NoiseDock *m_dock;
	NoiseToolBar *m_toolBar;
	NoiseSimulation *m_shownSimulation;
	NoiseContextMenu *m_contextMenu;
	NoiseMenu *m_menu;	
	
public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged();  // override from ModuleBase
	void onHideDocks(bool hide);
	void setShownSimulation(NoiseSimulation *newSimulation, bool forceReload = false);
	NoiseSimulation* getShownSimulation() { return m_shownSimulation; }

	void onNeedUpdate() { update(); }
	void reloadAllGraphs () { reloadAllGraphCurves(); }	
};

extern NoiseModule *g_noiseModule;

#endif // NOISEMODULE_H
