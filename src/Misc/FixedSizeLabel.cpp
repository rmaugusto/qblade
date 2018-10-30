#include "FixedSizeLabel.h"

FixedSizeLabel::FixedSizeLabel(const QString &text, int width) {
	setFixedWidth(width);
	setText(text);
}

void FixedSizeLabel::setText(const QString &value) {
	m_fullString = value;
	
	if (m_fixedWidth == 0) {
		QLabel::setText(value);
	} else {
		QString shortenedValue = value;
		QFont defaultFont;  // the empty constructor provides the default font
		QFontMetrics metrics (defaultFont);
		if (metrics.width(shortenedValue) > m_fixedWidth) {
			const int dotsLength = metrics.width("...");
			do {
				shortenedValue.remove(shortenedValue.size()-4, 1);
			} while (metrics.width(shortenedValue) + dotsLength > m_fixedWidth && shortenedValue.size() > 3);
			shortenedValue.insert(shortenedValue.size()-3, "...");
			setToolTip(value);
		} else {
			setToolTip("");
		}
		QLabel::setText(shortenedValue);
	}
}

QString FixedSizeLabel::text() const {
	return m_fullString;
}

void FixedSizeLabel::setFixedWidth(int width) {
	m_fixedWidth = width;
	QLabel::setMaximumWidth(width);
}
