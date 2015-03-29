#include "NewGraph.h"

#include <cmath>
#include <limits>
#include <QPainter>
#include <QPoint>
#include <QVector>
#include <QFontMetrics>
#include <QDebug>
#include <QSettings>
#include <QDate>
#include <QTime>
#include <QPen>

#include "NewCurve.h"
#include "ShowAsGraphInterface.h"
#include "../Store.h"
#include "../XUnsteadyBEM/FASTSimulation.h"
#include "../QFem/BladeStructure.h"
#include "../XUnsteadyBEM/FASTModule.h"
#include "../XLLT/QLLTModule.h"
#include "../Noise/noisemodule.h"
#include "../XGlobals.h" //TODO remove this line when QLLTStore implemented


// the graph won't update itself when changes are made. The calling class have to take care about that.
NewGraph::NewGraph(QString nameInSettings, GraphType graphType, TwoDWidgetInterface *twoDInterface) {
	m_graphType = None;
	
	m_twoDInterface = twoDInterface;
	m_nameInSettings = nameInSettings;
	m_xAxis.setTickSizeFactor(2.5);
	m_yAxis.setTickSizeFactor(5);
	m_xAxis.setLimits(-1.1, 1.1);
	m_yAxis.setLimits(-1.1, 1.1);
	m_gapWidth = 3;
	m_borderGapWidth = 3;
	
	if (! loadStylesFromSettings()) {  // set the default style, if this graph is not found in the settings
		setGraphType(graphType);
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
	setOptimalLimits();
}

NewGraph::~NewGraph() {
	removeAllCurves ();
	saveStylesToSettings ();
}

void NewGraph::setGraphType(NewGraph::GraphType graphType) {
	if (m_graphType == graphType)
		return;  // nothing to do
	
	/* disconnect old type */
	switch (m_graphType) {
	case None:
		break;
	case FastSimulation:
		disconnect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
		disconnect(g_fastModule, SIGNAL(updateFastGraphs()), this, SLOT(updateGraph()));
		break;
	case BladeGraph:
		disconnect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
		disconnect(g_fastModule, SIGNAL(updateBladeGraphs()), this, SLOT(updateGraph()));
		break;
    case QFEMSimulation:
        disconnect(&g_bladeStructureStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    case QLLTSimulation:
        disconnect(&g_QLLTHAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    case NoiseSimulationGraph:
        disconnect(&g_NoiseSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    }
	
	m_graphType = graphType;
	
	/* connect new type */
	switch (m_graphType) {
	case None:
		break;
	case FastSimulation:
		connect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
		connect(g_fastModule, SIGNAL(updateFastGraphs()), this, SLOT(updateGraph()));
		break;
	case BladeGraph:
		connect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
		connect(g_fastModule, SIGNAL(updateBladeGraphs()), this, SLOT(updateGraph()));
		break;
    case QFEMSimulation:
        connect(&g_bladeStructureStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    case QLLTSimulation:
        connect(&g_QLLTHAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    case NoiseSimulationGraph:
        connect(&g_NoiseSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateGraph()));
        break;
    }
}

void NewGraph::drawGraph(QPainter &painter) {
	painter.setClipRect(m_drawingArea);
	painter.fillRect(m_drawingArea, m_borderColor);
	const int nettoBorder = m_borderWidth - m_borderGapWidth;
	painter.fillRect(m_drawingArea.adjusted(nettoBorder, nettoBorder, -nettoBorder, -nettoBorder), m_backgroundColor);
	
	drawTicks (painter);
	drawGrid (painter);
	drawTitles (painter);
	drawCurves (painter);
}

void NewGraph::exportGraph(QString fileName, int fileType) {

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

	QFile file (fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream (&file);

		int numberOfPoints = 0;
		QVector<QPointF> points[m_curves.size()];  // preparation of this array is of vital importance for performance
        stream << "Export File Created with QBlade v0.8 on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << endl;
		for (int i = 0; i < m_curves.size(); ++i) {
			if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
				stream << QString("  %1").arg(m_curves[i]->getAssociatedObject()->getObjectName(), 30);
				if (m_curves[i]->getNumberOfPoints() > numberOfPoints) {  // find maximal number of points
					numberOfPoints = m_curves[i]->getNumberOfPoints();
				}
				points[i] = m_curves[i]->getAllPoints();
			}
		}
		stream << endl;
		for (int i = 0; i < m_curves.size(); ++i) {
			if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
                if (fileType == 1) stream << QString(" %1 %2").arg(m_xAxisTitle, 15).arg(m_yAxisTitle, 15);
                if (fileType == 2) stream << QString("%1;%2;").arg(m_xAxisTitle, 15).arg(m_yAxisTitle, 15);
			}
		}
		stream << endl;
		
		for (int i = 0; i < numberOfPoints; ++i) {  // write the values
			for (int j = 0; j < m_curves.size(); ++j) {
				if (m_curves[j]->getAssociatedObject()->isShownInGraph()) {				
					if (i < m_curves[j]->getNumberOfPoints()) {
	//					stream << QString(" %1").arg(m_curves[j]->getAllPoints()[i].x(), 15, 'e', 5) <<  // NM too bad performance
	//							  QString(" %1").arg(m_curves[j]->getAllPoints()[i].y(), 15, 'e', 5);
                        if (fileType == 1){ stream << QString(" %1").arg(points[j][i].x(), 15, 'e', 5) <<
                                  QString(" %1").arg(points[j][i].y(), 15, 'e', 5);}
                        if (fileType == 2){ stream << QString(" %1;").arg(points[j][i].x(), 15, 'e', 5) <<
                                                      QString(" %1;").arg(points[j][i].y(), 15, 'e', 5);}
					} else {
                        if (fileType == 1) stream << QString().fill(' ', 32);
                        if (fileType == 2) stream << QString(";;");
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
	const double dx = mapPixelToXCoordinate(from.x()) - mapPixelToXCoordinate(to.x());
	const double dy = -1 * (mapPixelToYCoordinate(from.y()) - mapPixelToYCoordinate(to.y()));
	m_xAxis.moveLimits(dx, dx);
	m_yAxis.moveLimits(dy, dy);
}

void NewGraph::zoomX(double zoom) {
	const double xMiddle = (m_xAxis.getLowLimit() + m_xAxis.getHighLimit()) / 2.0;
	m_xAxis.setLimits(xMiddle + (m_xAxis.getLowLimit()-xMiddle)*zoom,
					  xMiddle + (m_xAxis.getHighLimit()-xMiddle)*zoom);
	
	calculatePerPixel ();
}

void NewGraph::zoomY(double zoom) {
	const double yMiddle = (m_yAxis.getLowLimit() + m_yAxis.getHighLimit()) / 2.0;
	m_yAxis.setLimits(yMiddle + (m_yAxis.getLowLimit()-yMiddle)*zoom,
					  yMiddle + (m_yAxis.getHighLimit()-yMiddle)*zoom);
	
	calculatePerPixel ();
}

void NewGraph::setOptimalLimits() {
	setOptimalXLimits();
	setOptimalYLimits();
}

void NewGraph::setOptimalXLimits() {
	double lowX = std::numeric_limits<double>::max();
	double highX = -std::numeric_limits<double>::max();
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			if (m_curves[i]->getLowX() < lowX) {
				lowX = m_curves[i]->getLowX();
			}
			if (m_curves[i]->getHighX() > highX) {
				highX = m_curves[i]->getHighX();
			}
		}
	}
	
	if (lowX == std::numeric_limits<double>::max()) {
		lowX = -1;
	}
	if (highX == -std::numeric_limits<double>::max()) {
		highX = 1;
	}
	
	double bufferX = (highX - lowX) * 0.05;  // for a better look only
	if (fabs(bufferX) < 0.000000001) {  // the limits must never be 0 and 0
		bufferX = 1e-5;
	}
	m_xAxis.setLimits(lowX-bufferX, highX+bufferX);
	
	calculatePerPixel ();
}

void NewGraph::setOptimalYLimits() {
	double lowY = std::numeric_limits<double>::max();
	double highY = -std::numeric_limits<double>::max();
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			if (m_curves[i]->getLowY() < lowY) {
				lowY = m_curves[i]->getLowY();
			}
			if (m_curves[i]->getHighY() > highY) {
				highY = m_curves[i]->getHighY();
			}
		}
	}
	
	if (lowY == std::numeric_limits<double>::max()) {
		lowY = -1;
	}
	if (highY == -std::numeric_limits<double>::max()) {
		highY = 1;
	}
	
	double bufferY = (highY - lowY) * 0.05;  // for a better look only
	if (fabs(bufferY) < 0.000000001) {  // the limits must never be 0 and 0
		bufferY = 1e-5;
	}
	m_yAxis.setLimits(lowY-bufferY, highY+bufferY);
	
	calculatePerPixel ();
}

void NewGraph::removeAllCurves() {
	for (int i = 0; i < m_curves.size(); ++i) {
		delete m_curves[i];
	}
	m_curves.clear();
}

bool NewGraph::loadStylesFromSettings() {
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#else
	QSettings settings(QSettings::IniFormat,QSettings::UserScope,"QBLADE");
#endif
	
	if (settings.contains(QString("graphs/" + m_nameInSettings + "/type"))) {
		settings.beginGroup(QString("graphs/" + m_nameInSettings));
		
		setGraphType(static_cast<GraphType> (settings.value("type").toInt()));
		m_xAxisTitle = settings.value("xAxis").toString();
		m_yAxisTitle = settings.value("yAxis").toString();
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
#ifdef Q_WS_MAC
		QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#else
		QSettings settings(QSettings::IniFormat,QSettings::UserScope,"QBLADE");
#endif
		
		settings.beginGroup(QString("graphs/" + m_nameInSettings));

		settings.setValue("type", m_graphType);
		settings.setValue("xAxis", m_xAxisTitle);
		settings.setValue("yAxis", m_yAxisTitle);
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

	NewCurve *curve;
	switch (m_graphType) {
    case None:
		break;
    case FastSimulation:
		for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
			curve = g_FASTSimulationStore.at(i)->newCurve(m_xAxisTitle, m_yAxisTitle, m_graphType);
			if (curve) {
				m_curves.append(curve);
			}
        }
		break;
    case BladeGraph:
		for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
			curve = g_FASTSimulationStore.at(i)->newCurve(m_xAxisTitle, m_yAxisTitle, m_graphType);
			if (curve) {
				m_curves.append(curve);
			}
        }		
		break;
    case QFEMSimulation:
        for (int i = 0; i < g_bladeStructureStore.size(); ++i) {
            curve = g_bladeStructureStore.at(i)->newCurve(m_xAxisTitle, m_yAxisTitle, m_graphType);
            if (curve) {
                m_curves.append(curve);
            }
		}		
		break;
    case QLLTSimulation:
        for (int i = 0; i < g_QLLTHAWTSimulationStore.size(); ++i) {
            curve = g_QLLTHAWTSimulationStore.at(i)->newCurve(m_xAxisTitle, m_yAxisTitle, m_graphType);
            if (curve) {
                m_curves.append(curve);
            }
        }
        break;
    case NoiseSimulationGraph:
        for (int i = 0; i < g_NoiseSimulationStore.size(); ++i) {
            for (int j = 0; j < g_NoiseSimulationStore.at(i)->Calculation()->NoiseParam()->OpPoints().size(); ++j) {
                curve = g_NoiseSimulationStore.at(i)->newCurve(m_xAxisTitle, m_yAxisTitle, m_graphType,j);
                if (curve) {
                    m_curves.append(curve);
                }
            }
        }

        break;
    }
}

void NewGraph::updateGraph() {
	reloadCurves();
	if (!m_noAutoResize) {
		setOptimalLimits();
	}
	m_twoDInterface->update();
}

void NewGraph::setShownVariables(QString xVariable, QString yVariable) {
	m_xAxisTitle = xVariable;
	m_yAxisTitle = yVariable;
	
	reloadCurves();
}

QStringList NewGraph::getAvailableVariables() {
	return m_twoDInterface->getAvailableGraphVariables();
}

void NewGraph::drawGrid(QPainter &painter) {
	painter.setClipRect(m_graphArea.adjusted(0, -1, 1, 0));  // adjust the clip for that the highLimits can be drawn
	
	/* draw main axes at zero or at the edge */
	painter.setPen(m_mainAxesPen);
	int axisPosition;
	if (m_xAxis.getLowLimit() < 0 && m_xAxis.getHighLimit() > 0) {
		axisPosition = mapXCoordinateToPixel(0);
	} else if (m_xAxis.getLowLimit() > 0) {
		axisPosition = m_graphArea.left();
	} else {
		axisPosition = m_graphArea.right();
	}
	painter.drawLine(axisPosition, m_graphArea.top(),axisPosition, m_graphArea.bottom());
	if (m_yAxis.getLowLimit() < 0 && m_yAxis.getHighLimit() > 0) {
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
	const int exponentWidth = tickMetrics.width(m_xAxis.getExponentString());
	const int stringYPosition = m_graphArea.bottom() + m_gapWidth + 1.25*tickMetrics.height();
	const int exponentYPosition = m_graphArea.bottom() + m_gapWidth + tickMetrics.height();
	QString currentString;
	for (int i = 0; i < m_xAxis.getNumberOfTicks(); ++i) {
		currentString = m_xAxis.getTickStringAt(i);
		stringWidth = tickMetrics.width(currentString);
		if (m_xAxis.getUseExponent()) {
			stringXPosition = mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)) - (stringWidth + exponentWidth) / 2;
			if (stringXPosition + stringWidth + exponentWidth < 
									m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle)) {
				painter.drawText(stringXPosition, stringYPosition, currentString);
				painter.drawText(stringXPosition + stringWidth, exponentYPosition, m_xAxis.getExponentString());
			}
		} else {
			stringXPosition = mapXCoordinateToPixel(m_xAxis.getTickValueAt(i)) - stringWidth / 2;
			if (stringXPosition + stringWidth < m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle)) {
				painter.drawText(stringXPosition, stringYPosition, currentString);
			}
		}
	}
	
	/* draw y ticks */
	const int pointPosition = m_drawingArea.left() + m_borderWidth + tickMetrics.width("-8");
	const int pointPositionNoExponent = m_drawingArea.left() + m_borderWidth + tickMetrics.width("-888");
	const int integerRight = m_drawingArea.left() + m_borderWidth + tickMetrics.width("-888");
	int pointIndex;
	for (int i = 0; i < m_yAxis.getNumberOfTicks(); ++i) {
		currentString = m_yAxis.getTickStringAt(i);
		pointIndex = currentString.indexOf('.');
		if (pointIndex != -1) {
			if (m_yAxis.getUseExponent()) {
				painter.drawText(pointPosition - tickMetrics.width(currentString, pointIndex),
								 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)) + tickMetrics.height()/4,
								 currentString);
				painter.drawText(pointPosition + tickMetrics.width(currentString.mid(pointIndex)),
								 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)),
								 m_yAxis.getExponentString());
			} else {
				painter.drawText(pointPositionNoExponent - tickMetrics.width(currentString, pointIndex),
								 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)) + tickMetrics.height()/4,
								 currentString);
			}
		} else {
			painter.drawText(integerRight - tickMetrics.width((currentString)),
							 mapYCoordinateToPixel(m_yAxis.getTickValueAt(i)) + tickMetrics.height()/4,
							 currentString);			
		}
	}
}

