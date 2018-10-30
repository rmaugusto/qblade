/****************************************************************************

    QFEMModule Class
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

#ifndef QFEMMODULE_H
#define QFEMMODULE_H

#include <QModelIndex>
#include "../Module.h"
#include "../Misc/GLLightDlg.h"
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

	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
	QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	virtual QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType);

    CBlade* GetCurrentBlade();
    BladeStructure* GetCurrentStructure();

    void CleanUp();


	void drawGL ();  // override from GLModule
	void overpaint (QPainter &painter);
    void addMainMenuEntries();
	QStringList prepareMissingObjectMessage (); 

    bool m_axes;
    bool QFEMCompleted;
    bool m_internalChecked;
    int m_modeType;
    int m_modeNumber;

    CBlade* getBlade();
    virtual void initView();  // override from Module

//private:
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

    QFEMTwoDContextMenu *m_twoDContextMenu;


public slots:
	virtual void onActivationActionTriggered();  // override from ModuleBase
	virtual void onModuleChanged ();  // override from ModuleBase
    void OnChangeCoordinates();

private slots:
	void OnSelChangeRotor();
	void OnSelChangeBladeStructure();
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
	
	void reloadFemGraphs () { reloadAllGraphCurves(); }	
};

extern QFEMModule *g_QFEMModule;

#endif // QFEMMODULE_H
