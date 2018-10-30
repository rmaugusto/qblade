#ifndef TWODWIDGETINTERFACE_H
#define TWODWIDGETINTERFACE_H

#include <QRect>
class QPaintEvent;
class QResizeEvent;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QContextMenuEvent;
class TwoDContextMenu;

#include "Graph/QGraph.h"
#include "Graph/NewGraph.h"
class TwoDWidget;


/* This class can be inherited to receive default event handlers for the TwoDWidget.
 * It could be integrated into TwoDModule as soon as this is inherited by all modules */
class TwoDWidgetInterface
{
public:
    enum GraphArrangement {Single, Vertical, Vertical3, Horizontal, Quad, QuadVertical};
	
	TwoDWidgetInterface();
	void setContextMenu (TwoDContextMenu *contextMenu) { m_contextMenu = contextMenu; }
	void update ();
	void reloadForGraphType (NewGraph::GraphType type = NewGraph::None);
	void reloadAllGraphCurves ();
	
	virtual QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
											NewGraph::GraphType graphTypeMulti) = 0;
	virtual QStringList getAvailableGraphVariables (bool xAxis) = 0;
	virtual QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType) = 0;
	virtual int getHighlightIndex(NewGraph::GraphType /*graphTypeMulti*/) { return -1; }
	virtual void showAll () = 0;  // show all available graphs
	virtual void hideAll () = 0;  // hide all available graphs
	GraphArrangement getGraphArrangement () { return m_graphArrangement; }
	void setGraphArrangement (GraphArrangement newArrangement);
	
	virtual void onPaintEvent (QPaintEvent *event);
	virtual void onResizeEvent ();
	virtual void onKeyPressEvent (QKeyEvent *event);
	virtual void onKeyReleaseEvent (QKeyEvent *event);
	virtual void onMousePressEvent (QMouseEvent *event);
	virtual void onMouseReleaseEvent (QMouseEvent *event);
	virtual void onMouseDoubleClickEvent (QMouseEvent *event);
	virtual void onMouseMoveEvent (QMouseEvent *event);
	virtual void onWheelEvent (QWheelEvent *event );
	virtual void onContextMenuEvent (QContextMenuEvent *event);
    NewGraph *m_graph[4];  // it's up to the module to allocate the graphs
	
protected:
	TwoDWidget *m_twoDWidget;  // pointer to the TwoDWidget of QBlade
	int m_currentGraphIndex;  // the active graph, determined by the mouse pointer
	
private:
	int findCursorPositionIndex (QPoint position);
	
	TwoDContextMenu *m_contextMenu;
	GraphArrangement m_graphArrangement;
	bool m_xKeyPressed, m_yKeyPressed;  // NM todo there probably is a Qt function to ask for currently pressed keys
	QPoint m_lastMousePosition;
	
public/* slots*/:  // should be slots, which is not possible, because this class can't be a QObject
	void resetScale (bool force);
	void resetScaleForGraphType (bool force, NewGraph::GraphType type = NewGraph::None);
	void autoResetSwitch (bool status);
	void exportGraph ();
	void changeGraphType (NewGraph::GraphType graphType);
	void changeGraphTypeMulti (NewGraph::GraphType graphTypeMulti);
};

#endif // TWODWIDGETINTERFACE_H
