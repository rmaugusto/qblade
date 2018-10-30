#include "NewGraph.h"

#include <limits>
#include <QPainter>
#include <QPoint>
#include <QVector>
#include <QFontMetrics>
#include <QSettings>
#include <QDate>
#include <QTime>

#include "NewCurve.h"
#include "ShowAsGraphInterface.h"
#include "../Store.h"
#include "../XUnsteadyBEM/FASTModule.h"
#include "../XDMS/TurDmsModule.h"
#include "../XDMS/MultiDmsModule.h"
#include "../XLLT/QLLTModule.h"
#include "../XUnsteadyBEM/FASTSimulation.h"
#include "../QFem/BladeStructure.h"
#include "../XBEM/TData.h"
#include "../Noise/NoiseModule.h"


/* the graph won't reprint itself when changes are made. The calling class has to take care of that. */
NewGraph::NewGraph(QString nameInSettings, TwoDWidgetInterface *twoDInterface, const GraphDefaults &defaults)
	: m_graphType(None),
	  m_graphTypeMulti(None),
	  m_twoDInterface(twoDInterface),
	  m_xAxis (-1.1, 1.1, 2.5, defaults.xLogarithmic),
	  m_yAxis (-1.1, 1.1, 5, defaults.yLogarithmic),
	  m_nameInSettings(nameInSettings),
	  m_borderGapWidth(3),
	  m_gapWidth(3)
{
	static bool registerMetaType = false;  // this is needed to save and load this type from settings
	if (!registerMetaType) {  // execute once
		registerMetaType = true;
		qRegisterMetaType<QMap<int, QPair<QString,QString> > >("GraphTypeMap");
		qRegisterMetaTypeStreamOperators<QMap<int, QPair<QString,QString> > >("GraphTypeMap");
		qRegisterMetaType<QMap<int, int> >("GraphTypeMultiMap");
		qRegisterMetaTypeStreamOperators<QMap<int, int> >("GraphTypeMultiMap");
	}
	
	if (! loadStylesFromSettings()) {  // set the default style, if this graph is not found in the settings
		setGraphType(defaults.graphType);
		m_xAxisTitle = defaults.xTitle;
		m_yAxisTitle = defaults.yTitle;
		m_backgroundColor = QColor ("white");
		m_borderColor = QColor ("lightgrey");
		m_titleColor = QColor ("black");
		m_tickColor = QColor ("black");
		m_titleFont.setPointSize(18);
		m_mainAxesPen.setStyle(Qt::SolidLine);
		m_mainAxesPen.setWidth(3);
		m_mainAxesPen.setColor("grey");
		m_xGridPen.setStyle(Qt::DashLine);
		m_xGridPen.setWidth(1);
		m_xGridPen.setColor("grey");
		m_yGridPen.setStyle(Qt::DashLine);
		m_yGridPen.setWidth(1);
		m_yGridPen.setColor("grey");
		setBorderWidth(1);
		m_noAutoResize = false;
	}
	
	reloadCurves();
}

NewGraph::~NewGraph() {
	removeAllCurves ();
	saveStylesToSettings ();
}

void NewGraph::setGraphType(GraphType graphType) {
	if (m_graphType == graphType)
		return;  // nothing to do
	
	/* store type */
	m_formerAxisTitles[m_graphType] = QPair<QString,QString> (m_xAxisTitle, m_yAxisTitle);  // store the former axis titles
	m_formerGraphTypeMulti[m_graphType] = m_graphTypeMulti;
	
	/* switch type */
	m_graphType = graphType;

	/* prepare new type */
	if (m_formerAxisTitles.contains(m_graphType)) {  // restore former axis titles for new type
		m_xAxisTitle = m_formerAxisTitles[graphType].first;
		m_yAxisTitle = m_formerAxisTitles[graphType].second;
	} else {
		m_xAxisTitle = "";
		m_yAxisTitle = "";
	}
	if (graphType == TurbineRotor || graphType == RotorLegend) {  // force RotorAllSims
		m_graphTypeMulti = RotorAllSimulations;
	} else if (m_formerGraphTypeMulti.contains(graphType)) {
		m_graphTypeMulti = static_cast<GraphType>(m_formerGraphTypeMulti[graphType]);
	} else {
		m_graphTypeMulti = None;
	}
}

