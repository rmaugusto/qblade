/****************************************************************************

    QLLTDock Class
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

#ifndef QLLTDOCK_H
#define QLLTDOCK_H

#include <QObject>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressDialog>
#include <QThread>
#include <QDebug>
#include <QLabel>
#include <QDockWidget>
#include <QSpinBox>
#include "QLLTCutPlane.h"
#include "../StoreAssociatedComboBox_include.h"
#include "../ScrolledDock.h"
#include "../Misc/NumberEdit.h"
#include "QLLTSimulation.h"



class LineButton;
class QLLTModule;

class QLLTDock : public ScrolledDock
{

    Q_OBJECT

private:
    class QLLTThread : public QThread
    {
    public:
        QLLTSimulation *simulation;

    private:
        void run () { simulation->onStartAnalysis(); }
    };

    QLLTThread *m_QLLTThread;


public:
    QLLTDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QLLTModule *module);

    void setShownLLTSimulation(QLLTSimulation *newSimulation);

    QPushButton *m_colorStrength, *m_colorAge, *m_showText, *m_shed,*m_nodes, *m_singleblade,*m_trail,*stopButton, *continueButton, *m_addPlane, *m_deleteAllPlanes, *m_exportPlane, *m_exportVelVolume, *m_exportAllPlanes, *m_canceldeletePlane;
    QCheckBox   *m_showPointsCheckBox, *m_showCurveCheckBox, *m_showCheckBox;
    QPushButton  *m_perspectiveView, *m_showWindfield, *m_showVelVectors, *m_showCutPlanes, *m_showRotor, *m_showCoordinates, *m_showModel;
    LineButton *m_simulationLineButton;
    QButtonGroup *m_colorMapButton, *m_componentButton;
    QDoubleSpinBox *m_x_cut, *m_y_cut, *m_z_cut, *m_x_rot, *m_y_rot, *m_z_rot, *m_length, *m_width, *m_sectionEdit, *m_range;
    QSpinBox *m_X_res, *m_Y_res, *m_GroupSizeBox;
    QCheckBox *m_allTimeSteps;
    QDoubleSpinBox *m_linesize, *m_pointsize;

    QLabel *m_cutPlaneTime, *m_absoluteSection, *m_GroupSizeLabel;

    LLTCutPlaneComboBox *m_VAWTCutPlaneBox, *m_HAWTCutPlaneBox;

    void adjustShowCheckBox();
    void SetRange(double range){ m_range->setValue(range); }
    double GetRange() { return m_range->value(); }


private:

    QPushButton *m_startSimulationButton;
    QPushButton *m_editCopyButton, *m_renameButton, *m_newButton, *m_deleteButton;

    QLLTModule *m_module;
    QLLTSimulation *m_shownLLTSimulation;
    QProgressBar *m_progress;
    QComboBox *m_UseDevice;
    QGroupBox *m_stylebox, *m_vizbox, *cutVizBox, *m_cutBox, *m_sectionBox;

    void WriteVelFieldToFile(QString fileName, CVector*** velocities, CVector*** positions, int XR, int YR, int ZR, double time);
    void WriteBladeGeomToFile(QString fileName, int timestep);
    void OnReportConvergenceProblems();
    void OnReportAbort();


public slots:
    void updateProgress(int i);
    void onSimFinished();
    void onSimStarted();
    void onReplayStopped();
    void onReplayStarted();
    QComboBox* GetDeviceBox(){ return m_UseDevice; }

    void OnTwoDView();
    void OnGLView();
    void OnHAWTView();
    void OnVAWTView();
    void OnAge();
    void OnStrength();



private slots:
    void onStartSimulationButtonClicked();
    void onContinueSimulationButtonClicked();
    void onNewButtonClicked();
    void onEditCopyButtonClicked ();
    void onRenameButtonClicked ();
    void onDeleteButtonClicked ();
    void onLineButtonClicked ();
    void onShowCheckBoxCanged ();
    void onShowPointsCheckBoxCanged ();
    void onShowCurveCheckBoxCanged ();
    void onSectionChanged(double section);
    void onUseDeviceChanged(int i);

    void OnCreateCutPlane();
    void OnExportVelField();
    void OnComputeCutPlane();
    void OnCancelCutPlane();
    void OnExportAllPlanes();
    void OnExportPlane();

};

#endif // QLLTDOCK_H
