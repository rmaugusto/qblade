/****************************************************************************

    QFEMModule Class
        Copyright (C) 2014 David Marten david.marten@tu-berlin.de

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

#include <QDebug>
#include <QStandardItemModel>
#include <QGroupBox>
#include "QFEMModule.h"
#include <QColorDialog>
#include <QMenuBar>
#include "QFEMToolBar.h"
#include "QFEMMenu.h"
#include "QFEMDock.h"
#include <QTableView>
#include "structelem.h"
#include "Clipper/clipper.hpp"
#include "../StoreAssociatedComboBox.h"
#include "../GLWidget.h"
#include "../Graph/NewGraph.h"
#include "../Globals.h"
#include "QFEMTwoDContextMenu.h"
#include "BladeStructureLoading.h"
#include "../GlobalFunctions.h"
#include "../TwoDWidget.h"
#include "../Misc/NumberEdit.h"
#include "../Store.h"
#include "../TwoDGraphMenu.h"

extern bool ObjectIsEdited;

#ifndef WIN32
#define __stdcall
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

using namespace std;
using Eigen::Vector2d;

stringstream ss;
GLdouble vertices[200][6];               // arrary to store newly created vertices (x,y,z,r,g,b) by combine callback
int vertexIndex = 0;

void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);
    // DEBUG //
    ss << "glBegin";
}

void CALLBACK tessEndCB()
{
    glEnd();

    // DEBUG //
    ss << "glEnd();\n";
}



void CALLBACK tessVertexCB(const GLvoid *data)
{
    // cast back to double type
    const GLdouble *ptr = (const GLdouble*)data;

    glVertex3dv(ptr);

    // DEBUG //
    ss << "  glVertex3d(" << *ptr << ", " << *(ptr+1) << ", " << *(ptr+2) << ");\n";
}



void CALLBACK tessErrorCB(GLenum errorCode)
{
    const GLubyte *errorStr;

    errorStr = gluErrorString(errorCode);
    cerr << "[ERROR]: " << errorStr << endl;
}

void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData)
{
    // copy new intersect vertex to local array
    // Because newVertex is temporal and cannot be hold by tessellator until next
    // vertex callback called, it must be copied to the safe place in the app.
    // Once gluTessEndPolygon() called, then you can safly deallocate the array.
    vertices[vertexIndex][0] = newVertex[0];
    vertices[vertexIndex][1] = newVertex[1];
    vertices[vertexIndex][2] = newVertex[2];

    // compute vertex color with given weights and colors of 4 neighbors
    // the neighborVertex[4] must hold required info, in this case, color.
    // neighborVertex was actually the third param of gluTessVertex() and is
    // passed into here to compute the color of the intersect vertex.
    vertices[vertexIndex][3] = neighborWeight[0] * neighborVertex[0][0];   // red this line is only uncommented to prevent compile warnings :-)
//                               neighborWeight[1] * neighborVertex[1][3] +
//                               neighborWeight[2] * neighborVertex[2][3] +
//                               neighborWeight[3] * neighborVertex[3][3];
//    vertices[vertexIndex][4] = neighborWeight[0] * neighborVertex[0][4] +   // green
//                               neighborWeight[1] * neighborVertex[1][4] +
//                               neighborWeight[2] * neighborVertex[2][4] +
//                               neighborWeight[3] * neighborVertex[3][4];
//    vertices[vertexIndex][5] = neighborWeight[0] * neighborVertex[0][5] +   // blue
//                               neighborWeight[1] * neighborVertex[1][5] +
//                               neighborWeight[2] * neighborVertex[2][5] +
//                               neighborWeight[3] * neighborVertex[3][5];


    // return output data (vertex coords and others)
    *outData = vertices[vertexIndex];   // assign the address of new intersect vertex

    ++vertexIndex;  // increase index for next vertex
}

QFEMModule::QFEMModule(QMainWindow *mainWindow, QToolBar *toolbar) {
    m_axes = false;
	m_needToRerender = true;
	m_globalModuleIndentifier = QFEMMODULE;
    m_newSectionHighlight = true;
    m_modeType = 0;
    m_modeNumber = 0;
    QFEMCompleted = false;

	m_bResetglGeom = true;
	m_structure = NULL;
    m_deformed_rotor = NULL;
    m_rotor = NULL;
	m_bStructEdited = false;

    registrateAtToolbar(tr("QFEM - Structural Blade Design and Analysis"),
						tr("Define the Blade Internal Blade Structure and Perform a Modal Analysis"),
						":/images/fem.png", toolbar);
    g_mainFrame->BEMViewMenu->addAction(m_activationAction);
    m_QFEMToolBar = new QFEMToolBar(mainWindow, this);
    m_QFEMDock = new QFEMDock (tr("QFEM"), mainWindow, 0, this);

    m_twoDContextMenu = new QFEMTwoDContextMenu (g_mainFrame, this);
    setContextMenu(m_twoDContextMenu);
	
	connect(&g_bladeStructureStore, SIGNAL(objectListChanged(bool)), this, SLOT(reloadFemGraphs()));
}

QFEMModule::~QFEMModule() {
	if (m_firstView == false) {
		delete m_graph[0];
		delete m_graph[1];
		delete m_graph[2];
		delete m_graph[3];
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		settings.setValue(QString("modules/QFEMModule/graphArrangement"), getGraphArrangement());	
	}
}

QList<NewCurve *> QFEMModule::prepareCurves(QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											NewGraph::GraphType /*graphTypeMulti*/) {
	QList<NewCurve*> curves;
	g_bladeStructureStore.addAllCurves(&curves, xAxis, yAxis, graphType);
	return curves;
}

QStringList QFEMModule::getAvailableGraphVariables(bool /*xAxis*/) {
    if (m_structure) {
		return m_structure->getAvailableVariables(m_graph[m_currentGraphIndex]->getGraphType());
    } else {
        return QStringList();
	}
}

QPair<ShowAsGraphInterface *, int> QFEMModule::getHighlightDot(NewGraph::GraphType) {
	return QPair<ShowAsGraphInterface *, int> (NULL, -1);  // TODO
}

CBlade* QFEMModule::GetCurrentBlade()
{
    return m_QFEMToolBar->m_rotorComboBox->currentObject();
}

BladeStructure* QFEMModule::GetCurrentStructure()
{
    return m_QFEMToolBar->m_BladeStructureComboBox->currentObject();
}


void QFEMModule::OnGLView() {
    if (m_QFEMDock->m_tabwidget->currentIndex()==0) g_mainFrame->m_iView = QFEMSTRUCTVIEW;
    if (m_QFEMDock->m_tabwidget->currentIndex()==1) g_mainFrame->m_iView = QFEMLOADINGVIEW;
    GLView = true;
    TwoDView = false;
    m_QFEMToolBar->GLView->setChecked(GLView);
    m_QFEMToolBar->TwoDView->setChecked(TwoDView);
    m_QFEMDock->CurvesBox->hide();
    m_QFEMDock->viewWidget->show();


    setGLView();
    if (!m_QFEMToolBar->HideWidgetsAct->isChecked() && g_mainFrame->m_iApp == QFEMMODULE){
        m_QFEMDock->show();
    }
    UpdateGeomRerenderGL();
}

void QFEMModule::OnTwoDView() {
    GLView = false;
    TwoDView = true;
    g_mainFrame->m_iView = QFEMTWODVIEW;
    m_QFEMToolBar->GLView->setChecked(GLView);
    m_QFEMToolBar->TwoDView->setChecked(TwoDView);
    m_QFEMDock->viewWidget->hide();
    m_QFEMDock->CurvesBox->show();
    setTwoDView();
    if (!m_QFEMToolBar->HideWidgetsAct->isChecked() && g_mainFrame->m_iApp == QFEMMODULE){
        m_QFEMDock->show();
    }
//    m_QFEMDock->hide();
}


void QFEMModule::drawGL () {
	if (m_axes && getBlade())
		getBlade()->drawCoordinateAxes();
	
	render();
}

