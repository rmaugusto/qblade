#ifndef AXISTICK_H
#define AXISTICK_H

#include <QString>
#include <QVector>
#include <QStringList>


class Axis
{
public:
	double getLowLimit () { return m_lowLimit; }
	double getHighLimit () { return m_highLimit; }
	int getNumberOfTicks () { return m_tick.size(); }
	double getTickValueAt (int position) { return m_tick[position]; }
	QString getTickStringAt (int position);
	bool getUseExponent () { return m_useExponent; }
	QString getExponentString () { return m_exponentString; }
	double getTickSize () { return m_tickSize; }
	int getExponent () { return m_exponent; }
	void setTickSizeFactor (double tickSizeFactor);
	void setLimits (double min, double max);
	void setTickSize (double tickSize);
	void moveLimits (double deltaMin, double deltaMax);
	
private:
	void calculateTicks ();
	void calculateTickSize();
	int calculateExponent (double value);
	void setBestExponent();
	void calculateNeededPrecision ();
	
	double m_lowLimit, m_highLimit;  // the lower and higher limit of this axis
	double m_tickSize;
	double m_tickSizeFactor;  // this value influences the tick size
	int m_tickExponent;  // the exponent of the ticks
	QVector<double> m_tick;
	bool m_useExponent;
	int m_exponent;
	int m_neededPrecision;	
	QString m_exponentString;
	QStringList m_mantissaStrings;
};

#endif // AXISTICK_H
