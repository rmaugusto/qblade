#include "ShowAsGraphInterface.h"

#include <QDebug>

#include "../Serializer.h"
#include "../MainFrame.h"
#include "../TwoDWidgetInterface.h"


ShowAsGraphInterface::ShowAsGraphInterface(bool initialise) {
	if (initialise) {
		m_shownInGraph = true;
		m_drawCurve = true;
		m_drawPoints = false;
//		m_color =  // NM this must be done in the deriving class' constructor (with colorManager)!
	}
}

QPen ShowAsGraphInterface::getPen(int curveIndex, int highlightedIndex, bool forTheDot) {
	return doGetPen(curveIndex, highlightedIndex, forTheDot);
}

void ShowAsGraphInterface::serialize() {
	g_serializer.readOrWriteBool(&m_shownInGraph);
	g_serializer.readOrWriteBool(&m_drawCurve);
	g_serializer.readOrWriteBool(&m_drawPoints);
	g_serializer.readOrWritePen(&m_pen);
}
