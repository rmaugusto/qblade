#include "TwoDWidgetEventInterface.h"

#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QDebug>
#include <QCursor>
#include <QFileDialog>

#include "MainFrame.h"
#include "TwoDWidget.h"
#include "Graph/GraphOptionsDialog.h"
#include "Graph/NewGraph.h"
#include "GlobalFunctions.h"
#include "TwoDContextMenu.h"


TwoDWidgetInterface::TwoDWidgetInterface() {
	m_twoDWidget = g_mainFrame->getTwoDWidget();
	m_graphArrangement = Vertical3;
	m_xKeyPressed = false;
	m_yKeyPressed = false;
	for (int i = 0; i < 4; ++i) {
		m_graph[i] = NULL;
	}
}

void TwoDWidgetInterface::update() {
    if (g_mainFrame->getTwoDWidgetInterface() == this) {
		m_twoDWidget->update();
	}
}

void TwoDWidgetInterface::onPaintEvent(QPaintEvent */*event*/) {
	QPainter painter (m_twoDWidget);
	painter.fillRect(m_twoDWidget->rect(), g_mainFrame->getBackgroundColor());

	if (TwoDscreenMessage(g_mainFrame->m_iApp,g_mainFrame->m_iView,painter,m_twoDWidget)) {
		return;
	}

	switch (m_graphArrangement) {
	case Quad:
		m_graph[3]->drawGraph(painter);
	case Vertical3:
		m_graph[2]->drawGraph(painter);
	case Vertical:
	case Horizontal:
		m_graph[1]->drawGraph(painter);
	case Single:
		m_graph[0]->drawGraph(painter);
		break;
	}
}

void TwoDWidgetInterface::onResizeEvent() {
	const int border = 0;
	const int gap = 0;
	int w, h;
	QRect max (m_twoDWidget->rect());
	
	switch (m_graphArrangement) {
	case Single:
		w = max.width()-2*border;
		h = max.height()-2*border;
		m_graph[0]->setDrawingArea(QRect(max.x()+border, max.y()+border, w, h));
		break;
	case Vertical:
		w = max.width()-2*border;
		h = (max.height()-2*border-gap) / 2;
		m_graph[0]->setDrawingArea(QRect(max.x()+border, max.y()+border, w, h));
		m_graph[1]->setDrawingArea(QRect(max.x()+border, max.y()+border+h+gap, w, h+max.height()%2));
		break;
	case Vertical3:
		w = max.width()-2*border;
		h = (max.height()-2*border-2*gap) / 3;
		m_graph[0]->setDrawingArea(QRect(max.x()+border, max.y()+border, w, h));
		m_graph[1]->setDrawingArea(QRect(max.x()+border, max.y()+border+h+gap, w, h));
		m_graph[2]->setDrawingArea(QRect(max.x()+border, max.y()+border+2*(h+gap), w, h+max.height()%3));
		break;
	case Horizontal:
		w = (max.width()-2*border-gap) / 2;
		h = max.height()-2*border;
		m_graph[0]->setDrawingArea(QRect(max.x()+border, max.y()+border, w, h));
		m_graph[1]->setDrawingArea(QRect(max.x()+border+w+gap, max.y()+border, w+max.width()%2, h));
		break;
	case Quad:
		w = (max.width()-2*border-gap) / 2;
		h = (max.height()-2*border-gap) / 2;
		m_graph[0]->setDrawingArea(QRect(max.x()+border, max.y()+border, w, h));
		m_graph[1]->setDrawingArea(QRect(max.x()+border+w+gap, max.y()+border, w+max.width()%2, h));
		m_graph[2]->setDrawingArea(QRect(max.x()+border, max.y()+border+h+gap, w, h+max.height()%2));
		m_graph[3]->setDrawingArea(QRect(max.x()+border+w+gap, max.y()+border+h+gap, w+max.width()%2, h+max.height()%2));
		break;
	}
}

void TwoDWidgetInterface::onKeyPressEvent(QKeyEvent *event) {
	m_currentGraphIndex = findCursorPositionIndex(m_twoDWidget->mapFromGlobal(QCursor::pos()));

	switch (event->key()) {
	case Qt::Key_X:
		m_xKeyPressed = true;
		break;
	case Qt::Key_Y:
		m_yKeyPressed = true;
		break;
	case Qt::Key_R:
		resetScale();
		break;
	default:
		event->ignore();  // NM this will pass the event to the next widget
	}
}

