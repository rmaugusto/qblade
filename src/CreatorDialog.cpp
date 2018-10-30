#include "CreatorDialog.h"

#include <QBoxLayout>
#include <QPushButton>


CreatorDialog::CreatorDialog() {
	m_contentVBox = new QVBoxLayout ();
	setLayout(m_contentVBox);
	
		m_contentVBox->addStretch ();
				
		m_buttonHBox = new QHBoxLayout ();
		m_contentVBox->addLayout(m_buttonHBox);
			m_buttonHBox->addStretch();
			QPushButton *button = new QPushButton ("Cancel");
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));
			m_buttonHBox->addWidget (button);
			button = new QPushButton ("Create");
			button->setDefault(true);
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onCreateButtonClicked()));
			m_buttonHBox->addWidget (button);
}

CreatorDialog::~CreatorDialog() {
	
}

void CreatorDialog::onCancelButtonClicked() {
	reject();
}
