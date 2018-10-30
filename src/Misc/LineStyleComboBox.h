#ifndef LINESTYLECOMBOBOX_H
#define LINESTYLECOMBOBOX_H

#include <QComboBox>
#include <QPen>


/**
 * @brief ComboBox containing different QPen.
 *
 * This class represents a ComboBox in which 5 different QPen are choosable. They either differ in style or width,
 * depending on the parameter of the constructor. When another QPen is selected, an appropriate signal is emited.
 */
class LineStyleComboBox : public QComboBox
{
	Q_OBJECT
	
public:
	enum Application {STYLE, WIDTH};
	
	LineStyleComboBox(Application application);
	
	void paintEvent(QPaintEvent *event);
	QPen currentPen ();
	
private:
	QSize sizeHint() const;
	Application m_application;
	
public slots:
	void onCurrentIndexChanged ();
	void setCurrentPen (QPen pen);
	
signals:
	void currentPenChanged (QPen newPen);
};

#endif // LINESTYLECOMBOBOX_H