void TwoDWidgetInterface::onKeyReleaseEvent(QKeyEvent *event) {
	switch (event->key()) {
	case Qt::Key_X:
		if (!event->isAutoRepeat()) {
			m_xKeyPressed = false;
		}
		break;
	case Qt::Key_Y:
		if (!event->isAutoRepeat()) {
			m_yKeyPressed = false;
		}
		break;
	}
}

void TwoDWidgetInterface::onMousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_currentGraphIndex = findCursorPositionIndex(event->pos());
		if (m_currentGraphIndex != -1) {
			m_lastMousePosition = event->pos();
			m_twoDWidget->setCursor(Qt::ClosedHandCursor);
		}
	}
}

void TwoDWidgetInterface::onMouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton && m_currentGraphIndex != -1) {
		m_twoDWidget->setCursor(Qt::CrossCursor);
	}
}

void TwoDWidgetInterface::onMouseDoubleClickEvent(QMouseEvent *event) {
	m_currentGraphIndex = findCursorPositionIndex(event->pos());
	GraphOptionsDialog dialog (m_graph[m_currentGraphIndex]);
	dialog.exec();
}

void TwoDWidgetInterface::onMouseMoveEvent(QMouseEvent *event) {
	if (event->buttons() & Qt::LeftButton) {
		m_graph[m_currentGraphIndex]->translate(m_lastMousePosition, event->pos());
		
		m_lastMousePosition = event->pos();
		m_twoDWidget->update();
	}
}

void TwoDWidgetInterface::onWheelEvent(QWheelEvent *event) {
	double zoomFactor = 1.06;
	if ((event->delta() < 0) ^ g_mainFrame->m_bReverseZoom) {
		zoomFactor = 1/zoomFactor;
	}
	int index = findCursorPositionIndex(event->pos());
	if (index != -1) {
		if (!m_yKeyPressed) {
			m_graph[index]->zoomX(zoomFactor);
		}
		if (!m_xKeyPressed) {
			m_graph[index]->zoomY(zoomFactor);
		}
	}
	
	m_twoDWidget->update();	
}

void TwoDWidgetInterface::onContextMenuEvent(QContextMenuEvent *event) {
	m_currentGraphIndex = findCursorPositionIndex(event->pos());
	if (m_currentGraphIndex != -1) {
		setContextMenuGraphType(m_graph[m_currentGraphIndex]->getGraphType());
		m_contextMenu->setAutoReset(m_graph[m_currentGraphIndex]->getNoAutoResize());
		m_contextMenu->exec(event->globalPos());
	}
}

int TwoDWidgetInterface::findCursorPositionIndex(QPoint position) {
	for (int i = 0; i < 4; ++i) {
		if (m_graph[i]->contains(position)) {
			return i;
		}
	}
	return -1;
}

void TwoDWidgetInterface::resetScale() {
	m_graph[m_currentGraphIndex]->setOptimalLimits();
	m_twoDWidget->update();	
}

void TwoDWidgetInterface::resetScaleAll() {
	for (int i = 0; i < 4; ++i) {
		if (m_graph[i]) {
			m_graph[i]->setOptimalLimits();
		}
	}
}

void TwoDWidgetInterface::autoResetSwitch(bool status) {
	m_graph[m_currentGraphIndex]->setNoAutoResize(status);
}

void TwoDWidgetInterface::exportGraph() {
    int pos,type;
    QString fileName = QFileDialog::getSaveFileName(g_mainFrame, QString("Export Graph"), g_mainFrame->m_ExportLastDirName,                                              QString("Text File (*.txt);;Comma Separated Values (*.csv)"), &g_mainFrame->m_GraphExportFilter);
    pos = fileName.indexOf(".csv");
    if(pos>0) type=2; else type=1;
    m_graph[m_currentGraphIndex]->exportGraph(fileName,type);
}

void TwoDWidgetInterface::changeGraphType(NewGraph::GraphType graphType) {
	m_graph[m_currentGraphIndex]->setGraphType(graphType);
	
	/* set default parameters after type change */
	switch (graphType) {
	case NewGraph::FastSimulation: m_graph[m_currentGraphIndex]->setShownVariables("Time", "RotPwr"); break;
	case NewGraph::BladeGraph: m_graph[m_currentGraphIndex]->setShownVariables("Radius", "CLift"); break;
	default: m_graph[m_currentGraphIndex]->setShownVariables("", "");
	}

	m_graph[m_currentGraphIndex]->updateGraph();
}
