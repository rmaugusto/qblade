#ifndef FASTSIMULATIONTOOLBAR_H
#define FASTSIMULATIONTOOLBAR_H

#include <QToolBar>
class QComboBox;
class QSlider;

#include "../StoreAssociatedComboBox_include.h"
class FASTModule;
class FASTSimulation;
class NumberEdit;


class FASTSimulationToolBar : public QToolBar
{
	Q_OBJECT
	
public:
	FASTSimulationToolBar (QMainWindow *parent, FASTModule *module);
	
	int getCurrentBladeSection ();
	void useFASTSimulation (FASTSimulation *newShownFASTSimulation);  // will show the current simulation information
	
private slots:
	void onSectionBoxChanged ();
	void onTimeSliderChange (int newValue);
	void onTimeEditChange ();
	
private:
	void setShownTimeForAllSimulations ();
	
	FASTModule *m_module;
	
	FASTSimulationComboBox *m_FASTSimulationComboBox;
	NumberEdit *m_timeEdit;
	QSlider *m_timeSlider;
	bool m_enableTimeChange;  // to help synchronize the slider and the edit
	QComboBox *m_sectionComboBox;
	bool m_enableSectionChange;  // false if the shownBladeSection of the FASTSimulations is meant to not change
};

#endif // FASTSIMULATIONTOOLBAR_H
