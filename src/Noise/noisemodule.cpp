#include "noisemodule.h"
#include "../XGlobals.h"
#include <QMenu>
#include "../StoreAssociatedComboBox.h"
#include <QtOpenGL>
#include "../GLWidget.h"
#include "../Objects/Polar.h"
#include "../GlobalFunctions.h"

NoiseModule::NoiseModule(QMainWindow *mainWindow, QToolBar *toolbar)
{

    m_DockWidth = 220;

    setGraphArrangement(Single);

    m_graph[0] = new NewGraph ("NoiseGraphOne", NewGraph::NoiseGraph, this);
    m_graph[1] = new NewGraph ("NoiseGraphTwo", NewGraph::NoiseGraph, this);
    m_graph[2] = new NewGraph ("NoiseGraphThree", NewGraph::NoiseGraph, this);
    m_graph[3] = new NewGraph ("NoiseGraphFour", NewGraph::NoiseGraph, this);


    registrateAtToolbar(tr("NOISE"), tr("NOISE"), ":/images/Noise-Icon.png", toolbar);
    g_mainFrame->NoiseViewMenu->addAction(m_activationAction);
    m_NoiseToolBar = new NoiseToolBar(mainWindow, this);
    m_NoiseDock = new NoiseDock (tr("Noise"), mainWindow, 0, this);



}

void NoiseModule::onRedraw () {

    render();

}

NoiseModule::~NoiseModule() {
}

void NoiseModule::configureGL() {
    // set background

    //qDebug() << "configure gl";

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


void NoiseModule::addMainMenuEntries(){

}

void NoiseModule::OnCenterScene()
{

}


void NoiseModule::render(){

    if (GLscreenMessage(NOISE_MODULE,NOISE_BPMTE_VIEW,m_glWidget)) return;
    if (TwoDView) return;

}

void NoiseModule::UpdateView()
{
    m_glWidget->updateGL();
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

void NoiseModule::setContextMenuGraphType(NewGraph::GraphType graphType){

}

void NoiseModule::onActivationActionTriggered(){
    ModuleBase::onActivationActionTriggered();
    DualModule::showModule();

    g_mainFrame->m_iApp = NOISE_MODULE;

    if (GLView) OnGLView();
    else OnTwoDView();

    m_NoiseToolBar->show();
    m_NoiseDock->show();
    m_NoiseDock->setMinimumWidth(m_DockWidth);
    m_NoiseDock->setMaximumWidth(m_DockWidth);


    OnCenterScene();
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
    GLView = true;
    TwoDView = false;

    g_mainFrame->m_iView = NOISE_Q3D_VIEW;

    setGLView();
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