void QFEMModule::overpaint(QPainter &painter) {

    if (m_QFEMDock->m_LoadingShown && m_QFEMDock->m_Loading && m_structure && !ObjectIsEdited) {
		/* find the extreme values on this blade */
		double maxmax = 0;
		double minmin = std::numeric_limits<double>::max();
		BladeStructureLoading *loading = m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject();
		if (loading) {
			for (int i = 0; i <= m_rotor->m_NSurfaces; ++i) {
				for (int j = 0; j < m_structure->m_numFoilPoints-2; ++j) {
					if (loading->VMStresses.at(i).at(j) > maxmax)
						maxmax = loading->VMStresses.at(i).at(j);
					if (loading->VMStresses.at(i).at(j)<minmin)
						minmin = loading->VMStresses.at(i).at(j);
				}
			}
        }

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
						 QString("%1 MPa").arg(maxmax/1000000, 0, 'f', 2));
		painter.drawText(marginLeft+barWidth+5, marginTop+barHeight+6,
						 QString("%1 MPa").arg(minmin/1000000, 0, 'f', 2));
		
    }

    if (m_structure && !ObjectIsEdited){
        /* find some parameters that are to be drawn */
        QString modeTypeName;
        double frequency = 0;
        if (m_modeType == 0 && m_structure->FlapwiseFrequencies.size()) {
            frequency = m_structure->FlapwiseFrequencies.at(m_modeNumber);
            modeTypeName = "Flapwise";
        } else if (m_modeType == 1 && m_structure->EdgewiseFrequencies.size()) {
            frequency = m_structure->EdgewiseFrequencies.at(m_modeNumber);
            modeTypeName = "Edgewise";
        } else if (m_modeType == 2 && m_structure->TorsionalFrequencies.size()) {
            frequency = m_structure->TorsionalFrequencies.at(m_modeNumber);
            modeTypeName = "Torsional";
        } else if (m_modeType == 3 && m_structure->RadialFrequencies.size()) {
            frequency = m_structure->RadialFrequencies.at(m_modeNumber);
            modeTypeName = "Radial";
        } else if (m_modeType == 4 && m_structure->UnsortedFrequencies.size()) {
            frequency = m_structure->UnsortedFrequencies.at(m_modeNumber);
            modeTypeName = "Unsorted";
        }

        const double width = m_glWidget->width();
        const double height = m_glWidget->height();
        if (width > 300) {
            painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 10));
            if (m_structure->QFEMCompleted && m_QFEMDock->m_tabwidget->currentIndex() == 0) {
                painter.drawText(10, height-40, modeTypeName + QString(" Mode ") + QString("%1").number(m_modeNumber+1) +
                                 QString(" Eigenfrequency: ") + QString("%1").number(frequency)+QString(" Hz"));
                painter.drawText(width-220, height-40, QString("Blade Mass: ") + QString("%1").number(m_structure->blademass) +
                                 QString(" kg"));
                painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 15));
                painter.drawText(10, 25, m_rotor->getName()+QString(": ")+m_structure->getName());
            } else if (m_QFEMDock->m_Loading) {
                if (m_QFEMDock->m_Loading->simulationFinished){
                painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 10));
                painter.drawText(10, height-40, QString("X Axis Tip Defl.: ") +
                                 QString("%1").number(m_QFEMDock->m_Loading->nodeTranslations.at(
                                                          m_structure->m_numElems-1).at(0)) + QString(" [m] "));
                painter.drawText(width-220, height-40, QString("Z Axis Tip Defl.: ") +
                                 QString("%1").number(m_QFEMDock->m_Loading->nodeTranslations.at(
                                                          m_structure->m_numElems-1).at(1))+QString(" [m] "));
                painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 15));
                painter.drawText(10, 25, m_QFEMDock->m_Loading->getName());
                }
            }
        }
    }
}

void QFEMModule::initView() {
    if (m_firstView) {
		m_firstView = false;
		
		m_graph[0] = new NewGraph ("FemGraphOne", this, {NewGraph::QFEMSimulation, "", "", false, false});
	    m_graph[1] = new NewGraph ("FemGraphTwo", this, {NewGraph::QFEMSimulation, "", "", false, false});
	    m_graph[2] = new NewGraph ("FemGraphThree", this, {NewGraph::QFEMSimulation, "", "", false, false});
	    m_graph[3] = new NewGraph ("FemGraphFour", this, {NewGraph::QFEMSimulation, "", "", false, false});
		
        OnCenterScene();
        OnGLView();
		
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");
		setGraphArrangement(static_cast<TwoDWidgetInterface::GraphArrangement>
							(settings.value("modules/QFEMModule/graphArrangement", TwoDWidgetInterface::Quad).toInt()));
    }
    m_QFEMDock->InitStructureTable();
}

void QFEMModule::configureGL() {
	// set background
	glClearColor(g_mainFrame->m_BackgroundColor.redF(),
				 g_mainFrame->m_BackgroundColor.greenF(),
				 g_mainFrame->m_BackgroundColor.blueF(),
				 0.0);
	// enable depth test
	glEnable(GL_DEPTH_TEST);
	// accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// polygons are filled from both sides
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// polygons get a reduced Z-value. Now the grid is drawn onto the WindField surface
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 0);
	// line width
	glLineWidth(1);
	// disable smooth functions that otherwise make rendering worse
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_LINE_SMOOTH);
}

void QFEMModule::showAll() {
	g_bladeStructureStore.showAllCurves(true);
    m_QFEMDock->m_showCheckBox->setChecked(m_structure->isShownInGraph());
	reloadAllGraphCurves();
}

void QFEMModule::hideAll() {
	g_bladeStructureStore.showAllCurves(false, m_structure);
    m_QFEMDock->m_showCheckBox->setChecked(m_structure->isShownInGraph());
	reloadAllGraphCurves();
}

void QFEMModule::onActivationActionTriggered() {
	ModuleBase::onActivationActionTriggered();
	DualModule::showModule();

    if (GLView) OnGLView();
    else OnTwoDView();

    OnCenterScene();

	m_QFEMToolBar->show();
}

void QFEMModule::onModuleChanged()
{
	if (g_mainFrame->getCurrentModule() == this) {
		ModuleBase::onModuleChanged();
		DualModule::hideModule();
        m_QFEMDock->hide();
		m_QFEMToolBar->hide();
	}
}

void QFEMModule::addMainMenuEntries() {
	g_mainFrame->menuBar()->addMenu(g_mainFrame->BEMViewMenu);
	g_mainFrame->menuBar()->addMenu(m_graphMenu);
}

QStringList QFEMModule::prepareMissingObjectMessage() {
	return BladeStructure::prepareMissingObjectMessage();
}

void QFEMModule::OnSelChangeRotor()
{
		m_rotor = m_QFEMToolBar->m_rotorComboBox->currentObject();

        m_structure = NULL;
        m_QFEMDock->m_Loading = NULL;

        OnCenterScene();
}

void QFEMModule::CleanUp(){
    if (m_deformed_rotor) delete m_deformed_rotor;
    m_deformed_rotor = NULL;
}

void QFEMModule::OnSelChangeBladeStructure()
{
        m_structure = m_QFEMToolBar->m_BladeStructureComboBox->currentObject();

        if (m_structure)
        {
            if (m_structure->QFEMCompleted)
            {
                QFEMCompleted = true;
            }
            else QFEMCompleted = false;
        }
        m_QFEMDock->SetShownBladeStructure(m_structure);

        m_QFEMDock->CheckButtons();
}

void QFEMModule::OnSelChangeLoading()
{
    m_QFEMDock->m_Loading = m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject();

    DeformBlade();

    if (g_mainFrame->m_iApp == QFEMMODULE){

    if (g_mainFrame->m_iView == QFEMSTRUCTVIEW || !m_QFEMDock->m_Loading) m_QFEMDock->InitStructureTable(); // add data to the table and cause the rendering of the current selected rotor

    if (g_mainFrame->m_iView == QFEMLOADINGVIEW) m_QFEMDock->InitLoadingTable();

    }

    if (m_structure){
    m_structure->addQVecToResults(m_QFEMDock->m_Loading);
    m_structure->FillVariableList(m_QFEMDock->m_Loading);
    }

    m_QFEMDock->CheckButtons();
    reloadAllGraphCurves();
    m_twoDWidget->update();
}


void QFEMModule::ReadSectionData(int sel)
{
	if (sel >= m_QFEMDock->m_pStructModel->rowCount())
		return;
	
	double d;
	bool bOK;
	QString strong;
	QStandardItem *pItem;

    if (m_structure->AbsoluteShell)
    {
	pItem = m_QFEMDock->m_pStructModel->item(sel,0);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
    if(bOK) m_structure->ShellThickness[sel] = (d / g_mainFrame->m_mtoUnit) / m_structure->ChordLengths[sel];
    }
    else
    {
    pItem = m_QFEMDock->m_pStructModel->item(sel,0);
    strong = pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if (d>=1) d=1.0;
    if(bOK) m_structure->ShellThickness[sel] = d;
    }

    if (m_structure->AbsoluteSpar)
    {
    pItem = m_QFEMDock->m_pStructModel->item(sel,1);
    strong = pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_structure->SparThickness[sel] = (d / g_mainFrame->m_mtoUnit) / m_structure->ChordLengths[sel];
    }
    else
    {
    pItem = m_QFEMDock->m_pStructModel->item(sel,1);
    strong = pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if (d>=1) d=1.0;
    if(bOK) m_structure->SparThickness[sel] = d;
    }


	pItem = m_QFEMDock->m_pStructModel->item(sel,2);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_structure->SparLocation[sel] = d;

	pItem = m_QFEMDock->m_pStructModel->item(sel,3);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_structure->SparAngle[sel] = d;
}

