#include "WindFieldModule.h"

#include <QMenuBar>
#include <QFileDialog>

#include "../MainFrame.h"
#include "WindFieldDock.h"
#include "WindFieldToolBar.h"
#include "WindFieldMenu.h"
#include "../GlobalFunctions.h"
#include "../GLWidget.h"
#include "WindField.h"
#include "../Store.h"

WindFieldModule::WindFieldModule(QMainWindow *mainWindow, QToolBar *toolbar) {
	m_globalModuleIndentifier = WINDFIELDMODULE;
	
	m_shownWindField = NULL;
	m_windFieldMenu = new WindFieldMenu (mainWindow, this);
	mainWindow->menuBar()->addMenu(m_windFieldMenu);
	m_windFieldToolbar = new WindFieldToolBar (mainWindow, this);
	m_windFieldDock = new WindFieldDock ("Windfield", mainWindow, 0, this);

	registrateAtToolbar(tr("Turbulent Windfield Generator"), tr("Create a Turbulent Windfield for a FAST Simulation"),
																	":/images/WindFieldCreator.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);

}

WindFieldModule::~WindFieldModule () {
	
}

void WindFieldModule::OnImportBinaryWindField(){
    QString fileName;

    fileName = QFileDialog::getOpenFileName(g_mainFrame, "Open Binary Windfield File", g_mainFrame->m_LastDirName,
											"Binary Wind Field File (*.bts)");
    if(!fileName.length()) return;
    int pos = fileName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_LastDirName = fileName.left(pos);

    QString windfieldname = g_mainFrame->m_LastDirName = fileName.right(pos);

    pos = fileName.lastIndexOf(".");
    if(pos>0) windfieldname = fileName.left(pos);

    QFile windfieldFile (fileName);
    if (windfieldFile.open(QIODevice::ReadOnly)) {
        QDataStream fileStream (&windfieldFile);
        fileStream.setByteOrder(QDataStream::LittleEndian);
        fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
		WindField *importWindField = WindField::newByImport(fileStream);
		
        g_windFieldStore.add(importWindField);
        setShownWindField(importWindField);
        reportGLChange();
		
		windfieldFile.close();
    }
}

void WindFieldModule::setShownWindField(WindField *newShownWindField) {
	m_shownWindField = newShownWindField;
	m_windFieldDock->setShownObject(m_shownWindField);
	m_windFieldToolbar->useWindField(m_shownWindField);
	reportGLChange();
}

void WindFieldModule::drawGL() {
	if (m_shownWindField && m_shownWindField->isValid()) {
		m_shownWindField->render();
	}
}

void WindFieldModule::overpaint(QPainter &painter) {
	if (m_shownWindField && m_shownWindField->isValid()) {
		const int barWidth = 25;
		const int barHeight = 100;
		const int marginLeft = 15;
		const int marginTop = 55;
		
		QLinearGradient gradient (QPointF(0, marginTop), QPointF(0, marginTop+barHeight));
		for (int i = 0; i < 30; ++i) {
			QColor color;
			color.setHsv(225.0/29*i, 255, 255);
			gradient.setColorAt(1.0/29*i, color);
		}
		
		painter.setPen(QPen(QBrush("black"), 1));
		painter.setBrush(gradient);
		painter.drawRect(marginLeft, marginTop, barWidth, barHeight);
		painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 12));
		painter.drawText(marginLeft+barWidth+5, marginTop+6,
						 QString("%1 m/s").arg(m_shownWindField->maxValue(), 0, 'f', 2));
		painter.drawText(marginLeft+barWidth+5, marginTop+barHeight+6,
						 QString("%1 m/s").arg(/*m_shownWindField->minValue()*/0.0, 0, 'f', 2));
		painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 25));
		painter.drawText(marginLeft, 35, QString("%1").number(m_shownWindField->getShownTimestep()+1));
	}
}

void WindFieldModule::addMainMenuEntries() {
    g_mainFrame->menuBar()->addMenu(g_mainFrame->BEMViewMenu);
	g_mainFrame->menuBar()->addMenu(m_windFieldMenu);
}

QStringList WindFieldModule::prepareMissingObjectMessage() {
	return WindField::prepareMissingObjectMessage();
}

void WindFieldModule::initView() {
	if (m_firstView) {
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
	m_windFieldDock->show();
	m_windFieldToolbar->show();
    configureGL();
}

void WindFieldModule::onModuleChanged() {
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		GLModule::hideModule();
		m_windFieldDock->hide();
		m_windFieldToolbar->hide();
	}
}

WindFieldModule *g_windFieldModule;
