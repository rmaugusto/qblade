#include "ScrolledDock.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QMainWindow>
#include <QApplication>


ScrolledDock::ScrolledDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags)
	: QDockWidget (title, parent, flags)
{
	QScrollArea *scroll = new QScrollArea ();
	setWidget (scroll);
		QWidget *widget = new QWidget ();
		widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		scroll->setWidget(widget);
			m_contentVBox = new QVBoxLayout ();
			// the next line is extremely important! Everything inside the layout will shrink if not here
			m_contentVBox->setSizeConstraint(QLayout::SetMinimumSize);
			widget->setLayout(m_contentVBox);
}

void ScrolledDock::addScrolledDock (Qt::DockWidgetArea area, QMainWindow *parent) {
	/* store the size of the dockable */
	m_storedSize = m_contentVBox->minimumSize();
	m_storedSize.setWidth (m_storedSize.width() + 20);  // increased by arbitrary value to avoid initial scrollbar

	/* add the dock to MainFrame and hide it for the moment */
	hide();
	parent->addDockWidget (area, this);
	connect(this, SIGNAL(visibilityChanged(bool)), SLOT(onVisibilityChanged(bool)));
}

void ScrolledDock::onVisibilityChanged(bool visible) {
	if (visible) {
		if (m_storedSize.isValid())
		{
			setMinimumSize(m_storedSize);  // the only way to force a dockable to change it's size
			setMaximumSize(m_storedSize);
			qApp->processEvents();  // needed to avoid optimization that would skip size change
			setMinimumSize(0, 0);
			setMaximumSize(9999, 9999);
		}
	} else {
		m_storedSize = size();
	}
}
