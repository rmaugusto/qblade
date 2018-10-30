#include "NewColorButton.h"

#include <QColorDialog>
#include <QPainter>


NewColorButton::NewColorButton(QColor color) {
	setColor(color);
	
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void NewColorButton::paintEvent(QPaintEvent *event) {
	QPushButton::paintEvent(event);
	
	QPainter painter(this);
	QPen pen (m_color);
	if (!isEnabled()) {
		QColor color = pen.color();
		color.setAlpha(60);
		pen.setColor(color);
	}
	painter.setPen(pen);
	painter.setBrush(QBrush(pen.color(), Qt::SolidPattern));
	
	const QRect colorRect = rect().adjusted(+10, +7, -10, -7);
	painter.drawRect(colorRect);
}

void NewColorButton::setColor(QColor color) {
	if (m_color != color) {
		m_color = color;
		update();
		emit colorChanged(m_color);
	}
}

void NewColorButton::setColor(QPen pen) {
	setColor(pen.color());
}

void NewColorButton::onClicked() {
	QColor chosenColor = QColorDialog::getColor(m_color, this);
	if (chosenColor.isValid()) {  // if user cancels, the color is not valid
		setColor(chosenColor);
	}
}

QSize NewColorButton::sizeHint() const {
	QSize buttonSize = QPushButton::sizeHint();
	buttonSize.setWidth(130);
	return buttonSize;
}
