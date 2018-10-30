#ifndef WINDFIELDTOOLBAR_H
#define WINDFIELDTOOLBAR_H

#include <QToolBar>
class QSlider;
class QComboBox;

#include "../StoreAssociatedComboBox_include.h"
class WindFieldModule;
class NumberEdit;


class WindFieldToolBar : public QToolBar
{
	Q_OBJECT
	
public:
	WindFieldToolBar (QMainWindow *parent, WindFieldModule *module);
	
	void useWindField (WindField *newShownWindField);  // will show the current WindFields information
	
private:
	WindFieldModule *m_module;
	WindField *m_shownWindField;  // the currently shown WindField
	
	WindFieldComboBox *m_windFieldComboBox;
	NumberEdit *m_timestepEdit;
	QSlider *m_timestepSlider;
	
public slots:
	void onSliderChanged (int newValue);  // shows another timestep
	void onTimestepEditEdited ();  // shows another timestep
};

#endif // WINDFIELDTOOLBAR_H
