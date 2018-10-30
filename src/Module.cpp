#include "Module.h"

#include <QStackedWidget>
#include <QDebug>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QSettings>

#include "TwoDWidget.h"
#include "GLWidget.h"
#include "MainFrame.h"
#include "TwoDGraphMenu.h"


ModuleBase::ModuleBase ()
{
	m_firstView = true;
	connect (g_mainFrame, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));		
}

void ModuleBase::setActionIcon(QString path){
    m_activationAction->setIcon(QIcon(path));
}

void ModuleBase::registrateAtToolbar(QString name, QString tooltip, QString imagePath, QToolBar *toolbar) {
	/* create action that will call the module and add it to the main toolbar */
	m_activationAction = new QAction(QIcon(imagePath), name, g_mainFrame);
	m_activationAction->setCheckable(true);
	m_activationAction->setStatusTip(tooltip);
	connect(m_activationAction, SIGNAL(triggered()), this, SLOT(onActivationActionTriggered()));
    toolbar->addAction(m_activationAction);
}

void ModuleBase::setToolbarVisibility (bool visible){
    /* set the action in the toolbar to visible or invisible */
    m_activationAction->setVisible (visible);
}

void ModuleBase::onActivationActionTriggered() {
	g_mainFrame->setIApp(m_globalModuleIndentifier);  // will deactivate last module as side effect
	g_mainFrame->setCurrentModule(this);
	m_activationAction->setChecked (true);
	g_mainFrame->SetMenus();
	initView();
}

void ModuleBase::onModuleChanged () {
	m_activationAction->setChecked (false);
}

void ModuleBase::addMainMenuEntries() {
	// nothing by default
}



GLModule::GLModule () {
	m_glWidget = g_mainFrame->getGlWidget();
	
	static int indexer = 0;
	m_cameraSaveIndex = indexer++;
}

void GLModule::reportGLChange() {
	m_glWidget->update();
}

void GLModule::showModule() {
	/* The saving of GL settings and especially the restoring of the former unfortunately isn't done by
	 * modules that do not inherit this class. Therefore in configureGL several settings must be set
	 * back to default for the correct behaviour
	 * */
	glPushAttrib(GL_ALL_ATTRIB_BITS);  // saves current GL settings
	m_glWidget->camera()->playPath(m_cameraSaveIndex);
	m_glWidget->camera()->deletePath(m_cameraSaveIndex);
	configureGL();
}

void GLModule::hideModule() {
	m_glWidget->camera()->addKeyFrameToPath(m_cameraSaveIndex);
	glPopAttrib();  // restores the saved GL settings
}



TwoDModule::TwoDModule () {
	m_graphMenu = new TwoDGraphMenu (g_mainFrame, this);
}

TwoDModule::~TwoDModule() {
	
}

void TwoDModule::showModule() {
	onResizeEvent();  // the size might have changed while this module was inactive
	m_twoDWidget->setMouseTracking(false);  // the old modules need this property set true for bad reasons
}

void TwoDModule::hideModule() {
	m_twoDWidget->setMouseTracking(true);
}



DualModule::DualModule () {

}

void DualModule::hideModule() {
	GLModule::hideModule();
	TwoDModule::hideModule();
}

void DualModule::showModule() {
	GLModule::showModule();
	TwoDModule::showModule();
}

void DualModule::setTwoDView() {
	g_mainFrame->switchToTwoDWidget();
}

void DualModule::setGLView() {
	g_mainFrame->switchToGlWidget();
}