void NewGraph::drawCurves(QPainter &painter) {
	painter.setClipRect(m_graphArea);
	QPen pen;
	
	const int pointRadius = 2;
	
	QVector<QPointF> points, mappedPoints;
	for (int i = 0; i < m_curves.size(); ++i) {
		if (m_curves[i]->getAssociatedObject()->isShownInGraph()) {
			painter.setPen(m_curves[i]->getAssociatedObject()->getPen());
			painter.setBrush(m_curves[i]->getAssociatedObject()->getPen().color());
			
			points = m_curves[i]->getAllPoints();
			mappedPoints.resize(points.size());
			for (int j = 0; j < points.size(); ++j) {
				mappedPoints[j].setX(mapXCoordinateToPixel(points[j].x()));
				mappedPoints[j].setY(mapYCoordinateToPixel(points[j].y()));
			}
			
			if (m_curves[i]->getAssociatedObject()->isDrawCurve()) {
				painter.drawPolyline(mappedPoints.data(), mappedPoints.size());
			}
			if (m_curves[i]->getAssociatedObject()->isDrawPoints()) {
				for (int j = 0; j < mappedPoints.size(); ++j) {
					painter.drawRect(QRect(mappedPoints[j].x()-pointRadius, mappedPoints[j].y()-pointRadius,
										   2*pointRadius, 2*pointRadius));
				}
			}
			
			/* draw the bog dot which marks the time in a FastGraph */
			if (m_graphType == FastSimulation &&
				(m_xAxisTitle == "Time" || m_yAxisTitle == "Time") &&
				g_fastModule->getShownFASTSimulation()->hasAeroDynResults() &&
				m_curves[i]->getAssociatedObject() == g_fastModule->getShownFASTSimulation())
			{
				const float time = g_fastModule->getShownFASTSimulation()->getShownTime();
				const bool onXAxis = (m_xAxisTitle == "Time");
				for (int j = 0; j < points.size(); ++j) {
					if ((onXAxis?points[j].x():points[j].y()) == time) {
						painter.drawEllipse(QPointF(mapXCoordinateToPixel(points[j].x()),
													mapYCoordinateToPixel(points[j].y())),
											pen.width()+3, pen.width()+3);
						break;
					}
				}
			}
		}
	}
}

