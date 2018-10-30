#ifndef DMSSIMULATIONCREATORDIALOG_H
#define DMSSIMULATIONCREATORDIALOG_H

#include "../SimulationCreatorDialog.h"
class TDMSData;
class TurDmsModule;


class TurDmsSimulationCreatorDialog : public SimulationCreatorDialog<Parameter::TDMSData>
{
	Q_OBJECT	
	
public:
	TurDmsSimulationCreatorDialog(TDMSData *presetSimulation, TurDmsModule *module);
	
private:
	void initView ();
	
	/* loading and saving the recently used values to settings is useful, because when creating a new simulation for
	 * a different turbine, there would be no way to take those values over.
	 * */
	void loadValuesFromSettings();
	void saveValuesToSettings();
	
	TurDmsModule *m_module;
	TDMSData *m_editedSimulation;
	
private slots:
	void onCreateButtonClicked ();
	void onRestoreDefaultClicked();
	void onUnitsChanged () { }  // no need for this
};

#endif // DMSSIMULATIONCREATORDIALOG_H
