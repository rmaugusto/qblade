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
	void setAllPoints (T *xValues, T *yValues, int dimension);

	ShowAsGraphInterface* getAssociatedObject () { return m_associatedObject; }
	QVector<QPointF> getAllPoints () { return m_points; }
	int getNumberOfPoints () { return m_points.size(); }
	float getLowX () { return m_lowX; }
	float getHighX () { return m_highX; }
	float getLowY () { return m_lowY; }
	float getHighY () { return m_highY; }
	
private:
	void removeAllPoints ();
	
	ShowAsGraphInterface *m_associatedObject;
	QVector<QPointF> m_points;
	float m_lowX, m_highX, m_lowY, m_highY;  // the extreme values of the curve
};

#endif // NEWCURVE_H
