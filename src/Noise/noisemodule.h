#ifndef NOISEMODULE_H
#define NOISEMODULE_H

#include "../Module.h"
#include "../Params.h"

#include <QModelIndex>

#include "../XBEM/Blade.h"
#include "../XUnsteadyBEM/WindField.h"
#include "../Miarex/GLLightDlg.h"
#include "../XBEM/360Polar.h"
#include "../MainFrame.h"
#include "noisetoolbar.h"
#include "noisedock.h"

class NoiseToolBar;
class NoiseDock;

class NoiseModule : public ModuleBase, public DualModule
{

    friend class TwoDWidgetInterface;
    Q_OBJECT

public:

    NoiseModule(QMainWindow *mainWindow, QToolBar *toolbar);
    ~NoiseModule ();

    QStringList getAvailableGraphVariables();  // override from TwoDWidgetInterface
    virtual void onRedraw ();  // override from GLModule
    virtual void addMainMenuEntries();
    virtual void initView();  // override from Module

private:

    int m_DockWidth;
    bool GLView, TwoDView;

    NoiseToolBar* m_NoiseToolBar;
    NoiseDock* m_NoiseDock;

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
    void OnGLView();
    void OnTwoDView();
};

#endif // NOISEMODULE_H
