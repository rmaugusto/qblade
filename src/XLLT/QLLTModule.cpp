/****************************************************************************

    QLLTModule Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "QLLTModule.h"
#include <QtOpenGL>
#include <QMenu>
#include "QLLTToolBar.h"
#include "QLLTDock.h"
#include "../StoreAssociatedComboBox.h"
#include "../GLWidget.h"
#include "../Objects/Polar.h"
#include "../GlobalFunctions.h"
#include "QLLTTwoDContextMenu.h"
#include "../Store.h"
#include "QLLTSimulation.h"
#include "../TwoDWidget.h"
#include "../TwoDGraphMenu.h"


QLLTModule::QLLTModule(QMainWindow *mainWindow, QToolBar *toolbar) {
    m_globalModuleIndentifier = QLLTMODULE;
    m_QLLTSim = NULL;
    CutPlaneRendered = false;

    registrateAtToolbar(tr("QLLT - Nonlinear Lifting Line Simulations"),
                        tr("Performs an unsteady simulation of a HAWT or VAWT in the time domain"),
                        ":/images/LLT.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);
    m_QLLTToolBar = new QLLTToolBar(mainWindow, this);
    m_QLLTDock = new QLLTDock (tr("QLLT"), mainWindow, 0, this);

    m_QLLTContextMenu = new QLLTTwoDContextMenu (m_twoDWidget, this);
    setContextMenu(m_QLLTContextMenu);
    m_bComponentChanged = false;

    setGroupSize(g_mainFrame->m_WORK_GROUPS);

    m_bisVAWT = false;
	
	connect(&g_QLLTHAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadAllGraphs()));	
	connect(&g_QLLTVAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadAllGraphs()));	
}

void QLLTModule::drawGL () {
	if (TwoDView)
		return;
	if (m_QLLTSim) {
        m_QLLTSim->callGLLists();
		if (!m_QLLTSim->isRunning()){
            glEnable(GL_DEPTH_TEST);
			glCallList(GLCUTPLANES);
			glCallList(GLCUTPLANESETUP);
		}
	}
}

void QLLTModule::overpaint(QPainter &painter) {
	if (m_QLLTSim) {
		m_QLLTSim->drawOverpaint(painter);
	}
}

void QLLTModule::ChangeHAWTVAWTView (bool isVawt) {
	if (isVawt){
		m_bisVAWT = true;
		m_QLLTToolBar->OnVAWTView();
		m_QLLTDock->OnVAWTView();
		disconnect (m_QLLTDock->m_HAWTCutPlaneBox, SIGNAL(valueChanged(int)), this, SLOT(OnSelChangeCutPlane()));
		connect (m_QLLTDock->m_VAWTCutPlaneBox, SIGNAL(valueChanged(int)), this, SLOT(OnSelChangeCutPlane()));
		OnSelChangeVAWTLLTSimulation();
	}
	else {
		m_bisVAWT = false;
		m_QLLTToolBar->OnHAWTView();
		m_QLLTDock->OnHAWTView();
		disconnect (m_QLLTDock->m_VAWTCutPlaneBox, SIGNAL(valueChanged(int)), this, SLOT(OnSelChangeCutPlane()));
		connect (m_QLLTDock->m_HAWTCutPlaneBox, SIGNAL(valueChanged(int)), this, SLOT(OnSelChangeCutPlane()));
		OnSelChangeLLTSimulation();
	}
	
	OnSelChangeCutPlane();
	
    if (m_QLLTSim) {
        m_QLLTSim->setForceReRender();
        UpdateView();
	}
    if (TwoDView) {
        m_twoDWidget->update();
	}
}

QLLTModule::~QLLTModule() {
    if (m_firstView == false) {
        delete m_graph[0];
        delete m_graph[1];
        delete m_graph[2];
        delete m_graph[3];
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		settings.setValue(QString("modules/QLLTModule/graphArrangement"), getGraphArrangement());	
	}
}

void QLLTModule::addMainMenuEntries() {
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
}

QList<NewCurve *> QLLTModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											NewGraph::GraphType /*graphTypeMulti*/) {
	QList<NewCurve*> curves;
	if (m_bisVAWT) {
		g_QLLTVAWTSimulationStore.addAllCurves(&curves, xAxis, yAxis, graphType);
	} else {
		g_QLLTHAWTSimulationStore.addAllCurves(&curves, xAxis, yAxis, graphType);
	}
	return curves;
}

