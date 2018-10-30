#ifndef TURDMSTOOLBAR_H
#define TURDMSTOOLBAR_H

class QAction;
class QComboBox;
class QDoubleSpinBox;

#include "../SimulationToolBar.h"
#include "../StoreAssociatedComboBox_include.h"
class TurDmsModule;


class TurDmsToolBar : public SimulationToolBar<TData,TDMSData>
{
	Q_OBJECT
	
public:
	TurDmsToolBar (QMainWindow *parent, TurDmsModule *module);
	void setShownSimulation (TDMSData *newSimulation);
	int getWindspeedIndex();
	void getWeibullParameters (double *k, double *kMoreLess, double *a, double *aMoreLess);
	
private:
	virtual SimulationModule<TData,TDMSData>* module();
	
	TurDmsModule *m_module;
	QComboBox *m_windspeedComboBox, *m_heightPositionComboBox;
	QDoubleSpinBox *m_kEdit, *m_aEdit, *m_kMoreLessEdit, *m_aMoreLessEdit;
	
private slots:
	void onSimulationStoreChange();
};

#endif // TURDMSTOOLBAR_H