void QFEMModule::ReadParams()
{
    if(!m_structure) return;

	for (int i=0; i< m_QFEMDock->m_pStructModel->rowCount();  i++) {
		ReadSectionData(i);
	}
	m_bResetglGeom = true;

}

void QFEMModule::OnStructTypeChanged()
{
	m_structure->StructType = m_QFEMDock->m_pctrlStructureCombobox->currentIndex();

    m_QFEMDock->OnResize();

    m_needToRerender = true;
    m_bResetglGeom = true;

	ComputeGeometry();
	reportGLChange();

}

void QFEMModule::OnCellChanged()
{
	ReadParams();
	ComputeGeometry();
	reportGLChange();
}



void QFEMModule::OnItemClicked(const QModelIndex &index)
{
		SetCurrentSection(index.row());

        m_needToRerender = true;
        m_newSectionHighlight = true;
		reportGLChange();
}

void QFEMModule::SetCurrentSection(int section)
{
		m_QFEMDock->m_iSection = section;
}

void QFEMModule::FillDataTable()
{
		if(!m_structure) return;
		int i;
		m_QFEMDock->m_pStructModel->setRowCount(m_structure->m_numElems);

		for(i=0; i<m_structure->m_numElems; i++)
		{
				FillTableRow(i);
		}


}

void QFEMModule::OnChangeCoordinates()
{
        if (!m_structure) return;
        if (m_QFEMDock->AbsoluteSpar->isChecked())
        {
            m_structure->AbsoluteSpar = true;
        }
        else
        {
            m_structure->AbsoluteSpar = false;
        }

        if (m_QFEMDock->AbsoluteShell->isChecked())
        {
            m_structure->AbsoluteShell=true;
        }
        else
        {
            m_structure->AbsoluteShell = false;
        }

        QString str;

        GetLengthUnit(str, g_mainFrame->m_LengthUnit);

        if (m_structure->AbsoluteShell) m_QFEMDock->m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(")+str+")");
        else m_QFEMDock->m_pStructModel->setHeaderData(0, Qt::Horizontal, tr("Shell Thickness(%)"));

        if (m_structure->AbsoluteSpar) m_QFEMDock->m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(")+str+")");
        else m_QFEMDock->m_pStructModel->setHeaderData(1, Qt::Horizontal, tr("Spar Thickness(%)"));

        FillDataTable();

}

void QFEMModule::FillTableRow(int row)
{
		QModelIndex ind;

        if (m_structure->AbsoluteShell)
        {
		ind = m_QFEMDock->m_pStructModel->index(row, 0, QModelIndex());
        m_QFEMDock->m_pStructModel->setData(ind, m_structure->ShellThickness[row] * g_mainFrame->m_mtoUnit * m_structure->ChordLengths[row]);
        }
        else
        {
        ind = m_QFEMDock->m_pStructModel->index(row, 0, QModelIndex());
        m_QFEMDock->m_pStructModel->setData(ind, m_structure->ShellThickness[row]);
        }

        if (m_structure->AbsoluteSpar)
        {
		ind = m_QFEMDock->m_pStructModel->index(row, 1, QModelIndex());
        m_QFEMDock->m_pStructModel->setData(ind, m_structure->SparThickness[row] * g_mainFrame->m_mtoUnit * m_structure->ChordLengths[row]);
        }
        else
        {
        ind = m_QFEMDock->m_pStructModel->index(row, 1, QModelIndex());
        m_QFEMDock->m_pStructModel->setData(ind, m_structure->SparThickness[row]);
        }

		ind = m_QFEMDock->m_pStructModel->index(row, 2, QModelIndex());
		m_QFEMDock->m_pStructModel->setData(ind, m_structure->SparLocation[row]);

		ind = m_QFEMDock->m_pStructModel->index(row, 3, QModelIndex());
		m_QFEMDock->m_pStructModel->setData(ind, m_structure->SparAngle[row]);

}

void QFEMModule::DisableAll()
{
	m_QFEMToolBar->setEnabled(false);
	m_QFEMToolBar->setEnabled(false);

	g_mainFrame->m_pctrlMainToolBar->setEnabled(false);

}

void QFEMModule::EnableAll()
{
	m_QFEMToolBar->setEnabled(true);
	m_QFEMToolBar->setEnabled(true);

	g_mainFrame->m_pctrlMainToolBar->setEnabled(true);

}


void QFEMModule::OnSelChangeInnerMaterial(int i)
{
	if (i==0)
	{
		m_QFEMDock->m_pctrlIntELineEdit->setEnabled(true);
		m_QFEMDock->m_pctrlIntRhoLineEdit->setEnabled(true);
	}
	else
	{
		m_QFEMDock->m_pctrlIntELineEdit->setEnabled(false);
		m_QFEMDock->m_pctrlIntRhoLineEdit->setEnabled(false);
	}

	m_QFEMDock->m_pctrlIntELineEdit->setValue(m_QFEMDock->EModList.at(i));
	m_QFEMDock->m_pctrlIntRhoLineEdit->setValue(m_QFEMDock->RhoList.at(i));
}

void QFEMModule::OnSelChangeShellMaterial(int i)
{
	if (i==0)
	{
		m_QFEMDock->m_pctrlShellELineEdit->setEnabled(true);
		m_QFEMDock->m_pctrlShellRhoLineEdit->setEnabled(true);
	}
	else
	{
		m_QFEMDock->m_pctrlShellELineEdit->setEnabled(false);
		m_QFEMDock->m_pctrlShellRhoLineEdit->setEnabled(false);
	}

	m_QFEMDock->m_pctrlShellELineEdit->setValue(m_QFEMDock->EModList.at(i));
	m_QFEMDock->m_pctrlShellRhoLineEdit->setValue(m_QFEMDock->RhoList.at(i));
}

void QFEMModule::ComputeGeometry()
{
		// Computes the blades's characteristics from the panel data
		if(m_rotor)
		{
		m_needToRerender = true;
		m_rotor->CreateSurfaces();
		m_rotor->ComputeGeometry();
        }
}

void QFEMModule::OnCenterScene()
{
	if(!m_rotor) return;
	if (g_mainFrame->getCurrentModule() != this) return;
	
	if (m_QFEMDock->m_pctrlRotor->isChecked())
	{
		m_glWidget->setSceneRadius(float(m_rotor->getRotorRadius()));
        m_glWidget->setSceneCenter(qglviewer::Vec(0,0,0));
	}
	else
	{
		m_glWidget->setSceneRadius(float(m_rotor->getRotorRadius()/2.0));
        m_glWidget->setSceneCenter(qglviewer::Vec(0,m_rotor->getRotorRadius()/2.0,0));
	}
    m_glWidget->showEntireScene();
	m_glWidget->updateGL();
}

void QFEMModule::render() {
    if (TwoDView || m_structure == NULL || (g_mainFrame->m_iView == QFEMLOADINGVIEW && m_QFEMDock->m_Loading == NULL))
		return;
    if (m_structure)
    {
        if (m_structure->QFEMCompleted && m_needToRerender)
        { 
            GLCreateGeom(m_deformed_rotor);
        }
        else if (m_needToRerender)
        {
            GLCreateGeom(m_rotor);
        }
    }
    else if (m_needToRerender)
    {
        GLCreateGeom(m_rotor);
    }

    m_glWidget->GLSetupLight(m_GLLightDlg, 0, 1.0);
    if (m_QFEMDock->m_LoadingShown && m_QFEMDock->m_Loading && m_structure && !ObjectIsEdited){
        glCallList(VMSTRESSES);
        GLRenderStressLegend();
    }
    if (m_structure && m_QFEMDock->m_pctrlInternal->isChecked()) glCallList(INNERGEOM);
    if (m_bStructEdited) glCallList(SECTIONHIGHLIGHT);
    if (m_QFEMDock->m_pctrlTopSurface->isChecked()) glCallList(TOPSURFACES);
    if (m_QFEMDock->m_pctrlSurfaces->isChecked()) glCallList(BOTTOMSURFACES);
    if (m_QFEMDock->m_pctrlOutline->isChecked()){
        if (m_structure && m_QFEMDock->m_pctrlInternal->isChecked()) glCallList(WING2OUTLINE);
        glCallList(WINGOUTLINE);
    }

    if (m_QFEMDock->m_pctrlRotor->isChecked())
    {
        for (int i=1;i<m_rotor->m_blades;i++)
        {
            glRotated(360.0/double(m_rotor->m_blades),0,0,1);
            if (m_structure && m_QFEMDock->m_pctrlInternal->isChecked()) glCallList(INNERGEOM);
            if (m_bStructEdited) glCallList(SECTIONHIGHLIGHT);
            if (m_QFEMDock->m_pctrlTopSurface->isChecked()) glCallList(TOPSURFACES);
            if (m_QFEMDock->m_pctrlSurfaces->isChecked()) glCallList(BOTTOMSURFACES);
            if (m_QFEMDock->m_LoadingShown && m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()) glCallList(VMSTRESSES);
            if (m_QFEMDock->m_pctrlOutline->isChecked()){
                if (m_structure && m_QFEMDock->m_pctrlInternal->isChecked()) glCallList(WING2OUTLINE);
                glCallList(WINGOUTLINE);
            }
        }
    }
}

