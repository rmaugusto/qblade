/****************************************************************************

    DMS Class
        Copyright (C) 2012 Juliane Wendler

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

#ifndef DMS_H
#define DMS_H

#include "../XBEM/BEM.h"
#include "SimuWidgetDMS.h"
#include "BladeDelegate2.h"
#include "DMSData.h"
#include "DData.h"
#include "TDMSData.h"
#include "CDMSData.h"
#include "DMSToolbar.h"
#include "../StoreAssociatedComboBox_include.h"



class QDMS : public QBEM
{
    Q_OBJECT

    friend class QBEM;
    friend class DData;
    friend class SimuWidgetDMS;
    friend class MainFrame;
    friend class OptimizeDlgVAWT;
    friend class CreateDMSDlg;
    friend class TwoDWidget;
	friend class DMSDock;

public:
    QDMS(QWidget *parent = NULL);

	QStringList prepareMissingObjectMessage();

	
private slots:
    void CheckButtons();
    void OnNewBlade();
    void OnSaveBlade();
    void InitBladeTable();
    void OnDeleteBlade();
    void OnEditBlade();
    void OnInsertAfter();
    void OnInsertBefore();
    void OnCellChanged();
    void OnOptimize();
    void OnScaleBlade();
    void OnStartRotorSimulation();
    void OnCreateRotorSimulation();
    void OnStartTurbineSimulation();
	void OnCreateTurbineSimulation();
	void OnCreateCharacteristicSimulation();
	void OnStartCharacteristicSimulation();
	void InitTurbineSimulationParams(TDMSData *bladedata);
	void InitBladeSimulationParams(DMSData *bladedata);
	void InitCharSimulationParams(CDMSData *bladedata);
    void UpdateRotorSimulation();
    void OnNewTurbine();
    void OnSaveTurbine();
    void OnEditTurbine();
    void OnDeleteTurbine();
    DData* GetBladeData(QString Lambda);
    DData* GetTurbineBladeData(QString Windspeed);
    void UpdateBladeData();
    void UpdateTurbineBladeData();
    void OnSelChangeBladeData(int i);
	void OnSelChangeRotorSimulation();
	void OnSelChangeTurbineSimulation();
    void OnSelChangeTurbineBladeData(int i);
    void OnSelChangeHeightData(int i);
    void OnSelChangeTurbineHeightData(int i);
    void SetRotorGraphTitles(Graph* pGraph);
    void SetPowerGraphTitles(Graph* pGraph);
    void OnGraphSettings();
    void InitTurbineData(TData *pTData);
	void UpdateUnits();
	void OnDeleteRotorSim();
	void OnDeleteCharSim();
    void OnDeleteTurbineSim();
	void UpdateCharacteristicsSimulation();
	void OnSelChangeCharSimulation();
	void SetCharGraphTitles(Graph* pGraph);
    void OnCenterScene();
    void OnBladeColor();
    void OnSectionColor();
    void OnDeleteSection();


public slots:
    void onModuleChanged ();  // NM will hide this module if no longer active
	
	void drawGL ();  // NM new functions equal to the interface that GLModule offers
    void OnResize();
	void OnBladeView();
	void OnRotorsimView();
	void OnCharView();
    void OnTurbineView();
    void UpdateBlades();
    void OnSelChangeWing(int i);
    void FillComboBoxes(bool bEnable = true);
    void SetCurveParams();
    void OnShowAllRotorCurves();
    void OnHideAllRotorCurves();
    void OnBladeGraph();
    void OnRotorGraph();
	void OnAziGraph();
	void OnSetWeibull();
	//void OnSetWeibullA(double A);
	//void OnSetWeibullK(double K);
    void PaintCharLegend(QPoint place, int bottom, QPainter &painter);
    void PaintCharacteristicsGraphs(QPainter &painter);
    void PaintRotorGraphs(QPainter &painter);
    void PaintPowerGraphs(QPainter &painter);
    void PaintPowerLegend(QPoint place, int bottom, QPainter &painter);
    void PaintView(QPainter &painter);
    void UpdateCurve();
    void OnShowPoints();
    void OnShowCurve();
    void OnShowOpPoint();
	void UpdateTurbines();
	void OnSelChangeTurbine(int i);
    void InvertedClicked();
    void OnLengthHeightChanged();
    void OnHubValChanged();


protected:
    void LoadSettings(QSettings *pSettings);
    void SaveSettings(QSettings *pSettings);
    void ScaleSpan(double NewSpan);
    void ShiftSpan(double NewShift);
    void ScaleChord(double NewChord);
    void SetOffset(double NewChord);
    void SetTwist(double NewChord);

private:

    void Connect();
    void SetupLayout();
    void EnableAllButtons();
    void DisableAllButtons();
    void FillTableRow(int row);
    bool InitDialog(CBlade *pWing);
    void ReadSectionData(int sel);
    CBlade * GetWing(QString WingName);
    void CreateRotorCurves();
    void CheckTurbineButtons();
    void * GetBladeVariable(void *data, int iVar);
    void * GetRotorVariable(void *data, int iVar);
    void * GetAziVariable(void *data, int iVar);
    void UpdateTurbineSimulation();
    void CreatePowerCurves();
    void * GetTurbineRotorVariable(void *Data, int iVar);
    void * GetTurbineBladeVariable(void *Data, int iVar);
    void * GetTurbineAziVariable(void *Data, int iVar);
    void FillRotorCurve(CCurve *pCurve, void *Data, int XVar, int YVar);
    void FillPowerCurve(CCurve *pCurve, void *Data, int XVar, int YVar);
    void CheckWing();
	void UpdateCharacteristicsMatrix();
	void CreateCharacteristicsCurves();
	void FillCharacteristicCurve(CCurve *pCurve, void *Data, int XVar, int YVar, int num_param);
    float *** GetCharVariable(void *Data, int iVar);
	void * GetWeibullXVariable(int iVar);
	void * GetWeibullYVariable(void *Data, int iVar);
	TData * GetTurbine(QString m_TurbineName);
    void GLCallViewLists();


	DMSToolbar *m_DMSToolBar;

    DData *m_pDData;
    DMSData *m_pDMSData;
    TDMSData *m_pTDMSData;
	CDMSData *m_pCDMSData;
    DData *m_pTurbineDData;
	QList <CDMSData *> s_poaCDMSData;

    void *m_pSimuWidgetDMS;
    BladeDelegate2 *m_pBladeDelegate;

    NumberEdit *m_pctrlHeight;
    QLabel *m_pctrlHeightLabel, *m_pctrlHeightUnitLabel, *m_pctrlBladesAndHeightLabel;

    QLabel *RotorHeightLabel, *RotorHeight;
    QLabel *TurbineHeightLabel, *TurbineHeight;
    QLabel *TurbineOffsetLabel, *TurbineOffset;
    QLabel *TurbineRadiusLabel, *TurbineRadius;
    QLabel *TurbineSweptAreaLabel, *TurbineSweptArea;
    NumberEdit *m_pctrlOffset;
    QLabel *m_pctrlOffsetLabel;
	QLabel *length0, *Area1, *Length0, *Length2, *Length3;
    QCheckBox *m_pctrlInvertBox;

	QDoubleSpinBox *m_pctrlWk2, *m_pctrlWA2;
	QDoubleSpinBox *m_pctrlPMk2, *m_pctrlPMA2;

    NumberEdit *hubEdit;
    QButtonGroup *m_heightLengthGroup;


    // variables for the dialog default values //
    bool dlg_powerlaw;
    bool dlg_constant;
    bool dlg_logarithmic;
    double dlg_exponent;
    double dlg_roughness;

    int selected_height;
	int same_height;

    bool m_bisHeight;


};


extern QDMS *g_qdms;

#endif // DMS_H
