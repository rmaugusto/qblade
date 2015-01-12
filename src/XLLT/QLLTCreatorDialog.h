#ifndef QLLTSIMULATIONCREATORDIALOG_H
#define QLLTSIMULATIONCREATORDIALOG_H

class QScrollArea;
class QLabel;

#include "QLLTSimulation.h"
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include "../XWidgets.h"
#include "../Misc/NumberEdit.h"
#include "QLLTModule.h"
#include <QCheckBox>
#include "../StoreAssociatedComboBox_include.h"
class QLLTModule;


class QLLTCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    QLLTCreatorDialog (QLLTSimulation *presetSimulation, QLLTModule *module);

private:
    void initView ();

    QLLTModule *m_module;
    QLLTSimulation *m_editedLLTSimulation;

    QLabel *m_errorLabel;
    QLineEdit *m_nameEdit;  // defines the simulations name
    WindFieldComboBox *m_windFieldComboBox;
    RotorComboBox *m_rotorComboBox;
    NumberEdit *m_totalRunTimeEdit, *m_timeStepEdit, *m_aziStepEdit, *m_TSREdit, *m_rotorSpeedEdit, *m_nacelleYawEdit, *m_nacelleTeetherEdit, *m_infloweAngleEdit,
               *m_airDensEdit, *m_kinViscEdit, *m_turbulentViscosity;
    QPushButton *m_createButton, *m_cancelButton;
    NumberEdit *m_maxWakeAge, *m_firstWakeRowLength, *m_fullWakeRevs, *m_fineWakeRevs, *m_thinFactor, *m_vortexTimeOffset, *m_Epsilon, *m_relaxationFactor, *m_maxIterations, *m_cutOff, *m_numberTimesteps, *m_numberRevs;
    QButtonGroup *m_TowerShadowGroup, *m_GroundEffectGroup, *m_ShedTrailGroup, *m_WakeRollupGroup, *m_PrescribedGroup,*m_TurbConvectionGroup;
    NumberEdit *m_TowerRadius, *m_TowerDragCoeff, *m_TowerDistance, *m_HubHeight;

private slots:
    void onWindFieldOrRotorChanged ();
    void onCreateButtonClicked ();
    void onCancelButtonClicked ();

    void OnRunTimeChanged ();
    void OnNumberTimestepsChanged ();
    void OnNumberRevolutionsChanged ();
    void OnTSRChanged ();
    void OnRotationalSpeedChanged ();
    void OnAziChanged ();
    void OnTimestepChanged ();

    void OnIncludeTowerChanged ();
    void OnIncludeGroundChanged ();



};

#endif // QLLTSIMULATIONCREATORDIALOG_H
