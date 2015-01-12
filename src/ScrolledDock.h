#ifndef SCROLLEDDOCK_H
#define SCROLLEDDOCK_H

#include <QDockWidget>
class QMainWindow;
class QVBoxLayout;

#include "MainFrame.h"  // needed only for friend function declaration


/**
 * @brief Parent class for all DockWidgets
 *
 * Inherit this class and put your content widgets in the m_contentVBox. The dock will be scrollable and
 * automaticly stores and restores it's size.
 */
class ScrolledDock : public QDockWidget
{
	Q_OBJECT
	friend void MainFrame::onViewChanged (int, int);  // this function needs for an unknown reason acces to m_storedSize
	
public:
	ScrolledDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags);
	
	/**
	 * @brief Initializes the widgets in the dock.
	 *
	 * This function must be implemented by the inheriting class. All formular widgets should be initialized
	 * with default values to give the user an idea of what kind of values are expected.
	 */
	virtual void initView () = 0;
	
protected:
	/**
	 * @brief Finish the construction of the dock
	 *
	 * This function must be called at the end of a docks constructor.
	 * @param area Where the dock will initialy appear.
	 * @param parent Where to add the dock to.
	 */
	void addScrolledDock (Qt::DockWidgetArea area, QMainWindow *parent);
	
private slots:
	/**
	 * @brief Stores and restores the docks size
	 *
	 * In Qt the size of a QDockWidget is set to the size of the last shown one. That's very impractical
	 * due to the very different need of space of the contents. This function stores or restores the recent
	 * size of this particular QDockWidget to avoid that issue.
	 * @param visible If the dock now is shown or hidden.
	 */
	void onVisibilityChanged (bool visible);
	
protected:
	QVBoxLayout *m_contentVBox;  /**< Put all content widgets in here */

private:
	QSize m_storedSize;  /**< Stores the size of this dock */
};

#endif // SCROLLEDDOCK_H
