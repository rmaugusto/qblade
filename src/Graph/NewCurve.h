#ifndef NEWCURVE_H
#define NEWCURVE_H

#include <QVector>
#include <QColor>
#include <QString>
#include <QPoint>

class ShowAsGraphInterface;


class NewCurve
{
public:
	NewCurve(ShowAsGraphInterface *associatedObject);
	
	void addPoint (const double xValue, const double yValue);
	template <class T>
	void setAllPoints (const T *xValues, const T *yValues, const int dimension);

	ShowAsGraphInterface* getAssociatedObject () { return m_associatedObject; }
	QVector<QPointF> getAllPoints () { return m_points; }
	int getNumberOfPoints () { return m_points.size(); }
	double getLowX (bool greaterZero = false) { return (greaterZero ? m_lowXGreaterZero : m_lowX); }
	double getHighX () { return m_highX; }
	double getLowY (bool greaterZero = false) { return (greaterZero ? m_lowYGreaterZero : m_lowY); }
	double getHighY () { return m_highY; }
	
private:
	void removeAllPoints ();
	
	ShowAsGraphInterface *m_associatedObject;
	QVector<QPointF> m_points;
	double m_lowX, m_highX, m_lowY, m_highY;  // the extreme values of the curve
	double m_lowXGreaterZero, m_lowYGreaterZero;  // the lowest value that is greater zero
};

#endif // NEWCURVE_H
