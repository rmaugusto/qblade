#ifndef TWODWIDGETEVENTINTERFACE_H
#define TWODWIDGETEVENTINTERFACE_H

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
	enum GraphArrangement {Single, Vertical, Vertical3, Horizontal, Quad};
	
	TwoDWidgetInterface();
	void setContextMenu (TwoDContextMenu *contextMenu) { m_contextMenu = contextMenu; }
	void update ();
	
	virtual QStringList getAvailableGraphVariables () = 0;
	virtual void showAll () = 0;
	virtual void hideAll () = 0;
	
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
	GraphArrangement getGraphArrangement () { return m_graphArrangement; }
    void setGraphArrangement(GraphArrangement ga){m_graphArrangement = ga;}
	
	virtual void setContextMenuGraphType (NewGraph::GraphType graphType) = 0;
	
	TwoDWidget *m_twoDWidget;  // pointer to the TwoDWidget of QBlade
	int m_currentGraphIndex;
	
private:
	int findCursorPositionIndex (QPoint position);
	
	TwoDContextMenu *m_contextMenu;
	GraphArrangement m_graphArrangement;
	bool m_xKeyPressed, m_yKeyPressed;
	QPoint m_lastMousePosition;
	
public/* slots*/:  // should be slots, which is not possible, because this class can't be a QObject
	void resetScale ();
	void resetScaleAll ();  // reset the scale for all graphs
	void autoResetSwitch (bool status);
	void exportGraph ();
	void changeGraphType (NewGraph::GraphType graphType);
};

#endif // TWODWIDGETEVENTINTERFACE_H