void NewGraph::drawGraph(QPainter &painter) {	
	painter.setClipRect(m_drawingArea);
	painter.fillRect(m_drawingArea, m_borderColor);
	const int nettoBorder = m_borderWidth - m_borderGapWidth;
	const QRect inner (m_drawingArea.adjusted(nettoBorder, nettoBorder, -nettoBorder, -nettoBorder));
	painter.fillRect(inner, m_backgroundColor);
	
	switch (m_graphType) {
	case TurbineLegend:
	{
		painter.setClipRect(inner);
		painter.setFont(QFont());
		painter.setPen(QPen());
		const int textHeight = QFontMetrics(QFont()).boundingRect("fg").height();
		const int left = inner.left() + 20;
		int yPosition = inner.top() + 5;
		for (int i = 0; i < g_verttdataStore.size(); ++i) {
			TData *turbine = g_verttdataStore.at(i);
			TDMSData *simulation;
			bool first = true;
			for (int j = 0; turbine->getChild(j) != NULL; ++j) {
				simulation = dynamic_cast<TDMSData*>(turbine->getChild(j));
				if (simulation != NULL && simulation->isShownInGraph()) {
					if (first) {
						first = false;
						yPosition += textHeight;
						painter.drawText(left, yPosition, turbine->getName());
					}
					
					yPosition += textHeight;
					painter.setPen(simulation->getPen());
					painter.drawLine(left+10, yPosition - textHeight/3, left + 110, yPosition - textHeight/3);
					painter.setPen(QPen());
					painter.drawText(left + 120, yPosition, simulation->getName());
				}
			}
		}
		break;
	}
	case RotorLegend:
	{
		painter.setClipRect(inner);
		painter.setFont(QFont());
		painter.setPen(QPen());
		const int textHeight = QFontMetrics(QFont()).boundingRect("fg").height();
		const int left = inner.left() + 20;
		int yPosition = inner.top() + 5;
		for (int i = 0; i < g_verticalRotorStore.size(); ++i) {
			CBlade *rotor = g_verticalRotorStore.at(i);
			DMSData *simulation;
			bool first = true;
			for (int j = 0; rotor->getChild(j) != NULL; ++j) {
				simulation = dynamic_cast<DMSData*>(rotor->getChild(j));
				if (simulation != NULL && simulation->isShownInGraph()) {
					if (first) {
						first = false;
						yPosition += textHeight;
						painter.drawText(left, yPosition, rotor->getName());
					}

					yPosition += textHeight;
					painter.setPen(simulation->getPen());
					painter.drawLine(left+10, yPosition - textHeight/3, left + 110, yPosition - textHeight/3);
					painter.setPen(QPen());
					painter.drawText(left + 120, yPosition, simulation->getName());
				}
			}
		}
		break;
	}
	default:
		drawTicks (painter);
		drawGrid (painter);
		drawTitles (painter);
		drawCurves (painter);
	}
}

void NewGraph::exportGraph(QString fileName, ExportType type) {
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

	QFile file (fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream (&file);

		int numberOfPoints = 0;
		QVector<QPointF> points[m_curves.size()];  // preparation of this array is of vital importance for performance
        stream << "Export File Created with "<< g_mainFrame->m_VersionName<<" on "<<date.toString("dd.MM.yyyy")
			   << " at " << time.toString("hh:mm:ss") << endl;
		for (int i = 0; i < m_curves.size(); ++i) {
			if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
				if (type == HumanReadable)
					stream << QString("  %1").arg(m_curves[i]->getAssociatedObject()->getObjectName(), 30);
                if (type == CSV)
					stream << QString("%1;;").arg(m_curves[i]->getAssociatedObject()->getObjectName());
				if (m_curves[i]->getNumberOfPoints() > numberOfPoints) {  // find maximal number of points
					numberOfPoints = m_curves[i]->getNumberOfPoints();
				}
				points[i] = m_curves[i]->getAllPoints();
			}
		}
		stream << endl;
		
		for (int i = 0; i < m_curves.size(); ++i) {  // write axis titles
			if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
                if (type == HumanReadable)
					stream << QString(" %1 %2").arg(m_xAxisTitle, 15).arg(m_yAxisTitle, 15);
                if (type == CSV)
					stream << QString("%1;%2;").arg(m_xAxisTitle).arg(m_yAxisTitle);
			}
		}
		stream << endl;
		
		for (int i = 0; i < numberOfPoints; ++i) {  // write the values
			for (int j = 0; j < m_curves.size(); ++j) {
				if (m_curves[j]->getAssociatedObject()->isShownInGraph()) {				
					if (i < m_curves[j]->getNumberOfPoints()) {
                        if (type == HumanReadable) {
							stream << QString(" %1").arg(points[j][i].x(), 15, 'e', 5)
								   << QString(" %1").arg(points[j][i].y(), 15, 'e', 5);
						}
                        if (type == CSV) {
                            stream << QString("%1;").arg(points[j][i].x(), 0, 'e', 5).replace(".",",")
                                   << QString("%1;").arg(points[j][i].y(), 0, 'e', 5).replace(".",",");
						}
					} else {
                        if (type == HumanReadable)
							stream << QString().fill(' ', 32);
                        if (type == CSV)
							stream << QString(";;");
					}
				}
			}
			stream << endl;
		}
	}
    file.close();
}

