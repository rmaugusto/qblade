#ifndef FASTMODULE_H
#define FASTMODULE_H

#include "../Module.h"
class FASTSimulation;
class FASTSimulationToolBar;
class FASTDock;
class QGraph;
class FastTwoDContextMenu;
class FASTMenu;
class StorableObject;


class FASTModule : public ModuleBase, public TwoDModule
{
	Q_OBJECT
	
public:
	FASTModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~FASTModule();
	
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	virtual QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	FASTSimulation* getShownFASTSimulation () { return m_shownFASTSimulation; }
	
    virtual void addMainMenuEntries();
	virtual QStringList prepareMissingObjectMessage();
	
private:
	FASTDock *m_FASTDock;
	FASTSimulationToolBar *m_FASTToolBar;
	FASTSimulation *m_shownFASTSimulation;
	FastTwoDContextMenu *m_fastContextMenu;
    FASTMenu *m_FASTMenu;
	
	void initView();
	void showAll();
	void hideAll();
	
public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged ();  // override from ModuleBase
	void setShownFASTSimulation (FASTSimulation *newShownFASTSimulation, bool forceReload = false);
	void reloadFastGraphs () { reloadForGraphType(NewGraph::FastSimulation); }
	void reloadBladeGraphs () { reloadForGraphType(NewGraph::BladeGraph); }
};

extern FASTModule *g_fastModule;

#endif // FASTMODULE_H
