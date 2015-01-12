#ifndef QFEMDOCK_H
#define QFEMDOCK_H

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
#include <QTableView>
#include <QStackedWidget>
#include <QStandardItemModel>
#include "../XWidgets.h"
#include "../ScrolledDock.h"
#include "QFEMModule.h"
#include "StructDelegate.h"
#include "QProgressDialog"
#include "../Misc/FloatEditDelegate.h"

class LineButton;
class QFEMModule;


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
    QCheckBox *m_pctrlPerspective, *m_pctrlSurfaces, *m_pctrlTopSurface, *m_pctrlInternal, *m_pctrlOutline, *m_pctrlAxes, *m_pctrlPositions, *m_pctrlFoilNames, *m_pctrlRotor;
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

	void initView();

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
