#ifndef NOISETOOLBAR_H
#define NOISETOOLBAR_H

#include <QToolBar>
class QComboBox;

#include "../StoreAssociatedComboBox_include.h"
class NoiseModule;


class NoiseToolBar : public QToolBar
{
public:
	NoiseToolBar(QMainWindow *parent, NoiseModule *module);
	void setShownSimulation (NoiseSimulation *newSimulation);
	int getShownOpPointIndex ();
	
private:
	NoiseModule *m_module;
	NoiseSimulationComboBox *m_simulationComboBox;
	QComboBox *m_opPointComboBox;
};

#endif // NOISETOOLBAR_H
