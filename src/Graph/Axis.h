#ifndef AXISTICK_H
#define AXISTICK_H

#include <QString>
#include <QVector>
#include <QStringList>


class Axis
{
public:
	Axis (double lowLimit, double highLimit, double tickSizeFactor, bool logarithmic);
	
	bool getLogarithmic () { return m_logarithmic; }
	void setLogarithmic (bool logarithmic);
	double getLowLimit () { return m_lowLimit; }
	double getHighLimit () { return m_highLimit; }
	int getNumberOfTicks () { return m_tick.size(); }
	double getTickValueAt (int position) { return m_tick[position]; }
	QString getTickStringAt (int position);
	bool getUseExponent ();
	QString getExponentString (int position);
	double getTickSize () { return m_tickSize; }
	int getExponent () { return m_exponent; }
	
	void setAvailablePixels (int availablePixels);
	void setLimits (double min, double max);
	void setOptimalLimits (double low, double high);
	void setTickSize (double tickSize);
	void moveLimits (int pixels);
	double pixelToCoordinate (int pixel);
	int coordinateToPixel (double coordinate);
	void zoom (double factor);
	
	static const double PRECISION;
	
private:
	void calculatePerPixel ();
	void calculateTicks ();
	void calculateTickSize();
	int calculateExponent (double value);
	void setBestExponent();
	void calculateNeededPrecision ();
	
	bool m_logarithmic;
	int m_availablePixels;  // how many pixels long the axis is
	double m_perPixel;  // factor for translating pixel into coordinate
	double m_lowLimit, m_highLimit;  // the lower and higher limit of this axis
	double m_tickSize;
	double m_tickSizeFactor;  // this value influences the tick size
	int m_tickExponent;  // the exponent of the ticks
	QVector<double> m_tick;
	bool m_useExponent;
	int m_exponent;
	int m_neededPrecision;	
	QString m_exponentString;
};

#endif // AXISTICK_H
