#ifndef MULTIDMSTOOLBAR_H
#define MULTIDMSTOOLBAR_H

class QComboBox;

#include "../SimulationToolBar.h"
class MultiDmsModule;
class CBlade;
class CDMSData;


class MultiDmsToolBar : public SimulationToolBar<CBlade,CDMSData>
{
	Q_OBJECT
	
public:
	MultiDmsToolBar (QMainWindow *parent, MultiDmsModule *module);
	void setShownSimulation (CDMSData *newSimulation);
	
private:
	virtual SimulationModule<CBlade,CDMSData>* module();
	
	MultiDmsModule *m_module;
	QComboBox *m_windspeedComboBox, *m_rotationalComboBox, *m_pitchComboBox;
};

#endif // MULTIDMSTOOLBAR_H