void QLLTModule::configureGL() {
    // set background
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

    if (m_bisVAWT) return;

    m_QLLTSim = m_QLLTToolBar->m_LLTHAWTSimulationComboBox->currentObject();

    OnStopReplay();
    m_QLLTToolBar->onStopReplay();

    setShownLLTSimulation(m_QLLTSim, true);

    OnCenterScene();
}

void QLLTModule::OnSelChangeVAWTLLTSimulation(){

    if (!m_bisVAWT) return;

    m_QLLTSim = m_QLLTToolBar->m_LLTVAWTSimulationComboBox->currentObject();

    OnStopReplay();
    m_QLLTToolBar->onStopReplay();

    setShownLLTSimulation(m_QLLTSim, true);

    OnCenterScene();
}

void QLLTModule::OnSelChangeCutPlane() {
    if (m_bisVAWT)
		m_QLLTCutPlane = m_QLLTDock->m_VAWTCutPlaneBox->currentObject();
    else
		m_QLLTCutPlane = m_QLLTDock->m_HAWTCutPlaneBox->currentObject();

    if (m_QLLTCutPlane){
    m_QLLTDock->m_X_res->setValue(m_QLLTCutPlane->m_X_res);
    m_QLLTDock->m_Y_res->setValue(m_QLLTCutPlane->m_Y_res);
    m_QLLTDock->m_width->setValue(m_QLLTCutPlane->m_width);
    m_QLLTDock->m_length->setValue(m_QLLTCutPlane->m_length);
    m_QLLTDock->m_x_rot->setValue(m_QLLTCutPlane->m_X_rot);
    m_QLLTDock->m_y_rot->setValue(m_QLLTCutPlane->m_Y_rot);
    m_QLLTDock->m_z_rot->setValue(m_QLLTCutPlane->m_Z_rot);
    m_QLLTDock->m_x_cut->setValue(m_QLLTCutPlane->m_X);
    m_QLLTDock->m_y_cut->setValue(m_QLLTCutPlane->m_Y);
    m_QLLTDock->m_z_cut->setValue(m_QLLTCutPlane->m_Z);
    QString strong;
    m_QLLTDock->m_cutPlaneTime->setText("At Time: "+strong.number(m_QLLTCutPlane->m_time)+" [s]");
    }
    else{
    m_QLLTDock->m_X_res->setValue(0);
    m_QLLTDock->m_Y_res->setValue(0);
    m_QLLTDock->m_width->setValue(0);
    m_QLLTDock->m_length->setValue(0);
    m_QLLTDock->m_x_rot->setValue(0);
    m_QLLTDock->m_y_rot->setValue(0);
    m_QLLTDock->m_z_rot->setValue(0);
    m_QLLTDock->m_x_cut->setValue(0);
    m_QLLTDock->m_y_cut->setValue(0);
    m_QLLTDock->m_z_cut->setValue(0);
    m_QLLTDock->m_cutPlaneTime->setText("At Time: - [s]");
    }

    OnRenderCutPlanes();
}

QStringList QLLTModule::prepareMissingObjectMessage() {
	return QLLTSimulation::prepareMissingObjectMessage();
}

void QLLTModule::OnDeleteCutPlane(){
    if (m_QLLTCutPlane) g_QLLTCutPlaneStore.remove(m_QLLTCutPlane);
}

void QLLTModule::OnDeleteAllCutPlanes(){

    g_QLLTCutPlaneStore.disableSignal();

    if (m_QLLTSim){
        for (int i=0;i<g_QLLTCutPlaneStore.size();i++){
            if (g_QLLTCutPlaneStore.at(i)->getParent() == m_QLLTSim){
                g_QLLTCutPlaneStore.removeAt(i);
                i--;
            }
        }
    }

    g_QLLTCutPlaneStore.enableSignal();
    g_QLLTCutPlaneStore.emitObjectListChanged(false);
}

