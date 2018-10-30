/****************************************************************************

    QFEMDock Class
        Copyright (C) 2014 David Marten david.marten@tu-berlin.de

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

#ifndef QFEMDOCK_H
#define QFEMDOCK_H


//class LineButton;
//class QFEMModule;
class QLabel;
class QComboBox;
class QLineEdit;
class QTableView;
class QPushButton;
class QCheckBox;
class QGroupBox;
class QHBoxLayout;
class QSlider;
class QStandardItemModel;
class QStackedWidget;
class QTabWidget;
class QProgressDialog;

#include "../ScrolledDock.h"
#include "../StoreAssociatedComboBox_include.h"
class ColorButton;
class LineButton;
class NumberEdit;
class StructDelegate;
class FloatEditDelegate;
class QFEMModule;
class BladeStructure;
class BladeStructureLoading;


class QFEMDock : public ScrolledDock
{
	friend class QFEMModule;
	friend class StructDelegate;
	Q_OBJECT

private:
    QLabel *m_pctrlStructureLabel, *m_pctrlShellELabel, *m_pctrlShellRhoLabel, *m_pctrlIntELabel, *m_pctrlIntRhoLabel, *m_pctrlNameLabel, *m_pctrlOmegaLabel, *m_pctrlLoadingNameLabel;
	QComboBox *m_pctrlStructureCombobox, *m_pctrlInnerMaterial, *m_pctrlShellMaterial;
    NumberEdit *m_pctrlShellELineEdit, *m_pctrlShellRhoLineEdit, *m_pctrlIntELineEdit, *m_pctrlIntRhoLineEdit, *m_pctrlDesignOmega;
    QLineEdit *m_pctrlNameEdit, *m_pctrlLoadingNameEdit;
    QTableView *m_pctrlStructureTableView, *m_pctrlStructureTable, *m_LoadingTableView, *m_LoadingTable;
    QPushButton *m_pctrlNew, *m_pctrlEdit, *m_pctrlDelete, *m_pctrlSave, *m_pctrlRename, *m_pctrlCancel, *m_pctrlAllSections, *m_pctrlAlignSparMaxThickness;
    QCheckBox *m_pctrlPerspective, *m_pctrlSurfaces, *m_pctrlTopSurface, *m_pctrlInternal, *m_pctrlOutline, *m_pctrlAxes, *m_pctrlRotor;
    ColorButton *m_pctrlshellColor, *m_pctrlsparColor;
    QGroupBox *viewWidget, *ModesWidget, *CurvesBox;
    QCheckBox *AbsoluteSpar, *AbsoluteShell;
    QHBoxLayout *ModesTypesLayout;
	LineButton *m_simulationLineButton;
    QCheckBox *m_showCheckBox, *m_showCurveCheckBox, *m_showPointsCheckBox;

    QPushButton *m_newLoading, *m_deleteLoading, *m_editLoading, *m_saveLoading, *m_cancelLoading, *m_importLoading;

    QComboBox *m_pctrlModeTypeSelecta, *m_pctrlModeNumberSelecta;
    QSlider *m_modeSlider;

    QStandardItemModel *m_pStructModel, *m_pLoadingModel;
	StructDelegate *m_pBladeDelegate;
    FloatEditDelegate *m_pFloatDelegate;

    TDataComboBox *m_TDataBox;
    TBEMDataComboBox *m_TBEMDataBox;
    QComboBox *m_WindspeedBox;

	QList<QString> MaterialsList;
	QList<double> RhoList;
	QList<double> EModList;
    bool m_LoadingShown;


    bool InitStructureDialog(BladeStructure *pStructure);
	void InitStructureTable();

    bool InitLoadingDialog(BladeStructureLoading *pLoading);
    void InitLoadingTable();
    void CheckButtons();


	int m_iSection;


public:
	QFEMModule *m_module;
	QFEMDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QFEMModule *module);
    QStackedWidget *structureWidget;
    QTabWidget *m_tabwidget;

    bool m_cancelCalculation;
    int m_progressStep;
    int m_progressStepShown;

    QProgressDialog *m_progressDialog;  // showing the QFEM calculation progress
    BladeStructureLoading *m_Loading;

private slots:
	void OnNew();
	void OnEdit();
	void OnDelete();
	void OnSave();
	void OnRename();
	void OnCancel();
	void OnAllSections();
    void onProgressDialogCanceled();
    void onWindFieldProgress();
    void OnResize();

    void OnImportLoading();
    void OnFillWindspeeds();

    void OnTabChanged(int i);
    void OnNewLoading();
    void OnEditLoading();
    void OnDeleteLoading();
    void OnSaveLoading();
    void OnCancelLoading();
    void OnLoadingCellChanged();
    void ReadLoadingSectionData(int sel);
    void ReadLoadingParams();
    void FillLoadingDataTable();
    void FillLoadingTableRow(int row);
    void onLineButtonClicked();
    void onShowCheckBoxCanged();
    void onShowPointsCheckBoxCanged();
    void onShowCurveCheckBoxCanged();
    void SetShownBladeStructure(BladeStructure *structure);


};

#endif // QFEMDOCK_H
