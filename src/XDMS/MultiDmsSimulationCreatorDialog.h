#ifndef MULTIDMSSIMULATIONCREATORDIALOG_H
#define MULTIDMSSIMULATIONCREATORDIALOG_H

#include "../SimulationCreatorDialog.h"
#include "../ParameterViewer.h"
class CDMSData;
class MultiDmsModule;


class MultiDmsSimulationCreatorDialog : public SimulationCreatorDialog<Parameter::CDMSData>
{
	Q_OBJECT	
	
public:
	MultiDmsSimulationCreatorDialog(CDMSData *presetSimulation, MultiDmsModule *module);
	
private:
	void initView ();
	
	/* loading and saving the recently used values to settings is useful, because when creating a new simulation for
	 * a different turbine, there would be no way to take those values over. */
	void loadValuesFromSettings();
	void saveValuesToSettings();
	
	MultiDmsModule *m_module;
	CDMSData *m_editedSimulation;
	
private slots:
	void onCreateButtonClicked ();
	void onRestoreDefaultClicked();
	void onUnitsChanged () { }  // no need for this
};

#endif // MULTIDMSSIMULATIONCREATORDIALOG_H
