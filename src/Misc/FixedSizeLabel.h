#ifndef FIXEDSIZELABEL_H
#define FIXEDSIZELABEL_H

#include <QLabel>


class FixedSizeLabel : public QLabel
{
public:
	FixedSizeLabel (const QString &text = "", int width = 0);
	void setText (const QString &value);  // hides original from QLabel
	void setFixedWidth (int width);  // hides original from QWidget
	
private:
	int m_fixedWidth;
};

#endif // FIXEDSIZELABEL_H
