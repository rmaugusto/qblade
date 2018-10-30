#include "LineStyleButton.h"

#include <QPainter>

#include "LineStyleDialog.h"


LineStyleButton::LineStyleButton(bool style, bool width, bool color) 
	: m_modifiableStyle(style),
	  m_modifiableWidth(width),
	  m_modifiableColor(color)
{
	connect (this, SIGNAL(clicked()), this, SLOT(openDialog()));
}

void LineStyleButton::paintEvent(QPaintEvent *event) {
	QPushButton::paintEvent(event);
	
	QPainter painter(this);
	QPen pen = m_pen;
	if (!isEnabled()) {
		QColor color = pen.color();
		color.setAlpha(60);
		pen.setColor(color);
	}
	painter.setPen(pen);
	
	const QRect r = rect();
	painter.drawLine(r.left()+10, r.height()/2, r.width()-10, r.height()/2);
}

void LineStyleButton::openDialog() {
	LineStyleDialog *dialog = new LineStyleDialog (this, m_pen, m_modifiableStyle, m_modifiableWidth, m_modifiableColor);
	dialog->move(QCursor::pos() - QPoint(100, 100));
	if (dialog->exec() == QDialog::Accepted) {
		m_pen = dialog->getPen();
		emit styleChanged();
		update();
	}

	dialog->deleteLater();
}

QSize LineStyleButton::sizeHint() const {
	QSize buttonSize = QPushButton::sizeHint();
	buttonSize.setWidth(130);
	return buttonSize;
}
