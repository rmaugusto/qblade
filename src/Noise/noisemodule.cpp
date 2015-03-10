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

    m_DockWidth = 220;
    setGraphArrangement(Single);

    m_graph[0] = new NewGraph ("NoiseGraphOne", NewGraph::NoiseSimulationGraph, this);
    m_graph[1] = new NewGraph ("NoiseGraphTwo", NewGraph::NoiseSimulationGraph, this);
    m_graph[2] = new NewGraph ("NoiseGraphThree", NewGraph::NoiseSimulationGraph, this);
    m_graph[3] = new NewGraph ("NoiseGraphFour", NewGraph::NoiseSimulationGraph, this);


    m_graph[0]->setTitle("UHUHUH");

    registrateAtToolbar(tr("NOISE"), tr("NOISE"), ":/images/Noise-Icon.png", toolbar);
    g_mainFrame->NoiseViewMenu->addAction(m_activationAction);
    m_NoiseToolBar = new NoiseToolBar(mainWindow, this);
    m_NoiseDock = new NoiseDock (tr("Noise"), mainWindow, 0, this);

}

void NoiseModule::onRedraw () {
    //NOT IMPLEMENTED
}

NoiseModule::~NoiseModule() {
}

void NoiseModule::configureGL() {
    //NOT IMPLEMENTED
}


void NoiseModule::addMainMenuEntries(){

}

void NoiseModule::OnCenterScene()
{

}


void NoiseModule::render(){
    //NOT IMPLEMENTED
}

void NoiseModule::UpdateView()
{
    //NOT IMPLEMENTED
}



void NoiseModule::initView(){

    if (m_firstView) {

        m_NoiseDock->initView();
        OnTwoDView();
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

    OnTwoDView();

    m_NoiseToolBar->show();
    m_NoiseDock->show();
    m_NoiseDock->setMinimumWidth(m_DockWidth);
    m_NoiseDock->setMaximumWidth(m_DockWidth);


    OnCenterScene();

    NoiseSimulation * ns = new NoiseSimulation();
    g_NoiseSimulationStore.add(ns);


    m_graph[0]->reloadCurves();

}

void NoiseModule::onModuleChanged (){

    if (g_mainFrame->getCurrentModule() == this) {
        ModuleBase::onModuleChanged();
        DualModule::hideModule();

        m_NoiseDock->hide();
        m_NoiseToolBar->hide();

    }

}

void NoiseModule::OnGLView()
{
    //NOT IMPLEMENTED
}

void NoiseModule::OnTwoDView()
{
    GLView = false;
    TwoDView = true;

    g_mainFrame->m_iView = NOISE_BPMTE_VIEW;

    setTwoDView();

}

QStringList NoiseModule::getAvailableGraphVariables(){

    QStringList lst = QStringList();

    lst.append("novo Item Graph");

    return lst;

}


