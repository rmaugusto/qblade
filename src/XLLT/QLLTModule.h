/****************************************************************************

    QLLTModule Class
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

#ifndef QLLTMODULE_H
#define QLLTMODULE_H

#include <QModelIndex>
#include "../Module.h"
#include "../Params.h"
#include "../XBEM/Blade.h"
#include "../XUnsteadyBEM/WindField.h"
#include "../Misc/GLLightDlg.h"
#include "../XBEM/360Polar.h"
#include "QLLTCutPlane.h"
#include "QLLTDock.h"

class QLLTSimulation;
class QLLTTwoDContextMenu;
class QLLTToolBar;
class QLLTSimulation;


class QLLTModule : public ModuleBase, public DualModule
{
    friend class QLLTSimulation;
    Q_OBJECT

public:
    QLLTModule(QMainWindow *mainWindow, QToolBar *toolbar);
    ~QLLTModule ();

	virtual void addMainMenuEntries();
	QList<NewCurve*> prepareCurves (QString xAxis, QString yAxis, NewGraph::GraphType graphType,
									NewGraph::GraphType graphTypeMulti);
    QStringList getAvailableGraphVariables(bool xAxis);  // override from TwoDWidgetInterface
	virtual QPair<ShowAsGraphInterface*,int> getHighlightDot(NewGraph::GraphType graphType);
	
    void drawGL ();  // override from GLModule
	void overpaint (QPainter &painter);
	virtual QStringList prepareMissingObjectMessage();
    virtual void initView();  // override from Module
    int getGroupSize(){ return m_QLLTDock->m_GroupSizeBox->value(); }
    void setGroupSize(int i){ m_QLLTDock->m_GroupSizeBox->setValue(i); }
    void ChangeHAWTVAWTView(bool isVawt);
    void setShownTimeForAllSimulations();
    void ResetTimestepSlider();
    void EnableButtons();
    void DisableButtons();
    QLLTToolBar* GetQLLTToolBar() { return m_QLLTToolBar; }
    QLLTDock* GetQLLTDock() { return m_QLLTDock; }
    GLWidget* getGLWidget(){ return m_glWidget; }

    int   GetDeviceType(){ return m_QLLTDock->GetDeviceBox()->currentIndex(); }
    void  AddDeviceType(QString strong){ return m_QLLTDock->GetDeviceBox()->addItem(strong); }


    QLLTSimulation* GetQLLTSim() { return m_QLLTSim; }
    QLLTCutPlane* GetCutPlane() {return m_QLLTCutPlane; }
    void SetCutPlane(QLLTCutPlane* plane) { m_QLLTCutPlane = plane; }
    void setShownLLTSimulation (QLLTSimulation *newShownLLTSimulation, bool forceReload = false);
    QLLTSimulation* getShownQLLTSimulation () { return m_QLLTSim; }
    bool IsCutPlaneRendered() { return CutPlaneRendered; }


    bool m_bisVAWT;


private:
    bool GLView, TwoDView;
    bool CutPlaneRendered;
    bool m_bComponentChanged;
    QLLTSimulation *m_QLLTSim;
    QLLTTwoDContextMenu *m_QLLTContextMenu;
    QLLTToolBar *m_QLLTToolBar;
    QLLTDock *m_QLLTDock;
    QLLTCutPlane *m_QLLTCutPlane;

    virtual void configureGL ();  // override from GLModule
    void showAll();
    void hideAll();
    void OnCenterScene();


    void GLCreateRotorMesh();
    void GLCreateWakeMesh();

public slots:
    virtual void onActivationActionTriggered();  // override from ModuleBase
    virtual void onModuleChanged ();  // override from ModuleBase
    void UpdateView();
    void onStopSimulation();
    void forceReRender();
    void UpdateDockToolbar();
    void OnDeleteCutPlane();
    void OnDeleteAllCutPlanes();
    void OnUpdateCutPlane();
    void OnStopReplay();
    void OnRenderCutPlanes();
    void OnComponentChanged() { m_bComponentChanged = true; }
	
	void reloadTimeGraphs () { reloadForGraphType(NewGraph::QLLTTimeGraph); }
	void reloadBladeGraphs () { reloadForGraphType(NewGraph::QLLTBladeGraph); }
    void reloadAllGraphs () { reloadAllGraphCurves(); }

private slots:
    void OnSelChangeLLTSimulation();
    void OnSelChangeCutPlane();
    void OnSelChangeVAWTLLTSimulation();

    void OnGLView();
    void OnTwoDView();
};

extern QLLTModule *g_QLLTModule;

#endif // QLLTMODULE_H
