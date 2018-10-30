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
	if (xValue > 0 && xValue < m_lowXGreaterZero) {
		m_lowXGreaterZero = xValue;
	}
	if (xValue > m_highX) {
		m_highX = xValue;
	}
	if (yValue < m_lowY) {
		m_lowY = yValue;
	}
	if (yValue > 0 && yValue < m_lowYGreaterZero) {
		m_lowYGreaterZero = yValue;
	}
	if (yValue > m_highY) {
		m_highY = yValue;
	}
}

template <class T>
void NewCurve::setAllPoints(const T *xValues, const T *yValues, const int dimension) {
	removeAllPoints();
	
	for (int i = 0; i < dimension; ++i) {
		addPoint(xValues[i], yValues[i]);
	}
}
template void NewCurve::setAllPoints<float> (const float *xValues, const float *yValues, const int dimension);
template void NewCurve::setAllPoints<double> (const double *xValues, const double *yValues, const int dimension);

void NewCurve::removeAllPoints() {
	m_points.clear();
	m_lowX = std::numeric_limits<double>::max();
	m_lowXGreaterZero = std::numeric_limits<double>::max();
	m_highX = std::numeric_limits<double>::lowest();
	m_lowY = std::numeric_limits<double>::max();
	m_lowYGreaterZero = std::numeric_limits<double>::max();
	m_highY = std::numeric_limits<double>::lowest();
}
