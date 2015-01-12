#ifndef FASTMODULE_H
#define FASTMODULE_H

#include "../Module.h"
class FASTSimulation;
class FASTSimulationToolBar;
class FASTDock;
class QGraph;
class FastTwoDContextMenu;
class FASTMenu;


class FASTModule : public ModuleBase, public TwoDModule
{
	Q_OBJECT
	
public:
	FASTModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~FASTModule();
	
	QStringList getAvailableGraphVariables();  // override from TwoDWidgetInterface
	FASTSimulation* getShownFASTSimulation () { return m_shownFASTSimulation; }
	void setShownFASTSimulation (FASTSimulation *newShownFASTSimulation, bool forceReload = false);
	
	void reloadAllGraphCurves ();
	void emitUpdateFastGraphs () { emit updateFastGraphs(); }
	void emitUpdateBladeGraphs () { emit updateBladeGraphs(); }
    virtual void addMainMenuEntries();
	
private:
	FASTDock *m_FASTDock;
	FASTSimulationToolBar *m_FASTToolBar;
	FASTSimulation *m_shownFASTSimulation;
	FastTwoDContextMenu *m_fastContextMenu;
    FASTMenu *m_FASTMenu;
    int m_DockWidth;
	
	void initView();
	void showAll();
	void hideAll();
	void setContextMenuGraphType(NewGraph::GraphType graphType);
	
public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged ();  // override from ModuleBase
	void onShownFASTSimulationChanged (const QString newName);  // connected to the comboBox in the toolBar
	
signals:
	void updateFastGraphs ();
	void updateBladeGraphs ();
};

extern FASTModule *g_fastModule;

#endif // FASTMODULE_H
