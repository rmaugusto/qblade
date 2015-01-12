#include "QLLTModule.h"
#include "../XGlobals.h"
#include "QLLTToolBar.h"
#include "QLLTDock.h"
#include <QMenu>
#include "../StoreAssociatedComboBox.h"
#include <QtOpenGL>
#include "../GLWidget.h"
#include "../Objects/Polar.h"
#include "../GlobalFunctions.h"








QLLTModule::QLLTModule(QMainWindow *mainWindow, QToolBar *toolbar) {

    m_globalModuleIndentifier = QLLTMODULE;
    m_QLLTSim = NULL;
    m_DockWidth = 220;

    m_graph[0] = new NewGraph ("LLTGraphOne", NewGraph::QFEMSimulation, this);
    m_graph[1] = new NewGraph ("LLTGraphTwo", NewGraph::QFEMSimulation, this);
    m_graph[2] = new NewGraph ("LLTGraphThree", NewGraph::QFEMSimulation, this);
    m_graph[3] = new NewGraph ("LLTGraphFour", NewGraph::QFEMSimulation, this);

    registrateAtToolbar(tr("QFEM - Structural Blade Design and Analysis"), tr("Define the Blade Internal Blade Structure and Perform a Modal Analysis"), ":/images/fem.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);
    m_QLLTToolBar = new QLLTToolBar(mainWindow, this);
    m_QLLTDock = new QLLTDock (tr("QLLT"), mainWindow, 0, this);


}

void QLLTModule::onRedraw () {

    render();

}

QLLTModule::~QLLTModule() {
    if (m_firstView == false) {
        delete m_graph[0];
        delete m_graph[1];
        delete m_graph[2];
        delete m_graph[3];
    }
}

void QLLTModule::configureGL() {
    // set background

    qDebug() << "configure gl";

    glClearColor(g_mainFrame->m_BackgroundColor.redF(),
                 g_mainFrame->m_BackgroundColor.greenF(),
                 g_mainFrame->m_BackgroundColor.blueF(),
                 0.0);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);  // accept fragment if it is closer to the camera than the former one
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // polygons are filled from both sides
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glLineWidth(3.0);
    // disable smooth functions that otherwise make rendering worse

//    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

void QLLTModule::OnSelChangeLLTSimulation(){

    m_QLLTSim = m_QLLTToolBar->m_LLTSimulationComboBox->currentObject();

    onRedraw();

    reportGLChange();

    OnCenterScene();

}




QLLTSimulation* QLLTModule::GetQLLTSim(){
    if (m_QLLTSim) return m_QLLTSim;
    else return NULL;
}


void QLLTModule::addMainMenuEntries(){

}

void QLLTModule::OnCenterScene()
{
    if(!m_QLLTSim) return;
    if (g_mainFrame->getCurrentModule() != this) return;

    m_glWidget->setSceneRadius(float(m_QLLTSim->m_pBladeList[0]->getRotorRadius()*10));
    m_glWidget->setSceneCenter(qglviewer::Vec(0,0,0));

    m_glWidget->showEntireScene();
    m_glWidget->updateGL();
}


void QLLTModule::render(){

    if (GLscreenMessage(QLLTMODULE,QLLTGLVIEW,m_glWidget)) return;
    if (TwoDView) return;

    if (m_QLLTSim) m_QLLTSim->render();
}

void QLLTModule::UpdateView()
{
    m_glWidget->updateGL();
}



void QLLTModule::initView(){

        if (m_firstView) {
            m_graph[0] = new NewGraph ("LLTGraphOne", NewGraph::QLLTSimulation, this);
            m_graph[1] = new NewGraph ("LLTGraphTwo", NewGraph::QLLTSimulation, this);
            m_graph[2] = new NewGraph ("LLTGraphThree", NewGraph::QLLTSimulation, this);
            m_graph[3] = new NewGraph ("LLTGraphFour", NewGraph::QLLTSimulation, this);

            m_QLLTDock->initView();
            OnGLView();
            m_firstView = false;

            OnCenterScene();
        }
}


void QLLTModule::showAll(){

}

void QLLTModule::hideAll(){

}

void QLLTModule::setContextMenuGraphType(NewGraph::GraphType graphType){

}

void QLLTModule::onActivationActionTriggered(){
    ModuleBase::onActivationActionTriggered();
    DualModule::showModule();

    if (GLView) OnGLView();
    else OnTwoDView();

    m_QLLTToolBar->show();
    m_QLLTDock->show();
    m_QLLTDock->setMinimumWidth(m_DockWidth);
    m_QLLTDock->setMaximumWidth(m_DockWidth);

    OnCenterScene();
}

void QLLTModule::onModuleChanged (){

    if (g_mainFrame->getCurrentModule() == this) {
        ModuleBase::onModuleChanged();
        DualModule::hideModule();
        m_QLLTDock->hide();
        m_QLLTToolBar->hide();
    }

}

void QLLTModule::OnGLView()
{
    GLView = true;
    TwoDView = false;
    m_QLLTToolBar->GLView->setChecked(GLView);
    m_QLLTToolBar->TwoDView->setChecked(TwoDView);

    g_mainFrame->m_iView = QLLTGLVIEW;

    setGLView();
}

void QLLTModule::OnTwoDView()
{
    GLView = false;
    TwoDView = true;
    m_QLLTToolBar->GLView->setChecked(GLView);
    m_QLLTToolBar->TwoDView->setChecked(TwoDView);

    g_mainFrame->m_iView = QLLTGRAPHVIEW;

    setTwoDView();

}

QStringList QLLTModule::getAvailableGraphVariables(){
    if (m_QLLTSim) return m_QLLTSim->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
    else return QStringList();
}

void QLLTModule::setShownLLTSimulation(QLLTSimulation *newShownLLTSimulation, bool forceReload) {
    if (forceReload || m_QLLTSim != newShownLLTSimulation) {
        m_QLLTSim = newShownLLTSimulation;
        m_QLLTDock->setShownLLTSimulation(m_QLLTSim);
        m_QLLTToolBar->useLLTSimulation(m_QLLTSim);
    }
}