void QLLTModule::OnUpdateCutPlane(){
    if (!m_QLLTCutPlane) return;

    m_QLLTCutPlane->m_length = m_QLLTDock->m_length->value();
    m_QLLTCutPlane->m_width = m_QLLTDock->m_width->value();
    m_QLLTCutPlane->m_X = m_QLLTDock->m_x_cut->value();
    m_QLLTCutPlane->m_Y = m_QLLTDock->m_y_cut->value();
    m_QLLTCutPlane->m_Z = m_QLLTDock->m_z_cut->value();
    m_QLLTCutPlane->m_X_res = m_QLLTDock->m_X_res->value();
    m_QLLTCutPlane->m_Y_res = m_QLLTDock->m_Y_res->value();
    m_QLLTCutPlane->m_X_rot = m_QLLTDock->m_x_rot->value();
    m_QLLTCutPlane->m_Y_rot = m_QLLTDock->m_y_rot->value();
    m_QLLTCutPlane->m_Z_rot = m_QLLTDock->m_z_rot->value();

    m_QLLTCutPlane->Update();
    OnRenderCutPlanes();
}


void QLLTModule::OnStopReplay(){
    if (!m_QLLTSim) return;
    if (!m_QLLTToolBar->m_QLLTThread) return;
    if (!m_QLLTToolBar->m_QLLTThread->isRunning()) return;

    m_QLLTSim->onStopReplay();
}

void QLLTModule::OnCenterScene() {
    if(!m_QLLTSim) return;
    if (g_mainFrame->getCurrentModule() != this) return;

    m_glWidget->setSceneRadius(float(m_QLLTSim->getUsedBlade()->getRotorRadius()*6));
    m_glWidget->setSceneCenter(qglviewer::Vec(0,0,0));

    m_glWidget->showEntireScene();
    m_glWidget->updateGL();
}

void QLLTModule::onStopSimulation(){
    m_QLLTSim->StopRequested();
    m_QLLTSim->setCanContinue(true);
}

void QLLTModule::forceReRender(){
    if (!m_QLLTSim) return;
    m_QLLTSim->setForceReRender();
    UpdateView();
}

