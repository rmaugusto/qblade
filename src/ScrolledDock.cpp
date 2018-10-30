#include "ScrolledDock.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QMainWindow>
#include <QApplication>


ScrolledDock::ScrolledDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags)
	: QDockWidget (title, parent, flags)
{
	setObjectName(title);
	
	QScrollArea *scroll = new QScrollArea ();
	scroll->setWidgetResizable(true);
	setWidget (scroll);
		QWidget *widget = new QWidget ();
		scroll->setWidget(widget);
			m_contentVBox = new QVBoxLayout ();
			widget->setLayout(m_contentVBox);
}

void ScrolledDock::addScrolledDock (Qt::DockWidgetArea area, QMainWindow *parent, int initialWidth/* = -1*/) {
	if (initialWidth == -1) {
		m_storedWidth = m_contentVBox->minimumSize().width();
	} else {
		m_storedWidth = initialWidth;
	}
	
	/* add the dock to MainFrame and hide it for the moment */
	hide();
	parent->addDockWidget (area, this);
	connect(this, SIGNAL(visibilityChanged(bool)), SLOT(onVisibilityChanged(bool)));
}

void ScrolledDock::onVisibilityChanged(bool visible) {  // TODO http://stackoverflow.com/questions/14215897/resize-a-qdockwidget
	if (visible) {
		setMinimumWidth(m_storedWidth);  // the only way to force a dockable to change it's size
		setMaximumWidth(m_storedWidth);
		qApp->processEvents();  // needed to avoid optimization that would skip size change
		setMinimumWidth(0);
		setMaximumWidth(9999);
	} else {
		m_storedWidth = width();
	}
}

void ScrolledDock::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    emit resized();
}
