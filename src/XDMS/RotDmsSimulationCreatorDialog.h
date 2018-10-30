#ifndef ROTDMSSIMULATIONCREATORDIALOG_H
#define ROTDMSSIMULATIONCREATORDIALOG_H

#include "../SimulationCreatorDialog.h"
class DMSData;
class RotDmsModule;


class RotDmsSimulationCreatorDialog : public SimulationCreatorDialog<Parameter::DMSData>
{
	Q_OBJECT

public:
	RotDmsSimulationCreatorDialog(DMSData *presetSimulation, RotDmsModule *module);

private:
	void initView ();

	/* loading and saving the recently used values to settings is useful, because when creating a new simulation for
	 * a different rotor, there would be no way to take those values over.
	 * */
	void loadValuesFromSettings();
	void saveValuesToSettings();

	RotDmsModule *m_module;
	DMSData *m_editedSimulation;

private slots:
	void onCreateButtonClicked ();
	void onRestoreDefaultClicked();
	void onUnitsChanged () { }  // no need for this
};

#endif // ROTDMSSIMULATIONCREATORDIALOG_H
