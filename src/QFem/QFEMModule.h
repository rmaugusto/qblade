#ifndef QFEMMODULE_H
#define QFEMMODULE_H

#include <QModelIndex>

#include "../Module.h"
#include "../Miarex/GLLightDlg.h"
class CBlade;
class BladeStructure;
class QFEMToolBar;
class QFEMMenu;
class QFEMDock;
class QFEMTwoDContextMenu;


class QFEMModule : public ModuleBase, public DualModule
{
	friend class QFEMDock;
	Q_OBJECT

public:
	QFEMModule(QMainWindow *mainWindow, QToolBar *toolbar);
	~QFEMModule ();
    BladeStructure* getShownBladeStructure () { return m_structure; }


	QStringList getAvailableGraphVariables();  // override from TwoDWidgetInterface

    CBlade* GetCurrentBlade();
    BladeStructure* GetCurrentStructure();


	virtual void onRedraw ();  // override from GLModule
    virtual void addMainMenuEntries();

    bool m_perspective;
    bool m_axes;
    bool m_positions;
    bool m_airfoils;
    bool QFEMCompleted;
    bool m_internalChecked;
    int modeType;
    int modeNumber;

   int numrender;
    CBlade* GetBlade();
    virtual void initView();  // override from Module

private:

    int m_DockWidth;

    QFEMToolBar* m_QFEMToolBar;
    QFEMDock* m_QFEMDock;

	CBlade *m_rotor;
    CBlade *m_deformed_rotor;
	BladeStructure *m_structure;
    QColor shellColor, sparColor;


    GLLightDlg m_GLLightDlg;
    bool TwoDView;
    bool GLView;

	bool m_bResetglGeom;
	bool m_bStructEdited;
	bool m_needToRerender;
    bool m_newSectionHighlight;

	virtual void configureGL ();  // override from GLModule
	void showAll();
	void hideAll();
	void setContextMenuGraphType(NewGraph::GraphType graphType);

	void SetCurrentSection(int section);
	void ReadParams();
	void ReadSectionData(int sel);
	void FillDataTable();
	void FillTableRow(int row);
	void DisableAll();
	void EnableAll();
	void ComputeGeometry();
	void render();
	void GLCreateGeom(CBlade *pWing);
    void GLRenderStressLegend();
    void UpdateModeNumber();
    void drawFrequency();
    void drawLoadingName();


    QFEMTwoDContextMenu *m_twoDContextMenu;


public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged ();  // override from ModuleBase
    void OnChangeCoordinates();

private slots:
	void OnSelChangeRotor();
	void OnSelChangeBladeStructure();
    void reloadAllGraphCurves();
    void OnSelChangeLoading();
	void OnItemClicked(const QModelIndex &index);
	void OnCellChanged();
	void OnStructTypeChanged();
	void OnSelChangeShellMaterial(int i);
	void OnSelChangeInnerMaterial(int i);
    void OnSparColor();
    void OnShellColor();
    void OnAlignSparAtMaxThickness();
    void UpdateGeomRerenderGL();
    void OnLightDlg();
    void OnSelChangeModeType(int i);
    void OnSelChangeModeNumber(int i);
    void DeformBlade();
    void OnHideWidgets();
    void OnGLView();
    void OnTwoDView();
    void OnCenterScene();
    void SliderPressed();
    void SliderReleased();
};

#endif // QFEMMODULE_H
