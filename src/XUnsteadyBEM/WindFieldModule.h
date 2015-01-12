#ifndef WINDFIELDMODULE_H
#define WINDFIELDMODULE_H

#include "../Module.h"
class WindField;
class WindFieldCreatorDock;
class WindFieldToolBar;
class WindFieldMenu;


class WindFieldModule : public ModuleBase, public GLModule
{
	Q_OBJECT
	
public:
	WindFieldModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~WindFieldModule ();
	
	WindField* getShownWindField () { return m_shownWindField; }
	void setShownWindField (WindField *newShownWindField);
	
	virtual void onRedraw ();  // override from GLModule
	virtual void addMainMenuEntries();  // override from ModuleBase
	
private:
	WindFieldMenu *m_windFieldMenu;  // contains the menu with all actions
	WindFieldToolBar *m_windFieldToolbar;  // where to set shown timestep and see information about the field
	WindFieldCreatorDock *m_windFieldCreatorDock;  // where the parameters for WindField are set
	WindField *m_shownWindField;  // the currently shown windfield

    int m_DockWidth;
	
	virtual void initView();  // override from Module
	virtual void configureGL ();  // override from GLModule
	void drawLegend ();  // draws the legend into the GLWidget
	
public slots:
	void onShownWindFieldChanged (int newIndex);  // connected to the comboBox in the toolBar
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged ();  // override from ModuleBase
};

extern WindFieldModule *g_windFieldModule;

#endif // WINDFIELDMODULE_H
