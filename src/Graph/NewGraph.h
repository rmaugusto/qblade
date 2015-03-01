#ifndef NEWGRAPH_H
#define NEWGRAPH_H

#include <QObject>
#include <QRect>
#include <QVector>
#include <QFont>
#include <QColor>
#include <QPen>
#include <QStringList>
class QPainter;
class QPoint;

#include "Axis.h"
//#include "ShowAsGraphInterface.h"
class TwoDWidgetInterface;
class TwoDWidgetEventInterface;
class NewCurve;


class NewGraph : public QObject
{
	Q_OBJECT
	
public:
    enum GraphType {None, FastSimulation, BladeGraph, QFEMSimulation, QLLTSimulation, NoiseSimulationGraph};
	
	NewGraph(QString nameInSettings, GraphType graphType, TwoDWidgetInterface *twoDInterface);
	~NewGraph();
	
	void setGraphType (GraphType graphType);
	void setDrawingArea (QRect drawingArea);
	void setTitle (QString title) { m_title = title; }
	void setShownVariables (QString xVariable, QString yVariable);
	void setBackgroundColor (QColor color) { m_backgroundColor = color; }
	void setBorderColor (QColor color) { m_borderColor = color; }
	void setTickFont (QFont font) { m_tickFont = font; calculateGraphArea(); }
	void setTitleFont (QFont font) { m_titleFont = font; calculateGraphArea(); }
	void setMainAxesPen (QPen pen) { m_mainAxesPen = pen; }
	void setXGridPen (QPen pen) { m_xGridPen = pen; }
	void setYGridPen (QPen pen) { m_yGridPen = pen; }
	void setTitleColor (QColor color) { m_titleColor = color; }
	void setTickColor (QColor color) { m_tickColor = color; }
	void setBorderWidth (int width) { m_borderWidth = width + m_borderGapWidth; }
	void setXLimits (double lowLimit, double highLimit)	{ m_xAxis.setLimits(lowLimit, highLimit); calculatePerPixel(); }
	void setYLimits (double lowLimit, double highLimit) { m_yAxis.setLimits(lowLimit, highLimit); calculatePerPixel(); }
	void setXTickSize (double tickSize) { m_xAxis.setTickSize(tickSize); }
	void setYTickSize (double tickSize) { m_yAxis.setTickSize(tickSize); }
	void setNoAutoResize (bool autoResize) { m_noAutoResize = autoResize; }
	
	GraphType getGraphType () { return m_graphType; }
	QStringList getAvailableVariables ();
	QString getTitle () { return m_title; }
	QString getShownXVariable () { return m_xAxisTitle; }
	QString getShownYVariable () { return m_yAxisTitle; }
	QColor getBackgroundColor () { return m_backgroundColor; }
	QColor getBorderColor () { return m_borderColor; }
	QFont getTickFont () { return m_tickFont; }
	QFont getTitleFont () { return m_titleFont; }
	QPen getMainAxesPen () { return m_mainAxesPen; }
	QPen getXGridPen () { return m_xGridPen; }
	QPen getYGridPen () { return m_yGridPen; }
	QColor getTitleColor () { return m_titleColor; }
	QColor getTickColor () { return m_tickColor; }
	int getBorderWidth () { return m_borderWidth - m_borderGapWidth; }
	double getXLowLimit () { return m_xAxis.getLowLimit(); }
	double getXHighLimit () { return m_xAxis.getHighLimit(); }
	double getYLowLimit () { return m_yAxis.getLowLimit(); }
	double getYHighLimit () { return m_yAxis.getHighLimit(); }
	double getXTickSize () { return m_xAxis.getTickSize(); }
	double getYTickSize () { return m_yAxis.getTickSize(); }
	bool getNoAutoResize () { return m_noAutoResize; }

	void reloadCurves ();
	void drawGraph (QPainter &painter);
    void exportGraph (QString fileName, int fileType);
	bool contains (QPoint point);
	void translate (QPoint from, QPoint to);
	void zoomX (double zoom);
	void zoomY (double zoom);
	void setOptimalLimits();
	void setOptimalXLimits();
	void setOptimalYLimits();
	
public slots:
	void updateGraph ();

private:
	void drawGrid (QPainter &painter);
	void drawTicks (QPainter &painter);
	void drawCurves (QPainter &painter);
	void drawTitles (QPainter &painter);
	void calculateGraphArea ();
	int mapXCoordinateToPixel (double xValue);
	int mapYCoordinateToPixel (double yValue);
	double mapPixelToXCoordinate (int xPixel);
	double mapPixelToYCoordinate (int yPixel);
	void calculatePerPixel ();
	void removeAllCurves ();
	bool loadStylesFromSettings ();
	void saveStylesToSettings ();
	
private:
	GraphType m_graphType;
	TwoDWidgetInterface *m_twoDInterface;
	QRect m_drawingArea;  // the area for title, tick, grid and graph
	QRect m_graphArea;  // only grid and graph
	QString m_title, m_xAxisTitle, m_yAxisTitle;
	Axis m_xAxis, m_yAxis;
	double m_xPerPixel, m_yPerPixel;
	QList<NewCurve*> m_curves;
	QString m_nameInSettings;
	
	/* styles */
	QColor m_backgroundColor, m_borderColor, m_titleColor, m_tickColor;
	QFont m_tickFont, m_titleFont;
	QPen m_mainAxesPen, m_xGridPen, m_yGridPen;
	int m_borderWidth;  // the whole border. User set + borderGap
	int m_borderGapWidth;  // a part of the border with the background color. Simply looks better like this
	int m_gapWidth;  // gap between the graphArea and the labels
	bool m_noAutoResize;  // if false the graph never performes the resize action automaticly
};

#endif // NEWGRAPH_H
