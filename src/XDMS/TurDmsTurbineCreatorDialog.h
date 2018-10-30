#ifndef TURDMSTURBINECREATORDIALOG_H
#define TURDMSTURBINECREATORDIALOG_H

#include <QDialog>
class QButtonGroup;
class QLineEdit;

#include "../StoreAssociatedComboBox_include.h"
#include "../CreatorDialog.h"
#include "../ParameterViewer.h"
class TData;
class TurDmsModule;
class NumberEdit;


class TurDmsTurbineCreatorDialog : public CreatorDialog, public ParameterViewer<Parameter::TData>
{
	Q_OBJECT	
	
public:
	TurDmsTurbineCreatorDialog(TData *presetTurbine, TurDmsModule *module);

private:
	void initView ();
	
	TurDmsModule *m_module;
	TData *m_editedTurbine;
	
	QButtonGroup *m_transmissionButtonGroup;
	QWidget *m_singleWidget, *m_variableWidget, *m_prescribedWidget;
	QPushButton *m_prescribeSpeedButton;
	QList<double> m_prescribedWindSpeed, m_prescribedRotorSpeed;
	
private slots:
	void onTransmissionChanged (int buttonId, bool checked);
	void onPrescribeButtonClicked ();
	void onCreateButtonClicked ();  // creates simulation object
	void onUnitsChanged () { }  // no need for this
};

#endif // TURDMSTURBINECREATORDIALOG_H