void QLLTModule::OnRenderCutPlanes(){
    CutPlaneRendered = false;

    if (m_bComponentChanged){

        if (m_QLLTDock->m_componentButton->checkedId() == 0){
            m_QLLTDock->SetRange(0.8);
        }
        else if (m_QLLTDock->m_componentButton->checkedId() == 1){
            m_QLLTDock->SetRange(0.4);
        }
        else if (m_QLLTDock->m_componentButton->checkedId()  == 2){
            m_QLLTDock->SetRange(0.15);
        }
        else if (m_QLLTDock->m_componentButton->checkedId()  == 3){
            m_QLLTDock->SetRange(0.15);
        }
        else if (m_QLLTDock->m_componentButton->checkedId()  == 4){
            m_QLLTDock->SetRange(0.4);
        }
        else if (m_QLLTDock->m_componentButton->checkedId()  == 5){
            m_QLLTDock->SetRange(0.05);
        }
        else if (m_QLLTDock->m_componentButton->checkedId()  == 6){
            m_QLLTDock->SetRange(0.15);
        }
    }
    m_bComponentChanged = false;


    if (!GLView) return;
    if (glIsList(GLCUTPLANES))  glDeleteLists(GLCUTPLANES, 1);
    if (glIsList(GLCUTPLANESETUP))  glDeleteLists(GLCUTPLANESETUP, 1);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glNewList(GLCUTPLANES,GL_COMPILE);
    if (m_bisVAWT && m_QLLTDock->m_showCutPlanes->isChecked()){
        for (int i=0;i<m_QLLTDock->m_VAWTCutPlaneBox->count();i++){
            if (!m_QLLTSim->getStoreWake() || (m_QLLTSim->getStoreWake() && m_QLLTSim->getResultsArray().at(0)->at(m_QLLTSim->getShownTimeIndex()) == m_QLLTDock->m_VAWTCutPlaneBox->getObjectAt(i)->m_time)){
                m_QLLTDock->m_VAWTCutPlaneBox->getObjectAt(i)->render(m_QLLTDock->m_colorMapButton->button(0)->isChecked(), m_QLLTDock->m_showVelVectors->isChecked(), m_QLLTDock->m_componentButton->checkedId(), m_QLLTDock->GetRange());
                CutPlaneRendered = true;
            }
        }
    }
    else if (m_QLLTDock->m_showCutPlanes->isChecked()){
        for (int i=0;i<m_QLLTDock->m_HAWTCutPlaneBox->count();i++){
            if (!m_QLLTSim->getStoreWake() || (m_QLLTSim->getStoreWake() && m_QLLTSim->getResultsArray().at(0)->at(m_QLLTSim->getShownTimeIndex()) == m_QLLTDock->m_HAWTCutPlaneBox->getObjectAt(i)->m_time)){
                m_QLLTDock->m_HAWTCutPlaneBox->getObjectAt(i)->render(m_QLLTDock->m_colorMapButton->button(0)->isChecked(), m_QLLTDock->m_showVelVectors->isChecked(), m_QLLTDock->m_componentButton->checkedId(), m_QLLTDock->GetRange());
                CutPlaneRendered = true;
            }
        }
    }
    glEndList();


    if (m_QLLTCutPlane){
    glNewList(GLCUTPLANESETUP,GL_COMPILE);
    if (!m_QLLTCutPlane->is_computed){
        m_QLLTCutPlane->drawFrame();
        m_QLLTCutPlane->render(m_QLLTDock->m_colorMapButton->button(0)->isChecked(), m_QLLTDock->m_showVelVectors->isChecked(), m_QLLTDock->m_componentButton->checkedId());
    }
    else if (m_QLLTCutPlane->m_time == m_QLLTSim->getResultsArray().at(0)->at(m_QLLTSim->getShownTimeIndex()) && m_QLLTSim->getStoreWake() && m_QLLTDock->m_showCutPlanes->isChecked()) m_QLLTCutPlane->drawFrame();
    else if (!m_QLLTSim->getStoreWake()&& m_QLLTDock->m_showCutPlanes->isChecked()) m_QLLTCutPlane->drawFrame();
    glEndList();
    }


    m_glWidget->update();
}

void QLLTModule::setShownTimeForAllSimulations(){
    m_QLLTToolBar->setShownTimeForAllSimulations();
}

void QLLTModule::ResetTimestepSlider(){
    return m_QLLTToolBar->m_Timesteps->setValue(0);
}


void QLLTModule::UpdateView() {
    if (m_QLLTSim) m_QLLTSim->drawGL();
    m_glWidget->update();
}

