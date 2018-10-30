#ifndef CREATORDOCK_H
#define CREATORDOCK_H

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>

#include "ScrolledDock.h"


class CreatorDockSignalsSlots : public ScrolledDock
{
	Q_OBJECT
	
protected:
	CreatorDockSignalsSlots(const QString &title, QMainWindow *parent, Qt::WindowFlags flags)
		: ScrolledDock(title, parent, flags) { }
	
protected slots:
	virtual void onEditCopyButtonClicked () = 0;
	virtual void onRenameButtonClicked () = 0;
	virtual void onDeleteButtonClicked () = 0;
	virtual void onNewButtonClicked () = 0;
	
	/* inheriting Docks slots; don't use in CreatorDock; must be here, because inheriting Docks can't have slots themselves */
	virtual void onStartSimulationButtonClicked () { throw "illegal use"; }
	virtual void onShowChanged () { throw "illegal use"; }
	virtual void onShowCurveChanged () { throw "illegal use"; }
	virtual void onShowPointsChanged () { throw "illegal use"; }
	virtual void onShowSelectChanged () { throw "illegal use"; }
	virtual void onStyleChanged () { throw "illegal use"; }
};


template <class Object>
class CreatorDock : public CreatorDockSignalsSlots
{
//	Q_OBJECT  // incompatible
	
public:
	virtual void setShownObject (Object *newObject);
	
protected:
	CreatorDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags);

	Object *m_shownObject;
	QGridLayout *m_controlsGridLayout;
	
private:
	QPushButton *m_editCopyButton, *m_renameButton, *m_newButton, *m_deleteButton;
};


template <class Object>
void CreatorDock<Object>::setShownObject(Object *newObject) {
	m_shownObject = newObject;
	
	m_editCopyButton->setEnabled(m_shownObject);
	m_renameButton->setEnabled(m_shownObject);
	m_deleteButton->setEnabled(m_shownObject);
}

template <class Object>
CreatorDock<Object>::CreatorDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags)
	: CreatorDockSignalsSlots(title, parent, flags), m_shownObject(NULL)
{
	QGroupBox *groupBox = new QGroupBox ("Controls");
	m_contentVBox->addWidget(groupBox);
		m_controlsGridLayout = new QGridLayout ();
		m_controlsGridLayout->setColumnStretch(2, 1);  // prevents controles from growing
		groupBox->setLayout(m_controlsGridLayout);
			m_renameButton = new QPushButton ("Rename");
			connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
			m_controlsGridLayout->addWidget (m_renameButton, 1, 0);
			m_editCopyButton = new QPushButton ("Edit/Copy");
			connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));					
			m_controlsGridLayout->addWidget (m_editCopyButton, 1, 1);
			m_deleteButton = new QPushButton ("Delete");
			connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
			m_controlsGridLayout->addWidget (m_deleteButton, 2, 0);
			m_newButton = new QPushButton ("New");
			connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
			m_controlsGridLayout->addWidget (m_newButton, 2, 1);
}

#endif // CREATORDOCK_H
