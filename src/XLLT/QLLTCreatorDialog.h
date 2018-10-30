/****************************************************************************

    QLLTCreatorDialog Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef QLLTSIMULATIONCREATORDIALOG_H
#define QLLTSIMULATIONCREATORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>

class QScrollArea;
class QLabel;
#include "../XWidgets.h"
#include "../Misc/NumberEdit.h"
#include "QLLTModule.h"
#include "../StoreAssociatedComboBox_include.h"
#include "../XUnsteadyBEM/WindField.h"

class QLLTModule;
class QLLTSimulation;


class QLLTCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    QLLTCreatorDialog (QLLTSimulation *presetSimulation, QLLTModule *module, bool isVawt);    

private:
    void initView ();
    int GetWindInputType ();
    int GetDiscType ();
    QList< QList<double> > m_SimParameters;
    QList<QString> m_FileHeader;


    QLLTModule *m_module;
    QLLTSimulation *m_editedLLTSimulation;
    bool m_bisVawt;

    QLabel *m_ShearLabel, *m_vortSizeLabel, *m_vortInductionLabel, *m_errorLabel, *m_SimFileLabel, *m_WindfieldLabel, *m_InflowLabel, *m_AeroDynLabel, *m_maxWakeAgeLabel, *m_fineWakeRevsLabel, *m_fullWakeRevsLabel;
    QLineEdit *m_nameEdit;  // defines the simulations name
    QPushButton *m_OpenFile, *m_ViewFile;
    WindFieldComboBox *m_windFieldComboBox;
    RotorComboBox *m_rotorComboBox;
    NumberEdit *m_minGammaFrac, *m_InflowSpeed, *m_totalRunTimeEdit, *m_timeStepEdit, *m_aziStepEdit, *m_TSREdit, *m_rotorSpeedEdit, *m_nacelleYawEdit, *m_shaftTilt, *m_horInflowAngleEdit, *m_VertInflowSpeedEdit, *m_ShearCoefficientEdit, *m_refHeightEdit,
               *m_airDensEdit, *m_kinViscEdit, *m_maxStretch, *m_turbulentViscosity, *m_XRollEdit, *m_YRollEdit, *m_TowerHeight, *m_ConeAngle;
    QPushButton *m_createButton, *m_cancelButton;
    NumberEdit *m_Nth_WakeStep, *m_maxWakeAge, *m_firstWakeRowLength, *m_fullWakeRevs, *m_fineWakeRevs, *m_thinFactor, *m_vortexTimeOffset, *m_Epsilon, *m_relaxationFactor, *m_maxIterations, *m_z_leishmanEdit, *m_numberTimesteps, *m_numberRevs, *m_aziStart, *m_initRot, *m_momInertia;
    QButtonGroup *m_strutGroup, *m_ShearGroup, *m_vortexLiftGroup, *m_EvalCenterGroup, *m_DSGroup, *m_upWindGroup, *m_discTypeGroup, *m_IntegrationSchemeGroup, *m_RotorStandstillGroup, *m_TowerShadowGroup, *m_GroundEffectGroup, *m_ShedGroup, *m_TrailGroup, *m_WakeRollupGroup, *m_HimmelskampGroup, *m_strainGroup, *m_desingGroup, *m_startupGroup, *m_wakeCounterGroup,*m_TurbConvectionGroup, *m_storeWakePositions, *m_WindInputGroup, *m_useWindfieldGroup, *m_adaptiveGroup;
    NumberEdit *m_TowerTopRadius, *m_TowerBottomRadius, *m_TowerDragCoeff, *m_RotorOverhang, *m_HubHeight, *m_TStart, *m_DecFactor, *m_Tp, *m_Tf, *m_Tv, *m_Tvl;
    QSpinBox *m_numPanels, *m_numStrutPanels;
    NumberEdit *m_BoundVortexPos, *m_AoAPos;

    QLabel *strutLab1, *strutLab2;

    double getMeanWindSpeed();
    QString GetFileName();

    int wakeCounter;

private slots:
    void onRotorChanged ();
    void onWindFieldChanged ();
    void OnStrainChanged();


    void onCreateButtonClicked ();
    void onCancelButtonClicked ();

    void OnRunTimeChanged ();
    void OnNumberTimestepsChanged ();
    void OnIncludeDSChanged();
    void OnVortSizeChanged();
    void OnNumberRevolutionsChanged ();
    void OnTSRChanged ();
    void OnRotationalSpeedChanged ();
    void OnDesingChanged();
    void OnStartupChanged();
    void OnAziChanged ();
    void OnTimestepChanged ();
    void OnWindTypeChanged ();
    void OnWakeCountChanged();
    void OnDiscTypeChanged ();
    void OnStandStillChanged();
    void OnShearChanged();

    void OnUseWindField();
    WindField* GetUsedWindField();
    void OnImportWindFile();
    void OnViewWindFile();

    void OnIncludeTowerChanged ();



};

#endif // QLLTSIMULATIONCREATORDIALOG_H
