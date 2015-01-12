#ifndef SHOWASGRAPHINTERFACE_H
#define SHOWASGRAPHINTERFACE_H

#include <QString>
class QStringList;

#include "../Store_include.h"
#include "../MainFrame.h"
#include "../Graph/NewGraph.h"
class NewCurve;


class ShowAsGraphInterface
{
public:
	ShowAsGraphInterface (bool initialise = true);
	virtual NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType) = 0;  // returns NULL if var not available
	virtual QStringList getAvailableVariables (NewGraph::GraphType graphType) = 0;
	virtual QString getObjectName () = 0;
	
	void setDrawPoints (bool drawPoints) { m_drawPoints = drawPoints; }
	void setDrawCurve (bool drawCurve) { m_drawCurve = drawCurve; }
	void setShownInGraph (bool shownInGraph) { m_shownInGraph = shownInGraph; }
	void setPen (QPen pen) { m_pen = pen; }
	bool isDrawPoints () { return m_drawPoints; }
	bool isDrawCurve () { return m_drawCurve; }
	bool isShownInGraph () { return m_shownInGraph; }
	QPen getPen () { return m_pen; }
	
protected:
	void serialize ();
	template <class OBJECT>
	QColor findColor (Store<OBJECT> *storeToSearch);
	
	bool m_shownInGraph;  // if this simulation will have a representation in the graphs
	bool m_drawPoints, m_drawCurve;
	QPen m_pen;
};


template <class OBJECT>  // template functions must be in the header
QColor ShowAsGraphInterface::findColor (Store<OBJECT> *storeToSearch) {
	bool colorFound;
	for (int i = 0; i < 24; ++i) {
		colorFound = false;
		for (int j = 0; j < storeToSearch->size(); ++j) {
			if (storeToSearch->at(j)->getPen().color() == g_mainFrame->m_crColors[i]) {
				colorFound = true;
				break;
			}
		}
		if (!colorFound) {
			return g_mainFrame->m_crColors[i];
		}
	}
	return g_mainFrame->m_crColors[storeToSearch->size() % 24];
}

#endif // SHOWASGRAPHINTERFACE_H
