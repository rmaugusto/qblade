#include "Axis.h"

#include <cmath>
#include <algorithm>
#include <QDebug>


void Axis::setTickSizeFactor(double tickSizeFactor) {
	m_tickSizeFactor = tickSizeFactor;
}

QString Axis::getTickStringAt(int position) {
	if (m_useExponent) {
		return QString("%1 10").arg(m_tick[position]/pow(10, m_exponent), 0, 'f', 2);
	} else {
		return QString("%1").arg(m_tick[position], 0, 'f', m_neededPrecision);
	}
}

void Axis::setLimits(double min, double max) {
	m_lowLimit = min;
	m_highLimit = max;
	
	calculateTickSize();
	calculateTicks();
}

void Axis::setTickSize(double tickSize) {
	m_tickSize = tickSize;
	m_tickExponent = calculateExponent(m_tickSize);
	
	calculateTicks();	
}

void Axis::moveLimits(double deltaMin, double deltaMax) {
	m_lowLimit = m_lowLimit+deltaMin;
	m_highLimit = m_highLimit+deltaMax;
	
	calculateTicks();
}

void Axis::calculateTicks() {
	m_tick.clear();	
	double currentTick = m_tickSize * ceil(m_lowLimit/m_tickSize);  // calculate lowest shown number
	while (currentTick <= m_highLimit) {  // calculate all shown numbers
		if (fabs(currentTick) < 0.000000001) {  // to avoid -0
			m_tick.append(0);
		} else {
			m_tick.append(currentTick);
		}
		currentTick += m_tickSize;
	}
	
	setBestExponent();
	calculateNeededPrecision();
}

void Axis::calculateTickSize() {
	double targetedTickSize = (m_highLimit - m_lowLimit) / m_tickSizeFactor;
	m_tickExponent = calculateExponent(targetedTickSize);

	double normalized = targetedTickSize / pow(10, m_tickExponent);
	if (normalized < 2) {
		m_tickSize = pow(10, m_tickExponent);
	} else if (normalized < 5) {
		m_tickSize = 2.0 * pow(10, m_tickExponent);
	} else {
		m_tickSize = 5.0 * pow(10, m_tickExponent);	
	}
}

int Axis::calculateExponent(double value) {
	value = (fabs(value) < 0.000000001) ? 0 : value;  // catch rounding errors
	int exponent = (value == 0) ? 0 : log10 (fabs(value)+0.000000001);  // log10 can't handle 0; catch rounding errors	
	if (fabs(value) < 1 && value != 0) {
		exponent--;  // casting to int makes log10 give wrong result for negativ exponents
	}
	return exponent;
}

void Axis::setBestExponent() {
	if (! m_tick.isEmpty()) {  // can be empty, if tickSize is smaller than precision
		int exponent1 = calculateExponent(m_tick.first());
		int exponent2 = calculateExponent(m_tick.last());
		if (fabs(m_tick.first()) < 0.000000001) {  // NM for that value 0 doesn't set exponent allways to 0
			m_exponent = exponent2;
		} else if (fabs(m_tick.last()) < 0.000000001) {
			m_exponent = exponent1;
		} else {
			m_exponent = std::max(exponent1, exponent2);
		}
		
		m_useExponent = (m_exponent >= 3 || m_exponent <= -2);
		if (m_useExponent) {
			m_exponentString = QString("%1").arg(m_exponent);
		} else {
			m_exponentString = "";
		}
	}
}

void Axis::calculateNeededPrecision() {
	if (int(m_tickSize*1000) % 10 != 0 ) {
		m_neededPrecision = 3;
	} else if (int(m_tickSize*100) % 10 != 0 ) {
		m_neededPrecision = 2;
	} else if (int(m_tickSize*10) % 10 != 0 ) {
		m_neededPrecision = 1;
	} else if (fabs(m_tickSize) >= 1){
		m_neededPrecision = 0;
	} else {  // this case occures for small tickSizes like 0.0005
		m_neededPrecision = 3;
	}
}
