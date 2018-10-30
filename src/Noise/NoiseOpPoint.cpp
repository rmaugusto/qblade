#include "NoiseOpPoint.h"

#include <cmath>

#include "../Objects/OpPoint.h"


NoiseOpPoint::NoiseOpPoint(OpPoint *opPoint)
	: m_reynolds(-1), 
	  m_alpha(-1),
	  m_opPoint(opPoint)
{
}

NoiseOpPoint::NoiseOpPoint(double reynolds, double alpha) 
	: m_reynolds(reynolds),
	  m_alpha(alpha),
	  m_opPoint(NULL)
{
}

double NoiseOpPoint::getReynolds() {
	return (m_opPoint ? m_opPoint->Reynolds : m_reynolds);
}

double NoiseOpPoint::getAlphaDegree() {
	return (m_opPoint ? m_opPoint->Alpha : m_alpha);
}

double NoiseOpPoint::getAlphaDegreeAbsolute() {
	return fabs(getAlphaDegree());
}

int NoiseOpPoint::getNSide1() {
	return m_opPoint->nSide1;
}

int NoiseOpPoint::getNSide2() {
	return m_opPoint->nSide2;
}

double NoiseOpPoint::getXValue(int index, int topOrBot) {
	return (topOrBot == 1 ? m_opPoint->x1Values[index] : m_opPoint->x2Values[index]);	
}

double NoiseOpPoint::getDstrAt(int index,int topOrBot) {
	return (topOrBot == 1 ? m_opPoint->topDStar.second[index] : m_opPoint->botDStar.second[index]);	
}
