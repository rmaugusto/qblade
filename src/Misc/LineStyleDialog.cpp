#include "LineStyleDialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

#include "../MainFrame.h"
#include "LineStyleComboBox.h"
#include "NewColorButton.h"


LineStyleDialog::LineStyleDialog(QWidget *parent, QPen pen, bool style, bool width, bool color)
	: QDialog (parent)
{
	setWindowTitle("Choose Line Style");
	
	QGridLayout *grid = new QGridLayout ();
	setLayout(grid);
		QLabel *label = new QLabel ("Style:");
		grid->addWidget(label, 0, 0);
		m_styleButton = new LineStyleComboBox (LineStyleComboBox::STYLE);
		grid->addWidget(m_styleButton, 0, 1);
		label = new QLabel ("Width:");
		grid->addWidget(label, 1, 0);
		m_widthButton = new LineStyleComboBox (LineStyleComboBox::WIDTH);
		grid->addWidget(m_widthButton, 1, 1);
		label = new QLabel ("Color:");
		grid->addWidget(label, 2, 0);
		m_colorButton = new NewColorButton ();
		grid->addWidget(m_colorButton, 2, 1);
		QHBoxLayout *hBox = new QHBoxLayout ();
		grid->addLayout(hBox, 3, 0, 1, 2);
			m_cancelButton = new QPushButton ("Cancel");
			hBox->addWidget(m_cancelButton);
			m_okButton = new QPushButton ("Ok");
			hBox->addWidget(m_okButton);
			
	m_styleButton->setEnabled(style);
	m_widthButton->setEnabled(width);
	m_colorButton->setEnabled(color);
			
	setPen(pen);
	
	connect(m_styleButton, SIGNAL(currentPenChanged(QPen)), this, SLOT(setPen(QPen)));
	connect(m_widthButton, SIGNAL(currentPenChanged(QPen)), this, SLOT(setPen(QPen)));
	connect(m_colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
	
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

QPen LineStyleDialog::getPen() {
	return m_styleButton->currentPen();
}

void LineStyleDialog::setPen(QPen pen) {
	m_styleButton->setCurrentPen(pen);
	m_widthButton->setCurrentPen(pen);
	m_colorButton->setColor(pen);
}

void LineStyleDialog::setColor(QColor color) {
	QPen pen = m_styleButton->currentPen();
	pen.setColor(color);
	setPen(pen);
}