void QFEMModule::GLRenderStressLegend(){
}


void QFEMModule::GLCreateGeom(CBlade *pWing)
{
	QVector< QVector <double> > TempInner, TempSpar;
	QVector< QVector < QVector <double> > > AllInner, AllSpar;
	
	QVector<double> inner;
	
	if(!pWing) return;
	pWing->CreateSurfaces();
	
	static int j, l, style;
	static CVector Pt, PtNormal, A, B, C, D, N, BD, AC;
	static QColor color;
	QColor outlineColor = QColor(0,0,0);
	int outlinewidth = 1;
	
	static CFoil * pFoilA, *pFoilB;
	const int SIDEPOINTS = 51;
	
	static double x, xDistrib[SIDEPOINTS];
	double xx;
	double param = 50;// increase to refine L.E. and T.E.
	for(int i=0; i<SIDEPOINTS; i++)
	{
		xx = (double)i/(double)(SIDEPOINTS-1);
		xDistrib[i] = (asinh(param*(xx-0.5))/asinh(param/2.)+1.)/2.;
	}
	
	N.Set(0.0, 0.0, 0.0);
	
	
	
	if (g_mainFrame->m_iView == QFEMLOADINGVIEW && m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject() && m_structure)
	{
		glNewList(VMSTRESSES,GL_COMPILE);
		{
			
			
			double maxmax = 0;
			double minmin = 10e20;
			hsv hs;
			for (int i=0;i<=pWing->m_NSurfaces;i++)
			{
				for (int j=0;j<m_structure->m_numFoilPoints-2;j++)
				{
					if (m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(i).at(j)>maxmax) maxmax = m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(i).at(j);
					if (m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(i).at(j)<minmin) minmin = m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(i).at(j);
				}
			}			
			
			glLineWidth(1.0);
			color = pWing->m_WingColor;
			style = 0;
			
			glColor4d(color.redF(),color.greenF(),color.blueF(),color.alphaF());
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0, 0.0);
			glEnable(GL_DEPTH_TEST);
			//                bottom surface
			
			
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for (l=0; l<m_structure->m_numFoilPoints-2; l++)
					{
						hs.h = (1-m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(j).at(l)/maxmax)*225;
//						double kkk = hs.h/360;
						hs.s = 1.0;
						hs.v = 1.0;
						
						glColor3d( hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
//						glColor3d(  kkk, 0, 1-kkk);
						
						CVector xy;
						xy.x = (m_structure->xFoilCoords[j][l]*m_structure->ChordLengths.at(j)-pWing->m_TFoilPAxisX[j]*pWing->m_TChord[j]);
						xy.y = (-m_structure->yFoilCoords[j][l]*m_structure->ChordLengths.at(j)-pWing->m_TFoilPAxisZ[j]*pWing->m_TChord[j]);
						
						xy.RotateZ(CVector(0,0,0),-pWing->m_TTwist[j]);
						
						
                        glVertex3d(xy.x+pWing->m_TOffsetX[j], pWing->m_TPos[j], xy.y+pWing->m_TOffsetZ[j]);
						
						hs.h = (1-m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(j+1).at(l)/maxmax)*225;
//						kkk = hs.h/360;
						
						glColor3d( hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
//						glColor3d(  kkk, 0, 1-kkk);
						
						xy.x = (m_structure->xFoilCoords[j+1][l]*m_structure->ChordLengths.at(j+1)-pWing->m_TFoilPAxisX[j+1]*pWing->m_TChord[j+1]);
						xy.y = (-m_structure->yFoilCoords[j+1][l]*m_structure->ChordLengths.at(j+1)-pWing->m_TFoilPAxisZ[j+1]*pWing->m_TChord[j+1]);
						
						xy.RotateZ(CVector(0,0,0),-pWing->m_TTwist[j+1]);
						
                        glVertex3d(xy.x+pWing->m_TOffsetX[j+1], pWing->m_TPos[j+1], xy.y+pWing->m_TOffsetZ[j+1]);
					}
					
					hs.h = (1-m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(j).at(0)/maxmax)*225;
//					double kkk = hs.h/360;
					hs.s = 1.0;
					hs.v = 1.0;
					
					glColor3d( hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
//					glColor3d(  kkk, 0, 1-kkk);
					
					CVector xy;
					xy.x = (m_structure->xFoilCoords[j][0]*m_structure->ChordLengths.at(j)-pWing->m_TFoilPAxisX[j+1]*pWing->m_TChord[j]);
					xy.y = (-m_structure->yFoilCoords[j][0]*m_structure->ChordLengths.at(j)-pWing->m_TFoilPAxisZ[j]*pWing->m_TChord[j]);
					
					xy.RotateZ(CVector(0,0,0),-pWing->m_TTwist[j]);
					
					
                    glVertex3d(xy.x+pWing->m_TOffsetX[j], pWing->m_TPos[j], xy.y+pWing->m_TOffsetZ[j]);
					
					hs.h = (1-m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->VMStresses.at(j+1).at(0)/maxmax)*225;
//					kkk = hs.h/360;
					
					glColor3d( hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b);
//					glColor3d(  kkk, 0, 1-kkk);
					
					xy.x = (m_structure->xFoilCoords[j+1][0]*m_structure->ChordLengths.at(j+1)-pWing->m_TFoilPAxisX[j+1]*pWing->m_TChord[j+1]);
					xy.y = (-m_structure->yFoilCoords[j+1][0]*m_structure->ChordLengths.at(j+1)-pWing->m_TFoilPAxisZ[j+1]*pWing->m_TChord[j+1]);
					
					xy.RotateZ(CVector(0,0,0),-pWing->m_TTwist[j+1]);
					
                    glVertex3d(xy.x+pWing->m_TOffsetX[j+1], pWing->m_TPos[j+1], xy.y+pWing->m_TOffsetZ[j+1]);
				}
				glEnd();
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
	}
	
	glPolygonOffset(1.0, 1.0);
	
	
	if (m_bResetglGeom)
	{
		glNewList(BOTTOMSURFACES,GL_COMPILE);
		{
			glLineWidth(1.0);
			color = pWing->m_WingColor;
			style = 0;
			
			glColor4d(color.redF(),color.greenF(),color.blueF(),color.alphaF());
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glEnable(GL_DEPTH_TEST);
			//                bottom surface
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						
						pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
						glNormal3d(-PtNormal.x, -PtNormal.y, -PtNormal.z);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable (GL_LINE_STIPPLE);
			
		}
		glEndList();
		glNewList(TOPSURFACES,GL_COMPILE);
		{
			glLineWidth(1.0);
			color = pWing->m_WingColor;
			style = 0;
			
			glColor4d(color.redF(),color.greenF(),color.blueF(),color.alphaF());
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glEnable(GL_DEPTH_TEST);
			
			//                            top surface
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
						glNormal3d(PtNormal.x, PtNormal.y, PtNormal.z);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
			}
			
//left tip surface


//                            glBegin(GL_QUAD_STRIP);
//                            {
//                                    pWing->m_Surface[0].GetPanel(0, 0, BOTSURFACE);
//                                    C. Copy(pWing->m_Surface[0].LA);
//                                    D. Copy(pWing->m_Surface[0].TA);
//                                    pWing->m_Surface[0].GetPanel(0, 0, TOPSURFACE);
//                                    A. Copy(pWing->m_Surface[0].TA);
//                                    B. Copy(pWing->m_Surface[0].LA);

//                                    BD = D-B;
//                                    AC = C-A;
//                                    N  = AC*BD;
//                                    N.Normalize();
//                                    glNormal3d( N.x, N.y, N.z);

//                                    for (l=0; l<SIDEPOINTS; l++)
//                                    {
//                                            x = xDistrib[l];
//                                            pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,1);

//                                            glVertex3d(Pt.x, Pt.y, Pt.z);

//                                            pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,-1);
//                                            glVertex3d(Pt.x, Pt.y, Pt.z);
//                                    }
//                            }
//                            glEnd();

//						 right tip surface

			glBegin(GL_QUAD_STRIP);
			{
				pWing->m_Surface[pWing->m_NSurfaces-1].GetPanel(pWing->m_Surface[pWing->m_NSurfaces-1].m_NYPanels-1,0, TOPSURFACE);
				A. Copy(pWing->m_Surface[0].TB);
				B. Copy(pWing->m_Surface[0].LB);
				pWing->m_Surface[pWing->m_NSurfaces-1].GetPanel(pWing->m_Surface[pWing->m_NSurfaces-1].m_NYPanels-1,0, BOTSURFACE);
				C. Copy(pWing->m_Surface[0].LB);
				D. Copy(pWing->m_Surface[0].TB);
				
				BD = D-B;
				AC = C-A;
				N  = BD * AC;
				N.Normalize();
				glNormal3d( N.x,  N.y,  N.z);
				
				for (l=0; l<SIDEPOINTS; l++)
				{
					x = xDistrib[l];
					pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,1);
					
					glVertex3d(Pt.x, Pt.y, Pt.z);
					pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,-1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
			}
			glEnd();
			
			
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
	}
	//OUTLINE
	if (m_QFEMDock->m_pctrlOutline->isChecked())
	{
		glNewList(WINGOUTLINE,GL_COMPILE);
		{
			pWing->CreateSurfaces();
			
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glPolygonOffset(-8.0, -8.0);
			glEnable (GL_LINE_STIPPLE);
			glLineWidth((GLfloat)2);
			
			color = outlineColor;
			style = 0; /*m_OutlineStyle;*/
			
			if     (style == 1) 	glLineStipple (1, 0x1111);
			else if(style == 2) 	glLineStipple (1, 0x0F0F);
			else if(style == 3) 	glLineStipple (1, 0x1C47);
			else					glLineStipple (1, 0xFFFF);
			
			glColor3d(color.redF(),color.greenF(),color.blueF());
			glLineWidth((GLfloat)outlinewidth);
			
			//TOP outline
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
				
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
			}
			//BOTTOM outline
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
				
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<=100; l++)
					{
						x = (double)l/100.0;
						pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
				}
				glEnd();
				
			}
			
			
			//WingContour
			//Leading edge outline
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_LINES);
				{
					pWing->m_Surface[j].GetPanel(0,pWing->m_Surface[j].m_NXPanels-1, MIDSURFACE);
					glVertex3d(pWing->m_Surface[j].LA.x,
							   pWing->m_Surface[j].LA.y,
							   pWing->m_Surface[j].LA.z);
					pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,pWing->m_Surface[j].m_NXPanels-1, MIDSURFACE);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
				}
				glEnd();
			}
			//Trailing edge outline
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				glBegin(GL_LINES);
				{
					pWing->m_Surface[j].GetPanel(0,0, MIDSURFACE);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
					pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1, 0, MIDSURFACE);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
			//flap outline....
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				pFoilA = pWing->m_Surface[j].m_pFoilA;
				pFoilB = pWing->m_Surface[j].m_pFoilB;
				if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap)
				{
					glBegin(GL_LINES);
					{
						if(pFoilA->m_bTEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
														 0.0, Pt, 1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						
						if(pFoilB->m_bTEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
														 1.0, Pt, 1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						if(pFoilA->m_bTEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
														 0.0, Pt, -1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						
						if(pFoilB->m_bTEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
														 1.0, Pt, -1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
					glEnd();
				}
			}
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				pFoilA = pWing->m_Surface[j].m_pFoilA;
				pFoilB = pWing->m_Surface[j].m_pFoilB;
				if(pFoilA && pFoilB && pFoilA->m_bLEFlap && pFoilB->m_bLEFlap)
				{
					glBegin(GL_LINES);
					{
						if(pFoilA->m_bLEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
														 0.0, Pt, 1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						
						if(pFoilB->m_bLEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
														 1.0, Pt, 1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						if(pFoilA->m_bLEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
														 0.0, Pt, -1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
						
						if(pFoilB->m_bLEFlap)
							pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
														 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
														 1.0, Pt, -1);
						else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
						glVertex3d(Pt.x, Pt.y, Pt.z);
					}
					glEnd();
				}
			}
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
	}
	
	if (m_newSectionHighlight)
	{
		glNewList(SECTIONHIGHLIGHT,GL_COMPILE);
		{
			glPolygonMode(GL_FRONT,GL_LINE);
			glPolygonOffset(-3.0, -3.0);
			glDisable (GL_LINE_STIPPLE);
			glColor3d(1.0, 0.0, 0.0);
			glLineWidth(3);
            if (m_QFEMDock->m_iSection>=0){
                if(m_QFEMDock->m_iSection<pWing->m_NPanel)
                {
                    j = m_QFEMDock->m_iSection;
                    glBegin(GL_LINE_STRIP);
                    {
                        for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
                        {
                            pWing->m_Surface[j].GetPanel(0, l, TOPSURFACE);
                            glVertex3d(pWing->m_Surface[j].TA.x,
                                       pWing->m_Surface[j].TA.y,
                                       pWing->m_Surface[j].TA.z);
                        }

                        glVertex3d(pWing->m_Surface[j].LA.x,
                                   pWing->m_Surface[j].LA.y,
                                   pWing->m_Surface[j].LA.z);

                        for (l=pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
                        {
                            pWing->m_Surface[j].GetPanel(0, l, BOTSURFACE);
                            glVertex3d(pWing->m_Surface[j].TA.x,
                                       pWing->m_Surface[j].TA.y,
                                       pWing->m_Surface[j].TA.z);
                        }
                    }
                    glEnd();
                }
                else
                {
                    j = m_QFEMDock->m_iSection-1;
                    glBegin(GL_LINE_STRIP);
                    {
                        for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
                        {
                            pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, TOPSURFACE);
                            glVertex3d(pWing->m_Surface[j].TB.x,
                                       pWing->m_Surface[j].TB.y,
                                       pWing->m_Surface[j].TB.z);
                        }

                        glVertex3d(pWing->m_Surface[j].LB.x,
                                   pWing->m_Surface[j].LB.y,
                                   pWing->m_Surface[j].LB.z);

                        for (l=pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
                        {
                            pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, BOTSURFACE);
                            glVertex3d(pWing->m_Surface[j].TB.x,
                                       pWing->m_Surface[j].TB.y,
                                       pWing->m_Surface[j].TB.z);
                        }
                    }
                    glEnd();
                }
        }
		}
		glEndList();
	}
	
	if (m_structure && m_bResetglGeom && m_QFEMDock->m_pctrlInternal->isChecked())
	{
		// create tessellator
		GLUtesselator *tess = gluNewTess();
		
		// register callback functions
		gluTessCallback(tess, GLU_TESS_BEGIN, (void (__stdcall*)(void))tessBeginCB);
		gluTessCallback(tess, GLU_TESS_END, (void (__stdcall*)(void))tessEndCB);
		gluTessCallback(tess, GLU_TESS_ERROR, (void (__stdcall*)(void))tessErrorCB);
		gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)(void))tessVertexCB);
		gluTessCallback(tess, GLU_TESS_COMBINE, (void (__stdcall*)(void))tessCombineCB);
		
		glNewList(INNERGEOM,GL_COMPILE);
		{
			for (int k = 0;k<m_structure->m_numElems-1;k++)
			{
				bool klappt = false;
				glEnable (GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPolygonOffset(5.0, 5.0);
				
				
				int scale = 10e3;
				Polygons OuterProfiles(1);
				Polygons InnerProfiles(1);
				Polygons SparPoly(1),InnerPoly(1), solution(1);
				///// create inner and outer polygons
				
				for (l=0; l<SIDEPOINTS; l++)
				{
					x = xDistrib[l];
					pWing->m_Surface[k].GetPoint(x,x,1.0,Pt, PtNormal,1);
					
					double scaledX = Pt.x*scale;
					double scaledZ = Pt.z*scale;
					
					OuterProfiles[0].push_back(IntPoint((int) scaledX,(int) scaledZ));
					
				}
				for (l=SIDEPOINTS-1; l>=0; l--)
				{
					x = xDistrib[l];
					pWing->m_Surface[k].GetPoint(x,x,1.0,Pt, PtNormal,-1);
					
					double scaledX = Pt.x*scale;
					double scaledZ = Pt.z*scale;
					
					OuterProfiles[0].push_back(IntPoint((int) scaledX,(int) scaledZ));
					
				}
				if (m_structure->StructType < 2) OffsetPolygons(OuterProfiles,InnerProfiles,-m_structure->ShellThickness[k+1] * m_structure->ChordLengths[k+1]*scale,jtRound,0.1,true);
				
				bool hasSpar = false;
				
				if (m_structure->StructType == 0)
				{
					
					hasSpar = true;
					////////// the spar struture is created here
					vector<double> SparXCoords,SparYCoords;
					//Adjust Values for chordlength.
					double Position = (m_structure->SparLocation[k+1]-pWing->m_TFoilPAxisX[k+1])*m_structure->ChordLengths[k+1]+pWing->m_TPAxisX[k+1];
					
					double Thickness  =m_structure->SparThickness[k+1]*m_structure->ChordLengths[k+1];
					
					if (Thickness < 0.00001) hasSpar = false;
					
					if (hasSpar)
					{
						
						//Create the spar coords declaring from the bottom left corner then
						//moving anti-clockwise. The spar at this stage is unclipped by the
						// foil and set about 0, 0
						//X Corners
						SparXCoords.push_back(-Thickness/2.0);
						SparXCoords.push_back(Thickness/2.0);
						SparXCoords.push_back(Thickness/2.0);
						SparXCoords.push_back(-Thickness/2.0);
						
						//Y Corners
						//Defined based on the assumption that the x Length will be the
						//Longest, as such the spar length is set that large so that
						//if 90 Degree angle is entered the spar is long enough.
						double xMax = 2*m_rotor->m_TChord[k+1];
						
						SparYCoords.push_back(-xMax*1.1);
						SparYCoords.push_back(-xMax*1.1);
						SparYCoords.push_back(xMax*1.1);
						SparYCoords.push_back(xMax*1.1);
						//-------------------------------------------------------
						//Affine Rotation about the coord position and the x axis
						Eigen::Rotation2D<double> Rotation(((m_structure->SparAngle[k+1])*2*PI)/360.0+pWing->m_TTwist[k+1]*2*PI/360);
						Vector2d Point,Rotated;
						//Reload rotated coordinates.
						for(int i = 0; i < (int)SparXCoords.size();i++)
						{
							Point(0)=SparXCoords.at(i);
							Point(1)=SparYCoords.at(i);
							Rotated = Rotation*Point;
							SparXCoords.at(i)=Rotated(0)+Position;//Centre about the chordwise position.
                            SparYCoords.at(i)=Rotated(1)-pWing->m_TFoilPAxisZ[k+1]*pWing->m_TChord[k+1]+pWing->m_TOffsetZ[k];
						}
						//-------------------------------------------------------
						//Load up the polygons with the coords
						//Scaling factor used to recast doubles to ints.
						//This is nessesary for the clipper library.
						
						//Create Polygon of the Spar
						for(int i=SparXCoords.size()-1; i>=0;i--)
						{
							IntPoint NewPoint;
							double tempX = SparXCoords.at(i)*scale;
							double tempY = SparYCoords.at(i)*scale;
							NewPoint.X =(int) tempX;
							NewPoint.Y =(int) tempY;
							SparPoly.at(0).push_back(NewPoint);
						}
						//Create scaled polygon of inner coordinates
						for (l = 0; l < (int)InnerProfiles[0].size(); l++)
						{
							IntPoint NewPoint;
							double tempX = InnerProfiles[0][l].X;
							double tempY = InnerProfiles[0][l].Y;
							NewPoint.X =(int) tempX;
							NewPoint.Y =(int) tempY;
							InnerPoly.at(0).push_back(NewPoint);
						}
						//------------------------------------------------------
						//create a intersection binary operation meaning take the
						//area that is common to both polygons.
						Clipper c;
						c.AddPolygons(SparPoly,ptSubject);
						c.AddPolygons(InnerPoly,ptClip);
						c.Execute(ctIntersection,solution,pftNonZero,pftNonZero);
						
						if (solution.size()) klappt = true;
					}
				}
				glColor3d(shellColor.redF(), shellColor.greenF(),shellColor.blueF());
				
				//// arrays outerpoints and innerpoints are needed to store voxels for the tesselation
				
				GLdouble outerpoints[(int)OuterProfiles[0].size()][3];
				GLdouble innerpoints[(int)InnerProfiles[0].size()][3];
				gluTessBeginPolygon(tess, 0);                   // with NULL data
				
				gluTessBeginContour(tess);
				{
					
					for (l = 0; l < (int)OuterProfiles[0].size(); l++)
					{
						double yy = pWing->m_TPos[k+1];
						IntPoint Point = OuterProfiles[0][l];
						x = double(Point.X) / scale;
						double z = double(Point.Y) / scale;
						
						outerpoints[l][0] = x;
						outerpoints[l][1] = yy;
						outerpoints[l][2] = z;
						gluTessVertex(tess, outerpoints[l], outerpoints[l]);
					}
					
				}
				gluTessEndContour(tess);
				
				if (InnerProfiles[0].size() > 4 && m_structure->StructType < 2) //if shell too thick no need to make hollow
				{
					gluTessBeginContour(tess);
					for (l = 0; l < (int)InnerProfiles[0].size(); l++)
					{
						double yy = pWing->m_TPos[k+1];
						IntPoint Point = InnerProfiles[0][l];
						x = double(Point.X) / scale;
						double z = double(Point.Y) / scale;
						
						innerpoints[l][0] = x;
						innerpoints[l][1] = yy;
						innerpoints[l][2] = z;
						gluTessVertex(tess, innerpoints[l], innerpoints[l]);
						
					}
					gluTessEndContour(tess);
				}
				gluTessEndPolygon(tess);
				vertexIndex = 0; // needed for combineCB from tesselation
				
				// the coordinates of the inner outline are stored for visualization purposes
				if (InnerProfiles[0].size() > 4 && m_structure->StructType < 2) //if shell too thick no need to make hollow
				{
					TempInner.clear();
					for (l = 0; l < (int)InnerProfiles[0].size(); l++)
					{
						inner.clear();
						inner.append(double(InnerProfiles.at(0).at(l).X)/scale);
						inner.append(pWing->m_TPos[k+1]);
						inner.append(double(InnerProfiles.at(0).at(l).Y)/scale);
						TempInner.append(inner);
					}
					AllInner.append(TempInner);
				}
				
				glColor3d(sparColor.redF(), sparColor.greenF(),sparColor.blueF());
				
				if (klappt && InnerProfiles[0].size() > 4 && m_structure->StructType == 0 && hasSpar)
				{
					glBegin(GL_POLYGON);
					{
						for (l = 0; l < (int)solution[0].size(); l++)
						{
							
							glVertex3d(double(solution.at(0).at(l).X)/double(scale), pWing->m_TPos[k+1], double(solution.at(0).at(l).Y)/double(scale));
							
						}
						glVertex3d(double(solution.at(0).at(0).X)/double(scale), pWing->m_TPos[k+1], double(solution.at(0).at(0).Y)/double(scale));
						
					}
					glEnd();
				}
				
				// the coordinates of the inner outline are stored for visualization purposes
				if (klappt && InnerProfiles[0].size() > 4 && m_structure->StructType == 0 && hasSpar)
				{
					TempSpar.clear();
					for (l = 0; l < (int)solution[0].size(); l++)
					{
						inner.clear();
						inner.append(double(solution.at(0).at(l).X)/double(scale));
						inner.append(pWing->m_TPos[k+1]);
						inner.append(double(solution.at(0).at(l).Y)/double(scale));
						TempSpar.append(inner);
					}
					AllSpar.append(TempSpar);
				}
				glColor3d(shellColor.redF(), shellColor.greenF(),shellColor.blueF());
				
				if (k==0)
				{
					OuterProfiles[0].clear();
					InnerProfiles[0].clear();
					
					for (l=0; l<SIDEPOINTS; l++)
					{
						x = xDistrib[l];
						pWing->m_Surface[k].GetPoint(x,x,0,Pt, PtNormal,1);
						
						double scaledX = Pt.x*scale;
						double scaledZ = Pt.z*scale;
						
						OuterProfiles[0].push_back(IntPoint((int) scaledX,(int) scaledZ));
						
					}
					
					
					for (l=SIDEPOINTS-1; l>=0; l--)
					{
						x = xDistrib[l];
						pWing->m_Surface[k].GetPoint(x,x,0,Pt, PtNormal,-1);
						
						double scaledX = Pt.x*scale;
						double scaledZ = Pt.z*scale;
						
						OuterProfiles[0].push_back(IntPoint((int) scaledX,(int) scaledZ));
						
					}
					
					if (m_structure->StructType < 2) OffsetPolygons(OuterProfiles,InnerProfiles,-m_structure->ShellThickness[k] * pWing->m_TChord[k]*scale,jtRound,0.1,true);
					
					hasSpar = false;
					if (m_structure->StructType == 0)
					{
						hasSpar = true;
						InnerPoly[0].clear();
						SparPoly[0].clear();
						solution[0].clear();
						
						////////// the spar struture is created here
						vector<double> SparXCoords,SparYCoords;
						//Adjust Values for chordlength.
						double Position = (m_structure->SparLocation[k]-pWing->m_TFoilPAxisX[k])*m_structure->ChordLengths[k]+pWing->m_TPAxisX[k];
						double Thickness  =m_structure->SparThickness[k]*m_structure->ChordLengths[k];
						
						if (Thickness < 0.00001) hasSpar = false;
						
						if (hasSpar)
						{
							//Create the spar coords declaring from the bottom left corner then
							//moving anti-clockwise. The spar at this stage is unclipped by the
							// foil and set about 0, 0
							//X Corners
							SparXCoords.push_back(-Thickness/2.0);
							SparXCoords.push_back(Thickness/2.0);
							SparXCoords.push_back(Thickness/2.0);
							SparXCoords.push_back(-Thickness/2.0);
							
							//Y Corners
							//Defined based on the assumption that the x Length will be the
							//Longest, as such the spar length is set that large so that
							//if 90 Degree angle is entered the spar is long enough.
							
							double xMax = 2*m_structure->ChordLengths[k];
							
							SparYCoords.push_back(-xMax*1.1);
							SparYCoords.push_back(-xMax*1.1);
							SparYCoords.push_back(xMax*1.1);
							SparYCoords.push_back(xMax*1.1);
							//-------------------------------------------------------
							//Affine Rotation about the coord position and the x axis
							Eigen::Rotation2D<double> Rotation(((m_structure->SparAngle[k])*2*PI)/360.0+pWing->m_TTwist[k]*2*PI/360);
							Vector2d Point,Rotated;
							//Reload rotated coordinates.
							for(int i = 0; i < (int)SparXCoords.size();i++)
							{
								Point(0)=SparXCoords.at(i);
								Point(1)=SparYCoords.at(i);
								Rotated = Rotation*Point;
								SparXCoords.at(i)=Rotated(0)+Position;//Centre about the chordwise position.
                                SparYCoords.at(i)=Rotated(1)-pWing->m_TFoilPAxisZ[k]*pWing->m_TChord[k]+pWing->m_TOffsetZ[k];
								
							}
							//-------------------------------------------------------
							//Load up the polygons with the coords
							//Scaling factor used to recast doubles to ints.
							//This is nessesary for the clipper library.
							
							//Create Polygon of the Spar
							for(int i=SparXCoords.size()-1; i>=0;i--)
							{
								IntPoint NewPoint;
								double tempX = SparXCoords.at(i)*scale;
								double tempY = SparYCoords.at(i)*scale;
								NewPoint.X =(int) tempX;
								NewPoint.Y =(int) tempY;
								SparPoly.at(0).push_back(NewPoint);
								
							}
							//Create scaled polygon of inner coordinates
							for (l = 0; l < (int)InnerProfiles[0].size(); l++)
							{
								IntPoint NewPoint;
								double tempX = InnerProfiles[0][l].X;
								double tempY = InnerProfiles[0][l].Y;
								NewPoint.X =(int) tempX;
								NewPoint.Y =(int) tempY;
								InnerPoly.at(0).push_back(NewPoint);
								
							}
							
							//------------------------------------------------------
							//create a intersection binary operation meaning take the
							//area that is common to both polygons.
							Clipper c;
							c.AddPolygons(SparPoly,ptSubject);
							c.AddPolygons(InnerPoly,ptClip);
							c.Execute(ctIntersection,solution,pftNonZero,pftNonZero);
							
							if (solution.size()) klappt = true;
							
						}
					}
					
					// the coordinates of the inner spar outline are stored for visualization purposes
					if (InnerProfiles[0].size() > 4 && m_structure->StructType < 2) //if shell too thick no need to make hollow
					{
						TempInner.clear();
						for (l = 0; l < (int)InnerProfiles[0].size(); l++)
						{
							inner.clear();
							inner.append(double(InnerProfiles.at(0).at(l).X)/scale);
							inner.append(pWing->m_TPos[k]);
							inner.append(double(InnerProfiles.at(0).at(l).Y)/scale);
							TempInner.append(inner);
							
						}
						AllInner.append(TempInner);
					}
					
					
					// the coordinates of the inner spar outline are stored for visualization purposes
					if (klappt && InnerProfiles[0].size() > 4 && m_structure->StructType == 0 && hasSpar)
					{
						TempSpar.clear();
						for (l = 0; l < (int)solution[0].size(); l++)
						{
							inner.clear();
							inner.append(double(solution.at(0).at(l).X)/double(scale));
							inner.append(pWing->m_TPos[k]);
							inner.append(double(solution.at(0).at(l).Y)/double(scale));
							TempSpar.append(inner);
						}
						AllSpar.append(TempSpar);
					}
					
					
					GLdouble outerpoints[(int)OuterProfiles[0].size()][3];
					GLdouble innerpoints[(int)InnerProfiles[0].size()][3];
					gluTessBeginPolygon(tess, 0);                   // with NULL data
					
					gluTessBeginContour(tess);
					{
						
						for (l = 0; l < (int)OuterProfiles[0].size(); l++)
						{
							double yy = pWing->m_TPos[k];
							IntPoint Point = OuterProfiles[0][l];
							x = double(Point.X) / scale;
							double z = double(Point.Y) / scale;
							
							outerpoints[l][0] = x;
							outerpoints[l][1] = yy;
							outerpoints[l][2] = z;
							gluTessVertex(tess, outerpoints[l], outerpoints[l]);
						}
						
					}
					gluTessEndContour(tess);
					
					if (InnerProfiles[0].size() > 4 && m_structure->StructType < 2) //if shell too thick no need to make hollow
					{
						gluTessBeginContour(tess);
						for (l = 0; l < (int)InnerProfiles[0].size(); l++)
						{
							double yy = pWing->m_TPos[k];
							IntPoint Point = InnerProfiles[0][l];
							x = double(Point.X) / scale;
							double z = double(Point.Y) / scale;
							
							innerpoints[l][0] = x;
							innerpoints[l][1] = yy;
							innerpoints[l][2] = z;
							gluTessVertex(tess, innerpoints[l], innerpoints[l]);
							
						}
						gluTessEndContour(tess);
					}
					gluTessEndPolygon(tess);
					vertexIndex = 0; // needed for combineCB from tesselation
					
					
					
					glColor3d(sparColor.redF(), sparColor.greenF(),sparColor.blueF());
					
					if (klappt && InnerProfiles[0].size() > 4 && m_structure->StructType == 0 && hasSpar)
					{
						glBegin(GL_POLYGON);
						{
							for (l = 0; l < (int)solution[0].size(); l++)
							{
								glVertex3d(double(solution.at(0).at(l).X)/double(scale), pWing->m_TPos[k], double(solution.at(0).at(l).Y)/double(scale));
							}
							glVertex3d(double(solution.at(0).at(0).X)/double(scale), pWing->m_TPos[k], double(solution.at(0).at(0).Y)/double(scale));
							
						}
						glEnd();
					}
					
					glColor3d(shellColor.redF(), shellColor.greenF(),shellColor.blueF());
				}
			}
		}
		glEndList();
		
		
		
		
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		
		gluDeleteTess(tess);        // delete after tessellation
	}
	
    if (m_structure && m_bResetglGeom && m_QFEMDock->m_pctrlInternal->isChecked()){
	glNewList(WING2OUTLINE,GL_COMPILE);
	{
		
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glPolygonOffset(-8.0, -8.0);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)2);
		
		color = outlineColor;
		style = 0; /*m_OutlineStyle;*/
		
		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);
		
		glColor3d(color.redF(),color.greenF(),color.blueF());
		glLineWidth((GLfloat)outlinewidth);
		for (int kk = 0; kk < AllInner.size(); kk++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l = 0; l < AllInner.at(kk).size(); l++)
				{
					glVertex3d(AllInner.at(kk).at(l).at(0), AllInner.at(kk).at(l).at(1), AllInner.at(kk).at(l).at(2));
				}
				glVertex3d(AllInner.at(kk).at(0).at(0), AllInner.at(kk).at(0).at(1), AllInner.at(kk).at(0).at(2));
				
			}
			glEnd();
		}
		
		for (int kk = 0; kk < AllSpar.size(); kk++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l = 0; l < AllSpar.at(kk).size(); l++)
				{
					glVertex3d(AllSpar.at(kk).at(l).at(0), AllSpar.at(kk).at(l).at(1), AllSpar.at(kk).at(l).at(2));
				}
				glVertex3d(AllSpar.at(kk).at(0).at(0), AllSpar.at(kk).at(0).at(1), AllSpar.at(kk).at(0).at(2));
				
			}
			glEnd();
		}
		
		
		
		
	}
	glEndList();
    }

	
	m_needToRerender = false;
	m_bResetglGeom = false;
	m_newSectionHighlight = false;
	
}

