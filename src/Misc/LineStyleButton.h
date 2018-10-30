#ifndef LINESTYLEBUTTON_H
#define LINESTYLEBUTTON_H

#include <QPen>
#include <QPushButton>


class LineStyleButton : public QPushButton
{
	Q_OBJECT
	
public:
	LineStyleButton(bool style = true, bool width = true, bool color = true);
	
	void paintEvent (QPaintEvent *event);
	
	QPen getPen () { return m_pen; }
	void setPen (QPen pen) { m_pen = pen; update(); }
	
private slots:
	void openDialog ();
	
private:
	QSize sizeHint() const;
	QPen m_pen;
	bool m_modifiableStyle, m_modifiableWidth, m_modifiableColor;
	
signals:
	void styleChanged ();
};

#endif // LINESTYLEBUTTON_H
