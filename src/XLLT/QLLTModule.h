#ifndef QLLTMODULE_H
#define QLLTMODULE_H


#include "../Module.h"
#include "../Params.h"

#include <QModelIndex>

#include "../XBEM/Blade.h"
#include "../XUnsteadyBEM/WindField.h"
#include "../Miarex/GLLightDlg.h"
#include "../XBEM/360Polar.h"
#include "QLLTSimulation.h"

class QLLTToolBar;
class QLLTDock;
class QLLTSimulation;

class QLLTModule : public ModuleBase, public DualModule
{
    friend class QLLTSimulation;
    Q_OBJECT

public:

    QLLTModule(QMainWindow *mainWindow, QToolBar *toolbar);
    ~QLLTModule ();

    QStringList getAvailableGraphVariables();  // override from TwoDWidgetInterface
    virtual void onRedraw ();  // override from GLModule
    virtual void addMainMenuEntries();
    virtual void initView();  // override from Module

    QLLTSimulation* GetQLLTSim();
    void setShownLLTSimulation (QLLTSimulation *newShownLLTSimulation, bool forceReload = false);


private:

    int m_DockWidth;
    bool GLView, TwoDView;
    QLLTSimulation *m_QLLTSim;


    QLLTToolBar* m_QLLTToolBar;
    QLLTDock* m_QLLTDock;




    virtual void configureGL ();  // override from GLModule
    void showAll();
    void hideAll();
    void setContextMenuGraphType(NewGraph::GraphType graphType);
    void OnCenterScene();


    void GLCreateRotorMesh();
    void GLCreateWakeMesh();
    void render();








public slots:
    virtual void onActivationActionTriggered();  // override from ModuleBase
    virtual void onModuleChanged ();  // override from ModuleBase
    void UpdateView();



private slots:
    void OnSelChangeLLTSimulation();
    void OnGLView();
    void OnTwoDView();
};

#endif // QLLTMODULE_H
