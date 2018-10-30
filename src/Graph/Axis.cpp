#include "Axis.h"

#include <limits>
#include <cmath>
#include <algorithm>
#include <QDebug>


const double Axis::PRECISION (10e-12);

Axis::Axis(double lowLimit, double highLimit, double tickSizeFactor, bool logarithmic) {
	m_tickSizeFactor = tickSizeFactor;
    m_logarithmic = logarithmic;
	setLimits(lowLimit, highLimit);
}

void Axis::setLogarithmic(bool logarithmic) {
	m_logarithmic = logarithmic;
	setLimits(m_lowLimit, m_highLimit);  // recalculate the ticks and so on
}

QString Axis::getTickStringAt(int position) {
	if (m_logarithmic) {
		QString tick = QString::number(m_tick[position], 'e', 1);
		return (tick.startsWith('1') ? QString("10") : QString(""));
	} else if (m_useExponent) {
		QString string = (m_tick[position] == 0 ? "%1" : "%1 10");
		return string.arg(m_tick[position]/pow(10, m_exponent), 0, 'f', 2);
	} else {
		return QString("%1").arg(m_tick[position], 0, 'f', m_neededPrecision);
	}
}

bool Axis::getUseExponent() {
	return (m_logarithmic ? true : m_useExponent);
}

QString Axis::getExponentString(int position) {
	if (m_logarithmic) {
		QString tick = QString::number(m_tick[position], 'e', 1);
		return (tick.startsWith('1') ? QString::number(calculateExponent(m_tick[position])) : QString(""));		
	} else {
		return (m_tick[position] == 0 ? "" : m_exponentString);
	}
}

void Axis::setAvailablePixels(int availablePixels) {
	m_availablePixels = availablePixels;
	calculatePerPixel();
}

void Axis::setLimits(double min, double max) {
	if (max < min || max-min < PRECISION)  // refuse nonsense
		return;
	
	if (m_logarithmic) {
		m_lowLimit = (min <= PRECISION*10 ? PRECISION*10 : min);  // no values lesser equal 0 allowed
		m_highLimit = (max <= PRECISION*10 ? PRECISION*10 : max);
		calculateTicks();
	} else {
		m_lowLimit = min;
		m_highLimit = max;
		
		calculateTickSize();
		calculateTicks();
	}
	
	calculatePerPixel();
}

void Axis::setOptimalLimits(double low, double high) {
	if (m_logarithmic) {
		if (low == std::numeric_limits<double>::max()) {
			low = 1;
			high = 1000;
		}
		setLimits(low*0.9, high*1.1);  // multiply some small factor for a better look
	} else {
		if (low == std::numeric_limits<double>::max()) {
			low = -1;
			high = 1;
		}
		const double buffer = (high - low == 0) ? 0.05 : (high - low) * 0.05;
		setLimits(low-buffer, high+buffer);  // add some small buffer for a better look
	}
}

void Axis::setTickSize(double tickSize) {
	if (m_logarithmic) {
		// nothing to do
	} else {
		m_tickSize = tickSize;
		m_tickExponent = calculateExponent(m_tickSize);
		
		calculateTicks();	
	}
}

void Axis::moveLimits(int pixels) {
	setLimits(pixelToCoordinate(pixels), pixelToCoordinate(pixels+m_availablePixels));
}

double Axis::pixelToCoordinate(int pixel) {
	if (m_logarithmic) {
		return pow(10, log10(m_lowLimit) + pixel * m_perPixel);
	} else {
		return m_lowLimit + pixel * m_perPixel;
	}
}

int Axis::coordinateToPixel(double coordinate) {
	if (m_logarithmic) {
		return round((log10(coordinate)-log10(m_lowLimit)) / m_perPixel);
	} else {
		return round((coordinate-m_lowLimit) / m_perPixel);
	}
}

void Axis::zoom(double factor) {
	if (m_logarithmic) {
		setLimits(m_lowLimit/factor, m_highLimit*factor);
	} else {
		const double middle = (m_lowLimit + m_highLimit) / 2.0;
		setLimits(middle + (m_lowLimit-middle)*factor,
				  middle + (m_highLimit-middle)*factor);
	}
}

void Axis::calculatePerPixel() {
	if (m_logarithmic) {
		m_perPixel = (log10(m_highLimit) - log10(m_lowLimit)) / m_availablePixels;
	} else {
		m_perPixel = (m_highLimit - m_lowLimit) / m_availablePixels;
	}
}

void Axis::calculateTicks() {
	m_tick.clear();
	
	if (m_logarithmic) {
		int exponent = calculateExponent(m_lowLimit);
		double tickSize = pow(10, exponent);
		double currentTick = ceil(m_lowLimit / tickSize) * tickSize;
		while (currentTick <= m_highLimit) {
			if (fabs(currentTick - 1) < PRECISION) {  // to avoid 1-epsilon which results in 10^-1
				m_tick.append(1);
			} else {
				m_tick.append(currentTick);
			}
			
			if (fabs(currentTick - tickSize*10) < PRECISION) {  // increase step size
				exponent++;
				tickSize = pow(10, exponent);
			}
			currentTick += tickSize;
		}
	} else {
		double currentTick = m_tickSize * ceil(m_lowLimit/m_tickSize);  // calculate lowest shown number
		while (currentTick <= m_highLimit) {  // calculate all shown numbers
			if (fabs(currentTick) < PRECISION) {  // to avoid -0
				m_tick.append(0);
			} else {
				m_tick.append(currentTick);
			}
			currentTick += m_tickSize;
		}
		
		setBestExponent();
		calculateNeededPrecision();
	}
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
	value = (fabs(value) < PRECISION) ? 0 : value;  // catch rounding errors
	int exponent = (value == 0) ? 0 : log10(fabs(value)+PRECISION);  // log10 can't handle 0; catch rounding errors	
	if (fabs(value) < 1 && value != 0) {
		exponent--;  // casting to int makes log10 give wrong result for negativ exponents
	}
	return exponent;
}

void Axis::setBestExponent() {
	if (! m_tick.isEmpty()) {  // can be empty, if tickSize is smaller than precision
		int exponent1 = calculateExponent(m_tick.first());
		int exponent2 = calculateExponent(m_tick.last());
		if (fabs(m_tick.first()) < PRECISION) {  // for that value 0 doesn't set exponent allways to 0
			m_exponent = exponent2;
		} else if (fabs(m_tick.last()) < PRECISION) {
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
