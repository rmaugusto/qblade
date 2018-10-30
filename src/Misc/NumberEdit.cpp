#include "NumberEdit.h"
#include <QTextStream>
#include <QDebug>

NumberEdit::NumberEdit (OutputFormat format, int automaticPrecision, double minimum, double maximum) {
	m_minimum = minimum;
	m_maximum = maximum;
	m_automaticPrecision = automaticPrecision;
	m_format = format;
	
	connect(this, SIGNAL(editingFinished()), SLOT(onEditingFinished()));
	
	setValue (0, false);
	showValue();
}

void NumberEdit::setValue(double newValue, bool chopPrecision) {
	if (newValue < m_minimum) {
		newValue = m_minimum;
	} else if (newValue > m_maximum) {
		newValue = m_maximum;
	}
	
	if (m_automaticPrecision == 0) {  // round allways, if precision is 0 (integer)
		newValue = round(newValue);
	} else if (chopPrecision) {
		double factor = 1;
		if (m_format == Scientific) {
			factor = pow(10.0, m_automaticPrecision+1 - ceil(log10(fabs(newValue))));
		} else if (m_format == Standard) {
			factor = pow(10.0, m_automaticPrecision);
		}
		newValue = round(newValue * factor) / factor;
	}

	if (m_value != newValue) {
		m_value = newValue;
		emit valueChanged(newValue);
	}
	
	showValue();
}

double NumberEdit::getValue(bool processValueFirst) {
	if (processValueFirst) {
		onEditingFinished();
	}
	return m_value;
}

void NumberEdit::setMinimum(double newMinimum) {
	setRange(newMinimum, m_maximum);
}

void NumberEdit::setMaximum(double newMaximum) {
	setRange(m_minimum, newMaximum);
}

void NumberEdit::setRange(double newMinimum, double newMaximum) {
	m_minimum = newMinimum;
	m_maximum = newMaximum;
	setValue(m_value, false);
}

void NumberEdit::setAutomaticPrecision(int newPrecision) {
	m_automaticPrecision = newPrecision;
}

void NumberEdit::setFormat(OutputFormat newFormat) {
	m_format = newFormat;
}

void NumberEdit::showValue() {
	QString value = QString("%1").arg(m_value, 0, (m_format == Standard ? 'f' : 'e'), 8);
	if (value.contains('.')) {
		if (m_format == Standard) {
			while(value.endsWith('0'))
				value.chop(1);
			if (value.endsWith('.'))
				value.chop(1);
		} else if (m_format == Scientific) {
			int posOfE = value.indexOf('e');
			int posOfFirstZero = posOfE-1;
			while (value[posOfFirstZero] == '0')
				--posOfFirstZero;
			if (value[posOfFirstZero] == '.')
				--posOfFirstZero;
			value.remove(posOfFirstZero+1, posOfE - posOfFirstZero - 1);
		}
	}
	setText(value);
}

void NumberEdit::onEditingFinished() {
	bool success;
	double value = text().toDouble(&success);
	if (success) {
		setValue(value, false);
	} else {
		showValue();
	}
}
