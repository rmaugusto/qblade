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
#include "../TwoDContextMenu.h"
#include "noisewarningdialog.h"
#include "noisetoolbar.h"
#include "noisedock.h"
#include "noisesimulation.h"
#include "NewGraphLegend.h"


class NoiseToolBar;
class NoiseDock;

/**
 * @brief This class is responsible to create a Noise module
 */
class NoiseModule : public ModuleBase, public DualModule
{

    Q_OBJECT

public:

    NoiseModule(QMainWindow *mainWindow, QToolBar *toolbar);
    ~NoiseModule ();

    /**
     * @brief getAvailableGraphVariables override from TwoDWidgetInterface
     * @return
     */
    QStringList getAvailableGraphVariables();
    /**
     * @brief onRedraw override from GLModule
     */
    virtual void onRedraw ();
    /**
     * @brief addMainMenuEntries
     */
    virtual void addMainMenuEntries();
    /**
     * @brief initView
     */
    virtual void initView();

private:

    static const int DOCK_WIDTH = 220;

    NoiseToolBar* m_NoiseToolBar;
    NoiseDock* m_NoiseDock;
    QToolBar * m_MainToolBar;
    QMainWindow *m_MainWindow;
    NoiseSimulation *m_CurNoiseSimulation;
    bool m_firstActivation;
    TwoDContextMenu *m_twoDContextMenu;

    virtual void configureGL ();  // override from GLModule
    void showAll();
    void hideAll();
    void setContextMenuGraphType(NewGraph::GraphType graphType);
    void OnCenterScene();
    void render();
    void initComponents();

public:
    void reloadAllGraphics();

public slots:
    virtual void onActivationActionTriggered();  // override from ModuleBase
    virtual void onModuleChanged ();  // override from ModuleBase
    void UpdateView();

private slots:
    void OnQ3dView();
    void OnBpmteView();
    void OnSelChangeNoiseSimulation(int simulationName);
};

#endif // NOISEMODULE_H