void QFEMModule::OnShellColor()
{
        if(!m_structure) return;

        QColor shell = QColorDialog::getColor(shellColor, g_mainFrame, "Select the outer shell color", QColorDialog::ShowAlphaChannel);
        if(shell.isValid()) shellColor = shell;

        m_QFEMDock->m_pctrlshellColor->SetColor(shellColor);
        m_bResetglGeom = true;
        m_needToRerender = true;
        reportGLChange();
}

void QFEMModule::OnSparColor()
{
    if(!m_structure) return;

    QColor spar = QColorDialog::getColor(sparColor, g_mainFrame, "Select the inner spar color", QColorDialog::ShowAlphaChannel);
    if(spar.isValid()) sparColor = spar;

    m_QFEMDock->m_pctrlsparColor->SetColor(sparColor);
    m_bResetglGeom = true;
    m_needToRerender = true;
    reportGLChange();

}

void QFEMModule::OnAlignSparAtMaxThickness()
{
    for (int i=0; i<m_structure->m_numElems;i++)
    {
        CFoil *foil = g_foilStore.getObjectByName(m_rotor->m_Airfoils[i], NULL);
        m_structure->SparLocation[i] = foil->m_fXThickness;
        FillTableRow(i);
    }
    m_bResetglGeom = true;
    m_needToRerender = true;
    reportGLChange();
}


