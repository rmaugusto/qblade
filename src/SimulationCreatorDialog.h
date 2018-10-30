#ifndef SIMULATIONCREATORDIALOG_H
#define SIMULATIONCREATORDIALOG_H

class QGroupBox;

#include "CreatorDialog.h"
#include "ParameterViewer.h"


template <class ParameterGroup>
class SimulationCreatorDialog : public CreatorDialog, public ParameterViewer<ParameterGroup>
{
protected:
	typedef ParameterGroup P;

	SimulationCreatorDialog();
	QGroupBox* constructParameterBox(QString defaultName);
	QGroupBox* constructCorrectionsBox();
	virtual void onRestoreDefaultClicked();
	virtual void loadValuesFromSettings();
	virtual void saveValuesToSettings();
	
	QVBoxLayout *m_leftContentVBox, *m_rightContentVBox;
};

#endif // SIMULATIONCREATORDIALOG_H