void NewGraph::setDrawingArea(QRect drawingArea) {
	m_drawingArea = drawingArea;
	calculateGraphArea();
}

bool NewGraph::contains(QPoint point) {
	return m_drawingArea.contains(point);
}

void NewGraph::translate(QPoint from, QPoint to) {
	m_xAxis.moveLimits(from.x() - to.x());
	m_yAxis.moveLimits(to.y() - from.y());
}

void NewGraph::zoomX(double zoom) {
	m_xAxis.zoom(zoom);
}

void NewGraph::zoomY(double zoom) {
	m_yAxis.zoom(zoom);
}

void NewGraph::setOptimalLimits(bool force) {
	if (!m_noAutoResize || force) {
		setOptimalXLimits();
		setOptimalYLimits();
	}
}

void NewGraph::setOptimalXLimits() {
	double lowX = std::numeric_limits<double>::max();
	double highX = std::numeric_limits<double>::lowest();
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			if (m_curves[i]->getLowX(m_xAxis.getLogarithmic()) < lowX) {
				lowX = m_curves[i]->getLowX(m_xAxis.getLogarithmic());
			}
			if (m_curves[i]->getHighX() > highX) {
				highX = m_curves[i]->getHighX();
			}
		}
	}
	
	m_xAxis.setOptimalLimits(lowX, highX);
}

void NewGraph::setOptimalYLimits() {
	double lowY = std::numeric_limits<double>::max();
	double highY = std::numeric_limits<double>::lowest();
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			if (m_curves[i]->getLowY(m_yAxis.getLogarithmic()) < lowY) {
				lowY = m_curves[i]->getLowY(m_yAxis.getLogarithmic());
			}
			if (m_curves[i]->getHighY() > highY) {
				highY = m_curves[i]->getHighY();
			}
		}
	}
	
	m_yAxis.setOptimalLimits(lowY, highY);
}

NewCurve* NewGraph::getCurve(ShowAsGraphInterface *object, int curveIndex /*= -1*/) {
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject() == object && curveIndex-- <= 0) {
			return m_curves[i];
		}
	}
	return NULL;
}

void NewGraph::removeAllCurves() {
	for (int i = 0; i < m_curves.size(); ++i) {
		delete m_curves[i];
	}
	m_curves.clear();
}

bool NewGraph::loadStylesFromSettings() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope,"QBLADE");

	if (settings.contains(QString("graphs/" + m_nameInSettings + "/type"))) {
		settings.beginGroup(QString("graphs/" + m_nameInSettings));
		
		setGraphType(static_cast<GraphType> (settings.value("type").toInt()));
		setGraphTypeMulti(static_cast<GraphType> (settings.value("typeMulti").toInt()));
		m_xAxisTitle = settings.value("xAxis").toString();
		m_yAxisTitle = settings.value("yAxis").toString();
		m_formerAxisTitles = settings.value("formerAxT").value<GraphTypeMap>();
		m_formerGraphTypeMulti = settings.value("formerMulti").value<GraphTypeMultiMap>();
		m_xAxis.setLogarithmic(settings.value("xLog").toBool());
		m_yAxis.setLogarithmic(settings.value("yLog").toBool());
		m_backgroundColor = settings.value("bgCol").value<QColor>();
		m_titleColor = settings.value("titCol").value<QColor>();
		m_tickColor = settings.value("tickCol").value<QColor>();
		m_titleFont = settings.value("titFont").value<QFont>();
		m_tickFont = settings.value("tickFont").value<QFont>();
		m_mainAxesPen = settings.value("mainAxPen").value<QPen>();
		m_xGridPen = settings.value("xGridPen").value<QPen>();
		m_yGridPen = settings.value("yGridPen").value<QPen>();
		m_borderColor = settings.value("bordCol").value<QColor>();
		m_borderWidth = settings.value("bordWid").toInt();
		m_noAutoResize = settings.value("noAutoResize").toBool();
		
		settings.endGroup();
		return true;
	} else {
		return false;
	}
}