void NewGraph::drawTitles(QPainter &painter) {
	painter.setClipRect(m_drawingArea);
	QFontMetrics tickMetrics (m_tickFont);
	QFontMetrics titleMetrics (m_titleFont);
	
	painter.setPen(m_titleColor);
	painter.setFont(m_titleFont);
	painter.drawText(m_drawingArea.left() + m_borderWidth + 0.35*m_drawingArea.width(),
					 m_drawingArea.top() + m_borderWidth + titleMetrics.height(),
					 m_title);
	
	painter.setPen(m_tickColor);
	painter.setFont(m_tickFont);
	painter.drawText(m_drawingArea.right() - m_borderWidth - tickMetrics.width(m_xAxisTitle),
					 m_drawingArea.bottom() - m_borderWidth,
					 m_xAxisTitle);
	int xPosition = std::max ((m_drawingArea.left() + m_graphArea.left() - tickMetrics.width(m_yAxisTitle)) / 2,
							  m_drawingArea.left() + m_borderWidth);
	painter.drawText(xPosition,
					 m_drawingArea.top() + m_borderWidth + titleMetrics.height(),
					 m_yAxisTitle);
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
	
	calculatePerPixel ();
}

int NewGraph::mapXCoordinateToPixel(double xValue) {
	return m_graphArea.left() + round ((xValue-m_xAxis.getLowLimit())/m_xPerPixel);
}

int NewGraph::mapYCoordinateToPixel(double yValue) {
	return m_graphArea.bottom() - round ((yValue-m_yAxis.getLowLimit())/m_yPerPixel);
}

double NewGraph::mapPixelToXCoordinate(int xPixel) {
	return (xPixel - m_graphArea.left()) * m_xPerPixel;
}

double NewGraph::mapPixelToYCoordinate(int yPixel) {
	return (yPixel + m_graphArea.bottom()) * m_yPerPixel;
}

void NewGraph::calculatePerPixel () {
	m_xPerPixel = (m_xAxis.getHighLimit() - m_xAxis.getLowLimit()) / m_graphArea.width();
	m_yPerPixel = (m_yAxis.getHighLimit() - m_yAxis.getLowLimit()) / m_graphArea.height();
}
