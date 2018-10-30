#ifndef SHOWASGRAPHINTERFACE_H
#define SHOWASGRAPHINTERFACE_H

#include <QString>
#include <QDebug>
class QStringList;

#include "../Store_include.h"
#include "../MainFrame.h"
#include "../Graph/NewGraph.h"
class NewCurve;


class ShowAsGraphInterface
{
public:
	ShowAsGraphInterface (bool initialise = true);
	virtual NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType) = 0;  // returns NULL if var n.a.
	virtual QString getObjectName () = 0;
	// any inheriting class should implement the following function rather as static or common member function
	//	virtual QStringList getAvailableVariables (NewGraph::GraphType graphType) = 0;
	
	void setDrawPoints (bool drawPoints) { m_drawPoints = drawPoints; }
	void setDrawCurve (bool drawCurve) { m_drawCurve = drawCurve; }
	void setShownInGraph (bool shownInGraph) { m_shownInGraph = shownInGraph; }
	void setPen (QPen pen) { m_pen = pen; }
	QPen* pen () { return &m_pen; }
	virtual bool isDrawPoints () { return m_drawPoints; }
	virtual bool isDrawCurve () { return m_drawCurve; }
	bool isShownInGraph () { return m_shownInGraph; }
	QPen getPen (int curveIndex = -1, int highlightedIndex = -1, bool forTheDot = false);

protected:
	void serialize ();
	
	bool m_shownInGraph;  // if this simulation will have a representation in the graphs
	bool m_drawPoints, m_drawCurve;
	QPen m_pen;

private:
	virtual QPen doGetPen (int /*curveIndex*/, int /*highlightedIndex*/, bool /*forTheDot*/) { return m_pen; }
};

#endif // SHOWASGRAPHINTERFACE_H
