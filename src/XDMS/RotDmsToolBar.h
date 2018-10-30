#ifndef ROTDMSTOOLBAR_H
#define ROTDMSTOOLBAR_H

class QComboBox;

#include "../SimulationToolBar.h"
class RotDmsModule;
class CBlade;
class DMSData;


class RotDmsToolBar : public SimulationToolBar<CBlade,DMSData>
{
	Q_OBJECT

public:
	RotDmsToolBar (QMainWindow *parent, RotDmsModule *module);
	void setShownSimulation (DMSData *newSimulation);
	int getTipSpeedIndex();

private:
	SimulationModule<CBlade,DMSData>* module();

	RotDmsModule *m_module;
	QComboBox *m_tipSpeedComboBox, *m_heightComboBox;
};


#endif // ROTDMSTOOLBAR_H
