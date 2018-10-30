/****************************************************************************

    BEM Class
        Copyright (C) 2010 David Marten david.marten@tu-berlin.de

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

#include "BEM.h"

#include <QWidget>
#include "../GLWidget.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Misc/NumberEdit.h"
#include "../XDirect/XDirect.h"
#include "BData.h"
#include "BEMData.h"
#include "../Graph/GraphDlg.cpp"
#include "ExportGeomDlg.h"

#include "../TwoDWidget.h"
#include <math.h>
#include <QDebug>
#include <qstring.h>
#include <complex>
#include "TData.h"
#include "OptimizeDlg.h"
#include "CreateBEMDlg.h"
#include "TBEMData.h"
#include "CBEMData.h"
#include "BladeScaleDlg.h"
#include "Edit360PolarDlg.h"
#include "PrescribedValuesDlg.h"
#include "../Design/AFoil.h"
#include "AboutBEM.h"
#include "CircularFoilDlg.h"
#include "../StoreAssociatedComboBox.h"
#include <QProgressDialog>
#include "../ScrolledDock.h"
#include "../XDMS/DMS.h"
#include "360Polar.h"
#include "../GlobalFunctions.h"
#include "PolarSelectionDialog.h"


#define SIDEPOINTS 51


using namespace std;

extern bool ObjectIsEdited;


QBEM::QBEM(QWidget *parent)
    : QWidget(parent)
{

    any_number = QRegExp("^\\-?\\+?[0-9]+(\\.[0-9]+)?(e?E?\\-?\\+?[0-9]+)?$"); //regexp for a positive or negative floating number...


	/////////////// new NM ///////////////
	if (!g_qbem) {
		g_qbem = this;
	}
	/////////// end new NM ///////////////	
    m_CurveStyle = 0;
    m_CurveWidth = 1;
    m_CurveColor = QColor(0,0,0);
    selected_windspeed = -1;
    selected_lambda = 0;
    m_widthfrac = 6;

    m_360Graph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph1.SetXMin(-0.0);
    m_360Graph1.SetXMax( 0.1);
    m_360Graph1.SetYMin(-0.01);
    m_360Graph1.SetYMax( 0.01);
    m_360Graph1.SetType(0);
    m_360Graph1.SetMargin(50);
    m_360Graph1.SetXVariable(0);
    m_360Graph1.SetYVariable(1);
    m_360Graph1.SetGraphName(tr("360 Graph 1"));

    m_360Graph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph2.SetXMin(-0.0);
    m_360Graph2.SetXMax( 0.1);
    m_360Graph2.SetYMin(-0.01);
    m_360Graph2.SetYMax( 0.01);
    m_360Graph2.SetType(0);
    m_360Graph2.SetMargin(50);
    m_360Graph2.SetXVariable(0);
    m_360Graph2.SetYVariable(2);
    m_360Graph2.SetGraphName(tr("360 Graph 2"));

    m_360Graph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph3.SetXMin(-0.0);
    m_360Graph3.SetXMax( 0.1);
    m_360Graph3.SetYMin(-0.01);
    m_360Graph3.SetYMax( 0.01);
    m_360Graph3.SetType(0);
    m_360Graph3.SetMargin(50);
    m_360Graph3.SetXVariable(0);
    m_360Graph3.SetYVariable(2);
    m_360Graph3.SetGraphName(tr("360 Graph 3"));

    m_360Graph4.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph4.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_360Graph4.SetXMin(-0.0);
    m_360Graph4.SetXMax( 0.1);
    m_360Graph4.SetYMin(-0.01);
    m_360Graph4.SetYMax( 0.01);
    m_360Graph4.SetType(0);
    m_360Graph4.SetMargin(50);
    m_360Graph4.SetXVariable(0);
    m_360Graph4.SetYVariable(2);
    m_360Graph4.SetGraphName(tr("360 Graph 4"));


    m_RotorGraph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph1.SetXMin(-0.0);
	m_RotorGraph1.SetXMax( 0.1);
	m_RotorGraph1.SetYMin(-0.01);
	m_RotorGraph1.SetYMax( 0.01);
    m_RotorGraph1.SetType(0);
    m_RotorGraph1.SetMargin(50);
    m_RotorGraph1.SetXVariable(4);
    m_RotorGraph1.SetYVariable(0);
    m_RotorGraph1.SetGraphName(tr("Char Graph 1"));
    m_RotorGraph1.m_Type=0;


    m_RotorGraph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph2.SetXMin(-0.0);
    m_RotorGraph2.SetXMax( 0.1);
    m_RotorGraph2.SetYMin(-0.01);
	m_RotorGraph2.SetYMax( 0.01);
    m_RotorGraph2.SetType(0);
    m_RotorGraph2.SetMargin(50);
    m_RotorGraph2.SetXVariable(4);
    m_RotorGraph2.SetYVariable(1);
    m_RotorGraph2.SetGraphName(tr("Char Graph 2"));
    m_RotorGraph2.m_Type=0;


    m_RotorGraph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph3.SetXMin(-0.0);
    m_RotorGraph3.SetXMax( 0.1);
    m_RotorGraph3.SetYMin(-0.01);
	m_RotorGraph3.SetYMax( 0.01);
    m_RotorGraph3.SetType(0);
    m_RotorGraph3.SetMargin(50);
    m_RotorGraph3.SetXVariable(1);
    m_RotorGraph3.SetYVariable(0);
    m_RotorGraph3.SetGraphName(tr("Char Graph 3"));
    m_RotorGraph3.m_Type=1;



    m_PowerGraph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph1.SetXMin(-0.0);
    m_PowerGraph1.SetXMax( 0.1);
    m_PowerGraph1.SetYMin(-0.01);
    m_PowerGraph1.SetYMax( 0.01);
    m_PowerGraph1.SetType(0);
    m_PowerGraph1.SetMargin(50);
    m_PowerGraph1.SetXVariable(4);
    m_PowerGraph1.SetYVariable(0);
    m_PowerGraph1.SetGraphName(tr("Power Graph 1"));
    m_PowerGraph1.m_Type=0;




    m_PowerGraph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph2.SetXMin(-0.0);
    m_PowerGraph2.SetXMax( 0.1);
    m_PowerGraph2.SetYMin(-0.01);
    m_PowerGraph2.SetYMax( 0.01);
    m_PowerGraph2.SetType(0);
    m_PowerGraph2.SetMargin(50);
    m_PowerGraph2.SetXVariable(4);
    m_PowerGraph2.SetYVariable(1);
    m_PowerGraph2.SetGraphName(tr("Power Graph 2"));
    m_PowerGraph2.m_Type=0;



    m_PowerGraph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph3.SetXMin(-0.0);
    m_PowerGraph3.SetXMax( 0.1);
    m_PowerGraph3.SetYMin(-0.01);
    m_PowerGraph3.SetYMax( 0.01);
    m_PowerGraph3.SetType(0);
    m_PowerGraph3.SetMargin(50);
    m_PowerGraph3.SetXVariable(12);
    m_PowerGraph3.SetYVariable(10);
    m_PowerGraph3.SetGraphName(tr("Power Graph 3"));
    m_PowerGraph3.m_Type=1;



    m_CharGraph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph1.SetXMin(-0.0);
    m_CharGraph1.SetXMax( 0.1);
    m_CharGraph1.SetYMin(-0.01);
    m_CharGraph1.SetYMax( 0.01);
    m_CharGraph1.SetType(0);
    m_CharGraph1.SetMargin(50);
    m_CharGraph1.SetXVariable(4);
    m_CharGraph1.SetYVariable(0);
    m_CharGraph1.SetGraphName(tr("Characteristic Graph 1"));
    m_CharGraph1.m_Type=0;
    m_CharGraph1.m_MainVar=0;
    m_CharGraph1.m_Param=1;


    m_CharGraph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph2.SetXMin(-0.0);
    m_CharGraph2.SetXMax( 0.1);
    m_CharGraph2.SetYMin(-0.01);
    m_CharGraph2.SetYMax( 0.01);
    m_CharGraph2.SetType(0);
    m_CharGraph2.SetMargin(50);
    m_CharGraph2.SetXVariable(5);
    m_CharGraph2.SetYVariable(0);
    m_CharGraph2.SetGraphName(tr("Characteristic Graph 2"));
    m_CharGraph2.m_Type=0;
    m_CharGraph2.m_MainVar=1;
    m_CharGraph2.m_Param=0;


    m_CharGraph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph3.SetXMin(-0.0);
    m_CharGraph3.SetXMax( 0.1);
    m_CharGraph3.SetYMin(-0.01);
    m_CharGraph3.SetYMax( 0.01);
    m_CharGraph3.SetType(0);
    m_CharGraph3.SetMargin(50);
    m_CharGraph3.SetXVariable(5);
    m_CharGraph3.SetYVariable(2);
    m_CharGraph3.SetGraphName(tr("Characteristic Graph 3"));
    m_CharGraph3.m_Type=0;
    m_CharGraph3.m_MainVar=1;
    m_CharGraph3.m_Param=0;

    m_CharGraph4.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph4.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_CharGraph4.SetXMin(-0.0);
    m_CharGraph4.SetXMax( 0.1);
    m_CharGraph4.SetYMin(-0.01);
    m_CharGraph4.SetYMax( 0.01);
    m_CharGraph4.SetType(0);
    m_CharGraph4.SetMargin(50);
    m_CharGraph4.SetXVariable(6);
    m_CharGraph4.SetYVariable(0);
    m_CharGraph4.SetGraphName(tr("Characteristic Graph 4"));
    m_CharGraph4.m_Type=0;
    m_CharGraph4.m_MainVar=2;
    m_CharGraph4.m_Param=0;

	SetPowerGraphTitles(&m_PowerGraph1);
	SetPowerGraphTitles(&m_PowerGraph2);
	SetPowerGraphTitles(&m_PowerGraph3);
	//new code JW//
	SetWeibullGraphTitles(&m_PowerGraph1);
	SetWeibullGraphTitles(&m_PowerGraph2);
	SetWeibullGraphTitles(&m_PowerGraph3);
	//end new code JW //
    SetRotorGraphTitles(&m_RotorGraph1);
    SetRotorGraphTitles(&m_RotorGraph2);
    SetRotorGraphTitles(&m_RotorGraph3);
    SetPolarGraphTitles(&m_360Graph1);
    SetPolarGraphTitles(&m_360Graph2);
    SetPolarGraphTitles(&m_360Graph3);
    SetPolarGraphTitles(&m_360Graph4);
    SetCharGraphTitles(&m_CharGraph1);
    SetCharGraphTitles(&m_CharGraph2);
    SetCharGraphTitles(&m_CharGraph3);
    SetCharGraphTitles(&m_CharGraph4);

    pitchwindspeeds.append(0);
    rotwindspeeds.append(0);
    rotspeeds.append(200);
    pitchangles.append(0);







    dlg_lambda = 7;

    dlg_relax = 0.35;
    dlg_rho = 1.225;
    dlg_epsilon = 0.001;
    dlg_iterations = 100;
    dlg_elements = 40;
    dlg_tiploss = false;
    dlg_rootloss = false;
    dlg_3dcorrection = false;
    dlg_interpolation = false;
    dlg_newtiploss = false;
    dlg_newrootloss = false;

    dlg_lambdastart     = 1;
    dlg_lambdaend       = 10;
    dlg_lambdadelta     = 0.5;
    dlg_windspeed       = 7;


    dlg_windstart       = 1;
    dlg_windend         = 20;
    dlg_winddelta       = 0.5;


    dlg_windstart2      = 1;
    dlg_windend2        = 20;
    dlg_winddelta2      = 0.5;

    dlg_pitchstart      = 0;
    dlg_pitchend        = 10;
    dlg_pitchdelta      = 1;

    dlg_rotstart        = 200;
    dlg_rotend          = 500;
    dlg_rotdelta        = 100;

    selected_wind = 0;
    selected_rot = 0;
    selected_pitch = 0;



    dlg_visc = 1.6468e-5;

    m_PitchOld = 0;
    pitch_new = 0;


    m_CD90 = 1.8;
    m_AR=1.8;//JW variable


	m_bAbsoluteBlade = false;

    m_pBlade=NULL;
    m_pTData=NULL;
    m_pCurFoil = NULL;
    m_pCurPolar = NULL;
    m_pCurGraph = NULL;
    m_pWingModel = NULL;
    m_pBladeDelegate = NULL;
    m_pCur360Polar = NULL;
    m_pBladeAxisModel = NULL;
	m_pBladeData = NULL;
	m_pTurbineBData = NULL;
	m_pBData = NULL;
	m_pBEMData = NULL;
	m_pTBEMData = NULL;
	m_pCBEMData = NULL;

    m_bShowOpPoint              = true;
    m_bRightSide                = false;
    m_bResetglGeom              = true;
    m_bShowLight                = false;
    m_bCrossPoint               = false;
    m_bOutline                  = true;
    m_bglLight                  = true;
    m_bSurfaces                 = true;
    m_bResetglLegend            = false;
    m_bResetglSectionHighlight  = true;
    m_b360PolarChanged          = true;
    m_bNew360Polar              = false;
    m_bDecompose                = false;
    m_bXPressed                 = false;
    m_bYPressed                 = false;
    m_WingEdited                = false;
    m_TurbineEdited             = false;
    m_bIsolateBladeCurve        = false;
    m_bCompareBladeCurve        = false;
    m_bIs2DScaleSet             = false;
    m_bAutoScales               = false;
    m_bHideWidgets              = false;
    m_bSingleGraphs             = false;
    m_bAdvancedEdit             = false;
    m_bStallModel               = false;


    m_GLScale       = 1.0;
    m_ClipPlanePos  = 15.0;
    m_glScaled      = 1.0;
    m_OutlineWidth  = 1;
    m_OutlineStyle  = 0;
    m_OutlineColor  = QColor(0,0,0);

	g_mainFrame->setIView(POLARVIEW,BEM);


    m_rCltRect.setWidth(0);
    m_rCltRect.setHeight(0);
    m_rSingleRect.setWidth(0);
    m_rSingleRect.setHeight(0);

    m_glViewportTrans.x  = 0.0;
    m_glViewportTrans.y  = 0.0;
    m_glViewportTrans.z  = 0.0;

    m_LastPoint.setX(0);
    m_LastPoint.setY(0);
    m_PointDown.setX(0);
    m_PointDown.setY(0);


    /////////////// new NM ///////////////
    // connect signals
    connect (g_mainFrame, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));
	/////////// end new NM ///////////////

	m_A.clear();
	m_k.clear();
}

QStringList QBEM::prepareMissingObjectMessage() {
	switch (g_mainFrame->m_iView) {
	case BLADEVIEW:
		return CBlade::prepareMissingObjectMessage(false);
	case POLARVIEW:
		return C360Polar::prepareMissingObjectMessage();
	case BEMSIMVIEW:
		return BEMData::prepareMissingObjectMessage();
	case TURBINEVIEW:
		return TBEMData::prepareMissingObjectMessage();
	case CHARSIMVIEW:
		return CBEMData::prepareMissingObjectMessage();
	default:
		return QStringList("unknown view");
	}
}

double QBEM::CD90(CFoil *pFoil, double alpha)
{
    double res;
    res=m_CD90-1.46*pFoil->m_fThickness/2+1.46*pFoil->m_fCamber*sin(alpha/360*2*PI);
    return res;
}

double QBEM::CDPlate(double alpha)
{
    double res;
    res=CD90(m_pCurFoil,alpha)*pow(sin(alpha/360*2*PI),2);
    return res;
}

void QBEM::CheckButtons()
{
    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    m_pctrlBladeCoordinates->setChecked(!m_bAbsoluteBlade);
    m_pctrlBladeCoordinates2->setChecked(!m_bAbsoluteBlade);
    m_pctrlBladeCoordinates2->setVisible(m_pBlade);

    m_BEMToolBar->m_polarComboBox->setEnabled(!m_bNew360Polar && !m_bDecompose && m_BEMToolBar->m_polarComboBox->count());
    m_BEMToolBar->m_polar360ComboBox->setEnabled(!m_bNew360Polar && !m_bDecompose && m_BEMToolBar->m_polar360ComboBox->count());
    m_BEMToolBar->m_foilComboBox->setEnabled(!m_bNew360Polar && !m_bDecompose && m_BEMToolBar->m_foilComboBox->count());


    UpdateUnits();
    ///context menu///
	//g_mainFrame->DeleteCurrentCharSimAct->setVisible(true);
	g_mainFrame->current360PolarMenu->setEnabled(m_pCur360Polar);
    g_mainFrame->currentBladeMenu->setEnabled(m_pBlade);
    g_mainFrame->currentBladeMenu2->setEnabled(m_pBlade);
	g_mainFrame->currentTurbineMenu->setEnabled(m_pTData);
	g_mainFrame->DeleteCurrentRotorSimAct->setEnabled(m_pBEMData);
	g_mainFrame->DeleteCurrentCharSimAct->setEnabled(m_pCBEMData);
	g_mainFrame->DeleteCurrentTurbineSimAct->setEnabled(m_pTBEMData);
	g_mainFrame->DeleteCurrentTurbineAct->setEnabled(m_pTData);
    g_mainFrame->ExportCurrentRotorAct->setEnabled(m_pBlade);  // new code NM
    g_mainFrame->ExportCurrentRotorAeroDynAct->setEnabled(m_pBlade);  // new code NM
    g_mainFrame->DeleteCurrentBladeAct->setEnabled(m_pBlade);
    g_mainFrame->ExportBladeGeomAct->setEnabled(m_pBlade);
    g_mainFrame->OnImportBladeGeometry->setEnabled(m_pCur360Polar);
    g_mainFrame->Export360PolarAct->setEnabled(m_pCur360Polar);
    g_mainFrame->ExportBladeTableAct->setEnabled(m_pBlade);
    g_mainFrame->ExportCurrentRotorAct->setVisible(true);
    g_mainFrame->ExportBladeTableAct->setVisible(true);
    g_mainFrame->OnImportBladeGeometry->setVisible(true);
    g_mainFrame->ExportCurrentRotorAeroDynAct->setVisible(true);


    ///

    InitTurbineData(m_pTData);

    ///////enable or disable according to data present in simuwidget

    pSimuWidget->m_pctrlDefineTurbineSim->setEnabled(m_pTData);
	pSimuWidget->m_pctrlStartTurbineSim->setEnabled(m_pTBEMData);
    pSimuWidget->m_pctrlDeleteTBEM->setEnabled(m_pTBEMData);

    pSimuWidget->m_pctrlWind1->setEnabled(m_pTBEMData);
    pSimuWidget->m_pctrlWind2->setEnabled(m_pTBEMData);
    pSimuWidget->m_pctrlWindDelta->setEnabled(m_pTBEMData);

    pSimuWidget->m_pctrlCreateBEM->setEnabled(m_pBlade);
    pSimuWidget->m_pctrlStartBEM->setEnabled(m_pBEMData);
    pSimuWidget->m_pctrlLSLineEdit->setEnabled(m_pBEMData);
    pSimuWidget->m_pctrlLELineEdit->setEnabled(m_pBEMData);
    pSimuWidget->m_pctrlLDLineEdit->setEnabled(m_pBEMData);
    pSimuWidget->m_pctrlWindspeed->setEnabled(m_pBEMData);
    pSimuWidget->m_pctrlDeleteBEM->setEnabled(m_pBEMData);



    pSimuWidget->CreateCharSim->setEnabled(m_pBlade);
	pSimuWidget->StartCharSim->setEnabled(m_pCBEMData);
    pSimuWidget->m_pctrlDeleteCBEM->setEnabled(m_pCBEMData);

    pSimuWidget->WindStart->setEnabled(m_pCBEMData);
    pSimuWidget->WindEnd->setEnabled(m_pCBEMData);
    pSimuWidget->WindDelta->setEnabled(m_pCBEMData);
    pSimuWidget->PitchStart->setEnabled(m_pCBEMData);
    pSimuWidget->PitchEnd->setEnabled(m_pCBEMData);
    pSimuWidget->PitchDelta->setEnabled(m_pCBEMData);
    pSimuWidget->RotStart->setEnabled(m_pCBEMData);
    pSimuWidget->RotEnd->setEnabled(m_pCBEMData);
    pSimuWidget->RotDelta->setEnabled(m_pCBEMData);
    pSimuWidget->WindFixed->setEnabled(m_pCBEMData);
    pSimuWidget->PitchFixed->setEnabled(m_pCBEMData);
    pSimuWidget->RotFixed->setEnabled(m_pCBEMData);

    //// is something fixed in simuwidget
    if (pSimuWidget->WindFixed->isChecked()) pSimuWidget->WindDelta->setDisabled(true);
    if (pSimuWidget->WindFixed->isChecked()) pSimuWidget->WindEnd->setDisabled(true);

    if (pSimuWidget->PitchFixed->isChecked())pSimuWidget->PitchDelta->setDisabled(true);
    if (pSimuWidget->PitchFixed->isChecked())pSimuWidget->PitchEnd->setDisabled(true);

    if (pSimuWidget->RotFixed->isChecked())pSimuWidget->RotDelta->setDisabled(true);
    if (pSimuWidget->RotFixed->isChecked())pSimuWidget->RotEnd->setDisabled(true);

    ///init values in simuwidget

    pSimuWidget->m_pctrlLSLineEdit->setValue(dlg_lambdastart);
    pSimuWidget->m_pctrlLELineEdit->setValue(dlg_lambdaend);
    pSimuWidget->m_pctrlLDLineEdit->setValue(dlg_lambdadelta);
    pSimuWidget->m_pctrlWindspeed->setValue(dlg_windspeed);


    pSimuWidget->m_pctrlWind1->setValue(dlg_windstart);
    pSimuWidget->m_pctrlWind2->setValue(dlg_windend);
    pSimuWidget->m_pctrlWindDelta->setValue(dlg_winddelta);

    pSimuWidget->WindStart->setValue(dlg_windstart2);
    pSimuWidget->WindEnd->setValue(dlg_windend2);
    pSimuWidget->WindDelta->setValue(dlg_winddelta2);

    pSimuWidget->PitchStart->setValue(dlg_pitchstart);
    pSimuWidget->PitchEnd->setValue(dlg_pitchend);
    pSimuWidget->PitchDelta->setValue(dlg_pitchdelta);

    pSimuWidget->RotStart->setValue(dlg_rotstart);
    pSimuWidget->RotEnd->setValue(dlg_rotend);
    pSimuWidget->RotDelta->setValue(dlg_rotdelta);


    m_pctrlNewTurbine->setEnabled(g_rotorStore.size());
    m_pctrlDeleteTurbine->setEnabled(m_pTData);

    m_pctrlEditTurbine->setEnabled(m_pTData);

    m_pctrlEditWing->setEnabled(m_pBlade);
    m_pctrlNewWing->setEnabled(g_360PolarStore.size());
    m_pctrlDeleteWing->setEnabled(m_pBlade);

    m_pctrlDecompose360->setEnabled(m_pCur360Polar);

    if (m_bDecompose) m_pctrlDecompose360 ->setEnabled(false);

    if (m_pCur360Polar){
        m_pctrlDelete360Polar->setEnabled(!m_bNew360Polar && !m_bDecompose);
        m_pctrlRename360Polar->setEnabled(!m_bNew360Polar && !m_bDecompose);
        IsDecomposed->setVisible(true);
        if (m_pCur360Polar->m_bisDecomposed) IsDecomposed->setText("Decomposed Polar");
        else IsDecomposed->setText("Not Decomposed");
        IsDecomposed->setVisible(!m_bNew360Polar && !m_bDecompose);
    }
    else{
        m_pctrlDelete360Polar->setEnabled(false);
        m_pctrlRename360Polar->setEnabled(false);
        IsDecomposed->setVisible(false);
    }




    if (m_pCurPolar)
    {
        m_pctrlNew360->setEnabled(!m_bNew360Polar && !m_bDecompose);
        if(m_pCurFoil->m_bisCircular) m_pctrlNew360->setEnabled(false);
        if(m_pCurFoil->m_bisCircular) m_pctrlDecompose360->setEnabled(false);

        if(m_pCurFoil->m_bisCircular) m_pctrlStallModelMontg->setEnabled(false);
        else m_pctrlStallModelMontg->setEnabled(!m_bNew360Polar && !m_bDecompose);
        if(m_pCurFoil->m_bisCircular) m_pctrlStallModelVit->setEnabled(false);
        else m_pctrlStallModelVit->setEnabled(!m_bNew360Polar&& !m_bDecompose);
    }
    else
    {
        m_pctrlNew360->setEnabled(false);
        m_pctrlStallModelVit->setEnabled(false);
        m_pctrlStallModelMontg->setEnabled(false);
    }
    m_pctrlSave360->setEnabled(m_bNew360Polar || m_bDecompose);
    m_pctrlCancel360->setEnabled(m_bNew360Polar || m_bDecompose);




    QString str;
	GetLengthUnit(str, g_mainFrame->m_LengthUnit);
    m_pctrlHubRadiusUnitLabel->setText(str);

    ////size policy assures the widgets are resized according to content

    pSimuWidget->middleControls->setCurrentIndex(2);
	pSimuWidget->middleControls->currentWidget()->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

	if (g_mainFrame->m_iView == BEMSIMVIEW) pSimuWidget->middleControls->setCurrentIndex(0);// new JW, old: m_iView
	if (g_mainFrame->m_iView == TURBINEVIEW) pSimuWidget->middleControls->setCurrentIndex(1);// new JW, old: m_iView
	if (g_mainFrame->m_iView == CHARSIMVIEW) pSimuWidget->middleControls->setCurrentIndex(2);// new JW, old: m_iView

	pSimuWidget->middleControls->currentWidget()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ///// new code JW /////
	g_mainFrame->AziGraphAct->setVisible(false);
    ///// new code JW /////

    if (m_pCurGraph){
        g_mainFrame->autoResetCurGraphScales->setChecked(!m_pCurGraph->m_autoReset);
    }

    if(!m_pCurGraph)
    {
        g_mainFrame->autoResetCurGraphScales->setChecked(false);

		g_mainFrame->BladeGraphAct->setEnabled(false);
		g_mainFrame->RotorGraphAct->setEnabled(false);
		g_mainFrame->WeibullGraphAct->setEnabled(false);

		g_mainFrame->BladeGraphAct->setChecked(false);
		g_mainFrame->RotorGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(false);

		g_mainFrame->MainWindAct->setEnabled(false);
		g_mainFrame->MainPitchAct->setEnabled(false);
		g_mainFrame->MainRotAct->setEnabled(false);
		g_mainFrame->ParamPitchAct->setEnabled(false);
		g_mainFrame->ParamWindAct->setEnabled(false);
		g_mainFrame->ParamRotAct->setEnabled(false);
		g_mainFrame->ParamNoneAct->setEnabled(false);


		g_mainFrame->MainWindAct->setChecked(false);
		g_mainFrame->MainPitchAct->setChecked(false);
		g_mainFrame->MainRotAct->setChecked(false);
		g_mainFrame->ParamPitchAct->setChecked(false);
		g_mainFrame->ParamWindAct->setChecked(false);
		g_mainFrame->ParamRotAct->setChecked(false);
		g_mainFrame->ParamNoneAct->setChecked(false);



    }
    else if(m_pCurGraph->m_Type == 1)
    {
		g_mainFrame->BladeGraphAct->setEnabled(true);
		g_mainFrame->RotorGraphAct->setEnabled(true);
		g_mainFrame->BladeGraphAct->setChecked(true);
		g_mainFrame->RotorGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(false);
	}
	else if(m_pCurGraph->m_Type == 0)
	{
		g_mainFrame->BladeGraphAct->setEnabled(true);
		g_mainFrame->RotorGraphAct->setEnabled(true);
		g_mainFrame->BladeGraphAct->setChecked(false);
		g_mainFrame->RotorGraphAct->setChecked(true);
		g_mainFrame->WeibullGraphAct->setChecked(false);
	}
	else if(m_pCurGraph->m_Type == 3)
	{
		g_mainFrame->BladeGraphAct->setEnabled(true);
		g_mainFrame->RotorGraphAct->setEnabled(true);
		g_mainFrame->BladeGraphAct->setChecked(false);
		g_mainFrame->RotorGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(true);
	}
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
		g_mainFrame->WeibullGraphAct->setEnabled(true);
	else
		g_mainFrame->WeibullGraphAct->setEnabled(false);

	if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCurGraph && m_pCBEMData)// new JW, old: m_iView
    {
		g_mainFrame->MainWindAct->setEnabled(true);
		g_mainFrame->MainPitchAct->setEnabled(true);
		g_mainFrame->MainRotAct->setEnabled(true);
		g_mainFrame->ParamPitchAct->setEnabled(m_pCurGraph->m_MainVar!=2);
		g_mainFrame->ParamWindAct->setEnabled(m_pCurGraph->m_MainVar!=0);
		g_mainFrame->ParamRotAct->setEnabled(m_pCurGraph->m_MainVar!=1);
		g_mainFrame->ParamNoneAct->setEnabled(true);

		g_mainFrame->MainWindAct->setChecked(m_pCurGraph->m_MainVar==0);
		g_mainFrame->MainRotAct->setChecked(m_pCurGraph->m_MainVar==1);
		g_mainFrame->MainPitchAct->setChecked(m_pCurGraph->m_MainVar==2);

		g_mainFrame->ParamWindAct->setChecked(m_pCurGraph->m_Param==0);
		g_mainFrame->ParamRotAct->setChecked(m_pCurGraph->m_Param==1);
		g_mainFrame->ParamPitchAct->setChecked(m_pCurGraph->m_Param==2);
		g_mainFrame->ParamNoneAct->setChecked(m_pCurGraph->m_Param==-1);


    }

	if (g_mainFrame->m_iApp == BEM)
    {


            if(!m_bHideWidgets)
            {
				if (g_mainFrame->m_iView==BLADEVIEW)// new JW, old: m_iView
                {
					mainWidget->setCurrentIndex(0);
                    if (m_WingEdited) bladeWidget->setCurrentIndex(1);
                    else if (!m_WingEdited) bladeWidget->setCurrentIndex(0);
                }
				else if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
                {
					mainWidget->setCurrentIndex(1);

                }
				else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
                {
                    if (m_TurbineEdited) mainWidget->setCurrentIndex(3);
                    else if (!m_TurbineEdited) mainWidget->setCurrentIndex(2);
                }
                else
                {
					mainWidget->setCurrentIndex(0);
                }

            }


			if (g_mainFrame->m_iView == BLADEVIEW)// new JW, old: m_iView
			{
			if (!m_bHideWidgets)g_mainFrame->m_pctrlBEMWidget->show();
			if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidget->hide();



            }
			else if (g_mainFrame->m_iView == POLARVIEW )// new JW, old: m_iView
            {
			if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidget->hide();
			if (!m_bHideWidgets)g_mainFrame->m_pctrlBEMWidget->show();
            }
			else if (g_mainFrame->m_iView == BEMSIMVIEW )// new JW, old: m_iView
			{
			if (!m_bHideWidgets)g_mainFrame->m_pctrlBEMWidget->hide();
			if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidget->show();
            }
			else if (g_mainFrame->m_iView == TURBINEVIEW )// new JW, old: m_iView
            {
			if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidget->show();
			if (!m_bHideWidgets)g_mainFrame->m_pctrlBEMWidget->show();
            }
			else if (g_mainFrame->m_iView == CHARSIMVIEW )// new JW, old: m_iView
            {
			if (!m_bHideWidgets)g_mainFrame->m_pctrlBEMWidget->hide();
			if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidget->show();
            }
            if (m_bHideWidgets)
            {
				g_mainFrame->m_pctrlBEMWidget->hide();
				g_mainFrame->m_pctrlSimuWidget->hide();
            }

    }



    if (!m_pCur360Polar) m_360Name->clear();

    ///////// JW modification //////////////////////
    m_pctrlA->setEnabled(m_bNew360Polar     && !m_bStallModel);
    m_pctrlB->setEnabled(m_bNew360Polar     && !m_bStallModel);
    m_pctrlAm->setEnabled(m_bNew360Polar    && !m_bStallModel);
    m_pctrlBm->setEnabled(m_bNew360Polar    && !m_bStallModel);
    m_pctrlCD90->setEnabled(m_bNew360Polar  && !m_bStallModel);

    SliderGroup->setVisible(m_bNew360Polar && !m_bStallModel);
    RangeGroup->setVisible(m_bNew360Polar);


    m_posStall->setEnabled(m_bNew360Polar || m_bDecompose);
    m_posSep->setEnabled(m_bNew360Polar || m_bDecompose);
    m_negStall->setEnabled(m_bNew360Polar || m_bDecompose);
    m_negSep->setEnabled(m_bNew360Polar || m_bDecompose);
    m_pos180Stall->setEnabled(m_bNew360Polar || m_bDecompose);
    m_pos180Sep->setEnabled(m_bNew360Polar || m_bDecompose);
    m_neg180Stall->setEnabled(m_bNew360Polar || m_bDecompose);
    m_neg180Sep->setEnabled(m_bNew360Polar || m_bDecompose);

    DecomposeGroup->setVisible(m_bNew360Polar || m_bDecompose);
    m_360Name->setEnabled(m_bNew360Polar    || m_bDecompose);


    /////////end JW modification //////////////////////
    ///////// new code JW //////////////////////
    ViternaGroup->setVisible(m_bNew360Polar   && m_bStallModel);
    /////////end new code JW //////////////////////


    m_pctrlSave360->setVisible(m_pctrlSave360->isEnabled());
    m_pctrlCancel360->setVisible(m_pctrlCancel360->isEnabled());
    m_pctrlNew360->setVisible(!m_bNew360Polar && !m_bDecompose);
    m_pctrlDelete360Polar->setVisible(!m_bNew360Polar && !m_bDecompose);
    m_pctrlDecompose360->setVisible(!m_bNew360Polar && !m_bDecompose);
    m_pctrlRename360Polar->setVisible(!m_bNew360Polar && !m_bDecompose);

    SetCurveParams();



}

void QBEM::CheckWing()
{
    bool finished = true;
    double max = -10000;
    int station=0;
    QString text, var, var2;

    for (int i=0;i<=m_pBlade->m_NPanel;i++)
    {
        if (m_pBlade->m_Polar.at(i) == "-----") finished = false;
        if (m_pBlade->m_Airfoils.at(i) == "-----") finished = false;
        if (!m_pBlade->m_bisSinglePolar && m_pBlade->m_Range.at(i) == "-----") finished = false;
    }

    //// check if solidity < 1 at all stations
    for (int i=0;i<=m_pBlade->m_NPanel;i++)
    {
        if (m_pBlade->m_blades*m_pBlade->m_TChord[i]/(m_pBlade->m_TPos[i]*2*PI) > max)
        {
        max = m_pBlade->m_blades*m_pBlade->m_TChord[i]*cos(m_pBlade->m_TTwist[i]/360*2*PI)/(m_pBlade->m_TPos[i]*2*PI);
        station = i;
        }
    }

    if (max >= 1)
    {
        text = "<font color='Red'> Rotor solidity at section " +var2.sprintf("%.0f",double(station+1))+" is " +var.sprintf("%.2f",double(max))+ " (must be smaller 1) reduce chord or increase twist at section " +var2+"</font>";
        finished = false;
    }

    //// check if stations are in ascending order

    for (int i=0;i<m_pBlade->m_NPanel;i++)
    {
        if (m_pBlade->m_TPos[i] > m_pBlade->m_TPos[i+1])
        {
            text = "<font color='Red'>Positions are not in ascending order";
            finished = false;
        }
    }

    //// check if stations are not at the same position

    for (int i=0;i<=m_pBlade->m_NPanel;i++)
    {
        for (int j=0;j<=m_pBlade->m_NPanel;j++)
        {
            if (i!=j)
            {

                if (fabs(m_pBlade->m_TPos[i] - m_pBlade->m_TPos[j]) <= 0.000001)
                {
                    text = "<font color='Red'> Two stations are at the same position";
                    finished = false;
                }
            }
        }
    }



    if (finished) text = "";

    m_pctrlSolidityLabel->setText(text);

    m_pctrlSave->setEnabled(finished);
    m_pctrlOptimize->setEnabled(finished);

}

void QBEM::CheckTurbineButtons()
{
    m_pctrlSwitch->setEnabled(m_pctrl2Step->isChecked());
    m_pctrlRot2->setEnabled(!m_pctrlFixed->isChecked());
    m_pctrlLambda->setEnabled(m_pctrlVariable->isChecked());

    m_pctrlRot1Label->show();
    m_pctrlRot2Label->show();
    m_pctrlFixedPitch->show();
    m_pctrlFixedPitchLabel->show();
    m_pctrlFixedPitchUnit->show();


    if (m_pctrlPitch->isChecked())
    {
        m_pctrlGenerator->show();
        m_pctrlGeneratorLabel->show();
        power1->show();
    }
    else
    {
        m_pctrlGenerator->hide();
        m_pctrlGeneratorLabel->hide();
        power1->hide();
    }


    if (m_pctrlVariable->isChecked())
    {
        m_pctrlRot1Label->setText(tr("Rot. Speed Min"));
        m_pctrlRot2Label->setText(tr("Rot. Speed Max"));
        m_pctrlSwitchLabel->setText("");
        m_pctrlSwitchLabel->hide();

        m_pctrlLambdaLabel->setText("TSR at Design Point");
        m_pctrlLambdaLabel->show();

        m_pctrlRot1->show();
        m_pctrlRot2->show();

        rotspeed1->show();
        rotspeed2->show();

        m_pctrlSwitch->hide();

        speed3->hide();

        m_pctrlLambda->show();

    }
    if (m_pctrl2Step->isChecked())
    {
        m_pctrlRot1Label->setText(tr("Rot. Speed 1"));
        m_pctrlRot1Label->show();
        m_pctrlRot2Label->setText(tr("Rot. Speed 2"));
        m_pctrlRot2Label->show();

        m_pctrlSwitchLabel->setText("V Switch");
        m_pctrlSwitchLabel->show();

        m_pctrlLambdaLabel->setText("");
        m_pctrlLambdaLabel->hide();

        m_pctrlRot1->show();
        m_pctrlRot2->show();
        rotspeed1->show();
        rotspeed2->show();
        m_pctrlSwitch->show();
        speed3->show();

        m_pctrlLambda->hide();

    }
    if (m_pctrlFixed->isChecked())
    {
        m_pctrlRot1Label->setText(tr("Rot. Speed"));
        m_pctrlRot1Label->show();

        m_pctrlRot2Label->setText(tr(""));
        m_pctrlRot2Label->hide();

        m_pctrlSwitchLabel->setText("");
        m_pctrlSwitchLabel->hide();

        m_pctrlLambdaLabel->setText("");
        m_pctrlLambdaLabel->hide();

        m_pctrlRot1->show();
        rotspeed1->show();

        m_pctrlRot2->hide();
        rotspeed2->hide();

        m_pctrlSwitch->hide();
        speed3->hide();

        m_pctrlLambda->hide();

    }

    if (m_pctrlPrescribedPitch->isChecked())
    {
        m_pctrlPitchList->show();
        m_pctrlFixedPitch->hide();
        m_pctrlFixedPitchLabel->hide();
        m_pctrlFixedPitchUnit->hide();
    }
    else
    {
        m_pctrlPitchList->hide();
    }

    if (m_pctrlPrescribedRot->isChecked())
    {
        m_pctrlRotList->show();
        m_pctrlRot1->hide();
        m_pctrlRot2->hide();
        m_pctrlRot1Label->hide();
        m_pctrlRot2Label->hide();
        m_pctrlSwitch->hide();
        m_pctrlSwitchLabel->hide();
        rotspeed1->hide();
        rotspeed2->hide();
        speed3->hide();
        m_pctrlLambdaLabel->hide();
        m_pctrlLambda->hide();
    }
    else
    {
        m_pctrlRotList->hide();
    }

}

void QBEM::ComputeDecomposition(){

    double AoA, AoA_sep1 = m_negSep->value(), AoA_sep2 = m_posSep->value(), AoA_sep3 = m_pos180Sep->value(), AoA_sep4 = m_neg180Sep->value(), AoA0;
    double AoA_fs1 = m_negStall->value() , AoA_fs2 = m_posStall->value(), AoA_fs3 = m_pos180Stall->value(), AoA_fs4 = m_neg180Stall->value();
    double Cl_st, Cl_lin, fst;    

    m_bDecompose = true;

    m_pCur360Polar->m_Cl_att.clear();
    m_pCur360Polar->m_Cl_sep.clear();
    m_pCur360Polar->m_fst.clear();
    AoA0=m_pCur360Polar->alpha_zero;

    // calculate linear lift
    // determine separation and fully separated AoA
    for (int i=0; i < m_pCur360Polar->m_Alpha.size(); i++)
    {
        AoA = m_pCur360Polar->m_Alpha.at(i);
        Cl_st = m_pCur360Polar->m_Cl.at(i);

        if (fabs(Cl_st) < 0.0001)
            Cl_st=0;

        if (AoA>-90 && AoA<90)
        {
            Cl_lin = m_pCur360Polar->slope*(AoA-AoA0);
            fst=pow(2*sqrt(Cl_st/Cl_lin * fabs((AoA-AoA0)*PI/180/(sin((AoA-AoA0)*PI/180))))-1, 2);
        }
        else if (AoA<=-90)
        {
            Cl_lin = 0.8*m_pCur360Polar->slope*(AoA-AoA0+180);
            fst=pow(2*sqrt(Cl_st/Cl_lin * fabs((AoA-AoA0+180)*PI/180/(sin((AoA-AoA0+180)*PI/180))))-1, 2);
        }
            else
        {
            Cl_lin = 0.8*m_pCur360Polar->slope*(AoA-AoA0-180);
            fst=pow(2*sqrt(Cl_st/Cl_lin * fabs((AoA-AoA0-180)*PI/180/(sin((AoA-AoA0-180)*PI/180))))-1, 2);
        }

//        // separation point function
//        fst=pow(2*sqrt(Cl_st/Cl_lin)-1, 2);

        // fully attached
        if (fst>1 || (AoA>AoA_sep1 && AoA<AoA_sep2) || AoA>AoA_sep3 || AoA<AoA_sep4 )
        {
            fst = 1;
            m_pCur360Polar->m_Cl_att.append(Cl_st);
            m_pCur360Polar->m_Cl_sep.append(0.5*Cl_st);
        }
        // fully separated
        else if (fst<0 || (AoA<AoA_fs1 && AoA>AoA_fs4) || (AoA>AoA_fs2 && AoA<AoA_fs3))
        {
            fst = 0;
            m_pCur360Polar->m_Cl_att.append(Cl_lin);
            m_pCur360Polar->m_Cl_sep.append(Cl_st);
        }
        // transition region
        else
        {
            m_pCur360Polar->m_Cl_att.append(Cl_lin);
            m_pCur360Polar->m_Cl_sep.append((Cl_st-Cl_lin*fst)/(1-fst));
        }

        if (std::isnan(fst)) fst = 0;

        m_pCur360Polar->m_fst.append(fst);
    }

    m_pCur360Polar->m_bisDecomposed = true;

}

void QBEM::ComputePolar(){
    if (!m_bStallModel) Compute360Polar();
    else ComputeViterna360Polar();
}


void QBEM::Compute360Polar()
{
    double CLzero = 0, CL180 = 0;
    double alphazero = 0, slope = 0, deltaCD = 0;
    double deltaalpha = 1;

    int posalphamax = 0;

    double a1plus,a1minus,a2plus,a2minus;
    double CL1plus, CL1minus,CL2plus,CL2minus;
    double f1plus,f1minus,f2plus,f2minus;

    double am, G , k;


    slope = m_Slope->value();

    m_pCur360Polar->m_Alpha.clear();
    m_pCur360Polar->m_Cl.clear();
    m_pCur360Polar->m_Cd.clear();
    m_pCur360Polar->m_Cm.clear();
    m_pCur360Polar->slope = slope;


    alphazero=m_pCur360Polar->alpha_zero;
    posalphamax=m_pCur360Polar->posalphamax;
    CLzero=m_pCur360Polar->CLzero;

    //start constructing the positive extrapolation

        m_pctrlA->setMaximum(30);
        m_pctrlA->setMinimum(-10);

    if (m_pctrlA->value()+posalphamax < m_pCurPolar->m_Alpha.size() && m_pctrlA->value()+posalphamax  >= 0)
    {
    a1plus = m_pCurPolar->m_Alpha.at(posalphamax+m_pctrlA->value());
    CL1plus= m_pCurPolar->m_Cl.at(posalphamax+m_pctrlA->value());
    }
    else
    {
    a1plus = (posalphamax+m_pctrlA->value())*deltaalpha;
    CL1plus = PlateFlow(alphazero,CLzero, a1plus)+0.03;
    }


    if ((posalphamax+m_pctrlB->value()+m_pctrlA->value()) < m_pCurPolar->m_Alpha.size() && (posalphamax+m_pctrlB->value()+m_pctrlA->value()) >= 0)
    {
    a2plus = m_pCurPolar->m_Alpha.at(posalphamax+m_pctrlB->value()+m_pctrlA->value());
    CL2plus =m_pCurPolar->m_Cl.at(posalphamax+m_pctrlB->value()+m_pctrlA->value());
    }
    else
    {
    a2plus = (posalphamax+m_pctrlB->value()+m_pctrlA->value())*deltaalpha;
    CL2plus = PlateFlow(alphazero,CLzero, a2plus)+0.03;
    }

    f1plus=((CL1plus-PlateFlow(alphazero, CLzero, a1plus))/(PotFlow(CLzero, slope, a1plus)-PlateFlow(alphazero, CLzero, a1plus)));
    f2plus=((CL2plus-PlateFlow(alphazero, CLzero, a2plus))/(PotFlow(CLzero, slope, a2plus)-PlateFlow(alphazero, CLzero, a2plus)));

    if (f1plus == 1) f1plus += 10e-6;
    if (f2plus == 1) f2plus += 10e-6;


    G=pow((fabs((1/f1plus-1)/(1/f2plus-1))),0.25);


    am=(a1plus-G*a2plus)/(1-G);


    k=(1/f2plus-1)*1/pow((a2plus-am),4);

    /////////rear end flying first

    double deltaCL, Re, slope2 ,am2, k2, G2 ;

    CL180 = PlateFlow(alphazero, CLzero, 180);

    slope2 = 0.8*slope;
    Re=m_pCurPolar->m_Reynolds;
    deltaCL=1.324*pow((1-exp(Re/1000000*(-0.2))), 0.7262);

    CL1plus=CL180-deltaCL;
    a1plus = 170+CL180/slope2;
    a2plus=a1plus-15;
    CL2plus = PlateFlow(alphazero, CLzero, a2plus)-0.01;


    f1plus=(CL1plus-PlateFlow(alphazero, CLzero, a1plus))/(PotFlow(CL180, slope2, a1plus-180)-PlateFlow(alphazero, CLzero, a1plus));
    f2plus=(CL2plus-PlateFlow(alphazero, CLzero, a2plus))/(PotFlow(CL180, slope2, a2plus-180)-PlateFlow(alphazero, CLzero, a2plus));

    G2=pow(fabs(((1/f1plus-1)/(1/f2plus-1))),0.25);

    am2=(a1plus-G2*a2plus)/(1-G2);



    k2=(1/f2plus-1)*1/pow((a2plus-am2),4);



    // construct the positive extrapolation

    double f,delta,alpha=int(1);

    while (alpha <= 180)

    {
        if (alpha < am2 - 70)
        {
        if (alpha<am)
        {
        delta=0;
        }
        else
        {
        delta=am-alpha;
        }
        f=1/(1+k*pow(delta,4));
        m_pCur360Polar->m_Alpha.append(alpha);
        m_pCur360Polar->m_Cl.append(f*PotFlow(CLzero,slope,alpha)+(1-f)*PlateFlow(alphazero, CLzero, alpha));

        }
        else if (alpha < am2)
        {
        delta=am2-alpha;
        f=1/(1+k2*pow(delta,4));
        m_pCur360Polar->m_Alpha.append(alpha);
        m_pCur360Polar->m_Cl.append(f*PotFlow(CL180,slope2,alpha-180)+(1-f)*PlateFlow(alphazero, CLzero, alpha));

        }
        else
        {
        m_pCur360Polar->m_Alpha.append(alpha);
        m_pCur360Polar->m_Cl.append(PotFlow(CL180,slope2,alpha-180));

        }

        //////CD Curve/////
        if (alpha<am)
        {
        delta=0;
        }
        else
        {
        delta=am-alpha;
        }
        f=1/(1+k*pow(delta,4));
        deltaCD=0.13*((f-1)*PotFlow(CLzero,slope,alpha)-(1-f)*PlateFlow(alphazero, CLzero, alpha));
        if (deltaCD <=0) deltaCD=0;
        m_pCur360Polar->m_Cd.append(f*(deltaCD+0.006+1.25*pow(m_pCurFoil->m_fThickness,2)/180*fabs(alpha))+(1-f)*CDPlate(alpha)+0.006);
        ////////////

        alpha+=deltaalpha;
    }



    // start constructing the negative extrapolation

m_pctrlAm->setMinimum(1);
m_pctrlAm->setMaximum(80);




a1minus = (-double(m_pctrlAm->value())/20-CLzero)/slope-4;
CL1minus= -double(m_pctrlAm->value())/20;


a2minus = a1minus-m_pctrlBm->value()*2;
CL2minus = PlateFlow(alphazero,CLzero, a2minus)-0.03;




f1minus=(CL1minus-PlateFlow(alphazero, CLzero, a1minus))/(PotFlow(CLzero, slope, a1minus)-PlateFlow(alphazero, CLzero, a1minus));
f2minus=(CL2minus-PlateFlow(alphazero, CLzero, a2minus))/(PotFlow(CLzero, slope, a2minus)-PlateFlow(alphazero, CLzero, a2minus));

G=pow(fabs((1/f1minus-1)/(1/f2minus-1)),0.25);


am=(a1minus-G*a2minus)/(1-G);


k=(1/f2minus-1)*1/pow((a2minus-am),4);



//////////////////rear end flying first//////////

CL1minus=CL180+deltaCL;
a1minus = -170+CL180/slope2;
a2minus=a1minus+15;
CL2minus = PlateFlow(alphazero, CLzero, a2minus)-0.01;

f1minus=(CL1minus-PlateFlow(alphazero, CLzero, a1minus))/(PotFlow(CL180, slope2, a1minus+180)-PlateFlow(alphazero, CLzero, a1minus));
f2minus=(CL2minus-PlateFlow(alphazero, CLzero, a2minus))/(PotFlow(CL180, slope2, a2minus+180)-PlateFlow(alphazero, CLzero, a2minus));

G2=pow(fabs(((1/f1minus-1)/(1/f2minus-1))),0.25);


am2=(a1minus-G2*a2minus)/(1-G2);



k2=(1/f2minus-1)*1/pow((a2minus-am2),4);




///////////create curve/////////////

alpha=int(0);

while (alpha >= -180)

{
    if (alpha > am2 + 70)
    {
    if (alpha > am)
    {
        delta=0;
    }
    else
    {
    delta=am-alpha;
    }
    f=1/(1+fabs(k*pow(delta,4)));
    m_pCur360Polar->m_Alpha.prepend(alpha);
    m_pCur360Polar->m_Cl.prepend(f*PotFlow(CLzero,slope,alpha)+(1-f)*PlateFlow(alphazero, CLzero, alpha));

    }
    else if (alpha > am2)
    {
    delta=am2-alpha;
    f=1/(1+fabs(k2*pow(delta,4)));
    m_pCur360Polar->m_Alpha.prepend(alpha);
    m_pCur360Polar->m_Cl.prepend(f*PotFlow(CL180,slope2,alpha+180)+(1-f)*PlateFlow(alphazero, CLzero, alpha));

    }
    else
    {
    m_pCur360Polar->m_Alpha.prepend(alpha);
    m_pCur360Polar->m_Cl.prepend(PotFlow(CL180,slope2,alpha+180));

    }

    //////CD Curve/////
    if (alpha > am)
    {
        delta=0;
    }
    else
    {
    delta=am-alpha;
    }
    f=1/(1+k*pow(delta,4));
    deltaCD=0.13*(PotFlow(CLzero,slope,alpha)-f*PotFlow(CLzero,slope,alpha)-(1-f)*PlateFlow(alphazero, CLzero, alpha));
    if (deltaCD <=0) deltaCD=0;
    m_pCur360Polar->m_Cd.prepend(f*(deltaCD+0.006+1.25*pow(m_pCurFoil->m_fThickness,2)/180*fabs(alpha))+(1-f)*CDPlate(alpha)+0.006);
    ////////////

    alpha=alpha-deltaalpha;
}



CombinePolars();

ComputeDecomposition();

}

////// new code JW //////////
void QBEM::ComputeViterna360Polar()
{
    double CLs1 = 0, CDs1 = 0, AoAs1 = 0;
    double CLs2 = 1000, CDs2 = 0, AoAs2 = 0;
    int posCLmax = 0, posCLmin = 0;

    m_pCur360Polar->m_Alpha.clear();
    m_pCur360Polar->m_Cl.clear();
    m_pCur360Polar->m_Cd.clear();
    m_pCur360Polar->m_Cm.clear();

    //get important variables from current polar
    for (int i=0; i<m_pCurPolar->m_Alpha.size(); i++)
    {
        if (m_pCurPolar->m_Alpha.at(i) > -25 && m_pCurPolar->m_Alpha.at(i) < 25)
        {
            // find stall position
            if (m_pCurPolar->m_Cl.at(i) > CLs1)
            {
                CLs1 = m_pCurPolar->m_Cl.at(i);
                CDs1 = m_pCurPolar->m_Cd.at(i);
                AoAs1 = m_pCurPolar->m_Alpha.at(i);
                posCLmax=i;
            }

            if (m_pCurPolar->m_Cl.at(i) < CLs2)
            {
                CLs2 = m_pCurPolar->m_Cl.at(i);
                CDs2 = m_pCurPolar->m_Cd.at(i);
                AoAs2 = m_pCurPolar->m_Alpha.at(i);
                posCLmin=i;
            }
        }

    }

    // Aspect Ratio:
    // AR = R/c;
    // Direct Foil Design: dimensionless foil, c=1.0
    // R...blade tip radius
    // c...blade chord

    double CDmax = m_AR;


    // positive extrapolation
    double deltaalpha=1,alpha=int(m_pCurPolar->m_Alpha.at(posCLmax)), A2, B2;

    B2 = CDs1-(CDmax*pow(sin(AoAs1/360*2*PI),2))/cos(AoAs1/360*2*PI);
    A2 = (CLs1-CDmax*sin(AoAs1/360*2*PI)*cos(AoAs1/360*2*PI))*(sin(AoAs1/360*2*PI))/(pow(cos(AoAs1/360*2*PI),2));

    while (alpha <= 90)
    {
        m_pCur360Polar->m_Alpha.append(alpha);
        m_pCur360Polar->m_Cd.append(CDmax*pow(sin(alpha/360*2*PI),2)+B2*cos(alpha/360*2*PI));
        m_pCur360Polar->m_Cl.append(CDmax/2.0*sin(2*alpha/360*2*PI)+A2*pow(cos(alpha/360*2*PI),2)/sin(alpha/360*2*PI));

        alpha=alpha+deltaalpha;
    }

    // 90 to 180deg flat plate
    int pos=int(m_pCur360Polar->m_Alpha.size())-1;
    for (int i=0; i<90; i++)
    {
        m_pCur360Polar->m_Alpha.append(alpha);
        m_pCur360Polar->m_Cd.append(FlatPlateCD(alpha, pos));
        m_pCur360Polar->m_Cl.append(FlatPlateCL(alpha, pos));

        alpha=alpha+deltaalpha;
    }

    // negative extrapolation
    alpha=int(m_pCurPolar->m_Alpha.at(posCLmin));

    B2 = CDs2-(CDmax*pow(sin(AoAs2/360*2*PI),2))/cos(AoAs2/360*2*PI);
    A2 = (CLs2-CDmax*sin(AoAs2/360*2*PI)*cos(AoAs2/360*2*PI))*(sin(AoAs2/360*2*PI))/(pow(cos(AoAs2/360*2*PI),2));

    while (alpha >= -90)
    {
        m_pCur360Polar->m_Alpha.prepend(alpha);
        m_pCur360Polar->m_Cd.prepend(CDmax*pow(sin(alpha/360*2*PI),2)+B2*cos(alpha/360*2*PI));
        m_pCur360Polar->m_Cl.prepend(CDmax/2.0*sin(2*alpha/360*2*PI)+A2*pow(cos(alpha/360*2*PI),2)/sin(alpha/360*2*PI));

        alpha=alpha-deltaalpha;
    }

    // -90 to -180deg flat plate
    pos = 0;
    for (int i=0; i<90; i++)
    {
        m_pCur360Polar->m_Alpha.prepend(alpha);
        m_pCur360Polar->m_Cd.prepend(FlatPlateCD(alpha, pos));
        m_pCur360Polar->m_Cl.prepend(FlatPlateCL(alpha, pos));

        alpha=alpha-deltaalpha;
        pos++;
    }

    CombinePolars();

    ComputeDecomposition();

}
////// end new code JW //////

void QBEM::ComputeGeometry(bool isVawt)
{
        // Computes the wing's characteristics from the panel data

        m_pBlade->CreateSurfaces(isVawt);
        m_pBlade->ComputeGeometry();

        for (int i=0;i<m_pBlade->m_StrutList.size();i++){
            m_pBlade->m_StrutList.at(i)->CreateSurfaces(1);
        }

        UpdateView();
}

void QBEM::ComputePolarVars()
{
    double CLmax=0, CLmin=100, CLabsmin=100, CLzero = 0;
    double alphazero = 0, slope = 0;
    double smallestalpha = 100, smallestAlpha=100;
    int posalphamax = 0;

    //get important variables from current polar
    for (int i=0; i<m_pCurPolar->m_Alpha.size(); i++)
    {

        if (m_pCurPolar->m_Alpha.at(i) > -25 && m_pCurPolar->m_Alpha.at(i) < 25)
        {
            if (m_pCurPolar->m_Cl.at(i) > CLmax)
            {
                CLmax = m_pCurPolar->m_Cl.at(i);
                posalphamax=i;
            }

            if (m_pCurPolar->m_Cl.at(i) < CLmin)
            {
                CLmin = m_pCurPolar->m_Cl.at(i);
            }

            if (fabs(m_pCurPolar->m_Alpha.at(i)) < smallestAlpha)
            {
                smallestAlpha = fabs(m_pCurPolar->m_Alpha.at(i));
                smallestalpha=m_pCurPolar->m_Alpha.at(i);
                if ((i+5)<m_pCurPolar->m_Cl.size() && (i-3) >= 0){
                slope = (m_pCurPolar->m_Cl.at(i+5)-m_pCurPolar->m_Cl.at(i-3))/(m_pCurPolar->m_Alpha.at(i+5)-m_pCurPolar->m_Alpha.at(i-3));
                }
                else slope = pow(PI,2)/90;// 2PI slope
            }
        }

    }

    for (int i=0; i<m_pCurPolar->m_Alpha.size(); i++)
    {
        if (m_pCurPolar->m_Alpha.at(i) > -25 && m_pCurPolar->m_Alpha.at(i) < 25)
        {
            if (m_pCurPolar->m_Alpha.at(i) == smallestalpha)
            {
                CLzero = m_pCurPolar->m_Cl.at(i)-slope*smallestalpha;
            }

            if (fabs(m_pCurPolar->m_Cl.at(i)) < CLabsmin)
            {
                CLabsmin=fabs(m_pCurPolar->m_Cl.at(i));
                if ((i+5)<m_pCurPolar->m_Cl.size() && (i-3) >= 0){
                slope = (m_pCurPolar->m_Cl.at(i+5)-m_pCurPolar->m_Cl.at(i-3))/(m_pCurPolar->m_Alpha.at(i+5)-m_pCurPolar->m_Alpha.at(i-3));
                }
                else slope = pow(PI,2)/90; // 2PI slope
                alphazero = m_pCurPolar->m_Alpha.at(i)-m_pCurPolar->m_Cl.at(i)/slope;
            }
        }
    }

    m_pCur360Polar->slope = slope;
    m_pCur360Polar->alpha_zero = alphazero;
    m_pCur360Polar->posalphamax = posalphamax;
    m_pCur360Polar->CLzero = CLzero;

}

void QBEM::Connect()
{
connect(m_pctrlInsertBefore, SIGNAL(clicked()),this, SLOT(OnInsertBefore()));
connect(m_pctrlInsertAfter, SIGNAL(clicked()),this, SLOT(OnInsertAfter()));
connect(m_pctrlDeleteSection, SIGNAL(clicked()),this, SLOT(OnDeleteSection()));

connect(m_pctrlWingColor, SIGNAL(clicked()),this, SLOT(OnBladeColor()));
connect(m_pctrlSectionColor, SIGNAL(clicked()),this, SLOT(OnSectionColor()));

connect(m_pctrlSurfaces, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlOutline, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlOutlineEdge, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlAirfoils, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlAxes, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlPositions, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
connect(m_pctrlFoilNames, SIGNAL(clicked()),this, SLOT(UpdateGeom()));

connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));

connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowPoints()));
connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));

connect(m_SingleMultiGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnSingleMultiPolarChanged()));




// viterna 360 polar
//---------------------------------------------------------------------------------
///////////// new code JW ////////////////////////////////////////////////
connect(m_pctrlStallModelVit, SIGNAL(clicked()), SLOT(OnStallModel()));
connect(m_pctrlStallModelMontg, SIGNAL(clicked()), SLOT(OnStallModel()));
connect(m_pctrlAR, SIGNAL(valueChanged(double)), SLOT(OnSetAR(double)));
///////////// end new code JW ////////////////////////////////////////////






connect(m_pctrlSave, SIGNAL(clicked()),this, SLOT (OnSaveBlade()));

connect(m_pctrlHubRadius,  SIGNAL(editingFinished()), this, SLOT(OnHubChanged()));
connect(m_pctrlBlades,  SIGNAL(valueChanged(int)), this, SLOT(OnCellChanged()));


connect(m_pctrlNew360, SIGNAL(clicked()),this, SLOT (OnNew360Polar()));
connect(m_pctrlDecompose360, SIGNAL(clicked()),this, SLOT (OnDecompose360Polar()));

connect(m_pctrlCancel360, SIGNAL(clicked()),this, SLOT (OnDiscard360Polar()));
connect(m_pctrlDelete360Polar, SIGNAL(clicked()),this, SLOT (OnDelete360Polar()));
connect(m_pctrlRename360Polar, SIGNAL(clicked()),this, SLOT (OnRename360Polar()));


connect(m_pctrlA, SIGNAL(valueChanged(int)), this, SLOT (Compute360Polar()));
connect(m_pctrlB, SIGNAL(valueChanged(int)), this, SLOT (Compute360Polar()));
connect(m_pctrlAm, SIGNAL(valueChanged(int)), this, SLOT (Compute360Polar()));
connect(m_pctrlBm, SIGNAL(valueChanged(int)), this, SLOT (Compute360Polar()));


connect(m_Slope, SIGNAL(valueChanged(double)), this, SLOT (ComputePolar()));
connect(m_posAoA, SIGNAL(valueChanged(double)), this, SLOT (ComputePolar()));
connect(m_negAoA, SIGNAL(valueChanged(double)), this, SLOT (ComputePolar()));


connect(m_posStall, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_posSep, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_negStall, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_negSep, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_pos180Stall, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_pos180Sep, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_neg180Stall, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));
connect(m_neg180Sep, SIGNAL(valueChanged(double)), this, SLOT (ComputeDecomposition()));

connect(m_posStall, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_posSep, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_negStall, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_negSep, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_pos180Stall, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_pos180Sep, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_neg180Stall, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_neg180Sep, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_Slope, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_posAoA, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));
connect(m_negAoA, SIGNAL(valueChanged(double)), this, SLOT (CreateSinglePolarCurve()));


connect(m_pctrlA, SIGNAL(valueChanged(int)), this, SLOT (CreateSinglePolarCurve()));
connect(m_pctrlB, SIGNAL(valueChanged(int)), this, SLOT (CreateSinglePolarCurve()));
connect(m_pctrlAm, SIGNAL(valueChanged(int)), this, SLOT (CreateSinglePolarCurve()));
connect(m_pctrlBm, SIGNAL(valueChanged(int)), this, SLOT (CreateSinglePolarCurve()));

connect(m_pctrlPitchBlade, SIGNAL(valueChanged(double)), this, SLOT (PitchBlade()));
connect(m_pctrlSave360, SIGNAL(clicked()), this, SLOT (OnSave360Polar()));



//---------------//
connect(m_pctrlNewWing, SIGNAL(clicked()), this, SLOT (OnNewBlade()));
connect(m_pctrlEditWing, SIGNAL(clicked()), this, SLOT (OnEditBlade()));
connect(m_pctrlDeleteWing, SIGNAL(clicked()), this, SLOT (OnDeleteBlade()));


connect(m_pctrlScale, SIGNAL(clicked()), this, SLOT (OnScaleBlade()));


connect(m_pctrlBladeCoordinates, SIGNAL(clicked()), SLOT(OnChangeCoordinates()));
connect(m_pctrlBladeCoordinates2, SIGNAL(clicked()), SLOT(BladeCoordsChanged()));




connect(m_pctrlPerspective, SIGNAL(clicked()), SLOT(onPerspectiveChanged()));

connect(m_pctrlShowTurbine, SIGNAL(clicked()), SLOT(UpdateView()));
connect(m_pctrlShowTurbine, SIGNAL(clicked()), SLOT(OnCenterScene()));




connect(m_pctrlPitch, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrlStall, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrlFixed, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrl2Step, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrlVariable, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrlPrescribedPitch, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
connect(m_pctrlPrescribedRot, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));



connect(m_pctrlSaveTurbine, SIGNAL(clicked()), SLOT(OnSaveTurbine()));
connect(m_pctrlDiscardTurbine, SIGNAL(clicked()), SLOT(OnDiscardTurbine()));

connect(m_pctrlNewTurbine, SIGNAL(clicked()), SLOT(OnNewTurbine()));
connect(m_pctrlEditTurbine, SIGNAL(clicked()), SLOT(OnEditTurbine()));
connect(m_pctrlDeleteTurbine, SIGNAL(clicked()), SLOT(OnDeleteTurbine()));


connect(m_pctrlOptimize, SIGNAL(clicked()), SLOT(OnOptimize()));
connect(m_pctrlBack, SIGNAL(clicked()), SLOT(OnDiscardBlade()));


// new code JW //
connect(m_pctrlWA, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
connect(m_pctrlWk, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
connect(m_pctrlPMA, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
connect(m_pctrlPMk, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
// end new code JW //

connect(m_pctrlCD90, SIGNAL(valueChanged(double)), SLOT(OnSetCD90(double)));

connect(m_pctrlAlignMaxThickness, SIGNAL(clicked()), SLOT(OnAlignMaxThickness()));

connect(m_pctrlPitchList, SIGNAL(clicked()), SLOT(OnPrescribePitch()));
connect(m_pctrlRotList, SIGNAL(clicked()), SLOT(OnPrescribeRot()));
}

void QBEM::CreateSinglePolarCurve(bool showPolar)
{
    m_360Graph1.DeleteCurves();
    m_360Graph2.DeleteCurves();
    m_360Graph3.DeleteCurves();
    m_360Graph4.DeleteCurves();

    m_360Graph1.ResetIfAuto();
    m_360Graph2.ResetIfAuto();
    m_360Graph3.ResetIfAuto();
    m_360Graph4.ResetIfAuto();

    if (m_pCurPolar && m_pCur360Polar)
    {
        if (m_pCurPolar->m_Alpha.size()>0 && showPolar)
        {
            if (!(m_360Graph1.GetXVariable() > 3) && !(m_360Graph1.GetYVariable() > 3)){
                CCurve* pPolarCurve = m_360Graph1.AddCurve();
//                pPolarCurve->ShowPoints(m_pCurPolar->m_bShowPoints);
//                pPolarCurve->SetStyle(m_pCurPolar->m_Style);
                pPolarCurve->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve->SetWidth(m_pCur360Polar->m_Width+2);
                FillPolarCurve(pPolarCurve, m_pCurPolar, m_360Graph1.GetXVariable(), m_360Graph1.GetYVariable());
                pPolarCurve->SetTitle(m_pCurPolar->getName());
            }

            if (!(m_360Graph2.GetXVariable() > 3) && !(m_360Graph2.GetYVariable() > 3)){
                CCurve* pPolarCurve = m_360Graph2.AddCurve();
//                pPolarCurve->ShowPoints(m_pCurPolar->m_bShowPoints);
//                pPolarCurve->SetStyle(m_pCurPolar->m_Style);
                pPolarCurve->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve->SetWidth(m_pCur360Polar->m_Width+2);
                FillPolarCurve(pPolarCurve, m_pCurPolar, m_360Graph2.GetXVariable(), m_360Graph2.GetYVariable());
                pPolarCurve->SetTitle(m_pCurPolar->getName());
            }
            if (!(m_360Graph3.GetXVariable() > 3) && !(m_360Graph3.GetYVariable() > 3)){
                CCurve* pPolarCurve = m_360Graph3.AddCurve();
//                pPolarCurve->ShowPoints(m_pCurPolar->m_bShowPoints);
//                pPolarCurve->SetStyle(m_pCurPolar->m_Style);
                pPolarCurve->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve->SetWidth(m_pCur360Polar->m_Width+2);
                FillPolarCurve(pPolarCurve, m_pCurPolar, m_360Graph3.GetXVariable(), m_360Graph3.GetYVariable());
                pPolarCurve->SetTitle(m_pCurPolar->getName());
            }

            if (!(m_360Graph4.GetXVariable() > 3) && !(m_360Graph4.GetYVariable() > 3)){
                CCurve* pPolarCurve = m_360Graph4.AddCurve();
//                pPolarCurve->ShowPoints(m_pCurPolar->m_bShowPoints);
//                pPolarCurve->SetStyle(m_pCurPolar->m_Style);
                pPolarCurve->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve->SetWidth(m_pCur360Polar->m_Width+2);
                FillPolarCurve(pPolarCurve, m_pCurPolar, m_360Graph4.GetXVariable(), m_360Graph4.GetYVariable());
                pPolarCurve->SetTitle(m_pCurPolar->getName());
            }
        }
    }

    if (m_pCur360Polar)
    {

            if (m_pCur360Polar->m_bIsVisible && m_pCur360Polar->m_Alpha.size()>0)
            {
                CCurve* pPolarCurve2 = m_360Graph1.AddCurve();

                pPolarCurve2->ShowPoints(m_pCur360Polar->m_bShowPoints);
                pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve2->SetWidth(m_pCur360Polar->m_Width);

                FillPolarCurve(pPolarCurve2, m_pCur360Polar, m_360Graph1.GetXVariable(), m_360Graph1.GetYVariable());
                pPolarCurve2->SetTitle(m_pCur360Polar->getName());

                pPolarCurve2= m_360Graph2.AddCurve();

                pPolarCurve2->ShowPoints(m_pCur360Polar->m_bShowPoints);
                pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve2->SetWidth(m_pCur360Polar->m_Width);
                FillPolarCurve(pPolarCurve2, m_pCur360Polar, m_360Graph2.GetXVariable(), m_360Graph2.GetYVariable());
                pPolarCurve2->SetTitle(m_pCur360Polar->getName());

                pPolarCurve2= m_360Graph3.AddCurve();

                pPolarCurve2->ShowPoints(m_pCur360Polar->m_bShowPoints);
                pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve2->SetWidth(m_pCur360Polar->m_Width);
                FillPolarCurve(pPolarCurve2, m_pCur360Polar, m_360Graph3.GetXVariable(), m_360Graph3.GetYVariable());
                pPolarCurve2->SetTitle(m_pCur360Polar->getName());

                pPolarCurve2= m_360Graph4.AddCurve();

                pPolarCurve2->ShowPoints(m_pCur360Polar->m_bShowPoints);
                pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                pPolarCurve2->SetWidth(m_pCur360Polar->m_Width);
                FillPolarCurve(pPolarCurve2, m_pCur360Polar, m_360Graph4.GetXVariable(), m_360Graph4.GetYVariable());
                pPolarCurve2->SetTitle(m_pCur360Polar->getName());

                if (m_pctrlA->value()+m_pCur360Polar->posalphamax < m_pCurPolar->m_Alpha.size() && m_pctrlA->value()+m_pCur360Polar->posalphamax  >= 0 && !m_bStallModel && showPolar && m_pctrlA->isEnabled()){

                    QList <double> *pX;
                    QList <double> *pY;
                    QList <double> ClCd;
                    if (!(m_360Graph1.GetXVariable() > 3) && !(m_360Graph1.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph1.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph1.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph1.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph1.GetXVariable() == 3 && m_360Graph1.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph1.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph1.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph2.GetXVariable() > 3) && !(m_360Graph2.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph2.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph2.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph2.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph2.GetXVariable() == 3 && m_360Graph2.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph2.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph2.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph3.GetXVariable() > 3) && !(m_360Graph3.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph3.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph3.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph3.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph3.GetXVariable() == 3 && m_360Graph3.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph3.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph3.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph4.GetXVariable() > 3) && !(m_360Graph4.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph4.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph4.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph4.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph4.GetXVariable() == 3 && m_360Graph4.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph4.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph4.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }
                }
                if (m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax < m_pCurPolar->m_Alpha.size() && m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax  >= 0 && !m_bStallModel && showPolar && m_pctrlB->isEnabled()){

                    QList <double> *pX;
                    QList <double> *pY;
                    QList <double> ClCd;
                    if (!(m_360Graph1.GetXVariable() > 3) && !(m_360Graph1.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph1.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph1.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph1.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph1.GetXVariable() == 3 && m_360Graph1.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph1.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph1.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph2.GetXVariable() > 3) && !(m_360Graph2.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph2.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph2.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph2.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph2.GetXVariable() == 3 && m_360Graph2.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph2.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph2.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph3.GetXVariable() > 3) && !(m_360Graph3.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph3.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph3.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph3.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph3.GetXVariable() == 3 && m_360Graph3.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph3.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph3.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }

                    if (!(m_360Graph4.GetXVariable() > 3) && !(m_360Graph4.GetYVariable() > 3)){
                        pPolarCurve2 = m_360Graph4.AddCurve();
                        pPolarCurve2->ShowPoints(true);
                        pPolarCurve2->SetStyle(m_pCur360Polar->m_Style);
                        pPolarCurve2->SetColor(m_pCur360Polar->m_Color);
                        pPolarCurve2->SetWidth(m_pCur360Polar->m_Width+3);
                        pX = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph4.GetXVariable());
                        pY = (QList <double> *) GetVariable(m_pCurPolar, m_360Graph4.GetYVariable());
                        for (int i=0;i<m_pCurPolar->m_Cl.size();i++)
                        {
                            ClCd.append(m_pCurPolar->m_Cl.at(i)/m_pCurPolar->m_Cd.at(i));
                        }
                        pPolarCurve2->SetSelected(-1);
                        int i = m_pctrlA->value()+m_pctrlB->value()+m_pCur360Polar->posalphamax;
                        if (m_360Graph4.GetXVariable() == 3 && m_360Graph4.GetYVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i),ClCd.at(i));
                        else if (m_360Graph4.GetXVariable() == 3) pPolarCurve2->AddPoint(ClCd.at(i), pY->at(i));
                        else if (m_360Graph4.GetYVariable() == 3) pPolarCurve2->AddPoint(pX->at(i), ClCd.at(i));
                        else pPolarCurve2->AddPoint(pX->at(i),pY->at(i));
                        pPolarCurve2->SetTitle(m_pCurPolar->getName()+"vizpoint");
                    }
                }


            }

    }

    if (m_b360PolarChanged)
    {
        m_360Graph1.SetAutoY(true);
        m_360Graph1.SetXMin(-180);
        m_360Graph1.SetXMax(180);

        m_360Graph2.SetAutoY(true);
        m_360Graph2.SetXMin(-180);
        m_360Graph2.SetXMax(180);

        m_360Graph3.SetAutoY(true);
        m_360Graph3.SetXMin(-180);
        m_360Graph3.SetXMax(180);

        m_360Graph4.SetAutoY(true);
        m_360Graph4.SetXMin(-180);
        m_360Graph4.SetXMax(180);

        m_b360PolarChanged = false;
    }
    UpdateView();
}

void QBEM::CreatePolarCurve()
{
    m_360Graph1.DeleteCurves();
    m_360Graph2.DeleteCurves();
    m_360Graph3.DeleteCurves();
    m_360Graph4.DeleteCurves();
	
    m_360Graph1.ResetIfAuto();
    m_360Graph2.ResetIfAuto();
    m_360Graph3.ResetIfAuto();
    m_360Graph4.ResetIfAuto();

    if (g_360PolarStore.size())
	{
        for (int i=0; i< g_360PolarStore.size();i++)
        {
            if (g_360PolarStore.at(i)->m_bIsVisible && g_360PolarStore.at(i)->m_Alpha.size()>0)
            {
                CCurve* pPolarCurve2 = m_360Graph1.AddCurve();

                pPolarCurve2->ShowPoints(g_360PolarStore.at(i)->m_bShowPoints);
                pPolarCurve2->SetStyle(g_360PolarStore.at(i)->m_Style);
                pPolarCurve2->SetColor(g_360PolarStore.at(i)->m_Color);
                pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width);
                if (g_360PolarStore.at(i) == m_pCur360Polar) pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width+2);


                FillPolarCurve(pPolarCurve2, g_360PolarStore.at(i), m_360Graph1.GetXVariable(), m_360Graph1.GetYVariable());
                pPolarCurve2->SetTitle(g_360PolarStore.at(i)->getName());


                pPolarCurve2= m_360Graph2.AddCurve();

                pPolarCurve2->ShowPoints(g_360PolarStore.at(i)->m_bShowPoints);
                pPolarCurve2->SetStyle(g_360PolarStore.at(i)->m_Style);
                pPolarCurve2->SetColor(g_360PolarStore.at(i)->m_Color);
                pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width);
                if (g_360PolarStore.at(i) == m_pCur360Polar) pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width+2);

                FillPolarCurve(pPolarCurve2, g_360PolarStore.at(i), m_360Graph2.GetXVariable(), m_360Graph2.GetYVariable());
                pPolarCurve2->SetTitle(g_360PolarStore.at(i)->getName());

                pPolarCurve2= m_360Graph3.AddCurve();

                pPolarCurve2->ShowPoints(g_360PolarStore.at(i)->m_bShowPoints);
                pPolarCurve2->SetStyle(g_360PolarStore.at(i)->m_Style);
                pPolarCurve2->SetColor(g_360PolarStore.at(i)->m_Color);
                pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width);
                if (g_360PolarStore.at(i) == m_pCur360Polar) pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width+2);

                FillPolarCurve(pPolarCurve2, g_360PolarStore.at(i), m_360Graph3.GetXVariable(), m_360Graph3.GetYVariable());
                pPolarCurve2->SetTitle(g_360PolarStore.at(i)->getName());

                pPolarCurve2= m_360Graph4.AddCurve();

                pPolarCurve2->ShowPoints(g_360PolarStore.at(i)->m_bShowPoints);
                pPolarCurve2->SetStyle(g_360PolarStore.at(i)->m_Style);
                pPolarCurve2->SetColor(g_360PolarStore.at(i)->m_Color);
                pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width);
                if (g_360PolarStore.at(i) == m_pCur360Polar) pPolarCurve2->SetWidth(g_360PolarStore.at(i)->m_Width+2);

                FillPolarCurve(pPolarCurve2, g_360PolarStore.at(i), m_360Graph4.GetXVariable(), m_360Graph4.GetYVariable());
                pPolarCurve2->SetTitle(g_360PolarStore.at(i)->getName());

            }
        }
	}
	
	if (m_b360PolarChanged)
	{
        m_360Graph1.SetAutoY(true);
        m_360Graph1.SetXMin(-180);
        m_360Graph1.SetXMax(180);
		
        m_360Graph2.SetAutoY(true);
        m_360Graph2.SetXMin(-180);
        m_360Graph2.SetXMax(180);

        m_360Graph3.SetAutoY(true);
        m_360Graph3.SetXMin(-180);
        m_360Graph3.SetXMax(180);

        m_360Graph4.SetAutoY(true);
        m_360Graph4.SetXMin(-180);
        m_360Graph4.SetXMax(180);
		
		m_b360PolarChanged = false;
	}
	UpdateView();
}

void QBEM::CreateRotorCurves()
{
    m_RotorGraph1.DeleteCurves();
    m_RotorGraph2.DeleteCurves();
    m_RotorGraph3.DeleteCurves();


	for (int i=0;i<g_bemdataStore.size();i++)
    {
        if (m_RotorGraph1.m_Type == 0)
        {

			if (g_bemdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_RotorGraph1.AddCurve();

			pCurve->ShowPoints(g_bemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_bemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_bemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_bemdataStore.at(i)->m_Width);
			FillRotorCurve(pCurve, g_bemdataStore.at(i), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
			pCurve->SetTitle(g_bemdataStore.at(i)->m_BEMName);
            }

        }
        else if (m_pBEMData && m_pBData)

        {

            if(m_bIsolateBladeCurve)
            {
                if (m_pBEMData->m_bIsVisible)
                {
                CCurve *pCurve = m_RotorGraph1.AddCurve();
                pCurve->ShowPoints(m_pBEMData->m_bShowPoints);
                pCurve->SetStyle(m_pBEMData->m_Style);
                pCurve->SetColor(m_pBEMData->m_Color);
                pCurve->SetWidth(m_pBEMData->m_Width);
                FillRotorCurve(pCurve, m_pBData, m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
                pCurve->SetTitle(m_pBData->m_BEMName);
                }

                if (m_bCompareBladeCurve)
                {
					 for (int k=0; k<g_bemdataStore.size();k++)
                        {
						for (int l=0;l<g_bemdataStore.at(k)->m_BData.size();l++)
                         {
						 if (g_bemdataStore.at(k)->m_BData.at(l)->lambdaglobal == m_pBData->lambdaglobal)
                            {
							if (g_bemdataStore.at(k)->m_bIsVisible)
                             {
                                CCurve *pCurve = m_RotorGraph1.AddCurve();

								pCurve->ShowPoints(g_bemdataStore.at(k)->m_bShowPoints);
								pCurve->SetStyle(g_bemdataStore.at(k)->m_Style);
								pCurve->SetColor(g_bemdataStore.at(k)->m_Color);
								pCurve->SetWidth(g_bemdataStore.at(k)->m_Width);
								FillRotorCurve(pCurve, g_bemdataStore.at(k)->m_BData.at(l), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
								pCurve->SetTitle(g_bemdataStore.at(k)->m_BData.at(l)->m_BEMName);

                             }
                            }
                         }
                        }
                }
            }
            else
            {
            for (int j=0; j< m_pBEMData->m_BData.size();j++)
                {

                CCurve *pCurve = m_RotorGraph1.AddCurve();

                pCurve->ShowPoints(m_pBEMData->m_BData.at(j)->m_bShowPoints);
                pCurve->SetStyle(m_pBEMData->m_BData.at(j)->m_Style);
                pCurve->SetColor(m_pBEMData->m_BData.at(j)->m_Color);
                if (m_pBEMData->m_BData.at(j)==m_pBData)
                {
                pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width+4);
                }
                else
                {
                pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width);
                }
                FillRotorCurve(pCurve, m_pBEMData->m_BData.at(j), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
                pCurve->SetTitle(m_pBEMData->m_BData.at(j)->m_BEMName);
                }
            }
        }
    }

	for (int i=0;i<g_bemdataStore.size();i++)
    {

        if (m_RotorGraph2.m_Type == 0)
        {

			if (g_bemdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_RotorGraph2.AddCurve();

			pCurve->ShowPoints(g_bemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_bemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_bemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_bemdataStore.at(i)->m_Width);
			FillRotorCurve(pCurve, g_bemdataStore.at(i), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
			pCurve->SetTitle(g_bemdataStore.at(i)->m_BEMName);
            }
        }
        else if (m_pBEMData && m_pBData)
        {
            if(m_bIsolateBladeCurve)
            {
                if (m_pBEMData->m_bIsVisible)
                {
                CCurve *pCurve = m_RotorGraph2.AddCurve();

                pCurve->ShowPoints(m_pBEMData->m_bShowPoints);
                pCurve->SetStyle(m_pBEMData->m_Style);
                pCurve->SetColor(m_pBEMData->m_Color);
                pCurve->SetWidth(m_pBEMData->m_Width);
                FillRotorCurve(pCurve, m_pBData, m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
                pCurve->SetTitle(m_pBData->m_BEMName);
                }

                if (m_bCompareBladeCurve)
                {
					 for (int k=0; k<g_bemdataStore.size();k++)
                        {
						for (int l=0;l<g_bemdataStore.at(k)->m_BData.size();l++)
                         {
						 if (g_bemdataStore.at(k)->m_BData.at(l)->lambdaglobal == m_pBData->lambdaglobal)
                            {
							if (g_bemdataStore.at(k)->m_bIsVisible)
                             {
                                CCurve *pCurve = m_RotorGraph2.AddCurve();

								pCurve->ShowPoints(g_bemdataStore.at(k)->m_bShowPoints);
								pCurve->SetStyle(g_bemdataStore.at(k)->m_Style);
								pCurve->SetColor(g_bemdataStore.at(k)->m_Color);
								pCurve->SetWidth(g_bemdataStore.at(k)->m_Width);
								FillRotorCurve(pCurve, g_bemdataStore.at(k)->m_BData.at(l), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
								pCurve->SetTitle(g_bemdataStore.at(k)->m_BData.at(l)->m_BEMName);

                             }
                            }
                         }
                        }
                }
            }
            else
            {
            for (int j=0; j< m_pBEMData->m_BData.size();j++)
            {

            CCurve *pCurve = m_RotorGraph2.AddCurve();
            pCurve->ShowPoints(m_pBEMData->m_BData.at(j)->m_bShowPoints);
            pCurve->SetStyle(m_pBEMData->m_BData.at(j)->m_Style);
            pCurve->SetColor(m_pBEMData->m_BData.at(j)->m_Color);
            if (m_pBEMData->m_BData.at(j)==m_pBData)
            {
            pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width+4);
            }
            else
            {
            pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width);
            }
            FillRotorCurve(pCurve, m_pBEMData->m_BData.at(j), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
            pCurve->SetTitle(m_pBEMData->m_BData.at(j)->m_BEMName);
            }
            }
        }
    }


	for (int i=0;i<g_bemdataStore.size();i++)
    {
        if (m_RotorGraph3.m_Type == 0)
        {

			if (g_bemdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_RotorGraph3.AddCurve();

			pCurve->ShowPoints(g_bemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_bemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_bemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_bemdataStore.at(i)->m_Width);
			FillRotorCurve(pCurve, g_bemdataStore.at(i), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
			pCurve->SetTitle(g_bemdataStore.at(i)->m_BEMName);
            }
        }
        else if (m_pBEMData && m_pBData)
        {
            if(m_bIsolateBladeCurve)
            {
                if (m_pBEMData->m_bIsVisible)
                {
                CCurve *pCurve = m_RotorGraph3.AddCurve();

                pCurve->ShowPoints(m_pBEMData->m_bShowPoints);
                pCurve->SetStyle(m_pBEMData->m_Style);
                pCurve->SetColor(m_pBEMData->m_Color);
                pCurve->SetWidth(m_pBEMData->m_Width);
                FillRotorCurve(pCurve, m_pBData, m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
                pCurve->SetTitle(m_pBData->m_BEMName);
                }

                if (m_bCompareBladeCurve)
                {
					 for (int k=0; k<g_bemdataStore.size();k++)
                        {
						for (int l=0;l<g_bemdataStore.at(k)->m_BData.size();l++)
                         {
						 if (g_bemdataStore.at(k)->m_BData.at(l)->lambdaglobal == m_pBData->lambdaglobal)
                            {
							if (g_bemdataStore.at(k)->m_bIsVisible)
                             {
                                CCurve *pCurve = m_RotorGraph3.AddCurve();

								pCurve->ShowPoints(g_bemdataStore.at(k)->m_bShowPoints);
								pCurve->SetStyle(g_bemdataStore.at(k)->m_Style);
								pCurve->SetColor(g_bemdataStore.at(k)->m_Color);
								pCurve->SetWidth(g_bemdataStore.at(k)->m_Width);
								FillRotorCurve(pCurve, g_bemdataStore.at(k)->m_BData.at(l), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
								pCurve->SetTitle(g_bemdataStore.at(k)->m_BData.at(l)->m_BEMName);

                             }
                            }
                         }
                        }
                }
            }
            else
            {
            for (int j=0; j< m_pBEMData->m_BData.size();j++)
            {

            CCurve *pCurve = m_RotorGraph3.AddCurve();

            pCurve->ShowPoints(m_pBEMData->m_BData.at(j)->m_bShowPoints);
            pCurve->SetStyle(m_pBEMData->m_BData.at(j)->m_Style);
            pCurve->SetColor(m_pBEMData->m_BData.at(j)->m_Color);
            if (m_pBEMData->m_BData.at(j)==m_pBData)
            {
            pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width+4);
            }
            else
            {
            pCurve->SetWidth(m_pBEMData->m_BData.at(j)->m_Width);
            }
            FillRotorCurve(pCurve, m_pBEMData->m_BData.at(j), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
            pCurve->SetTitle(m_pBEMData->m_BData.at(j)->m_BEMName);
            }
            }
        }
    }

    if (m_pBEMData && selected_lambda >= 0 && m_bShowOpPoint)
    {

    if (m_pBEMData->m_bIsVisible && m_RotorGraph1.m_Type == 0)
    {
    CCurve* pPolarCurve = m_RotorGraph1.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(m_pBEMData->m_Width+3);
    pPolarCurve->SetColor(m_pBEMData->m_Color);
    QList <double> *X = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph1.GetXVariable());
    QList <double> *Y = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph1.GetYVariable());
    pPolarCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
    }

    if (m_pBEMData->m_bIsVisible && m_RotorGraph2.m_Type == 0)
    {
    CCurve* pPolarCurve = m_RotorGraph2.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(m_pBEMData->m_Width+3);
    pPolarCurve->SetColor(m_pBEMData->m_Color);
    QList <double> *X = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph2.GetXVariable());
    QList <double> *Y = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph2.GetYVariable());
    pPolarCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
    }

    if (m_pBEMData->m_bIsVisible && m_RotorGraph3.m_Type == 0)
    {
    CCurve* pPolarCurve = m_RotorGraph3.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(m_pBEMData->m_Width+3);
    pPolarCurve->SetColor(m_pBEMData->m_Color);
    QList <double> *X = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph3.GetXVariable());
    QList <double> *Y = (QList <double> *) GetRotorVariable(m_pBEMData, m_RotorGraph3.GetYVariable());
    pPolarCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
    }

    }


    UpdateView();
}

void QBEM::CreateCharacteristicsCurves()
{
    CCurve *pCurve;

    m_CharGraph1.DeleteCurves();
    m_CharGraph2.DeleteCurves();
    m_CharGraph3.DeleteCurves();
    m_CharGraph4.DeleteCurves();

//    m_CharGraph1.SetAuto(true);
//    m_CharGraph2.SetAuto(true);
//    m_CharGraph3.SetAuto(true);
//    m_CharGraph4.SetAuto(true);


    if(m_pCBEMData && m_pCBEMData->simulated)
    {
    float *** pX;
    float *** pY;

    int all=0;

    if (m_CharGraph1.m_Param == 0) all = m_pCBEMData->windtimes;
    if (m_CharGraph1.m_Param == 1) all = m_pCBEMData->rottimes;
    if (m_CharGraph1.m_Param == 2) all = m_pCBEMData->pitchtimes;
    if (m_CharGraph1.m_Param == -1) all = 1;

    for (int i=0; i<all;i++)

    {
            pCurve = m_CharGraph1.AddCurve();

            pCurve->ShowPoints(m_pCBEMData->m_bShowPoints);
            pCurve->SetStyle(m_pCBEMData->m_Style);
            if (all == 1) pCurve->SetColor(m_pCBEMData->m_Color);
			else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
            pCurve->SetWidth(m_pCBEMData->m_Width);
            FillCharacteristicCurve(pCurve, m_pCBEMData, m_CharGraph1.GetXVariable(), m_CharGraph1.GetYVariable(),i);
            pCurve->SetTitle(m_pCBEMData->m_SimName);
    }

    if (m_bShowOpPoint)
    {
            pCurve = m_CharGraph1.AddCurve();
            pCurve->ShowPoints(true);
            pCurve->SetWidth(m_pCBEMData->m_Width+3);
            if (all == 1)
            {
                pCurve->SetColor(m_pCBEMData->m_Color);
            }
            else
            {
				if (m_CharGraph1.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
				if (m_CharGraph1.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
				if (m_CharGraph1.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
            }
            pX =   GetCharVariable(m_pCBEMData, m_CharGraph1.GetXVariable());
            pY =   GetCharVariable(m_pCBEMData, m_CharGraph1.GetYVariable());
            pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
    }




            if (m_CharGraph2.m_Param == 0) all = m_pCBEMData->windtimes;
            if (m_CharGraph2.m_Param == 1) all = m_pCBEMData->rottimes;
            if (m_CharGraph2.m_Param == 2) all = m_pCBEMData->pitchtimes;
            if (m_CharGraph2.m_Param == -1) all = 1;

            for (int i=0; i<all;i++)
            {
                    pCurve = m_CharGraph2.AddCurve();

                    pCurve->ShowPoints(m_pCBEMData->m_bShowPoints);
                    pCurve->SetStyle(m_pCBEMData->m_Style);
                    if (all == 1) pCurve->SetColor(m_pCBEMData->m_Color);
					else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
                    pCurve->SetWidth(m_pCBEMData->m_Width);
                    FillCharacteristicCurve(pCurve, m_pCBEMData, m_CharGraph2.GetXVariable(), m_CharGraph2.GetYVariable(),i);
                    pCurve->SetTitle(m_pCBEMData->m_SimName);


            }
            if (m_bShowOpPoint)
            {
                    pCurve = m_CharGraph2.AddCurve();
                    pCurve->ShowPoints(true);
                    pCurve->SetWidth(m_pCBEMData->m_Width+3);
                    if (all == 1)
                    {
                        pCurve->SetColor(m_pCBEMData->m_Color);
                    }
                    else
                    {
						if (m_CharGraph2.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
						if (m_CharGraph2.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
						if (m_CharGraph2.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
                    }
                    pX =   GetCharVariable(m_pCBEMData, m_CharGraph2.GetXVariable());
                    pY =   GetCharVariable(m_pCBEMData, m_CharGraph2.GetYVariable());
                    pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
            }



                    if (m_CharGraph3.m_Param == 0) all = m_pCBEMData->windtimes;
                    if (m_CharGraph3.m_Param == 1) all = m_pCBEMData->rottimes;
                    if (m_CharGraph3.m_Param == 2) all = m_pCBEMData->pitchtimes;
                    if (m_CharGraph3.m_Param == -1) all = 1;

                    for (int i=0; i<all;i++)
                    {
                            pCurve = m_CharGraph3.AddCurve();

                            pCurve->ShowPoints(m_pCBEMData->m_bShowPoints);
                            pCurve->SetStyle(m_pCBEMData->m_Style);
                            if (all == 1) pCurve->SetColor(m_pCBEMData->m_Color);
							else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
                            pCurve->SetWidth(m_pCBEMData->m_Width);
                            FillCharacteristicCurve(pCurve, m_pCBEMData, m_CharGraph3.GetXVariable(), m_CharGraph3.GetYVariable(),i);
                            pCurve->SetTitle(m_pCBEMData->m_SimName);


                    }

                    if (m_bShowOpPoint)
                    {
                            pCurve = m_CharGraph3.AddCurve();
                            pCurve->ShowPoints(true);
                            pCurve->SetWidth(m_pCBEMData->m_Width+3);
                            if (all == 1)
                            {
                                pCurve->SetColor(m_pCBEMData->m_Color);
                            }
                            else
                            {
								if (m_CharGraph3.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
								if (m_CharGraph3.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
								if (m_CharGraph3.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
                            }
                            pX=  GetCharVariable(m_pCBEMData, m_CharGraph3.GetXVariable());
                            pY=  GetCharVariable(m_pCBEMData, m_CharGraph3.GetYVariable());
                            pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
                    }




                    if (m_CharGraph4.m_Param == 0) all = m_pCBEMData->windtimes;
                    if (m_CharGraph4.m_Param == 1) all = m_pCBEMData->rottimes;
                    if (m_CharGraph4.m_Param == 2) all = m_pCBEMData->pitchtimes;
                    if (m_CharGraph4.m_Param == -1) all = 1;

                    for (int i=0; i<all;i++)
                    {
                            pCurve = m_CharGraph4.AddCurve();

                            pCurve->ShowPoints(m_pCBEMData->m_bShowPoints);
                            pCurve->SetStyle(m_pCBEMData->m_Style);
                            if (all == 1) pCurve->SetColor(m_pCBEMData->m_Color);
							else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
                            pCurve->SetWidth(m_pCBEMData->m_Width);
                            FillCharacteristicCurve(pCurve, m_pCBEMData, m_CharGraph4.GetXVariable(), m_CharGraph4.GetYVariable(),i);
                            pCurve->SetTitle(m_pCBEMData->m_SimName);


                    }

                    if (m_bShowOpPoint)
                    {
                            pCurve = m_CharGraph4.AddCurve();
                            pCurve->ShowPoints(true);
                            pCurve->SetWidth(m_pCBEMData->m_Width+3);
                            if (all == 1)
                            {
                                pCurve->SetColor(m_pCBEMData->m_Color);
                            }
                            else
                            {
								if (m_CharGraph4.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
								if (m_CharGraph4.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
								if (m_CharGraph4.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
                            }
                            pX=  GetCharVariable(m_pCBEMData, m_CharGraph4.GetXVariable());
                            pY=  GetCharVariable(m_pCBEMData, m_CharGraph4.GetYVariable());
                            pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
                    }

    }



    UpdateView();


}


void QBEM::CreatePowerCurves()
{

    m_PowerGraph1.DeleteCurves();
    m_PowerGraph2.DeleteCurves();
    m_PowerGraph3.DeleteCurves();

//    m_PowerGraph1.SetAuto(true);
//    m_PowerGraph2.SetAuto(true);
//    m_PowerGraph3.SetAuto(true);


	for (int i=0;i<g_tbemdataStore.size();i++)
    {
        if (m_PowerGraph1.m_Type == 0)
        {

			if (g_tbemdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_PowerGraph1.AddCurve();

			pCurve->ShowPoints(g_tbemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_tbemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_tbemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_tbemdataStore.at(i)->m_Width);
			FillPowerCurve(pCurve, g_tbemdataStore.at(i), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
			pCurve->SetTitle(g_tbemdataStore.at(i)->m_TurbineName);


            }
        }

		else if (m_pTBEMData && m_pTurbineBData)
		{
			// new code JW
			if (m_PowerGraph1.m_Type == 3)
			{



						for (int k=0; k<g_tbemdataStore.size();k++)
						{
							if (g_tbemdataStore.at(k)->m_bIsVisible)
							{
								CCurve *pCurve = m_PowerGraph1.AddCurve();

								pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
								pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
								pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
								pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);

								FillWeibullCurve(pCurve, g_tbemdataStore.at(k), m_PowerGraph1.GetXVariable());//,50);
								pCurve->SetTitle(g_tbemdataStore.at(k)->m_BEMName);

							}
						}




			}
			// end new code JW
			else
			{
				if(m_bIsolateBladeCurve)
				{
					if (m_pTBEMData->m_bIsVisible)
					{
					CCurve *pCurve = m_PowerGraph1.AddCurve();

					pCurve->ShowPoints(m_pTBEMData->m_bShowPoints);
					pCurve->SetStyle(m_pTBEMData->m_Style);
					pCurve->SetColor(m_pTBEMData->m_Color);
					pCurve->SetWidth(m_pTBEMData->m_Width);
					FillPowerCurve(pCurve, m_pTurbineBData, m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
					pCurve->SetTitle(m_pTurbineBData->m_BEMName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tbemdataStore.size();k++)
							{
							for (int l=0;l<g_tbemdataStore.at(k)->m_BData.size();l++)
							 {
							 if (g_tbemdataStore.at(k)->m_BData.at(l)->windspeedStr == m_pTurbineBData->windspeedStr)
								{
								if (g_tbemdataStore.at(k)->m_bIsVisible)
								 {
									CCurve *pCurve = m_PowerGraph1.AddCurve();

									pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
									pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
									pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
									pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);
									FillPowerCurve(pCurve, g_tbemdataStore.at(k)->m_BData.at(l), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
									pCurve->SetTitle(g_tbemdataStore.at(k)->m_BData.at(l)->m_BEMName);

								 }
								}
							 }
							}
					}
				}

				else
				{
					for (int j=0; j< m_pTBEMData->m_BData.size();j++)
					{
						CCurve *pCurve = m_PowerGraph1.AddCurve();

						pCurve->ShowPoints(m_pTBEMData->m_BData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTBEMData->m_BData.at(j)->m_Style);
						pCurve->SetColor(m_pTBEMData->m_BData.at(j)->m_Color);
						if (m_pTBEMData->m_BData.at(j)==m_pTurbineBData)
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTBEMData->m_BData.at(j), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
						pCurve->SetTitle(m_pTBEMData->m_BData.at(j)->m_BEMName);
					}
				}
			}
		}
	}



	for (int i=0;i<g_tbemdataStore.size();i++)
	{
		if (m_PowerGraph2.m_Type == 0)
		{

			if (g_tbemdataStore.at(i)->m_bIsVisible)
			{

			CCurve *pCurve = m_PowerGraph2.AddCurve();

			pCurve->ShowPoints(g_tbemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_tbemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_tbemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_tbemdataStore.at(i)->m_Width);
			FillPowerCurve(pCurve, g_tbemdataStore.at(i), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
			pCurve->SetTitle(g_tbemdataStore.at(i)->m_TurbineName);


			}
		}

		else if (m_pTBEMData && m_pTurbineBData)
		{
			// new code JW
			if (m_PowerGraph2.m_Type == 3)
			{
				for (int k=0; k<g_tbemdataStore.size();k++)
				{
					if (g_tbemdataStore.at(k)->m_bIsVisible)
					{
						CCurve *pCurve = m_PowerGraph2.AddCurve();

						pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
						pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
						pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
						pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);

						FillWeibullCurve(pCurve, g_tbemdataStore.at(k), m_PowerGraph2.GetXVariable());//,50);
						pCurve->SetTitle(g_tbemdataStore.at(k)->m_BEMName);

					}
				}

			}
			// end new code JW
			else
			{
				if(m_bIsolateBladeCurve)
				{
					if (m_pTBEMData->m_bIsVisible)
					{
					CCurve *pCurve = m_PowerGraph2.AddCurve();

					pCurve->ShowPoints(m_pTBEMData->m_bShowPoints);
					pCurve->SetStyle(m_pTBEMData->m_Style);
					pCurve->SetColor(m_pTBEMData->m_Color);
					pCurve->SetWidth(m_pTBEMData->m_Width);
					FillPowerCurve(pCurve, m_pTurbineBData, m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
					pCurve->SetTitle(m_pTurbineBData->m_BEMName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tbemdataStore.size();k++)
							{
							for (int l=0;l<g_tbemdataStore.at(k)->m_BData.size();l++)
							 {
							 if (g_tbemdataStore.at(k)->m_BData.at(l)->windspeedStr == m_pTurbineBData->windspeedStr)
								{
								if (g_tbemdataStore.at(k)->m_bIsVisible)
								 {
									CCurve *pCurve = m_PowerGraph2.AddCurve();

									pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
									pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
									pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
									pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);
									FillPowerCurve(pCurve, g_tbemdataStore.at(k)->m_BData.at(l), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
									pCurve->SetTitle(g_tbemdataStore.at(k)->m_BData.at(l)->m_BEMName);

								 }
								}
							 }
							}
					}
				}

				else
				{
					for (int j=0; j< m_pTBEMData->m_BData.size();j++)
					{
						CCurve *pCurve = m_PowerGraph2.AddCurve();

						pCurve->ShowPoints(m_pTBEMData->m_BData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTBEMData->m_BData.at(j)->m_Style);
						pCurve->SetColor(m_pTBEMData->m_BData.at(j)->m_Color);
						if (m_pTBEMData->m_BData.at(j)==m_pTurbineBData)
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTBEMData->m_BData.at(j), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
						pCurve->SetTitle(m_pTBEMData->m_BData.at(j)->m_BEMName);
					}
				}
			}
		}
	}




	for (int i=0;i<g_tbemdataStore.size();i++)
	{
		if (m_PowerGraph3.m_Type == 0)
		{

			if (g_tbemdataStore.at(i)->m_bIsVisible)
			{

			CCurve *pCurve = m_PowerGraph3.AddCurve();

			pCurve->ShowPoints(g_tbemdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_tbemdataStore.at(i)->m_Style);
			pCurve->SetColor(g_tbemdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_tbemdataStore.at(i)->m_Width);
			FillPowerCurve(pCurve, g_tbemdataStore.at(i), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
			pCurve->SetTitle(g_tbemdataStore.at(i)->m_TurbineName);


			}
		}

		else if (m_pTBEMData && m_pTurbineBData)
		{
			// new code JW
			if (m_PowerGraph3.m_Type == 3)
			{
				for (int k=0; k<g_tbemdataStore.size();k++)
				{
					if (g_tbemdataStore.at(k)->m_bIsVisible)
					{
						CCurve *pCurve = m_PowerGraph3.AddCurve();

						pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
						pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
						pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
						pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);

						FillWeibullCurve(pCurve, g_tbemdataStore.at(k), m_PowerGraph3.GetXVariable());//,50);
						pCurve->SetTitle(g_tbemdataStore.at(k)->m_BEMName);

					}
				}
			}
			// end new code JW
			else
			{
				if(m_bIsolateBladeCurve)
				{
					if (m_pTBEMData->m_bIsVisible)
					{
					CCurve *pCurve = m_PowerGraph3.AddCurve();

					pCurve->ShowPoints(m_pTBEMData->m_bShowPoints);
					pCurve->SetStyle(m_pTBEMData->m_Style);
					pCurve->SetColor(m_pTBEMData->m_Color);
					pCurve->SetWidth(m_pTBEMData->m_Width);
					FillPowerCurve(pCurve, m_pTurbineBData, m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
					pCurve->SetTitle(m_pTurbineBData->m_BEMName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tbemdataStore.size();k++)
							{
							for (int l=0;l<g_tbemdataStore.at(k)->m_BData.size();l++)
							 {
							 if (g_tbemdataStore.at(k)->m_BData.at(l)->windspeedStr == m_pTurbineBData->windspeedStr)
								{
								if (g_tbemdataStore.at(k)->m_bIsVisible)
								 {
									CCurve *pCurve = m_PowerGraph3.AddCurve();

									pCurve->ShowPoints(g_tbemdataStore.at(k)->m_bShowPoints);
									pCurve->SetStyle(g_tbemdataStore.at(k)->m_Style);
									pCurve->SetColor(g_tbemdataStore.at(k)->m_Color);
									pCurve->SetWidth(g_tbemdataStore.at(k)->m_Width);
									FillPowerCurve(pCurve, g_tbemdataStore.at(k)->m_BData.at(l), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
									pCurve->SetTitle(g_tbemdataStore.at(k)->m_BData.at(l)->m_BEMName);

								 }
								}
							 }
							}
					}
				}

				else
				{
					for (int j=0; j< m_pTBEMData->m_BData.size();j++)
					{
						CCurve *pCurve = m_PowerGraph3.AddCurve();

						pCurve->ShowPoints(m_pTBEMData->m_BData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTBEMData->m_BData.at(j)->m_Style);
						pCurve->SetColor(m_pTBEMData->m_BData.at(j)->m_Color);
						if (m_pTBEMData->m_BData.at(j)==m_pTurbineBData)
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTBEMData->m_BData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTBEMData->m_BData.at(j), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
						pCurve->SetTitle(m_pTBEMData->m_BData.at(j)->m_BEMName);
					}
				}
			}
		}
	}



    if (m_pTBEMData && selected_windspeed >= 0 && m_bShowOpPoint)
    {

        if (m_pTBEMData->m_bIsVisible && m_PowerGraph1.m_Type == 0)
        {
        CCurve* pPolarCurve = m_PowerGraph1.AddCurve();
        pPolarCurve->ShowPoints(true);
        pPolarCurve->SetWidth(m_pTBEMData->m_Width+3);
        pPolarCurve->SetColor(m_pTBEMData->m_Color);
        QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph1.GetXVariable());
        QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph1.GetYVariable());
        pPolarCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
        }

        if (m_pTBEMData->m_bIsVisible && m_PowerGraph2.m_Type == 0)
        {
        CCurve* pPolarCurve = m_PowerGraph2.AddCurve();
        pPolarCurve->ShowPoints(true);
        pPolarCurve->SetWidth(m_pTBEMData->m_Width+3);
        pPolarCurve->SetColor(m_pTBEMData->m_Color);
        QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph2.GetXVariable());
        QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph2.GetYVariable());
        pPolarCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
        }

        if (m_pTBEMData->m_bIsVisible && m_PowerGraph3.m_Type == 0)
        {
        CCurve* pPolarCurve = m_PowerGraph3.AddCurve();
        pPolarCurve->ShowPoints(true);
        pPolarCurve->SetWidth(m_pTBEMData->m_Width+3);
        pPolarCurve->SetColor(m_pTBEMData->m_Color);
        QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph3.GetXVariable());
        QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTBEMData, m_PowerGraph3.GetYVariable());
        pPolarCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
		}

    }



    UpdateView();
}

void QBEM::DisableAllButtons()
{

	m_BEMToolBar->DisableAll();

    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

	g_mainFrame->BEMViewMenu->setEnabled(false);
	g_mainFrame->BEM360PolarMenu->setEnabled(false);
	g_mainFrame->BEMBladeMenu->setEnabled(false);
	g_mainFrame->BEMCtxMenu->setEnabled(false);
	g_mainFrame->BEMTurbine->setEnabled(false);
	g_mainFrame->optionsMenu->setEnabled(false);
	g_mainFrame->fileMenu->setEnabled(false);

	g_mainFrame->m_pctrlMainToolBar->setEnabled(false);

    pSimuWidget->m_pctrlDeleteTBEM->setEnabled(false);
    pSimuWidget->m_pctrlDefineTurbineSim->setEnabled(false);
    pSimuWidget->m_pctrlStartTurbineSim->setEnabled(false);
    pSimuWidget->m_pctrlWind1->setEnabled(false);
    pSimuWidget->m_pctrlWind2->setEnabled(false);
    pSimuWidget->m_pctrlWindDelta->setEnabled(false);

    pSimuWidget->m_pctrlCreateBEM->setEnabled(false);
    pSimuWidget->m_pctrlDeleteBEM->setEnabled(false);
    pSimuWidget->m_pctrlStartBEM->setEnabled(false);
    pSimuWidget->m_pctrlLSLineEdit->setEnabled(false);
    pSimuWidget->m_pctrlLELineEdit->setEnabled(false);
    pSimuWidget->m_pctrlLDLineEdit->setEnabled(false);
    pSimuWidget->m_pctrlWindspeed->setEnabled(false);

}

void QBEM::EnableAllButtons()
{
	m_BEMToolBar->EnableAll();


	g_mainFrame->BEMViewMenu->setEnabled(true);
	g_mainFrame->BEM360PolarMenu->setEnabled(true);
	g_mainFrame->BEMBladeMenu->setEnabled(true);
	g_mainFrame->BEMCtxMenu->setEnabled(true);
	g_mainFrame->BEMTurbine->setEnabled(true);

	g_mainFrame->m_pctrlMainToolBar->setEnabled(true);

	g_mainFrame->fileMenu->setEnabled(true);
	g_mainFrame->optionsMenu->setEnabled(true);
}

void QBEM::FillComboBoxes(bool bEnable)
{

        SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

        if(!bEnable)
        {
                pSimuWidget->m_pctrlCurveColor->setEnabled(false);
                pSimuWidget->m_pctrlCurveStyle->setEnabled(false);
                pSimuWidget->m_pctrlCurveWidth->setEnabled(false);
                pSimuWidget->m_pctrlShowCurve->setEnabled(false);
                pSimuWidget->m_pctrlShowPoints->setEnabled(false);
                pSimuWidget->m_pctrlShowOpPoint->setEnabled(false);
        }
        else
        {
                pSimuWidget->m_pctrlCurveColor->setEnabled(true);
                pSimuWidget->m_pctrlCurveStyle->setEnabled(true);
                pSimuWidget->m_pctrlCurveWidth->setEnabled(true);
                pSimuWidget->m_pctrlShowCurve->setEnabled(true);
                pSimuWidget->m_pctrlShowPoints->setEnabled(true);
                pSimuWidget->m_pctrlShowOpPoint->setEnabled(true);
        }
        int LineWidth[5];
        for (int i=0; i<5;i++) LineWidth[i] = m_CurveWidth;
        pSimuWidget->m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles
        pSimuWidget->m_pStyleDelegate->SetLineColor(m_CurveColor);

        int LineStyle[5];
        for (int i=0; i<5;i++) LineStyle[i] = m_CurveStyle;
        pSimuWidget->m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
        pSimuWidget->m_pWidthDelegate->SetLineColor(m_CurveColor);

        pSimuWidget->m_pctrlCurveStyle->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);
        pSimuWidget->m_pctrlCurveWidth->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);

        pSimuWidget->m_pctrlCurveColor->SetColor(m_CurveColor);
        pSimuWidget->m_pctrlCurveColor->SetStyle(m_CurveStyle);
        pSimuWidget->m_pctrlCurveColor->SetWidth(m_CurveWidth);

        pSimuWidget->m_pctrlCurveStyle->update();
        pSimuWidget->m_pctrlCurveWidth->update();
        pSimuWidget->m_pctrlCurveColor->update();

        pSimuWidget->m_pctrlCurveStyle->setCurrentIndex(m_CurveStyle);
        pSimuWidget->m_pctrlCurveWidth->setCurrentIndex(m_CurveWidth-1);

		if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
        {

            if(!bEnable)
            {
                    m_pctrlCurveColor->setEnabled(false);
                    m_pctrlCurveStyle->setEnabled(false);
                    m_pctrlCurveWidth->setEnabled(false);
                    m_pctrlShowCurve->setEnabled(false);
                    m_pctrlShowPoints->setEnabled(false);
            }
            else
            {
                    m_pctrlCurveColor->setEnabled(true);
                    m_pctrlCurveStyle->setEnabled(true);
                    m_pctrlCurveWidth->setEnabled(true);
                    m_pctrlShowCurve->setEnabled(true);
                    m_pctrlShowPoints->setEnabled(true);
            }
            int LineWidth[5];
            for (int i=0; i<5;i++) LineWidth[i] = m_CurveWidth;
            m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles
            m_pStyleDelegate->SetLineColor(m_CurveColor);

            int LineStyle[5];
            for (int i=0; i<5;i++) LineStyle[i] = m_CurveStyle;
            m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
            m_pWidthDelegate->SetLineColor(m_CurveColor);

            m_pctrlCurveStyle->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);
            m_pctrlCurveWidth->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);

            m_pctrlCurveColor->SetColor(m_CurveColor);
            m_pctrlCurveColor->SetStyle(m_CurveStyle);
            m_pctrlCurveColor->SetWidth(m_CurveWidth);

            m_pctrlCurveStyle->update();
            m_pctrlCurveWidth->update();
            m_pctrlCurveColor->update();

            m_pctrlCurveStyle->setCurrentIndex(m_CurveStyle);
            m_pctrlCurveWidth->setCurrentIndex(m_CurveWidth-1);
        }

}

void QBEM::FillDataTable()
{
        if(!m_pBlade) return;
        int i;
        if (!m_bAdvancedEdit) m_pWingModel->setRowCount(m_pBlade->m_NPanel+1);
        if (m_bAdvancedEdit)  m_pBladeAxisModel->setRowCount(m_pBlade->m_NPanel+1);

        for(i=0; i<=m_pBlade->m_NPanel; i++)
        {
            if (m_bAdvancedEdit)
            {
                FillAdvancedTableRow(i);
            }
            else
            {
                FillTableRow(i);
            }
        }


}

void QBEM::FillPolarCurve(CCurve *pCurve, CPolar *pPolar, int XVar, int YVar)
{
	int i;
	QList <double> *pX;
	QList <double> *pY;
	QList <double> ClCd;
	
	pX = (QList <double> *) GetVariable(pPolar, XVar);
	pY = (QList <double> *) GetVariable(pPolar, YVar);
	
	for (int i=0;i<pPolar->m_Cl.size();i++)
	{
		ClCd.append(pPolar->m_Cl.at(i)/pPolar->m_Cd.at(i));
	}
	
	pCurve->SetSelected(-1);
	for (i=0; i<pPolar->m_Alpha.size(); i++)
	{
		if (XVar == 3 && YVar == 3) pCurve->AddPoint(ClCd.at(i),ClCd.at(i));
		else if (XVar == 3) pCurve->AddPoint(ClCd.at(i), pY->at(i));
		else if (YVar == 3) pCurve->AddPoint(pX->at(i), ClCd.at(i));
		else pCurve->AddPoint(pX->at(i),pY->at(i));
	}
}

void QBEM::FillPolarCurve(CCurve *pCurve, C360Polar *pPolar, int XVar, int YVar)
{
		int i;
		QList <double> *pX;
		QList <double> *pY;
		QList <double> ClCd;

		pX = (QList <double> *) GetVariable(pPolar, XVar);
		pY = (QList <double> *) GetVariable(pPolar, YVar);

		for (int i=0;i<pPolar->m_Cl.size();i++)
		{
			ClCd.append(pPolar->m_Cl.at(i)/pPolar->m_Cd.at(i));
		}



		pCurve->SetSelected(-1);

		for (i=0; i<pPolar->m_Alpha.size(); i++)
		{
		if (XVar == 3 && YVar == 3) pCurve->AddPoint(ClCd.at(i),ClCd.at(i));
		else if (XVar == 3) pCurve->AddPoint(ClCd.at(i), pY->at(i));
		else if (YVar == 3) pCurve->AddPoint(pX->at(i), ClCd.at(i));
		else pCurve->AddPoint(pX->at(i),pY->at(i));
		}
}

void QBEM::FillTableRow(int row)
{
        //QString str, strong;
        QModelIndex ind;

        if (m_bAbsoluteBlade)
        {
            ind = m_pWingModel->index(row, 0, QModelIndex());
            m_pWingModel->setData(ind, m_pBlade->m_TPos[row] * g_mainFrame->m_mtoUnit);
        }
        else
        {
            ind = m_pWingModel->index(row, 0, QModelIndex());
            if (m_pBlade->m_TRelPos[row] <= 0.0000000001) m_pBlade->m_TRelPos[row] = 0;
            m_pWingModel->setData(ind, m_pBlade->m_TRelPos[row] * g_mainFrame->m_mtoUnit);
        }

        ind = m_pWingModel->index(row, 1, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_TChord[row] * g_mainFrame->m_mtoUnit);

        ind = m_pWingModel->index(row, 2, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_TTwist[row]);

        ind = m_pWingModel->index(row, 3, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_Airfoils[row]);

        ind = m_pWingModel->index(row, 4, QModelIndex());

        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setData(ind, m_pBlade->m_Polar.at(row));
        else m_pWingModel->setData(ind, m_pBlade->m_Range.at(row));

}

void QBEM::FillAdvancedTableRow(int row)
{
        QModelIndex ind;

        if (m_bAbsoluteBlade)
        {
            ind = m_pBladeAxisModel->index(row, 0, QModelIndex());
            m_pBladeAxisModel->setData(ind, m_pBlade->m_TPos[row] * g_mainFrame->m_mtoUnit);
        }
        else
        {
            ind = m_pBladeAxisModel->index(row, 0, QModelIndex());
            m_pBladeAxisModel->setData(ind, m_pBlade->m_TRelPos[row] * g_mainFrame->m_mtoUnit);
        }

        ind = m_pBladeAxisModel->index(row, 1, QModelIndex());
        m_pBladeAxisModel->setData(ind, m_pBlade->m_TOffsetX[row] * g_mainFrame->m_mtoUnit);

        ind = m_pBladeAxisModel->index(row, 2, QModelIndex());
        m_pBladeAxisModel->setData(ind, m_pBlade->m_TOffsetZ[row]);

        ind = m_pBladeAxisModel->index(row, 3, QModelIndex());
        m_pBladeAxisModel->setData(ind, m_pBlade->m_TFoilPAxisX[row]);

}

void QBEM::FillRotorCurve(CCurve *pCurve, void *Data, int XVar, int YVar)
{
        QList <double> *pX;
        QList <double> *pY;
        Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

        if (pGraph->m_Type == 0)
        {
        pX = (QList <double> *) GetRotorVariable(Data, XVar);
        pY = (QList <double> *) GetRotorVariable(Data, YVar);
        }
        else
        {
        pX = (QList <double> *) GetBladeVariable(Data, XVar);
        pY = (QList <double> *) GetBladeVariable(Data, YVar);
        }

        pCurve->SetSelected(-1);

        for (int i=0;i<pX->size();i++)
        {
            pCurve->AddPoint(pX->at(i),pY->at(i));
        }



}

void QBEM::FillPowerCurve(CCurve *pCurve, void *Data, int XVar, int YVar)
{
        QList <double> *pX;
		QList <double> *pY;
        Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

		if(pGraph->m_Type == 0)
        {
        pX = (QList <double> *) GetTurbineRotorVariable(Data, XVar);
        pY = (QList <double> *) GetTurbineRotorVariable(Data, YVar);
		}
		else
		{
		pX = (QList <double> *) GetTurbineBladeVariable(Data, XVar);
		pY = (QList <double> *) GetTurbineBladeVariable(Data, YVar);
		}

        pCurve->SetSelected(-1);

        for (int i=0;i<pX->size();i++)
        {
            pCurve->AddPoint(pX->at(i),pY->at(i));
        }

}

// new code JW
void QBEM::FillWeibullCurve(CCurve *pCurve, void *Data, int XVar)//, int count)
{

	QList <double> *pX;
	QList <double> *pY;

	pX =  (QList <double> *) GetWeibullXVariable(XVar);
	pY =  (QList <double> *) GetWeibullYVariable(Data, XVar);

	pCurve->SetSelected(-1);

	for (int i=0;i<pX->size();i++)
	{
		pCurve->AddPoint(pX->at(i),pY->at(i)/1000.0);
	}


	/*
	QList<QList<double> > *pYList;
	pYList = (QList<QList<double> > *) GetWeibullYVariable(Data);

	if (XVar==0)
	{
		for (int i=0;i<101;i++)
		{
			pY =  (QList <double> *) &pYList->at(i);
			pCurve->AddPoint(pX->at(i),pY->at(count));
		}
	}
	else if (XVar==1)
	{
		pY =  (QList <double> *) &pYList->at(count);
		for (int i=0;i<101;i++)
		{
			pCurve->AddPoint(pX->at(i),pY->at(i));
		}
	}
	else
	{
		for (int i=0;i<101;i++)
		{
			pY =  (QList <double> *) &pYList->at(i);
			pCurve->AddPoint(pX->at(i),pY->at(count));
		}
	}
	*/

}
// end new code JW

void QBEM::FillCharacteristicCurve(CCurve *pCurve, void *Data, int XVar, int YVar, int num_param)
{

    float *** pX;
    float *** pY;
    Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

    pX =   GetCharVariable(Data, XVar);
    pY =   GetCharVariable(Data, YVar);


    pCurve->SetSelected(-1);


    if( pGraph->m_MainVar==0)
    {
        for (int i=0;i<m_pCBEMData->windtimes;i++)
        {
            if (pGraph->m_Param == 1) pCurve->AddPoint(pX[i][num_param][selected_pitch],pY[i][num_param][selected_pitch]);
            if (pGraph->m_Param == 2) pCurve->AddPoint(pX[i][selected_rot][num_param],pY[i][selected_rot][num_param]);
            if (pGraph->m_Param == -1) pCurve->AddPoint(pX[i][selected_rot][selected_pitch],pY[i][selected_rot][selected_pitch]);
        }
        if (pGraph->m_Param == 1 && num_param == selected_rot)
        {
            pCurve->CurveWidth+=2;
        }
        if (pGraph->m_Param == 2 && num_param == selected_pitch)
        {
            pCurve->CurveWidth+=2;
        }
    }

    if( pGraph->m_MainVar==1)
    {
        for (int i=0;i<m_pCBEMData->rottimes;i++)
        {
            if (pGraph->m_Param == 0) pCurve->AddPoint(pX[num_param][i][selected_pitch],pY[num_param][i][selected_pitch]);
            if (pGraph->m_Param == 2) pCurve->AddPoint(pX[selected_wind][i][num_param],pY[selected_wind][i][num_param]);
            if (pGraph->m_Param == -1) pCurve->AddPoint(pX[selected_wind][i][selected_pitch],pY[selected_wind][i][selected_pitch]);
        }
        if (pGraph->m_Param == 0 && num_param == selected_wind)
        {
            pCurve->CurveWidth+=2;
        }
        if (pGraph->m_Param == 2 && num_param == selected_pitch)
        {
            pCurve->CurveWidth+=2;
        }
    }

    if( pGraph->m_MainVar==2)
    {
        for (int i=0;i<m_pCBEMData->pitchtimes;i++)
        {
            if (pGraph->m_Param == 0) pCurve->AddPoint(pX[num_param][selected_rot][i],pY[num_param][selected_rot][i]);
            if (pGraph->m_Param == 1) pCurve->AddPoint(pX[selected_wind][num_param][i],pY[selected_wind][num_param][i]);
            if (pGraph->m_Param == -1) pCurve->AddPoint(pX[selected_wind][selected_rot][i],pY[selected_wind][selected_rot][i]);
        }
        if (pGraph->m_Param == 0 && num_param == selected_wind)
        {
            pCurve->CurveWidth+=2;
        }
        if (pGraph->m_Param == 1 && num_param == selected_rot)
        {
            pCurve->CurveWidth+=2;
        }
    }


}


// TODO this function is ambiguous! Multiple 360Polars can have same name and same grand parent foil but another parent polar!
C360Polar* QBEM::Get360Polar(QString m_FoilName, QString PolarName) {
	if (!PolarName.length()) {
		//try to get the first from the Combobox
        if(!g_qbem->m_BEMToolBar->m_polar360ComboBox->count()) return NULL;        //nothing more to try
        PolarName = g_qbem->m_BEMToolBar->m_polar360ComboBox->itemText(0); //... and carry on
	}
	
	C360Polar *pPolar;
	for (int i=0; i < g_360PolarStore.size() ; i++) {
		pPolar = g_360PolarStore.at(i);
		if (pPolar->m_airfoil->getName() == m_FoilName && pPolar->getName() == PolarName) {
			return pPolar;
		}
	}
	return NULL;
}

CFoil* QBEM::GetFoil(QString strFoilName)
{
        //returns a pointer to the foil with the corresponding name
        // returns NULL if not found
        if(!strFoilName.length()) return NULL;
        CFoil* pFoil;
		for (int i=0; i < g_foilStore.size(); i++)
        {
				pFoil = g_foilStore.at(i);
                if (pFoil->getName() == strFoilName)
                {
					return pFoil;
                }
        }

        return NULL;
}

CBlade* QBEM::GetCurrentBlade()
{
    if (m_pBlade) return m_pBlade;
	else return NULL;
}

QGraph* QBEM::GetGraph(QPoint &pt)
{
//	//returns a pointer to the graph in which the point pt lies
		if(g_mainFrame->m_iView==POLARVIEW)// new JW, old: m_iView
        {

                        if(m_360Graph1.IsInDrawRect(pt)){return &m_360Graph1;}
                        else if(m_360Graph2.IsInDrawRect(pt)){return &m_360Graph2;}
                        else if(m_360Graph3.IsInDrawRect(pt)){return &m_360Graph3;}
                        else if(m_360Graph4.IsInDrawRect(pt)){return &m_360Graph4;}

                        else return NULL;

        }
		if(g_mainFrame->m_iView==BEMSIMVIEW)// new JW, old: m_iView
        {         
                        if(m_RotorGraph1.IsInDrawRect(pt)){return &m_RotorGraph1;}
                        if(m_RotorGraph2.IsInDrawRect(pt)){return &m_RotorGraph2;}
                        if(m_RotorGraph3.IsInDrawRect(pt)){return &m_RotorGraph3;}
                        else return NULL;
        }
		if(g_mainFrame->m_iView==TURBINEVIEW)// new JW, old: m_iView
        {
                        if(m_PowerGraph1.IsInDrawRect(pt)){return &m_PowerGraph1;}
                        if(m_PowerGraph2.IsInDrawRect(pt)){return &m_PowerGraph2;}
                        if(m_PowerGraph3.IsInDrawRect(pt)){return &m_PowerGraph3;}
                        else return NULL;
        }
		if(g_mainFrame->m_iView==CHARSIMVIEW)// new JW, old: m_iView
        {
                        if(m_CharGraph1.IsInDrawRect(pt)){return &m_CharGraph1;}
                        if(m_CharGraph2.IsInDrawRect(pt)){return &m_CharGraph2;}
                        if(m_CharGraph3.IsInDrawRect(pt)){return &m_CharGraph3;}
                        if(m_CharGraph4.IsInDrawRect(pt)){return &m_CharGraph4;}
                        else return NULL;
        }




return NULL;
}

CPolar* QBEM::GetPolar(QString m_FoilName, QString PolarName)  // NM TODO replace by store functions
{



        if (!PolarName.length())
        {
                //try to get the first from the Combobox
				if(!m_BEMToolBar->m_polarComboBox->count()) return NULL;        //nothing more to try
				PolarName = m_BEMToolBar->m_polarComboBox->itemText(0); //... and carry on
        }

        CPolar *pPolar;
		for (int i=0; i < g_polarStore.size() ; i++)
        {
				pPolar = (CPolar *) g_polarStore.at(i);
				if (pPolar->getParent()->getName() == m_FoilName &&  pPolar->getName() == PolarName)
                {
                        return pPolar;
                }
        }
        return NULL;
}

void * QBEM::GetRotorVariable(void *Data, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        void * pVar;
        BEMData *pBEMData = (BEMData *) Data;

        switch (iVar){

                case 0:

                        pVar = &pBEMData->m_Cp;
                        break;
                case 1:

                        pVar = &pBEMData->m_Ct;
                        break;
                case 2:

                        pVar = &pBEMData->m_Cm;
                        break;
                case 3:

                        pVar = &pBEMData->m_Kp;
                        break;
                case 4:

                        pVar = &pBEMData->m_Lambda;
                        break;
                case 5:

                        pVar = &pBEMData->m_one_over_Lambda;
                        break;
                case 6:

                        pVar = &pBEMData->m_P;
                        break;
                case 7:

                        pVar = &pBEMData->m_S;
                        break;
                case 8:

                        pVar = &pBEMData->m_Torque;
                        break;
                case 9:

                        pVar = &pBEMData->m_Omega;
                        break;
                case 10:

                        pVar = &pBEMData->m_V;
                        break;
                case 11:

                        pVar = &pBEMData->m_Bending;
                        break;
                default:

                        pVar = &pBEMData->m_Lambda;
                        break;
        }
        return pVar;
}

void * QBEM::GetBladeVariable(void *Data, int iVar)
{
    // returns a pointer to the variable array defined by its index iVar
    void * pVar;
    BData *pBData = (BData *) Data;

    switch (iVar){

            case 0:

                    pVar = &pBData->m_a_axial;
                    break;
            case 1:

                    pVar = &pBData->m_pos;
                    break;
            case 2:

                    pVar = &pBData->m_a_tangential;
                    break;
            case 3:

                    pVar = &pBData->m_lambda_local;
                    break;
            case 4:

                    pVar = &pBData->m_Cn;
                    break;
            case 5:

                    pVar = &pBData->m_Ct;
                    break;
            case 6:

                    pVar = &pBData->m_phi;
                    break;
            case 7:

                    pVar = &pBData->m_alpha;
                    break;
            case 8:

                    pVar = &pBData->m_theta;
                    break;
            case 9:

                    pVar = &pBData->m_c_local;
                    break;
            case 10:

                    pVar = &pBData->m_CL;
                    break;
            case 11:

                    pVar = &pBData->m_CD;
                    break;
            case 12:

                    pVar = &pBData->m_LD;
                    break;
            case 13:

                    pVar = &pBData->m_F;
                    break;
            case 14:

                    pVar = &pBData->m_Reynolds;
                    break;
            case 15:

                    pVar = &pBData->m_DeltaReynolds;
                    break;
            case 16:

                    pVar = &pBData->m_p_tangential;
                    break;
            case 17:

                    pVar = &pBData->m_p_normal;
                    break;
            case 18:

                    pVar = &pBData->m_circ;
                    break;
            case 19:

                    pVar = &pBData->m_Windspeed;
                    break;
            case 20:

                    pVar = &pBData->m_Iterations;
                    break;
            case 21:

                    pVar = &pBData->m_Fa_axial;
                    break;
            case 22:

                    pVar = &pBData->m_Fa_radial;
                    break;
            default:

                    pVar = &pBData->m_pos;
                    break;
    }
    return pVar;
}

BData* QBEM::GetBladeData(QString Lambda)
{

    if (!m_pBEMData->m_BData.size()) return NULL;

    for (int i=0; i<m_pBEMData->m_BData.size(); i++)
    {
        if (m_pBEMData->m_BData.at(i)->lambdaglobal==Lambda)
        {
            return m_pBEMData->m_BData.at(i);
        }
    }

    return NULL;
}

TData * QBEM::GetTurbine(QString m_TurbineName)
{
	for (int i=0;i<g_tdataStore.size();i++)
    {
		if (g_tdataStore.at(i)->m_TurbineName == m_TurbineName) return g_tdataStore.at(i);
    }
    return NULL;
}

BData* QBEM::GetTurbineBladeData(QString Windspeed)
{

    if (!m_pTBEMData->m_BData.size()) return NULL;

    for (int i=0; i<m_pTBEMData->m_BData.size(); i++)
    {
        if (m_pTBEMData->m_BData.at(i)->windspeedStr==Windspeed)
        {
            return m_pTBEMData->m_BData.at(i);
        }
    }

    return NULL;
}

// new code JW //
void * QBEM::GetWeibullXVariable(int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;

	switch (iVar){
			case 0:
					pVar = &m_k;
					break;
			case 1:
					pVar = &m_A;
					break;
			default:
					pVar = &m_k;
					break;
	}
	return pVar;
}

void * QBEM::GetWeibullYVariable(void *Data, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;
	TBEMData *pTBEMData = (TBEMData *) Data;

	switch (iVar){
			case 0:
					pVar = &pTBEMData->m_aepk;
					break;
			case 1:
					pVar = &pTBEMData->m_aepA;
					break;
			default:
					pVar = &pTBEMData->m_aepk;
					break;
	}
	return pVar;

}
// end new code JW

void * QBEM::GetTurbineRotorVariable(void *Data, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        void * pVar;
        TBEMData *pTBEMData = (TBEMData *) Data;

        switch (iVar){
                case 0:

                        pVar = &pTBEMData->m_P;
                        break;
                case 1:

                        pVar = &pTBEMData->m_S;
                        break;
                case 2:

                        pVar = &pTBEMData->m_Torque;
                        break;
                case 3:

                        pVar = &pTBEMData->m_Bending;
                        break;
                case 4:

                        pVar = &pTBEMData->m_V;
                        break;
                case 5:

                        pVar = &pTBEMData->m_Lambda;
                        break;
                case 6:

                        pVar = &pTBEMData->m_one_over_Lambda;
                        break;
                case 7:

                        pVar = &pTBEMData->m_Omega;
                        break;
                case 8:

                        pVar = &pTBEMData->m_Pitch;
                        break;
                case 9:

                        pVar = &pTBEMData->m_Cp;
                        break;
                case 10:

                        pVar = &pTBEMData->m_Ct;
                        break;
                case 11:

                        pVar = &pTBEMData->m_Cm;
                        break;
                case 12:

                        pVar = &pTBEMData->m_Kp;
                        break;
                case 13:

                        pVar = &pTBEMData->m_Cp_loss;
                        break;
                case 14:

                        pVar = &pTBEMData->m_Weibull;
                        break;
//                case 15:

//						pVar = &pTBEMData->m_WeibullV3;
//                        break;
                default:

                        pVar = &pTBEMData->m_Lambda;
                        break;
        }
        return pVar;
}


void * QBEM::GetTurbineBladeVariable(void *Data, int iVar)
{
    void * pVar;
    BData *pBData = (BData *) Data;



    switch (iVar){

            case 0:

                    pVar = &pBData->m_Cn;
                    break;
            case 1:

                    pVar = &pBData->m_Ct;
                    break;
            case 2:

                    pVar = &pBData->m_phi;
                    break;
            case 3:

                    pVar = &pBData->m_alpha;
                    break;
            case 4:

                    pVar = &pBData->m_theta;
                    break;
            case 5:

                    pVar = &pBData->m_c_local;
                    break;
            case 6:

                    pVar = &pBData->m_CL;
                    break;
            case 7:

                    pVar = &pBData->m_CD;
                    break;
            case 8:

                    pVar = &pBData->m_LD;
                    break;
            case 9:

                    pVar = &pBData->m_F;
                    break;
            case 10:

                    pVar = &pBData->m_a_axial;
                    break;
            case 11:

                    pVar = &pBData->m_a_tangential;
                    break;
            case 12:

                    pVar = &pBData->m_pos;
                    break;
            case 13:

                    pVar = &pBData->m_lambda_local;
                    break;
            case 14:

                    pVar = &pBData->m_Reynolds;
                    break;
            case 15:

                    pVar = &pBData->m_DeltaReynolds;
                    break;
            case 16:

                    pVar = &pBData->m_Roughness;
                    break;
            case 17:

                    pVar = &pBData->m_Windspeed;
                    break;
            case 18:

                    pVar = &pBData->m_p_tangential;
                    break;
            case 19:

                    pVar = &pBData->m_p_normal;
                    break;
            case 20:

                    pVar = &pBData->m_Iterations;
                    break;
            case 21:

                    pVar = &pBData->m_Mach;
                    break;
            case 22:

                    pVar = &pBData->m_circ;
                    break;
            case 23:

                    pVar = &pBData->m_Fa_axial;
                    break;
            case 24:

                    pVar = &pBData->m_Fa_radial;
                    break;
            default:

                    pVar = &pBData->m_pos;
                    break;
    }
    return pVar;
}

float *** QBEM::GetCharVariable(void *Data, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        float *** pVar;
        CBEMData *pCBEMData = (CBEMData *) Data;

        switch (iVar){
                case 0:

                        pVar = pCBEMData->m_P;
                        break;
                case 1:

                        pVar = pCBEMData->m_S;
                        break;
                case 2:

                        pVar = pCBEMData->m_Torque;
                        break;
                case 3:

                        pVar = pCBEMData->m_Bending;
                        break;
                case 4:

                        pVar = pCBEMData->m_V;
                        break;
                case 5:

                        pVar = pCBEMData->m_Lambda;
                        break;
                case 6:

                        pVar = pCBEMData->m_one_over_Lambda;
                        break;
                case 7:

                        pVar = pCBEMData->m_Omega;
                        break;
                case 8:

                        pVar = pCBEMData->m_Pitch;
                        break;
                case 9:

                        pVar = pCBEMData->m_Cp;
                        break;
                case 10:

                        pVar = pCBEMData->m_Ct;
                        break;
                case 11:

                        pVar = pCBEMData->m_Cm;
                        break;
                case 12:

                        pVar = pCBEMData->m_Kp;
                        break;


                default:

                        pVar = pCBEMData->m_P;
                        break;
        }
        return pVar;
}



void * QBEM::GetVariable(CPolar *pPolar, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        void * pVar;
        switch (iVar){
                case 0:
                        pVar = &pPolar->m_Alpha;
                        break;
                case 1:
                        pVar = &pPolar->m_Cl;
                        break;
                case 2:
                        pVar = &pPolar->m_Cd;
                        break;
                case 3:
                        pVar = &pPolar->m_Cd;
                        break;

                default:
                        pVar = &pPolar->m_Alpha;
                        break;
        }
        return pVar;
}

void * QBEM::GetVariable(C360Polar *pPolar, int iVar)
{
		// returns a pointer to the variable array defined by its index iVar
		void * pVar;
		switch (iVar){
				case 0:
						pVar = &pPolar->m_Alpha;
						break;
				case 1:
						pVar = &pPolar->m_Cl;
						break;
				case 2:
						pVar = &pPolar->m_Cd;
						break;
				case 3:
						pVar = &pPolar->m_Cd;
						break;  
                case 4:
                        pVar = &pPolar->m_Cl_att;
                        break;
                case 5:
                        pVar = &pPolar->m_Cl_sep;
                        break;
                case 6:
                        pVar = &pPolar->m_fst;
                        break;

				default:
						pVar = &pPolar->m_Alpha;
						break;
		}
		return pVar;
}

CBlade * QBEM::GetWing(QString WingName)
{
        int i;
        CBlade* pWing;
        for (i=0; i < g_rotorStore.size(); i++)
        {
                pWing = g_rotorStore.at(i);
                if (pWing->getName() == WingName) return pWing;
        }
        return NULL;
}

void QBEM::InvertedClicked(){

}

void QBEM::OnLengthHeightChanged(){

}

void QBEM::OnHubValChanged(){

}


void QBEM::GLCallViewLists() {
	if(m_bSurfaces) {
		if (m_pBlade)
			glCallList(WINGSURFACES);
	}
	
	if(m_bOutline) {
		if(m_pBlade)
			glCallList(WINGOUTLINE);
	}
	
	if(m_iSection>=0) {
		glCallList(SECTIONHIGHLIGHT);
	}
	
	if (m_pBlade) {
		if (m_pctrlShowTurbine->isChecked()) {
			for (int i = 1; i < m_pBlade->m_blades; ++i) {
				glRotated (360.0/m_pBlade->m_blades,0,0,1);
				if(m_bSurfaces) {
					if(m_pBlade)
						glCallList(WINGSURFACES);
				}
				
				if(m_bOutline) {
					if(m_pBlade)  glCallList(WINGOUTLINE);
				}
				
				if(m_iSection>=0) {
					glCallList(SECTIONHIGHLIGHT);
				}
			}
		}
	}
}

void QBEM::configureGL() {
	glPushAttrib(GL_ALL_ATTRIB_BITS);  // saves current GL settings
    glClearColor(g_mainFrame->m_BackgroundColor.redF(), g_mainFrame->m_BackgroundColor.greenF(), g_mainFrame->m_BackgroundColor.blueF(), 0.0);
	if (m_pctrlPerspective->isChecked()) {
		m_pGLWidget->camera()->setType(qglviewer::Camera::PERSPECTIVE);
	} else {
		m_pGLWidget->camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
	}
    UpdateView();
}

void QBEM::drawGL() {
	if (!m_pBlade) {  // NM TODO this should not be possible! Assumption needs validation.
		return;
	}    
	
	GLWidget *glWidget = g_mainFrame->getGlWidget();
	
    GLDraw3D();  // prepares the lists
    GLRenderView();  // renders the lists
	
	if (m_pctrlAxes->isChecked()) {
        glWidget->setOverpaintFont(QFont(g_mainFrame->m_TextFont.family(), 15));
		m_pBlade->drawCoordinateAxes();
	}

	if (m_pctrlFoilNames->isChecked()) {
		glWidget->setOverpaintFont(QFont(g_mainFrame->m_TextFont.family(), 10));
		for (int i = 0; i <= m_pBlade->m_NPanel; ++i) {
			glWidget->overpaintText(m_pBlade->m_PlanformSpan/10, m_pBlade->m_TPos[i], 0.0, m_pBlade->m_Airfoils[i]);
		}
	}
	
	if (m_pctrlPositions->isChecked()) {
		glWidget->setOverpaintFont(QFont(g_mainFrame->m_TextFont.family(), 10));
        for (int i = 0; i <= m_pBlade->m_NPanel; ++i) {
			glWidget->overpaintText(-m_pBlade->m_PlanformSpan/10, m_pBlade->m_TPos[i], 0.0,
									QString("%1 m").arg(m_pBlade->m_TPos[i], 7, 'f', 3, ' '));
        }
    }
}

void QBEM::overpaint(QPainter &painter) {
	if (!m_pBlade) {
		return;
	}
	
	if (m_pGLWidget->width() > 300) {
		painter.setFont(QFont(g_mainFrame->m_TextFont.family(), 15));
		painter.drawText(10, 25, m_pBlade->getName());
	}
}

void QBEM::GLCreateGeom(CBlade *pWing, int List)
{
        if(!pWing) return;
        if (!pWing->m_Surface.size()) return;
        if (pWing->m_NSurfaces < 1) return;



        static int j, l, style, width;
        static CVector Pt, PtNormal, A, B, C, D, N, BD, AC, LATB, TALB;
        static QColor color;
        static CFoil * pFoilA, *pFoilB;

        static double x, xDistrib[SIDEPOINTS];
        double xx;
        double param = 50;// increase to refine L.E. and T.E.
        for(int i=0; i<SIDEPOINTS; i++)
        {
                xx = (double)i/(double)(SIDEPOINTS-1);
                xDistrib[i] = (asinh(param*(xx-0.5))/asinh(param/2.)+1.)/2.;
        }

        N.Set(0.0, 0.0, 0.0);


        glNewList(List,GL_COMPILE);
        {
                glLineWidth(1.0);

                color = pWing->m_WingColor;
                style = 0;
                width = 0;

//              glColor3d(color.redF(),color.greenF(),color.blueF());


                glColor4d(color.redF(),color.greenF(),color.blueF(),color.alphaF());
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0, 1.0);
                glEnable(GL_DEPTH_TEST);

//                top surface

                if (!m_bAdvancedEdit && m_pctrlSurfaces->isChecked())
                {
                    for (j=0; j<pWing->m_NSurfaces; j++)
                    {
                            glBegin(GL_QUAD_STRIP);
                            {
                                    for (l=0; l<=100; l++)
                                    {

                                            x = (double)l/100.0;

                                            pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                            glNormal3d(PtNormal.x, PtNormal.y, PtNormal.z);
                                            glVertex3d(Pt.x, Pt.y, Pt.z);

                                            pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                            glVertex3d(Pt.x, Pt.y, Pt.z);

                                    }
                            }
                            glEnd();
                    }
                }

                //strut top
                if (m_pctrlSurfaces->isChecked())
                {
                for (int k=0; k<pWing->m_StrutList.size();k++){
                    for (j=0; j<pWing->m_StrutList.at(k)->m_Surface.size(); j++)
                    {
                            glBegin(GL_QUAD_STRIP);
                            {
                                    for (l=0; l<=100; l++)
                                    {

                                            x = (double)l/100.0;

                                            pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                            glNormal3d(PtNormal.x, PtNormal.y, PtNormal.z);
                                            glVertex3d(Pt.x, Pt.y, Pt.z);

                                            pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                            glVertex3d(Pt.x, Pt.y, Pt.z);

                                    }
                            }
                            glEnd();
                    }
                }
                }

//                bottom surface
                        if (m_pctrlSurfaces->isChecked())
                        {
                                for (j=0; j<pWing->m_NSurfaces; j++)
                                {
                                        glBegin(GL_QUAD_STRIP);
                                        {
                                                for (l=0; l<=100; l++)
                                                {
                                                    x = (double)l/100.0;

                                                    pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                                    glNormal3d(-PtNormal.x, -PtNormal.y, -PtNormal.z);
                                                    glVertex3d(Pt.x, Pt.y, Pt.z);

                                                    pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                                    glVertex3d(Pt.x, Pt.y, Pt.z);
                                                }
                                        }
                                        glEnd();

                                }

                        }

                        //strut bottom
                        if (m_pctrlSurfaces->isChecked())
                        {
                        for (int k=0; k<pWing->m_StrutList.size();k++){
                            for (j=0; j<pWing->m_StrutList.at(k)->m_Surface.size(); j++)
                            {
                                    glBegin(GL_QUAD_STRIP);
                                    {
                                            for (l=0; l<=100; l++)
                                            {

                                                    x = (double)l/100.0;

                                                    pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                                    glNormal3d(-PtNormal.x, -PtNormal.y, -PtNormal.z);
                                                    glVertex3d(Pt.x, Pt.y, Pt.z);

                                                    pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                                    glVertex3d(Pt.x, Pt.y, Pt.z);

                                            }
                                    }
                                    glEnd();
                            }
                        }
                        }




                for (j=0; j<pWing->m_NSurfaces; j++)
                {                  

//left tip surface
                        if (j==0 && !m_bAdvancedEdit && m_pctrlSurfaces->isChecked())
                        {

                            glBegin(GL_QUAD_STRIP);
                            {
                                    pWing->m_Surface[0].GetPanel(0, 0, BOTSURFACE);
                                    C. Copy(pWing->m_Surface[0].LA);
                                    D. Copy(pWing->m_Surface[0].TA);
                                    pWing->m_Surface[0].GetPanel(0, 0, TOPSURFACE);
                                    A. Copy(pWing->m_Surface[0].TA);
                                    B. Copy(pWing->m_Surface[0].LA);

                                    BD = D-B;
                                    AC = C-A;
                                    N  = AC*BD;
                                    N.Normalize();
                                    glNormal3d( N.x, N.y, N.z);

                                    for (l=0; l<SIDEPOINTS; l++)
                                    {
                                            x = xDistrib[l];
                                            pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,1);

                                            glVertex3d(Pt.x, Pt.y, Pt.z);

                                            pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                            glVertex3d(Pt.x, Pt.y, Pt.z);
                                    }
                            }
                            glEnd();

                        }

                        // right tip surface
                        if (j==pWing->m_NSurfaces-1 && !m_bAdvancedEdit && m_pctrlSurfaces->isChecked())
                        {
                                      glBegin(GL_QUAD_STRIP);
                                      {
                                              pWing->m_Surface[pWing->m_NSurfaces-1].GetPanel(pWing->m_Surface[pWing->m_NSurfaces-1].m_NYPanels-1,0, TOPSURFACE);
                                              A. Copy(pWing->m_Surface[0].TB);
                                              B. Copy(pWing->m_Surface[0].LB);
                                              pWing->m_Surface[pWing->m_NSurfaces-1].GetPanel(pWing->m_Surface[pWing->m_NSurfaces-1].m_NYPanels-1,0, BOTSURFACE);
                                              C. Copy(pWing->m_Surface[0].LB);
                                              D. Copy(pWing->m_Surface[0].TB);

                                              BD = D-B;
                                              AC = C-A;
                                              N  = BD * AC;
                                              N.Normalize();
                                              glNormal3d( N.x,  N.y,  N.z);

                                              for (l=0; l<SIDEPOINTS; l++)
                                              {
                                                      x = xDistrib[l];
                                                      pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,1);

                                                      glVertex3d(Pt.x, Pt.y, Pt.z);
                                                      pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                                      glVertex3d(Pt.x, Pt.y, Pt.z);
                                              }
                                      }
                                      glEnd();
                        }

                        if (m_bAdvancedEdit || m_pctrlAirfoils->isChecked())
                        {
                                      glColor3d(m_OutlineColor.redF(),m_OutlineColor.greenF(),m_OutlineColor.blueF());
                                      glBegin(GL_QUAD_STRIP);
                                      {
                                              pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, TOPSURFACE);
                                              A. Copy(pWing->m_Surface[0].TB);
                                              B. Copy(pWing->m_Surface[0].LB);
                                              pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, BOTSURFACE);
                                              C. Copy(pWing->m_Surface[0].LB);
                                              D. Copy(pWing->m_Surface[0].TB);

                                              BD = D-B;
                                              AC = C-A;
                                              N  = BD * AC;
                                              N.Normalize();
                                              glNormal3d( N.x,  N.y,  N.z);

                                              for (l=0; l<SIDEPOINTS; l++)
                                              {
                                                      x = xDistrib[l];
                                                      pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);

                                                      glVertex3d(Pt.x, Pt.y, Pt.z);
                                                      pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                                      glVertex3d(Pt.x, Pt.y, Pt.z);
                                              }
                                      }
                                      glEnd();

                                      if (j==0)
                                      {
                                          glBegin(GL_QUAD_STRIP);
                                          {
                                                  pWing->m_Surface[j].GetPanel(0, 0, BOTSURFACE);
                                                  C. Copy(pWing->m_Surface[0].LA);
                                                  D. Copy(pWing->m_Surface[0].TA);
                                                  pWing->m_Surface[j].GetPanel(0, 0, TOPSURFACE);
                                                  A. Copy(pWing->m_Surface[0].TA);
                                                  B. Copy(pWing->m_Surface[0].LA);

                                                  BD = D-B;
                                                  AC = C-A;
                                                  N  = AC*BD;
                                                  N.Normalize();
                                                  glNormal3d( N.x, N.y, N.z);

                                                  for (l=0; l<SIDEPOINTS; l++)
                                                  {
                                                          x = xDistrib[l];
                                                          pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);

                                                          glVertex3d(Pt.x, Pt.y, Pt.z);

                                                          pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                                          glVertex3d(Pt.x, Pt.y, Pt.z);
                                                  }
                                          }
                                          glEnd();
                                      }
                        }
                }
        }

        for (int k=0; k<pWing->m_StrutList.size();k++){
            for (j=0; j<pWing->m_StrutList.at(k)->m_Surface.size(); j++)
            {

//left tip surface strut
                if (j==0 && !m_bAdvancedEdit && m_pctrlSurfaces->isChecked())
                {

                    glBegin(GL_QUAD_STRIP);
                    {
                            pWing->m_StrutList.at(k)->m_Surface[0].GetPanel(0, 0, BOTSURFACE);
                            C. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LA);
                            D. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TA);
                            pWing->m_StrutList.at(k)->m_Surface[0].GetPanel(0, 0, TOPSURFACE);
                            A. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TA);
                            B. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LA);

                            BD = D-B;
                            AC = C-A;
                            N  = AC*BD;
                            N.Normalize();
                            glNormal3d( N.x, N.y, N.z);

                            for (l=0; l<SIDEPOINTS; l++)
                            {
                                    x = xDistrib[l];
                                    pWing->m_StrutList.at(k)->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,1);

                                    glVertex3d(Pt.x, Pt.y, Pt.z);

                                    pWing->m_StrutList.at(k)->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                    glVertex3d(Pt.x, Pt.y, Pt.z);
                            }
                    }
                    glEnd();

                }

                // right tip surface strut
                if (j==pWing->m_StrutList.at(k)->m_Surface.size()-1 && !m_bAdvancedEdit && m_pctrlSurfaces->isChecked())
                {
                              glBegin(GL_QUAD_STRIP);
                              {
                                      pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].GetPanel(pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].m_NYPanels-1,0, TOPSURFACE);
                                      A. Copy(pWing->m_Surface[0].TB);
                                      B. Copy(pWing->m_Surface[0].LB);
                                      pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].GetPanel(pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].m_NYPanels-1,0, BOTSURFACE);
                                      C. Copy(pWing->m_Surface[0].LB);
                                      D. Copy(pWing->m_Surface[0].TB);

                                      BD = D-B;
                                      AC = C-A;
                                      N  = BD * AC;
                                      N.Normalize();
                                      glNormal3d( N.x,  N.y,  N.z);

                                      for (l=0; l<SIDEPOINTS; l++)
                                      {
                                              x = xDistrib[l];
                                              pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].GetPoint(x,x,1.0,Pt, PtNormal,1);

                                              glVertex3d(Pt.x, Pt.y, Pt.z);
                                              pWing->m_StrutList.at(k)->m_Surface[pWing->m_StrutList.at(k)->m_Surface.size()-1].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                              glVertex3d(Pt.x, Pt.y, Pt.z);
                                      }
                              }
                              glEnd();
                }

                if (m_bAdvancedEdit || m_pctrlAirfoils->isChecked())
                {
                              glColor3d(m_OutlineColor.redF(),m_OutlineColor.greenF(),m_OutlineColor.blueF());
                              glBegin(GL_QUAD_STRIP);
                              {
                                      pWing->m_StrutList.at(k)->m_Surface[j].GetPanel(pWing->m_StrutList.at(k)->m_Surface[j].m_NYPanels-1,0, TOPSURFACE);
                                      A. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TB);
                                      B. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LB);
                                      pWing->m_StrutList.at(k)->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, BOTSURFACE);
                                      C. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LB);
                                      D. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TB);

                                      BD = D-B;
                                      AC = C-A;
                                      N  = BD * AC;
                                      N.Normalize();
                                      glNormal3d( N.x,  N.y,  N.z);

                                      for (l=0; l<SIDEPOINTS; l++)
                                      {
                                              x = xDistrib[l];
                                              pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);

                                              glVertex3d(Pt.x, Pt.y, Pt.z);
                                              pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                              glVertex3d(Pt.x, Pt.y, Pt.z);
                                      }
                              }
                              glEnd();

                              if (j==0)
                              {
                                  glBegin(GL_QUAD_STRIP);
                                  {
                                          pWing->m_StrutList.at(k)->m_Surface[j].GetPanel(0, 0, BOTSURFACE);
                                          C. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LA);
                                          D. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TA);
                                          pWing->m_StrutList.at(k)->m_Surface[j].GetPanel(0, 0, TOPSURFACE);
                                          A. Copy(pWing->m_StrutList.at(k)->m_Surface[0].TA);
                                          B. Copy(pWing->m_StrutList.at(k)->m_Surface[0].LA);

                                          BD = D-B;
                                          AC = C-A;
                                          N  = AC*BD;
                                          N.Normalize();
                                          glNormal3d( N.x, N.y, N.z);

                                          for (l=0; l<SIDEPOINTS; l++)
                                          {
                                                  x = xDistrib[l];
                                                  pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);

                                                  glVertex3d(Pt.x, Pt.y, Pt.z);

                                                  pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                                  glVertex3d(Pt.x, Pt.y, Pt.z);
                                          }
                                  }
                                  glEnd();
                              }
                }
        }
        }


        glEndList();

        //OUTLINE
        glNewList(WINGOUTLINE,GL_COMPILE);
        {

                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glEnable (GL_LINE_STIPPLE);
                glLineWidth((GLfloat)m_OutlineWidth);

                color = m_OutlineColor;
                style = m_OutlineStyle;
                width = m_OutlineWidth;

                if     (style == 1) 	glLineStipple (1, 0x1111);
                else if(style == 2) 	glLineStipple (1, 0x0F0F);
                else if(style == 3) 	glLineStipple (1, 0x1C47);
                else					glLineStipple (1, 0xFFFF);

                glColor3d(color.redF(),color.greenF(),color.blueF());
                glLineWidth((GLfloat)width);

                if (m_pctrlOutline->isChecked())
                {
                //TOP outline
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();
                }

                //BOTTOM outline
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                }
                }


                if (m_pctrlOutline->isChecked())
                {
                //TOP outline strut
                for (int k=0; k<pWing->m_StrutList.size();k++){
                for (j=0; j<pWing->m_StrutList.at(k)->m_Surface.size(); j++)
                {
                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();
                }
                }

                //BOTTOM outline strut
                for (int k=0; k<pWing->m_StrutList.size();k++){
                for (j=0; j<pWing->m_StrutList.at(k)->m_Surface.size(); j++)
                {
                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                        glBegin(GL_LINE_STRIP);
                        {
                                for (l=0; l<=100; l++)
                                {
                                        x = (double)l/100.0;
                                        pWing->m_StrutList.at(k)->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                        }
                        glEnd();

                }
                }
                }

                if (m_pctrlOutlineEdge->isChecked()){

                glLineWidth((GLfloat)1);

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_Surface[pWing->m_NSurfaces-1].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();


                //WingContour
                //Leading edge outline
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        glBegin(GL_LINES);
                        {
                                pWing->m_Surface[j].GetPanel(0,pWing->m_Surface[j].m_NXPanels-1, MIDSURFACE);
                                glVertex3d(pWing->m_Surface[j].LA.x,
                                                   pWing->m_Surface[j].LA.y,
                                                   pWing->m_Surface[j].LA.z);
                                pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,pWing->m_Surface[j].m_NXPanels-1, MIDSURFACE);
                                glVertex3d(pWing->m_Surface[j].LB.x,
                                                   pWing->m_Surface[j].LB.y,
                                                   pWing->m_Surface[j].LB.z);
                        }
                        glEnd();
                }
                //Trailing edge outline
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        glBegin(GL_LINES);
                        {
                                pWing->m_Surface[j].GetPanel(0,0, MIDSURFACE);
                                glVertex3d(pWing->m_Surface[j].TA.x,
                                                   pWing->m_Surface[j].TA.y,
                                                   pWing->m_Surface[j].TA.z);
                                pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1, 0, MIDSURFACE);
                                glVertex3d(pWing->m_Surface[j].TB.x,
                                                   pWing->m_Surface[j].TB.y,
                                                   pWing->m_Surface[j].TB.z);
                        }
                        glEnd();
                }


                for (int k=0; k<pWing->m_StrutList.size();k++){


                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_StrutList[k]->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_StrutList[k]->m_Surface[0].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_StrutList[k]->m_Surface[pWing->m_StrutList[k]->m_Surface.size()-1].GetPoint(x,x,1.0,Pt, PtNormal,1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                {
                        for (l=0; l<=100; l++)
                        {
                                x = (double)l/100.0;
                                pWing->m_StrutList[k]->m_Surface[pWing->m_StrutList[k]->m_Surface.size()-1].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                                glVertex3d(Pt.x, Pt.y, Pt.z);
                        }
                }
                glEnd();


                //WingContour
                //Leading edge outline strut
                for (j=0; j<pWing->m_StrutList[k]->m_Surface.size(); j++)
                {
                        glBegin(GL_LINES);
                        {
                                pWing->m_StrutList[k]->m_Surface[j].GetPanel(0,pWing->m_StrutList[k]->m_Surface[j].m_NXPanels-1, MIDSURFACE);
                                glVertex3d(pWing->m_StrutList[k]->m_Surface[j].LA.x,
                                                   pWing->m_StrutList[k]->m_Surface[j].LA.y,
                                                   pWing->m_StrutList[k]->m_Surface[j].LA.z);
                                pWing->m_StrutList[k]->m_Surface[j].GetPanel( pWing->m_StrutList[k]->m_Surface[j].m_NYPanels-1,pWing->m_StrutList[k]->m_Surface[j].m_NXPanels-1, MIDSURFACE);
                                glVertex3d(pWing->m_StrutList[k]->m_Surface[j].LB.x,
                                                   pWing->m_StrutList[k]->m_Surface[j].LB.y,
                                                   pWing->m_StrutList[k]->m_Surface[j].LB.z);
                        }
                        glEnd();
                }
                //Trailing edge outline strut
                for (j=0; j<pWing->m_StrutList[k]->m_Surface.size(); j++)
                {
                        glBegin(GL_LINES);
                        {
                                pWing->m_StrutList[k]->m_Surface[j].GetPanel(0,0, MIDSURFACE);
                                glVertex3d(pWing->m_StrutList[k]->m_Surface[j].TA.x,
                                                   pWing->m_StrutList[k]->m_Surface[j].TA.y,
                                                   pWing->m_StrutList[k]->m_Surface[j].TA.z);
                                pWing->m_StrutList[k]->m_Surface[j].GetPanel( pWing->m_StrutList[k]->m_Surface[j].m_NYPanels-1, 0, MIDSURFACE);
                                glVertex3d(pWing->m_StrutList[k]->m_Surface[j].TB.x,
                                                   pWing->m_StrutList[k]->m_Surface[j].TB.y,
                                                   pWing->m_StrutList[k]->m_Surface[j].TB.z);
                        }
                        glEnd();
                }





                }



                //flap outline....
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        pFoilA = pWing->m_Surface[j].m_pFoilA;
                        pFoilB = pWing->m_Surface[j].m_pFoilB;
                        if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap)
                        {
                                glBegin(GL_LINES);
                                {
                                        if(pFoilA->m_bTEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
                                                                                                         0.0, Pt, 1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);

                                        if(pFoilB->m_bTEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
                                                                                                         1.0, Pt, 1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                                glEnd();
                                glBegin(GL_LINES);
                                {
                                        if(pFoilA->m_bTEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
                                                                                                         0.0, Pt, -1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);

                                        if(pFoilB->m_bTEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
                                                                                                         1.0, Pt, -1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                                glEnd();
                        }
                }
                for (j=0; j<pWing->m_NSurfaces; j++)
                {
                        pFoilA = pWing->m_Surface[j].m_pFoilA;
                        pFoilB = pWing->m_Surface[j].m_pFoilB;
                        if(pFoilA && pFoilB && pFoilA->m_bLEFlap && pFoilB->m_bLEFlap)
                        {
                                glBegin(GL_LINES);
                                {
                                        if(pFoilA->m_bLEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
                                                                                                         0.0, Pt, 1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);

                                        if(pFoilB->m_bLEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
                                                                                                         1.0, Pt, 1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                                glEnd();
                                glBegin(GL_LINES);
                                {
                                        if(pFoilA->m_bLEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
                                                                                                         0.0, Pt, -1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);

                                        if(pFoilB->m_bLEFlap)
                                                pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
                                                                                                         pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
                                                                                                         1.0, Pt, -1);
                                        else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
                                        glVertex3d(Pt.x, Pt.y, Pt.z);
                                }
                                glEnd();
                        }
                }

        }





                if (m_bAdvancedEdit)
                {

                if (!m_pctrlAxes->isChecked()){
                glLineWidth((GLfloat)2);
                glColor3d(255,0,0);
                glBegin(GL_LINE_STRIP);
                {
                    for (j=0; j<=pWing->m_NPanel; j++)
                    {
                        Pt=CVector(/*pWing->m_TPAxisX[j],*/0,pWing->m_TPAxisY[j],0/*pWing->m_TOffsetZ[j]*/);
                        glVertex3d(Pt.x, Pt.y, Pt.z);
                    }
                }
                glEnd();
                }

                glColor3d(0,0,255);
                glPointSize(5.0);
                glBegin(GL_POINTS);
                {
                    for (j=0; j<=pWing->m_NPanel; j++)
                    {
                        Pt=CVector(pWing->m_TPAxisX[j],pWing->m_TPAxisY[j],pWing->m_TOffsetZ[j]);
                        glVertex3d(Pt.x, Pt.y, Pt.z);
                    }
                }
                glEnd();
                }
                glDisable (GL_LINE_STIPPLE);
        }
        glEndList();

        glDisable (GL_BLEND);


}

void QBEM::GLCreateSectionHighlight()
{
        int j,l, num;

        num = 50;

        if (m_pBlade)
        {


        glNewList(SECTIONHIGHLIGHT,GL_COMPILE);
        {

                glPolygonMode(GL_FRONT,GL_LINE);
                glDisable (GL_LINE_STIPPLE);
//                glDisable (GL_LINE_SMOOTH);

                glColor3d(1.0, 0.0, 0.0);
                glLineWidth(5);

                double pos;
                CVector Pt, PtNormal;

                        if(m_iSection<m_pBlade->m_NPanel)
                        {
                                j = m_iSection;
                                glBegin(GL_LINE_STRIP);
                                {
                                        for (l=0; l<num; l++)
                                        {
                                            pos = 1.0/(double(num)-1.0)*l;
                                            m_pBlade->m_Surface[j].GetPoint(pos,pos,0.0,Pt, PtNormal,1);
                                            glVertex3d(Pt.x,Pt.y,Pt.z);

                                        }


                                        for (l=num; l>=0; l--)
                                        {
                                            pos = 1.0/(double(num)-1.0)*l;
                                            m_pBlade->m_Surface[j].GetPoint(pos,pos,0.0,Pt, PtNormal,-1);
                                            glVertex3d(Pt.x,Pt.y,Pt.z);
                                        }
                                }
                                glEnd();
                        }
                        else
                        {
                                j = m_iSection-1;
                                glBegin(GL_LINE_STRIP);
                                {
                                        for (l=0; l<num; l++)
                                        {
                                            pos = 1.0/(double(num)-1.0)*l;
                                            m_pBlade->m_Surface[j].GetPoint(pos,pos,1.0,Pt, PtNormal,1);
                                            glVertex3d(Pt.x,Pt.y,Pt.z);

                                        }


                                        for (l=num; l>=0; l--)
                                        {
                                            pos = 1.0/(double(num)-1.0)*l;
                                            m_pBlade->m_Surface[j].GetPoint(pos,pos,1.0,Pt, PtNormal,-1);
                                            glVertex3d(Pt.x,Pt.y,Pt.z);
                                        }
                                }
                                glEnd();
                        }

        }
        glEndList();
    }
}

void QBEM::GLDraw3D() {
	if (!m_pBlade) {
		m_bResetglGeom = true;
	}
    if (!m_pBlade->m_Surface.size()) return;
	
	if (m_bResetglGeom  && g_mainFrame->m_iView==BLADEVIEW) {
		if (m_pBlade) {
			if (glIsList(WINGSURFACES)) {
				glDeleteLists (WINGSURFACES, 2);
			}
			GLCreateGeom(m_pBlade,WINGSURFACES);
		}
		m_bResetglGeom = false;
	}
	
	if (m_bResetglSectionHighlight) {
		if (glIsList(SECTIONHIGHLIGHT)) {
			glDeleteLists (SECTIONHIGHLIGHT, 1);
		}
		if (m_iSection>=0) {
			GLCreateSectionHighlight ();
			m_bResetglSectionHighlight = false;
		}
	}
	m_bResetglGeom=false;
}

void QBEM::GLRenderView()
{
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);  // accept fragment if it is closer to the camera than the former one
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // polygons are filled from both sides
    glEnable(GL_POLYGON_OFFSET_FILL);  // polygons get a reduced Z-value. Now the grid is drawn onto the surface
    glPolygonOffset(1.0, 0);
    glLineWidth(1);

    glDisable(GL_POLYGON_SMOOTH);  // disable smooth functions that otherwise make rendering worse
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    double LightFactor;
    if(m_pBlade) LightFactor =  (GLfloat)pow(m_pBlade->m_PlanformSpan/2.0,0.1);
    else           LightFactor = 1.0;
    m_pGLWidget->GLSetupLight(m_GLLightDlg, 0, LightFactor);
    GLCallViewLists();
}

void QBEM::OnSingleMultiPolarChanged(){
    m_pBlade->m_bisSinglePolar = m_SingleMultiGroup->button(0)->isChecked();

    if (g_mainFrame->m_iApp == BEM){
        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar"));
        else m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar Range"));
    }
    else if (g_mainFrame->m_iApp == DMS){
        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar"));
        else m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar Range"));
    }

    for (int i=0; i<=m_pBlade->m_NPanel;i++){
        FillTableRow(i);
    }

    CheckWing();

}

void QBEM::OnPolarDialog(){
    if (m_iSection < 0) return;

    CFoil *pFoil = GetFoil(m_pBlade->m_Airfoils.at(m_iSection));
    if (pFoil){

        bool bFound = false;
        for (int i=0;i<g_360PolarStore.size();i++){
            if (g_360PolarStore.at(i)->m_airfoil->getName() == pFoil->getName()) bFound = true;
        }
        if (!bFound) return;
    }
    else return;

    PolarSelectionDialog dialog(pFoil, m_pBlade);

    if (QDialog::Accepted==dialog.exec()){
        bool bExisting = false;
        for (int i=0; i<m_pBlade->m_PolarAssociatedFoils.size();i++){
            if (m_pBlade->m_PolarAssociatedFoils.at(i) == pFoil->getName()){
                m_pBlade->m_MultiPolars.replace(i, dialog.GetPolarNamesList());
                m_pBlade->m_MinMaxReynolds.replace(i,QString("%1 to %2").arg(dialog.GetMin()).arg(dialog.GetMax()));
                bExisting = true;
                break;
            }
        }
        if(!bExisting){
            m_pBlade->m_PolarAssociatedFoils.append(pFoil->getName());
            m_pBlade->m_MultiPolars.append(dialog.GetPolarNamesList());
            m_pBlade->m_MinMaxReynolds.append(QString("%1 to %2").arg(dialog.GetMin()).arg(dialog.GetMax()));
        }
        for (int i=0;i<m_pBlade->m_Range.size();i++){
            if (m_pBlade->m_Airfoils.at(i) == pFoil->getName()){
            ReadSectionData(i);
            }
        }
    }

}



bool QBEM::InitDialog(CBlade *pWing)
{
        m_bResetglSectionHighlight = true;

        QString str;
        m_iSection = 0;

        if (m_pWingModel) delete m_pWingModel;
        if (m_pBladeDelegate) delete m_pBladeDelegate;

        m_pBlade = pWing;

        m_pctrlSave->setEnabled(false);
        m_pctrlOptimize->setEnabled(false);

        if(!m_pBlade) return false;

        ComputeGeometry();

		GetLengthUnit(str, g_mainFrame->m_LengthUnit);

        m_pctrlWingName->setText(m_pBlade->getName());

        m_pWingModel = new QStandardItemModel;
        m_pWingModel->setRowCount(100);//temporary
        m_pWingModel->setColumnCount(5);

        m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Pos (")+str+")");
        m_pWingModel->setHeaderData(1, Qt::Horizontal, tr("Chord (")+str+")");
        m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Twist"));
        m_pWingModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Foil"));

        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar"));
        else m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar Range"));

        m_pctrlBladeTable->setModel(m_pWingModel);

        OnResize();

        QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pWingModel);
        m_pctrlBladeTable->setSelectionModel(selectionModel);
        connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));

        m_pBladeDelegate = new BladeDelegate(m_pBlade, this);
        m_pctrlBladeTable->setItemDelegate(m_pBladeDelegate);
        connect(m_pBladeDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged()));

        int  *precision = new int[3];
        precision[0] = 3;
        precision[1] = 3;
        precision[2] = 2;

        m_pBladeDelegate->SetPointers(precision,&m_pBlade->m_NPanel);
        m_pBladeDelegate->itemmodel = m_pWingModel;

        FillDataTable();
        m_pctrlBladeTable->selectRow(m_iSection);
        SetCurrentSection(m_iSection);
        m_bResetglGeom = true;
        UpdateView();
        return true;
}

bool QBEM::InitAdvancedDialog(CBlade *pWing)
{

    m_bResetglSectionHighlight = true;

    QString str;
    m_iSection = 0;

    if (m_pBladeAxisModel) delete m_pBladeAxisModel;

    m_pBlade = pWing;

    m_pctrlSave->setEnabled(false);
    m_pctrlOptimize->setEnabled(false);


    if(!m_pBlade) return false;
    ComputeGeometry();

	GetLengthUnit(str, g_mainFrame->m_LengthUnit);

    m_pctrlWingName->setText(m_pBlade->getName());


    m_pBladeAxisModel = new QStandardItemModel;
    m_pBladeAxisModel->setRowCount(100);//temporary
    m_pBladeAxisModel->setColumnCount(4);

    m_pBladeAxisModel->setHeaderData(0, Qt::Horizontal, tr("Position (")+str+")");
    m_pBladeAxisModel->setHeaderData(1, Qt::Horizontal, tr("X Offset (")+str+")");
//    m_pBladeAxisModel->setHeaderData(2, Qt::Horizontal, tr("Dihedral (in deg)"));
    m_pBladeAxisModel->setHeaderData(2, Qt::Horizontal, tr("Z Offset (")+str+")");
    m_pBladeAxisModel->setHeaderData(3, Qt::Horizontal, tr("Twist Pt (%c)"));

//    m_pBladeAxisModel->setHeaderData(3, Qt::Horizontal, tr("Thread Axis Z (% chord)"));

    m_pctrlBladeAxisTable->setModel(m_pBladeAxisModel);

    OnResize();

    QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pBladeAxisModel);
    m_pctrlBladeAxisTable->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));

    m_pBladeAxisDelegate = new BladeAxisDelegate;
    m_pctrlBladeAxisTable->setItemDelegate(m_pBladeAxisDelegate);

    connect(m_pBladeAxisDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged()));


    int  *precision = new int[5];
    precision[0] = 3;
    precision[1] = 2;
    precision[2] = 2;
    precision[3] = 2;
    precision[4] = 2;

    m_pBladeAxisDelegate->SetPointers(precision,&m_pBlade->m_NPanel);

    m_pBladeAxisDelegate->itemmodel = m_pWingModel;

    FillDataTable();
    m_pctrlBladeAxisTable->selectRow(m_iSection);
    SetCurrentSection(m_iSection);
    m_bResetglGeom = true;
    UpdateView();
    return true;
}

void QBEM::OnResize()
{
    m_pctrlBladeTableView->setMaximumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    m_pctrlBladeTableView->setMinimumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    int unitwidth = (int)((m_pctrlBladeTableView->width())/6);
    m_pctrlBladeTableView->setColumnWidth(0,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(1,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(2,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(3,1.5*unitwidth);
    m_pctrlBladeTableView->setColumnWidth(4,1.5*unitwidth);

    m_pctrlBladeAxisTable->setMaximumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    m_pctrlBladeAxisTable->setMinimumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    unitwidth = (int)((m_pctrlBladeAxisTable->width())/4);
    m_pctrlBladeAxisTable->setColumnWidth(0,unitwidth);
    m_pctrlBladeAxisTable->setColumnWidth(1,unitwidth);
    m_pctrlBladeAxisTable->setColumnWidth(2,unitwidth);
    m_pctrlBladeAxisTable->setColumnWidth(3,unitwidth);

    m_pctrlBladeTable->setMaximumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    m_pctrlBladeTable->setMinimumWidth(0.9*g_mainFrame->m_pctrlBEMWidget->width());
    unitwidth = (int)((m_pctrlBladeTable->width())/6);
    m_pctrlBladeTable->setColumnWidth(0,unitwidth);
    m_pctrlBladeTable->setColumnWidth(1,unitwidth);
    m_pctrlBladeTable->setColumnWidth(2,unitwidth);
    m_pctrlBladeTable->setColumnWidth(3,1.5*unitwidth);
    m_pctrlBladeTable->setColumnWidth(4,1.5*unitwidth);
}

void QBEM::InitBladeTable()
{
	m_bResetglSectionHighlight = true;

    if (m_pBlade)
	{
		if (m_pWingModel) delete m_pWingModel;
				
		QString str;
		GetLengthUnit(str, g_mainFrame->m_LengthUnit);
		
		QString text, blades, hub;
        blades.sprintf("%.0f",double(m_pBlade->m_blades));
        hub.sprintf("%.2f",m_pBlade->m_HubRadius*g_mainFrame->m_mtoUnit);
        text = blades+" blades and "+hub+" "+str+" hub radius";
		m_pctrlBladesAndHubLabel->setText(text);

        m_pctrlWingNameLabel->setText(m_pBlade->getName());
        if (m_pBlade->m_bisSinglePolar) m_pctrlSingleMultiLabel->setText("");
        else m_pctrlSingleMultiLabel->setText("Multi Reynolds Number Polars");

		
		
		m_pWingModel = new QStandardItemModel;
		m_pWingModel->setRowCount(100);//temporary
		m_pWingModel->setColumnCount(5);
		
		m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Pos (")+str+")");
		m_pWingModel->setHeaderData(1, Qt::Horizontal, tr("Chord (")+str+")");
		m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Twist"));
		m_pWingModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Foil"));
        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar"));
        else m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Polar Range"));

		
		m_pctrlBladeTableView->setModel(m_pWingModel);
		
        OnResize();

		m_iSection = -1;
		FillDataTable();
		ComputeGeometry();
		m_bResetglGeom = true;
		UpdateView();
	}
	else
	{
		if (m_pWingModel) delete m_pWingModel;
		m_pWingModel = new QStandardItemModel;
		m_pWingModel->setRowCount(0);
		m_pWingModel->setColumnCount(0);
		m_pctrlBladeTableView->setModel(m_pWingModel);
	}
}

void QBEM::InitTurbineData(TData *pTData)
{

    QString strong, str;

    if (pTData)
    {
        if (pTData->isStall) Type->setText("Stall");
        if (pTData->isPitch) Type->setText("Pitch");
        if (pTData->isFixed) Trans->setText("Single");
        if (pTData->is2Step) Trans->setText("2 Step");
        if (pTData->isVariable) Trans->setText("Variable");
        if (pTData->isPrescribedRot) Trans->setText("Prescribed");
        if (pTData->isPrescribedPitch) Type->setText("Prescribed Pitch");


        Rot1->setText(strong.number(pTData->Rot1,'f',2));
        Rot2->setText(strong.number(pTData->Rot2,'f',2));
        Lambda0->setText(strong.number(pTData->Lambda0,'f',2));
		Generator->setText(strong.number(pTData->Generator*g_mainFrame->m_WtoUnit,'f',2));
		CutIn->setText(strong.number(pTData->CutIn*g_mainFrame->m_mstoUnit,'f',2));
		CutOut->setText(strong.number(pTData->CutOut*g_mainFrame->m_mstoUnit,'f',2));
		Switch->setText(strong.number(pTData->Switch*g_mainFrame->m_mstoUnit,'f',2));
		FixedLosses->setText(strong.number(pTData->m_fixedLosses*g_mainFrame->m_WtoUnit,'f',2));
        VariableLosses->setText(strong.number(pTData->VariableLosses,'f',3));
		OuterRadius->setText(strong.number(pTData->OuterRadius*g_mainFrame->m_mtoUnit,'f',2));
        Blade->setText(pTData->m_WingName);
        FixedPitch->setText(strong.number(pTData->FixedPitch,'f',2));




		GetLengthUnit(str, g_mainFrame->m_LengthUnit);
        Length1->setText(str);
		GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
        Speed1->setText(str);
        Speed2->setText(str);
        Speed3->setText(str);
		GetPowerUnit(str, g_mainFrame->m_PowerUnit);
        Power1->setText(str);
        Power2->setText(str);


TypeLabel->show();
Type->show();
GeneratorTypeLabel->show();
Trans->show();
GeneratorLabel->show();
Generator->show();
Power1->show();
CutInLabel->show();
CutIn->show();
Speed1->show();
CutOutLabel->show();
CutOut->show();
Speed2->show();
SwitchLabel->show();
Switch->show();
Speed3->show();
Rot1Label->show();
Rot1->show();
Rotspeed1->show();
Rot2Label->show();
Rot2->show();
Rotspeed2->show();
LambdaLabel->show();
Lambda0->show();
BladeLabel->show();
Blade->show();
OuterRadiusLabel->show();
OuterRadius->show();
Length1->show();
FixedPitchLabel->show();
FixedPitch->show();
VariableLossesLabel->show();
FixedLossesLabel->show();
Power2->show();
VariableLosses->show();
FixedLosses->show();



        if (pTData->isPitch)
        {
            Generator->show();
            GeneratorLabel->show();
            Power1->show();

        }
        else
        {
            Generator->hide();
            GeneratorLabel->hide();
            Power1->hide();

        }

        if(pTData->isPrescribedPitch)
        {
            Generator->hide();
            GeneratorLabel->hide();
            Power1->hide();
            FixedPitch->hide();
            FixedPitchLabel->hide();

        }

        if(pTData->isPrescribedRot)
        {
            Rot1Label->hide();
            Rot2Label->setText(tr(""));
            SwitchLabel->setText("");
            LambdaLabel->setText("");
            Rot1->hide();
            Rotspeed1->hide();
            Rot2->hide();
            Rotspeed2->hide();
            Switch->hide();
            Speed3->hide();
            Lambda0->hide();
        }


        if (pTData->isVariable)
        {
            Rot1Label->setText(tr("Rotational Speed Min"));
            Rot2Label->setText(tr("Rotational Speed Max"));
            SwitchLabel->setText("");
            LambdaLabel->setText("Tip Speed Ratio at Design Point");
            Rot1->show();
            Speed1->show();
            Rot2->show();
            Speed2->show();
            Switch->hide();
            Speed3->hide();
            Lambda0->show();

        }
        if (pTData->is2Step)
        {
            Rot1Label->setText(tr("Rotational Speed 1"));
            Rot2Label->setText(tr("Rotational Speed 2"));
            SwitchLabel->setText("V Switch");
            LambdaLabel->setText("");
            Rot1->show();
            Rotspeed1->show();
            Rot2->show();
            Rotspeed2->show();
            Switch->show();
            Speed3->show();
            Lambda0->hide();
        }
        if (pTData->isFixed)
        {
            Rot1Label->setText(tr("Rotational Speed"));
            Rot2Label->setText(tr(""));
            SwitchLabel->setText("");
            LambdaLabel->setText("");
            Rot1->show();
            Rotspeed1->show();
            Rot2->hide();
            Rotspeed2->hide();
            Switch->hide();
            Speed3->hide();
            Lambda0->hide();
        }
    }
    else
    {
        TypeLabel->hide();
        Type->hide();
        GeneratorTypeLabel->hide();
        Trans->hide();
        GeneratorLabel->hide();
        Generator->hide();
        Power1->hide();
        CutInLabel->hide();
        CutIn->hide();
        Speed1->hide();
        CutOutLabel->hide();
        CutOut->hide();
        Speed2->hide();
        SwitchLabel->hide();
        Switch->hide();
        Speed3->hide();
        Rot1Label->hide();
        Rot1->hide();
        Rotspeed1->hide();
        Rot2Label->hide();
        Rot2->hide();
        Rotspeed2->hide();
        LambdaLabel->hide();
        Lambda0->hide();
        BladeLabel->hide();
        Blade->hide();
        OuterRadiusLabel->hide();
        OuterRadius->hide();
        Length1->hide();
        FixedPitchLabel->hide();
        FixedPitch->hide();
        VariableLossesLabel->hide();
        FixedLossesLabel->hide();
        Power2->hide();
        VariableLosses->hide();
        FixedLosses->hide();
    }


}

void QBEM::InitTurbineSimulationParams(TBEMData *bladedata)
{

    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    QString strong;

    if(bladedata)
    {
    pSimuWidget->m_pctrlRhoVal->setText(strong.number(bladedata->rho,'f',4));
    pSimuWidget->m_pctrlElementsVal->setText(strong.number(bladedata->elements,'f',0));
    pSimuWidget->m_pctrlIterationVal->setText(strong.number(bladedata->iterations,'f',0));
    pSimuWidget->m_pctrlEpsilonVal->setText(strong.number(bladedata->epsilon,'f',4));
    pSimuWidget->m_pctrlRelaxVal->setText(strong.number(bladedata->relax,'f',1));
    pSimuWidget->m_pctrlViscVal->setText(strong.number(bladedata->visc,'e',8));
    pSimuWidget->m_pctrlTipLoss->setChecked(bladedata->m_bTipLoss);
    pSimuWidget->m_pctrlRootLoss->setChecked(bladedata->m_bRootLoss);
    pSimuWidget->m_pctrl3DCorrection->setChecked(bladedata->m_b3DCorrection);
    pSimuWidget->m_pctrlInterpolation->setChecked(bladedata->m_bInterpolation);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(bladedata->m_bNewRootLoss);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(bladedata->m_bNewTipLoss);
    pSimuWidget->m_pctrlCdReynolds->setChecked(bladedata->m_bCdReynolds);

    }
    if(!bladedata)
    {
    pSimuWidget->m_pctrlRhoVal->setText("");
    pSimuWidget->m_pctrlElementsVal->setText("");
    pSimuWidget->m_pctrlIterationVal->setText("");
    pSimuWidget->m_pctrlEpsilonVal->setText("");
    pSimuWidget->m_pctrlRelaxVal->setText("");
    pSimuWidget->m_pctrlViscVal->setText("");
    pSimuWidget->m_pctrlTipLoss->setChecked(false);
    pSimuWidget->m_pctrlRootLoss->setChecked(false);
    pSimuWidget->m_pctrl3DCorrection->setChecked(false);
    pSimuWidget->m_pctrlInterpolation->setChecked(false);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(false);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(false);
    pSimuWidget->m_pctrlCdReynolds->setChecked(false);

    }

    if(bladedata)
	{
		// JW modification
		//m_pctrlWA->setValue(bladedata->A);
		//m_pctrlWk->setValue(bladedata->k);
		m_pctrlWA->setEnabled(true);
		m_pctrlWk->setEnabled(true);
		m_pctrlPMA->setEnabled(true);
		m_pctrlPMk->setEnabled(true);
    }
    if(!bladedata)
	{
		m_pctrlWA->setEnabled(false);
		m_pctrlWk->setEnabled(false);
		m_pctrlPMA->setEnabled(false);
		m_pctrlPMk->setEnabled(false);
		m_pctrlYield->setText("");
    }



}

void QBEM::InitBladeSimulationParams(BEMData *bladedata)
{
    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    QString strong;

    if(bladedata)
    {
    pSimuWidget->m_pctrlRhoVal->setText(strong.number(bladedata->rho,'f',4));
    pSimuWidget->m_pctrlElementsVal->setText(strong.number(bladedata->elements,'f',0));
    pSimuWidget->m_pctrlIterationVal->setText(strong.number(bladedata->iterations,'f',0));
    pSimuWidget->m_pctrlEpsilonVal->setText(strong.number(bladedata->epsilon,'f',4));
    pSimuWidget->m_pctrlRelaxVal->setText(strong.number(bladedata->relax,'f',1));
    pSimuWidget->m_pctrlViscVal->setText(strong.number(bladedata->visc,'e',8));
    pSimuWidget->m_pctrlTipLoss->setChecked(bladedata->m_bTipLoss);
    pSimuWidget->m_pctrlRootLoss->setChecked(bladedata->m_bRootLoss);
    pSimuWidget->m_pctrl3DCorrection->setChecked(bladedata->m_b3DCorrection);
    pSimuWidget->m_pctrlInterpolation->setChecked(bladedata->m_bInterpolation);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(bladedata->m_bNewRootLoss);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(bladedata->m_bNewTipLoss);
    pSimuWidget->m_pctrlCdReynolds->setChecked(bladedata->m_bCdReynolds);

    }
    else
    {
    pSimuWidget->m_pctrlRhoVal->setText("");
    pSimuWidget->m_pctrlElementsVal->setText("");
    pSimuWidget->m_pctrlIterationVal->setText("");
    pSimuWidget->m_pctrlEpsilonVal->setText("");
    pSimuWidget->m_pctrlRelaxVal->setText("");
    pSimuWidget->m_pctrlViscVal->setText("");
    pSimuWidget->m_pctrlTipLoss->setChecked(false);
    pSimuWidget->m_pctrlRootLoss->setChecked(false);
    pSimuWidget->m_pctrl3DCorrection->setChecked(false);
    pSimuWidget->m_pctrlInterpolation->setChecked(false);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(false);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(false);
    pSimuWidget->m_pctrlCdReynolds->setChecked(false);

    }




}

void QBEM::InitCharSimulationParams(CBEMData *bladedata)
{
    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    QString strong;

    if(bladedata)
    {
    pSimuWidget->m_pctrlRhoVal->setText(strong.number(bladedata->rho,'f',4));
    pSimuWidget->m_pctrlElementsVal->setText(strong.number(bladedata->elements,'f',0));
    pSimuWidget->m_pctrlIterationVal->setText(strong.number(bladedata->iterations,'f',0));
    pSimuWidget->m_pctrlEpsilonVal->setText(strong.number(bladedata->epsilon,'f',4));
    pSimuWidget->m_pctrlRelaxVal->setText(strong.number(bladedata->relax,'f',1));
    pSimuWidget->m_pctrlViscVal->setText(strong.number(bladedata->visc,'e',8));
    pSimuWidget->m_pctrlTipLoss->setChecked(bladedata->m_bTipLoss);
    pSimuWidget->m_pctrlRootLoss->setChecked(bladedata->m_bRootLoss);
    pSimuWidget->m_pctrl3DCorrection->setChecked(bladedata->m_b3DCorrection);
    pSimuWidget->m_pctrlInterpolation->setChecked(bladedata->m_bInterpolation);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(bladedata->m_bNewRootLoss);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(bladedata->m_bNewTipLoss);
    pSimuWidget->m_pctrlCdReynolds->setChecked(bladedata->m_bCdReynolds);

    }
    if(!bladedata)
    {        
    pSimuWidget->m_pctrlRhoVal->setText("");
    pSimuWidget->m_pctrlElementsVal->setText("");
    pSimuWidget->m_pctrlIterationVal->setText("");
    pSimuWidget->m_pctrlEpsilonVal->setText("");
    pSimuWidget->m_pctrlRelaxVal->setText("");
    pSimuWidget->m_pctrlViscVal->setText("");
    pSimuWidget->m_pctrlTipLoss->setChecked(false);
    pSimuWidget->m_pctrlRootLoss->setChecked(false);
    pSimuWidget->m_pctrl3DCorrection->setChecked(false);
    pSimuWidget->m_pctrlInterpolation->setChecked(false);
    pSimuWidget->m_pctrlNewRootLoss->setChecked(false);
    pSimuWidget->m_pctrlNewTipLoss->setChecked(false);
    pSimuWidget->m_pctrlCdReynolds->setChecked(false);

    }



}

void QBEM::keyPressEvent(QKeyEvent *event)
{
        switch (event->key())
        {
         case Qt::Key_Control:
         {
                 UpdateView();
                 break;
         }

        case Qt::Key_X:
                m_bXPressed = true;
                break;

        case Qt::Key_Y:
                m_bYPressed = true;
                break;

         default:
                 QWidget::keyPressEvent(event);

         }
}

void QBEM::keyReleaseEvent(QKeyEvent *event)
{

        switch (event->key())
        {
                case Qt::Key_Control:
                {
                        UpdateView();
                        break;
                }

                case Qt::Key_X:
                        if(!event->isAutoRepeat()) m_bXPressed = false;
                        break;

                case Qt::Key_Y:
                        if(!event->isAutoRepeat()) m_bYPressed = false;
                        break;

                default:
                    event->ignore();
        }
}

void QBEM::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XBEM");
	{
		dlg_lambda      =       pSettings->value("Lambda").toDouble();
		dlg_epsilon     =       pSettings->value("Epsilon").toDouble();
		dlg_iterations  =       pSettings->value("Interations").toInt();
		dlg_elements    =       pSettings->value("Elements").toInt();
		dlg_rho         =       pSettings->value("Rho").toDouble();
		dlg_relax       =       pSettings->value("Relax").toDouble();
		dlg_tiploss     =       pSettings->value("TipLoss").toBool();
		dlg_rootloss    =       pSettings->value("RootLoss").toBool();
		dlg_3dcorrection=       pSettings->value("3DCorrection").toBool();
		dlg_interpolation=      pSettings->value("Interpolation").toBool();
		//m_iView         =       pSettings->value("iView").toInt();
        dlg_windspeed =       pSettings->value("tsrwindspeed").toDouble();
		dlg_lambdastart =       pSettings->value("lambdastart").toDouble();
		dlg_lambdaend   =       pSettings->value("lambdaend").toDouble();
		dlg_lambdadelta =       pSettings->value("lambdadelta").toDouble();
		dlg_windstart   =       pSettings->value("windstart").toDouble();
		dlg_windend     =       pSettings->value("windend").toDouble();
		dlg_winddelta   =       pSettings->value("winddelta").toDouble();
		dlg_newtiploss  =       pSettings->value("newtiploss").toBool();
		dlg_newrootloss =       pSettings->value("newrootloss").toBool();
		dlg_visc        =       pSettings->value("visc").toDouble();
		dlg_pitchstart  =       pSettings->value("pitchstart").toDouble();
		dlg_pitchend    =       pSettings->value("pitchend").toDouble();
		dlg_pitchdelta  =       pSettings->value("pitchdelta").toDouble();
		dlg_rotstart    =       pSettings->value("rotstart").toDouble();
		dlg_rotend      =       pSettings->value("rotend").toDouble();
		dlg_rotdelta    =       pSettings->value("rotdelta").toDouble();
		dlg_windstart2  =       pSettings->value("windstartt").toDouble();
		dlg_windend2    =       pSettings->value("windendt").toDouble();
		dlg_winddelta2  =       pSettings->value("winddeltat").toDouble();
		dlg_reynolds    =       pSettings->value("reynolds").toDouble();
	}
	pSettings->endGroup();
	
	m_GLLightDlg.LoadSettings(pSettings);
	
	m_RotorGraph1.LoadSettings(pSettings);
	m_RotorGraph2.LoadSettings(pSettings);
	m_RotorGraph3.LoadSettings(pSettings);
	m_PowerGraph1.LoadSettings(pSettings);
	m_PowerGraph2.LoadSettings(pSettings);
	m_PowerGraph3.LoadSettings(pSettings);
	m_CharGraph1.LoadSettings(pSettings);
	m_CharGraph2.LoadSettings(pSettings);
	m_CharGraph3.LoadSettings(pSettings);
	m_CharGraph4.LoadSettings(pSettings);
    m_360Graph1.LoadSettings(pSettings);
    m_360Graph2.LoadSettings(pSettings);
    m_360Graph3.LoadSettings(pSettings);
    m_360Graph4.LoadSettings(pSettings);

	SetPowerGraphTitles(&m_PowerGraph1);
	SetPowerGraphTitles(&m_PowerGraph2);
	SetPowerGraphTitles(&m_PowerGraph3);
	//new code JW//
	SetWeibullGraphTitles(&m_PowerGraph1);
	SetWeibullGraphTitles(&m_PowerGraph2);
	SetWeibullGraphTitles(&m_PowerGraph3);
	//end new code JW //
	SetRotorGraphTitles(&m_RotorGraph1);
	SetRotorGraphTitles(&m_RotorGraph2);
	SetRotorGraphTitles(&m_RotorGraph3);
    SetPolarGraphTitles(&m_360Graph1);
    SetPolarGraphTitles(&m_360Graph2);
    SetPolarGraphTitles(&m_360Graph3);
    SetPolarGraphTitles(&m_360Graph4);
	SetCharGraphTitles(&m_CharGraph1);
	SetCharGraphTitles(&m_CharGraph2);
	SetCharGraphTitles(&m_CharGraph3);
	SetCharGraphTitles(&m_CharGraph4);
}

void QBEM::mouseDoubleClickEvent ( QMouseEvent * /*event*/ )
{
        if(!m_pCurGraph) return;

        OnGraphSettings();
}

void QBEM::MouseMoveEvent(QMouseEvent *event)
{
        if(!hasFocus()) setFocus();
        static QPoint Delta, point;
//        static bool bCtrl;
        QPoint pt(event->pos().x(), event->pos().y());

        Delta.setX(event->pos().x() - m_LastPoint.x());
        Delta.setY(event->pos().y() - m_LastPoint.y());
        point = event->pos();
        m_pCurGraph = GetGraph(point);
//        bCtrl = false;
		if(event->modifiers() & Qt::ControlModifier) {/*bCtrl =true*/;}

        {

                m_pCurGraph = GetGraph(pt);
				if ((event->buttons() & Qt::LeftButton))// &&  ((m_iView==POLARVIEW) || (m_iView==BEMSIMVIEW)))
                {
                        if(m_pCurGraph)
                        {

                                // we translate the curves inside the graph
                                double xu, yu;
                                m_pCurGraph->SetAuto(false);
                                double x1 =  m_pCurGraph->ClientTox(m_LastPoint.x()) ;
                                double y1 =  m_pCurGraph->ClientToy(m_LastPoint.y()) ;

                                xu = m_pCurGraph->ClientTox(point.x());
                                yu = m_pCurGraph->ClientToy(point.y());

                                double xmin = m_pCurGraph->GetXMin() - xu+x1;
                                double xmax = m_pCurGraph->GetXMax() - xu+x1;
                                double ymin = m_pCurGraph->GetYMin() - yu+y1;
                                double ymax = m_pCurGraph->GetYMax() - yu+y1;

                                m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);
                                UpdateView();
                        }
						else if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
                        {
                                m_CharLegendOffset.rx() += Delta.x();
                                m_CharLegendOffset.ry() += Delta.y();
                                UpdateView();
                        }
						else if(g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
                        {
                                m_PowerLegendOffset.rx() += Delta.x();
                                m_PowerLegendOffset.ry() += Delta.y();
                                UpdateView();
                        }
                    }
            }
        m_LastPoint = point;
}

void QBEM::MousePressEvent(QMouseEvent *event)
{
        QPoint pt(event->x(), event->y()); //client coordinates

        m_pCurGraph = GetGraph(pt);

        if (event->buttons() & Qt::LeftButton)
        {
                QPoint point = event->pos();

                m_PointDown = point;
                m_LastPoint = point;
          }
          if (event->buttons() & Qt::RightButton)
          {
          m_pCurGraph = GetGraph(pt);
          }
    }

void QBEM::MouseReleaseEvent(QMouseEvent */*event*/)
{

}


void QBEM::OnPrescribePitch()
{

    QSizePolicy szPolicyMinimum;
    szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Expanding);
    szPolicyMinimum.setVerticalPolicy(QSizePolicy::Expanding);

    PrescribedValuesDlg *pTableWidget = new PrescribedValuesDlg;

    pTableWidget->editpitch=true;

    pTableWidget->setModal(true);

    pTableWidget->setSizePolicy(szPolicyMinimum);

    pTableWidget->cut_in = m_pctrlCutIn->getValue();
    pTableWidget->cut_out = m_pctrlCutOut->getValue();


    if (!pitchwindspeeds.size())
    {
            pTableWidget->OnReset();
    }
    else
    {
            pTableWidget->windspeeds = pitchwindspeeds;
            pTableWidget->value = pitchangles;
    }

    pTableWidget->InitDialog();


    pTableWidget->show();

    if(QDialog::Accepted == pTableWidget->exec())
    {
        pitchwindspeeds = pTableWidget->windspeeds;
        pitchangles = pTableWidget->value;
    }

}

void QBEM::OnPrescribeRot()
{

    QSizePolicy szPolicyMinimum;
    szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Expanding);
    szPolicyMinimum.setVerticalPolicy(QSizePolicy::Expanding);


    PrescribedValuesDlg *pTableWidget = new PrescribedValuesDlg;

    pTableWidget->editrot=true;

    pTableWidget->setModal(true);

    pTableWidget->setSizePolicy(szPolicyMinimum);


    pTableWidget->cut_in = m_pctrlCutIn->getValue();
    pTableWidget->cut_out = m_pctrlCutOut->getValue();



    if (!rotwindspeeds.size())
    {
            pTableWidget->OnReset();
    }
    else
    {
            pTableWidget->windspeeds = rotwindspeeds;
            pTableWidget->value = rotspeeds;
    }




    pTableWidget->InitDialog();

    pTableWidget->show();

    if(QDialog::Accepted == pTableWidget->exec())
    {
        rotwindspeeds = pTableWidget->windspeeds;
        rotspeeds = pTableWidget->value;
    }

}

void QBEM::TabChanged()
{

    if (SimpleAdvanced->currentIndex() == 0)
    {
    m_bAdvancedEdit = false;
    m_pBlade->setName(m_pctrlWingName->text());
    InitDialog(m_pBlade);
    }
    else if (SimpleAdvanced->currentIndex() == 1)
    {
    m_bAdvancedEdit = true;
    m_pBlade->setName(m_pctrlWingName->text());
    InitAdvancedDialog(m_pBlade);
    }

//    mainWidget->setCurrentIndex(0);
//    bladeWidget->setCurrentIndex(1);
}

void QBEM::OnAlignMaxThickness()
{
    CBlade *blade = m_pBlade;

	for (int i=0; i<=blade->m_NPanel;i++)
	{
		CFoil *foil = GetFoil(blade->m_Airfoils[i]);

		blade->m_TFoilPAxisX[i] = foil->m_fXThickness;

		for (int j=0;j<1001;j++)
		{
			if (foil->m_rpMid[j].x > foil->m_fXThickness)
			{

				blade->m_TFoilPAxisZ[i] = foil->m_rpMid[j-1].y + (foil->m_fXThickness-foil->m_rpMid[j-1].x)*(foil->m_rpMid[j].y-foil->m_rpMid[j-1].y)/(foil->m_rpMid[j].x-foil->m_rpMid[j-1].x);
				break;
			}
		}
		FillAdvancedTableRow(i);


	}
	ComputeGeometry();
	GLCreateSectionHighlight();
	m_bResetglGeom = true;
	UpdateView();

}

void QBEM::OnHubChanged()
{
    /////if the hub radius was changed, the blade positions are updated



    m_pBlade->m_HubRadius = m_pctrlHubRadius->getValue() / g_mainFrame->m_mtoUnit;
    double add;
    add = m_pBlade->m_HubRadius-m_pBlade->m_TPos[0];

    if (m_bAbsoluteBlade){
        m_pBlade->m_TPos[0]+=add;
        FillTableRow(0);
    }
    else{
        for (int i = 0;i<=m_pBlade->m_NPanel;i++)
        {
        m_pBlade->m_TPos[i]+=add;
        FillTableRow(i);
        }
    }

    ReadParams();
}

void QBEM::OnBladeColor()
{
    if(!m_pBlade) return;

    QColor WingColor = QColorDialog::getColor(m_pBlade->m_WingColor,
                                      this, "Select the wing color", QColorDialog::ShowAlphaChannel);
    if(WingColor.isValid()) m_pBlade->m_WingColor = WingColor;

    m_pctrlWingColor->SetColor(m_pBlade->m_WingColor);
    m_bResetglGeom = true;
    ComputeGeometry();
    UpdateView();

}

void QBEM::OnSectionColor()
{
    if(!m_pBlade) return;

    QColor SectionColor = QColorDialog::getColor(m_OutlineColor,
                                      this, "Select the section color", QColorDialog::ShowAlphaChannel);
    if(SectionColor.isValid()) m_OutlineColor = SectionColor;

    m_pctrlSectionColor->SetColor(m_OutlineColor);
    m_bResetglGeom = true;
    ComputeGeometry();
    UpdateView();

}

void QBEM::OnLightDlg()
{

    GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;

    GLLightDlg::s_bLight = m_bglLight;
    m_GLLightDlg.m_pGLWidget = m_pGLWidget;

    m_GLLightDlg.show();

    m_bglLight = GLLightDlg::s_bLight;


    double LightFactor;
    if(m_pBlade) LightFactor =  (GLfloat)pow(m_pBlade->m_PlanformSpan/2.0,0.1);
    else           LightFactor = 1.0;
    pGLWidget->GLSetupLight(m_GLLightDlg, 0, LightFactor);
    UpdateView();
	
}

void QBEM::onPerspectiveChanged() {
	if (m_pctrlPerspective->isChecked()) {
		m_pGLWidget->camera()->setType(qglviewer::Camera::PERSPECTIVE);
    } else {
		m_pGLWidget->camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
	}
	UpdateView();
}

void QBEM::OnSelChangeTurbine(int /*i*/)
{

			m_pTData = m_BEMToolBar->m_tdataComboBox->currentObject();


            InitTurbineData(m_pTData);
            UpdateTurbines();
            CheckButtons();


}


void QBEM::OnStartRotorSimulation()
{


    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    double lstart, lend, ldelta, inflowspeed;
    int times;

    lstart  =   pSimuWidget->m_pctrlLSLineEdit->getValue();
    lend    =   pSimuWidget->m_pctrlLELineEdit->getValue();
    ldelta  =   pSimuWidget->m_pctrlLDLineEdit->getValue();
    inflowspeed = pSimuWidget->m_pctrlWindspeed->getValue();
    times   =   int((lend-lstart)/ldelta);


    dlg_lambdastart = pSimuWidget->m_pctrlLSLineEdit->getValue();
    dlg_lambdaend   = pSimuWidget->m_pctrlLELineEdit->getValue();
    dlg_lambdadelta = pSimuWidget->m_pctrlLDLineEdit->getValue();
    dlg_windspeed = pSimuWidget->m_pctrlWindspeed->getValue();


    m_pBEMData->Clear();

    QProgressDialog progress("", "Abort BEM", 0, times, this);
    progress.setMinimumDuration(1000);
    progress.setModal(true);

    for (int i=0;i<=times;i++)
    {
    if (progress.wasCanceled()) break;

    QString curlambda;
    curlambda.sprintf("%.1f",lstart+i*ldelta);
    QString text = "Compute BEM for Lambda " + curlambda;
    progress.setLabelText(text);
    progress.setValue(i);


    m_pBladeData = new BData;
    m_pBEMData->Compute(m_pBladeData,m_pBlade,lstart+i*ldelta,inflowspeed);

	m_pBladeData->m_Color = g_mainFrame->GetColor(9);

    m_pBData = m_pBEMData->m_BData[0];

    selected_lambda = -1;

    CreateRotorCurves();
    }




    UpdateBlades();
    SetCurveParams();
    FillComboBoxes();

}


void QBEM::OnStartCharacteristicSimulation()
{
    if (!m_pCBEMData) return;
    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    BData *pBData;
    pBData = NULL;

    double vstart, vend, vdelta, windspeed;
    double rotstart, rotend, rotdelta, rot;
    double pitchstart, pitchend, pitchdelta, pitch;
    double lambda;
    int vtimes, rottimes, pitchtimes, times;

    m_pCBEMData->DeleteArrays(); //// if the simulation was run previously the old arrays are deleted

	vstart  = pSimuWidget->WindStart->getValue()/g_mainFrame->m_mstoUnit;;
    m_pCBEMData->windstart = vstart;
	vend    = pSimuWidget->WindEnd->getValue()/g_mainFrame->m_mstoUnit;;
    m_pCBEMData->windend = vend;
	vdelta  = pSimuWidget->WindDelta->getValue()/g_mainFrame->m_mstoUnit;;
    m_pCBEMData->winddelta = vdelta;
    vtimes = int((vend-vstart)/vdelta)+1;
    if (pSimuWidget->WindFixed->isChecked()) vtimes = 1;
    m_pCBEMData->windtimes = vtimes;

    rotstart    = pSimuWidget->RotStart->getValue();
    m_pCBEMData->rotstart = rotstart;
    rotend      = pSimuWidget->RotEnd->getValue();
    m_pCBEMData->rotend = rotend;
    rotdelta    = pSimuWidget->RotDelta->getValue();
    m_pCBEMData->rotdelta = rotdelta;
    rottimes = int((rotend-rotstart)/rotdelta)+1;
    if (pSimuWidget->RotFixed->isChecked()) rottimes = 1;
    m_pCBEMData->rottimes = rottimes;

    pitchstart  = pSimuWidget->PitchStart->getValue();
    m_pCBEMData->pitchstart = pitchstart;
    pitchend    = pSimuWidget->PitchEnd->getValue();
    m_pCBEMData->pitchend = pitchend;
    pitchdelta  = pSimuWidget->PitchDelta->getValue();
    m_pCBEMData->pitchdelta = pitchdelta;
    pitchtimes = int((pitchend-pitchstart)/pitchdelta)+1;
    if (pSimuWidget->PitchFixed->isChecked()) pitchtimes = 1;
    m_pCBEMData->pitchtimes = pitchtimes;

    times = (rottimes)*(vtimes)*(pitchtimes);

    dlg_windstart2  = pSimuWidget->WindStart->getValue();
    dlg_windend2    = pSimuWidget->WindEnd->getValue();
    dlg_winddelta2  = pSimuWidget->WindDelta->getValue();

    dlg_pitchstart  = pSimuWidget->PitchStart->getValue();
    dlg_pitchend    = pSimuWidget->PitchEnd->getValue();
    dlg_pitchdelta  = pSimuWidget->PitchDelta->getValue();

    dlg_rotstart    = pSimuWidget->RotStart->getValue();
    dlg_rotend      = pSimuWidget->RotEnd->getValue();
    dlg_rotdelta    = pSimuWidget->RotDelta->getValue();

    ////////get wing and associated polars;
    CBlade *pWing;
    pWing = m_pBlade;

    QProgressDialog progress("", "Abort BEM", 0, times, this);
    progress.setMinimumDuration(1000);
    progress.setModal(true);

    m_pCBEMData->initArrays(vtimes,rottimes,pitchtimes);



    int count = 0;
    for (int i=0;i<vtimes;i++)
    {
        windspeed = vstart+vdelta*i;       

        for (int j=0;j<rottimes;j++)
        {
            rot = rotstart+rotdelta*j;

            for (int k=0;k<pitchtimes;k++)
            {

            pitch = pitchstart+pitchdelta*k;

            if (pBData) delete pBData;
            pBData = new BData;



            if (progress.wasCanceled())
            {
                m_pCBEMData->simulated = false;
                UpdateCharacteristicsSimulation();
                return;
            }



            QString curwind;
            curwind.sprintf("%3.1f",windspeed);
            QString currot;
            currot.sprintf("%4.1f",rot);
            QString curpitch;
            curpitch.sprintf("%3.1f",pitch);
            QString text = "Compute BEM for Windspeed " + curwind + ", Rotational Speed " + currot + ", Pitch " + curpitch +"deg";
            progress.setLabelText(text);
            count++;
            progress.setValue(count);

            lambda = pWing->m_TPos[pWing->m_NPanel]*2*PI/60/windspeed*rot;


            m_pCBEMData->Compute(pBData,pWing,lambda,pitch,windspeed);



            m_pCBEMData->m_Omega[i][j][k]=(rot);
            m_pCBEMData->m_V[i][j][k]=(windspeed);
            m_pCBEMData->m_Torque[i][j][k]=(PI/2*pow(pWing->m_TPos[pWing->m_NPanel],2)*m_pCBEMData->rho*pow(windspeed,3)*pBData->cp/(rot/60*2*PI));
            m_pCBEMData->m_Ct[i][j][k]=(pBData->ct);
            m_pCBEMData->m_Lambda[i][j][k]=(lambda);
            m_pCBEMData->m_S[i][j][k]=(pow(pWing->m_TPos[pWing->m_NPanel],2)*PI*m_pCBEMData->rho/2*pow(windspeed,2)*pBData->ct);
            m_pCBEMData->m_Pitch[i][j][k]=(pitch);
            m_pCBEMData->m_Cp[i][j][k]=(pBData->cp);
            m_pCBEMData->m_Cm[i][j][k]=(pBData->cp/lambda);
            m_pCBEMData->m_Kp[i][j][k]=(pBData->cp/pow(pBData->lambda_global,3));
            m_pCBEMData->m_one_over_Lambda[i][j][k]=(1/pBData->lambda_global);
            m_pCBEMData->m_P[i][j][k]=(PI/2*pow(pWing->m_TPos[pWing->m_NPanel],2)*m_pCBEMData->rho*pow(windspeed,3)*pBData->cp);

            double bending = 0;
            for (int d=0;d<pBData->m_Reynolds.size();d++)
            {

				bending = bending + pow(pow((pow(windspeed*(1-pBData->m_a_axial.at(d)),2)+pow(windspeed*pBData->m_lambda_local.at(d)*(1+pBData->m_a_tangential.at(d)),2)),0.5),2)*m_pCBEMData->rho*0.5*pBData->m_c_local[d]*pBData->m_Cn[d]*pBData->deltas.at(d)*pBData->m_pos.at(d);

            }

			m_pCBEMData->m_Bending[i][j][k]= bending;

            }

        }

    }

    m_pCBEMData->simulated = true;




    UpdateCharacteristicsSimulation();

//    SetCurveParams();

//    FillComboBoxes();




}



void QBEM::OnStartTurbineSimulation()
{


    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

    double vstart, vend, vdelta, windspeed, lambda, rot;
    int times;
    double pitch = m_pTData->FixedPitch;

    rot = 200;



	vstart  =   pSimuWidget->m_pctrlWind1->getValue()/g_mainFrame->m_mstoUnit;
	vend    =   pSimuWidget->m_pctrlWind2->getValue()/g_mainFrame->m_mstoUnit;;
	vdelta  =   pSimuWidget->m_pctrlWindDelta->getValue()/g_mainFrame->m_mstoUnit;;
    times   =   int((vend-vstart)/vdelta);

    dlg_windstart   = pSimuWidget->m_pctrlWind1->getValue();
    dlg_windend     = pSimuWidget->m_pctrlWind2->getValue();
    dlg_winddelta   = pSimuWidget->m_pctrlWindDelta->getValue();

    m_pTBEMData->Clear();

    ////////get wing and associated polars;
    CBlade *pWing = NULL;
    for (int i=0; i < g_rotorStore.size(); i++)
    {
            pWing = g_rotorStore.at(i);
            if (pWing->getName() == m_pTData->m_WingName)
            {
                break;
            }
    }
    /////////////////

    QProgressDialog progress("", "Abort BEM", 0, times, this);
    progress.setMinimumDuration(1000);
    progress.setModal(true);

    for (int i=0;i<=times;i++)
    {

        m_pBladeData = new BData;

        if (progress.wasCanceled()) break;

        windspeed = vstart+vdelta*i;

        //// check which rotational speed is used (for fixed, 2step and variable or prescribed)////
        if (m_pTData->isFixed) rot = m_pTData->Rot1;

        if (m_pTData->is2Step)
        {
            if (windspeed < m_pTData->Switch) rot = m_pTData->Rot1;
            if (windspeed >= m_pTData->Switch) rot = m_pTData->Rot2;
        }

        if (m_pTData->isVariable)
        {
            rot = m_pTData->Lambda0*windspeed*60/2/PI/m_pTData->OuterRadius;
            if (rot<m_pTData->Rot1) rot = m_pTData->Rot1;
            if (rot>m_pTData->Rot2) rot = m_pTData->Rot2;

        }

        ////// gets the prescribed rotspeed lists and interpolated between windspeeds if neccessary

        if (m_pTData->isPrescribedRot)
        {
            for (int p=0;p<m_pTData->rotwindspeeds.size()-1;p++)
            {
                if (windspeed < m_pTData->rotwindspeeds.at(0))
                {
                    rot = 200;
                    break;
                }
                if (windspeed > m_pTData->rotwindspeeds.at(m_pTData->rotwindspeeds.size()-1))
                {
                    rot = 200;
                    break;
                }
                if (windspeed == m_pTData->rotwindspeeds.at(m_pTData->rotwindspeeds.size()-1))
                {
                    rot = m_pTData->rotspeeds.at(m_pTData->rotwindspeeds.size()-1);
                    break;
                }
                if (windspeed == m_pTData->rotwindspeeds.at(p))
                {
                    rot = m_pTData->rotspeeds.at(p);
                    break;
                }
                if (windspeed > m_pTData->rotwindspeeds.at(p) && windspeed < m_pTData->rotwindspeeds.at(p+1))
                {
                    rot = m_pTData->rotspeeds.at(p)+(m_pTData->rotspeeds.at(p+1)-m_pTData->rotspeeds.at(p))*(windspeed-m_pTData->rotwindspeeds.at(p))/(m_pTData->rotwindspeeds.at(p+1)-m_pTData->rotwindspeeds.at(p));
                    break;
                }
            }
        }

        ////////////////////////////////////////////////////////////
        QString curwind;
        curwind.sprintf("%.1f",windspeed);
        QString text = "Compute BEM for Windspeed " + curwind;
        progress.setLabelText(text);
        progress.setValue(i);

        lambda = m_pTData->OuterRadius*2*PI/60/windspeed*rot;


        ////// gets the prescribed pitch lists and interpolated between windspeeds if neccessary
        if (windspeed>=m_pTData->CutIn && windspeed<=m_pTData->CutOut)
        {
            if (m_pTData->isPrescribedPitch)
            {
                for (int p=0;p<m_pTData->pitchwindspeeds.size()-1;p++)
                {
                    if (windspeed < m_pTData->pitchwindspeeds.at(0))
                    {
                        pitch = 0;
                        break;
                    }
                    if (windspeed > m_pTData->pitchwindspeeds.at(m_pTData->pitchwindspeeds.size()-1))
                    {
                        pitch = 0;
                        break;
                    }
                    if (windspeed == m_pTData->pitchwindspeeds.at(m_pTData->pitchwindspeeds.size()-1))
                    {
                        pitch = m_pTData->pitchangles.at(m_pTData->pitchwindspeeds.size()-1);
                        break;
                    }
                    if (windspeed == m_pTData->pitchwindspeeds.at(p))
                    {
                        pitch = m_pTData->pitchangles.at(p);
                        break;
                    }
                    if (windspeed > m_pTData->pitchwindspeeds.at(p) && windspeed < m_pTData->pitchwindspeeds.at(p+1))
                    {
                        pitch = m_pTData->pitchangles.at(p)+(m_pTData->pitchangles.at(p+1)-m_pTData->pitchangles.at(p))*(windspeed-m_pTData->pitchwindspeeds.at(p))/(m_pTData->pitchwindspeeds.at(p+1)-m_pTData->pitchwindspeeds.at(p));
                        break;
                    }
                }
            }


/////


			m_pTBEMData->Compute(m_pBladeData,pWing,lambda,pitch,windspeed);



            int oo=0;


            if (m_pTData->isPitch)
                {
//if pitch controll is enabled compute pitch angles to reduce power output

                if ((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp-m_pTData->m_fixedLosses > m_pTData->Generator)
                    {

                        while ((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp-m_pTData->m_fixedLosses > m_pTData->Generator)
                            {
                                if (m_pBladeData) delete m_pBladeData;
                                m_pBladeData= new BData;
                                pitch = pitch - 0.05;
                                m_pTBEMData->Compute(m_pBladeData,pWing,lambda,pitch,windspeed);
                                oo++;
                                QString curpitch;
                                curpitch.sprintf("%.1f",pitch);
                                text = "Now Pitching at " + curpitch +"deg";
                                progress.setLabelText(text);
                                progress.setValue(i+oo);
                                if (progress.wasCanceled()) break;
                            }
                                m_pBladeData->cp = (m_pTData->Generator + m_pTData->m_fixedLosses)/((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3));
                    }

                }



// fill turbine data



                m_pTBEMData->m_Omega.append(rot);
                m_pTBEMData->m_V.append(windspeed);
                m_pTBEMData->m_BData.append(m_pBladeData);
                if ((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp-m_pTData->m_fixedLosses > 0)
                {
                m_pTBEMData->m_P.append((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp-m_pTData->m_fixedLosses);
                }
                else m_pTBEMData->m_P.append(0);
                m_pTBEMData->m_Torque.append(PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp/(rot/60*2*PI));
                m_pTBEMData->m_Cp_loss.append(((1-m_pTData->VariableLosses)*PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)*m_pBladeData->cp-m_pTData->m_fixedLosses)/(PI/2*pow(m_pTData->OuterRadius,2)*m_pTBEMData->rho*pow(windspeed,3)));
                m_pTBEMData->m_Ct.append(m_pBladeData->ct);
                m_pTBEMData->m_Lambda.append(lambda);
                m_pTBEMData->m_S.append(pow(m_pTData->OuterRadius,2)*PI*m_pTBEMData->rho/2*pow(windspeed,2)*m_pBladeData->ct);
                m_pTBEMData->m_Pitch.append(pitch);
                m_pTBEMData->m_Weibull.append(0);
				//m_pTBEMData->m_WeibullV3.append(0);
                m_pTBEMData->m_Cp.append(m_pBladeData->cp);
                m_pTBEMData->m_Cm.append(m_pBladeData->cp/lambda);
                m_pTBEMData->m_Kp.append(m_pBladeData->cp/pow(m_pBladeData->lambda_global,3));
                m_pTBEMData->m_one_over_Lambda.append(1/m_pBladeData->lambda_global);

                double bending = 0;
                for (int d=0;d<m_pBladeData->m_Reynolds.size();d++)
                {
                    m_pBladeData->m_Roughness[d] = 1000*100*dlg_visc/m_pTBEMData->rho/pow((pow(windspeed*(1-m_pBladeData->m_a_axial.at(d)),2)+pow(windspeed*m_pBladeData->m_lambda_local.at(d)*(1+m_pBladeData->m_a_tangential.at(d)),2)),0.5);
                    bending = bending + m_pBladeData->m_p_normal.at(d)*m_pBladeData->deltas.at(d)*m_pBladeData->m_pos.at(d);
                }

                m_pTBEMData->m_Bending.append(bending);


				m_pBladeData->m_Color = g_mainFrame->GetColor(10);

                m_pTurbineBData = m_pTBEMData->m_BData[0];


//                m_PowerGraph1.SetAuto(true);
//                m_PowerGraph2.SetAuto(true);
//                m_PowerGraph3.SetAuto(true);

				selected_windspeed = -1;
				//new code JW
				if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
					OnSetWeibull();
				else
				//end new code JW
					CreatePowerCurves();


        }

    }



    UpdateTurbines();
    SetCurveParams();
    FillComboBoxes();


}

void QBEM::OnOptimize()
{


    OptimizeDlg OptDlg(this);
	OptDlg.move(g_mainFrame->m_DlgPos);
    OptDlg.InitDialog();
    if (OptDlg.exec())
    {
    dlg_lambda = OptDlg.Lambda0->getValue();
    }
}

void QBEM::OnCreateCharacteristicSimulation()
{
	QString strong, num;
	CreateBEMDlg pBEMDlg(this);
	
    strong = m_pBlade->getName() + " Simulation";

	int j=1;


	for (int i=0;i<g_cbemdataStore.size();i++)
	{
		if (strong == g_cbemdataStore.at(i)->m_SimName)
		{
            qDebug() << g_cbemdataStore.size() << i;


			j++;
			num.sprintf("%1.0f",double(j));
            strong = m_pBlade->getName() + " Simulation" + " ("+num+")";
			i=0;
		}
    }

	pBEMDlg.SimName->setText(strong);
	
	if (pBEMDlg.exec())
	{
		CBEMData *pCBEMData = new CBEMData;
		
		pCBEMData->m_SimName = pBEMDlg.SimName->text();
		pCBEMData->setName(pBEMDlg.SimName->text());
		
		
		pCBEMData->elements = pBEMDlg.ElementsEdit->getValue();
		pCBEMData->iterations = pBEMDlg.IterationsEdit->getValue();
		pCBEMData->epsilon = pBEMDlg.EpsilonEdit->getValue();
		pCBEMData->m_bTipLoss = pBEMDlg.TipLossBox->isChecked();
		pCBEMData->m_bRootLoss = pBEMDlg.RootLossBox->isChecked();
		pCBEMData->m_b3DCorrection = pBEMDlg.ThreeDBox->isChecked();
		pCBEMData->m_bInterpolation = pBEMDlg.InterpolationBox->isChecked();
		pCBEMData->relax = pBEMDlg.RelaxEdit->getValue();
		pCBEMData->m_Color = g_mainFrame->GetColor(12);
        pCBEMData->m_WingName = m_pBlade->getName();
        pCBEMData->setSingleParent(m_pBlade);
		pCBEMData->rho = pBEMDlg.RhoEdit->getValue();
		pCBEMData->visc = pBEMDlg.ViscEdit->getValue();
		pCBEMData->m_bNewRootLoss = pBEMDlg.NewRootLossBox->isChecked();
		pCBEMData->m_bNewTipLoss=pBEMDlg.NewTipLossBox->isChecked();
		pCBEMData->m_bCdReynolds=pBEMDlg.ReynoldsBox->isChecked();
		

        if (!g_cbemdataStore.add(pCBEMData)) pCBEMData = NULL;

		m_pCBEMData = pCBEMData;

        m_BEMToolBar->m_cbemdataComboBox->setCurrentObject(m_pCBEMData);
		
        //////set the selected values as standard values for next definition of a simulation///
		dlg_rho = pCBEMData->rho;
		dlg_relax = pCBEMData->relax;
		dlg_iterations = pCBEMData->iterations;
		dlg_elements = pCBEMData->elements;
		dlg_tiploss = pCBEMData->m_bTipLoss;
		dlg_epsilon = pCBEMData->epsilon;
		dlg_rootloss = pCBEMData->m_bRootLoss;
		dlg_3dcorrection = pCBEMData->m_b3DCorrection;
		dlg_interpolation = pCBEMData->m_bInterpolation;
		dlg_visc = pCBEMData->visc;
		dlg_newrootloss = pCBEMData->m_bNewRootLoss;
		dlg_newtiploss = pCBEMData->m_bNewTipLoss;
		dlg_reynolds = pCBEMData->m_bCdReynolds;
		////

		CheckButtons();
		UpdateCharacteristicsSimulation();
	}
}

void QBEM::OnCreateRotorSimulation()
{
	QString strong, num;
	CreateBEMDlg pBEMDlg(this);
    strong = m_pBlade->getName() + " Simulation";
	
	int j=1;
	for (int i=0;i<g_bemdataStore.size();i++)
	{
		if (strong == g_bemdataStore.at(i)->m_BEMName)
		{
			j++;
			num.sprintf("%1.0f",double(j));
            strong = m_pBlade->getName() + " Simulation" + " ("+num+")";
			i=0;
		}
	}
	
	pBEMDlg.SimName->setText(strong);
	
	if (pBEMDlg.exec())
	{
		BEMData *pBEMData = new BEMData;
		pBEMData->m_BEMName = pBEMDlg.SimName->text();
		pBEMData->setName(pBEMDlg.SimName->text());
		
		
		pBEMData->elements = pBEMDlg.ElementsEdit->getValue();
		pBEMData->iterations = pBEMDlg.IterationsEdit->getValue();
		pBEMData->epsilon = pBEMDlg.EpsilonEdit->getValue();
		pBEMData->m_bTipLoss = pBEMDlg.TipLossBox->isChecked();
		pBEMData->m_bRootLoss = pBEMDlg.RootLossBox->isChecked();
		pBEMData->m_b3DCorrection = pBEMDlg.ThreeDBox->isChecked();
		pBEMData->m_bInterpolation = pBEMDlg.InterpolationBox->isChecked();
		pBEMData->relax = pBEMDlg.RelaxEdit->getValue();
		pBEMData->m_Color = g_mainFrame->GetColor(7);
        pBEMData->m_WingName = m_pBlade->getName();
        pBEMData->setSingleParent(m_pBlade);
		pBEMData->rho = pBEMDlg.RhoEdit->getValue();
		pBEMData->visc = pBEMDlg.ViscEdit->getValue();
		pBEMData->m_bNewRootLoss = pBEMDlg.NewRootLossBox->isChecked();
		pBEMData->m_bNewTipLoss=pBEMDlg.NewTipLossBox->isChecked();
		pBEMData->m_bCdReynolds = pBEMDlg.ReynoldsBox->isChecked();
		
        if (!g_bemdataStore.add(pBEMData)) pBEMData = NULL;
		m_pBEMData = pBEMData;
		
		//////set the selected values as standart values for next definition of a simulation///
		dlg_rho = pBEMData->rho;
		dlg_relax = pBEMData->relax;
		dlg_iterations = pBEMData->iterations;
		dlg_elements = pBEMData->elements;
		dlg_tiploss = pBEMData->m_bTipLoss;
		dlg_epsilon = pBEMData->epsilon;
		dlg_rootloss = pBEMData->m_bRootLoss;
		dlg_3dcorrection = pBEMData->m_b3DCorrection;
		dlg_interpolation = pBEMData->m_bInterpolation;
		dlg_visc = pBEMData->visc;
		dlg_newrootloss = pBEMData->m_bNewRootLoss;
		dlg_newtiploss = pBEMData->m_bNewTipLoss;
		dlg_reynolds = pBEMData->m_bCdReynolds;
		////
		
		CheckButtons();
		UpdateRotorSimulation();
	}
}

void QBEM::OnCreateTurbineSimulation()
{
 QString strong, num;

 strong = m_pTData->m_TurbineName + " Simulation";

 int j=1;

 for (int i=0;i<g_tbemdataStore.size();i++)
 {
		if (strong == g_tbemdataStore.at(i)->m_SimName)
        {
        j++;
        num.sprintf("%1.0f",double(j));
        strong = m_pTData->m_TurbineName + " Simulation" + " ("+num+")";
        i=0;
        }
 }


 CreateBEMDlg pBEMDlg(this);

 pBEMDlg.SimName->setText(strong);

 if (pBEMDlg.exec())
 {

 TBEMData *pTBEMData = new TBEMData;

 pTBEMData->m_SimName = pBEMDlg.SimName->text();
 pTBEMData->setName(pBEMDlg.SimName->text());


 pTBEMData->m_TurbineName = m_pTData->m_TurbineName;
 pTBEMData->setSingleParent(m_pTData);
 pTBEMData->OuterRadius = m_pTData->OuterRadius;
 pTBEMData->relax = pBEMDlg.RelaxEdit->getValue();
 pTBEMData->elements = pBEMDlg.ElementsEdit->getValue();
 pTBEMData->iterations = pBEMDlg.IterationsEdit->getValue();
 pTBEMData->epsilon = pBEMDlg.EpsilonEdit->getValue();
 pTBEMData->m_bTipLoss = pBEMDlg.TipLossBox->isChecked();
 pTBEMData->m_bRootLoss = pBEMDlg.RootLossBox->isChecked();
 pTBEMData->m_b3DCorrection = pBEMDlg.ThreeDBox->isChecked();
 pTBEMData->m_bInterpolation = pBEMDlg.InterpolationBox->isChecked();
 pTBEMData->rho = pBEMDlg.RhoEdit->getValue();
 pTBEMData->visc = pBEMDlg.ViscEdit->getValue();
 pTBEMData->m_bNewRootLoss = pBEMDlg.NewRootLossBox->isChecked();
 pTBEMData->m_bNewTipLoss=pBEMDlg.NewTipLossBox->isChecked();
 pTBEMData->m_bCdReynolds=pBEMDlg.ReynoldsBox->isChecked();


 pTBEMData->m_Color = g_mainFrame->GetColor(8);

 if (!g_tbemdataStore.add(pTBEMData)) pTBEMData = NULL;

 m_pTBEMData = pTBEMData;
 m_pTurbineBData = NULL;
 selected_windspeed = -1;

 dlg_rho = pTBEMData->rho;
 dlg_relax = pTBEMData->relax;
 dlg_iterations = pTBEMData->iterations;
 dlg_elements = pTBEMData->elements;
 dlg_tiploss = pTBEMData->m_bTipLoss;
 dlg_epsilon = pTBEMData->epsilon;
 dlg_rootloss = pTBEMData->m_bRootLoss;
 dlg_3dcorrection = pTBEMData->m_b3DCorrection;
 dlg_interpolation = pTBEMData->m_bInterpolation;
 dlg_visc = pTBEMData->visc;
 dlg_newrootloss = pTBEMData->m_bNewRootLoss;
 dlg_newtiploss = pTBEMData->m_bNewTipLoss;
 dlg_reynolds = pTBEMData->m_bCdReynolds;

 CheckButtons();
 UpdateTurbines();
 InitTurbineSimulationParams(m_pTBEMData);
 }



}




void QBEM::OnCurveColor()
{
        bool bOK;
        QRgb rgb = m_CurveColor.rgba();
        rgb = QColorDialog::getRgba(rgb, &bOK);
        m_CurveColor = QColor::fromRgba(rgb);

        FillComboBoxes();

        UpdateCurve();
}

void QBEM::OnCurveStyle(int index)
{
        m_CurveStyle = index;
        FillComboBoxes();
        UpdateCurve();
}
void QBEM::OnCurveWidth(int index)
{
        m_CurveWidth = index+1;
        FillComboBoxes();
        UpdateCurve();
}





void QBEM::OnShowPoints()
{
        SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if (m_pBEMData)
                {
                        m_pBEMData->m_bShowPoints = pSimuWidget->m_pctrlShowPoints->isChecked();
                }
                CreateRotorCurves();
        }
        else if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
        {
            if (m_pCur360Polar)
            {
                m_pCur360Polar->m_bShowPoints = m_pctrlShowPoints->isChecked();
            }
            if (!m_bNew360Polar && !m_bDecompose) CreatePolarCurve();
            else CreateSinglePolarCurve();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
            if (m_pTBEMData)
            {
                m_pTBEMData->m_bShowPoints = pSimuWidget->m_pctrlShowPoints->isChecked();
            }
                CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
        {
            if (m_pCBEMData)
            {
                m_pCBEMData->m_bShowPoints = pSimuWidget->m_pctrlShowPoints->isChecked();
            }
                CreateCharacteristicsCurves();
        }

		g_mainFrame->SetSaveState(false);
        UpdateView();
}

void QBEM::OnShowCurve()
{
        //user has toggled visible switch
        SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if (m_pBEMData)
                {
                        m_pBEMData->m_bIsVisible = pSimuWidget->m_pctrlShowCurve->isChecked();

                }
                CreateRotorCurves();
        }
        else if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
        {
            if (m_pCur360Polar)
            {
                m_pCur360Polar->m_bIsVisible = m_pctrlShowCurve->isChecked();
            }
            if (!m_bNew360Polar && !m_bDecompose) CreatePolarCurve();
            else CreateSinglePolarCurve();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
            if (m_pTBEMData)
            {
                m_pTBEMData->m_bIsVisible = pSimuWidget->m_pctrlShowCurve->isChecked();
            }
                CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
        {
            if (m_pCBEMData)
            {
                m_pCBEMData->m_bIsVisible = pSimuWidget->m_pctrlShowCurve->isChecked();
            }
                CreateCharacteristicsCurves();
        }
		g_mainFrame->SetSaveState(false);
        UpdateView();
}

void QBEM::OnShowOpPoint()
{
        //user has toggled visible switch
        SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

        m_bShowOpPoint = pSimuWidget->m_pctrlShowOpPoint->isChecked();


		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                CreateRotorCurves();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
                CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
        {
                CreateCharacteristicsCurves();
        }
        UpdateView();

}


void QBEM::OnBladeGraph()
{
	m_pCurGraph->m_Type = 1;
	m_pCurGraph->SetAutoX(true);
	m_pCurGraph->SetAutoY(true);
	
	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
	{
		m_pCurGraph->SetXVariable(1);
		m_pCurGraph->SetYVariable(0);
		SetRotorGraphTitles(m_pCurGraph);
	}
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
	{
		m_pCurGraph->SetXVariable(12);
		m_pCurGraph->SetYVariable(10);
		SetPowerGraphTitles(m_pCurGraph);
		SetWeibullGraphTitles(m_pCurGraph);//new code JW
	}
	
	m_pCurGraph->SetAuto(true);
	
	CreateRotorCurves();
	CreatePowerCurves();
}


void QBEM::OnRotorGraph()
{
m_pCurGraph->m_Type = 0;


m_pCurGraph->SetAutoX(true);
m_pCurGraph->SetAutoY(true);

if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
{
    m_pCurGraph->SetXVariable(0);
    m_pCurGraph->SetYVariable(2);
    SetRotorGraphTitles(m_pCurGraph);
}
if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
{
    m_pCurGraph->SetXVariable(2);
    m_pCurGraph->SetYVariable(0);
	SetPowerGraphTitles(m_pCurGraph);
	SetWeibullGraphTitles(m_pCurGraph);//new code JW
}

m_pCurGraph->SetAuto(true);


CreateRotorCurves();
CreatePowerCurves();
}


//new code JW
void QBEM::OnWeibullGraph()
{
m_pCurGraph->m_Type = 3;

m_pCurGraph->SetAutoX(true);
m_pCurGraph->SetAutoY(true);

m_pCurGraph->SetXVariable(0);
m_pCurGraph->SetYVariable(0);
SetWeibullGraphTitles(m_pCurGraph);

m_pCurGraph->SetAuto(true);

OnSetWeibull();

}

void QBEM::OnAziGraph()
{
    m_pCurGraph->m_Type = 2;
}
// end new code JW

void QBEM::OnShowAllRotorCurves()
{
	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		for (int i=0; i< g_bemdataStore.size();i++)
        {
			g_bemdataStore.at(i)->m_bIsVisible = true;
        }
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {
		for( int i=0;i<g_tbemdataStore.size();i++)
        {
			g_tbemdataStore.at(i)->m_bIsVisible = true;
        }
    }
    if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
    {
        for( int i=0;i<g_360PolarStore.size();i++)
        {
            g_360PolarStore.at(i)->m_bIsVisible = true;
        }
    }


    CreateRotorCurves();
    CreatePowerCurves();
    CreatePolarCurve();
    SetCurveParams();
}

void QBEM::OnHideAllRotorCurves()
{
	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		for (int i=0; i< g_bemdataStore.size();i++)
        {
			g_bemdataStore.at(i)->m_bIsVisible = false;
            if (m_pBEMData) m_pBEMData->m_bIsVisible = true;

        }
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {
		for (int i=0;i<g_tbemdataStore.size();i++)
        {
			g_tbemdataStore.at(i)->m_bIsVisible = false;
            if (m_pTBEMData) m_pTBEMData->m_bIsVisible = true;

        }
    }
    if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
    {
        for( int i=0;i<g_360PolarStore.size();i++)
        {
            g_360PolarStore.at(i)->m_bIsVisible = false;
            if (m_pCur360Polar) m_pCur360Polar->m_bIsVisible = true;
        }
    }

    CreateRotorCurves();
    CreatePowerCurves();
    CreatePolarCurve();
    SetCurveParams();
}

void QBEM::OnIsolateCurrentBladeCurve()
{


    if (m_bIsolateBladeCurve)
    {
	   g_mainFrame->IsolateCurrentBladeCurveAct->setChecked(false);
       m_bIsolateBladeCurve = false;
    }
    else
    {
	   g_mainFrame->IsolateCurrentBladeCurveAct->setChecked(true);
       m_bIsolateBladeCurve = true;
    }

	//m_pCurGraph->SetAuto(true);

    CreateRotorCurves();
    CreatePowerCurves();

}

void QBEM::OnCompareIsolatedBladeCurves()
{


    if (m_bCompareBladeCurve)
    {
	   g_mainFrame->CompareCurrentBladeCurveAct->setChecked(false);
       m_bCompareBladeCurve = false;
    }
    else
    {
	   g_mainFrame->CompareCurrentBladeCurveAct->setChecked(true);
       m_bCompareBladeCurve = true;
    }

	//m_pCurGraph->SetAuto(true);

    CreateRotorCurves();
    CreatePowerCurves();
}

void QBEM::OnDeleteCharSim()
{
    if(m_pCBEMData)
    {
               g_cbemdataStore.remove(m_pCBEMData);
               m_pCBEMData = NULL;
               UpdateBlades();
               CheckButtons();
    }
}

void QBEM::OnDeleteRotorSim()
{
    if(m_pBEMData)
    {
               g_bemdataStore.remove(m_pBEMData);
               m_pBEMData = NULL;
               m_pBData = NULL;
               UpdateBlades();
               CheckButtons();
    }

}


void QBEM::OnDeleteTurbineSim()
{
    if(m_pTBEMData)
    {

               g_tbemdataStore.remove(m_pTBEMData);
               m_pTurbineBData = NULL;
               m_pTBEMData = NULL;
               m_pBData = NULL;
               UpdateTurbines();
               CheckButtons();
    }

}

// new code JW //
void QBEM::OnSetWeibull()
{
	double A,k,PMk,PMA,V,f,energy = 0;

	// get A and k value
	k=m_pctrlWk->value();
	A=m_pctrlWA->value();

	//get plus minus range of k and A
	PMA=m_pctrlPMA->value();
	PMk=m_pctrlPMk->value();

	// create A and k evaluation range
	m_A.clear();
	m_k.clear();

	for (int i=0; i<101; i++)
	{
		//m_A.append(A+3/50.0*(i-50));
		//m_k.append(k+3/50.0*(i-50));
		m_A.append(A+PMA/50.0*(i-50));
		m_k.append(k+PMk/50.0*(i-50));
	}


	// if turbines are stored
	if (m_pTBEMData)
	{
		// for all turbine simulations
		for (int i=0; i< g_tbemdataStore.size(); i++)
		{
			g_tbemdataStore.at(i)->m_Weibull.clear();
			// reset selected A and k
			A = m_A[50];
			k = m_k[50];

			// calc weibull probability for selected A and k
			for (int j=0;j<g_tbemdataStore.at(i)->m_V.size();j++)
			{
			   V=g_tbemdataStore.at(i)->m_V.at(j);

			   f=k/A*pow((V/A),(k-1))*exp(-pow((V/A),k));
			   g_tbemdataStore.at(i)->m_Weibull.append(f);
			}


			// calc aep for A range
			g_tbemdataStore.at(i)->m_aepA.clear();

			for (int j=0; j<m_A.size(); j++)
			{
				A = m_A[j];
				k = m_k[50];

				// sum up annual yield for temp A
				energy=0;
				for (int m=0;m<g_tbemdataStore.at(i)->m_V.size()-1;m++)
				{
					f = exp(-pow(g_tbemdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tbemdataStore.at(i)->m_V.at(m+1)/A,k));

					energy = energy + 0.5*(g_tbemdataStore.at(i)->m_P.at(m)+g_tbemdataStore.at(i)->m_P.at(m+1))*f*8760;
				}
				g_tbemdataStore.at(i)->m_aepA.append(energy);
			}

			// calc aep for k range
			g_tbemdataStore.at(i)->m_aepk.clear();

			for (int j=0; j<m_k.size(); j++)
			{
				A = m_A[50];
				k = m_k[j];

				// sum up annual yield for temp k
				energy=0;
				for (int m=0;m<g_tbemdataStore.at(i)->m_V.size()-1;m++)
				{
					f = exp(-pow(g_tbemdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tbemdataStore.at(i)->m_V.at(m+1)/A,k));

					energy = energy + 0.5*(g_tbemdataStore.at(i)->m_P.at(m)+g_tbemdataStore.at(i)->m_P.at(m+1))*f*8760;
				}
				g_tbemdataStore.at(i)->m_aepk.append(energy);
			}


			/*
			// calc aep for A and k range
			g_tbemdataStore.at(i)->m_aep.clear();
			QList <double> aep;
			for (int j=0; j<m_A.size(); j++)
			{
				A = m_A[j];
				aep.clear();

				for (int l=0; l<m_k.size();l++)
				{

					k = m_k[l];

					// sum up annual yield for temp k and A
					energy=0;
					for (int m=0;m<g_tbemdataStore.at(i)->m_V.size()-1;m++)
					{
						f = exp(-pow(g_tbemdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tbemdataStore.at(i)->m_V.at(m+1)/A,k));

						energy = energy + 0.5*(g_tbemdataStore.at(i)->m_P.at(m)+g_tbemdataStore.at(i)->m_P.at(m+1))*f*8760;
					}
					//g_tbemdataStore.at(i)->m_aep[j][l]=energy;
					aep.append(energy);
				}

				g_tbemdataStore.at(i)->m_aep.append(aep);
			}
			*/
		}

		QString yield,str;
		GetPowerUnit(str, g_mainFrame->m_PowerUnit);
		//yield.sprintf("%.0f",m_pTBEMData->m_aep[50][50]*g_mainFrame->m_WtoUnit);
		yield.sprintf("%.0f",m_pTBEMData->m_aepk.at(50)*g_mainFrame->m_WtoUnit);
		m_pctrlYield->setText(yield+" "+str+"h");

		CreatePowerCurves();
	}
}
// end new code JW //


void QBEM::OnSetCD90(double val)
{
    m_CD90 = val;

    if (m_bNew360Polar)
    {
    Compute360Polar();
    CreateSinglePolarCurve();
    }

}

////// new code JW //////////
void QBEM::OnSetAR(double val)
{
    m_AR = val;

    if (m_bNew360Polar)
    {
    ComputeViterna360Polar();
    CreateSinglePolarCurve();
    }
}
////// end new code JW //////

void QBEM::OnDiscardBlade()
{
	QMessageBox msgBox;
	msgBox.setText(tr("If you cancel the blade design will be lost!"));
	msgBox.setInformativeText(tr("You want to proceed?"));
	msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
	QPushButton *backButton = msgBox.addButton(tr("Back to Design"), QMessageBox::ActionRole);
	msgBox.exec();

    if (msgBox.clickedButton() == backButton) return;

    for (int i=0; i<m_pBlade->m_StrutList.size();i++) g_StrutStore.remove(m_pBlade->m_StrutList.at(i));

    m_pBlade->deleteLater();

    if (g_mainFrame->m_iApp == BEM) SimpleAdvanced->setCurrentIndex(0); // return to simpleview

    ObjectIsEdited = false;
    m_WingEdited = false;

    if (g_mainFrame->m_iApp == BEM) m_pBlade = m_BEMToolBar->m_rotorComboBox->currentObject();
    if (g_mainFrame->m_iApp == DMS) m_pBlade = g_qdms->m_DMSToolBar->getRotorBox()->currentObject();

    UpdateBlades();
    OnCenterScene();
    EnableAllButtons();
    CheckButtons();
}

void QBEM::OnDiscardTurbine()
{
	QMessageBox msgBox;
	msgBox.setText(tr("If you cancel the turbine parameters will be lost!"));
	msgBox.setInformativeText(tr("You want to proceed?"));
	msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
	QPushButton *backButton = msgBox.addButton(tr("Back to Design"), QMessageBox::ActionRole);


	msgBox.exec();

	if (msgBox.clickedButton() == backButton)
	{
	return;
	}

    m_TurbineEdited = false;

    EnableAllButtons();
    UpdateTurbines();
    CheckButtons();
}

void QBEM::OnDiscard360Polar()
{
    delete m_pCur360Polar;

    m_pCur360Polar = NULL;

    EnableAllButtons();

    Update360Polars();

	m_bNew360Polar = false;
    m_bDecompose = false;


	CheckButtons();
}

void QBEM::OnScaleBlade()
{


    BladeScaleDlg dlg;
	dlg.move(g_mainFrame->m_DlgPos);
    dlg.InitDialog(m_pBlade->m_TPos[m_pBlade->m_NPanel], m_pBlade->m_TChord[0], m_pBlade->m_TTwist[0]);

    if(QDialog::Accepted == dlg.exec())
    {
            if (dlg.m_bSpan || dlg.m_bChord || dlg.m_bTwist)
            {
                    if(m_pBlade)
                    {
                            if(dlg.m_bSpan)  m_pBlade->ScaleSpan(dlg.m_NewSpan*2);
                            if(dlg.m_bChord) m_pBlade->ScaleChord(dlg.m_NewChord);
                            if(dlg.m_bTwist) m_pBlade->ScaleTwist(dlg.m_NewTwist);
                    }

            }

            m_pctrlHubRadius->setValue(m_pBlade->m_TPos[0]*g_mainFrame->m_mtoUnit);
            FillDataTable();
            m_bResetglGeom = true;
            m_bResetglSectionHighlight = true;
            ComputeGeometry();
            UpdateView();
    }


}


/////////////// new code JW //////////////
void QBEM::OnStallModel()
{
    m_bStallModel = m_pctrlStallModelVit->isChecked();
}
/////////////// end new code JW //////////


void QBEM::OnLoadCylindricFoil()
{
	QXDirect * pXDirect = (QXDirect*)s_pXDirect;
	CircularFoilDlg *pCircularFoilDlg = new CircularFoilDlg;
	
	if (pCircularFoilDlg->exec())
	{
		CFoil *pFoil = new CFoil();
		int num = 100;
		double ang = 2*PI / num;
		pFoil->nb=0;
		
		for (int i=0;i<=num;i++)
		{
			pFoil->xb[i] = 1-0.5*(sin(ang*i-PI/2)+1);
			pFoil->yb[i] = 0.5*cos(ang*i-PI/2);
			pFoil->nb++;
		}
		
		pFoil->setName(pCircularFoilDlg->m_CircleName->text());
		memcpy(pFoil->x, pFoil->xb, sizeof(pFoil->xb));
		memcpy(pFoil->y, pFoil->yb, sizeof(pFoil->yb));
		pFoil->n = pFoil->nb;
		pFoil->m_FoilColor = g_mainFrame->GetColor(0);
		pFoil->InitFoil();
		pFoil->m_bisCircular = true;
		
        if (g_mainFrame->StoreFoil(pFoil))
		{
			g_pCurFoil  = pFoil;
			pXDirect->m_pCurPolar = NULL;
			pXDirect->m_pCurOpp   = NULL;
			g_pCurFoil = pXDirect->SetFoil(pFoil);
			pXDirect->SetPolar();
			QAFoil *pAFoil= (QAFoil*)s_pAFoil;
			pAFoil->SelectFoil(pFoil);
			g_mainFrame->SetSaveState(false);
			pXDirect->SetControls();
			
			CPolar *xPolar = new CPolar(pCircularFoilDlg->m_CircleName->text() +" Placeholder", pFoil);
			
			g_polarStore.add(xPolar);
			
			C360Polar* pPolar = new C360Polar(QString("CD = " + QString::number(pCircularFoilDlg->m_CircularDrag->getValue()) +
													  "      360 Polar"), xPolar);
			
            pPolar->slope = 0;
            pPolar->alpha_zero = 0;
			pPolar->m_airfoil = g_foilStore.getObjectByNameOnly(pCircularFoilDlg->m_CircleName->text());
			
			for (int i=-180;i<=180;i++)
			{
				pPolar->m_Alpha.append(i);
				pPolar->m_Cd.append(pCircularFoilDlg->m_CircularDrag->getValue());
				pPolar->m_Cl.append(0);
                pPolar->m_Cl_att.append(0);
                pPolar->m_Cl_sep.append(0);
                pPolar->m_fst.append(0);
                pPolar->m_Cm.append(0);
			}
            pPolar->m_bisDecomposed = true;
			pPolar->m_Color = g_mainFrame->GetColor(11);
			g_360PolarStore.add(pPolar);
			m_pCurFoil = pFoil;
			UpdateFoils();
			pAFoil->FillFoilTable();
		}
	}
}


void QBEM::OnExportBladeGeometry()
{
if (m_pBlade)
    {
        QString FileName, BladeName;
        QString SelectedFilter;
        QFileDialog::Options options;
        int type;

        BladeName = m_pBlade->getName();
        BladeName.replace("/", "_");
        BladeName.replace(" ", "_");

        FileName = QFileDialog::getSaveFileName(this, tr("Export Blade Geometry"), g_mainFrame->m_ExportLastDirName+QDir::separator()+BladeName,
                                                                                        tr("STL File (*.stl);;Coordinates in Text File (*.txt)"),
                                                                                        &SelectedFilter, options);



        if(!FileName.length()) return;



        ExportGeomDlg geomDlg;

        geomDlg.Spanwise->setValue(200);
        geomDlg.Chordwise->setValue(20);
        geomDlg.setModal(true);

        if (!(geomDlg.exec() == QDialog::Accepted)) return;



        int spanwise_res = geomDlg.Spanwise->getValue()-1;
        int num_tri = geomDlg.Chordwise->getValue();

        if (!(num_tri % 2) == 0) num_tri += 1;

        int pos;
        pos = FileName.lastIndexOf("/");
		if(pos>0) g_mainFrame->m_LastDirName = FileName.left(pos);

        pos = FileName.indexOf(".stl");
        if(pos>0) type=1; else type=0;



        QFile XFile(FileName);

        if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

        QTextStream out(&XFile);
        QString strong;

        double x=0, xp;
        CVector Pt, PtNormal, A,B,C,D,BD,AC,N;
        int j,l,n=0;



        static double  xDistrib[500];
//        double xx;
//        double param = 100;// increase to refine L.E. and T.E.
//		for(int i=0; i<=num_tri; i++)
//        {
//				xx = (double)i/(double)(num_tri);
//                xDistrib[i] = (asinh(param*(xx-0.5))/asinh(param/2.)+1.)/2.;
//                qDebug() << xDistrib[i] ;
//        }

        ///sinusoidal spacing
        double  angle_incr, angle = 0, totlength=0;

        for (int l=0;l<int(num_tri);l++)
        {
            angle_incr = PI / (num_tri);
            totlength+=sin(angle);
            angle+=angle_incr;
        }

        angle = 0;
        for(int l=0; l<num_tri; l++)
        {
            x += sin(angle)/totlength;
            angle += angle_incr;
            xDistrib[l]=x;
            qDebug() << xDistrib[l] <<l<<num_tri;
        }
///////////////////


        double bladelength = m_pBlade->m_TPos[m_pBlade->m_NSurfaces]-m_pBlade->m_TPos[0];
        double frac = bladelength / spanwise_res;
        double position = m_pBlade->m_TPos[0];
        double a_frac, b_frac;




        if (type == 1)
        {

        out << "solid " <<m_pBlade->getName() <<"\n";

        // create the trinangles for the blade

        for (int k=0; k<spanwise_res; k++)
        {
                        for (j=0;j<m_pBlade->m_NSurfaces;j++){
                            if (position >= m_pBlade->m_TPos[j] && position < m_pBlade->m_TPos[j+1]){
                                a_frac = (position-m_pBlade->m_TPos[j])/(m_pBlade->m_TPos[j+1]-m_pBlade->m_TPos[j]);
                                for (n = j;n < m_pBlade->m_NSurfaces;n++){
                                    if (position + frac > m_pBlade->m_TPos[n] && position + frac <= m_pBlade->m_TPos[n+1]+0.00000001 ){
                                       b_frac = (position + frac - m_pBlade->m_TPos[n])/(m_pBlade->m_TPos[n+1]-m_pBlade->m_TPos[n]);
                                       break;
                                    }
                                }
                                break;
                            }
                        }

						for (l=0; l<num_tri-1; l++)
                        {
                                //top surface
                                x= xDistrib[l];
								xp = xDistrib[l+1];

                                m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(PtNormal.x,14,'e',5).arg(PtNormal.y,14,'e',5).arg(PtNormal.z,14,'e',5);
                                out << "facet normal "+strong;

                                out << "outer loop\n";

                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[n].GetPoint(xp,xp,b_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                out << "endloop\nendfacet\n";


                                m_pBlade->m_Surface[n].GetPoint(xp,xp,b_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(PtNormal.x,14,'e',5).arg(PtNormal.y,14,'e',5).arg(PtNormal.z,14,'e',5);
                                out << "facet normal "+strong;

                                out << "outer loop\n";

                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[j].GetPoint(xp,xp,a_frac,Pt, PtNormal,1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                out << "endloop\nendfacet\n";

                        }

                        for (l=0; l<num_tri-1; l++)
                        {
                        //bottom surface
                                x= xDistrib[l];
                                xp = xDistrib[l+1];

                                m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(PtNormal.x,14,'e',5).arg(PtNormal.y,14,'e',5).arg(PtNormal.z,14,'e',5);
                                out << "facet normal "+strong;

                                out << "outer loop\n";

                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[n].GetPoint(xp,xp,b_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                out << "endloop\nendfacet\n";


                                m_pBlade->m_Surface[n].GetPoint(xp,xp,b_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(PtNormal.x,14,'e',5).arg(PtNormal.y,14,'e',5).arg(PtNormal.z,14,'e',5);
                                out << "facet normal "+strong;

                                out << "outer loop\n";

                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[j].GetPoint(xp,xp,a_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,-1);
                                strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                                out << "vertex " + strong;

                                out << "endloop\nendfacet\n";

                        }

                        //////now check for a gap at the TE, if there is one close it!

                        x=xDistrib[num_tri-1];

                        m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,A, PtNormal,1);
                        m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,B, PtNormal,-1);
                        m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,C, PtNormal,1);
                        m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,D, PtNormal,-1);


                        if (CVector(A-B).VAbs()>0.001 || CVector(C-D).VAbs()>0.001)
                        {

                            N = CVector(A-D)*CVector(B-C);
                            N.Normalize();

                            strong = QString("%1 %2 %3\n").arg(N.x,14,'e',5).arg(N.y,14,'e',5).arg(N.z,14,'e',5);
                            out << "facet normal "+strong;

                            out << "outer loop\n";

                            strong = QString("%1 %2 %3\n").arg(A.x+1,14,'e',5).arg(A.y,14,'e',5).arg(A.z+1,14,'e',5);
                            out << "vertex " + strong;

                            m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,-1);
                            strong = QString("%1 %2 %3\n").arg(B.x+1,14,'e',5).arg(B.y,14,'e',5).arg(B.z+1,14,'e',5);
                            out << "vertex " + strong;

                            m_pBlade->m_Surface[j].GetPoint(xp,xp,a_frac,Pt, PtNormal,-1);
                            strong = QString("%1 %2 %3\n").arg(D.x+1,14,'e',5).arg(D.y,14,'e',5).arg(D.z+1,14,'e',5);
                            out << "vertex " + strong;

                            out << "endloop\nendfacet\n";


                            strong = QString("%1 %2 %3\n").arg(N.x,14,'e',5).arg(N.y,14,'e',5).arg(N.z,14,'e',5);

                            out << "facet normal "+strong;
                            out << "outer loop\n";

                            strong = QString("%1 %2 %3\n").arg(A.x+1,14,'e',5).arg(A.y,14,'e',5).arg(A.z+1,14,'e',5);
                            out << "vertex " + strong;

                            m_pBlade->m_Surface[j].GetPoint(x,x,a_frac,Pt, PtNormal,-1);
                            strong = QString("%1 %2 %3\n").arg(D.x+1,14,'e',5).arg(D.y,14,'e',5).arg(D.z+1,14,'e',5);
                            out << "vertex " + strong;

                            m_pBlade->m_Surface[j].GetPoint(xp,xp,a_frac,Pt, PtNormal,-1);
                            strong = QString("%1 %2 %3\n").arg(C.x+1,14,'e',5).arg(C.y,14,'e',5).arg(C.z+1,14,'e',5);
                            out << "vertex " + strong;

                            out << "endloop\nendfacet\n";

                        }
                        position += frac;

        }
                    ///////hub surface triangles
                    j=0;

                    m_pBlade->m_Surface[j].GetPanel(0, 0, BOTSURFACE);
                    C. Copy(m_pBlade->m_Surface[0].LA);
                    D. Copy(m_pBlade->m_Surface[0].TA);
                    m_pBlade->m_Surface[j].GetPanel(0, 0, TOPSURFACE);
                    A. Copy(m_pBlade->m_Surface[0].TA);
                    B. Copy(m_pBlade->m_Surface[0].LA);

                    BD = D-B;
                    AC = C-A;
                    N  = AC*BD;
                    N.Normalize();

                    for (l=0; l<num_tri-1; l++)
                    {

                        x= xDistrib[l];
                        xp = xDistrib[l+1];

                        m_pBlade->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(N.x,14,'e',5).arg(N.y,14,'e',5).arg(N.z,14,'e',5);

                        out << "facet normal "+strong;
                        out << "outer loop\n";

                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,0.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        out << "endloop\nendfacet\n";


                        m_pBlade->m_Surface[j].GetPoint(x,x,0.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(N.x,14,'e',5).arg(N.y,14,'e',5).arg(N.z,14,'e',5);
                        out << "facet normal "+strong;

                        out << "outer loop\n";
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,0.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,0.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        out << "endloop\nendfacet\n";

                    }

                    ///////right tip surface triangles

                    j= m_pBlade->m_NSurfaces-1;

                    for (l=0; l<num_tri-1; l++)
                    {
                        x= xDistrib[l];
                        xp = xDistrib[l+1];

                        m_pBlade->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(-N.x,14,'e',5).arg(-N.y,14,'e',5).arg(-N.z,14,'e',5);

                        out << "facet normal "+strong;
                        out << "outer loop\n";

                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,1.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        out << "endloop\nendfacet\n";



                        m_pBlade->m_Surface[j].GetPoint(x,x,1.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(N.x,14,'e',5).arg(N.y,14,'e',5).arg(N.z,14,'e',5);

                        out << "facet normal "+strong;
                        out << "outer loop\n";

                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,1.0,Pt, PtNormal,1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        m_pBlade->m_Surface[j].GetPoint(xp,xp,1.0,Pt, PtNormal,-1);
                        strong = QString("%1 %2 %3\n").arg(Pt.x+1,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z+1,14,'e',5);
                        out << "vertex " + strong;

                        out << "endloop\nendfacet\n";
                    }


        out << "endsolid "<<m_pBlade->getName() <<"\n";;

        }
        else
        {
            int kk;

            position = m_pBlade->m_TPos[0];

            out << m_pBlade->getName() +"\n";
            out << "             x                       y                       z\n";

            for (int k=0; k<spanwise_res; k++)
            {
                            for (kk=0;kk<m_pBlade->m_NSurfaces;kk++){
                                if (position >= m_pBlade->m_TPos[kk] && position < m_pBlade->m_TPos[kk+1]){
                                    a_frac = (position-m_pBlade->m_TPos[kk])/(m_pBlade->m_TPos[kk+1]-m_pBlade->m_TPos[kk]);
                                    for (n = kk;n < m_pBlade->m_NSurfaces;n++){
                                        if (position + frac > m_pBlade->m_TPos[n] && position + frac <= m_pBlade->m_TPos[n+1]+0.00000001 ){
                                           b_frac = (position + frac - m_pBlade->m_TPos[n])/(m_pBlade->m_TPos[n+1]-m_pBlade->m_TPos[n]);
                                           break;
                                        }
                                    }
                                    break;
                                }
                            }

                for (int j=num_tri;j>=0;j--)
                {
                x= xDistrib[j];
                m_pBlade->m_Surface[kk].GetPoint(x,x,a_frac,Pt, PtNormal,1);

                strong = QString("%1          %2          %3\n").arg(Pt.x,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z,14,'e',5);

                out << strong;
                }

                for (int j=0;j<=num_tri;j++)
                {
                x= xDistrib[j];
                m_pBlade->m_Surface[kk].GetPoint(x,x,a_frac,Pt, PtNormal,-1);

                strong = QString("%1          %2          %3\n").arg(Pt.x,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z,14,'e',5);

                out << strong;
                }

                out << "\n";

                if (n == m_pBlade->m_NSurfaces-1)
                {
                    for (int j=num_tri;j>=0;j--)
                    {
                    x= xDistrib[j];
                    m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,Pt, PtNormal,1);

                    strong = QString("%1          %2          %3\n").arg(Pt.x,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z,14,'e',5);

                    out << strong;
                    }
                    for (int j=0;j<=num_tri;j++)
                    {
                    x= xDistrib[j];
                    m_pBlade->m_Surface[n].GetPoint(x,x,b_frac,Pt, PtNormal,-1);

                    strong = QString("%1          %2          %3\n").arg(Pt.x,14,'e',5).arg(Pt.y,14,'e',5).arg(Pt.z,14,'e',5);

                    out << strong;
                    }
                }

                position += frac;

        }
        }

        XFile.close();
    }

}

void QBEM::OnEditCur360Polar()
{
    if (!m_pCur360Polar) return;
	
    if (g_mainFrame->m_iView != POLARVIEW) On360View();
	
    C360Polar *newPolar = new C360Polar(m_pCur360Polar->getName(), m_pCur360Polar->getParent());

    newPolar->alpha_zero = m_pCur360Polar->alpha_zero;
    newPolar->slope = m_pCur360Polar->slope;
    newPolar->CD90 = m_pCur360Polar->CD90;
    newPolar->reynolds = m_pCur360Polar->reynolds;
    newPolar->posalphamax = m_pCur360Polar->posalphamax;
    newPolar->m_bisDecomposed = m_pCur360Polar->m_bisDecomposed;
    newPolar->CLzero = m_pCur360Polar->CLzero;
    newPolar->m_airfoil = m_pCur360Polar->m_airfoil;

	for (int i=0; i<m_pCur360Polar->m_Alpha.size();i++)
	{
        newPolar->m_Alpha.append(m_pCur360Polar->m_Alpha.at(i));
        newPolar->m_Cl.append(m_pCur360Polar->m_Cl.at(i));
        newPolar->m_Cd.append(m_pCur360Polar->m_Cd.at(i));
        newPolar->m_Cl_att.append(m_pCur360Polar->m_Cl_att.at(i));
        newPolar->m_Cl_sep.append(m_pCur360Polar->m_Cl_sep.at(i));
        newPolar->m_fst.append(m_pCur360Polar->m_fst.at(i));
    }

    C360Polar *oldPolar = m_pCur360Polar;

    m_pCur360Polar = newPolar;
	
	Edit360PolarDlg dlg;
    dlg.m_pPolar = m_pCur360Polar;
	dlg.m_pBEM = this;
	dlg.InitDialog();
	
    m_pCur360Polar->m_bShowPoints = true;
	
    CreateSinglePolarCurve(false);
	UpdateView();
	
	if(dlg.exec() == QDialog::Accepted)
    {
        if (g_360PolarStore.add(m_pCur360Polar)){
        g_mainFrame->SetSaveState(false);
        m_pCur360Polar->m_bShowPoints = false;
        }
        else{
            m_pCur360Polar = oldPolar;
        }
	}
	else
	{
        m_pCur360Polar = oldPolar;
        delete newPolar;
	}
    CreatePolarCurve();
	UpdateView();
}

void QBEM::OnHideWidgets()
{
    QDMS *pDMS = (QDMS *) g_mainFrame->m_pDMS;
    QBEM *pBEM = (QBEM *) g_mainFrame->m_pBEM;


    if (m_bHideWidgets)
    {
        g_mainFrame->HideWidgetsAct->setChecked(false);
        pBEM->m_bHideWidgets = false;
        pDMS->m_bHideWidgets = false;
    }
    else
    {
        g_mainFrame->HideWidgetsAct->setChecked(true);
        pBEM->m_bHideWidgets = true;
        pDMS->m_bHideWidgets = true;

    }

    CheckButtons();

}

void QBEM::OnSingleGraphs()
{


    if (m_bSingleGraphs)
    {
		g_mainFrame->SingleGraphAct->setChecked(false);


        m_bSingleGraphs = false;
    }
    else
    {
		g_mainFrame->SingleGraphAct->setChecked(true);


        m_bSingleGraphs = true;
    }

    CheckButtons();
    UpdateView();

}





void QBEM::OnChangeCoordinates()
{
        m_pctrlBladeCoordinates2->setChecked(m_pctrlBladeCoordinates->isChecked());
        if (m_pctrlBladeCoordinates->isChecked())
        {
            m_pctrlHubRadiusLabel->setText(tr("Blade Hub Radius"));
            m_bAbsoluteBlade=false;
        }
        else
        {
            m_pctrlHubRadiusLabel->setText(tr("Innermost Station"));
            m_bAbsoluteBlade = true;
        }

        FillDataTable();


        UpdateView();
}


void QBEM::OnCellChanged()
{
    if (m_WingEdited)
    {
        ReadParams();
    }
}

void QBEM::OnDeleteSection()
{
        if(m_iSection <0 || m_iSection>m_pBlade->m_NPanel) return;

        if(m_iSection==0)
        {
                QMessageBox::warning(this, tr("Warning"),tr("The first section cannot be deleted"));
                return;
        }

        int k, size;

        size = m_pWingModel->rowCount();
        if(size<=2){
            QMessageBox::warning(this, tr("Warning"),tr("At least two sections must remain"));
            return;
        }

        m_pBlade->m_Airfoils.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Polar.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Range.append("-----");// add new dummy station to temporarily store values


        for (k=m_iSection; k<size; k++)
        {
                m_pBlade->m_TRelPos[k]      = m_pBlade->m_TRelPos[k+1];
                m_pBlade->m_TChord[k]    = m_pBlade->m_TChord[k+1];
                m_pBlade->m_TOffsetX[k]   = m_pBlade->m_TOffsetX[k+1];
                m_pBlade->m_TTwist[k]     = m_pBlade->m_TTwist[k+1];
                m_pBlade->m_TDihedral[k]  = m_pBlade->m_TDihedral[k+1];
                m_pBlade->m_NXPanels[k]   = m_pBlade->m_NXPanels[k+1];
                m_pBlade->m_NYPanels[k]   = m_pBlade->m_NYPanels[k+1];
                m_pBlade->m_Airfoils[k]      = m_pBlade->m_Airfoils[k+1];
                m_pBlade->m_Polar[k]      = m_pBlade->m_Polar[k+1];
                m_pBlade->m_Range[k]      = m_pBlade->m_Range[k+1];

                m_pBlade->m_TPos[k]      = m_pBlade->m_TPos[k+1];
                m_pBlade->m_TPAxisX[k] =   m_pBlade->m_TPAxisX[k+1];
                m_pBlade->m_TOffsetZ[k] =   m_pBlade->m_TOffsetZ[k+1];
                m_pBlade->m_TFoilPAxisX[k] = m_pBlade->m_TFoilPAxisX[k+1];
                m_pBlade->m_TFoilPAxisZ[k] = m_pBlade->m_TFoilPAxisZ[k+1];
        }
        m_pBlade->m_NPanel--;

        m_pBlade->m_Airfoils.removeLast(); //delete the 2 last stations, now one less than before section was deleted
        m_pBlade->m_Airfoils.removeLast();

        m_pBlade->m_Polar.removeLast(); //delete the 2 last stations, now one less than before section was deleted
        m_pBlade->m_Polar.removeLast();

        m_pBlade->m_Range.removeLast(); //delete the 2 last stations, now one less than before section was deleted
        m_pBlade->m_Range.removeLast();

        m_iSection--;

        FillDataTable();
        ComputeGeometry();
        ReadParams();
}

void QBEM::OnInsertBefore()
{
        if(m_iSection <0 || m_iSection>m_pBlade->m_NPanel) return;

        if (m_pBlade->m_NPanel==VLMMAXMATSIZE-1)
        {
                QMessageBox::warning(this, tr("Warning"), tr("The maximum number of panels has been reached"));
                return;
        }
        if(m_iSection<=0)
        {
                QMessageBox::warning(this, tr("Warning"), tr("No insertion possible before the first section"));
                return;
        }
        int k,n,ny;

        m_pBlade->m_Airfoils.append("-----"); // add new dummy station to temporarily store values
        m_pBlade->m_Polar.append("-----"); // add new dummy station to temporarily store values
        m_pBlade->m_Range.append("-----"); // add new dummy station to temporarily store values


        n  = m_iSection;
        for (k=m_pBlade->m_NPanel; k>=n; k--)
        {
                m_pBlade->m_TRelPos[k+1]      = m_pBlade->m_TRelPos[k];
                m_pBlade->m_TChord[k+1]    = m_pBlade->m_TChord[k];
                m_pBlade->m_TOffsetX[k+1]   = m_pBlade->m_TOffsetX[k];
                m_pBlade->m_TTwist[k+1]     = m_pBlade->m_TTwist[k];
                m_pBlade->m_TDihedral[k+1]  = m_pBlade->m_TDihedral[k];
                m_pBlade->m_NXPanels[k+1]   = m_pBlade->m_NXPanels[k];
                m_pBlade->m_NYPanels[k+1]   = m_pBlade->m_NYPanels[k];
                m_pBlade->m_Airfoils[k+1]      = m_pBlade->m_Airfoils[k];
                m_pBlade->m_Polar[k+1]      = m_pBlade->m_Polar[k];
                m_pBlade->m_Range[k+1]      = m_pBlade->m_Range[k];

                m_pBlade->m_TPos[k+1]      = m_pBlade->m_TPos[k];
                m_pBlade->m_TPAxisX[k+1] =   m_pBlade->m_TPAxisX[k];
                m_pBlade->m_TOffsetZ[k+1] =   m_pBlade->m_TOffsetZ[k];
                m_pBlade->m_TFoilPAxisX[k+1] = m_pBlade->m_TFoilPAxisX[k];
                m_pBlade->m_TFoilPAxisZ[k+1] = m_pBlade->m_TFoilPAxisZ[k];
        }

        ny = m_pBlade->m_NYPanels[n-1];
        m_pBlade->m_TRelPos[n]    = (m_pBlade->m_TRelPos[n+1]    + m_pBlade->m_TRelPos[n-1])   /2.0;
        m_pBlade->m_TChord[n]  = (m_pBlade->m_TChord[n+1]  + m_pBlade->m_TChord[n-1]) /2.0;
        m_pBlade->m_TOffsetX[n] = (m_pBlade->m_TOffsetX[n+1] + m_pBlade->m_TOffsetX[n-1])/2.0;
        m_pBlade->m_TTwist[n] = (m_pBlade->m_TTwist[n+1] + m_pBlade->m_TTwist[n-1])/2.0;


        m_pBlade->m_TPos[n]    = (m_pBlade->m_TPos[n+1]    + m_pBlade->m_TPos[n-1])   /2.0;
        m_pBlade->m_TPAxisX[n] = (m_pBlade->m_TPAxisX[n+1]+m_pBlade->m_TPAxisX[n-1]) /2.0;
        m_pBlade->m_TOffsetZ[n] = (m_pBlade->m_TOffsetZ[n+1]+m_pBlade->m_TOffsetZ[n-1]) / 2.0;
        m_pBlade->m_TFoilPAxisX[n] = (m_pBlade->m_TFoilPAxisX[n+1]+ m_pBlade->m_TFoilPAxisX[n-1]) /2.0;
        m_pBlade->m_TFoilPAxisZ[n] = (m_pBlade->m_TFoilPAxisZ[n+1] + m_pBlade->m_TFoilPAxisZ[n-1]) / 2.0;

        m_pBlade->m_NXPanels[n]   = m_pBlade->m_NXPanels[n-1];
        m_pBlade->m_NYPanels[n]   = (int)(ny/2);
        m_pBlade->m_NYPanels[n-1] = ny-m_pBlade->m_NYPanels[n];
        if(m_pBlade->m_NYPanels[n]==0)   m_pBlade->m_NYPanels[n]++;
        if(m_pBlade->m_NYPanels[n-1]==0) m_pBlade->m_NYPanels[n-1]++;

        m_pBlade->m_NPanel++;

        m_iSection++;

        FillDataTable();
        ComputeGeometry();

        m_bResetglSectionHighlight = true;
        ReadParams();
}

void QBEM::OnInsertAfter()
{
        if(m_iSection <0 || m_iSection>m_pBlade->m_NPanel) return;
        if (m_pBlade->m_NPanel==VLMMAXMATSIZE-1)
        {
                QMessageBox::warning(this, tr("Warning"), tr("The maximum number of panels has been reached"));
                return;
        }
        int k,n,ny;

        n  = m_iSection;

        if(n<0) n=m_pBlade->m_NPanel;
        ny = m_pBlade->m_NYPanels[n];

        m_pBlade->m_Airfoils.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Polar.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Range.append("-----");// add new dummy station to temporarily store values



        for (k=m_pBlade->m_NPanel+1; k>n; k--)
        {
                m_pBlade->m_TRelPos[k]       = m_pBlade->m_TRelPos[k-1];
                m_pBlade->m_TChord[k]     = m_pBlade->m_TChord[k-1];
                m_pBlade->m_TOffsetX[k]    = m_pBlade->m_TOffsetX[k-1];
                m_pBlade->m_TTwist[k]     = m_pBlade->m_TTwist[k-1];
                m_pBlade->m_TDihedral[k]  = m_pBlade->m_TDihedral[k-1];
                m_pBlade->m_NXPanels[k]   = m_pBlade->m_NXPanels[k-1];
                m_pBlade->m_NYPanels[k]   = m_pBlade->m_NYPanels[k-1];
                m_pBlade->m_Airfoils[k]      = m_pBlade->m_Airfoils[k-1];
                m_pBlade->m_Range[k]      = m_pBlade->m_Range[k-1];
                m_pBlade->m_Polar[k]      = m_pBlade->m_Polar[k-1];
                m_pBlade->m_TPos[k]      = m_pBlade->m_TPos[k-1];
                m_pBlade->m_TPAxisX[k] =   m_pBlade->m_TPAxisX[k-1];
                m_pBlade->m_TOffsetZ[k] =   m_pBlade->m_TOffsetZ[k-1];
                m_pBlade->m_TFoilPAxisX[k] = m_pBlade->m_TFoilPAxisX[k-1];
                m_pBlade->m_TFoilPAxisZ[k] = m_pBlade->m_TFoilPAxisZ[k-1];
        }

        if(n<m_pBlade->m_NPanel)
        {
                m_pBlade->m_TRelPos[n+1]    = (m_pBlade->m_TRelPos[n]    + m_pBlade->m_TRelPos[n+2])   /2.0;
                m_pBlade->m_TChord[n+1]  = (m_pBlade->m_TChord[n]  + m_pBlade->m_TChord[n+2]) /2.0;
                m_pBlade->m_TOffsetX[n+1] = (m_pBlade->m_TOffsetX[n] + m_pBlade->m_TOffsetX[n+2])/2.0;
                m_pBlade->m_TTwist[n+1] = (m_pBlade->m_TTwist[n] + m_pBlade->m_TTwist[n+2])/2.0;


                m_pBlade->m_TPos[n+1]    = (m_pBlade->m_TPos[n]    + m_pBlade->m_TPos[n+2])   /2.0;
                m_pBlade->m_TPAxisX[n+1] = (m_pBlade->m_TPAxisX[n]+m_pBlade->m_TPAxisX[n+2]) /2.0;
                m_pBlade->m_TOffsetZ[n+1] = (m_pBlade->m_TOffsetZ[n]+m_pBlade->m_TOffsetZ[n+2]) / 2.0;
                m_pBlade->m_TFoilPAxisX[n+1] = (m_pBlade->m_TFoilPAxisX[n]+ m_pBlade->m_TFoilPAxisX[n+2]) /2.0;
                m_pBlade->m_TFoilPAxisZ[n+1] = (m_pBlade->m_TFoilPAxisZ[n] + m_pBlade->m_TFoilPAxisZ[n+2]) / 2.0;
        }
        else
        {
                m_pBlade->m_TRelPos[n+1]     = m_pBlade->m_TRelPos[n+1]*1.1;
                m_pBlade->m_TChord[n+1]   = m_pBlade->m_TChord[n+1]/1.1;
                m_pBlade->m_TOffsetX[n+1]  = 0;//m_pBlade->m_TOffsetX[n+1] + m_pBlade->m_TChord[n] - m_pBlade->m_TChord[n+1] ;
                m_pBlade->m_TTwist[n+1]     = m_pBlade->m_TTwist[n];

        }

        m_pBlade->m_TDihedral[n+1]  = m_pBlade->m_TDihedral[n];
        m_pBlade->m_NXPanels[n+1]   = m_pBlade->m_NXPanels[n];
        m_pBlade->m_NYPanels[n+1]   = m_pBlade->m_NYPanels[n];
        m_pBlade->m_Airfoils[n+1]      = m_pBlade->m_Airfoils[n];
        m_pBlade->m_Range[n+1]      = m_pBlade->m_Range[n];
        m_pBlade->m_Polar[n+1]      = m_pBlade->m_Polar[n];


        m_pBlade->m_NYPanels[n+1] = qMax(1,(int)(ny/2));
        m_pBlade->m_NYPanels[n]   = qMax(1,ny-m_pBlade->m_NYPanels[n+1]);

        m_pBlade->m_NPanel++;


        FillDataTable();
        ComputeGeometry();
        ReadParams();
}

void QBEM::OnItemClicked(const QModelIndex &index)
{
        if(index.row()>m_pBlade->m_NPanel)
        {
                //the user has filled a cell in the last line
                if(index.row()<VLMMAXMATSIZE-1)
                {
                        //so add an item before reading
                        m_pBlade->m_NPanel++;
                        m_pWingModel->setRowCount(m_pBlade->m_NPanel+2);
                        FillTableRow(m_pBlade->m_NPanel);
                }
        }
        SetCurrentSection(index.row());

        UpdateView();
        CheckWing();



}

void QBEM::OnNewTurbine()
{

        QString str;


        pitchwindspeeds.clear();
        rotwindspeeds.clear();
        pitchangles.clear();
        rotspeeds.clear();


		GetPowerUnit(str,g_mainFrame->m_PowerUnit);
        power1->setText(str);
        power2->setText(str);
		GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
        speed1->setText(str);
        speed2->setText(str);
        speed3->setText(str);


        m_TurbineEdited = true;


        CheckButtons();
        DisableAllButtons();

        m_pctrlStall->setChecked(true);
        m_pctrlFixed->setChecked(true);
        CheckTurbineButtons();

        QString newname("New Turbine");
        for (int i=0;i<g_tdataStore.size();i++){
            if (newname == g_tdataStore.at(i)->getName()){
                newname = makeNameWithHigherNumber(newname);
                i = 0;
            }
        }

        m_pctrlTurbineName->setText(newname);


        CBlade *pWing;
        WingSelection->clear();
        for (int i=0; i < g_rotorStore.size();i++)
        {
            pWing = g_rotorStore.at(i);
            WingSelection->addItem(pWing->getName());
        }


}

void QBEM::OnEditTurbine()
{
QString str;
GetPowerUnit(str,g_mainFrame->m_PowerUnit);
power1->setText(str);
power2->setText(str);
GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
speed1->setText(str);
speed2->setText(str);
speed3->setText(str);



if (!m_pTData) return;

if (g_mainFrame->m_iView != TURBINEVIEW) OnTurbineView();// new JW, old: m_iView
m_TurbineEdited = true;

CheckButtons();
DisableAllButtons();

m_pctrlTurbineName->setText(m_pTData->m_TurbineName);

m_pctrlStall->setChecked(m_pTData->isStall);
m_pctrlPitch->setChecked(m_pTData->isPitch);
m_pctrlFixed->setChecked(m_pTData->isFixed);
m_pctrl2Step->setChecked(m_pTData->is2Step);
m_pctrlVariable->setChecked(m_pTData->isVariable);
m_pctrlPrescribedPitch->setChecked(m_pTData->isPrescribedPitch);
m_pctrlPrescribedRot->setChecked(m_pTData->isPrescribedRot);

m_pctrlCutIn->setValue(m_pTData->CutIn*g_mainFrame->m_mstoUnit);
m_pctrlCutOut->setValue(m_pTData->CutOut*g_mainFrame->m_mstoUnit);
m_pctrlSwitch->setValue(m_pTData->Switch*g_mainFrame->m_mstoUnit);

m_pctrlRot1->setValue(m_pTData->Rot1);
m_pctrlRot2->setValue(m_pTData->Rot2);
m_pctrlLambda->setValue(m_pTData->Lambda0);
m_pctrlGenerator->setValue(m_pTData->Generator*g_mainFrame->m_WtoUnit);
m_pctrlFixedPitch->setValue(m_pTData->FixedPitch);

m_pctrlVariableLosses->setValue(m_pTData->VariableLosses);
m_pctrlFixedLosses->setValue(m_pTData->m_fixedLosses*g_mainFrame->m_WtoUnit);

pitchwindspeeds = m_pTData->pitchwindspeeds;
pitchangles = m_pTData->pitchangles;
rotspeeds = m_pTData->rotspeeds;
rotwindspeeds = m_pTData->rotwindspeeds;



CBlade *pWing;
WingSelection->clear();
for (int i=0; i < g_rotorStore.size();i++)
{
    pWing = g_rotorStore.at(i);
    WingSelection->addItem(pWing->getName());
}

int pos = WingSelection->findText(m_pTData->m_WingName);
WingSelection->setCurrentIndex(pos);

CheckTurbineButtons();


}

void QBEM::OnExport360PolarNREL(){
    if (!m_pCur360Polar) return;

    QString FileName, PolarName;

    PolarName = m_pCur360Polar->getParent()->getParent()->getName()+"_"+m_pCur360Polar->getName();
    PolarName.replace("/", "_");
    PolarName.replace(" ", "_");
    FileName = QFileDialog::getSaveFileName(this, tr("Export 360 Polar to Aerodyn"), g_mainFrame->m_ExportLastDirName+QDir::separator()+PolarName,
                                            tr("Text File (*.dat)"));
    if(!FileName.length()) return;

    QFile XFile(FileName);

    if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

    QTextStream out(&XFile);

    m_pCur360Polar->ExportPolarNREL(out);

    XFile.close();
}

void QBEM::OnExportAll360PolarsNREL()
{
    QString FileName, DirName, PolarName;
    QFile XFile;
    QTextStream out(&XFile);

    //select the directory for output
    DirName = QFileDialog::getExistingDirectory(this,  tr("Export Directory"), g_mainFrame->m_ExportLastDirName);

    C360Polar *pPolar;
    for(int l=0; l<g_360PolarStore.size(); l++)
    {
        pPolar = g_360PolarStore.at(l);

        PolarName = pPolar->getParent()->getParent()->getName()+"_"+pPolar->getName();
        PolarName.replace("/", "_");
        PolarName.replace(" ", "_");

        FileName = DirName + QDir::separator() + PolarName;
        FileName += ".dat";

        XFile.setFileName(FileName);
        if (XFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            pPolar->ExportPolarNREL(out);
            XFile.close();
        }
    }
}

void QBEM::OnExportBladeTable(){
    if (!m_pBlade) return;

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString FileName, BladeName;

    BladeName = m_pBlade->getName();
    BladeName.replace("/", "_");
    BladeName.replace(" ", "_");
    FileName = QFileDialog::getSaveFileName(this, tr("Export Blade Table"), g_mainFrame->m_ExportLastDirName+QDir::separator()+BladeName,
                                            tr("Text File (*.txt)"),
                                            &g_mainFrame->m_GraphExportFilter);
    if(!FileName.length()) return;

    QFile XFile(FileName);

    if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

    QTextStream out(&XFile);

    out << "Blade Export File Created with "<< g_mainFrame->m_VersionName<<" v0.96 on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << "\n" ;
    out << QString(" %1 %2 %3 %4 %5 %6 %7").arg("Radial Position [m]",25).arg("Chord Length [m]",25).arg("Twist [deg]",25).arg("Pitch Axis Offset [m]",25).arg("Thread Axis in [% chord]",25).arg("Airfoil Name",25).arg("360 Polar Name",25) << endl;
    out << QString().fill('-',182) << endl;
    for (int i=0;i<=m_pBlade->m_NPanel;i++){
        out << QString(" %1 %2 %3 %4 %5 %6 %7").arg(m_pBlade->m_TPos[i], 25, 'e', 5).arg(m_pBlade->m_TChord[i], 25, 'e', 5).arg(m_pBlade->m_TTwist[i], 25, 'e', 5).arg(m_pBlade->m_TOffsetX[i], 25, 'e', 5).arg(m_pBlade->m_TFoilPAxisX[i], 25, 'e', 5).arg(m_pBlade->m_Airfoils.at(i), 25).arg(m_pBlade->m_Polar.at(i), 25)<<endl;
    }
    XFile.close();
}

void QBEM::OnImportBladeGeometry(){
    QString PathName, strong;
    QVector< QVector<double> > data;
    QVector<double> datarow;
    bool isAeroDyn = false;
    bool isQBlade = false;
    bool isWT_Perf = false;
    double HubRad = 0;

    PathName = QFileDialog::getOpenFileName(g_mainFrame, tr("Open File"),
                                            g_mainFrame->m_LastDirName,
                                            tr("QBlade, AeroDyn or WT_Perf Format (*.*)"));
    if(!PathName.length())		return ;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_LastDirName = PathName.left(pos);

    QFile XFile(PathName);
    if (!XFile.open(QIODevice::ReadOnly))
    {
        QString strange = tr("Could not read the file\n")+PathName;
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }
    QTextStream in(&XFile);

    bool valid = true;
    bool converted = true;

    while(!in.atEnd())
    {
        valid = true;
        strong = in.readLine().simplified();
        strong = UnifyString(strong);

        QStringList list = strong.split(QRegExp(" "),QString::SkipEmptyParts);
        for (int i=0; i<list.size();i++){
            if (QRegExp("BldNodes").exactMatch(list.at(i))) {
                std::cout << "AeroDyn Format"<< endl;
                isAeroDyn = true;}
            else if (QRegExp("HubRad:").exactMatch(list.at(i))) {
                HubRad = list.at(0).toDouble(&converted);
                if(!converted){
                    QString strange = tr("Data in file is corrupt or does contain wrong data and cannot be interpreted\n")+PathName;
                    QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                    return;
                }
                std::cout << "WT_Perf Format"<< endl;
                isWT_Perf = true;}

            if (!any_number.exactMatch(list.at(i)) && (i<3)){
            valid = false;
            }
        }

        if (list.size()>2 && valid){

            if (!isAeroDyn && !isWT_Perf){
            if (!isQBlade) std::cout << "QBlade Format" << endl;
            isQBlade = true;}

            datarow.clear();
            for (int j=0;j<list.size();j++){


                if (any_number.exactMatch(list.at(j))) {datarow.append(list.at(j).toDouble());
                if(!converted){
                    QString strange = tr("Data in file is corrupt or does contain wrong data and cannot be interpreted\n")+PathName;
                    QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                    return;
                }
                }
            }
            std::cout << endl;

            data.append(datarow);
            }
        }

        int size = data.size();

        bool isCorrect = true;
        for (int i=0;i<size;i++){
        if (data.at(i).at(0)<0) isCorrect = false;
        if (i > 0) if(data.at(i).at(0)<=data.at(i-1).at(0)) isCorrect = false;
        if (size<2) isCorrect = false;
        }

        if(!isCorrect){
            QString strange = tr("Data in file in wrong order or not enough blade nodes present (minimum 2)\n")+PathName;
            QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
            return;
        }

        CBlade *pBlade = new CBlade;
        if (isAeroDyn) pBlade->setName("AeroDyn Blade Import");
        if (isWT_Perf) pBlade->setName("WT_Perf Blade Import");
        if (isQBlade) pBlade->setName("QBlade Blade Import");
        pBlade->m_Airfoils.clear();
        pBlade->m_Polar.clear();
        pBlade->m_blades = 3;


        if (isAeroDyn){
            pBlade->m_NPanel = size;
            pBlade->m_NSurfaces = size;

            int cols = data.at(0).size();
            if (cols < 4){
                QString strange = tr("AeroDyn file does not contain enough data columns\n")+PathName;
                QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                return;
            }
            for (int i=0;i<size;i++){
                if (i==0){
                pBlade->m_TChord[i] = data.at(i).at(3)-data.at(i).at(2)/2*(data.at(i+1).at(3)-data.at(i).at(3))/(data.at(i+1).at(0)-data.at(i).at(0));//linear interpolation at the blade edges
                pBlade->m_TTwist[i] = data.at(i).at(1)-data.at(i).at(2)/2*(data.at(i+1).at(1)-data.at(i).at(1))/(data.at(i+1).at(0)-data.at(i).at(0));
                pBlade->m_TPos[i] = data.at(i).at(0)-data.at(i).at(2)/2;
                pBlade->m_TRelPos[i] = 0.0;
                pBlade->m_TOffsetX[i] = 0.0;
                pBlade->m_TFoilPAxisX[i] = 0.25;
                pBlade->m_TFoilPAxisZ[i] = 0.0;
                pBlade->m_TOffsetZ[i] = 0.0;
                pBlade->m_NYPanels[i]   = 1;
                pBlade->m_NXPanels[i]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
                else{
                pBlade->m_TChord[i] = (data.at(i).at(3)+data.at(i-1).at(3))/2;
                pBlade->m_TTwist[i] = (data.at(i).at(1)+data.at(i-1).at(1))/2;
                pBlade->m_TPos[i] = data.at(i).at(0)-data.at(i).at(2)/2;
                pBlade->m_TRelPos[i] = data.at(i).at(0)-data.at(i).at(2)/2-(data.at(0).at(0)-data.at(0).at(2)/2);
                pBlade->m_TOffsetX[i] = 0.0;
                pBlade->m_TFoilPAxisX[i] = 0.25;
                pBlade->m_TFoilPAxisZ[i] = 0.0;
                pBlade->m_TOffsetZ[i] = 0.0;
                pBlade->m_NYPanels[i]   = 1;
                pBlade->m_NXPanels[i]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
                if (i==size-1){
                pBlade->m_TChord[size] = data.at(i).at(3)+data.at(i).at(2)/2*(data.at(i-1).at(3)-data.at(i).at(3))/(data.at(i-1).at(0)-data.at(i).at(0));//linear interpolation at the blade edges
                pBlade->m_TTwist[size] = data.at(i).at(1)+data.at(i).at(2)/2*(data.at(i-1).at(1)-data.at(i).at(1))/(data.at(i-1).at(0)-data.at(i).at(0));
                pBlade->m_TPos[size] = data.at(i).at(0)+data.at(i).at(2)/2;
                pBlade->m_TRelPos[size] = data.at(i).at(0)+data.at(i).at(2)/2-(data.at(0).at(0)-data.at(0).at(2)/2);
                pBlade->m_TOffsetX[size] = 0.0;
                pBlade->m_TFoilPAxisX[size] = 0.25;
                pBlade->m_TFoilPAxisZ[size] = 0.0;
                pBlade->m_TOffsetZ[size] = 0.0;
                pBlade->m_NYPanels[size]   = 1;
                pBlade->m_NXPanels[size]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
            }
            pBlade->m_HubRadius = pBlade->m_TPos[0];
            pBlade->m_MaxRadius = pBlade->m_TPos[size];
        }
        else if(isWT_Perf){
            pBlade->m_NPanel = size;
            pBlade->m_NSurfaces = size;

            int cols = data.at(0).size();
            if (cols < 3){
                QString strange = tr("WT_Perf file does not contain enough data columns\n")+PathName;
                QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                return;
            }
            QVector<double> DRNodes;
            for (int i=0;i<size;i++){
                if (i==0){
                DRNodes.append((data.at(i).at(0)-HubRad)*2);
                }
                else{
                DRNodes.append((data.at(i).at(0)-(data.at(i-1).at(0)+DRNodes.at(i-1)/2))*2);
                }
                qDebug() << DRNodes.at(i);
            }
            for (int i=0;i<size;i++){
                if (i==0){
                pBlade->m_TChord[i] = data.at(i).at(2)-DRNodes.at(i)/2*(data.at(i+1).at(2)-data.at(i).at(2))/(data.at(i+1).at(0)-data.at(i).at(0));//linear interpolation at the blade edges
                pBlade->m_TTwist[i] = data.at(i).at(1)-DRNodes.at(i)/2*(data.at(i+1).at(1)-data.at(i).at(1))/(data.at(i+1).at(0)-data.at(i).at(0));
                pBlade->m_TPos[i] = data.at(i).at(0)-DRNodes.at(i)/2;
                pBlade->m_TRelPos[i] = 0.0;
                pBlade->m_TOffsetX[i] = 0.0;
                pBlade->m_TFoilPAxisX[i] = 0.25;
                pBlade->m_TFoilPAxisZ[i] = 0.0;
                pBlade->m_TOffsetZ[i] = 0.0;
                pBlade->m_NYPanels[i]   = 1;
                pBlade->m_NXPanels[i]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
                else{
                pBlade->m_TChord[i] = (data.at(i).at(2)+data.at(i-1).at(2))/2;
                pBlade->m_TTwist[i] = (data.at(i).at(1)+data.at(i-1).at(1))/2;
                pBlade->m_TPos[i] = data.at(i).at(0)-DRNodes.at(i)/2;
                pBlade->m_TRelPos[i] = data.at(i).at(0)-DRNodes.at(i)/2-(data.at(0).at(0)-DRNodes.at(0)/2);
                pBlade->m_TOffsetX[i] = 0.0;
                pBlade->m_TFoilPAxisX[i] = 0.25;
                pBlade->m_TFoilPAxisZ[i] = 0.0;
                pBlade->m_TOffsetZ[i] = 0.0;
                pBlade->m_NYPanels[i]   = 1;
                pBlade->m_NXPanels[i]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
                if (i==size-1){
                pBlade->m_TChord[size] = data.at(i).at(2)+DRNodes.at(i)/2*(data.at(i-1).at(2)-data.at(i).at(2))/(data.at(i-1).at(0)-data.at(i).at(0));//linear interpolation at the blade edges
                pBlade->m_TTwist[size] = data.at(i).at(1)+DRNodes.at(i)/2*(data.at(i-1).at(1)-data.at(i).at(1))/(data.at(i-1).at(0)-data.at(i).at(0));
                pBlade->m_TPos[size] = data.at(i).at(0)+data.at(i).at(2)/2;
                pBlade->m_TRelPos[size] = data.at(i).at(0)+data.at(i).at(2)/2-(data.at(0).at(0)-DRNodes.at(0)/2);
                pBlade->m_TOffsetX[size] = 0.0;
                pBlade->m_TFoilPAxisX[size] = 0.25;
                pBlade->m_TFoilPAxisZ[size] = 0.0;
                pBlade->m_TOffsetZ[size] = 0.0;
                pBlade->m_NYPanels[size]   = 1;
                pBlade->m_NXPanels[size]   = 1;
                pBlade->m_Airfoils.append("-----");
                pBlade->m_Polar.append("-----");
                pBlade->m_Range.append("-----");
                }
            }
            pBlade->m_HubRadius = pBlade->m_TPos[0];
            pBlade->m_MaxRadius = pBlade->m_TPos[size];

        }
        else if(isQBlade){
            pBlade->m_NPanel = size-1;
            pBlade->m_NSurfaces = size-1;

            int cols = data.at(0).size();
            for (int i=0;i<size;i++){
            pBlade->m_TPos[i] = data.at(i).at(0);
            pBlade->m_TRelPos[i] = data.at(i).at(0)-data.at(0).at(0);
            pBlade->m_TChord[i] = data.at(i).at(1);
            pBlade->m_TTwist[i] = data.at(i).at(2);
            if (cols > 3){
                if (data.at(i).at(4) > 1 || data.at(i).at(4) < 0){
                    QString strange = tr("Could not recognize a blade file format\n")+PathName;
                    QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
                    return;
                }
            }
            if (cols > 3) pBlade->m_TOffsetX[i] = data.at(i).at(3);
            else pBlade->m_TOffsetX[i] = 0;
            if (cols > 3) pBlade->m_TFoilPAxisX[i] = data.at(i).at(4);
            else pBlade->m_TFoilPAxisX[i] = 0.25;
            pBlade->m_NYPanels[i]   = 1;
            pBlade->m_NXPanels[i]   = 1;
            pBlade->m_TFoilPAxisZ[i] = 0.0;
            pBlade->m_TOffsetZ[i] = 0.0;
            pBlade->m_Airfoils.append("-----");
            pBlade->m_Polar.append("-----");
            pBlade->m_Range.append("-----");
            }
            pBlade->m_HubRadius = pBlade->m_TPos[0];
            pBlade->m_MaxRadius = pBlade->m_TPos[size-1];
        }
        else{
            QString strange = tr("Could not recognize a blade file format\n")+PathName;
            QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
            return;
        }

        m_PitchOld = 0;
        pitch_new = 0;
        m_pctrlPitchBlade->setValue(0);
        ObjectIsEdited = true;
        m_pctrlBlades->setValue(pBlade->m_blades);
        DisableAllButtons();
        if (g_mainFrame->m_iView != BLADEVIEW) OnBladeView();// new JW, old: m_iView
        m_WingEdited = true;
        m_pctrlHubRadius->setValue(pBlade->m_HubRadius * g_mainFrame->m_mtoUnit);
        m_pBlade = pBlade;
        InitDialog(pBlade);
        OnCenterScene();
        mainWidget->setCurrentIndex(0);
        bladeWidget->setCurrentIndex(1);

}


void QBEM::OnImportPolar(){
    QString PathName, strong;
    QVector<double> alphavalues, liftvalues, dragvalues;

    PathName = QFileDialog::getOpenFileName(g_mainFrame, tr("Open File"),
                                            g_mainFrame->m_LastDirName,
                                            tr("QBlade General Polar Format (*.*)"));

    if(!PathName.length())		return ;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_LastDirName = PathName.left(pos);

    QFile XFile(PathName);
    if (!XFile.open(QIODevice::ReadOnly))
    {
        QString strange = tr("Could not read the file\n")+PathName;
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    QTextStream in(&XFile);

    bool valid = true;

    while(!in.atEnd())
    {
        valid = true;
        strong = in.readLine().simplified();
        strong = UnifyString(strong);


        QStringList list = strong.split(QRegExp(" "),QString::SkipEmptyParts);
        for (int i=0; i<list.size();i++){
            if (!any_number.exactMatch(list.at(i)) || !list.at(i).length()){
            valid = false;
            }
        }

        if (list.size()>2 && valid){
        double alpha, lift, drag;

        bool *aOk = new bool;
        bool *lOk = new bool;
        bool *dOk = new bool;

        *aOk = true;
        *lOk = true;
        *dOk = true;

        alpha = list.at(0).toDouble(aOk);
        lift = list.at(1).toDouble(lOk);
        drag = list.at(2).toDouble(dOk);
//        qDebug() << alpha << lift << drag;


        if (aOk && lOk && dOk){
            alphavalues.append(alpha);
            liftvalues.append(lift);
            dragvalues.append(drag);
        }
        }

    }
    QString msg(tr("Cannot interpret polar data due to the following issues:"));
    bool isCorrect = true;
    for (int i=0;i<alphavalues.size();i++)
    {
        if (i>0) if (alphavalues.at(i)<alphavalues.at(i-1)){
            if (!msg.contains(tr("ascending"))) msg.append(tr("\n- AoA values must be in ascending order"));
            isCorrect = false;
        }
        if (alphavalues.at(i) < -180.0 || alphavalues.at(i) > 180.00){
            if (!msg.contains(tr("-180 and +180"))) msg.append(tr("\n- All AoA must be between -180 and +180"));
            isCorrect = false;
        }
        if (liftvalues.at(i) > 10.0 || liftvalues.at(i) < -10.0){
            if (!msg.contains(tr("Liftcoefficient"))) msg.append(tr("\n- Liftcoefficient with value higher/lower than 10 found"));
            isCorrect = false;
        }
        if (dragvalues.at(i) > 10.0 || dragvalues.at(i) < -10.0){
            if (!msg.contains(tr("Dragcoefficient"))) msg.append(tr("\n- Dragcoefficient with value higher/lower than 10 found"));
            isCorrect = false;
        }
        if (alphavalues.size()<10) {
            if (!msg.contains(tr("Less"))) msg.append(tr("\n- Less than 10 datalines in file"));
            isCorrect = false;
        }
    }

    if (!isCorrect){
        QString strange = msg;
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }
    else{
    CPolar *pPolar = new CPolar;
    pPolar->m_Color = g_mainFrame->GetColor(1);

    for (int i=0;i<alphavalues.size();i++)
    {
        pPolar->AddPoint(alphavalues.at(i), dragvalues.at(i), 0.0, liftvalues.at(i), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    }

    ConnectToAirfoilDialog *dialog = new ConnectToAirfoilDialog (pPolar);

    int response = dialog->exec();

    if (response == ConnectToAirfoilDialog::Ok){

        g_polarStore.add(pPolar);

        QXDirect *pXDirect = (QXDirect *) g_mainFrame->m_pXDirect;

        pXDirect->m_pCurPolar = pPolar;
        pXDirect->m_pCurOpp = NULL;
        g_mainFrame->m_pctrlFoil->setCurrentObject(g_mainFrame->GetFoil(pPolar->getParent()->getName()));
        g_mainFrame->OnSelChangeFoil(0);
        g_mainFrame->m_pctrlPolar->setCurrentObject(pPolar);
        g_mainFrame->OnSelChangePolar(0);


    }
    else delete pPolar;
    }

    CheckButtons();

}

void QBEM::OnImport360Polar(){
    QString PathName, strong;
    QVector<double> alphavalues, liftvalues, dragvalues;
    alphavalues.clear();
    liftvalues.clear();
    dragvalues.clear();

    PathName = QFileDialog::getOpenFileName(g_mainFrame, tr("Open File"),
                                            g_mainFrame->m_LastDirName,
                                            tr("QBlade General Polar Format (*.*)"));

    if(!PathName.length())		return ;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) g_mainFrame->m_LastDirName = PathName.left(pos);

    QFile XFile(PathName);
    if (!XFile.open(QIODevice::ReadOnly))
    {
        QString strange = tr("Could not read the file\n")+PathName;
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    QTextStream in(&XFile);

    bool valid = true;

    while(!in.atEnd())
    {
        valid = true;
        strong = in.readLine().simplified();
        strong = UnifyString(strong);



        QStringList list = strong.split(QRegExp(" "),QString::SkipEmptyParts);
        for (int i=0; i<list.size();i++){
            if (!any_number.exactMatch(list.at(i)) || !list.at(i).length()){
            valid = false;
            }
        }

        if (list.size()>2 && valid){
        double alpha, lift, drag;

        bool *aOk = new bool;
        bool *lOk = new bool;
        bool *dOk = new bool;

        *aOk = true;
        *lOk = true;
        *dOk = true;

        alpha = list.at(0).toDouble(aOk);
        lift = list.at(1).toDouble(lOk);
        drag = list.at(2).toDouble(dOk);
//        qDebug() << alpha << lift << drag;


        if (aOk && lOk && dOk){
            alphavalues.append(alpha);
            liftvalues.append(lift);
            dragvalues.append(drag);
        }
        }

    }

    bool isCorrect = true;
    for (int i=0;i<alphavalues.size();i++)
    {
        qDebug() << alphavalues.at(i);
        if (i>0) if (alphavalues.at(i) < alphavalues.at(i-1)) isCorrect = false;
        if (alphavalues.at(i) < -180.0 || alphavalues.at(i) > 180.00) isCorrect = false;
        if (liftvalues.at(i) > 10.0 || liftvalues.at(i) < -10.0) isCorrect = false;
        if (dragvalues.at(i) > 10.0 || dragvalues.at(i) < -10.0) isCorrect = false;
        if (alphavalues.size()<10) isCorrect = false;
    }

    if (!isCorrect){
        QString strange = tr("Data in file is corrupt or does contain wrong data and cannot be interpreted\n")+PathName;
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }
    else if (abs(alphavalues.at(0)+180.0) > 15 || abs(alphavalues.at(alphavalues.size()-1)-180.0) > 15){
        QString strange = tr("The first AoA in the file should be around -180 (deg) and the last AoA around +180 (deg)\n")+QString("\nfirst AoA: %1 (deg)\nlast AoA: %2 (deg)").arg(alphavalues.at(0),4).arg(alphavalues.at(alphavalues.size()-1),4);
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }
    else{
    C360Polar *pPolar = new C360Polar;
    pPolar->m_Color = g_mainFrame->GetColor(11);

    for (int i=0;i<alphavalues.size();i++)
    {
        pPolar->m_Alpha.append(alphavalues.at(i));
        pPolar->m_Cl.append(liftvalues.at(i));
        pPolar->m_Cd.append(dragvalues.at(i));
        pPolar->m_Cm.append(0);
        pPolar->m_Cl_att.append(0);
        pPolar->m_Cl_sep.append(0);
        pPolar->m_fst.append(0);
    }

    ////compute some 360Polar parameters
    double smallestAlpha = 10e5, residue = 10e5, CLabsmin = 10e5, slope = 0, alphazero = 0, CD90 = 10e5;

    for (int i=0; i<pPolar->m_Alpha.size(); i++)
    {
            if (fabs(pPolar->m_Alpha.at(i)) < smallestAlpha)
            {
                smallestAlpha = fabs(pPolar->m_Alpha.at(i));
                if (pPolar->m_Cl.size() > i+4) slope = (pPolar->m_Cl.at(i+4)-pPolar->m_Cl.at(i))/(pPolar->m_Alpha.at(i+4)-pPolar->m_Alpha.at(i));
            }
            if (fabs(pPolar->m_Alpha.at(i) -90) < residue)
            {
                residue = fabs(pPolar->m_Alpha.at(i)-90);
                CD90 = pPolar->m_Cd.at(i);
            }
    }
    for (int i=0; i<pPolar->m_Alpha.size(); i++)
    {
        if (pPolar->m_Alpha.at(i) > -25 && pPolar->m_Alpha.at(i) < 25)
        {
            if (fabs(pPolar->m_Cl.at(i)) < CLabsmin)
            {
                CLabsmin=fabs(pPolar->m_Cl.at(i));
                alphazero = pPolar->m_Alpha.at(i)-pPolar->m_Cl.at(i)/slope;
            }
        }
    }
    pPolar->slope = slope;
    pPolar->alpha_zero = alphazero;
    pPolar->CD90 = CD90;

    qDebug() <<"Polar Values"<< slope << alphazero << CD90;


    ConnectToPolarDialog *dialog = new ConnectToPolarDialog (pPolar);

    int response = dialog->exec();

    if (response == ConnectToPolarDialog::Ok){

        g_360PolarStore.add(pPolar);

        m_BEMToolBar->m_foilComboBox->setCurrentObject(g_foilStore.getObjectByName(pPolar->getParent()->getParent()->getName(),NULL));
        m_BEMToolBar->m_polarComboBox->setCurrentObject(g_polarStore.getObjectByName( pPolar->getParent()->getName(), pPolar->getParent()->getParent()));
        m_BEMToolBar->m_polar360ComboBox->setCurrentObject(pPolar);
        UpdateFoils();

    }
    else delete pPolar;
    }

    CheckButtons();

}

void QBEM::OnExportRotorToAeroDyn(){

     if (!m_pBlade) return;
    CBlade *rotor = m_pBlade;

    QString directoryName = QFileDialog::getExistingDirectory (g_mainFrame, tr("Choose directory for export"),
                                                                                g_mainFrame->m_ExportLastDirName);
    if (directoryName.isEmpty()) return;

    /* create the needed polar files */
    QStringList exportedPolars;  // will contain the names of exported polars
    QStringList polarFiles;  // will contain the filenames of the exported polars

    C360Polar *polar;
    for (int i = 0; i < rotor->m_NPanel+1; ++i) {
        if (! exportedPolars.contains(rotor->m_Polar[i])) {
            exportedPolars.append(rotor->m_Polar[i]);
            polarFiles.append(QString(rotor->m_Polar[i] + ".dat").replace(" ", "_").replace("/", "_"));

            // NM: name of the polar is unique. No idea why Get360Polar needs the name of Foil too...
            polar = Get360Polar (rotor->m_Airfoils[i], rotor->m_Polar[i]);
            QFile datFile (directoryName + "/" + polarFiles.last());
            if (datFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // option Text sets the correct line endings
                QTextStream datStream (&datFile);
                polar->ExportPolarNREL(datStream);
                datFile.close();
            }
        }
    }

    QFile file2 (QString(directoryName + QDir::separator() + "aerodyn.ipt"));
    if (file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        /* create the needed polar files */
        QVector<C360Polar*> exportedPolars;  // will contain the exported polars
        QStringList polarFiles;  // will contain the filenames of the exported polars

        C360Polar *polar360;
        for (int i = 0; i < m_pBlade->getNumberOfNodes(); ++i) {
            polar360 = m_pBlade->get360PolarAt(i);
            if (! exportedPolars.contains(polar360)) {
                exportedPolars.append(polar360);
                // TODO need a better method to find names for the 360Polar export files!
                polarFiles.append(QString(polar360->getName() + ".dat").replace(" ", "_").replace("/", "_"));

                QFile datFile (QString(directoryName + QDir::separator() + polarFiles.last()));
                if (datFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // option Text sets the correct line endings
                    QTextStream datStream (&datFile);
                    polar360->ExportPolarNREL(datStream);
                    datFile.close();
                } else {
                    throw QString(tr("Could not create file: ") + datFile.fileName());
                }
            }
        }

        QTextStream stream (&file2);

        /* write the whole file */
        stream << m_pBlade->getName() << " :: generated by QBlade" << endl <<
                  QString("%1").arg("SI", -39) <<
                  "SysUnits - System of units for used for input and output [must be SI for FAST] (unquoted string)" << endl <<
                  QString("%1").arg("BEDDOES", -39) <<
                  "StallMod - Dynamic stall included [BEDDOES or STEADY] (unquoted string)" << endl <<
                  QString("%1").arg("NO_CM", -39) <<
                  "UseCm    - Use aerodynamic pitching moment model? [USE_CM or NO_CM] (unquoted string)" << endl <<
                  QString("%1").arg("EQUIL", -39) <<
                  "InfModel - Inflow model [DYNIN or EQUIL] (unquoted string)" << endl <<
                  QString("%1").arg("WAKE", -39) <<
                  "IndModel - Induction-factor model [NONE or WAKE or SWIRL] (unquoted string)" << endl <<
                  QString("%1                               ").arg(0.005, 8, 'f', 3) <<
                  "AToler   - Induction-factor tolerance (convergence criteria) (-)" << endl <<
                  QString("%1").arg("PRANdtl", -39) <<
                  "TLModel  - Tip-loss model (EQUIL only) [PRANDtl, GTECH, or NONE] (unquoted string)" << endl <<
                  QString("%1").arg("PRANdtl", -39) <<
                  "HLModel  - Hub-loss model (EQUIL only) [PRANdtl or NONE] (unquoted string)" << endl <<
                  QString("%1").arg("\"windfieldfile.bts\"", -39) <<
                  "WindFile - Name of file containing wind data (quoted string)" << endl <<
                  QString("%1").arg("xxx", -39) <<
                  "HH       - Wind reference (hub) height [TowerHt+Twr2Shft+OverHang*SIN(ShftTilt)] (m)" << endl <<
                  QString("%1                                 ").arg(0.0, 6, 'f', 1) <<
                  "TwrShad  - Tower-shadow velocity deficit (-)" << endl <<
                  QString("%1                                 ").arg(1.0, 6, 'f', 1) <<
                  "ShadHWid - Tower-shadow half width (m)" << endl <<
                  QString("%1                               ").arg(0.0, 8, 'f', 3) <<
                  "T_Shad_Refpt - Tower-shadow reference point (m)" << endl <<
                  QString("%1                               ").arg(1.225, 8, 'f', 3) <<
                  "AirDens  - Air density (kg/m^3)" << endl <<
                  QString("%1                          ").arg(1.4661e-05, 13, 'e', 4) <<
                  "KinVisc  - Kinematic air viscosity (m^2/sec)" << endl <<
                  QString("%1                               ").arg(0.001, 8, 'f', 5) <<
                  "DTAero   - Time interval for aerodynamic calculations (sec)" << endl <<
                  QString("%1                                   ").arg(polarFiles.size(), 4) <<
                  "NumFoil  - Number of airfoil files (-)" << endl;
        for (int i = 0; i < polarFiles.size(); ++i) {
            if (i == 0)
                stream << QString("%1").arg("\"" + polarFiles[i] + "\"", -38) <<
                          " FoilNm   - Names of the airfoil files [NumFoil lines] (quoted strings)" << endl;
            else
                stream << QString("%1").arg("\"" + polarFiles[i] + "\"") << endl;
        }
        stream << QString("%1                                   ").arg(m_pBlade->m_NPanel, 4) <<
                  "BldNodes - Number of blade nodes used for analysis (-)" << endl <<
                  "RNodes   AeroTwst DRNodes Chord NFoil PrnElm" << endl;
        double position, twist, dr, chord;
        int airfoilNumber;
        for (int i = 0; i < m_pBlade->getNumberOfNodes()-1; ++i) {  // leave out the last QBlade node
            position = m_pBlade->getFASTRadiusAt(i);
            twist = m_pBlade->m_TTwist[i+1] / 2 + m_pBlade->m_TTwist[i] / 2;
            dr = m_pBlade->m_TPos[i+1] - m_pBlade->m_TPos[i];
            chord = m_pBlade->m_TChord[i+1] / 2 + m_pBlade->m_TChord[i] / 2;
            airfoilNumber = exportedPolars.indexOf(m_pBlade->get360PolarAt(i)) + 1;  // index starts at 0 but AeroDyn at 1
            stream << QString("%1").arg(position, 8, 'f', 5) <<
                      QString("%1").arg(twist, 6, 'f', 2) <<
                      QString("%1").arg(dr, 10, 'f', 4) <<
                      QString("%1").arg(chord, 7, 'f', 3) <<
                      QString("%1").arg(airfoilNumber, 4) <<
                      QString("%1").arg("NOPRINT", 10) << endl;

        }
        /* end of file */
        file2.close();
    } else {
        throw QString(tr("Could not create file: ") + file2.fileName());
    }
}

/////////////// new code NM ///////////////
void QBEM::OnExportRotorToWT_Perf()
{
    CBlade *rotor = m_pBlade;

	QString directoryName = QFileDialog::getExistingDirectory (g_mainFrame, tr("Choose directory for export"),
                                                                                g_mainFrame->m_ExportLastDirName);
	if (directoryName.isEmpty()) return;
	
	ExportRotorDialog *parameterDialog = new ExportRotorDialog(dlg_rho,dlg_visc);

	int response = parameterDialog->exec();
	if (response == ExportRotorDialog::Rejected) {
		return;
	}

	/* create the needed polar files */
	QStringList exportedPolars;  // will contain the names of exported polars
	QStringList polarFiles;  // will contain the filenames of the exported polars
	
	C360Polar *polar;
	for (int i = 0; i < rotor->m_NPanel+1; ++i) {
		if (! exportedPolars.contains(rotor->m_Polar[i])) {
			exportedPolars.append(rotor->m_Polar[i]);
			polarFiles.append(QString(rotor->m_Polar[i] + ".dat").replace(" ", "_").replace("/", "_"));
			
			// NM: name of the polar is unique. No idea why Get360Polar needs the name of Foil too...
			polar = Get360Polar (rotor->m_Airfoils[i], rotor->m_Polar[i]);
			QFile datFile (directoryName + "/" + polarFiles.last());
			if (datFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // option Text sets the correct line endings
				QTextStream datStream (&datFile);
                polar->ExportPolarNREL(datStream);
				datFile.close();
			}
		}
	}
	
	/* create the WT_perf file */
	QFile wtpFile (directoryName + "/" + rotor->getName() + ".wtp");
	if (wtpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {  // option Text sets the correct line endings
		QTextStream wtpStream (&wtpFile);
		wtpStream << "-----  WT_Perf Input File  -----------------------------------------------------" << endl <<
					 rotor->getName() << " :: generated by QBlade" << endl <<
					 "Compatible with WT_Perf v3.05.00a-adp." << endl <<
					 "-----  Input Configuration  ----------------------------------------------------" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "Echo:                      Echo input parameters to \"<rootname>.ech\"?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "DimenInp:                  Turbine parameters are dimensional?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "Metric:                    Turbine parameters are Metric (MKS vs FPS)?" << endl <<
					 "-----  Model Configuration  ----------------------------------------------------" << endl <<
					 QString("%1").arg(rotor->m_NPanel, -21) <<
					 "NumSect:                   Number of circumferential sectors." << endl <<
					 QString("%1").arg(13, -21) <<
					 "MaxIter:                   Max number of iterations for Newton's method to find induction factor." << endl <<
					 QString("%1").arg(35, -21) <<
					 "NSplit:                    Max number of splits for binary search method" << endl <<
					 QString("%1").arg(1.0e-5, -21, 'e', 1) <<
					 "ATol:                      Error tolerance for induction iteration." << endl <<
					 QString("%1").arg(1.0e-5, -21, 'e', 1) <<
					 "SWTol:                     Error tolerance for skewed-wake iteration." << endl <<
					 "-----  Algorithm Configuration  ------------------------------------------------" << endl <<
					 QString("%1").arg((parameterDialog->getTipLoss()?"True":"False"), -21) <<
					 "TipLoss:                   Use the Prandtl tip-loss model?" << endl <<
					 QString("%1").arg((parameterDialog->getHubLoss()?"True":"False"), -21) <<
					 "HubLoss:                   Use the Prandtl hub-loss model?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "Swirl:                     Include Swirl effects?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "SkewWake:                  Apply skewed-wake correction?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "IndType:                   Use BEM induction algorithm?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "AIDrag:                    Use the drag term in the axial induction calculation?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "TIDrag:                    Use the drag term in the tangential induction calculation?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "TISingularity:             Use the singularity avoidance method in the tangential-induction calculation?" << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "DAWT:                      Run Diffuser Augmented Water Turbine Analysis? [feature not implimented yet]" << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "Cavitation:                Run cavitation check? if cavitation, output sevens, check 12 oclock azimuth" << endl <<
					 "-----  Cavitation Model  -------------------------------------------------------" << endl <<
					 QString("%1").arg(101325, -21) <<
					 "PressAtm:                  Air Atmospheric Pressure, Pa units, absolute" << endl <<
					 QString("%1").arg(2500, -21) <<
					 "PressVapor:                Vapor Pressure of Water, Pa units, absolute" << endl <<
					 QString("%1").arg(1.0, -21, 'f', 1) <<
					 "CavSF:                     Cavitation safety factor" << endl <<
					 QString("%1").arg(33.0, -21, 'f', 1) <<
					 "WatDepth:                  Depth from water free surface to mudline (tower base)" << endl <<
					 "-----  Turbine Data  -----------------------------------------------------------" << endl <<
                     QString("%1").arg(rotor->m_blades, -21) <<
					 "NumBlade:                  Number of blades." << endl <<
					 QString("%1").arg(rotor->m_TPos[rotor->m_NPanel], -21, 'f', 4) <<
					 "RotorRad:                  Rotor radius [length]." << endl <<
					 QString("%1").arg(rotor->m_TPos[0], -21, 'f', 4) <<
					 "HubRad:                    Hub radius [length or div by radius]." << endl <<
					 QString("%1").arg(0.0, -21, 'f', 1) <<
					 "PreCone:                   Precone angle, positive downwind [deg]." << endl <<
					 QString("%1").arg(0.0, -21, 'f', 1) <<
					 "Tilt:                      Shaft tilt [deg]." << endl <<
					 QString("%1").arg(parameterDialog->getYaw(), -21, 'f', 2) <<
					 "Yaw:                       Yaw error [deg]." << endl <<
					 QString("%1").arg(3.3333, -21, 'f', 4) <<
					 "HubHt:                     Hub height [length or div by radius]." << endl <<
					 QString("%1").arg(rotor->m_NPanel, -21) <<
					 "NumSeg:                    Number of blade segments (entire rotor radius)." << endl <<
					 "   RElm      Twist      Chord  AFfile  PrntElem" << endl;
		
		double position, twist, chord;
		int airfoilNumber;
		for (int i = 0; i < rotor->m_NPanel; i++) {
            position = rotor->m_TPos[i] / 2 + rotor->m_TPos[i+1] / 2;
            twist = rotor->m_TTwist[i] / 2 + rotor->m_TTwist[i+1] / 2;
            chord = rotor->m_TChord[i] / 2 + rotor->m_TChord[i+1] / 2;
			airfoilNumber = exportedPolars.indexOf(rotor->m_Polar[i]) + 1;  // index starts at 0 but WT_perf at 1
			wtpStream << QString("%1%2%3%4%5").arg(position, 7, 'f', 3)
											  .arg(twist, 11, 'f', 3)
											  .arg(chord, 11, 'f', 4)
											  .arg(airfoilNumber, 6)
											  .arg((false?"True":"False"), 11) << endl;
		}
		
		wtpStream << "-----  Aerodynamic Data  -------------------------------------------------------" << endl <<
					 QString("%1").arg(parameterDialog->getRho(), -21, 'f', 7) <<
					 "Rho:                 Air density [mass/volume]." << endl <<
					 QString("%1").arg(parameterDialog->getKinVisc(), -21, 'f', 7) <<
					 "KinVisc:             Kinematic air viscosity" << endl <<
					 QString("%1").arg(0.143, -21, 'f', 3) <<
					 "ShearExp:            Wind shear exponent (1/7 law = 0.143)." << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "UseCm:               Are Cm data included in the airfoil tables?" << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "UseCpmin:            Are Cp,min data included in the airfoil tables?" << endl <<
					 QString("%1").arg(polarFiles.size(), -21) <<
					 "NumAF:               Number of airfoil files." << endl;
		for (int i = 0; i < polarFiles.size(); ++i) {
			wtpStream << polarFiles[i] << endl;
		}
//					 "AF_File:             List of NumAF airfoil files." << endl <<
		wtpStream << "-----  I/O Settings  -----------------------------------------------------------" << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "UnfPower:                  Write parametric power to an unformatted file?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "TabDel:                    When generating formatted output (OutForm=True), make output tab-delimited (fixed-width otherwise)." << endl <<
					 QString("%1").arg(1, -21) <<
					 "ConvFlag:                  For non-converging cases, 0 to output the result, 1 to output nines, 2 to output NaN (safest)." << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "Beep:                      Beep on exit." << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "KFact:                     Output dimensional parameters in K (e.g., kN instead on N)" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "WriteBED:                  Write out blade element data to \"<rootname>.bed\"?" << endl <<
					 QString("%1").arg((false?"True":"False"), -21) <<
					 "InputTSR:                  Input speeds as TSRs?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutMaxCp:                  Output conditions for the maximum Cp?" << endl <<
					 QString("%1").arg("\"fps\"", -21) <<
					 "SpdUnits:                  Wind-speed units (mps, fps, mph)." << endl <<
					 "-----  Combined-Case Analysis  -------------------------------------------------" << endl <<
					 QString("%1").arg(0, -21) <<
					 "NumCases:                  Number of cases to run.  Enter zero for parametric analysis." << endl <<
					 QString("%1").arg("WS or TSR   RotSpd", -21) <<
					 "Pitch                      Remove following block of lines if NumCases is zero." << endl <<
					 "-----  Parametric Analysis (Ignored if NumCases > 0)  --------------------------" << endl <<
					 QString("%1").arg(3, -21) <<
					 "ParRow:                    Row parameter    (1-rpm, 2-pitch, 3-tsr/speed)." << endl <<
					 QString("%1").arg(2, -21) <<
					 "ParCol:                    Column parameter (1-rpm, 2-pitch, 3-tsr/speed)." << endl <<
					 QString("%1").arg(1, -21) <<
					 "ParTab:                    Table parameter  (1-rpm, 2-pitch, 3-tsr/speed)." << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutPwr:                    Request output of rotor power for formatted output?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutCp:                     Request output of Cp for formatted output?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutTrq:                    Request output of shaft torque for formatted output?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutFlp:                    Request output of flap bending moment for formatted output?" << endl <<
					 QString("%1").arg((true?"True":"False"), -21) <<
					 "OutThr:                    Request output of rotor thrust for formatted output?" << endl <<
					 QString("%1").arg(QString("%1, %2, %3").arg(3).arg(4).arg(1), -21) <<
					 "PitSt, PitEnd, PitDel:     First, last, delta blade pitch (deg)." << endl <<
					 QString("%1").arg(QString("%1, %2, %3").arg(72).arg(73).arg(1), -21) <<
					 "OmgSt, OmgEnd, OmgDel:     First, last, delta rotor speed (rpm)." << endl <<
					 QString("%1").arg(QString("%1, %2, %3").arg(15).arg(75).arg(1), -21) <<
					 "SpdSt, SpdEnd, SpdDel:     First, last, delta speeds." << endl;

		wtpFile.close();
	}
	
	delete parameterDialog;
}
/////////// end new code NM ///////////////


void QBEM::OnDeleteTurbine()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to Delete this Turbine?"));
    msgBox.setInformativeText(tr("This will delete associated Simulation Data too!!"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {

        g_tdataStore.remove(m_pTData);
        m_pTData = NULL;
        m_pTBEMData = NULL;


        UpdateTurbines();
        CheckButtons();

    }
    if (msgBox.clickedButton() == cancelButton)
    {
        return;
    }

}

void QBEM::OnSaveTurbine()
{


        TData *pTData = new TData;
        CBlade *pWing;

        pWing=GetWing(WingSelection->currentText());


        pTData->turbtype=0;//new code JW
        pTData->m_TurbineName = m_pctrlTurbineName->text();
		pTData->setName(m_pctrlTurbineName->text());
        pTData->m_WingName = pWing->getName();
		pTData->setSingleParent(pWing);
        pTData->OuterRadius = pWing->m_TPos[pWing->m_NPanel];
		pTData->Generator = m_pctrlGenerator->getValue()/g_mainFrame->m_WtoUnit;
		pTData->CutIn = m_pctrlCutIn->getValue()/g_mainFrame->m_mstoUnit;
		pTData->CutOut = m_pctrlCutOut->getValue()/g_mainFrame->m_mstoUnit;
        pTData->Lambda0 = m_pctrlLambda->getValue();
        pTData->Rot1 = m_pctrlRot1->getValue();
        pTData->Rot2 = m_pctrlRot2->getValue();
		pTData->Switch = m_pctrlSwitch->getValue()/g_mainFrame->m_mstoUnit;
        pTData->is2Step = m_pctrl2Step->isChecked();
        pTData->isFixed = m_pctrlFixed->isChecked();
        pTData->isVariable = m_pctrlVariable->isChecked();
        pTData->isPitch = m_pctrlPitch->isChecked();
        pTData->isStall = m_pctrlStall->isChecked();
		pTData->m_fixedLosses = m_pctrlFixedLosses->getValue()/g_mainFrame->m_WtoUnit;
        pTData->VariableLosses = m_pctrlVariableLosses->getValue();
        pTData->FixedPitch = m_pctrlFixedPitch->getValue();
        pTData->isPrescribedPitch = m_pctrlPrescribedPitch->isChecked();
        pTData->isPrescribedRot = m_pctrlPrescribedRot->isChecked();

        pTData->pitchwindspeeds = pitchwindspeeds;
        pTData->pitchangles = pitchangles;
        pTData->rotwindspeeds = rotwindspeeds;
        pTData->rotspeeds = rotspeeds;

        if (!g_tdataStore.add(pTData)) pTData = NULL;

        m_pTData = pTData;

        InitTurbineData(m_pTData);
        m_TurbineEdited = false;

        EnableAllButtons();
        UpdateTurbines();
        CheckButtons();
        CreatePowerCurves();


}

void QBEM::OnDecompose360Polar() {


    C360Polar *pPolar = new C360Polar(m_pCur360Polar->getName(), m_pCurPolar);

    DisableAllButtons();

    pPolar->m_Color= m_pCur360Polar->m_Color;
    pPolar->m_Style= m_pCur360Polar->m_Style;
    pPolar->m_Width= m_pCur360Polar->m_Width;
    pPolar->m_bIsVisible = true;
    pPolar->m_airfoil = m_pCurFoil;
    pPolar->reynolds = m_pCurPolar->m_Reynolds;
    pPolar->CD90 = m_CD90;
    pPolar->m_bisDecomposed = m_pCur360Polar->m_bisDecomposed;
    pPolar->alpha_zero = m_pCur360Polar->alpha_zero;
    pPolar->slope = m_pCur360Polar->slope;
    pPolar->posalphamax = m_pCur360Polar->posalphamax;

    for (int i=0; i < m_pCur360Polar->m_Alpha.size(); i++)
    {
        pPolar->m_Alpha.append(m_pCur360Polar->m_Alpha.at(i));
        pPolar->m_Cl.append(m_pCur360Polar->m_Cl.at(i));
        pPolar->m_Cd.append(m_pCur360Polar->m_Cd.at(i));
        pPolar->m_Cm.append(m_pCur360Polar->m_Cd.at(i));
        pPolar->m_Cl_att.append(m_pCur360Polar->m_Cd.at(i));
        pPolar->m_Cl_sep.append(m_pCur360Polar->m_Cd.at(i));
        pPolar->m_fst.append(m_pCur360Polar->m_Cd.at(i));
    }

    m_pCur360Polar = pPolar;

    ComputeDecomposition();
    CreateSinglePolarCurve();
    CheckButtons();
    SetCurveParams();
    FillComboBoxes();
}


void QBEM::OnNew360Polar() {
	/* check first, if all requirements are fullfilled */
	QString errorMessage ("");

    if (m_pCurPolar->getName().contains("Circular")) errorMessage.append("\n- Polars from circular Foils cannot be extrapolated");

	if (m_pCurPolar->m_Alpha.size() < 10) {
		errorMessage.append("\n- Polar should have at least 10 points (currently: " +
							QString("%1").number(m_pCurPolar->m_Alpha.size())+")");
	}
	
	int count = 0;
	for (int i = 0; i < m_pCurPolar->m_Alpha.size(); ++i) {
		if (m_pCurPolar->m_Alpha.at(i) >= 0 && m_pCurPolar->m_Alpha.at(i) <= 20) count++;
	}
	
	if (count < 5) errorMessage.append("\n- At least 5 data points in the range 0 < alpha < 20 needed (currently: " +
									   QString("%1").number(count)+")");
	
	if (errorMessage != "") {
		QMessageBox::critical(this, tr("Create 360Polar"), QString(tr("The following error(s) occured:\n") +
																   errorMessage), QMessageBox::Ok);
		return;
	}
	
	QString strong;
	if (m_bStallModel) {
		strong = m_pCurPolar->getName() + " 360 V";
	} else {
		strong = m_pCurPolar->getName() + " 360 M";
	}	

    DisableAllButtons();
	
	C360Polar *pPolar = new C360Polar(strong, m_pCurPolar);
	
    m_bNew360Polar = true;

	QString newname(strong);
	for (int i=0;i<g_360PolarStore.size();i++){
        if (newname == g_360PolarStore.at(i)->getName() && g_360PolarStore.at(i)->getParent() == m_pCurPolar){
			newname = makeNameWithHigherNumber(newname);
            i = 0;
		}
	}

	m_360Name->setText(newname);
	pPolar->m_airfoil = m_pCurFoil;
	pPolar->m_Color =  g_mainFrame->GetColor(11);
	m_pCur360Polar = pPolar;

    ComputePolarVars();

    m_AR = m_pctrlAR->value();
    m_CD90 = m_pctrlCD90->value();
    m_Slope->setValue(m_pCur360Polar->slope);
    m_posAoA->setValue(m_pCurPolar->m_Alpha.at(m_pCurPolar->m_Alpha.size()-1));
    m_negAoA->setValue(m_pCurPolar->m_Alpha.at(0));



	if (!m_bStallModel) {
		Compute360Polar();
	} else {
		ComputeViterna360Polar();
	}
	
	CreateSinglePolarCurve();
	m_pctrlA->setValue(0);
	
	CheckButtons();
	SetCurveParams();
	FillComboBoxes();
}

void QBEM::OnNewBlade()
{
    if (!g_360PolarStore.size()) return;

    ObjectIsEdited = true;

	m_PitchOld = 0;
	pitch_new = 0;
	m_WingEdited = true;    
	
	DisableAllButtons();
	
    CBlade *pWing = new CBlade ("New Blade");

    m_pBlade = pWing;

    m_pBlade->m_Airfoils.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Airfoils.append("-----");

    m_pBlade->m_Polar.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Polar.append("-----");

    m_pBlade->m_Range.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Range.append("-----");

    QString newname(m_pBlade->getName());
    for (int i=0;i<g_rotorStore.size();i++){
        if (newname == g_rotorStore.at(i)->getName()){
            newname = makeNameWithHigherNumber(newname);
            i = 0;
        }
    }

    m_pBlade->setName(newname);
	
    m_pctrlHubRadius->setValue(m_pBlade->m_HubRadius * g_mainFrame->m_mtoUnit);

    m_SingleMultiGroup->button(0)->setChecked(m_pBlade->m_bisSinglePolar);

    OnCenterScene();
    InitDialog(m_pBlade);

    m_pctrlPitchBlade->setValue(0);
    m_pctrlBlades->setValue(3);

	
	mainWidget->setCurrentIndex(0);
	bladeWidget->setCurrentIndex(1);
}

void QBEM::OnEditBlade()
{

        m_PitchOld = 0;
        pitch_new = 0;
        m_pctrlPitchBlade->setValue(0);

        if (!m_pBlade) return;

        ObjectIsEdited = true;

        DisableAllButtons();

        if (g_mainFrame->m_iView != BLADEVIEW) OnBladeView();// new JW, old: m_iView

        m_WingEdited = true;

        CBlade *pWing = new CBlade;

        pWing->Duplicate(m_pBlade);

        m_SingleMultiGroup->button(0)->setChecked(pWing->m_bisSinglePolar);
        m_SingleMultiGroup->button(1)->setChecked(!pWing->m_bisSinglePolar);

		m_pctrlHubRadius->setValue(pWing->m_HubRadius * g_mainFrame->m_mtoUnit);

        InitDialog(pWing);

        m_pctrlBlades->setValue(pWing->m_blades);

        OnSingleMultiPolarChanged();

        mainWidget->setCurrentIndex(0);
        bladeWidget->setCurrentIndex(1);



}

void QBEM::OnDeleteBlade()
{
    if (!m_pBlade) return;

    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to delete this Blade?"));
    msgBox.setInformativeText(tr("This will delete all associated Simulation Data too!!"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);




    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {

        CBlade *pBlade = m_pBlade;

        m_pBData        = NULL;
        m_pTurbineBData = NULL;
        m_pBEMData      = NULL;
        m_pTData        = NULL;
        m_pTBEMData     = NULL;
        m_pCBEMData     = NULL;
        m_pBlade         = NULL;

        ObjectIsEdited = false;



        g_rotorStore.remove(pBlade);


        UpdateBlades();
        UpdateTurbines();
        UpdateCharacteristicsSimulation();
        CheckButtons();
        OnCenterScene();


    }
    if (msgBox.clickedButton() == cancelButton)
    {
        return;
    }




}

void QBEM::OnRename360Polar(){

    if (!m_pCur360Polar) return;

    QString OldName = m_pCur360Polar->getName();

    g_360PolarStore.rename(m_pCur360Polar,m_pCur360Polar->getName());

    QString strong = m_pCur360Polar->getName();

    for (int i=0; i<g_rotorStore.size(); i++)
    {
        for (int l=0; l<g_rotorStore.at(i)->m_Polar.size();l++)
        {
            if (g_rotorStore.at(i)->m_Polar.at(l) == OldName) g_rotorStore.at(i)->m_Polar[l] = strong;
        }

        for (int j=0; j<g_rotorStore.at(i)->m_MultiPolars.size();j++)
        {
            for (int k=0; k < g_rotorStore.at(i)->m_MultiPolars.at(j).size(); k++){
            if (g_rotorStore.at(i)->m_MultiPolars.at(j).at(k) == OldName) g_rotorStore.at(i)->m_MultiPolars[j][k] = strong;
            }
        }
    }

    for (int i=0; i<g_verticalRotorStore.size(); i++)
    {
        for (int l=0; l<g_verticalRotorStore.at(i)->m_Polar.size();l++)
        {
            if (g_verticalRotorStore.at(i)->m_Polar.at(l) == OldName) g_verticalRotorStore.at(i)->m_Polar[l] = strong;
        }
        for (int j=0; j<g_verticalRotorStore.at(i)->m_MultiPolars.size();j++)
        {
            for (int k=0; k < g_verticalRotorStore.at(i)->m_MultiPolars.at(j).size(); k++){
            if (g_verticalRotorStore.at(i)->m_MultiPolars.at(j).at(k) == OldName) g_verticalRotorStore.at(i)->m_MultiPolars[j][k] = strong;
            }
        }
    }

}


void QBEM::OnDelete360Polar()
{


    QMessageBox msgBox;
    msgBox.setText(tr("Do you really want to Delete this Polar?"));
    msgBox.setInformativeText(tr("This will delete associated Blades and Simulation Data too!!!"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {

    if (m_pCur360Polar)
    {
      m_pBlade = NULL;
      m_pBData = NULL;
      m_pTurbineBData = NULL;
      m_pBEMData = NULL;
      m_pTData = NULL;
      m_pTBEMData = NULL;
      m_pCBEMData = NULL;

      g_qdms->m_pBlade = NULL;
      g_qdms->m_pDData = NULL;
      g_qdms->m_pTurbineDData = NULL;
      g_qdms->m_pDMSData = NULL;
      g_qdms->m_pTData = NULL;
      g_qdms->m_pTDMSData = NULL;
      g_qdms->m_pCDMSData = NULL;

    g_360PolarStore.remove(m_pCur360Polar);

    m_pCur360Polar = NULL;


    Update360Polars();
	CheckButtons();
    }
    }
    else if (msgBox.clickedButton() == cancelButton)
    {
		return;
    }
}


void QBEM::CombinePolars(){

    C360Polar *pPolar = new C360Polar(m_360Name->text(), m_pCurPolar);

    pPolar->m_Color= m_pCur360Polar->m_Color;
    pPolar->m_Style= m_pCur360Polar->m_Style;
    pPolar->m_Width= m_pCur360Polar->m_Width;
    pPolar->m_bShowPoints= m_pCur360Polar->m_bShowPoints;
    pPolar->m_bIsVisible = true;
    pPolar->m_airfoil = m_pCurFoil;
    pPolar->reynolds = m_pCurPolar->m_Reynolds;
    pPolar->CD90 = m_CD90;
    pPolar->m_bisDecomposed = m_pCur360Polar->m_bisDecomposed;
    pPolar->alpha_zero = m_pCur360Polar->alpha_zero;
    pPolar->slope = m_pCur360Polar->slope;
    pPolar->posalphamax = m_pCur360Polar->posalphamax;
    pPolar->CLzero=m_pCur360Polar->CLzero;



    for (int i=0; i < m_pCurPolar->m_Alpha.size(); i++)
    {
        if (m_pCurPolar->m_Alpha.at(i) >= m_negAoA->value() && m_pCurPolar->m_Alpha.at(i) <= m_posAoA->value()){
            pPolar->m_Alpha.append(m_pCurPolar->m_Alpha.at(i));
            pPolar->m_Cl.append(m_pCurPolar->m_Cl.at(i));
            pPolar->m_Cd.append(m_pCurPolar->m_Cd.at(i));
        }
    }

    int num=0;
    for (int i=0; i< m_pCur360Polar->m_Alpha.size(); i++)
    {

        if (m_pCur360Polar->m_Alpha.at(i) < pPolar->m_Alpha.at(num))
        {
            pPolar->m_Alpha.insert(num,m_pCur360Polar->m_Alpha.at(i));
            pPolar->m_Cl.insert(num,m_pCur360Polar->m_Cl.at(i));
            pPolar->m_Cd.insert(num,m_pCur360Polar->m_Cd.at(i));
            num++;
        }
        if (m_pCur360Polar->m_Alpha.at(i) > pPolar->m_Alpha.at(pPolar->m_Alpha.size()-1))
        {
            pPolar->m_Alpha.append(m_pCur360Polar->m_Alpha.at(i));
            pPolar->m_Cl.append(m_pCur360Polar->m_Cl.at(i));
            pPolar->m_Cd.append(m_pCur360Polar->m_Cd.at(i));
        }
    }

    delete m_pCur360Polar;

    m_pCur360Polar = pPolar;
}

void QBEM::OnSave360Polar()
{

    m_pCur360Polar->setName(m_360Name->text());

    if (!g_360PolarStore.add(m_pCur360Polar)) m_pCur360Polar = m_BEMToolBar->m_polar360ComboBox->currentObject();

    m_BEMToolBar->m_polar360ComboBox->setCurrentObject(m_pCur360Polar);

	m_bNew360Polar = false;
    m_bDecompose = false;

    EnableAllButtons();

    Update360Polars();

    CheckButtons();
}

void QBEM::OnSaveBlade()
{
    SimpleAdvanced->setCurrentIndex(0);

    ReadParams();

    m_pBlade->m_blades = m_pctrlBlades->value();

    if (m_pBlade->m_bisSinglePolar){
        for (int i=0;i<=m_pBlade->m_NPanel;i++)
        {
            m_pBlade->addParent(Get360Polar(m_pBlade->m_Airfoils.at(i),m_pBlade->m_Polar.at(i)));
        }
    }
    else{
        for (int i=0;i<m_pBlade->m_PolarAssociatedFoils.size();i++)
        {
            for (int j=0;j<m_pBlade->m_MultiPolars.at(i).size();j++)
            {
            m_pBlade->addParent(Get360Polar(m_pBlade->m_PolarAssociatedFoils.at(i),m_pBlade->m_MultiPolars.at(i).at(j)));
            }
        }
    }

    if (!g_rotorStore.add(m_pBlade)) m_pBlade = NULL;

    m_WingEdited = false;

    if (m_pBlade) m_BEMToolBar->m_rotorComboBox->setCurrentObject(m_pBlade);
    else m_BEMToolBar->m_rotorComboBox->setCurrentIndex(0);

    UpdateBlades();
    UpdateTurbines();
    EnableAllButtons();
    CheckButtons();
}

void QBEM::OnSelChangeWing(int /*i*/)
{


            m_pBlade = m_BEMToolBar->m_rotorComboBox->currentObject();

            OnCenterScene();

            InitBladeTable();
            SetScale();
            UpdateBlades();
            CheckButtons();
}

void QBEM::OnSelChangeFoil(int /*i*/)
{

    m_bNew360Polar=false;
	m_pCurFoil = m_BEMToolBar->m_foilComboBox->currentObject();
    g_mainFrame->SetCurrentFoil(m_pCurFoil);

    m_pCur360Polar = NULL;
    m_pCurPolar = NULL;



	UpdateFoils();

    CheckButtons();
    SetCurveParams();

}

void QBEM::OnSelChangeWind(int i)
{

    selected_wind = i;
    m_CharGraph1.m_bAutoX = false;
    m_CharGraph2.m_bAutoX = false;
    m_CharGraph3.m_bAutoX = false;
    m_CharGraph4.m_bAutoX = false;
    m_CharGraph1.m_bAutoY = false;
    m_CharGraph2.m_bAutoY = false;
    m_CharGraph3.m_bAutoY = false;
    m_CharGraph4.m_bAutoY = false;
    CreateCharacteristicsCurves();

}

void QBEM::OnSelChangeRot(int i)
{

    selected_rot = i;
    m_CharGraph1.m_bAutoX = false;
    m_CharGraph2.m_bAutoX = false;
    m_CharGraph3.m_bAutoX = false;
    m_CharGraph4.m_bAutoX = false;
    m_CharGraph1.m_bAutoY = false;
    m_CharGraph2.m_bAutoY = false;
    m_CharGraph3.m_bAutoY = false;
    m_CharGraph4.m_bAutoY = false;
    CreateCharacteristicsCurves();

}

void QBEM::OnSelChangePitch(int i)
{

    selected_pitch = i;
    m_CharGraph1.m_bAutoX = false;
    m_CharGraph2.m_bAutoX = false;
    m_CharGraph3.m_bAutoX = false;
    m_CharGraph4.m_bAutoX = false;
    m_CharGraph1.m_bAutoY = false;
    m_CharGraph2.m_bAutoY = false;
    m_CharGraph3.m_bAutoY = false;
    m_CharGraph4.m_bAutoY = false;
    CreateCharacteristicsCurves();

}



void QBEM::OnSelChangePolar(int /*i*/)
{
    m_bNew360Polar=false;
    m_pCurPolar = m_BEMToolBar->m_polarComboBox->currentObject();
    m_pCur360Polar = NULL;

    UpdatePolars();

    CheckButtons();
    SetCurveParams();
}

void QBEM::OnSelChange360Polar(int /*i*/)
{
    m_bNew360Polar=false;

	m_pCur360Polar = m_BEMToolBar->m_polar360ComboBox->currentObject();

    Update360Polars();

    CheckButtons();
    SetCurveParams();



}

void QBEM::OnSelChangeBladeData(int i)
{

    QString strong;

	if (i>=0) strong = m_BEMToolBar->m_tsrComboBox->itemText(i);

    m_pBData = GetBladeData(strong);

    UpdateBladeData();

	CheckButtons();


}

void QBEM::OnSelChangeTurbineBladeData(int i)
{

    QString strong;



	if (i>=0) strong = m_BEMToolBar->m_turbinewindspeedComboBox->itemText(i);

    m_pTurbineBData = GetTurbineBladeData(strong);

    UpdateTurbineBladeData();

    CheckButtons();


}

void QBEM::OnBladeView()
{
    g_mainFrame->setIView(BLADEVIEW,BEM);
    g_mainFrame->setIApp(BEM);

    if (!m_WingEdited) UpdateBlades();

    OnCenterScene();

	g_mainFrame->OnBEM();

    CheckButtons();

    g_mainFrame->SetCentralWidget();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    g_mainFrame->m_pctrlBEMWidget->setMinimumWidth(width/m_widthfrac*2);

	UpdateView();

    configureGL();
}

void QBEM::On360View()
{
	g_mainFrame->setIView(POLARVIEW,BEM);// new JW //m_iView = POLARVIEW;
	g_mainFrame->setIApp(BEM);  // NM vorher: g_mainFrame->m_iApp = BEM;


    if (!m_bNew360Polar) UpdateFoils();

	g_mainFrame->OnBEM();

    CheckButtons();
    g_mainFrame->SetCentralWidget();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    g_mainFrame->m_pctrlBEMWidget->setFixedWidth(width/m_widthfrac);

	UpdateView();

}

void QBEM::OnTurbineView()
{
	g_mainFrame->setIView(TURBINEVIEW,BEM);// new JW //m_iView = TURBINEVIEW;
	g_mainFrame->setIApp(BEM);  // NM vorher: g_mainFrame->m_iApp = BEM;

	//g_mainFrame->m_pctrlBEMWidget->setMaximumWidth(300);
	//g_mainFrame->m_pctrlBEMWidget->setMaximumHeight(1000);

    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_PowerLegendOffset.rx() = 10;
    m_PowerLegendOffset.ry() = h2 + 30;

    if (!m_TurbineEdited) UpdateTurbines();

	g_mainFrame->OnBEM();

    CheckButtons();
	g_mainFrame->SetCentralWidget();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    g_mainFrame->m_pctrlBEMWidget->setFixedWidth(width/m_widthfrac);

	UpdateView();

}

void QBEM::OnCharView()
{
	g_mainFrame->setIView(CHARSIMVIEW,BEM);// new JW //m_iView = CHARSIMVIEW;
	g_mainFrame->setIApp(BEM);  // NM vorher: g_mainFrame->m_iApp = BEM;

    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_CharLegendOffset.rx() = 10;
    m_CharLegendOffset.ry() = h2 + 30;

    if(!m_WingEdited) UpdateBlades();

	g_mainFrame->OnBEM();

    CheckButtons();
	g_mainFrame->SetCentralWidget();
	UpdateView();
}


void QBEM::OnRotorsimView()
{


	g_mainFrame->setIView(BEMSIMVIEW,BEM);// new JW //m_iView = BEMSIMVIEW;
    g_mainFrame->setIApp(BEM);  // NM vorher: g_mainFrame->m_iApp = BEM;

    //g_mainFrame->m_pctrlBEMWidget->setMaximumWidth(300);

    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_CharLegendOffset.rx() = 10;
    m_CharLegendOffset.ry() = h2 + 30;

    if(!m_WingEdited) UpdateBlades();

    g_mainFrame->OnBEM();

    CheckButtons();
	g_mainFrame->SetCentralWidget();
    UpdateView();

	g_mainFrame->setIView(BEMSIMVIEW,BEM);// new JW, important for size change!

}

void QBEM::OnSelChangeRotorSimulation() {
	m_pBEMData = m_BEMToolBar->m_bemdataComboBox->currentObject();

    UpdateRotorSimulation();
    SetCurveParams();
    CheckButtons();
}

void QBEM::OnSelChangeCharSimulation() {
	m_pCBEMData = m_BEMToolBar->m_cbemdataComboBox->currentObject();

    UpdateCharacteristicsSimulation();

    CheckButtons();
}

void QBEM::OnSelChangeTurbineSimulation() {
	m_pTBEMData = m_BEMToolBar->m_tbemdataComboBox->currentObject();

    UpdateTurbineSimulation();

    SetCurveParams();

    CheckButtons();
}


//new code JW
void QBEM::OnSelChangeHeightData(int /*i*/)
{
}


void QBEM::OnSelChangeTurbineHeightData(int /*i*/)
{
}
//end new code JW


void QBEM::OnGraphSettings()
{
	QGraph *pGraph = NULL;
	GraphDlg dlg;
	dlg.move(g_mainFrame->m_DlgPos);
	
	pGraph = m_pCurGraph;
	if(!pGraph) return;
	
	if (g_mainFrame->m_iView == BEMSIMVIEW && m_pCurGraph->m_Type == 0)  dlg.m_iGraphType = 70;
	if (g_mainFrame->m_iView == BEMSIMVIEW && m_pCurGraph->m_Type == 1)  dlg.m_iGraphType = 71;
	
	if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 0) dlg.m_iGraphType = 80;
	if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 1) dlg.m_iGraphType = 81;
	
	if (g_mainFrame->m_iView == POLARVIEW) dlg.m_iGraphType = 90;
	
	if ((g_mainFrame->m_iView == CHARSIMVIEW) && m_pCurGraph->m_MainVar == 0 ) dlg.m_iGraphType = 100;
	if ((g_mainFrame->m_iView == CHARSIMVIEW) && m_pCurGraph->m_MainVar == 1 ) dlg.m_iGraphType = 101;
	if ((g_mainFrame->m_iView == CHARSIMVIEW) && m_pCurGraph->m_MainVar == 2 ) dlg.m_iGraphType = 102;
	
	if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 3) dlg.m_iGraphType = 112;
	
	QGraph graph;
	graph.CopySettings(pGraph);
	dlg.m_pMemGraph = &graph;
	dlg.m_pGraph = pGraph;
	
	dlg.SetParams();
	int res = dlg.exec();
	g_mainFrame->m_DlgPos = dlg.pos();
	
	if(res == QDialog::Accepted)
	{
		if(&m_RotorGraph1 == pGraph)
		{
			SetRotorGraphTitles(&m_RotorGraph1);
		}
		else if(&m_RotorGraph2 == pGraph)
		{
			SetRotorGraphTitles(&m_RotorGraph2);
		}
		else if(&m_RotorGraph3 == pGraph)
		{
			SetRotorGraphTitles(&m_RotorGraph3);
		}
		else if(&m_PowerGraph1 == pGraph)
		{
			SetPowerGraphTitles(&m_PowerGraph1);
			SetWeibullGraphTitles(&m_PowerGraph1);//new code JW
		}
		else if(&m_PowerGraph2 == pGraph)
		{
			SetPowerGraphTitles(&m_PowerGraph2);
			SetWeibullGraphTitles(&m_PowerGraph2);//new code JW
		}
		else if(&m_PowerGraph3 == pGraph)
		{
			SetPowerGraphTitles(&m_PowerGraph3);
			SetWeibullGraphTitles(&m_PowerGraph3);//new code JW
		}
        else if(&m_360Graph1 == pGraph)
		{
            SetPolarGraphTitles(&m_360Graph1);
		}
        else if(&m_360Graph2 == pGraph)
		{
            SetPolarGraphTitles(&m_360Graph2);
		}
        else if(&m_360Graph3 == pGraph)
        {
            SetPolarGraphTitles(&m_360Graph3);
        }
        else if(&m_360Graph4 == pGraph)
        {
            SetPolarGraphTitles(&m_360Graph4);
        }
		else if(&m_CharGraph1 == pGraph)
		{
			SetCharGraphTitles(&m_CharGraph1);
		}
		else if(&m_CharGraph2 == pGraph)
		{
			SetCharGraphTitles(&m_CharGraph2);
			
		}
		else if(&m_CharGraph3 == pGraph)
		{
			SetCharGraphTitles(&m_CharGraph3);
		}
		else if(&m_CharGraph4 == pGraph)
		{
			SetCharGraphTitles(&m_CharGraph4);
		}
		
		if(dlg.m_bVariableChanged)
		{
			pGraph->SetAuto(true);
			pGraph->SetAutoYMinUnit(true);
		}
		CreateRotorCurves();
		CreatePowerCurves();
        if (!m_bNew360Polar && !m_bDecompose) CreatePolarCurve();
        else CreateSinglePolarCurve();
		CreateCharacteristicsCurves();
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	UpdateView();
}

void QBEM::OnSetCharMainWind()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_MainVar=0;
    if (m_pCurGraph->m_Param==0) m_pCurGraph->m_Param=-1;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharMainRot()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_MainVar=1;
    if (m_pCurGraph->m_Param==1) m_pCurGraph->m_Param=-1;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharMainPitch()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_MainVar=2;
    if (m_pCurGraph->m_Param==2) m_pCurGraph->m_Param=-1;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharParamWind()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_Param=0;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharParamRot()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_Param=1;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharParamPitch()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_Param=2;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnSetCharParamNone()
{

    if (!m_pCurGraph) return;
    m_pCurGraph->m_Param=-1;
    m_pCurGraph->SetAuto(true);
    CreateCharacteristicsCurves();
    CheckButtons();

}

void QBEM::OnCenterScene()
{
	if(!m_pBlade) return;
	if (g_mainFrame->m_iApp != BEM) return;
	
	if (m_pctrlShowTurbine->isChecked())
	{
		m_pGLWidget->setSceneCenter(qglviewer::Vec(0,0,0));
        m_pGLWidget->setSceneRadius(float(m_pBlade->getRotorRadius()*1.4));
	}
	else
	{
		m_pGLWidget->setSceneCenter(qglviewer::Vec(0,m_pBlade->getRotorRadius()/2.0,0));
        m_pGLWidget->setSceneRadius(float(m_pBlade->getRotorRadius()/2.0)*1.4);
	}
	m_pGLWidget->showEntireScene();
	m_pGLWidget->updateGL();
}

void QBEM::PitchBlade()
{
        double delta;

        pitch_new = m_pctrlPitchBlade->value();

        delta = pitch_new - m_PitchOld;

        for (int i=0;i<=m_pBlade->m_NPanel;i++)
        {
            m_pBlade->m_TTwist[i] = m_pBlade->m_TTwist[i]-delta;
        }

        m_PitchOld = pitch_new;

        m_pBlade->setName(m_pctrlWingName->text());

        InitDialog(m_pBlade);
}

void QBEM::PaintCharLegend(QPoint place, int bottom, QPainter &painter)
{
        int LegendSize, LegendWidth, ypos, x1;
        int i,j,k,l,nc,ny,nFoils;

        LegendSize = 30;
        LegendWidth = 210;

        QFont TextFont;
		painter.setFont(g_mainFrame->m_TextFont);

        QFontMetrics fm(TextFont);
        ypos = fm.height();

		QPen TextPen(g_mainFrame->m_TextColor);
        painter.setPen(TextPen);
        TextPen.setWidth(1);

        QStringList str; // we need to make an inventory of wings

        for (j=0; j < g_rotorStore.size(); j++)
        {
                CBlade *pWing = g_rotorStore.at(j);
				for (i=0; i<g_bemdataStore.size(); i++)
                {
						if (g_bemdataStore.at(i)->m_WingName == pWing->getName() && g_bemdataStore.at(i)->m_Cp.size())
                        {
                                str.append(pWing->getName());
                                break;
                        }
                }// finished inventory
        }

        nFoils= str.size();

        painter.setBackgroundMode(Qt::TransparentMode);

        QPen LegendPen;
        LegendPen.setWidth(1);

        ny =0;
        for (k = 0; k<nFoils; k++)
        {
                int FoilPlrs = 0;
				for (l=0; l < g_bemdataStore.size(); l++)
                {
						if (g_bemdataStore.at(l)->m_Cp.size() &&
								g_bemdataStore.at(l)->m_Cp.length() &&
								g_bemdataStore.at(l)->m_bIsVisible &&
								g_bemdataStore.at(l)->m_WingName == str.at(k))
                                        FoilPlrs++;
                }
                if (FoilPlrs)
                {
                        int YPos = place.y() + (ny+FoilPlrs+2) * ypos;// bottom line of this foil's legend
                        if(abs(bottom) > abs(YPos))
                        {
                                ny++;
                        }
                        else if (k>0)
                        {
                        // move rigth if less than client bottom area
                                place.rx() += LegendWidth;
                                ny=1;
                        }
                        painter.setPen(TextPen);
                        painter.drawText(place.x() + (int)(0.5*LegendSize), place.y() + ypos*ny-(int)(ypos/2),
                                                         str.at(k));
                }
				for (nc=0; nc < g_bemdataStore.size(); nc++)
                {
						if(str.at(k) == g_bemdataStore.at(nc)->m_WingName)
                        {
								if (g_bemdataStore.at(nc)->m_Cp.size() && g_bemdataStore.at(nc)->m_WingName.length() && g_bemdataStore.at(nc)->m_bIsVisible)

                                {
                                        //is there anything to draw ?
										LegendPen.setColor(g_bemdataStore.at(nc)->m_Color);
										LegendPen.setStyle(GetStyle(g_bemdataStore.at(nc)->m_Style));
										LegendPen.setWidth(g_bemdataStore.at(nc)->m_Width);
                                        painter.setPen(LegendPen);

                                        painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
                                                                         place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
										if(g_bemdataStore.at(nc)->m_bShowPoints)
                                        {
                                                x1 = place.x() + (int)(1.5*LegendSize);
                                                painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
                                        }

                                        painter.setPen(TextPen);
                                        painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
																		 g_bemdataStore.at(nc)->m_BEMName);
                                        ny++ ;
                                }
                        }
                }
                if (FoilPlrs) ny++;
        }

}

void QBEM::PaintPowerLegend(QPoint place, int bottom, QPainter &painter)
{
        int LegendSize, LegendWidth, ypos, x1;
        QColor color;
        int i,j,k,l,nc,ny,nFoils;

        LegendSize = 30;
        LegendWidth = 210;

		painter.setFont(g_mainFrame->m_TextFont);

		QFontMetrics fm(g_mainFrame->m_TextFont);
        ypos = fm.height();

		QPen TextPen(g_mainFrame->m_TextColor);
        painter.setPen(TextPen);
        TextPen.setWidth(1);

        QStringList str; // we need to make an inventory of foils

		for (j=0; j<g_tdataStore.size(); j++)
        {

				for (i=0; i<g_tbemdataStore.size(); i++)
                {
						if (g_tbemdataStore.at(i)->m_TurbineName == g_tdataStore.at(j)->m_TurbineName && g_tbemdataStore.at(i)->m_P.size())
                        {
								str.append(g_tdataStore.at(j)->m_TurbineName);
                                break;
                        }
                }// finished inventory
        }

        nFoils= str.size();

        painter.setBackgroundMode(Qt::TransparentMode);

        QPen LegendPen;
        LegendPen.setWidth(1);

        ny =0;
        for (k = 0; k<nFoils; k++)
        {
                int FoilPlrs = 0;
				for (l=0; l < g_tbemdataStore.size(); l++)
                {
						if (g_tbemdataStore.at(l)->m_P.size() &&
								g_tbemdataStore.at(l)->m_P.length() &&
								g_tbemdataStore.at(l)->m_bIsVisible &&
								g_tbemdataStore.at(l)->m_TurbineName == str.at(k))
                                        FoilPlrs++;
                }
                if (FoilPlrs)
                {
                        int YPos = place.y() + (ny+FoilPlrs+2) * ypos;// bottom line of this foil's legend
                        if(abs(bottom) > abs(YPos))
                        {
                                ny++;
                        }
                        else if (k>0)
                        {
                        // move rigth if less than client bottom area
                                place.rx() += LegendWidth;
                                ny=1;
                        }
                        painter.setPen(TextPen);
                        painter.drawText(place.x() + (int)(0.5*LegendSize), place.y() + ypos*ny-(int)(ypos/2),
                                                         str.at(k));
                }
				for (nc=0; nc < g_tbemdataStore.size(); nc++)
                {
						if(str.at(k) == g_tbemdataStore.at(nc)->m_TurbineName)
                        {
								if (g_tbemdataStore.at(nc)->m_P.size() && g_tbemdataStore.at(nc)->m_TurbineName.length() && g_tbemdataStore.at(nc)->m_bIsVisible)

                                {
                                        //is there anything to draw ?
										LegendPen.setColor(g_tbemdataStore.at(nc)->m_Color);
										LegendPen.setStyle(GetStyle(g_tbemdataStore.at(nc)->m_Style));
										LegendPen.setWidth(g_tbemdataStore.at(nc)->m_Width);
                                        painter.setPen(LegendPen);

                                        painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
                                                                         place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
										if(g_tbemdataStore.at(nc)->m_bShowPoints)
                                        {
                                                x1 = place.x() + (int)(1.5*LegendSize);
                                                painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
                                        }

                                        painter.setPen(TextPen);
                                        painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
																		 g_tbemdataStore.at(nc)->m_SimName);
                                        ny++ ;
                                }
                        }
                }
                if (FoilPlrs) ny++;
        }

}

void QBEM::Paint360Legend(QPoint place, int bottom, QPainter &painter)
{
        int LegendSize, LegendWidth, ypos, x1;
        int i,j,k,l,nc,ny,nFoils;

        int x = place.x();
        int y = place.y();
        painter.setFont(QFont(g_mainFrame->m_TextFont.family(),20));

        if (!g_360PolarStore.size() && m_pCurPolar->m_Alpha.size()<4){
            painter.drawText(x+30,y+30,"Analyse the defined Polar in the ");
            painter.drawText(x+30,y+80,"XFOIL Direct Analysis Module");
        }
        else if (!g_360PolarStore.size()){
            painter.drawText(x+30,y+30,"Click 'New' to create ");
            painter.drawText(x+30,y+80,"a new 360 Polar");
        }

        LegendSize = 30;
        LegendWidth = 210;

        painter.setFont(g_mainFrame->m_TextFont);

        QFontMetrics fm(g_mainFrame->m_TextFont);
        ypos = fm.height();

        QPen TextPen(g_mainFrame->m_TextColor);
        painter.setPen(TextPen);
        TextPen.setWidth(1);

        QStringList str; // we need to make an inventory of foils

        for (j=0; j<g_polarStore.size(); j++)
        {

                for (i=0; i<g_360PolarStore.size(); i++)
                {
                        if (g_polarStore.at(j)->getName() == g_360PolarStore.at(i)->getParent()->getName() && g_360PolarStore.at(i)->m_Alpha.size())
                        {
                                str.append(QString(g_polarStore.at(j)->getParent()->getName()+" - "+g_polarStore.at(j)->getName()));
                                break;
                        }
                }// finished inventory
        }

        nFoils= str.size();

        painter.setBackgroundMode(Qt::TransparentMode);

        QPen LegendPen;
        LegendPen.setWidth(1);

        ny =0;
        for (k = 0; k<nFoils; k++)
        {
                int FoilPlrs = 0;
                for (l=0; l < g_360PolarStore.size(); l++)
                {
                        if (g_360PolarStore.at(l)->m_Alpha.size() &&
                                g_360PolarStore.at(l)->m_bIsVisible &&
                                QString(g_360PolarStore.at(l)->getParent()->getParent()->getName()+" - "+g_360PolarStore.at(l)->getParent()->getName()) == str.at(k))
                                        FoilPlrs++;
                }
                if (FoilPlrs)
                {
                        int YPos = place.y() + (ny+FoilPlrs+2) * ypos;// bottom line of this foil's legend
                        if(abs(bottom) > abs(YPos))
                        {
                                ny++;
                        }
                        else if (k>0)
                        {
                        // move rigth if less than client bottom area
                                place.rx() += LegendWidth;
                                ny=1;
                        }
                        painter.setPen(TextPen);
                        painter.drawText(place.x() + (int)(0.5*LegendSize), place.y() + ypos*ny-(int)(ypos/2),
                                                         str.at(k));
                }
                for (nc=0; nc < g_360PolarStore.size(); nc++)
                {
                        if(str.at(k) == QString(g_360PolarStore.at(nc)->getParent()->getParent()->getName()+" - "+g_360PolarStore.at(nc)->getParent()->getName()))
                        {
                                if (g_360PolarStore.at(nc)->m_Alpha.size() && g_360PolarStore.at(nc)->getParent()->getName().length() && g_360PolarStore.at(nc)->m_bIsVisible)

                                {
                                        //is there anything to draw ?
                                        LegendPen.setColor(g_360PolarStore.at(nc)->m_Color);
                                        LegendPen.setStyle(GetStyle(g_360PolarStore.at(nc)->m_Style));
                                        LegendPen.setWidth(g_360PolarStore.at(nc)->m_Width);
                                        painter.setPen(LegendPen);

                                        painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
                                                                         place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
                                        if(g_360PolarStore.at(nc)->m_bShowPoints)
                                        {
                                                x1 = place.x() + (int)(1.5*LegendSize);
                                                painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
                                        }

                                        painter.setPen(TextPen);
                                        painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
                                                                         g_360PolarStore.at(nc)->getName());
                                        ny++ ;
                                }
                        }
                }
                if (FoilPlrs) ny++;
        }

}


void QBEM::Paint360Graphs(QPainter &painter)
{

        if(!m_pCurGraph)
        {
                m_pCurGraph = &m_360Graph1;
        }

        int h  = m_rCltRect.height();
        int w  = m_rCltRect.width();
        int h3 = (int)(h/3);

        QRect Rect1(0,0,w,h3);
        QRect Rect2(0,h3,w,h3);
        QRect Rect3(0,2*h3,w,h3);

        QRect Single(0,0,w,h);

        if (m_bSingleGraphs)
        {
                m_360Graph1.DrawGraph(Single,painter);

        }
        else if(w>150 && h3>150)
        {
                m_360Graph1.DrawGraph(Rect1,painter);
                m_360Graph2.DrawGraph(Rect2,painter);
                m_360Graph3.DrawGraph(Rect3,painter);
        }

//        Paint360Legend(m_360LegendOffset, h, painter);

}

void QBEM::PaintRotorGraphs(QPainter &painter)
{

        if(!m_pCurGraph)
        {
                m_pCurGraph = &m_RotorGraph1;
//		return;
        }

        int h  = m_rCltRect.height();
        int w  = m_rCltRect.width();
        int w2 = (int)(w/2);

        QRect Rect1(0,0,w2,m_rCltRect.bottom()/2);
        QRect Rect2(w2,0,w2,m_rCltRect.bottom()/2);
        QRect Rect3(w2,m_rCltRect.bottom()/2,w2,m_rCltRect.bottom()/2);
        QRect Single(0,0,w,h);
        if (m_bSingleGraphs)
        {
                m_RotorGraph1.DrawGraph(Single,painter);

        }
        else if(w2>150 && h>150)
        {
                m_RotorGraph1.DrawGraph(Rect1,painter);
                m_RotorGraph2.DrawGraph(Rect2,painter);
                m_RotorGraph3.DrawGraph(Rect3,painter);
        }
        PaintCharLegend(m_CharLegendOffset, h, painter);

}

void QBEM::PaintCharacteristicsGraphs(QPainter &painter)
{

        if(!m_pCurGraph)
        {
                m_pCurGraph = &m_CharGraph1;
//		return;
        }

        int h  = m_rCltRect.height();
        int w  = m_rCltRect.width();
        int w2 = (int)(w/2);



        QRect Rect1(0,0,w2,m_rCltRect.bottom()/2);
        QRect Rect2(w2,0,w2,m_rCltRect.bottom()/2);
        QRect Rect3(w2,m_rCltRect.bottom()/2,w2,m_rCltRect.bottom()/2);
        QRect Rect4(0,m_rCltRect.bottom()/2,w2,m_rCltRect.bottom()/2);
        QRect Single(0,0,w,h);
        if (m_bSingleGraphs)
        {
                m_CharGraph1.DrawGraph(Single,painter);

        }
        else if(w2>150 && h>150)
        {
                m_CharGraph1.DrawGraph(Rect1,painter);
                m_CharGraph2.DrawGraph(Rect2,painter);
                m_CharGraph4.DrawGraph(Rect3,painter);
                m_CharGraph3.DrawGraph(Rect4,painter);
        }

}

void QBEM::PaintPowerGraphs(QPainter &painter)
{

        if(!m_pCurGraph)
        {
                m_pCurGraph = &m_PowerGraph1;
        }

        int h  = m_rCltRect.height();
        int w  = m_rCltRect.width();
        int w2 = (int)(w/2);



        QRect Rect1(0,0,w2,m_rCltRect.bottom()/2);
        QRect Rect2(w2,0,w2,m_rCltRect.bottom()/2);
        QRect Rect3(w2,m_rCltRect.bottom()/2,w2,m_rCltRect.bottom()/2);
        QRect Rect4(0,m_rCltRect.bottom()/2,w2,m_rCltRect.bottom()/2);
        QRect Single(0,0,w,h);

        if (m_bSingleGraphs)
        {
                m_PowerGraph1.DrawGraph(Single,painter);

        }
        else if(w2>150 && h>150)
        {
                m_PowerGraph1.DrawGraph(Rect1,painter);
                m_PowerGraph2.DrawGraph(Rect2,painter);
                m_PowerGraph3.DrawGraph(Rect3,painter);

        }

        PaintPowerLegend(m_PowerLegendOffset, h, painter);

}

void QBEM::PaintView(QPainter &painter)
{
        //Refresh the active view
        if (g_mainFrame->m_iView==POLARVIEW)// new JW, old: m_iView
        {
                Paint360Graphs(painter);
        }
        if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                PaintRotorGraphs(painter);
        }
        if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
                PaintPowerGraphs(painter);
        }
        if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
        {
                PaintCharacteristicsGraphs(painter);
        }
}


double QBEM::PlateFlow(double alphazero,double CLzero, double alpha)
{
    double res = (1+CLzero/sin(PI/4)*sin(alpha/360*2*PI))* CD90(m_pCurFoil, alpha) * sin((alpha-57.6*0.08*sin(alpha/360*2*PI) - alphazero*cos(alpha/360*2*PI))/360*2*PI) * cos((alpha-57.6*0.08*sin(alpha/360*2*PI) - alphazero*cos(alpha/360*2*PI))/360*2*PI);
    return res;
}

////// new code JW //////////
double QBEM::FlatPlateCL(double alpha, int pos)
{
    double res;
    res = m_pCur360Polar->m_Cd.at(pos) * sin(alpha/360*2*PI) * cos(alpha/360*2*PI);
    return res;
}

double QBEM::FlatPlateCD(double alpha, int pos)
{
    double res;
    res = m_pCur360Polar->m_Cd.at(pos) * pow(sin(alpha/360*2*PI),2);
    return res;
}
////// end new code JW //////


double QBEM::PotFlow(double CLzero, double slope, double alpha)
{
   double res;
   res = CLzero+slope*alpha;
   return res;
}


void QBEM::ReadParams(bool isVawt)
{
    if(!m_pBlade)
		return;
		
    m_pBlade->setName(m_pctrlWingName->text());  // this setName is ok, because m_pBlade ist not yet in the rotorStore
	    
	if(!m_bAdvancedEdit) {
		for (int i=0; i< m_pWingModel->rowCount();  i++) {
			ReadSectionData(i);
		}
	} else {
		for (int i=0; i< m_pBladeAxisModel->rowCount(); i++) {
			ReadAdvancedSectionData(i);
		}
	}

	m_bResetglGeom = true;
	m_bResetglSectionHighlight = true;
    m_pGLWidget->setSceneRadius(float(m_pBlade->getRotorRadius()*1.1)); //TEST DM scene needs to be captured if wind is made longer during construction
    ComputeGeometry(isVawt);
	UpdateView();
}

void QBEM::ReadAdvancedSectionData(int sel)
{

    if(sel>=m_pBladeAxisModel->rowCount()) return;
    double d;

    bool bOK;
    QString strong;
    QStandardItem *pItem;

    pItem = m_pBladeAxisModel->item(sel,0);

    strong =pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK)
    {
        if (m_bAbsoluteBlade)
        {
            m_pBlade->m_TRelPos[sel] =d / g_mainFrame->m_mtoUnit - m_pBlade->m_HubRadius;
            m_pBlade->m_TPos[sel] =d / g_mainFrame->m_mtoUnit;
        }
        else
        {
            m_pBlade->m_TRelPos[sel] =d / g_mainFrame->m_mtoUnit;
            m_pBlade->m_TPos[sel] =d / g_mainFrame->m_mtoUnit + m_pBlade->m_HubRadius;
        }
    }

    pItem = m_pBladeAxisModel->item(sel,1);
    strong =pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_pBlade->m_TOffsetX[sel] =d / g_mainFrame->m_mtoUnit;

    pItem = m_pBladeAxisModel->item(sel,2);
    strong =pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_pBlade->m_TOffsetZ[sel] =d / g_mainFrame->m_mtoUnit;

//    pItem = m_pBladeAxisModel->item(sel,1);
//    strong =pItem->text();
//    strong.replace(" ","");
//    d =strong.toDouble(&bOK);
//    if(bOK) m_pBlade->m_TFoilPAxisX[sel] = d;

//    pItem = m_pBladeAxisModel->item(sel,2);
//    strong =pItem->text();
//    strong.replace(" ","");
//    d =strong.toDouble(&bOK);
//    if(bOK) m_pBlade->m_TDihedral[sel] =d;

    pItem = m_pBladeAxisModel->item(sel,3);
    strong =pItem->text();
    strong.replace(" ","");
    d =strong.toDouble(&bOK);
    if(bOK) m_pBlade->m_TFoilPAxisX[sel] = d;

//    pItem = m_pBladeAxisModel->item(sel,3);
//    strong =pItem->text();
//    strong.replace(" ","");
//    d =strong.toDouble(&bOK);
//    if(bOK) m_pBlade->m_TFoilPAxisZ[sel] = d;
}

void QBEM::ReadSectionData(int sel)
{

        if(sel>=m_pWingModel->rowCount()) return;
        double d;

        bool bOK;
        QString strong;
        QStandardItem *pItem;

        pItem = m_pWingModel->item(sel,0);

        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK)
        {
            if (m_bAbsoluteBlade)
            {
                m_pBlade->m_TRelPos[sel] =d / g_mainFrame->m_mtoUnit - m_pBlade->m_HubRadius;
                m_pBlade->m_TPos[sel] =d / g_mainFrame->m_mtoUnit;
            }
            else
            {
                m_pBlade->m_TRelPos[sel] =d / g_mainFrame->m_mtoUnit;
                m_pBlade->m_TPos[sel] =d / g_mainFrame->m_mtoUnit + m_pBlade->m_HubRadius;
            }
        }


        pItem = m_pWingModel->item(sel,1);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TChord[sel] =d / g_mainFrame->m_mtoUnit;


        pItem = m_pWingModel->item(sel,2);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TTwist[sel] =d;


        pItem = m_pWingModel->item(sel,3);
        strong =pItem->text();
        m_pBlade->m_Airfoils[sel] = strong;


        QModelIndex ind;
        ind = m_pWingModel->index(sel, 4, QModelIndex());


        pItem = m_pWingModel->item(sel,4);
        strong =pItem->text();

            if (Get360Polar(m_pBlade->m_Airfoils[sel],strong))
            {
                if (Get360Polar(m_pBlade->m_Airfoils[sel],strong)->m_airfoil->getName() == m_pBlade->m_Airfoils[sel])
                {
                    m_pBlade->m_Polar.replace(sel,strong);
                }
                else
                {
                    m_pWingModel->setData(ind,"-----");
                    m_pBlade->m_Polar.replace(sel,"-----");
                    m_pBlade->m_Range.replace(sel,"-----");

                    for(int i=0; i< g_360PolarStore.size(); i++)
                    {
                            if (g_360PolarStore.at(i)->m_airfoil->getName() == m_pBlade->m_Airfoils[sel]){
                                m_pBlade->m_Polar.replace(sel, g_360PolarStore.at(i)->getName());
                                break;
                            }
                    }

                    for (int k=0;k<m_pBlade->m_PolarAssociatedFoils.size();k++){
                        if (m_pBlade->m_PolarAssociatedFoils.at(k) == m_pBlade->m_Airfoils.at(sel)) m_pBlade->m_Range.replace(sel,m_pBlade->m_MinMaxReynolds.at(k));
                    }

                    if (m_pBlade->m_bisSinglePolar) m_pWingModel->setData(ind, m_pBlade->m_Polar.at(sel));
                    else m_pWingModel->setData(ind, m_pBlade->m_Range.at(sel));
                }
            }
            else
            {
                m_pWingModel->setData(ind,"-----");
                m_pBlade->m_Polar.replace(sel,"-----");
                m_pBlade->m_Range.replace(sel,"-----");

                for(int i=0; i< g_360PolarStore.size(); i++)
                {
                        if (g_360PolarStore.at(i)->m_airfoil->getName() == m_pBlade->m_Airfoils[sel]){
                            m_pBlade->m_Polar.replace(sel, g_360PolarStore.at(i)->getName());
                            break;
                        }
                }

                for (int k=0;k<m_pBlade->m_PolarAssociatedFoils.size();k++){
                    if (m_pBlade->m_PolarAssociatedFoils.at(k) == m_pBlade->m_Airfoils.at(sel)) m_pBlade->m_Range.replace(sel,m_pBlade->m_MinMaxReynolds.at(k));
                }

                if (m_pBlade->m_bisSinglePolar) m_pWingModel->setData(ind, m_pBlade->m_Polar.at(sel));
                else m_pWingModel->setData(ind, m_pBlade->m_Range.at(sel));

            }


        m_pBlade->m_blades = m_pctrlBlades->value();

        CheckWing();
}

void QBEM::SnapClient(QString const &FileName)
{
    int NbBytes, bitsPerPixel;
    GLWidget * pGLWidget = (GLWidget*)m_pGLWidget;

    QRect m_r3DCltRect;

    m_r3DCltRect = pGLWidget->geometry();

    QSize size(m_r3DCltRect.width(),m_r3DCltRect.height());
    QGLFormat GLFormat = pGLWidget->format();

    if(!GLFormat.rgba())
    {
		QMessageBox::warning(g_mainFrame,tr("Warning"),tr("OpenGL color format is not recognized... Sorry"));
        return;
    }

    bitsPerPixel = 24;
    int width = size.width();
    switch(bitsPerPixel)
    {
        case 8:
        {
			QMessageBox::warning(g_mainFrame,tr("Warning"),tr("Cannot (yet ?) save 8 bit depth opengl screen images... Sorry"));
            return;
        }
        case 16:
        {
			QMessageBox::warning(g_mainFrame,tr("Warning"),tr("Cannot (yet ?) save 16 bit depth opengl screen images... Sorry"));
            size.setWidth(width - size.width() % 2);
            return;
        }
        case 24:
        {
            NbBytes = 4 * size.width() * size.height();//24 bits type BMP
//			size.setWidth(width - size.width() % 4);
            break;
        }
        case 32:
        {
            NbBytes = 4 * size.width() * size.height();//32 bits type BMP
            break;
        }
        default:
        {
			QMessageBox::warning(g_mainFrame,tr("Warning"),tr("Unidentified bit depth... Sorry"));
            return;
        }
    }
    uchar *pPixelData = new uchar[NbBytes];

    // Copy from OpenGL
    glReadBuffer(GL_FRONT);
    switch(bitsPerPixel)
    {
        case 8: return;
        case 16: return;
        case 24:
        {
#if QT_VERSION >= 0x040400
              glReadPixels(0,0,size.width(),size.height(),GL_RGB,GL_UNSIGNED_BYTE,pPixelData);
              QImage Image(pPixelData, size.width(),size.height(), QImage::Format_RGB888);
              QImage FlippedImaged;
              FlippedImaged = Image.mirrored();	//flip vertically
              FlippedImaged.save(FileName);
#else
			  QMessageBox::warning(g_mainFrame,tr("Warning"),"The version of Qt used to compile the code is older than 4.4 and does not support 24 bit images... Sorry");
#endif
              break;
        }
        case 32:
        {
            glReadPixels(0,0,size.width(),size.height(),GL_RGBA,GL_UNSIGNED_BYTE,pPixelData);
            QImage Image(pPixelData, size.width(),size.height(), QImage::Format_ARGB32);
            QImage FlippedImaged;
            FlippedImaged = Image.mirrored();	//flip vertically
            FlippedImaged.save(FileName);
            break;
        }
        default: break;
    }
}



void QBEM::SetCurveParams()
{

    SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;

		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if(m_pBEMData)
                {
                        if(m_pBEMData->m_bIsVisible)  pSimuWidget->m_pctrlShowCurve->setChecked(true);  else  pSimuWidget->m_pctrlShowCurve->setChecked(false);
                        if(m_pBEMData->m_bShowPoints) pSimuWidget->m_pctrlShowPoints->setChecked(true); else  pSimuWidget->m_pctrlShowPoints->setChecked(false);
                        if(m_bShowOpPoint) pSimuWidget->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidget->m_pctrlShowOpPoint->setChecked(false);


                        m_CurveColor = m_pBEMData->m_Color;
                        m_CurveStyle = m_pBEMData->m_Style;
                        m_CurveWidth = m_pBEMData->m_Width;
                        FillComboBoxes();
                }
                else
                {
                        FillComboBoxes(false);
                }
        }
		else if(g_mainFrame->m_iView == POLARVIEW)// new JW, old: m_iView
        {
                if(m_pCur360Polar)
                {
                        if(m_pCur360Polar->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
                        if(m_pCur360Polar->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);
                        m_CurveColor = m_pCur360Polar->m_Color;
                        m_CurveStyle = m_pCur360Polar->m_Style;
                        m_CurveWidth = m_pCur360Polar->m_Width;
                        FillComboBoxes();
                }
                else
                {
                        FillComboBoxes(false);
                }
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
                    if(m_pTBEMData)
                    {
                            if(m_pTBEMData->m_bIsVisible)  pSimuWidget->m_pctrlShowCurve->setChecked(true);  else  pSimuWidget->m_pctrlShowCurve->setChecked(false);
                            if(m_pTBEMData->m_bShowPoints) pSimuWidget->m_pctrlShowPoints->setChecked(true); else  pSimuWidget->m_pctrlShowPoints->setChecked(false);
                            if(m_bShowOpPoint) pSimuWidget->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidget->m_pctrlShowOpPoint->setChecked(false);

                            m_CurveColor = m_pTBEMData->m_Color;
                            m_CurveStyle = m_pTBEMData->m_Style;
                            m_CurveWidth = m_pTBEMData->m_Width;
                            FillComboBoxes();
                    }
                    else
                    {
                            FillComboBoxes(false);
                    }
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
        {
                    if(m_pCBEMData)
                    {
                            if(m_pCBEMData->m_bIsVisible)  pSimuWidget->m_pctrlShowCurve->setChecked(true);  else  pSimuWidget->m_pctrlShowCurve->setChecked(false);
                            if(m_pCBEMData->m_bShowPoints) pSimuWidget->m_pctrlShowPoints->setChecked(true); else  pSimuWidget->m_pctrlShowPoints->setChecked(false);
                            if(m_bShowOpPoint) pSimuWidget->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidget->m_pctrlShowOpPoint->setChecked(false);

                            m_CurveColor = m_pCBEMData->m_Color;
                            m_CurveStyle = m_pCBEMData->m_Style;
                            m_CurveWidth = m_pCBEMData->m_Width;
                            FillComboBoxes();
                    }
                    else
                    {
                            FillComboBoxes(false);
                    }
        }

}


void QBEM::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XBEM");
	{
		pSettings->setValue("Lambda", dlg_lambda);
		pSettings->setValue("Epsilon", dlg_epsilon);
		pSettings->setValue("Interations", dlg_iterations);
		pSettings->setValue("Elements", dlg_elements);
		pSettings->setValue("Rho", dlg_rho);
		pSettings->setValue("Relax", dlg_relax);
		pSettings->setValue("TipLoss", dlg_tiploss);
		pSettings->setValue("RootLoss", dlg_rootloss);
		pSettings->setValue("3DCorrection", dlg_3dcorrection);
		pSettings->setValue("Interpolation", dlg_interpolation);
		pSettings->setValue("lambdastart", dlg_lambdastart);
		pSettings->setValue("lambdaend", dlg_lambdaend);
		pSettings->setValue("lambdadelta", dlg_lambdadelta);
        pSettings->setValue("tsrwindspeed", dlg_windspeed);

		pSettings->setValue("windstart", dlg_windstart);
		pSettings->setValue("windend", dlg_windend);
		pSettings->setValue("winddelta", dlg_winddelta);
		pSettings->setValue("newtiploss", dlg_newtiploss);
		pSettings->setValue("newrootloss", dlg_newrootloss);
		pSettings->setValue("visc", dlg_visc);
		pSettings->setValue("pitchstart", dlg_pitchstart);
		pSettings->setValue("pitchend", dlg_pitchend);
		pSettings->setValue("pitchdelta", dlg_pitchdelta);
		pSettings->setValue("rotstart", dlg_rotstart);
		pSettings->setValue("rotend", dlg_rotend);
		pSettings->setValue("rotdelta", dlg_rotdelta);
		pSettings->setValue("windstartt", dlg_windstart2);
		pSettings->setValue("windendt", dlg_windend2);
		pSettings->setValue("winddeltat", dlg_winddelta2);
		pSettings->setValue("reynolds", dlg_reynolds);
	}
	pSettings->endGroup();
	
	m_GLLightDlg.SaveSettings(pSettings);
	
	m_RotorGraph1.SaveSettings(pSettings);
	m_RotorGraph2.SaveSettings(pSettings);
	m_RotorGraph3.SaveSettings(pSettings);
	m_PowerGraph1.SaveSettings(pSettings);
	m_PowerGraph2.SaveSettings(pSettings);
	m_PowerGraph3.SaveSettings(pSettings);
	m_CharGraph1.SaveSettings(pSettings);
	m_CharGraph2.SaveSettings(pSettings);
	m_CharGraph3.SaveSettings(pSettings);
	m_CharGraph4.SaveSettings(pSettings);
    m_360Graph1.SaveSettings(pSettings);
    m_360Graph2.SaveSettings(pSettings);
    m_360Graph3.SaveSettings(pSettings);
    m_360Graph4.SaveSettings(pSettings);
}

void QBEM::BladeCoordsChanged(){
    m_pctrlBladeCoordinates->setChecked(m_pctrlBladeCoordinates2->isChecked());

    if (m_pctrlBladeCoordinates2->isChecked())
    {
        m_pctrlHubRadiusLabel->setText(tr("Blade Hub Radius"));
        m_bAbsoluteBlade=false;
    }
    else
    {
        m_pctrlHubRadiusLabel->setText(tr("Innermost Station"));
        m_bAbsoluteBlade = true;
    }

    FillDataTable();


    UpdateView();
}


void QBEM::SetupLayout()     
{

	bladeWidget = new QStackedWidget;

    //--------------------Wing Table Layout--------------//

    QHBoxLayout *BottomTopLayout = new QHBoxLayout;

    QVBoxLayout *BottomLayout = new QVBoxLayout;
    QHBoxLayout *EditNewLayout = new QHBoxLayout;

    m_pctrlBladesAndHubLabel = new QLabel;
    m_pctrlBladeTableView = new QTableView;
    m_pctrlWingNameLabel = new QLabel;
    m_pctrlSingleMultiLabel = new QLabel;

    m_pctrlBladeCoordinates2 = new QCheckBox(tr("Blade Root Coordinates"));


    m_pctrlBladeTableView->setSelectionMode(QAbstractItemView::NoSelection);
	m_pctrlBladeTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//	m_pctrlBladeTableView->setFixedHeight(550);
//    m_pctrlBladeTableView->setFixedWidth(505);

    m_pctrlEditWing = new QPushButton(tr("Edit"));
    m_pctrlNewWing = new QPushButton(tr("New"));
    m_pctrlDeleteWing = new QPushButton(tr("Delete"));

    QHBoxLayout *SingleMultiLayout = new QHBoxLayout;

    SingleMultiLayout->addWidget(m_pctrlWingNameLabel);
    SingleMultiLayout->addWidget(m_pctrlSingleMultiLabel);

    BottomTopLayout->addWidget(m_pctrlBladesAndHubLabel);
    BottomTopLayout->addWidget(m_pctrlBladeCoordinates2);
    BottomLayout->addLayout(SingleMultiLayout);
    BottomLayout->addLayout(BottomTopLayout);
    BottomLayout->addWidget(m_pctrlBladeTableView);

    EditNewLayout->addWidget(m_pctrlNewWing);
    EditNewLayout->addWidget(m_pctrlEditWing);
    EditNewLayout->addWidget(m_pctrlDeleteWing);

    BottomLayout->addLayout(EditNewLayout);

    QGroupBox *WingDataBox = new QGroupBox(tr("Blade Data"));
    WingDataBox->setLayout(BottomLayout);


    //-----------Wing Edit Layout-------------//
    QVBoxLayout *EditTopLayout = new QVBoxLayout;
    QVBoxLayout *EditBottomLayout = new QVBoxLayout;
    QVBoxLayout *EditLayout = new QVBoxLayout;




    QHBoxLayout *InsertLayout = new QHBoxLayout;
    m_pctrlInsertBefore   = new QPushButton(tr("Insert Before"));
    m_pctrlInsertAfter    = new QPushButton(tr("Insert After"));
    m_pctrlDeleteSection  = new QPushButton(tr("Delete Section"));
    InsertLayout->addWidget(m_pctrlInsertBefore);
    InsertLayout->addWidget(m_pctrlInsertAfter);
    InsertLayout->addWidget(m_pctrlDeleteSection);


    QHBoxLayout *NameLayout = new QHBoxLayout;  
    m_pctrlWingName     = new QLineEdit(tr("Blade Name"));
    m_pctrlWingColor    = new ColorButton;
    m_pctrlSectionColor    = new ColorButton;
    m_pctrlSectionColor->SetColor(QColor(0,0,0));        

    NameLayout->addWidget(m_pctrlWingColor);
    NameLayout->addWidget(m_pctrlWingName);
    NameLayout->addWidget(m_pctrlSectionColor);

    m_SingleMultiGroup = new QButtonGroup(NameLayout);
    QRadioButton *radioButton = new QRadioButton ("Single Polar");
    NameLayout->addWidget(radioButton);
    m_SingleMultiGroup->addButton(radioButton,0);
    radioButton = new QRadioButton ("Multi Polar");
    NameLayout->addWidget(radioButton);
    m_SingleMultiGroup->addButton(radioButton,1);

    QHBoxLayout *ParamLayout = new QHBoxLayout;
    m_pctrlBlades = new QSpinBox;
    m_pctrlBladesLabel = new QLabel("Blade Number");
    m_pctrlHubRadius = new NumberEdit;
    m_pctrlHubRadiusLabel = new QLabel(tr("Hub Radius"));
    m_pctrlHubRadiusUnitLabel = new QLabel;

    m_pctrlBladeCoordinates = new QCheckBox(tr("Root Coordinates"));

    m_pctrlSolidityLabel = new QLabel;



    m_pctrlBlades->setMinimum(1);

    ParamLayout->addWidget(m_pctrlBladesLabel);
    ParamLayout->addWidget(m_pctrlBlades);
    ParamLayout->addWidget(m_pctrlHubRadiusLabel);
    ParamLayout->addWidget(m_pctrlHubRadius);
    ParamLayout->addWidget(m_pctrlHubRadiusUnitLabel);
    ParamLayout->addWidget(m_pctrlBladeCoordinates);



    m_pctrlBladeTable = new QTableView(this);
    m_pctrlBladeTable->setWindowTitle(QObject::tr("Blade definition"));
    m_pctrlBladeTable->setMinimumWidth(450);
    m_pctrlBladeTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pctrlBladeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pctrlBladeTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                              QAbstractItemView::DoubleClicked |
                                                                              QAbstractItemView::SelectedClicked |
                                                                              QAbstractItemView::EditKeyPressed);


    m_pctrlSave           = new QPushButton(tr("Save"));
    m_pctrlOptimize       = new QPushButton(tr("Optimize"));
	m_pctrlBack           = new QPushButton(tr("Cancel"));
    m_pctrlScale          = new QPushButton(tr("Scale"));
    m_pctrlPitchBlade     = new QDoubleSpinBox;
    m_pctrlPitchBlade->setMaximum(180);
    m_pctrlPitchBlade->setMinimum(-180);
    m_pctrlPitchBlade->setSingleStep(0.5);

    QHBoxLayout *OptScale = new QHBoxLayout;
    QHBoxLayout *BackSave = new QHBoxLayout;
    QHBoxLayout *PitchBlade = new QHBoxLayout;

    QLabel *pitchLabel = new QLabel(tr("Blade Pitch"));

    PitchBlade->addWidget(pitchLabel);
    PitchBlade->addWidget(m_pctrlPitchBlade);

    OptScale->addWidget(m_pctrlScale);
    OptScale->addLayout(PitchBlade);
    OptScale->addWidget(m_pctrlOptimize);
    QGroupBox *OptBox = new QGroupBox(tr("Modify Shape"));
    OptBox->setLayout(OptScale);

    BackSave->addWidget(m_pctrlBack);
    BackSave->addWidget(m_pctrlSave);

    EditTopLayout->addLayout(NameLayout);
    EditTopLayout->addLayout(ParamLayout);
    EditTopLayout->addWidget(m_pctrlSolidityLabel);
    EditTopLayout->addLayout(InsertLayout);
    EditLayout->addWidget(m_pctrlBladeTable);
    EditBottomLayout->addWidget(OptBox);
    EditBottomLayout->addLayout(BackSave);

    //---------------------Advanced Blade Design--------------------//


    m_pctrlBladeAxisTable = new QTableView(this);
    m_pctrlBladeAxisTable->setWindowTitle(QObject::tr("Advanced Blade definition"));
//    m_pctrlBladeAxisTable->setFixedWidth(450);
    m_pctrlBladeAxisTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pctrlBladeAxisTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pctrlBladeAxisTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                              QAbstractItemView::DoubleClicked |
                                                                              QAbstractItemView::SelectedClicked |
                                                                              QAbstractItemView::EditKeyPressed);
	m_pctrlAlignMaxThickness = new QPushButton(tr("Thread at Centerline & Max Thickness "));

    QVBoxLayout *AdvancedEditLayout = new QVBoxLayout;
    QHBoxLayout *BottomButtons = new QHBoxLayout;
	BottomButtons->addWidget(m_pctrlAlignMaxThickness);
//    BottomButtons->addWidget(m_pctrlIsOrtho); /// dont want this feature anymore, maybe reimplement in the future

    AdvancedEditLayout->addWidget(m_pctrlBladeAxisTable);
    AdvancedEditLayout->addLayout(BottomButtons);


    //---------------------3D View Layout ---------------------------//

    QGridLayout *ThreeDView = new QGridLayout;

//    int checkButtonWidth = 75;

    m_pctrlPerspective = new QPushButton(tr("Perspective"));
    m_pctrlPerspective->setCheckable(true);
//    m_pctrlPerspective->setFixedWidth(checkButtonWidth);
    m_pctrlPerspective->setFlat(true);
    m_pctrlShowTurbine = new QPushButton(tr("Show Rotor"));
    m_pctrlShowTurbine->setCheckable(true);
//    m_pctrlShowTurbine->setFixedWidth(checkButtonWidth);
    m_pctrlShowTurbine->setFlat(true);
    m_pctrlSurfaces = new QPushButton(tr("Surfaces"));
    m_pctrlSurfaces->setCheckable(true);
//    m_pctrlSurfaces->setFixedWidth(checkButtonWidth);
    m_pctrlSurfaces->setFlat(true);
    m_pctrlOutline = new QPushButton(tr("Foil Out"));
    m_pctrlOutline->setCheckable(true);
//    m_pctrlOutline->setFixedWidth(checkButtonWidth);
    m_pctrlOutline->setFlat(true);
    m_pctrlOutlineEdge = new QPushButton(tr("TE/LE Out"));
    m_pctrlOutlineEdge->setCheckable(true);
//    m_pctrlOutlineEdge->setFixedWidth(checkButtonWidth);
    m_pctrlOutlineEdge->setFlat(true);
    m_pctrlAirfoils = new QPushButton(tr("Fill Foils"));
    m_pctrlAirfoils->setCheckable(true);
//    m_pctrlAirfoils->setFixedWidth(checkButtonWidth);
    m_pctrlAirfoils->setFlat(true);
    m_pctrlAxes = new QPushButton(tr("Coordinates"));
    m_pctrlAxes->setCheckable(true);
//    m_pctrlAxes->setFixedWidth(checkButtonWidth);
    m_pctrlAxes->setFlat(true);
    m_pctrlPositions = new QPushButton(tr("Foil Positions"));
    m_pctrlPositions->setCheckable(true);
//    m_pctrlPositions->setFixedWidth(checkButtonWidth);
    m_pctrlPositions->setFlat(true);
    m_pctrlFoilNames = new QPushButton(tr("Foil Names"));
    m_pctrlFoilNames->setCheckable(true);
//    m_pctrlFoilNames->setFixedWidth(checkButtonWidth);
    m_pctrlFoilNames->setFlat(true);

    m_pctrlLightDlg = new QPushButton(tr("GL settings"));
    m_pctrlResetView = new QPushButton(tr("Fit to Screen"));

    connect(m_pctrlLightDlg, SIGNAL(clicked()), this, SLOT(OnLightDlg()));
    connect(m_pctrlResetView, SIGNAL(clicked()), this, SLOT(OnCenterScene()));


    m_pctrlSurfaces->setChecked(true);
    m_pctrlOutline->setChecked(true);
    m_pctrlOutlineEdge->setChecked(true);
    m_pctrlAirfoils->setChecked(false);
    m_pctrlPositions->setChecked(false);
    m_pctrlFoilNames->setChecked(false);

    ThreeDView->addWidget(m_pctrlResetView,1,1);
    ThreeDView->addWidget(m_pctrlShowTurbine,1,2);
    ThreeDView->addWidget(m_pctrlSurfaces,1,3);
    ThreeDView->addWidget(m_pctrlOutline,1,4);
    ThreeDView->addWidget(m_pctrlOutlineEdge,1,5);
    ThreeDView->addWidget(m_pctrlAirfoils,1,6);
    ThreeDView->addWidget(m_pctrlLightDlg,2,1);
    ThreeDView->addWidget(m_pctrlPerspective,2,2);
    ThreeDView->addWidget(m_pctrlAxes,2,3);
    ThreeDView->addWidget(m_pctrlPositions,2,4);
    ThreeDView->addWidget(m_pctrlFoilNames,2,5);




    QVBoxLayout *ViewLayout = new QVBoxLayout;
    ViewLayout->addLayout(ThreeDView);

    QGroupBox *ViewControl = new QGroupBox(tr("3D View Controls"));
    ViewControl->setLayout(ViewLayout);

    QHBoxLayout *ViewLayout2 = new QHBoxLayout;
    ViewLayout2->addWidget(ViewControl);




    //--------------------360 Polar Layout--------------------//

            QVBoxLayout *SaveDelete = new QVBoxLayout;
            QGridLayout *Sliders = new QGridLayout;
            QVBoxLayout *Layout360 = new QVBoxLayout;
            QVBoxLayout *DecompBox = new QVBoxLayout;
            QGridLayout *Decomposers = new QGridLayout;

            QGridLayout *ARViterna = new QGridLayout;
            QGridLayout *MaxMinRange = new QGridLayout;




            ////////////////// new code JW /////////////////////////////
            QHBoxLayout *StallModel = new QHBoxLayout;
            m_pctrlStallModelMontg = new QRadioButton(tr("Montgomerie"));
            m_pctrlStallModelMontg->setChecked(!m_bStallModel);
            m_pctrlStallModelVit = new QRadioButton(tr("Viterna"));
            m_pctrlStallModelVit->setChecked(m_bStallModel);
            ////////////////// end new code JW /////////////////////////

            m_pctrlSave360 = new QPushButton(tr("Save"));
            m_pctrlNew360 = new QPushButton(tr("Extrapolate"));
			m_pctrlCancel360 = new QPushButton(tr("Cancel"));
            m_pctrlDelete360Polar = new QPushButton(tr("Delete"));
            m_pctrlDecompose360 = new QPushButton(tr("Decompose"));
            m_pctrlRename360Polar = new QPushButton(tr("Rename"));


            DecompBox->addLayout(Decomposers);


            m_360Name = new QLineEdit;
            IsDecomposed = new QLabel("");

            m_LabelA = new QLabel(tr("A+"));

            m_pctrlA = new QSlider(Qt::Horizontal);
            m_pctrlA->setMinimum(1);
            m_pctrlA->setMaximum(1);
            m_pctrlA->setValue(0);

            m_LabelB = new QLabel(tr("B+"));

            m_pctrlB = new QSlider(Qt::Horizontal);
            m_pctrlB->setMinimum(1);
            m_pctrlB->setMaximum(100);
            m_pctrlB->setValue(5);

            m_LabelAm = new QLabel(tr("A-"));

            m_pctrlAm = new QSlider(Qt::Horizontal);
            m_pctrlAm->setMinimum(1);
            m_pctrlAm->setMaximum(30);
            m_pctrlAm->setValue(15);

            m_LabelBm = new QLabel(tr("B-"));

            m_pctrlBm = new QSlider(Qt::Horizontal);
            m_pctrlBm->setMinimum(1);
            m_pctrlBm->setMaximum(70);
            m_pctrlBm->setValue(5);

            m_pos180Sep = new QDoubleSpinBox;
            m_pos180Sep->setMinimum(160);
            m_pos180Sep->setMaximum(175);
            m_pos180Sep->setSingleStep(1);
            m_pos180Sep->setValue(173);
            m_pos180Sep->setDecimals(0);

            m_pos180Stall = new QDoubleSpinBox;
            m_pos180Stall->setMinimum(100);
            m_pos180Stall->setMaximum(170);
            m_pos180Stall->setSingleStep(1);
            m_pos180Stall->setValue(160);
            m_pos180Stall->setDecimals(0);

            m_neg180Sep = new QDoubleSpinBox;
            m_neg180Sep->setMinimum(-175);
            m_neg180Sep->setMaximum(-160);
            m_neg180Sep->setSingleStep(1);
            m_neg180Sep->setValue(-173);
            m_neg180Sep->setDecimals(0);

            m_neg180Stall = new QDoubleSpinBox;
            m_neg180Stall->setMinimum(-170);
            m_neg180Stall->setMaximum(-100);
            m_neg180Stall->setSingleStep(1);
            m_neg180Stall->setValue(-160);
            m_neg180Stall->setDecimals(0);

            m_posStall = new QDoubleSpinBox;
            m_posStall->setMinimum(2);
            m_posStall->setMaximum(80);
            m_posStall->setSingleStep(1);
            m_posStall->setValue(20);
            m_posStall->setDecimals(0);

            m_posSep = new QDoubleSpinBox;
            m_posSep->setMinimum(1);
            m_posSep->setMaximum(20);
            m_posSep->setSingleStep(1);
            m_posSep->setValue(7);
            m_posSep->setDecimals(0);

            m_negStall = new QDoubleSpinBox;
            m_negStall->setMinimum(-80);
            m_negStall->setMaximum(-2);
            m_negStall->setSingleStep(1);
            m_negStall->setValue(-20);
            m_negStall->setDecimals(0);

            m_negSep = new QDoubleSpinBox;
            m_negSep->setMinimum(-20);
            m_negSep->setMaximum(-1);
            m_negSep->setSingleStep(1);
            m_negSep->setValue(-7);
            m_negSep->setDecimals(0);

            QHBoxLayout *savecancellayout = new QHBoxLayout;
            savecancellayout->addWidget(m_pctrlSave360);
            savecancellayout->addWidget(m_pctrlCancel360);

            QGridLayout *NewDeleteRenameLayout = new QGridLayout;
            NewDeleteRenameLayout->addWidget(m_pctrlRename360Polar,0,0);
            NewDeleteRenameLayout->addWidget(m_pctrlDecompose360,0,1);
            NewDeleteRenameLayout->addWidget(m_pctrlDelete360Polar,1,0);
            NewDeleteRenameLayout->addWidget(m_pctrlNew360,1,1);


            /////// new code JW //////////
            StallModel->addWidget(m_pctrlStallModelMontg);
            StallModel->addWidget(m_pctrlStallModelVit);
            SaveDelete->addLayout(StallModel);
            /////// end new code JW //////
            SaveDelete->addLayout(NewDeleteRenameLayout);
            SaveDelete->addLayout(savecancellayout);


            m_pctrlCD90 = new QDoubleSpinBox;
            m_pctrlCD90Label = new QLabel(tr("CD 90"));
            m_pctrlCD90->setMinimum(0.2);
            m_pctrlCD90->setMaximum(3);
            m_pctrlCD90->setSingleStep(0.01);
            m_pctrlCD90->setValue(m_CD90);

            m_pctrlARLabel = new QLabel(tr("CD 90"));
            m_pctrlAR = new QDoubleSpinBox;
            m_pctrlAR->setMinimum(0.2);
            m_pctrlAR->setSingleStep(0.01);
            m_pctrlAR->setValue(m_AR);

            QLabel *slop = new QLabel(tr("Slope"));
            m_Slope = new QDoubleSpinBox;
            m_Slope->setDecimals(3);
            m_Slope->setSingleStep(0.001);


            QLabel *posA = new QLabel(tr("max"));
            m_posAoA = new QDoubleSpinBox;
            m_posAoA->setSingleStep(0.5);
            m_posAoA->setRange(0,180);
            QLabel *negA = new QLabel(tr("min"));
            m_negAoA = new QDoubleSpinBox;
            m_negAoA->setRange(-180,0);
            m_negAoA->setSingleStep(0.5);

            MaxMinRange->addWidget(negA,1,1);
            MaxMinRange->addWidget(m_negAoA,1,2);
            MaxMinRange->addWidget(posA,1,3);
            MaxMinRange->addWidget(m_posAoA,1,4);



            Sliders->addWidget(m_LabelA,1,1);
            Sliders->addWidget(m_pctrlA,1,2);
            Sliders->addWidget(m_LabelB,2,1);
            Sliders->addWidget(m_pctrlB,2,2);
            Sliders->addWidget(m_LabelAm,3,1);
            Sliders->addWidget(m_pctrlAm,3,2);
            Sliders->addWidget(m_LabelBm,4,1);
            Sliders->addWidget(m_pctrlBm,4,2);
            Sliders->addWidget(slop,5,1);
            Sliders->addWidget(m_Slope,5,2);
            Sliders->addWidget(m_pctrlCD90Label,6,1);
            Sliders->addWidget(m_pctrlCD90,6,2);


            ARViterna->addWidget(m_pctrlARLabel,1,1);
            ARViterna->addWidget(m_pctrlAR,1,2);

            QLabel *lab;

            int gridrow = 1;
            lab = new QLabel(tr("Se+"));
            Decomposers->addWidget(lab,gridrow,1);
            Decomposers->addWidget(m_posSep,gridrow,2);
            lab = new QLabel(tr("St+"));
            Decomposers->addWidget(lab,gridrow,3);
            Decomposers->addWidget(m_posStall,gridrow,4);
            gridrow++;
            lab = new QLabel(tr("Se-"));
            Decomposers->addWidget(lab,gridrow,1);
            Decomposers->addWidget(m_negSep,gridrow,2);
            lab = new QLabel(tr("St-"));
            Decomposers->addWidget(lab,gridrow,3);
            Decomposers->addWidget(m_negStall,gridrow,4);
            gridrow++;
            lab = new QLabel(tr("BSe+"));
            Decomposers->addWidget(lab,gridrow,1);
            Decomposers->addWidget(m_pos180Sep,gridrow,2);
            lab = new QLabel(tr("BSt+"));
            Decomposers->addWidget(lab,gridrow,3);
            Decomposers->addWidget(m_pos180Stall,gridrow,4);
            gridrow++;
            lab = new QLabel(tr("BSe-"));
            Decomposers->addWidget(lab,gridrow,1);
            Decomposers->addWidget(m_neg180Sep,gridrow,2);
            lab = new QLabel(tr("BSt-"));
            Decomposers->addWidget(lab,gridrow,3);
            Decomposers->addWidget(m_neg180Stall,gridrow,4);
            gridrow++;

            /// \brief SaveDeleteGroup
            ///

            QGroupBox *SaveDeleteGroup = new QGroupBox(tr("Create 360 Polar"));
            SliderGroup = new QGroupBox(tr("Finetuning of Polar"));
            RangeGroup = new QGroupBox(tr("Range of original polar"));

            ViternaGroup = new QGroupBox(tr("Finetuning of Polar"));

            DecomposeGroup = new QGroupBox(tr("Finetuning of Decomposition"));

            ///////////////////curve style

            QHBoxLayout *CurveDisplay = new QHBoxLayout;
            m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
            m_pctrlShowPoints = new QCheckBox(tr("Points"));
            CurveDisplay->addWidget(m_pctrlShowCurve);
            CurveDisplay->addWidget(m_pctrlShowPoints);

            QVBoxLayout *PolarDisplay = new QVBoxLayout;

            m_pctrlCurveStyle = new LineCbBox();
            m_pctrlCurveWidth = new LineCbBox();
            m_pctrlCurveColor = new LineButton;
            for (int i=0; i<5; i++)
            {
                    m_pctrlCurveStyle->addItem("item");
                    m_pctrlCurveWidth->addItem("item");
            }
            m_pStyleDelegate = new LineDelegate;
            m_pWidthDelegate = new LineDelegate;
            m_pctrlCurveStyle->setItemDelegate(m_pStyleDelegate);
            m_pctrlCurveWidth->setItemDelegate(m_pWidthDelegate);

            QGridLayout *CurveStyleLayout = new QGridLayout;
            QLabel *lab200 = new QLabel(tr("Style"));
            QLabel *lab201 = new QLabel(tr("Width"));
            QLabel *lab202 = new QLabel(tr("Color"));
            lab200->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
            lab201->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
            lab202->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
            CurveStyleLayout->addWidget(lab200,1,1);
            CurveStyleLayout->addWidget(lab201,2,1);
            CurveStyleLayout->addWidget(lab202,3,1);
            CurveStyleLayout->addWidget(m_pctrlCurveStyle,1,2);
            CurveStyleLayout->addWidget(m_pctrlCurveWidth,2,2);
            CurveStyleLayout->addWidget(m_pctrlCurveColor,3,2);
			CurveStyleLayout->setColumnStretch(2,5);

            /////////////

            SaveDeleteGroup->setLayout(SaveDelete);
            SliderGroup->setLayout(Sliders);
            ViternaGroup->setLayout(ARViterna);
            RangeGroup->setLayout(MaxMinRange);

            DecomposeGroup->setLayout(DecompBox);

            PolarDisplay->addLayout(CurveDisplay);
            PolarDisplay->addLayout(CurveStyleLayout);


            Layout360->addWidget(m_360Name);
            Layout360->addWidget(IsDecomposed);
            Layout360->addWidget(SaveDeleteGroup);
            Layout360->addWidget(RangeGroup);
            Layout360->addWidget(SliderGroup);
            Layout360->addWidget(ViternaGroup);
            Layout360->addWidget(DecomposeGroup);
            Layout360->addLayout(PolarDisplay);
			Layout360->addStretch(0);

//---------------Turbine Edit Layout------------------//

    m_pctrlTypeLabel = new QLabel(tr("Power Regulation"));
    m_pctrlGeneratorTypeLabel = new QLabel(tr("Transmission"));
    m_pctrlStall = new QRadioButton(tr("None (Stall)"));
    m_pctrlPitch = new QRadioButton(tr("Pitch limited"));
    m_pctrlFixed = new QRadioButton(tr("Single"));
    m_pctrl2Step = new QRadioButton(tr("2 Step"));
    m_pctrlPrescribedPitch = new QRadioButton(tr("Prescribed"));
    m_pctrlPrescribedRot = new QRadioButton(tr("Prescribed"));
    m_pctrlVariable = new QRadioButton(tr("Optimal"));
    m_pctrlFixedPitch = new NumberEdit;
    m_pctrlFixedPitch->setMinimum(-180);
    m_pctrlFixedPitch->setMaximum(180);
    m_pctrlFixedPitch->setAutomaticPrecision(1);
    m_pctrlFixedPitch->setValue(0);
    m_pctrlFixedPitchLabel = new QLabel(tr("Fixed Pitch"));
    m_pctrlFixedPitchUnit = new QLabel(tr("deg"));

    m_pctrlPitchList = new QPushButton(tr("Prescribe Pitch Angles"));
    m_pctrlRotList = new QPushButton(tr("Prescribe Rot. Speed"));





    m_pctrlCutIn = new NumberEdit;
    m_pctrlCutIn->setValue(0);
    m_pctrlCutInLabel = new QLabel(tr("V Cut In"));
    m_pctrlSwitch = new NumberEdit;
    m_pctrlSwitch->setValue(0);
    m_pctrlSwitchLabel = new QLabel(tr("V Switch"));
    m_pctrlCutOut = new NumberEdit;
    m_pctrlCutOut->setValue(0);
    m_pctrlCutOutLabel = new QLabel(tr("V Cut Out"));

    m_pctrlVariableLossesLabel = new QLabel(tr("Loss Factor"));
    m_pctrlVariableLossesUnit = new QLabel(tr("0-1"));

    m_pctrlFixedLossesLabel = new QLabel(tr("Fixed Losses"));

    m_pctrlVariableLosses = new NumberEdit;
    m_pctrlFixedLosses = new NumberEdit;
    m_pctrlFixedLosses->setValue(0);
    m_pctrlVariableLosses->setMinimum(0);
    m_pctrlVariableLosses->setMaximum(1);
    m_pctrlVariableLosses->setAutomaticPrecision(3);
    m_pctrlVariableLosses->setValue(0);

    speed1 = new QLabel;
    speed2 = new QLabel;
    speed3 = new QLabel;
    rotspeed1 = new QLabel(tr("rpm"));
    rotspeed2 = new QLabel(tr("rpm"));
    power1 = new QLabel();
    power2 = new QLabel();








    m_pctrlRot1  = new NumberEdit;
    m_pctrlRot1->setValue(0.00001);
    m_pctrlRot1->setMinimum(0.00001);
    m_pctrlRot1Label = new QLabel(tr("Rot. Speed Min"));
    m_pctrlRot2  = new NumberEdit;
    m_pctrlRot2->setValue(0.00001);
    m_pctrlRot2->setMinimum(0.00001);
    m_pctrlRot2Label = new QLabel(tr("Rot. Speed Max"));

    m_pctrlLambda = new NumberEdit;
    m_pctrlLambdaLabel = new QLabel(tr("TSR at Design Point"));
    m_pctrlLambda->setValue(0);

    m_pctrlGenerator = new NumberEdit;
    m_pctrlGeneratorLabel = new QLabel(tr("Generator Capacity"));
    m_pctrlGenerator->setValue(0);

    m_pctrlSaveTurbine = new QPushButton(tr("Save"));
	m_pctrlDiscardTurbine = new QPushButton(tr("Cancel"));

    QHBoxLayout *SaveOrBackLayout = new QHBoxLayout;
    SaveOrBackLayout->addWidget(m_pctrlDiscardTurbine);
    SaveOrBackLayout->addWidget(m_pctrlSaveTurbine);

    QButtonGroup *Group1 = new QButtonGroup;
    QButtonGroup *Group2 = new QButtonGroup;

    Group1->addButton(m_pctrlStall);
    Group1->addButton(m_pctrlPitch);
    Group1->addButton(m_pctrlPrescribedPitch);

    Group2->addButton(m_pctrlFixed);
    Group2->addButton(m_pctrl2Step);
    Group2->addButton(m_pctrlVariable);
    Group2->addButton(m_pctrlPrescribedRot);


    QGridLayout *TypeLayout = new QGridLayout;
    TypeLayout->addWidget(m_pctrlTypeLabel,1,1);
    TypeLayout->addWidget(m_pctrlStall,2,1);
    TypeLayout->addWidget(m_pctrlPitch,2,2);
    TypeLayout->addWidget(m_pctrlPrescribedPitch,3,1);

    TypeLayout->addWidget(m_pctrlGeneratorTypeLabel,4,1);
    TypeLayout->addWidget(m_pctrlFixed,5,1);
    TypeLayout->addWidget(m_pctrl2Step,5,2);
    TypeLayout->addWidget(m_pctrlVariable,5,3);
    TypeLayout->addWidget(m_pctrlPrescribedRot,6,1);

    QGroupBox *TypeGroup = new QGroupBox(tr("Turbine Type"));
    TypeGroup->setLayout(TypeLayout);

    QGridLayout *SpeciLayout = new QGridLayout;
    SpeciLayout->addWidget(m_pctrlGeneratorLabel,1,1);
    SpeciLayout->addWidget(m_pctrlGenerator,1,2);
    SpeciLayout->addWidget(power1,1,3);
    SpeciLayout->addWidget(m_pctrlCutInLabel,2,1);
    SpeciLayout->addWidget(m_pctrlCutIn,2,2);
    SpeciLayout->addWidget(speed1,2,3);
    SpeciLayout->addWidget(m_pctrlSwitchLabel,3,1);
    SpeciLayout->addWidget(m_pctrlSwitch,3,2);
    SpeciLayout->addWidget(speed3,3,3);
    SpeciLayout->addWidget(m_pctrlCutOutLabel,4,1);
    SpeciLayout->addWidget(m_pctrlCutOut,4,2);
    SpeciLayout->addWidget(speed2,4,3);
    SpeciLayout->addWidget(m_pctrlRot1Label,5,1);
    SpeciLayout->addWidget(m_pctrlRot1,5,2);
    SpeciLayout->addWidget(rotspeed1,5,3);
    SpeciLayout->addWidget(m_pctrlRot2Label,6,1);
    SpeciLayout->addWidget(m_pctrlRot2,6,2);
    SpeciLayout->addWidget(rotspeed2,6,3);
    SpeciLayout->addWidget(m_pctrlLambdaLabel,7,1);
    SpeciLayout->addWidget(m_pctrlLambda,7,2);
    SpeciLayout->addWidget(m_pctrlFixedPitch,8,2);
    SpeciLayout->addWidget(m_pctrlFixedPitchLabel,8,1);
    SpeciLayout->addWidget(m_pctrlFixedPitchUnit,8,3);
    SpeciLayout->addWidget(m_pctrlPitchList,9,2);
    SpeciLayout->addWidget(m_pctrlRotList,10,2);
    SpeciLayout->addWidget(m_pctrlVariableLossesLabel,11,1);
    SpeciLayout->addWidget(m_pctrlVariableLossesUnit,11,3);
    SpeciLayout->addWidget(m_pctrlFixedLossesLabel,12,1);
    SpeciLayout->addWidget(m_pctrlVariableLosses,11,2);
    SpeciLayout->addWidget(m_pctrlFixedLosses,12,2);
    SpeciLayout->addWidget(power2,12,3);

    QGroupBox *SpeciGroup = new QGroupBox(tr("Turbine Specification"));
    SpeciGroup->setLayout(SpeciLayout);

    WingSelection = new QComboBox;
    QGridLayout *WingLayout = new QGridLayout;
    WingLayout->addWidget(WingSelection);
    QGroupBox *WingGroup = new QGroupBox(tr("Turbine Blade"));
    WingGroup->setLayout(WingLayout);

    m_pctrlTurbineName = new QLineEdit;
    QGridLayout *TNameLayout = new QGridLayout;
    TNameLayout->addWidget(m_pctrlTurbineName);
    QGroupBox *TNameGroup = new QGroupBox(tr("Turbine Name"));
    TNameGroup->setLayout(TNameLayout);







    //----------------Turbine Show Layout----------//
    TypeLabel = new QLabel(tr("Power Regulation"));
    GeneratorTypeLabel = new QLabel(tr("Transmission"));
    CutInLabel = new QLabel(tr("V Cut In"));
    SwitchLabel = new QLabel(tr("V Switch"));
    CutOutLabel = new QLabel(tr("V Cut Out"));
    Rot1Label = new QLabel(tr("Rotational Speed Min"));
    Rot2Label = new QLabel(tr("Rotational Speed Max"));
    LambdaLabel = new QLabel(tr("TSR at Design Point"));
    GeneratorLabel = new QLabel(tr("Pitch from Power"));

    FixedLossesLabel = new QLabel(tr("Fixed Losses"));
    VariableLossesLabel = new QLabel(tr("VariableLosses"));
    FixedPitchLabel = new QLabel(tr("Fixed Pitch"));



    OuterRadiusLabel = new QLabel(tr("Outer Radius"));

    Speed1 = new QLabel;
    Speed2 = new QLabel;
    Speed3 = new QLabel;
    Rotspeed1 = new QLabel(tr("rpm"));
    Rotspeed2 = new QLabel(tr("rpm"));
    Length1 = new QLabel;
    Power1 = new QLabel;
    Power2 = new QLabel;







    Type = new QLabel;
    Type->setAlignment(Qt::AlignRight);
    Trans = new QLabel;
    Trans->setAlignment(Qt::AlignRight);
    Capacity= new QLabel;
    Capacity->setAlignment(Qt::AlignRight);
    Rot1 = new QLabel;
    Rot1->setAlignment(Qt::AlignRight);
    Rot2 = new QLabel;
    Rot2->setAlignment(Qt::AlignRight);
    Lambda0 = new QLabel;
    Lambda0->setAlignment(Qt::AlignRight);
    CutIn = new QLabel;
    CutIn->setAlignment(Qt::AlignRight);
    CutOut = new QLabel;
    CutOut->setAlignment(Qt::AlignRight);
    Switch = new QLabel;
    Switch->setAlignment(Qt::AlignRight);
    Generator = new QLabel;
    Generator->setAlignment(Qt::AlignRight);
    Blade = new QLabel;
    Blade->setAlignment(Qt::AlignRight);
    OuterRadius = new QLabel;
    OuterRadius->setAlignment(Qt::AlignRight);
    BladeLabel = new QLabel(tr("Turbine Blade"));

    FixedLosses = new QLabel;
    FixedLosses->setAlignment(Qt::AlignRight);
    VariableLosses = new QLabel;
    VariableLosses->setAlignment(Qt::AlignRight);
    FixedPitch = new QLabel;
    FixedPitch->setAlignment(Qt::AlignRight);




    QGridLayout *TurbineDataLayout = new QGridLayout;

    TurbineDataLayout->addWidget(TypeLabel,1,1);
    TurbineDataLayout->addWidget(Type,1,2);
    TurbineDataLayout->addWidget(GeneratorTypeLabel,2,1);
    TurbineDataLayout->addWidget(Trans,2,2);
    TurbineDataLayout->addWidget(GeneratorLabel,3,1);
    TurbineDataLayout->addWidget(Generator,3,2);
    TurbineDataLayout->addWidget(Power1,3,3);

    TurbineDataLayout->addWidget(CutInLabel,4,1);
    TurbineDataLayout->addWidget(CutIn,4,2);
    TurbineDataLayout->addWidget(Speed1,4,3);
    TurbineDataLayout->addWidget(CutOutLabel,5,1);
    TurbineDataLayout->addWidget(CutOut,5,2);
    TurbineDataLayout->addWidget(Speed2,5,3);
    TurbineDataLayout->addWidget(SwitchLabel,6,1);
    TurbineDataLayout->addWidget(Switch,6,2);
    TurbineDataLayout->addWidget(Speed3,6,3);

    TurbineDataLayout->addWidget(Rot1Label,7,1);
    TurbineDataLayout->addWidget(Rot1,7,2);
    TurbineDataLayout->addWidget(Rotspeed1,7,3);
    TurbineDataLayout->addWidget(Rot2Label,8,1);
    TurbineDataLayout->addWidget(Rot2,8,2);
    TurbineDataLayout->addWidget(Rotspeed2,8,3);
    TurbineDataLayout->addWidget(LambdaLabel,9,1);
    TurbineDataLayout->addWidget(Lambda0,9,2);

    TurbineDataLayout->addWidget(BladeLabel,11,1);
    TurbineDataLayout->addWidget(Blade,11,2);
    TurbineDataLayout->addWidget(OuterRadiusLabel,12,1);
    TurbineDataLayout->addWidget(OuterRadius,12,2);
    TurbineDataLayout->addWidget(Length1,12,3);
    TurbineDataLayout->addWidget(FixedPitchLabel,13,1);
    TurbineDataLayout->addWidget(FixedPitch,13,2);
    TurbineDataLayout->addWidget(VariableLossesLabel,14,1);
    TurbineDataLayout->addWidget(FixedLossesLabel,15,1);
    TurbineDataLayout->addWidget(Power2,15,3);
    TurbineDataLayout->addWidget(VariableLosses,14,2);
    TurbineDataLayout->addWidget(FixedLosses,15,2);

    QGroupBox *TurbineDataGroup = new QGroupBox(tr("Turbine Data"));
    TurbineDataGroup->setLayout(TurbineDataLayout);

    QGridLayout *SDLayout = new QGridLayout;
    m_pctrlNewTurbine = new QPushButton("New");
    m_pctrlEditTurbine = new QPushButton(tr("Edit"));
    m_pctrlDeleteTurbine = new QPushButton(tr("Delete"));
    SDLayout->addWidget(m_pctrlNewTurbine,1,1);
    SDLayout->addWidget(m_pctrlEditTurbine,1,2);
    SDLayout->addWidget(m_pctrlDeleteTurbine,1,3);
    QGroupBox *SDGroup = new QGroupBox(tr("New/Edit/Delete Turbine"));
    SDGroup->setLayout(SDLayout);



	QGridLayout *WeibullLayout = new QGridLayout;
	m_pctrlWk = new QDoubleSpinBox;
	m_pctrlWk->setMinimum(0.01);
	m_pctrlWk->setSingleStep(0.05);
	m_pctrlWk->setValue(2);// JW modification
	m_pctrlWkLabel = new QLabel(tr("k"));
	m_pctrlWA = new QDoubleSpinBox;
	m_pctrlWA->setMinimum(0.01);
	m_pctrlWA->setSingleStep(0.05);
	m_pctrlWA->setValue(9);// JW modification
	m_pctrlWALabel = new QLabel(tr("A"));
	// new code JW //
	m_pctrlPMk = new QDoubleSpinBox;
	m_pctrlPMk->setMinimum(0.01);
	m_pctrlPMk->setSingleStep(0.05);
	m_pctrlPMk->setValue(3);
	m_pctrlPMkLabel = new QLabel(tr("+-"));
	m_pctrlPMA = new QDoubleSpinBox;
	m_pctrlPMA->setMinimum(0.01);
	m_pctrlPMA->setSingleStep(0.05);
	m_pctrlPMA->setValue(3);
	m_pctrlPMALabel = new QLabel(tr("+-"));
	// end new code JW
	m_pctrlYield = new QLabel;
	m_pctrlYield->setText("");// JW modification
	m_pctrlYieldLabel = new QLabel(tr("Annual Yield"));
	POwer = new QLabel;
	WeibullLayout->addWidget(m_pctrlWkLabel,1,2);
	WeibullLayout->addWidget(m_pctrlWk,1,1);
	WeibullLayout->addWidget(m_pctrlPMkLabel,2,2);
	WeibullLayout->addWidget(m_pctrlPMk,2,1);
	WeibullLayout->addWidget(m_pctrlWALabel,1,4);
	WeibullLayout->addWidget(m_pctrlWA,1,3);
	WeibullLayout->addWidget(m_pctrlPMALabel,2,4);
	WeibullLayout->addWidget(m_pctrlPMA,2,3);
	WeibullLayout->addWidget(m_pctrlYieldLabel,3,1);
	WeibullLayout->addWidget(m_pctrlYield,3,2);
	WeibullLayout->addWidget(POwer,3,3);
    QGroupBox *WeibullGroup = new QGroupBox(tr("Weibull Settings"));
    WeibullGroup->setLayout(WeibullLayout);


   //---------------------------------------------//

	QVBoxLayout *SimLayout = new QVBoxLayout;

    QVBoxLayout *PowerEditLayout = new QVBoxLayout;
    QVBoxLayout *PowerLayout = new QVBoxLayout;


    PowerLayout->addWidget(TurbineDataGroup);
    PowerLayout->addWidget(SDGroup);
    PowerLayout->addWidget(WeibullGroup);
	PowerLayout->addStretch(1000);

    PowerEditLayout->addWidget(TNameGroup);
    PowerEditLayout->addWidget(TypeGroup);
    PowerEditLayout->addWidget(SpeciGroup);
    PowerEditLayout->addWidget(WingGroup);
    PowerEditLayout->addLayout(SaveOrBackLayout);
	PowerEditLayout->addStretch(1000);



    EditWidget = new QWidget;
    EditWidget->setLayout(EditLayout);
    PolarWidget = new QWidget;
//	PolarWidget->setMaximumWidth(200);
    PolarWidget->setLayout(Layout360);
	PowerEditWidget = new QWidget;
//	PowerEditWidget->setMaximumWidth(300);
    PowerEditWidget->setLayout(PowerEditLayout);
    PowerWidget = new QWidget;
//	PowerWidget->setMaximumWidth(300);
	PowerWidget->setLayout(PowerLayout);
    AdvancedEditWidget = new QWidget;
    AdvancedEditWidget->setLayout(AdvancedEditLayout);


    SimpleAdvanced = new QTabWidget;
    SimpleAdvanced->addTab(EditWidget, tr("Basic Blade Design"));
    SimpleAdvanced->addTab(AdvancedEditWidget, (tr("Advanced Blade Design")));

    connect(SimpleAdvanced, SIGNAL(currentChanged(int)),this, SLOT(TabChanged()));


    QVBoxLayout *AllEdit = new QVBoxLayout;
    AllEdit->addLayout(EditTopLayout);
    AllEdit->addWidget(SimpleAdvanced);
    AllEdit->addLayout(EditBottomLayout);

    QWidget *tabwidget = new QWidget;
    tabwidget->setLayout(AllEdit);


    bladeWidget->addWidget(WingDataBox);
    bladeWidget->addWidget(tabwidget);



//    bladeWidget->addWidget(AdvancedEditWidget);

    SimLayout->addLayout(ViewLayout2);
    SimLayout->addWidget(bladeWidget);

	SimWidget  = new QWidget;
    SimWidget->setLayout(SimLayout);

	mainWidget = new QStackedWidget;
    mainWidget->addWidget(SimWidget);
    mainWidget->addWidget(PolarWidget);
    mainWidget->addWidget(PowerWidget);
    mainWidget->addWidget(PowerEditWidget);

//    QRect rec = QApplication::desktop()->screenGeometry();
//    int width = rec.width();
    mainWidget->setMinimumWidth(100);

//	QVBoxLayout *mainLayout = new QVBoxLayout;
//  mainLayout->addWidget(mainWidget);
//  setLayout(mainLayout);


}





void QBEM::SetCurrentSection(int section)
{
        m_iSection = section;
        if(m_iSection <0 || m_iSection>m_pBlade->m_NPanel)
        {
                m_pctrlInsertAfter->setEnabled(false);
                m_pctrlInsertBefore->setEnabled(false);
                m_pctrlDeleteSection->setEnabled(false);
        }
        else
        {
                m_pctrlInsertAfter->setEnabled(true);
                m_pctrlInsertBefore->setEnabled(true);
                m_pctrlDeleteSection->setEnabled(true);

                QString str;
                str = QString(tr("Insert after section %1")).arg(m_iSection+1);
                m_pctrlInsertAfter->setText(str);
                str = QString(tr("Insert before section %1")).arg(m_iSection+1);
                m_pctrlInsertBefore->setText(str);
                str = QString(tr("Delete section %1")).arg(m_iSection+1);
                m_pctrlDeleteSection->setText(str);
        }
        m_bResetglSectionHighlight = true;
}


void QBEM::SetCharGraphTitles(Graph* pGraph)
{

    if(!pGraph) return;
    {
        switch (pGraph->GetXVariable())
        {
                case 0:
                        pGraph->SetXTitle(tr("P [W]"));
                        break;
                case 1:
                        pGraph->SetXTitle(tr("S [N]"));
                        break;
                case 2:
                        pGraph->SetXTitle(tr("T [Nm]"));
                        break;
                case 3:
                        pGraph->SetXTitle(tr("M_b [Nm]"));
                        break;
                case 4:
                        pGraph->SetXTitle(tr("V [m/s]"));
                        break;   
                case 5:
                        pGraph->SetXTitle(tr("TSR"));
                        break;
                case 6:
                        pGraph->SetXTitle(tr("1/TSR"));
                        break;
                case 7:
						pGraph->SetXTitle(tr("Omega [rpm]"));
                        break;
                case 8:
                        pGraph->SetXTitle(tr("Pitch [deg]"));
                        break;
                case 9:
                        pGraph->SetXTitle(tr("Cp"));
                        break;
                case 10:
                        pGraph->SetXTitle(tr("Ct"));
                        break;
                case 11:
                        pGraph->SetXTitle(tr("Cm"));
                        break;
                case 12:
                        pGraph->SetXTitle(tr("Kp"));
                        break;
                default:
                        pGraph->SetXTitle(tr("P [W]"));
                        break;
        }

        switch (pGraph->GetYVariable())
        {
                case 0:
                        pGraph->SetYTitle(tr("P [W]"));
                        break;
                case 1:
                        pGraph->SetYTitle(tr("S [N]"));
                        break;
                case 2:
                        pGraph->SetYTitle(tr("T [Nm]"));
                        break;
                case 3:
                        pGraph->SetYTitle(tr("M_b [Nm]"));
                        break;
                case 4:
                        pGraph->SetYTitle(tr("V [m/s]"));
                        break;
                case 5:
                        pGraph->SetYTitle(tr("TSR"));
                        break;
                case 6:
                        pGraph->SetYTitle(tr("1/TSR"));
                        break;
                case 7:
						pGraph->SetYTitle(tr("Omega [rpm]"));
                        break;
                case 8:
                        pGraph->SetYTitle(tr("Pitch [deg]"));
                        break;
                case 9:
                        pGraph->SetYTitle(tr("Cp"));
                        break;
                case 10:
                        pGraph->SetYTitle(tr("Ct"));
                        break;
                case 11:
                        pGraph->SetYTitle(tr("Cm"));
                        break;
                case 12:
                        pGraph->SetYTitle(tr("Kp"));
                        break;
                default:
                        pGraph->SetYTitle(tr("P [W]"));
                        break;
        }


    }
}


void QBEM::SetRotorGraphTitles(Graph* pGraph)
{
	if(!pGraph) return;
	if (pGraph->m_Type == 0)
	{
		switch (pGraph->GetXVariable())
		{
		case 0:
			pGraph->SetXTitle(tr("Cp"));
			break;
		case 1:
			pGraph->SetXTitle(tr("Ct"));
			break;
		case 2:
			pGraph->SetXTitle(tr("Cm"));
			break;
		case 3:
			pGraph->SetXTitle(tr("Kp"));
			break;
		case 4:
			pGraph->SetXTitle(tr("TSR"));
			break;
		case 5:
			pGraph->SetXTitle(tr("1/TSR"));
			break;
        case 6:
            pGraph->SetXTitle(tr("Power [W]"));
            break;
        case 7:
            pGraph->SetXTitle(tr("Thrust [N]"));
            break;
        case 8:
            pGraph->SetXTitle(tr("Torque [Nm]"));
            break;
        case 9:
            pGraph->SetXTitle(tr("Rot [rpm]"));
            break;
        case 10:
            pGraph->SetXTitle(tr("Inflow [m/s]"));
            break;
        case 11:
            pGraph->SetXTitle(tr("Bending [Nm]"));
            break;
			
		default:
			pGraph->SetXTitle(tr("TSR"));
			break;
		}
	}
	if (pGraph->m_Type == 1)
	{
		switch (pGraph->GetXVariable())
		{
		case 0:
			pGraph->SetXTitle(tr("a_a"));
			break;
		case 1:
			pGraph->SetXTitle(tr("pos [m]"));
			break;
		case 2:
			pGraph->SetXTitle(tr("a_t"));
			break;
		case 3:
			pGraph->SetXTitle(tr("lambda_local"));
			break;
		case 4:
			pGraph->SetXTitle(tr("Cn"));
			break;
		case 5:
			pGraph->SetXTitle(tr("Ct"));
			break;
		case 6:
			pGraph->SetXTitle(tr("Phi"));
			break;
		case 7:
			pGraph->SetXTitle(tr("Alpha"));
			break;
		case 8:
			pGraph->SetXTitle(tr("Theta"));
			break;
		case 9:
			pGraph->SetXTitle(tr("Chord [m]"));
			break;
		case 10:
			pGraph->SetXTitle(tr("Lift Coeff"));
			break;
		case 11:
			pGraph->SetXTitle(tr("Drag Coeff"));
			break;
		case 12:
			pGraph->SetXTitle(tr("L/D Ratio"));
			break;
		case 13:
			pGraph->SetXTitle(tr("F"));
			break;
        case 14:
            pGraph->SetXTitle(tr("Re [-]"));
            break;
        case 15:
            pGraph->SetXTitle(tr("Delta Re [-]"));
            break;
        case 16:
            pGraph->SetXTitle(tr("F_t [N/m]"));
            break;
        case 17:
            pGraph->SetXTitle(tr("F_n [N/m]"));
            break;
        case 18:
            pGraph->SetXTitle(tr("Circ"));
            break;
        case 19:
            pGraph->SetXTitle(tr("V_loc [m/s]"));
            break;
        case 20:
			pGraph->SetXTitle(tr("It"));
			break;
        case 21:
			pGraph->SetXTitle(tr("Fa_a"));
			break;
        case 22:
			pGraph->SetXTitle(tr("Fa_t"));
			break;
		default:
			pGraph->SetXTitle(tr("pos [m]"));
			break;
		}
	}
	if (pGraph->m_Type == 0)
	{
		switch (pGraph->GetYVariable())
		{
		case 0:
			pGraph->SetYTitle(tr("Cp"));
			break;
		case 1:
			pGraph->SetYTitle(tr("Ct"));
			break;
		case 2:
			pGraph->SetYTitle(tr("Cm"));
			break;
		case 3:
			pGraph->SetYTitle(tr("Kp"));
			break;
		case 4:
			pGraph->SetYTitle(tr("TSR"));
			break;
		case 5:
			pGraph->SetYTitle(tr("1/TSR"));
			break;
        case 6:
            pGraph->SetYTitle(tr("Power [W]"));
            break;
        case 7:
            pGraph->SetYTitle(tr("Thrust [N]"));
            break;
        case 8:
            pGraph->SetYTitle(tr("Torque [Nm]"));
            break;
        case 9:
            pGraph->SetYTitle(tr("Rot [rpm]"));
            break;
        case 10:
            pGraph->SetYTitle(tr("Inflow [m/s]"));
            break;
        case 11:
            pGraph->SetYTitle(tr("Bending [Nm]"));
            break;
		default:
			pGraph->SetYTitle(tr("TSR"));
			break;
		}
	}
	if (pGraph->m_Type == 1)
	{
		switch (pGraph->GetYVariable())
		{
		case 0:
			pGraph->SetYTitle(tr("a_a"));
			break;
		case 1:
			pGraph->SetYTitle(tr("pos [m]"));
			break;
		case 2:
			pGraph->SetYTitle(tr("a_t"));
			break;
		case 3:
			pGraph->SetYTitle(tr("lambda_local"));
			break;
		case 4:
			pGraph->SetYTitle(tr("Cn"));
			break;
		case 5:
			pGraph->SetYTitle(tr("Ct"));
			break;
		case 6:
			pGraph->SetYTitle(tr("Phi"));
			break;
		case 7:
			pGraph->SetYTitle(tr("Alpha"));
			break;
		case 8:
			pGraph->SetYTitle(tr("Theta"));
			break;
		case 9:
			pGraph->SetYTitle(tr("Chord [m]"));
			break;
		case 10:
			pGraph->SetYTitle(tr("Lift Coeff"));
			break;
		case 11:
			pGraph->SetYTitle(tr("Drag Coeff"));
			break;
		case 12:
			pGraph->SetYTitle(tr("L/D Ratio"));
			break;
		case 13:
			pGraph->SetYTitle(tr("F"));
			break;
        case 14:
            pGraph->SetYTitle(tr("Re [-]"));
            break;
        case 15:
            pGraph->SetYTitle(tr("Delta Re [-]"));
            break;
        case 16:
            pGraph->SetYTitle(tr("F_t [N/m]"));
            break;
        case 17:
            pGraph->SetYTitle(tr("F_n [N/m]"));
            break;
        case 18:
            pGraph->SetYTitle(tr("Circ"));
            break;
        case 19:
            pGraph->SetYTitle(tr("V_loc [m/s]"));
            break;
        case 20:
			pGraph->SetYTitle(tr("It"));
			break;
        case 21:
			pGraph->SetYTitle(tr("Fa_a"));
			break;
        case 22:
			pGraph->SetYTitle(tr("Fa_t"));
			break;
		default:
			pGraph->SetYTitle(tr("pos [m]"));
			break;
		}
	}
}

void QBEM::SetPowerGraphTitles(Graph* pGraph)
{
        if(!pGraph) return;
        if (pGraph->m_Type == 0)
        {
        switch (pGraph->GetXVariable())
        {
                    case 0:
                            pGraph->SetXTitle(tr("P [W]"));
                            break;
                    case 1:
                            pGraph->SetXTitle(tr("S [N]"));
                            break;
                    case 2:
                            pGraph->SetXTitle(tr("T [Nm]"));
                            break;
                    case 3:
                            pGraph->SetXTitle(tr("M_b [Nm]"));
                            break;
                    case 4:
                            pGraph->SetXTitle(tr("V [m/s]"));
                            break;
                    case 5:
                            pGraph->SetXTitle(tr("TSR"));
                            break;
                    case 6:
                            pGraph->SetXTitle(tr("1/TSR"));
                            break;
                    case 7:
                            pGraph->SetXTitle(tr("Omega [rpm]"));
                            break;
                    case 8:
                            pGraph->SetXTitle(tr("Pitch"));
                            break;
                    case 9:
                            pGraph->SetXTitle(tr("Cp"));
                            break;
                    case 10:
                            pGraph->SetXTitle(tr("Ct"));
                            break;
                    case 11:
                            pGraph->SetXTitle(tr("Cm"));
                            break;
                    case 12:
                            pGraph->SetXTitle(tr("Kp"));
                            break;
                    case 13:
                            pGraph->SetXTitle(tr("Cp_loss"));
                            break;
                    case 14:
                            pGraph->SetXTitle(tr("f Wei"));
                            break;
                    case 15:
                            pGraph->SetXTitle(tr("f x V^3"));
                            break;
                    default:
                            pGraph->SetXTitle(tr("TSR"));
                            break;
                }
        }
        if(pGraph->m_Type == 1)
        {
            switch (pGraph->GetXVariable())
            {

                case 0:
                        pGraph->SetXTitle(tr("Cn"));
                        break;
                case 1:
                        pGraph->SetXTitle(tr("Ct"));
                        break;
                case 2:
                        pGraph->SetXTitle(tr("Phi [deg]"));
                        break;
                case 3:
                        pGraph->SetXTitle(tr("Alpha [deg]"));
                        break;
                case 4:
                        pGraph->SetXTitle(tr("Theta [deg]"));
                        break;
                case 5:
                        pGraph->SetXTitle(tr("Chord [m]"));
                        break;
                case 6:
                        pGraph->SetXTitle(tr("Cl"));
                        break;
                case 7:
                        pGraph->SetXTitle(tr("Cd"));
                        break;
                case 8:
                        pGraph->SetXTitle(tr("Cl / Cd"));
                        break;
                case 9:
                        pGraph->SetXTitle(tr("F"));
                        break;
                case 10:
                        pGraph->SetXTitle(tr("a_a"));
                        break;
                case 11:
                        pGraph->SetXTitle(tr("a_t"));
                        break;
                case 12:
                        pGraph->SetXTitle(tr("pos [m]"));
                        break;
                case 13:
                        pGraph->SetXTitle(tr("TSR"));
                        break;
                case 14:
                        pGraph->SetXTitle(tr("Re"));
                        break;
                case 15:
                        pGraph->SetXTitle(tr("Delta Re"));
                        break;
                case 16:
                        pGraph->SetXTitle(tr("k [mm]"));
                        break;
                case 17:
                        pGraph->SetXTitle(tr("V [m/s]"));
                        break;
                case 18:
                        pGraph->SetXTitle(tr("F_t [N/m]"));
                        break;
                case 19:
                        pGraph->SetXTitle(tr("F_n [N/m]"));
                        break;
                case 20:
                        pGraph->SetXTitle(tr("It"));
                        break;
                case 21:
                        pGraph->SetXTitle(tr("Mach"));
                        break;
                case 22:
                        pGraph->SetXTitle(tr("Gam.[m2/s]"));
                        break;
                case 23:
                        pGraph->SetXTitle(tr("Fa_a"));
                        break;
                case 24:
                        pGraph->SetXTitle(tr("Fa_t"));
                        break;
                default:
                        pGraph->SetXTitle(tr("Pos [m]"));
                        break;
                }
        }
        if (pGraph->m_Type == 0)
        {
        switch (pGraph->GetYVariable())
        {
                    case 0:
                            pGraph->SetYTitle(tr("P [W]"));
                            break;
                    case 1:
                            pGraph->SetYTitle(tr("S [N]"));
                            break;
                    case 2:
                            pGraph->SetYTitle(tr("T [Nm]"));
                            break;
                    case 3:
                            pGraph->SetYTitle(tr("M_b [Nm]"));
                            break;
                    case 4:
                            pGraph->SetYTitle(tr("V [m/s]"));
                            break;
                    case 5:
                            pGraph->SetYTitle(tr("TSR"));
                            break;
                    case 6:
                            pGraph->SetYTitle(tr("1/TSR"));
                            break;
                    case 7:
                            pGraph->SetYTitle(tr("Omega [rpm]"));
                            break;
                    case 8:
                            pGraph->SetYTitle(tr("Pitch"));
                            break;
                    case 9:
                            pGraph->SetYTitle(tr("Cp"));
                            break;
                    case 10:
                            pGraph->SetYTitle(tr("Ct"));
                            break;
                    case 11:
                            pGraph->SetYTitle(tr("Cm"));
                            break;
                    case 12:
                            pGraph->SetYTitle(tr("Kp"));
                            break;
                    case 13:
                            pGraph->SetYTitle(tr("Cp_loss"));
                            break;
                    case 14:
                            pGraph->SetYTitle(tr("f Wei"));
                            break;
                    case 15:
                            pGraph->SetYTitle(tr("f x V^3"));
                            break;
                    default:
                            pGraph->SetYTitle(tr("Alpha [deg]"));
                            break;
                }
        }
        if(pGraph->m_Type == 1)
        {
            switch (pGraph->GetYVariable())
            {

                case 0:
                        pGraph->SetYTitle(tr("Cn"));
                        break;
                case 1:
                        pGraph->SetYTitle(tr("Ct"));
                        break;
                case 2:
                        pGraph->SetYTitle(tr("Phi [deg]"));
                        break;
                case 3:
                        pGraph->SetYTitle(tr("Alpha [deg]"));
                        break;
                case 4:
                        pGraph->SetYTitle(tr("Theta [deg]"));
                        break;
                case 5:
                        pGraph->SetYTitle(tr("Chord [m]"));
                        break;
                case 6:
                        pGraph->SetYTitle(tr("Cl"));
                        break;
                case 7:
                        pGraph->SetYTitle(tr("Cd"));
                        break;
                case 8:
                        pGraph->SetYTitle(tr("Cl / Cd"));
                        break;
                case 9:
                        pGraph->SetYTitle(tr("F"));
                        break;
                case 10:
                        pGraph->SetYTitle(tr("a_a"));
                        break;
                case 11:
                        pGraph->SetYTitle(tr("a_t"));
                        break;
                case 12:
                        pGraph->SetYTitle(tr("Pos [m]"));
                        break;
                case 13:
                        pGraph->SetYTitle(tr("TSR"));
                        break;
                case 14:
                        pGraph->SetYTitle(tr("Re"));
                        break;
                case 15:
                        pGraph->SetYTitle(tr("Delta Re"));
                        break;
                case 16:
                        pGraph->SetYTitle(tr("k [mm]"));
                        break;
                case 17:
                        pGraph->SetYTitle(tr("V [m/s]"));
                        break;
                case 18:
                        pGraph->SetYTitle(tr("F_t [N/m]"));
                        break;
                case 19:
                        pGraph->SetYTitle(tr("F_n [N/m]"));
                        break;
                case 20:
                        pGraph->SetYTitle(tr("It"));
                        break;
                case 21:
                        pGraph->SetYTitle(tr("Mach"));
                        break;
                case 22:
                        pGraph->SetYTitle(tr("Gam.[m2/s]"));
                        break;
                case 23:
                        pGraph->SetYTitle(tr("Fa_a"));
                        break;
                case 24:
                        pGraph->SetYTitle(tr("Fa_t"));
                        break;
                default:
                        pGraph->SetYTitle(tr("Pos [m]"));
                        break;
                }
        }
}

// new code JW //
void QBEM::SetWeibullGraphTitles(Graph* pGraph)
{
	if(!pGraph) return;
	if (pGraph->m_Type == 3)
	{
		switch (pGraph->GetXVariable())
		{
		case 0:
			pGraph->SetXTitle(tr("k"));
			break;
		case 1:
			pGraph->SetXTitle(tr("A"));
			break;
		default:
			pGraph->SetXTitle(tr("k"));
			break;

		}

		pGraph->SetYTitle(tr("AEP [kWh]"));

	}
}
// end new code JW //

void QBEM::SetPolarGraphTitles(Graph* pGraph)
{
        if(!pGraph) return;
        switch (pGraph->GetXVariable())
        {
                case 0:
                        pGraph->SetXTitle(tr("Alpha [deg]"));
                        break;
                case 1:
                        pGraph->SetXTitle(tr("Cl"));
                        break;
                case 2:
                        pGraph->SetXTitle(tr("Cd"));
                        break;
                case 3:
                        pGraph->SetXTitle(tr("Cl / Cd"));
                        break; 
                case 4:
                        pGraph->SetXTitle(tr("Cl attached"));
                        break;
                case 5:
                        pGraph->SetXTitle(tr("Cl separated"));
                        break;
                case 6:
                        pGraph->SetXTitle(tr("f function"));
                        break;
                default:
                        pGraph->SetXTitle(tr("Alpha [deg]"));
                        break;
        }
        switch (pGraph->GetYVariable())
        {
                 case 0:
                         pGraph->SetYTitle(tr("Alpha [deg]"));
                         break;
                 case 1:
                         pGraph->SetYTitle(tr("Cl"));
                         break;
                 case 2:
                         pGraph->SetYTitle(tr("Cd"));
                         break;
                 case 3:
                         pGraph->SetYTitle(tr("Cl / Cd"));
                         break;
                 case 4:
                         pGraph->SetYTitle(tr("Cl attached"));
                         break;
                 case 5:
                         pGraph->SetYTitle(tr("Cl separated"));
                         break;
                 case 6:
                         pGraph->SetYTitle(tr("f function"));
                         break;
                 default:
                         pGraph->SetYTitle(tr("Alpha [deg]"));
                         break;
        }
}

void QBEM::SetLegendPos()
{
    int h   = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_CharLegendOffset.rx() = 10;
    m_CharLegendOffset.ry() = h2 + 30;

    m_PowerLegendOffset.rx() = 10;
    m_PowerLegendOffset.ry() = h2 + 30;
}

void QBEM::SetScale()
{
        Set2DScale();
}

void QBEM::SetScale(QRect CltRect)
{
        m_rCltRect = CltRect;
        m_rSingleRect.setRect(CltRect.x(), CltRect.y(), CltRect.width(), CltRect.height());

        Set2DScale();
}

void QBEM::Set2DScale()
{
        if(m_bIs2DScaleSet && !m_bAutoScales) return;

        int w,h;

        h = m_rCltRect.height();
        w = m_rCltRect.width();
        int w2  = (int)(w/2);
        int h34  = (int)(3*h/4);

        QRect CpRect(0,0,w,h34);


        m_rSingleRect.setRect(40,10,m_rCltRect.right()-80,m_rCltRect.bottom()-230);


        QRect Rect1(0,0,w2,m_rCltRect.bottom()-00);
        QRect Rect2(w2,0,w2,m_rCltRect.bottom()-00);

        m_360Graph1.SetDrawRect(Rect1);
        m_360Graph1.SetDrawRect(Rect2);

        m_360Graph1.Init();
        m_360Graph1.Init();


        m_360Graph1.SetAutoXUnit();
        m_360Graph1.SetAutoXUnit();

}

void QBEM::UpdateBladeData()

{
        // fills combobox with blade associated to QBEM's current blade

        int i/*, size*/, pos;
        QString strong;
		m_BEMToolBar->m_tsrComboBox->clear();
        bool exists=false;




        if(!m_pBEMData || !m_pBEMData->m_BData.size())
        {

				m_BEMToolBar->m_tsrComboBox->setEnabled(false);

                selected_lambda = -1;


        }

//        size = 0;
        if (m_pBEMData)
        {
        //count the number of simulations associated to the current blade
        for (i=0; i<m_pBEMData->m_BData.size(); i++)
        {
		m_BEMToolBar->m_tsrComboBox->addItem(m_pBEMData->m_BData.at(i)->lambdaglobal);
        exists=true;
        }

        if (exists)
        {

                // if any
				m_BEMToolBar->m_tsrComboBox->setEnabled(true);

                if(m_pBData)
                {

						pos = m_BEMToolBar->m_tsrComboBox->findText(m_pBData->lambdaglobal);
                        if(pos>=0)
                        {
							m_BEMToolBar->m_tsrComboBox->setCurrentIndex(pos);
							strong = m_BEMToolBar->m_tsrComboBox->itemText(pos);
                            m_pBData = GetBladeData(strong);
                            selected_lambda = pos;
                        }
                        else
                        {
							m_BEMToolBar->m_tsrComboBox->setCurrentIndex(0);
							strong = m_BEMToolBar->m_tsrComboBox->itemText(0);
                            m_pBData = GetBladeData(strong);
                            selected_lambda = -1;
                        }
                }
                else
                {
					m_BEMToolBar->m_tsrComboBox->setCurrentIndex(0);
					strong = m_BEMToolBar->m_tsrComboBox->itemText(0);
                    m_pBData = GetBladeData(strong);
                    selected_lambda = -1;
                }

        }

    }

        if (!exists)
        {
                // otherwise disable control
				m_BEMToolBar->m_tsrComboBox->setEnabled(false);
                m_pBData = NULL;
                selected_lambda = -1;
		}

        CreateRotorCurves();



}

void QBEM::UpdateCurve()
{
		if(g_mainFrame->m_iView == BEMSIMVIEW && m_pBEMData)// new JW, old: m_iView
        {
                m_pBEMData->m_Color = m_CurveColor;
                m_pBEMData->m_Style = m_CurveStyle;
                m_pBEMData->m_Width = (int)m_CurveWidth;
                CreateRotorCurves();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW && m_pTBEMData)// new JW, old: m_iView
        {
                m_pTBEMData->m_Color = m_CurveColor;
                m_pTBEMData->m_Style = m_CurveStyle;
				m_pTBEMData->m_Width = (int)m_CurveWidth;
				//new code JW
				if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
					OnSetWeibull();
				else
				//end new code JW
					CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == POLARVIEW && m_pCur360Polar)// new JW, old: m_iView
        {
                m_pCur360Polar->m_Color = m_CurveColor;
                m_pCur360Polar->m_Style = m_CurveStyle;
                m_pCur360Polar->m_Width = (int)m_CurveWidth;
                if (!m_bNew360Polar && !m_bDecompose) CreatePolarCurve();
                else CreateSinglePolarCurve();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCBEMData)// new JW, old: m_iView
        {
                m_pCBEMData->m_Color = m_CurveColor;
                m_pCBEMData->m_Style = m_CurveStyle;
                m_pCBEMData->m_Width = (int)m_CurveWidth;
                CreateCharacteristicsCurves();
        }

        UpdateView();
		g_mainFrame->SetSaveState(false);
}

void QBEM::UpdateTurbineBladeData()

{
        int i/*, size*/, pos;
        QString strong;
		m_BEMToolBar->m_turbinewindspeedComboBox->clear();
        bool exists=false;

        if(!m_pTBEMData || !m_pTBEMData->m_BData.size())
        {
				m_BEMToolBar->m_turbinewindspeedComboBox->setEnabled(false);
                selected_windspeed = -1;
        }

//        size = 0;

		if (m_pTBEMData)
        {
        //count the number of simulations associated to the current wing
        for (i=0; i<m_pTBEMData->m_BData.size(); i++)
        {
		m_BEMToolBar->m_turbinewindspeedComboBox->addItem(m_pTBEMData->m_BData.at(i)->windspeedStr);
        exists=true;
        }

        if (exists)
        {

                // if any
				m_BEMToolBar->m_turbinewindspeedComboBox->setEnabled(true);

                if(m_pTurbineBData)
                {

						pos = m_BEMToolBar->m_turbinewindspeedComboBox->findText(m_pTurbineBData->windspeedStr);
                        if(pos>=0)
                        {
							m_BEMToolBar->m_turbinewindspeedComboBox->setCurrentIndex(pos);
							strong = m_BEMToolBar->m_turbinewindspeedComboBox->itemText(pos);
                            m_pTurbineBData = GetTurbineBladeData(strong);
                            selected_windspeed = pos;
                        }
                        else
                        {
							m_BEMToolBar->m_turbinewindspeedComboBox->setCurrentIndex(0);
							strong = m_BEMToolBar->m_turbinewindspeedComboBox->itemText(0);
                            m_pTurbineBData = GetTurbineBladeData(strong);
                            selected_windspeed = -1;
                        }
                }
                else
                {
					m_BEMToolBar->m_turbinewindspeedComboBox->setCurrentIndex(0);
					strong = m_BEMToolBar->m_turbinewindspeedComboBox->itemText(0);
                    m_pTurbineBData = GetTurbineBladeData(strong);
                    selected_windspeed = -1;
                }

        }

    }

        if (!exists)
        {


                // otherwise disable control
				m_BEMToolBar->m_turbinewindspeedComboBox->setEnabled(false);
                m_pTurbineBData = NULL;
                selected_windspeed = -1;
        }

		if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
			OnSetWeibull();
		else
		//new code JW
			CreatePowerCurves();

		//OnSetWeibullA(m_pctrlWA->value());
		//OnSetWeibullK(m_pctrlWk->value());


//        m_RotorGraph1.m_bAutoX = false;
//        m_RotorGraph2.m_bAutoX = false;
//        m_RotorGraph3.m_bAutoX = false;
//        m_RotorGraph1.m_bAutoY = false;
//        m_RotorGraph2.m_bAutoY = false;
//        m_RotorGraph3.m_bAutoY = false;

}

void QBEM::Update360Polars()
{

     m_pCur360Polar =  m_BEMToolBar->m_polar360ComboBox->currentObject();
     CreatePolarCurve();

	 if (m_pCur360Polar)
	 {
		 SetCurveParams();
		 m_360Name->setText(m_pCur360Polar->getName());
         m_pctrlCD90->setValue(m_pCur360Polar->CD90);
	 }
	 else
	 {
		 FillComboBoxes(false);
	 }

}

void QBEM::UpdateFoils()
{
    if (g_pCurFoil){
        m_pCurFoil = g_pCurFoil;
        m_BEMToolBar->m_foilComboBox->setCurrentObject(m_pCurFoil);
    }
    else{
        m_pCurFoil =  m_BEMToolBar->m_foilComboBox->currentObject();
    }
        UpdatePolars();
}

void QBEM::UpdatePolars()
{
        m_pCurPolar =  m_BEMToolBar->m_polarComboBox->currentObject();
        Update360Polars();
}

void QBEM::UpdateRotorSimulation()
{

    if (m_BEMToolBar->m_bemdataComboBox->count())
    {
        if (m_pBEMData)
        {
            int pos = m_BEMToolBar->m_bemdataComboBox->findText(m_pBEMData->getName());
            if(pos>=0)
            {
                m_BEMToolBar->m_bemdataComboBox->setCurrentIndex(pos);
            }
            else
            {
                m_BEMToolBar->m_bemdataComboBox->setCurrentIndex(0);
                m_pBEMData =  m_BEMToolBar->m_bemdataComboBox->currentObject();
            }
        }
        else
        {
            m_BEMToolBar->m_bemdataComboBox->setCurrentIndex(0);
            m_pBEMData =  m_BEMToolBar->m_bemdataComboBox->currentObject();
        }
    }
    else
    {
        m_pBEMData = NULL;
    }

        UpdateBladeData();
        CreateRotorCurves();

		if (g_mainFrame->m_iView == BEMSIMVIEW) InitBladeSimulationParams(m_pBEMData);// new JW, old: m_iView

        if (m_pBEMData) SetCurveParams();
        else FillComboBoxes(false);

}


void QBEM::UpdateCharacteristicsSimulation()

{
        if (m_pCBEMData) m_BEMToolBar->m_cbemdataComboBox->setCurrentObject(m_pCBEMData);
        m_pCBEMData = m_BEMToolBar->m_cbemdataComboBox->currentObject();

        CreateCharacteristicsCurves();
        UpdateCharacteristicsMatrix();

		if (g_mainFrame->m_iView == CHARSIMVIEW) InitCharSimulationParams(m_pCBEMData);// new JW, old: m_iView

        if (m_pCBEMData) SetCurveParams();
        else FillComboBoxes(false);
}

void QBEM::UpdateCharacteristicsMatrix()
{
    QString strong;
    double windspeed, rot, pitch;


	m_BEMToolBar->m_windspeedComboBox->clear();
	m_BEMToolBar->m_rotComboBox->clear();
	m_BEMToolBar->m_pitchComboBox->clear();

    if (m_pCBEMData && m_pCBEMData->simulated)
    {
        for (int i=0;i<m_pCBEMData->windtimes;i++)
        {
            windspeed = m_pCBEMData->windstart+m_pCBEMData->winddelta*i;
			m_BEMToolBar->m_windspeedComboBox->addItem(strong.number(windspeed,'f',2));
        }

        for (int j=0;j<m_pCBEMData->rottimes;j++)
        {
            rot = m_pCBEMData->rotstart+m_pCBEMData->rotdelta*j;
			m_BEMToolBar->m_rotComboBox->addItem(strong.number(rot,'f',2));
        }

        for (int k=0;k<m_pCBEMData->pitchtimes;k++)
        {
        pitch = m_pCBEMData->pitchstart+m_pCBEMData->pitchdelta*k;
		m_BEMToolBar->m_pitchComboBox->addItem(strong.number(pitch,'f',2));
        }
		m_BEMToolBar->m_windspeedComboBox->setEnabled(true);
		m_BEMToolBar->m_rotComboBox->setEnabled(true);
		m_BEMToolBar->m_pitchComboBox->setEnabled(true);

    }
    else
    {
	m_BEMToolBar->m_windspeedComboBox->setEnabled(false);
	m_BEMToolBar->m_rotComboBox->setEnabled(false);
	m_BEMToolBar->m_pitchComboBox->setEnabled(false);
    }

    selected_wind = 0;
    selected_rot = 0;
    selected_pitch = 0;

    CreateCharacteristicsCurves();

}

void QBEM::UpdateTurbines()
{

	if (m_BEMToolBar->m_tdataComboBox->count())
	{
		if (m_pTData)
		{
			int pos = m_BEMToolBar->m_tdataComboBox->findText(m_pTData->getName());
			if(pos>=0)
			{
				m_BEMToolBar->m_tdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_BEMToolBar->m_tdataComboBox->setCurrentIndex(0);
				m_pTData =  m_BEMToolBar->m_tdataComboBox->currentObject();
			}
		}
		else
		{
			m_BEMToolBar->m_tdataComboBox->setCurrentIndex(0);
			m_pTData =  m_BEMToolBar->m_tdataComboBox->currentObject();
		}
	}
	else
	{
		m_pTData = NULL;
	}

    InitTurbineData(m_pTData);

    UpdateTurbineSimulation();


	//new code JW
	if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
		OnSetWeibull();
	else
	//end new code JW
		CreatePowerCurves();




}

void QBEM::UpdateUnits()
{

    SimuWidget *pSimuWidget = (SimuWidget * ) m_pSimuWidget;
    QString str;
	GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
    pSimuWidget->speed1->setText(str);
    pSimuWidget->speed2->setText(str);
    pSimuWidget->speed3->setText(str);
    QString sstr;
    sstr = " "+str+" ";
    pSimuWidget->WSpeed1->setText(sstr);
    pSimuWidget->WSpeed2->setText(sstr);
    pSimuWidget->WSpeed3->setText(sstr);

    InitTurbineData(m_pTData);
    if (m_pTData && m_pTBEMData) OnSetWeibull();

}

void QBEM::UpdateGeom()
{
    m_bResetglGeom = true;

    UpdateView();
}

void QBEM::UpdateView() {
	if (g_mainFrame->m_iView==BLADEVIEW) {
		m_pGLWidget->update();
	} else {
		m_p2DWidget->update();
	}
}

void QBEM::UpdateBlades()
{
   m_pBlade = m_BEMToolBar->m_rotorComboBox->currentObject();

   if (g_mainFrame->m_iView==BEMSIMVIEW)
	   UpdateRotorSimulation();
   else if (g_mainFrame->m_iView==CHARSIMVIEW)
	   UpdateCharacteristicsSimulation();

   InitBladeTable();
}

void QBEM::UpdateTurbineSimulation()

{
	if (m_BEMToolBar->m_tbemdataComboBox->count())
	{
		if (m_pTBEMData)
		{
			int pos = m_BEMToolBar->m_tbemdataComboBox->findText(m_pTBEMData->getName());
			if(pos>=0)
			{
				m_BEMToolBar->m_tbemdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_BEMToolBar->m_tbemdataComboBox->setCurrentIndex(0);
				m_pTBEMData =  m_BEMToolBar->m_tbemdataComboBox->currentObject();
			}
		}
		else
		{
			m_BEMToolBar->m_tbemdataComboBox->setCurrentIndex(0);
			m_pTBEMData =  m_BEMToolBar->m_tbemdataComboBox->currentObject();
		}
	}
	else
	{
		m_pTBEMData = NULL;
	}


        if (m_pTBEMData) SetCurveParams();
        else FillComboBoxes(false);

        UpdateTurbineBladeData();

		if (g_mainFrame->m_iView == TURBINEVIEW) InitTurbineSimulationParams(m_pTBEMData);// new JW, old: m_iView






}

void QBEM::WheelEvent(QWheelEvent *event)
{
	//The mouse button has been wheeled
	//Process the message
	QPoint pt(event->x(), event->y()); //client coordinates
	
	if(g_mainFrame->m_iView==BLADEVIEW)// new JW, old: m_iView
	{
        if(m_pBlade)
		{
			//zoom wing
			if(event->delta()>0) m_glScaled /= (GLfloat)1.06;
			else                 m_glScaled *= (GLfloat)1.06;
			UpdateView();
		}
	}
	else if(g_mainFrame->m_iView == POLARVIEW || g_mainFrame->m_iView==BEMSIMVIEW || g_mainFrame->m_iView == TURBINEVIEW || g_mainFrame->m_iView == CHARSIMVIEW )// new JW, old: m_iView
	{
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
		{
			if (m_bXPressed)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				if(event->delta()>0) m_pCurGraph->Scalex(1.06);
				else                 m_pCurGraph->Scalex(1.0/1.06);
			}
			else if(m_bYPressed)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				if(event->delta()>0) m_pCurGraph->Scaley(1.06);
				else                 m_pCurGraph->Scaley(1.0/1.06);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				if(event->delta()>0) m_pCurGraph->Scale(1.06);
				else                 m_pCurGraph->Scale(1.0/1.06);
			}
			
			m_pCurGraph->SetAutoXUnit();
			m_pCurGraph->SetAutoYUnit();
			UpdateView();
		}
	}
}

void QBEM::onModuleChanged() {
	if (g_mainFrame->m_iApp == BEM) {

		m_BEMToolBar->hide();

		g_mainFrame->m_pctrlBEMWidget->hide();
        g_mainFrame->m_pctrlSimuWidget->hide();

        g_mainFrame->OnRotorViewAct->setChecked(false);
		g_mainFrame->OnTurbineViewAct->setChecked(false);
		g_mainFrame->On360ViewAct->setChecked(false);
		g_mainFrame->OnBladeViewAct->setChecked(false);
		g_mainFrame->OnCharacteristicViewAct->setChecked(false);
		
		glPopAttrib();  // restores the saved GL settings		
	}
}

QBEM *g_qbem;