void NewGraph::saveStylesToSettings() {
	if (m_nameInSettings != "__default") {  // do not save a default graph
		QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
		
		settings.beginGroup(QString("graphs/" + m_nameInSettings));

		settings.setValue("type", m_graphType);
		settings.setValue("typeMulti", m_graphTypeMulti);
		settings.setValue("xAxis", m_xAxisTitle);
		settings.setValue("yAxis", m_yAxisTitle);
		settings.setValue("formerAxT", QVariant::fromValue(m_formerAxisTitles));
		settings.setValue("formerMulti", QVariant::fromValue(m_formerGraphTypeMulti));
		settings.setValue("xLog", m_xAxis.getLogarithmic());
		settings.setValue("yLog", m_yAxis.getLogarithmic());
		settings.setValue("bgCol", m_backgroundColor);
		settings.setValue("titCol", m_titleColor);
		settings.setValue("tickCol", m_tickColor);
		settings.setValue("titFont", m_titleFont);
		settings.setValue("tickFont", m_tickFont);
		settings.setValue("mainAxPen", m_mainAxesPen);
		settings.setValue("xGridPen", m_xGridPen);
		settings.setValue("yGridPen", m_yGridPen);
		settings.setValue("bordCol", m_borderColor);
		settings.setValue("bordWid", m_borderWidth);
		settings.setValue("noAutoResize", m_noAutoResize);
		
		settings.endGroup();
	}
}

void NewGraph::reloadCurves() {
    removeAllCurves();
	m_curves = m_twoDInterface->prepareCurves(m_xAxisTitle, m_yAxisTitle, m_graphType, m_graphTypeMulti);
	setOptimalLimits(false);
}

void NewGraph::setShownVariables(QString xVariable, QString yVariable) {
	m_xAxisTitle = xVariable;
	m_yAxisTitle = yVariable;
	
	reloadCurves();
}

QStringList NewGraph::getAvailableVariables(bool xAxis) {
	return m_twoDInterface->getAvailableGraphVariables(xAxis);
}

void NewGraph::drawGrid(QPainter &painter) {
	painter.setClipRect(m_graphArea.adjusted(0, -1, 1, 0));  // adjust the clip so that the highLimits can be drawn
	
	/* draw main axes at zero or at the edge */
	painter.setPen(m_mainAxesPen);
	int axisPosition;
	if (m_xAxis.getLowLimit() <= 0 && m_xAxis.getHighLimit() >= 0) {
		axisPosition = mapXCoordinateToPixel(0);
	} else if (m_xAxis.getLowLimit() > 0) {
		axisPosition = m_graphArea.left();
	} else {
		axisPosition = m_graphArea.right();
	}
	painter.drawLine(axisPosition, m_graphArea.top(),axisPosition, m_graphArea.bottom());
	if (m_yAxis.getLowLimit() <= 0 && m_yAxis.getHighLimit() >= 0) {
		axisPosition = mapYCoordinateToPixel(0);
	} else if (m_yAxis.getLowLimit() > 0) {
		axisPosition = m_graphArea.bottom();
	} else {
		axisPosition = m_graphArea.top();
	}
	painter.drawLine(m_graphArea.left(), axisPosition, m_graphArea.right(),axisPosition);
	
	/* draw grid */
	painter.setPen(m_xGridPen);
	for (int i = 0; i < m_xAxis.getNumberOfTicks(); ++i) {
		if (m_xAxis.getTickValueAt(i) != 0) {
			painter.drawLine(mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)), m_graphArea.top(),
							 mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)), m_graphArea.bottom());
		}
	}
	painter.setPen(m_yGridPen);
	for (int i = 0; i < m_yAxis.getNumberOfTicks(); ++i) {
		if (m_yAxis.getTickValueAt(i) != 0) {
			painter.drawLine(m_graphArea.left(), mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)),
							 m_graphArea.right(), mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)));
		}
	}
}

