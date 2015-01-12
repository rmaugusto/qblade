#include "NewCurve.h"

#include <limits>
#include <QDebug>


NewCurve::NewCurve(ShowAsGraphInterface *associatedObject) {
	removeAllPoints();
	m_associatedObject = associatedObject;
}

void NewCurve::addPoint(const double xValue, const double yValue) {
	m_points.append(QPointF(xValue, yValue));
	if (xValue < m_lowX) {
		m_lowX = xValue;
	}
	if (xValue > m_highX) {
		m_highX = xValue;
	}
	if (yValue < m_lowY) {
		m_lowY = yValue;
	}
	if (yValue > m_highY) {
		m_highY = yValue;
	}
}

template <class T>
void NewCurve::setAllPoints(T *xValues, T *yValues, int dimension) {
	removeAllPoints();
	
	for (int i = 0; i < dimension; ++i) {
		addPoint(xValues[i], yValues[i]);
	}
}
template void NewCurve::setAllPoints<float> (float *xValues, float *yValues, int dimension);
template void NewCurve::setAllPoints<double> (double *xValues, double *yValues, int dimension);

void NewCurve::removeAllPoints() {
	m_points.clear();
	m_lowX = std::numeric_limits<float>::max();
	m_highX = -std::numeric_limits<float>::max();
	m_lowY = std::numeric_limits<float>::max();
	m_highY = -std::numeric_limits<float>::max();	
}
