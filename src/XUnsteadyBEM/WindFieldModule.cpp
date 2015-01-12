#include "WindFieldModule.h"

#include <QMenuBar>

#include "../MainFrame.h"
#include "WindFieldCreatorDock.h"
#include "WindFieldToolBar.h"
#include "WindFieldMenu.h"
#include "../GlobalFunctions.h"

extern MainFrame *g_mainFrame;
extern WindFieldStore g_windFieldStore;


WindFieldModule::WindFieldModule(QMainWindow *mainWindow, QToolBar *toolbar) {
	m_globalModuleIndentifier = WINDFIELDMODULE;
    m_DockWidth = 220;
	
	m_shownWindField = NULL;
	m_windFieldMenu = new WindFieldMenu (mainWindow, this);
	mainWindow->menuBar()->addMenu(m_windFieldMenu);
	m_windFieldToolbar = new WindFieldToolBar (mainWindow, this);
	m_windFieldCreatorDock = new WindFieldCreatorDock (tr("Wind Field Creator"), mainWindow, 0);
	connect(&g_windFieldStore, SIGNAL(objectRenamed(QString,QString)),
										m_windFieldCreatorDock, SLOT(onWindFieldRenamed(QString,QString)));
    registrateAtToolbar(tr("Turbulent Windfield Generator"), tr("Create a Turbulent Windfield for a FAST Simulation"),
																	":/images/WindFieldCreator.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);

}

WindFieldModule::~WindFieldModule () {
}

void WindFieldModule::setShownWindField(WindField *newShownWindField) {
	m_shownWindField = newShownWindField;
	m_windFieldCreatorDock->useWindField(m_shownWindField);
	m_windFieldToolbar->useWindField(m_shownWindField);
	reportGLChange();
}

void WindFieldModule::onRedraw() {

    if (GLscreenMessage(WINDFIELDMODULE,0,m_glWidget)) return;

	if (m_shownWindField && m_shownWindField->isValid()) {
		m_shownWindField->render();
		drawLegend();
	}
}


void WindFieldModule::addMainMenuEntries() {
    g_mainFrame->menuBar()->addMenu(g_mainFrame->BEMViewMenu);
	g_mainFrame->menuBar()->addMenu(m_windFieldMenu);
}

void WindFieldModule::initView() {
	if (m_firstView) {
		m_windFieldCreatorDock->initView();
//        m_glWidget->setSceneCenter(qglviewer::Vec(0,0,0));
//        m_glWidget->setSceneRadius(1.0);
//        m_glWidget->showEntireScene();
		
		m_firstView = false;
	}
}

void WindFieldModule::configureGL() {
	// set background
	glClearColor(g_mainFrame->m_BackgroundColor.redF(),
				 g_mainFrame->m_BackgroundColor.greenF(),
				 g_mainFrame->m_BackgroundColor.blueF(),
				 0.0);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);  // accept fragment if it is closer to the camera than the former one
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // polygons are filled from both sides
	glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
	glPolygonOffset(1.0, 0);
	glLineWidth(1);
	// disable smooth functions that otherwise make rendering worse

    glDisable(GL_POLYGON_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
	
	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);	
}

void WindFieldModule::drawLegend() {
	glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();	
	glLoadIdentity();
	
	const double width = m_glWidget->width();
	const double height = m_glWidget->height();
	glDisable(GL_DEPTH_TEST);
    int num = 30;
    double w = 50;  //height and width of the legend
    double h = 200;
    double h_incr = h / num;

    hsv hs;         //the hue saturation value color struct
    hs.s = 1.0;
    hs.v = 1.0;
    hs.h = 0;

    for (int i = 1; i <= num;i++){
    glBegin (GL_QUADS);  // the filling

    glColor3f (hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
    glVertex2d((20.0)/width -1, 1- (100.0+h_incr*(i-1))/height);  // 20.0 is twice the distance to the edge
    glVertex2d((20.0+w)/width -1, 1- (100.0+h_incr*(i-1))/height);

    hs.h += 225/num;
    glColor3f (hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
    glVertex2d((20.0+w)/width -1, 1- (100.0+h_incr*i)/height);
    glVertex2d((20.0)/width -1, 1- (100.0+h_incr*i)/height);
    glEnd ();
    }


    glBegin (GL_LINE_LOOP);  // the edges
    glColor3d(0, 0, 0);
    glVertex2d(20.0/width -1, 1- 100.0/height);
    glVertex2d(70.0/width -1, 1- 100.0/height);
    glVertex2d(70.0/width -1, 1- 300.0/height);
    glVertex2d(20.0/width -1, 1- 300.0/height);
    glEnd ();
	
	m_glWidget->renderText(20.0/width -1, 1- 70.0/height, 0,
						   QString("%1").number(m_shownWindField->getShownTimestep()+1),  // index shift
						   QFont("Arial", 25));
	m_glWidget->renderText(80.0/width -1, 1- 112.0/height, 0,
						   QString("%1 m/s").arg(m_shownWindField->maxValue(), 0, 'f', 2),
						   QFont ("Arial", 12));
	m_glWidget->renderText(80.0/width -1, 1- 312.0/height, 0,
                           QString("%1 m/s").arg(/*m_shownWindField->minValue()*/0.0, 0, 'f', 2),
						   QFont ("Arial", 12));
	
	glMatrixMode(GL_MODELVIEW);  // restore state
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib ();
}

void WindFieldModule::onShownWindFieldChanged(int newIndex) {
	setShownWindField (g_windFieldStore.at(newIndex));
}

void WindFieldModule::onActivationActionTriggered() {
	m_glWidget->setSceneCenter(qglviewer::Vec (0, 0, 0));
	m_glWidget->setSceneRadius(2);
    m_glWidget->showEntireScene();
    m_glWidget->camera()->setType(qglviewer::Camera::PERSPECTIVE);
	ModuleBase::onActivationActionTriggered();
	GLModule::showModule();
	g_mainFrame->switchToGlWidget();
	m_windFieldCreatorDock->show();
	m_windFieldToolbar->show();
    m_windFieldCreatorDock->setMinimumWidth(m_DockWidth);
    m_windFieldCreatorDock->setMaximumWidth(m_DockWidth);
}

void WindFieldModule::onModuleChanged() {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		GLModule::hideModule();
		m_windFieldCreatorDock->hide();
		m_windFieldToolbar->hide();
	}
}