void NewGraph::drawTicks(QPainter &painter) {
	painter.setClipRect(m_drawingArea);
	painter.setPen(m_tickColor);
	painter.setFont(m_tickFont);
	QFontMetrics tickMetrics (m_tickFont);
	
	/* draw x ticks */
	int stringWidth;
	int stringXPosition;
	const int stringYPosition = m_graphArea.bottom() + 1.25*tickMetrics.height();
	const int exponentYPosition = m_graphArea.bottom() + tickMetrics.height();
	QString currentString;
	for (int i = 0; i < m_xAxis.getNumberOfTicks(); ++i) {
		const int exponentWidth = tickMetrics.width(m_xAxis.getExponentString(i));
		currentString = m_xAxis.getTickStringAt(i);
		stringWidth = tickMetrics.width(currentString);
		if (m_xAxis.getUseExponent()) {
			stringXPosition = mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)) - (stringWidth + exponentWidth) / 2;
			if (stringXPosition + stringWidth + exponentWidth < 
									m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle)) {
				painter.drawText(stringXPosition, stringYPosition, currentString);
				painter.drawText(stringXPosition + stringWidth, exponentYPosition, m_xAxis.getExponentString(i));
			}
		} else {
			stringXPosition = mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)) - stringWidth / 2;
			if (stringXPosition + stringWidth < m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle)) {
				painter.drawText(stringXPosition, stringYPosition, currentString);
			}
		}
	}
	
	/* draw y ticks */
	const int pointPosition = (m_yAxis.getUseExponent() ?
								   m_drawingArea.left() + m_borderWidth + tickMetrics.width("-8") :
								   m_drawingArea.left() + m_borderWidth + tickMetrics.width("-888"));
	const int integerRight = m_drawingArea.left() + m_borderWidth + tickMetrics.width("-888");
	for (int i = 0; i < m_yAxis.getNumberOfTicks(); ++i) {
		int position, positionExponent;
		currentString = m_yAxis.getTickStringAt(i);
		const int pointIndex = currentString.indexOf('.');
		if (pointIndex != -1) {  // float or exponential value
			position = pointPosition - tickMetrics.width(currentString, pointIndex);
			positionExponent = pointPosition + tickMetrics.width(currentString.mid(pointIndex));
		} else {  // integer value or logarithmic scale
			position = integerRight - tickMetrics.width((currentString));
			positionExponent = integerRight;
		}
		
		painter.drawText(position,
						 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)) + tickMetrics.height()/4,
						 currentString);
		if (m_yAxis.getUseExponent()) {
			painter.drawText(positionExponent,
							 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)),
							 m_yAxis.getExponentString(i));
		}
	}
}

void NewGraph::drawCurves(QPainter &painter) {
	painter.setClipRect(m_graphArea);
//	painter.setRenderHint(QPainter::HighQualityAntialiasing);  // TODO think about this as an option?
	QPen pen;
	const int pointRadius = 2;
	
	QVector<QPointF> points, mappedPoints;
	for (int i = 0; i < m_curves.size(); ++i) {
		/* By leaving the following condition in the code, the need to reload curves when the state switches to
		 * notShown is not generated. */
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			pen = m_curves[i]->getAssociatedObject()->getPen(i, m_twoDInterface->getHighlightIndex(m_graphTypeMulti));
			painter.setPen(pen);
			painter.setBrush(pen.color());
			
			points = m_curves[i]->getAllPoints();
			mappedPoints.resize(points.size());
            int skipped = 0;
			for (int j = 0; j < points.size(); ++j) {  // some calculations here but no performance issues noticed yet
				if ((m_xAxis.getLogarithmic() && points[j].x() <= 0) || (m_yAxis.getLogarithmic() && points[j].y() <= 0)) {
					skipped++;  // don't take log of negativ values or zero
				} else {
                    mappedPoints[j-skipped].setX(mapXCoordinateToPixel(points[j].x()));
                    mappedPoints[j-skipped].setY(mapYCoordinateToPixel(points[j].y()));
				}
			}
			mappedPoints.resize(mappedPoints.size() - skipped);
			
			if (m_curves[i]->getAssociatedObject()->isDrawCurve()) {
				painter.drawPolyline(mappedPoints.data(), mappedPoints.size());
			}
			if (m_curves[i]->getAssociatedObject()->isDrawPoints()) {
//				pen = painter.pen();
				pen.setStyle(Qt::SolidLine);
				painter.setPen(pen);
				for (int j = 0; j < mappedPoints.size(); ++j) {
					painter.drawRect(QRect(mappedPoints[j].x()-pointRadius, mappedPoints[j].y()-pointRadius,
										   2*pointRadius, 2*pointRadius));
				}
			}
		}
	}
	
	/* draw the highlight dot */
	QPair<ShowAsGraphInterface*,int> highlightDot = m_twoDInterface->getHighlightDot(m_graphType);
	if (highlightDot.first != NULL && highlightDot.first->isShownInGraph()) {
		NewCurve *curve = getCurve(highlightDot.first, m_twoDInterface->getHighlightIndex(m_graphTypeMulti));
		if (curve) {  // an object might have no curve, e.g. when not yet simulated
			points = curve->getAllPoints();
			if (highlightDot.second >= 0 && highlightDot.second < points.size()) {
				pen = highlightDot.first->getPen(m_curves.indexOf(curve),
												 m_twoDInterface->getHighlightIndex(m_graphTypeMulti), true);
				pen.setStyle(Qt::SolidLine);
				painter.setPen(pen);
				painter.setBrush(pen.color());
				painter.drawEllipse(QPointF(mapXCoordinateToPixel(points[highlightDot.second].x()),
											mapYCoordinateToPixel(points[highlightDot.second].y())),
									pen.width()+3, pen.width()+3);
			}
		}
	}
}