void QFEMModule::UpdateGeomRerenderGL() {
    m_bResetglGeom = true;
    m_needToRerender = true;
    m_newSectionHighlight = true;
    m_axes = m_QFEMDock->m_pctrlAxes->isChecked();
	if (m_QFEMDock->m_pctrlPerspective->isChecked()) {
		g_mainFrame->getGlWidget()->camera()->setType(qglviewer::Camera::PERSPECTIVE);
	} else {
		g_mainFrame->getGlWidget()->camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
	}
	
	reportGLChange();
}

void QFEMModule::OnLightDlg() {
    m_GLLightDlg.m_pGLWidget = m_glWidget;
    m_GLLightDlg.show();
    m_glWidget->GLSetupLight(m_GLLightDlg, 0,  1.0);

    reportGLChange();
}

CBlade* QFEMModule::getBlade() {
    return m_rotor;
}

void QFEMModule::OnSelChangeModeType(int i)
{    
    m_modeType = i;
    UpdateModeNumber();
}

void QFEMModule::UpdateModeNumber()
{
    QString strong;
    m_QFEMDock->m_pctrlModeNumberSelecta->clear();

    if (m_modeType == 0)
    {
        if (m_structure->FlapWiseNodeTranslations.size()>3)
        {
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("1");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("2");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("3");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("4");
        }
        else
        {
            for (int i=1;i<=m_structure->FlapWiseNodeTranslations.size();i++) m_QFEMDock->m_pctrlModeNumberSelecta->addItem(strong.number(i));
        }
    }

    else if (m_modeType == 1)
    {
        if (m_structure->EdgeWiseNodeTranslations.size()>3)
        {
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("1");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("2");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("3");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("4");
        }
        else
        {
            for (int i=1;i<=m_structure->EdgeWiseNodeTranslations.size();i++) m_QFEMDock->m_pctrlModeNumberSelecta->addItem(strong.number(i));
        }
    }

    else if (m_modeType == 2)
    {
        if (m_structure->TorsionalTranslations.size()>3)
        {
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("1");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("2");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("3");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("4");
        }
        else
        {
            for (int i=1;i<=m_structure->TorsionalTranslations.size();i++) m_QFEMDock->m_pctrlModeNumberSelecta->addItem(strong.number(i));
        }
    }

    else if (m_modeType == 3)
    {
        if (m_structure->RadialNodeTranslations.size()>3)
        {
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("1");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("2");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("3");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("4");
        }
        else
        {
            for (int i=1;i<=m_structure->RadialNodeTranslations.size();i++) m_QFEMDock->m_pctrlModeNumberSelecta->addItem(strong.number(i));
        }
    }

    else if (m_modeType == 4)
    {
        if (m_structure->UnsortedNodeTranslations.size()>3)
        {
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("1");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("2");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("3");
            m_QFEMDock->m_pctrlModeNumberSelecta->addItem("4");
        }
        else
        {
            for (int i=1;i<=m_structure->UnsortedNodeTranslations.size();i++) m_QFEMDock->m_pctrlModeNumberSelecta->addItem(strong.number(i));
        }
    }

    if (m_QFEMDock->m_pctrlModeNumberSelecta->count() > m_modeNumber)
    {
        m_QFEMDock->m_pctrlModeNumberSelecta->setCurrentIndex(m_modeNumber);
    }
    else
    {
        m_QFEMDock->m_pctrlModeNumberSelecta->setCurrentIndex(0);
        m_modeNumber = 0;
    }

}

