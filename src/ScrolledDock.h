#ifndef SCROLLEDDOCK_H
#define SCROLLEDDOCK_H

#include <QDockWidget>
#include <QScrollBar>
class QMainWindow;
class QVBoxLayout;


/**
 * @brief Parent class for all DockWidgets
 *
 * Inherit this class and put your content widgets in the m_contentVBox. The dock will be scrollable and
 * automaticly stores and restores it's size.
 */
class ScrolledDock : public QDockWidget
{
	Q_OBJECT
	
public:
	ScrolledDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags);
	
protected:
	/**
	 * @brief Finish the construction of the dock
	 *
	 * This function must be called at the end of a docks constructor.
	 * @param area Where the dock will initialy appear.
	 * @param parent Where to add the dock to.
	 */
	void addScrolledDock (Qt::DockWidgetArea area, QMainWindow *parent, int initialWidth = -1);

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
    void resizeEvent(QResizeEvent *event);

private:
	int m_storedWidth;  /**< Stores the width of this dock */

signals:
    void resized();  /**< Emits a signal when dock is resized */
};

#endif // SCROLLEDDOCK_H
