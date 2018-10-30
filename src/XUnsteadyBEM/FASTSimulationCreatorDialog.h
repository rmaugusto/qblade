#ifndef FASTSIMULATIONCREATORDIALOG_H
#define FASTSIMULATIONCREATORDIALOG_H

class QScrollArea;
class QLabel;

#include "FASTSimulation.h"
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include "../XWidgets.h"
#include "../Misc/NumberEdit.h"
#include "FASTModule.h"
#include <QCheckBox>
#include "../StoreAssociatedComboBox_include.h"
class FASTModule;


class FASTSimulationCreatorDialog : public QDialog
{
	Q_OBJECT	
	
public:
	FASTSimulationCreatorDialog (FASTSimulation *presetSimulation, FASTModule *module);
	
private:
	void initView ();
	
	FASTModule *m_module;
	FASTSimulation *m_editedFASTSimulation;
	
	QLabel *m_errorLabel;
	QLineEdit *m_nameEdit;  // defines the simulations name
	WindFieldComboBox *m_windFieldComboBox;
	RotorComboBox *m_rotorComboBox;
	BladeStructureComboBox *m_bladeStructureComboBox;
    NumberEdit *m_totalRunTimeEdit, *m_timeStepEdit, *m_aeroTimeStepEdit, *m_gravityEdit, *m_rotorSpeedEdit, *m_nacelleYawEdit, *m_hubHeightEdit,
			   *m_airDensEdit, *m_kinViscEdit, *m_tStartEdit, *m_decFactEdit;
	QCheckBox *m_flapDof1CheckBox, *m_flapDof2CheckBox, *m_edgeDofCheckBox;
    QPushButton *m_createButton, *m_cancelButton, *m_openWindfieldButton;
	QComboBox *m_stallModComboBox, *m_useCmComboBox, *m_infModelComboBox, *m_indModelComboBox,
			  *m_tlModelComboBox, *m_hlModelComboBox;
    QButtonGroup *m_twoOrThreeBladesGroup, *m_windfieldLocationGroup;
	QWidget *m_sectionTableWidget;  // Attention: this can be NULL
	QScrollArea *m_sectionTableScrollArea;
	int m_numberOfStrainGages;
	QList<QCheckBox*> m_strainGageCheckBoxList, m_aeroDynCheckBoxList;
	QList<QPushButton*> m_rotorParameterButtonList, m_bladeParameterButtonList;
	QCheckBox *m_blade1Output, *m_blade2Output, *m_blade3Output;
    QString m_WindfieldPathString;
	
private slots:
	void onWindFieldOrRotorChanged ();
    void onWindfieldLocationChanged();
    void onOpenWindfieldFile();
	void onRotorChanged ();
	void onTwoOrThreeBladesChanged ();
	void onDOFChanged ();
	void onCreateButtonClicked ();  // creates simulation object
	void onCancelButtonClicked ();
	void createSectionTable (int);  // the table in section tab
	void onStrainGageCheckBoxToggled (bool checked);
	void onSetAllClicked (bool checked);
};

#endif // FASTSIMULATIONCREATORDIALOG_H