void QLLTModule::initView(){
	if (m_firstView) {
		m_graph[0] = new NewGraph ("LLTGraphOne", this, {NewGraph::QLLTTimeGraph, "", "", false, false});
		m_graph[1] = new NewGraph ("LLTGraphTwo", this, {NewGraph::QLLTTimeGraph, "", "", false, false});
		m_graph[2] = new NewGraph ("LLTGraphThree", this, {NewGraph::QLLTTimeGraph, "", "", false, false});
		m_graph[3] = new NewGraph ("LLTGraphFour", this, {NewGraph::QLLTTimeGraph, "", "", false, false});
		
		OnGLView();
		m_firstView = false;
		
		m_glWidget->camera()->setUpVector(qglviewer::Vec(0,0,1));
		m_glWidget->camera()->setViewDirection(qglviewer::Vec(1,1,-1));
		
		OnCenterScene();
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
							(settings.value("modules/QLLTModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));
	}
}

void QLLTModule::showAll() {
	if (m_bisVAWT) {
		g_QLLTVAWTSimulationStore.showAllCurves(true);
    } else {
		g_QLLTHAWTSimulationStore.showAllCurves(true);
    }
    reloadAllGraphCurves();
    m_QLLTDock->adjustShowCheckBox();
}

void QLLTModule::hideAll() {
    if (m_bisVAWT) {
		g_QLLTVAWTSimulationStore.showAllCurves(false, m_QLLTSim);
    } else {
		g_QLLTHAWTSimulationStore.showAllCurves(false, m_QLLTSim);
    }
	reloadAllGraphCurves();
    m_QLLTDock->adjustShowCheckBox();
}

void QLLTModule::onActivationActionTriggered(){
    ModuleBase::onActivationActionTriggered();
    DualModule::showModule();

    if (GLView) OnGLView();
    else OnTwoDView();

    m_QLLTToolBar->show();
    m_QLLTDock->show();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    m_QLLTDock->setFixedWidth(width/5);


    OnCenterScene();
}

void QLLTModule::onModuleChanged (){
    if (g_mainFrame->getCurrentModule() == this) {
        ModuleBase::onModuleChanged();
        DualModule::hideModule();
        OnStopReplay();
        m_QLLTToolBar->onStopReplay();
        m_QLLTDock->hide();
        m_QLLTToolBar->hide();
    }
}

void QLLTModule::OnGLView()
{
    GLView = true;
    TwoDView = false;
    OnStopReplay();
    m_QLLTToolBar->onStopReplay();
    m_QLLTToolBar->GLView->setChecked(GLView);
    m_QLLTToolBar->TwoDView->setChecked(TwoDView);
    m_QLLTDock->OnGLView();
    setGLView();
    forceReRender();
    OnRenderCutPlanes();
}




void QLLTModule::OnTwoDView()
{
    GLView = false;
    TwoDView = true;
	reloadAllGraphs();
    m_QLLTToolBar->onStopReplay();
    m_QLLTToolBar->GLView->setChecked(GLView);
    m_QLLTToolBar->TwoDView->setChecked(TwoDView);
    m_QLLTDock->OnTwoDView();

    OnStopReplay();
    m_QLLTToolBar->onStopReplay();

    setTwoDView();

}

void QLLTModule::DisableButtons(){
    m_QLLTToolBar->DisableBoxes();
    g_mainFrame->HAWTToolbarView->setEnabled(false);
    g_mainFrame->VAWTToolbarView->setEnabled(false);
}

void QLLTModule::EnableButtons(){
    m_QLLTToolBar->EnableBoxes();
    g_mainFrame->HAWTToolbarView->setEnabled(true);
    g_mainFrame->VAWTToolbarView->setEnabled(true);

}

QStringList QLLTModule::getAvailableGraphVariables(bool /*xAxis*/){
    if (m_QLLTSim) return m_QLLTSim->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
	else return QStringList();
}

QPair<ShowAsGraphInterface *, int> QLLTModule::getHighlightDot(NewGraph::GraphType graphType) {
	switch (graphType) {
	case NewGraph::QLLTTimeGraph:
		if (m_QLLTSim) {
			return QPair<ShowAsGraphInterface*,int> (m_QLLTSim, m_QLLTSim->getShownTimeIndex());
		}
	default:
		return QPair<ShowAsGraphInterface*,int> (NULL, -1);
	}
}

void QLLTModule::UpdateDockToolbar(){
    m_QLLTToolBar->useLLTSimulation(m_QLLTSim);
    m_QLLTDock->setShownLLTSimulation(m_QLLTSim);
    forceReRender();
}


void QLLTModule::setShownLLTSimulation(QLLTSimulation *newShownLLTSimulation, bool forceReload) {
    if (forceReload || m_QLLTSim != newShownLLTSimulation) {
        m_QLLTSim = newShownLLTSimulation;
        m_QLLTDock->setShownLLTSimulation(m_QLLTSim);
        m_QLLTToolBar->useLLTSimulation(m_QLLTSim);
        forceReRender();
    }
}

QLLTModule *g_QLLTModule;
