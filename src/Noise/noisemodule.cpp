#include "noisemodule.h"
#include "../XGlobals.h"
#include <QMenu>
#include "../StoreAssociatedComboBox.h"
#include <QtOpenGL>
#include "../GLWidget.h"
#include "../Objects/Polar.h"
#include "../GlobalFunctions.h"
#include "../Store.h"

NoiseModule::NoiseModule(QMainWindow *mainWindow, QToolBar *toolbar)
{
    m_MainToolBar = toolbar;
    m_MainWindow = mainWindow;
    initComponents();
}

void NoiseModule::onRedraw () {
    //NOT IMPLEMENTED
}

NoiseModule::~NoiseModule() {

    delete m_graph[0];
    delete m_graph[1];
    delete m_graph[2];
    delete m_graph[3];
    delete m_NoiseToolBar;
    delete m_NoiseDock;

}

void NoiseModule::configureGL() {
    //NOT IMPLEMENTED
}


void NoiseModule::addMainMenuEntries(){

}

void NoiseModule::OnCenterScene()
{

}


void NoiseModule::render()
{
    //NOT IMPLEMENTED
}

void NoiseModule::initComponents()
{

    //Setup the graphics
    setGraphArrangement(Single);

    m_graph[0] = new NewGraph ("NoiseGraphOne", NewGraph::NoiseSimulationGraph, this);
    m_graph[1] = new NewGraph ("NoiseGraphTwo", NewGraph::NoiseSimulationGraph, this);
    m_graph[2] = new NewGraph ("NoiseGraphThree", NewGraph::NoiseSimulationGraph, this);
    m_graph[3] = new NewGraph ("NoiseGraphFour", NewGraph::NoiseSimulationGraph, this);


    m_graph[0]->setTitle("UHUHUH");

    //Create toolbar
    registrateAtToolbar(tr("Noise Prediction Module"), tr("Noise Prediction Module"), ":/images/Noise-Icon.png", m_MainToolBar);
    g_mainFrame->NoiseViewMenu->addAction(m_activationAction);
    m_NoiseToolBar = new NoiseToolBar(m_MainWindow, this);


    //Setup the dock
    m_NoiseDock = new NoiseDock (tr("Noise Prediction"), m_MainWindow, 0, this);


}

void NoiseModule::reloadAllGraphics()
{
    m_graph[0]->reloadCurves();
}

void NoiseModule::UpdateView()
{
    //NOT IMPLEMENTED
}

void NoiseModule::initView(){

    if (m_firstView) {

        m_NoiseDock->initView();
        OnBpmteView();
        m_firstView = false;

        OnCenterScene();
    }


}


void NoiseModule::showAll(){

}

void NoiseModule::hideAll(){

}

void NoiseModule::setContextMenuGraphType(NewGraph::GraphType){
    //NOT IMPLEMENTED
}

void NoiseModule::onActivationActionTriggered(){
    ModuleBase::onActivationActionTriggered();
    DualModule::showModule();

    g_mainFrame->m_iApp = NOISE_MODULE;

    OnBpmteView();

    m_NoiseToolBar->show();
    m_NoiseDock->show();
    m_NoiseDock->setMinimumWidth(DOCK_WIDTH);
    m_NoiseDock->setMaximumWidth(DOCK_WIDTH);


    OnCenterScene();

    if(m_firstActivation){
        m_firstActivation = false;

        NoiseWarningDialog * dialog = new NoiseWarningDialog();
        dialog->exec();
        delete dialog;

    }

    //m_graph[0]->reloadCurves();

}

void NoiseModule::onModuleChanged (){

    if (g_mainFrame->getCurrentModule() == this) {
        ModuleBase::onModuleChanged();
        DualModule::hideModule();

        m_NoiseDock->hide();
        m_NoiseToolBar->hide();

    }

}

void NoiseModule::OnQ3dView()
{
    //NOT IMPLEMENTED
}

void NoiseModule::OnBpmteView()
{

    g_mainFrame->m_iView = NOISE_BPMTE_VIEW;
    setTwoDView();

}

void NoiseModule::OnSelChangeNoiseSimulation(int simulationName)
{

    m_CurNoiseSimulation = g_NoiseSimulationStore.at(simulationName);
    reloadAllGraphics();

}

QStringList NoiseModule::getAvailableGraphVariables(){

    if (m_CurNoiseSimulation){
        return m_CurNoiseSimulation->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
    } else {
        return QStringList();
    }
}