void NewGraph::drawTitles(QPainter &painter) {
	painter.setClipRect(m_drawingArea);
	QFontMetrics tickMetrics (m_tickFont);
	QFontMetrics titleMetrics (m_titleFont);
	
	painter.setPen(m_tickColor);
	painter.setFont(m_tickFont);
	painter.drawText(m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle),
					 m_drawingArea.bottom() - m_borderWidth - m_gapWidth,
					 m_xAxisTitle);
	int xPosition = std::max ((m_drawingArea.left() + m_graphArea.left() - tickMetrics.width(m_yAxisTitle)) / 2,
							  m_drawingArea.left() + m_borderWidth);
	painter.drawText(xPosition,
					 m_drawingArea.top() + m_borderWidth + titleMetrics.height(),
					 m_yAxisTitle);
	
	painter.setPen(m_titleColor);
	painter.setFont(m_titleFont);
	xPosition = std::max (double(xPosition + tickMetrics.width(m_yAxisTitle) + 10),
						  m_drawingArea.left() + m_borderWidth + 0.35*m_drawingArea.width());
	painter.drawText(xPosition,
					 m_drawingArea.top() + m_borderWidth + titleMetrics.height(),
					 m_title);
}

void NewGraph::calculateGraphArea() {
	QFontMetrics tickMetrics (m_tickFont);
	QFontMetrics titleMetrics (m_titleFont);
	const int topSpace = m_borderWidth + titleMetrics.height() + m_gapWidth + 0.5*tickMetrics.height();
	const int bottomSpace = m_borderWidth + 1.25*tickMetrics.height() + m_gapWidth;
	const int leftSpace = m_borderWidth + tickMetrics.width("-8.88 10-8") + m_gapWidth;
	const int rightSpace = m_borderWidth;
	
	m_graphArea = QRect (m_drawingArea.left() + leftSpace,
						 m_drawingArea.top() + topSpace,
						 m_drawingArea.width() - leftSpace - rightSpace,
						 m_drawingArea.height() - topSpace - bottomSpace);
	
	m_xAxis.setAvailablePixels(m_graphArea.width());
	m_yAxis.setAvailablePixels(m_graphArea.height());
}

int NewGraph::mapXCoordinateToPixel(double xValue) {
	return m_graphArea.left() + m_xAxis.coordinateToPixel(xValue);
}

int NewGraph::mapYCoordinateToPixel(double yValue) {
	return m_graphArea.bottom() - m_yAxis.coordinateToPixel(yValue);
}

double NewGraph::mapPixelToXCoordinate(int xPixel) {
	return m_xAxis.pixelToCoordinate(xPixel - m_graphArea.left());
}

double NewGraph::mapPixelToYCoordinate(int yPixel) {
	return m_yAxis.pixelToCoordinate(m_graphArea.bottom() - yPixel);
}
