#ifndef NEWCOLORBUTTON_H
#define NEWCOLORBUTTON_H

#include <QPushButton>
#include <QColor>
#include <QPen>


class NewColorButton : public QPushButton
{
	Q_OBJECT
	
public:
	NewColorButton(QColor color = Qt::black);
	void paintEvent (QPaintEvent *event);
	QColor getColor () { return m_color; }
		
public slots:
	void setColor(QColor color);
	void setColor(QPen pen);
	
private slots:
	void onClicked ();
	
private:
	QSize sizeHint() const;
	QColor m_color;
	
signals:
	void colorChanged (QColor color);
};

#endif // NEWCOLORBUTTON_H