void QFEMModule::OnSelChangeModeNumber(int i)
{
    if (i >= 0)
    {
    m_modeNumber = i;
    DeformBlade();
    }
}

void QFEMModule::DeformBlade()
{
    if (m_bStructEdited) return;
    double max, val;
    if (!m_rotor) return;
    if (!m_structure) return;
    m_needToRerender = true;
    if (m_deformed_rotor) delete m_deformed_rotor;
    m_deformed_rotor = new CBlade;
    m_deformed_rotor->Duplicate(m_rotor);

    double rotorradialfrac = 0.15;

    if (!m_QFEMDock->m_LoadingShown)
    {
    if (m_modeType == 0 && m_QFEMDock->m_pctrlModeNumberSelecta->count())
    {
        max = 0;
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            val = m_structure->FlapWiseNodeTranslations.at(m_modeNumber).at(i).at(1)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
        }

        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->FlapWiseNodeTranslations.at(m_modeNumber).at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->FlapWiseNodeTranslations.at(m_modeNumber).at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->FlapWiseNodeTranslations.at(m_modeNumber).at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->FlapWiseNodeTranslations.at(m_modeNumber).at(i).at(1);
        }
    }
    if ( m_modeType == 1 && m_QFEMDock->m_pctrlModeNumberSelecta->count())
    {
        max = 0;
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            val = m_structure->EdgeWiseNodeTranslations.at(m_modeNumber).at(i).at(0)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
        }

        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->EdgeWiseNodeTranslations.at(m_modeNumber).at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->EdgeWiseNodeTranslations.at(m_modeNumber).at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->EdgeWiseNodeTranslations.at(m_modeNumber).at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->EdgeWiseNodeTranslations.at(m_modeNumber).at(i).at(1);
        }
    }
    if ( m_modeType == 2 && m_QFEMDock->m_pctrlModeNumberSelecta->count())
    {
        max = 0;
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            val = m_structure->TorsionalTranslations.at(m_modeNumber).at(i).at(3)/2/PI;
            if (fabs(val) >= max) max = val;
        }

        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]     += 2*rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->TorsionalTranslations.at(m_modeNumber).at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += 2*rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->TorsionalTranslations.at(m_modeNumber).at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += 2*rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->TorsionalTranslations.at(m_modeNumber).at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += 2*rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->TorsionalTranslations.at(m_modeNumber).at(i).at(1);
        }
    }
    if ( m_modeType == 3  && m_QFEMDock->m_pctrlModeNumberSelecta->count() )
    {
        max = 0;
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            val = m_structure->RadialNodeTranslations.at(m_modeNumber).at(i).at(2)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
        }

        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]       += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->RadialNodeTranslations.at(m_modeNumber).at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->RadialNodeTranslations.at(m_modeNumber).at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->RadialNodeTranslations.at(m_modeNumber).at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->RadialNodeTranslations.at(m_modeNumber).at(i).at(1);
        }
    }
    if ( m_modeType == 4 && m_QFEMDock->m_pctrlModeNumberSelecta->count() )
    {
        max = 0;
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            val = m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(2)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
            val = m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(3)/2/PI;
            if (fabs(val) >= max) max = val;
            val = 0.5*m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(0)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
            val = m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(1)/m_deformed_rotor->getRotorRadius();
            if (fabs(val) >= max) max = val;
        }

        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += rotorradialfrac/max*m_QFEMDock->m_modeSlider->value()/100*m_structure->UnsortedNodeTranslations.at(m_modeNumber).at(i).at(1);
        }
    }
    }
    else if (m_QFEMDock->m_LoadingShown && m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject())
    {
        for (int i = 0; i < m_structure->m_numElems; ++i)
        {
            m_deformed_rotor->m_TPos[i]     += m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->nodeTranslations.at(i).at(2);
            m_deformed_rotor->m_TTwist[i]     += m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->nodeTranslations.at(i).at(3)/2/PI*360;
            m_deformed_rotor->m_TOffsetX[i]    += m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->nodeTranslations.at(i).at(0);
            m_deformed_rotor->m_TOffsetZ[i]    += m_QFEMToolBar->m_BladeStructureLoadingComboBox->currentObject()->nodeTranslations.at(i).at(1);
        }
    }
    QFEMCompleted = true;
    UpdateGeomRerenderGL();
}

void QFEMModule::OnHideWidgets() {
    if (m_QFEMToolBar->HideWidgetsAct->isChecked()) {
        m_QFEMDock->hide();
    } else {
        m_QFEMDock->show();
    }
}

void QFEMModule::SliderPressed() {
    m_internalChecked = m_QFEMDock->m_pctrlInternal->isChecked();
    m_QFEMDock->m_pctrlInternal->setChecked(false);
}

void QFEMModule::SliderReleased() {
    m_QFEMDock->m_pctrlInternal->setChecked(m_internalChecked);
    UpdateGeomRerenderGL();
}

QFEMModule *g_QFEMModule;
