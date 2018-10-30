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

#include "DMS.h"
#include "../Globals.h"
#include "OptimizeDlgVAWT.h"
#include "BladeScaleDlgVAWT.h"
#include "CreateDMSDlg.h"
#include "../Graph/GraphDlg.h"
#include "../XBEM/PrescribedValuesDlg.h"
#include "QDebug"
#include "../StoreAssociatedComboBox.h"
#include "../ScrolledDock.h"
#include "../GlobalFunctions.h"
#include "../GLWidget.h"

using namespace std;

extern bool ObjectIsEdited;

QDMS::QDMS(QWidget *parent)
	: QBEM(parent)
{
	/////////////// new NM ///////////////
	if (!g_qdms) {
		g_qdms = this;
	}
	/////////// end new NM ///////////////

	m_pTurbineDData = NULL;
	m_pDData = NULL;
	m_pDMSData = NULL;
	m_pTDMSData = NULL;
	m_pCDMSData = NULL;
    m_pBladeDelegate = NULL;
    m_pWingModel = NULL;

    dlg_powerlaw = false;
    dlg_constant = false;
    dlg_logarithmic = false;
    dlg_exponent = 0.4;
    dlg_roughness = 1;

    selected_height = 0;

    m_RotorGraph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph1.SetXMin(-0.0);
    m_RotorGraph1.SetXMax( 0.1);
    m_RotorGraph1.SetYMin(-0.01);
    m_RotorGraph1.SetYMax( 0.01);
    m_RotorGraph1.SetType(0);
    m_RotorGraph1.SetMargin(50);
	m_RotorGraph1.SetXVariable(7);
    m_RotorGraph1.SetYVariable(0);
    m_RotorGraph1.SetGraphName(tr("DMSRotor1"));
    m_RotorGraph1.m_Type=0;
	SetRotorGraphTitles(&m_RotorGraph1);



    m_RotorGraph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph2.SetXMin(-0.0);
    m_RotorGraph2.SetXMax( 0.1);
    m_RotorGraph2.SetYMin(-0.01);
    m_RotorGraph2.SetYMax( 0.01);
    m_RotorGraph2.SetType(0);
    m_RotorGraph2.SetMargin(50);
    m_RotorGraph2.SetXVariable(4);
	m_RotorGraph2.SetYVariable(0);
    m_RotorGraph2.SetGraphName(tr("DMSRotor2"));
	m_RotorGraph2.m_Type=1;
	SetRotorGraphTitles(&m_RotorGraph2);



    m_RotorGraph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_RotorGraph3.SetXMin(-0.0);
    m_RotorGraph3.SetXMax( 0.1);
    m_RotorGraph3.SetYMin(-0.01);
    m_RotorGraph3.SetYMax( 0.01);
    m_RotorGraph3.SetType(0);
    m_RotorGraph3.SetMargin(50);
	m_RotorGraph3.SetXVariable(0);
	m_RotorGraph3.SetYVariable(16);
    m_RotorGraph3.SetGraphName(tr("DMSRotor3"));
	m_RotorGraph3.m_Type=2;
	SetRotorGraphTitles(&m_RotorGraph3);




    m_PowerGraph1.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph1.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph1.SetXMin(-0.0);
    m_PowerGraph1.SetXMax( 0.1);
    m_PowerGraph1.SetYMin(-0.01);
    m_PowerGraph1.SetYMax( 0.01);
    m_PowerGraph1.SetType(0);
    m_PowerGraph1.SetMargin(50);
	m_PowerGraph1.SetXVariable(0);
	m_PowerGraph1.SetYVariable(2);
    m_PowerGraph1.SetGraphName(tr("DMSPower1"));
    m_PowerGraph1.m_Type=0;
	SetPowerGraphTitles(&m_PowerGraph1);
	SetWeibullGraphTitles(&m_PowerGraph1);





    m_PowerGraph2.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph2.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph2.SetXMin(-0.0);
    m_PowerGraph2.SetXMax( 0.1);
    m_PowerGraph2.SetYMin(-0.01);
    m_PowerGraph2.SetYMax( 0.01);
    m_PowerGraph2.SetType(0);
    m_PowerGraph2.SetMargin(50);
    m_PowerGraph2.SetXVariable(4);
	m_PowerGraph2.SetYVariable(0);
    m_PowerGraph2.SetGraphName(tr("DMSPower2"));
	m_PowerGraph2.m_Type=1;
	SetPowerGraphTitles(&m_PowerGraph2);
	SetWeibullGraphTitles(&m_PowerGraph2);




    m_PowerGraph3.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph3.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_PowerGraph3.SetXMin(-0.0);
    m_PowerGraph3.SetXMax( 0.1);
    m_PowerGraph3.SetYMin(-0.01);
    m_PowerGraph3.SetYMax( 0.01);
    m_PowerGraph3.SetType(0);
    m_PowerGraph3.SetMargin(50);
	m_PowerGraph3.SetXVariable(0);
	m_PowerGraph3.SetYVariable(22);
    m_PowerGraph3.SetGraphName(tr("DMSPower3"));
	m_PowerGraph3.m_Type=2;
	SetPowerGraphTitles(&m_PowerGraph3);
	SetWeibullGraphTitles(&m_PowerGraph3);



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
	m_CharGraph1.SetGraphName(tr("DMSCharacteristic Graph 1"));
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
	m_CharGraph2.SetGraphName(tr("DMSCharacteristic Graph 2"));
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
	m_CharGraph3.SetGraphName(tr("DMSCharacteristic Graph 3"));
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
	m_CharGraph4.SetGraphName(tr("DMSCharacteristic Graph 4"));
	m_CharGraph4.m_Type=0;
	m_CharGraph4.m_MainVar=2;
	m_CharGraph4.m_Param=0;

	SetCharGraphTitles(&m_CharGraph1);
	SetCharGraphTitles(&m_CharGraph2);
	SetCharGraphTitles(&m_CharGraph3);
	SetCharGraphTitles(&m_CharGraph4);

    m_bisHeight = true;

    
    /////////////// new NM ///////////////
    // connect signals
    connect (g_mainFrame, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));
	/////////// end new NM ///////////////
}

QStringList QDMS::prepareMissingObjectMessage() {
	switch (g_mainFrame->m_iView) {
	case BLADEVIEW:
		return CBlade::prepareMissingObjectMessage(true);
	case BEMSIMVIEW:
		return DMSData::prepareMissingObjectMessage();
	case TURBINEVIEW:
		return TDMSData::prepareMissingObjectMessage();
	case CHARSIMVIEW:
		return CDMSData::prepareMissingObjectMessage();
	default:
		return QStringList("unknown view");
	}
}


void QDMS::CheckButtons()
{
   
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

    UpdateUnits();

	///context menu///
    g_mainFrame->currentBladeMenu->setEnabled(m_pBlade);
    g_mainFrame->currentBladeMenu2->setEnabled(m_pBlade);
    g_mainFrame->currentTurbineMenu->setEnabled(m_pTData);
    g_mainFrame->DeleteCurrentRotorSimAct->setEnabled(m_pDMSData);
	g_mainFrame->DeleteCurrentCharSimAct->setEnabled(m_pCDMSData);
    g_mainFrame->DeleteCurrentTurbineSimAct->setEnabled(m_pTDMSData);
    g_mainFrame->DeleteCurrentTurbineAct->setEnabled(m_pTData);
    g_mainFrame->DeleteCurrentBladeAct->setEnabled(m_pBlade);
    g_mainFrame->ExportBladeGeomAct->setEnabled(m_pBlade);
    g_mainFrame->ExportCurrentRotorAct->setVisible(false); // new code NM - should always be disabled
    g_mainFrame->ExportBladeTableAct->setVisible(false);  // new code DM - should always be disabled
    g_mainFrame->OnImportBladeGeometry->setVisible(false);  // new code DM - should always be disabled
    g_mainFrame->ExportCurrentRotorAeroDynAct->setVisible(false);


    InitTurbineData(m_pTData);

    ///////enable or disable according to data present in simuwidget
	pSimuWidgetDMS->m_pctrlWind1->setValue(dlg_windstart);
	pSimuWidgetDMS->m_pctrlWind2->setValue(dlg_windend);
	pSimuWidgetDMS->m_pctrlWindDelta->setValue(dlg_winddelta);

	pSimuWidgetDMS->m_pctrlDefineTurbineSim->setEnabled(m_pTData);
	pSimuWidgetDMS->m_pctrlStartTurbineSim->setEnabled(m_pTDMSData);
	pSimuWidgetDMS->m_pctrlWind1->setEnabled(m_pTDMSData);
	pSimuWidgetDMS->m_pctrlWind2->setEnabled(m_pTDMSData);
	pSimuWidgetDMS->m_pctrlWindDelta->setEnabled(m_pTDMSData);
    pSimuWidgetDMS->m_pctrlDeleteTDMS->setEnabled(m_pTDMSData);


    pSimuWidgetDMS->m_pctrlCreateDMS->setEnabled(m_pBlade);
	pSimuWidgetDMS->m_pctrlStartDMS->setEnabled(m_pDMSData);
	pSimuWidgetDMS->m_pctrlLSLineEdit->setEnabled(m_pDMSData);
	pSimuWidgetDMS->m_pctrlLELineEdit->setEnabled(m_pDMSData);
	pSimuWidgetDMS->m_pctrlLDLineEdit->setEnabled(m_pDMSData);
    pSimuWidgetDMS->m_pctrlWindspeed->setEnabled(m_pDMSData);
    pSimuWidgetDMS->m_pctrlDeleteDMS->setEnabled(m_pDMSData);



    pSimuWidgetDMS->CreateCharSim->setEnabled(m_pBlade);
	pSimuWidgetDMS->StartCharSim->setEnabled(m_pCDMSData);
    pSimuWidgetDMS->m_pctrlDeleteCDMS->setEnabled(m_pCDMSData);

	pSimuWidgetDMS->WindStart->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->WindEnd->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->WindDelta->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->PitchStart->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->PitchEnd->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->PitchDelta->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->RotStart->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->RotEnd->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->RotDelta->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->WindFixed->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->PitchFixed->setEnabled(m_pCDMSData);
	pSimuWidgetDMS->RotFixed->setEnabled(m_pCDMSData);

	//// is something fixed in simuwidget
	if (pSimuWidgetDMS->WindFixed->isChecked()) pSimuWidgetDMS->WindDelta->setDisabled(true);
	if (pSimuWidgetDMS->WindFixed->isChecked()) pSimuWidgetDMS->WindEnd->setDisabled(true);

	if (pSimuWidgetDMS->PitchFixed->isChecked())pSimuWidgetDMS->PitchDelta->setDisabled(true);
	if (pSimuWidgetDMS->PitchFixed->isChecked())pSimuWidgetDMS->PitchEnd->setDisabled(true);

	if (pSimuWidgetDMS->RotFixed->isChecked())pSimuWidgetDMS->RotDelta->setDisabled(true);
	if (pSimuWidgetDMS->RotFixed->isChecked())pSimuWidgetDMS->RotEnd->setDisabled(true);

    m_pctrlNewTurbine->setEnabled(g_verticalRotorStore.size());
    m_pctrlDeleteTurbine->setEnabled(m_pTData);

    m_pctrlEditTurbine->setEnabled(m_pTData);
    m_pctrlEditWing->setEnabled(m_pBlade);
    m_pctrlNewWing->setEnabled(g_360PolarStore.size());
    m_pctrlDeleteWing->setEnabled(m_pBlade);


    ///init values in simuwidget

	pSimuWidgetDMS->m_pctrlLSLineEdit->setValue(dlg_lambdastart);
	pSimuWidgetDMS->m_pctrlLELineEdit->setValue(dlg_lambdaend);
	pSimuWidgetDMS->m_pctrlLDLineEdit->setValue(dlg_lambdadelta);
    pSimuWidgetDMS->m_pctrlWindspeed->setValue(dlg_windspeed);


	pSimuWidgetDMS->m_pctrlWind1->setValue(dlg_windstart);
	pSimuWidgetDMS->m_pctrlWind2->setValue(dlg_windend);
	pSimuWidgetDMS->m_pctrlWindDelta->setValue(dlg_winddelta);

	pSimuWidgetDMS->WindStart->setValue(dlg_windstart2);
	pSimuWidgetDMS->WindEnd->setValue(dlg_windend2);
	pSimuWidgetDMS->WindDelta->setValue(dlg_winddelta2);

	pSimuWidgetDMS->PitchStart->setValue(dlg_pitchstart);
	pSimuWidgetDMS->PitchEnd->setValue(dlg_pitchend);
	pSimuWidgetDMS->PitchDelta->setValue(dlg_pitchdelta);

	pSimuWidgetDMS->RotStart->setValue(dlg_rotstart);
	pSimuWidgetDMS->RotEnd->setValue(dlg_rotend);
	pSimuWidgetDMS->RotDelta->setValue(dlg_rotdelta);



    ////size policy assures the widgets are resized according to content

	pSimuWidgetDMS->middleControls->setCurrentIndex(2);
	pSimuWidgetDMS->middleControls->currentWidget()->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

	if (g_mainFrame->m_iView == BEMSIMVIEW) pSimuWidgetDMS->middleControls->setCurrentIndex(0);// new JW, old: m_iView
	if (g_mainFrame->m_iView == TURBINEVIEW) pSimuWidgetDMS->middleControls->setCurrentIndex(1);// new JW, old: m_iView
	if (g_mainFrame->m_iView == CHARSIMVIEW) pSimuWidgetDMS->middleControls->setCurrentIndex(2);// new JW, old: m_iView

	pSimuWidgetDMS->middleControls->currentWidget()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	g_mainFrame->AziGraphAct->setVisible(true);

    if (m_pCurGraph){
        g_mainFrame->autoResetCurGraphScales->setChecked(!m_pCurGraph->m_autoReset);
    }

    if(!m_pCurGraph)
	{
        g_mainFrame->autoResetCurGraphScales->setChecked(false);
        g_mainFrame->BladeGraphAct->setEnabled(false);
		g_mainFrame->RotorGraphAct->setEnabled(false);
		g_mainFrame->AziGraphAct->setEnabled(false);
		g_mainFrame->WeibullGraphAct->setEnabled(false);
		g_mainFrame->MainWindAct->setEnabled(false);
		g_mainFrame->MainPitchAct->setEnabled(false);
		g_mainFrame->MainRotAct->setEnabled(false);
		g_mainFrame->ParamPitchAct->setEnabled(false);
		g_mainFrame->ParamWindAct->setEnabled(false);
		g_mainFrame->ParamRotAct->setEnabled(false);
		g_mainFrame->ParamNoneAct->setEnabled(false);

		g_mainFrame->BladeGraphAct->setChecked(false);
		g_mainFrame->RotorGraphAct->setChecked(false);
		g_mainFrame->AziGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(false);
		g_mainFrame->MainWindAct->setChecked(false);
		g_mainFrame->MainPitchAct->setChecked(false);
		g_mainFrame->MainRotAct->setChecked(false);
		g_mainFrame->ParamPitchAct->setChecked(false);
		g_mainFrame->ParamWindAct->setChecked(false);
		g_mainFrame->ParamRotAct->setChecked(false);
		g_mainFrame->ParamNoneAct->setChecked(false);
    }
    else if(m_pCurGraph->m_Type == 0)
    {
        g_mainFrame->BladeGraphAct->setEnabled(true);
        g_mainFrame->RotorGraphAct->setEnabled(true);
        g_mainFrame->AziGraphAct->setEnabled(true);
        g_mainFrame->BladeGraphAct->setChecked(false);
        g_mainFrame->RotorGraphAct->setChecked(true);
        g_mainFrame->AziGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(false);
    }
    else if(m_pCurGraph->m_Type == 1)
    {
        g_mainFrame->BladeGraphAct->setEnabled(true);
        g_mainFrame->RotorGraphAct->setEnabled(true);
        g_mainFrame->AziGraphAct->setEnabled(true);
        g_mainFrame->BladeGraphAct->setChecked(true);
        g_mainFrame->RotorGraphAct->setChecked(false);
        g_mainFrame->AziGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(false);
    }
    else if(m_pCurGraph->m_Type == 2)
    {
        g_mainFrame->BladeGraphAct->setEnabled(true);
        g_mainFrame->RotorGraphAct->setEnabled(true);
        g_mainFrame->AziGraphAct->setEnabled(true);
        g_mainFrame->BladeGraphAct->setChecked(false);
        g_mainFrame->RotorGraphAct->setChecked(false);
        g_mainFrame->AziGraphAct->setChecked(true);
		g_mainFrame->WeibullGraphAct->setChecked(false);
    }
	else if(m_pCurGraph->m_Type == 3)
	{
		g_mainFrame->BladeGraphAct->setEnabled(true);
		g_mainFrame->RotorGraphAct->setEnabled(true);
		g_mainFrame->AziGraphAct->setEnabled(true);
		g_mainFrame->BladeGraphAct->setChecked(false);
		g_mainFrame->RotorGraphAct->setChecked(false);
		g_mainFrame->AziGraphAct->setChecked(false);
		g_mainFrame->WeibullGraphAct->setChecked(true);
	}

	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
		g_mainFrame->WeibullGraphAct->setEnabled(true);
	else
		g_mainFrame->WeibullGraphAct->setEnabled(false);

	if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCurGraph&& m_pCDMSData)// new JW, old: m_iView
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

    if (g_mainFrame->m_iApp == DMS)
    {
            if(!m_bHideWidgets)
            {
				if (g_mainFrame->m_iView==BLADEVIEW)// new JW, old: m_iView
                {
					mainWidget->setCurrentIndex(0);
                    if (m_WingEdited) bladeWidget->setCurrentIndex(1);
                    else if (!m_WingEdited) bladeWidget->setCurrentIndex(0);

                }
				else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
				{
					if (m_TurbineEdited) mainWidget->setCurrentIndex(2);//PowWidget->setCurrentIndex(1);
					else if (!m_TurbineEdited) mainWidget->setCurrentIndex(1);//PowWidget->setCurrentIndex(0);
                }
                else
				{
					mainWidget->setCurrentIndex(0);
                }

            }

			if (g_mainFrame->m_iView == BLADEVIEW)// new JW, old: m_iView
			{
				if (!m_bHideWidgets)g_mainFrame->m_pctrlDMSWidget->show();
				if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidgetDMS->hide();
            }

			else if (g_mainFrame->m_iView == BEMSIMVIEW )// new JW, old: m_iView
			{
				if (!m_bHideWidgets)g_mainFrame->m_pctrlDMSWidget->hide();
				if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidgetDMS->show();

            }

			else if (g_mainFrame->m_iView == TURBINEVIEW )// new JW, old: m_iView
			{
				if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidgetDMS->show();
				if (!m_bHideWidgets)g_mainFrame->m_pctrlDMSWidget->show();
            }

			else if (g_mainFrame->m_iView == CHARSIMVIEW )// new JW, old: m_iView
			{
				if (!m_bHideWidgets)g_mainFrame->m_pctrlSimuWidgetDMS->show();
				if (!m_bHideWidgets)g_mainFrame->m_pctrlDMSWidget->hide();
			}

            if (m_bHideWidgets)
            {
                g_mainFrame->m_pctrlDMSWidget->hide();
				g_mainFrame->m_pctrlSimuWidgetDMS->hide();
            }
    }

}


void QDMS::CheckWing()
{
    bool finished = true;

    QString text;
    //// check if solidity < 1 at all stations

    for (int i=0;i<=m_pBlade->m_NPanel;i++)
    {
        if (m_pBlade->m_Polar.at(i) == "-----") finished = false;
        if (m_pBlade->m_Airfoils.at(i) == "-----") finished = false;
        if (!m_pBlade->m_bisSinglePolar && m_pBlade->m_Range.at(i) == "-----") finished = false;
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


void QDMS::Connect()
{
    //---------------------------------------------------------------------------------
    // wing design
    //---------------------------------------------------------------------------------
    connect(m_pctrlNewWing, SIGNAL(clicked()), this, SLOT (OnNewBlade()));
    connect(m_pctrlEditWing, SIGNAL(clicked()), this, SLOT (OnEditBlade()));
    connect(m_pctrlDeleteWing, SIGNAL(clicked()), this, SLOT (OnDeleteBlade()));

    connect(m_pctrlInsertBefore, SIGNAL(clicked()),this, SLOT(OnInsertBefore()));
    connect(m_pctrlInsertAfter, SIGNAL(clicked()),this, SLOT(OnInsertAfter()));
    connect(m_pctrlDeleteSection, SIGNAL(clicked()),this, SLOT(OnDeleteSection()));
    connect(m_pctrlScale, SIGNAL(clicked()), this, SLOT (OnScaleBlade()));
    connect(m_pctrlOptimize, SIGNAL(clicked()), SLOT(OnOptimize()));
    connect(m_pctrlBack, SIGNAL(clicked()), SLOT(OnDiscardBlade()));
    connect(m_pctrlSave, SIGNAL(clicked()),this, SLOT (OnSaveBlade()));

    //connect(m_pctrlIsOrtho, SIGNAL(clicked()),this, SLOT(OnOrtho()));
    connect(m_pctrlWingColor, SIGNAL(clicked()),this, SLOT(OnBladeColor()));
    connect(m_pctrlSectionColor, SIGNAL(clicked()),this, SLOT(OnSectionColor()));
    connect(m_pctrlSurfaces, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlOutline, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlOutlineEdge, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlAirfoils, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlAxes, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlPositions, SIGNAL(clicked()),this, SLOT(UpdateGeom()));
    connect(m_pctrlFoilNames, SIGNAL(clicked()),this, SLOT(UpdateGeom()));

    connect(m_SingleMultiGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnSingleMultiPolarChanged()));

    //connect(m_pctrlAdvancedDesign, SIGNAL(clicked()),this, SLOT(OnAdvancedDesign()));
    connect(m_pctrlBlades,  SIGNAL(valueChanged(int)), this, SLOT(OnCellChanged()));

    connect(m_pctrlPerspective, SIGNAL(clicked()), SLOT(onPerspectiveChanged()));
    connect(m_pctrlShowTurbine, SIGNAL(clicked()), SLOT(UpdateView()));
    connect(m_pctrlShowTurbine, SIGNAL(clicked()), SLOT(OnCenterScene()));



    //---------------------------------------------------------------------------------
    // VAWT simulation
    //---------------------------------------------------------------------------------
    connect(m_pctrlNewTurbine, SIGNAL(clicked()), SLOT(OnNewTurbine()));
    connect(m_pctrlEditTurbine, SIGNAL(clicked()), SLOT(OnEditTurbine()));
    connect(m_pctrlDeleteTurbine, SIGNAL(clicked()), SLOT(OnDeleteTurbine()));

    connect(m_pctrlSaveTurbine, SIGNAL(clicked()), SLOT(OnSaveTurbine()));
    connect(m_pctrlDiscardTurbine, SIGNAL(clicked()), SLOT(OnDiscardTurbine()));

	connect(m_pctrlWA2, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
	connect(m_pctrlWk2, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
	connect(m_pctrlPMA2, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));
	connect(m_pctrlPMk2, SIGNAL(valueChanged(double)), SLOT(OnSetWeibull()));

    connect(m_pctrlFixed, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
    connect(m_pctrlVariable, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));
    connect(m_pctrlPrescribedRot, SIGNAL(clicked()), SLOT(CheckTurbineButtons()));

    connect(m_pctrlRotList, SIGNAL(clicked()), SLOT(OnPrescribeRot()));

    connect(m_pctrlInvertBox, SIGNAL(clicked()),this, SLOT(InvertedClicked()));

    connect (m_heightLengthGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(OnLengthHeightChanged()));
    connect (hubEdit, SIGNAL(editingFinished()), this, SLOT(OnHubValChanged()));


}

void QDMS::OnHubValChanged(){
    if (!m_pBlade) return;

    m_pBlade->m_TOffsetX[0] = hubEdit->getValue() / g_mainFrame->m_mtoUnit;
    for (int i=0; i< m_pWingModel->rowCount();  i++) {
        ReadSectionData(i);
    }
    m_bResetglGeom = true;
    ComputeGeometry(true);
    UpdateView();

}

void QDMS::InvertedClicked(){

    m_bResetglGeom = true;
    m_bResetglSectionHighlight = true;
    m_pBlade->m_bIsInverted = m_pctrlInvertBox->isChecked();
    ComputeGeometry(true);
    UpdateView();

}

void QDMS::CheckTurbineButtons()
{
    m_pctrlRot2->setEnabled(!m_pctrlFixed->isChecked());
    m_pctrlLambda->setEnabled(m_pctrlVariable->isChecked());

    m_pctrlRot1Label->show();
    m_pctrlRot2Label->show();
//    m_pctrlFixedPitch->show();
//    m_pctrlFixedPitchLabel->show();
//    m_pctrlFixedPitchUnit->show();


//    if (m_pctrlPitch->isChecked())
//    {
//        m_pctrlGenerator->show();
//        m_pctrlGeneratorLabel->show();
//        power1->show();
//    }
//    else
//    {
//        m_pctrlGenerator->hide();
//        m_pctrlGeneratorLabel->hide();
//        power1->hide();
//    }


    if (m_pctrlVariable->isChecked())
    {
        m_pctrlRot1Label->setText(tr("Rot. Speed Min"));
        m_pctrlRot2Label->setText(tr("Rot. Speed Max"));
//        m_pctrlSwitchLabel->setText("");
//        m_pctrlSwitchLabel->hide();

        m_pctrlLambdaLabel->setText("TSR at Design Point");
        m_pctrlLambdaLabel->show();

        m_pctrlRot1->show();
        m_pctrlRot2->show();

        rotspeed1->show();
        rotspeed2->show();

        m_pctrlRotList->hide();


//        m_pctrlSwitch->hide();

//        speed3->hide();

        m_pctrlLambda->show();

    }
//    if (m_pctrl2Step->isChecked())
//    {
//        m_pctrlRot1Label->setText(tr("Rot. Speed 1"));
//        m_pctrlRot1Label->show();
//        m_pctrlRot2Label->setText(tr("Rot. Speed 2"));
//        m_pctrlRot2Label->show();

//        m_pctrlSwitchLabel->setText("V Switch");
//        m_pctrlSwitchLabel->show();

//        m_pctrlLambdaLabel->setText("");
//        m_pctrlLambdaLabel->hide();

//        m_pctrlRot1->show();
//        m_pctrlRot2->show();
//        rotspeed1->show();
//        rotspeed2->show();
//        m_pctrlSwitch->show();
//        speed3->show();

//        m_pctrlLambda->hide();

//    }
    if (m_pctrlFixed->isChecked())
    {
        m_pctrlRot1Label->setText(tr("Rot. Speed"));
        m_pctrlRot1Label->show();

        m_pctrlRot2Label->setText(tr(""));
        m_pctrlRot2Label->hide();

//        m_pctrlSwitchLabel->setText("");
//        m_pctrlSwitchLabel->hide();

        m_pctrlLambdaLabel->setText("");
        m_pctrlLambdaLabel->hide();

        m_pctrlRot1->show();
        rotspeed1->show();

        m_pctrlRot2->hide();
        rotspeed2->hide();

        m_pctrlRotList->hide();


//        m_pctrlSwitch->hide();
//        speed3->hide();

        m_pctrlLambda->hide();

    }

//    if (m_pctrlPrescribedPitch->isChecked())
//    {
//        m_pctrlPitchList->show();
//        m_pctrlFixedPitch->hide();
//        m_pctrlFixedPitchLabel->hide();
//        m_pctrlFixedPitchUnit->hide();
//    }
//    else
//    {
//        m_pctrlPitchList->hide();
//    }

    if (m_pctrlPrescribedRot->isChecked())
    {
        m_pctrlRotList->show();
        m_pctrlRot1->hide();
        m_pctrlRot2->hide();
        m_pctrlRot1Label->hide();
        m_pctrlRot2Label->hide();
//        m_pctrlSwitch->hide();
//        m_pctrlSwitchLabel->hide();
        rotspeed1->hide();
        rotspeed2->hide();
//        speed3->hide();
        m_pctrlLambdaLabel->hide();
        m_pctrlLambda->hide();
    }
    else
    {
        m_pctrlRotList->hide();
    }

}


void QDMS::CreateRotorCurves()
{
	m_RotorGraph1.DeleteCurves();
	m_RotorGraph2.DeleteCurves();
	m_RotorGraph3.DeleteCurves();



	if (m_RotorGraph1.m_Type != 2 && m_RotorGraph2.m_Type != 2 && m_RotorGraph3.m_Type != 2)
	{
		m_DMSToolBar->m_heightComboBox->setEnabled(false);
	}
	else if (m_DMSToolBar->m_heightComboBox->count()>=1)
	{
		m_DMSToolBar->m_heightComboBox->setEnabled(true);
	}


	for (int i=0;i<g_dmsdataStore.size();i++)
	{
		// rotor graph
		if (m_RotorGraph1.m_Type == 0)
		{

			if (g_dmsdataStore.at(i)->m_bIsVisible)
			{

				CCurve *pCurve = m_RotorGraph1.AddCurve();

				pCurve->ShowPoints(g_dmsdataStore.at(i)->m_bShowPoints);
				pCurve->SetStyle(g_dmsdataStore.at(i)->m_Style);
				pCurve->SetColor(g_dmsdataStore.at(i)->m_Color);
				pCurve->SetWidth(g_dmsdataStore.at(i)->m_Width);
				FillRotorCurve(pCurve, g_dmsdataStore.at(i), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
				pCurve->SetTitle(g_dmsdataStore.at(i)->m_DMSName);
			}

		} else if (m_pDMSData && m_pDData) {

			same_height = selected_height;

			if(m_bIsolateBladeCurve)
			{
				if (m_pDMSData->m_bIsVisible)
				{
					CCurve *pCurve = m_RotorGraph1.AddCurve();
					pCurve->ShowPoints(m_pDMSData->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_Style);
					pCurve->SetColor(m_pDMSData->m_Color);
					pCurve->SetWidth(m_pDMSData->m_Width);
					FillRotorCurve(pCurve, m_pDData, m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
					pCurve->SetTitle(m_pDData->m_DMSName);
				}

				if (m_bCompareBladeCurve)
				{
					for (int k=0; k<g_dmsdataStore.size();k++)
					{
						for (int l=0;l<g_dmsdataStore.at(k)->m_DData.size();l++)
						{
							if (g_dmsdataStore.at(k)->m_DData.at(l)->lambdaglobal == m_pDData->lambdaglobal)
							{
								if (g_dmsdataStore.at(k)->m_bIsVisible)
								{


									for (int z=0; z<g_dmsdataStore.at(k)->m_DData.at(l)->elements;z++)
									{
										if (fabs(g_dmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pDData->m_zeta.at(selected_height)) < 0.001)
										{
											same_height = z;
											CCurve *pCurve = m_RotorGraph1.AddCurve();

											pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
											pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
											pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
											pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
											FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
											pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}



									}
									if (m_RotorGraph1.m_Type == 1)
									{
										CCurve *pCurve = m_RotorGraph1.AddCurve();

										pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
										pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
										pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
										pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
										FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l),
													   m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
										pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
									}
								}
							}
						}
					}
				}
			}

			else
			{
				for (int j=0; j< m_pDMSData->m_DData.size();j++)
				{

					CCurve *pCurve = m_RotorGraph1.AddCurve();

					pCurve->ShowPoints(m_pDMSData->m_DData.at(j)->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_DData.at(j)->m_Style);
					pCurve->SetColor(m_pDMSData->m_DData.at(j)->m_Color);
					if (m_pDMSData->m_DData.at(j)==m_pDData)
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width+4);
					}
					else
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width);
					}
					FillRotorCurve(pCurve, m_pDMSData->m_DData.at(j), m_RotorGraph1.GetXVariable(), m_RotorGraph1.GetYVariable());
					pCurve->SetTitle(m_pDMSData->m_DData.at(j)->m_DMSName);
				}
			}
		}
	}

	for (int i=0;i<g_dmsdataStore.size();i++)
	{

		if (m_RotorGraph2.m_Type == 0)
		{

			if (g_dmsdataStore.at(i)->m_bIsVisible)
			{

				CCurve *pCurve = m_RotorGraph2.AddCurve();

				pCurve->ShowPoints(g_dmsdataStore.at(i)->m_bShowPoints);
				pCurve->SetStyle(g_dmsdataStore.at(i)->m_Style);
				pCurve->SetColor(g_dmsdataStore.at(i)->m_Color);
				pCurve->SetWidth(g_dmsdataStore.at(i)->m_Width);
				FillRotorCurve(pCurve, g_dmsdataStore.at(i), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
				pCurve->SetTitle(g_dmsdataStore.at(i)->m_DMSName);
			}
		}
		else if (m_pDMSData && m_pDData)
		{
			same_height = selected_height;

			if(m_bIsolateBladeCurve)
			{
				if (m_pDMSData->m_bIsVisible)
				{
					CCurve *pCurve = m_RotorGraph2.AddCurve();

					pCurve->ShowPoints(m_pDMSData->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_Style);
					pCurve->SetColor(m_pDMSData->m_Color);
					pCurve->SetWidth(m_pDMSData->m_Width);
					FillRotorCurve(pCurve, m_pDData, m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
					pCurve->SetTitle(m_pDData->m_DMSName);
				}

				if (m_bCompareBladeCurve)
				{
					for (int k=0; k<g_dmsdataStore.size();k++)
					{
						for (int l=0;l<g_dmsdataStore.at(k)->m_DData.size();l++)
						{
							if (g_dmsdataStore.at(k)->m_DData.at(l)->lambdaglobal == m_pDData->lambdaglobal)
							{
								if (g_dmsdataStore.at(k)->m_bIsVisible)
								{
									for (int z=0; z<g_dmsdataStore.at(k)->m_DData.at(l)->elements;z++)
									{
										if (fabs(g_dmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pDData->m_zeta.at(selected_height)) < 0.001)
										{
											same_height = z;
											CCurve *pCurve = m_RotorGraph2.AddCurve();

											pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
											pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
											pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
											pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
											FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
											pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}



									}
									if (m_RotorGraph2.m_Type == 1)
									{
										CCurve *pCurve = m_RotorGraph2.AddCurve();

										pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
										pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
										pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
										pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
										FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
										pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
									}

								}
							}
						}
					}
				}
			}
			else
			{
				for (int j=0; j< m_pDMSData->m_DData.size();j++)
				{

					CCurve *pCurve = m_RotorGraph2.AddCurve();
					pCurve->ShowPoints(m_pDMSData->m_DData.at(j)->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_DData.at(j)->m_Style);
					pCurve->SetColor(m_pDMSData->m_DData.at(j)->m_Color);
					if (m_pDMSData->m_DData.at(j)==m_pDData)
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width+4);
					}
					else
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width);
					}
					FillRotorCurve(pCurve, m_pDMSData->m_DData.at(j), m_RotorGraph2.GetXVariable(), m_RotorGraph2.GetYVariable());
					pCurve->SetTitle(m_pDMSData->m_DData.at(j)->m_DMSName);
				}
			}
		}
	}


	for (int i=0;i<g_dmsdataStore.size();i++)
	{
		if (m_RotorGraph3.m_Type == 0)
		{

			if (g_dmsdataStore.at(i)->m_bIsVisible)
			{

				CCurve *pCurve = m_RotorGraph3.AddCurve();

				pCurve->ShowPoints(g_dmsdataStore.at(i)->m_bShowPoints);
				pCurve->SetStyle(g_dmsdataStore.at(i)->m_Style);
				pCurve->SetColor(g_dmsdataStore.at(i)->m_Color);
				pCurve->SetWidth(g_dmsdataStore.at(i)->m_Width);
				FillRotorCurve(pCurve, g_dmsdataStore.at(i), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
				pCurve->SetTitle(g_dmsdataStore.at(i)->m_DMSName);
			}
		}
		else if (m_pDMSData && m_pDData)
		{
			same_height = selected_height;

			if(m_bIsolateBladeCurve)
			{
				if (m_pDMSData->m_bIsVisible)
				{
					CCurve *pCurve = m_RotorGraph3.AddCurve();

					pCurve->ShowPoints(m_pDMSData->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_Style);
					pCurve->SetColor(m_pDMSData->m_Color);
					pCurve->SetWidth(m_pDMSData->m_Width);
					FillRotorCurve(pCurve, m_pDData, m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
					pCurve->SetTitle(m_pDData->m_DMSName);
				}

				if (m_bCompareBladeCurve)
				{
					for (int k=0; k<g_dmsdataStore.size();k++)
					{
						for (int l=0;l<g_dmsdataStore.at(k)->m_DData.size();l++)
						{
							if (g_dmsdataStore.at(k)->m_DData.at(l)->lambdaglobal == m_pDData->lambdaglobal)
							{
								if (g_dmsdataStore.at(k)->m_bIsVisible)
								{
									for (int z=0; z<g_dmsdataStore.at(k)->m_DData.at(l)->elements;z++)
									{
										if (fabs(g_dmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pDData->m_zeta.at(selected_height)) < 0.001)
										{
											same_height = z;
											CCurve *pCurve = m_RotorGraph3.AddCurve();

											pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
											pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
											pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
											pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
											FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
											pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}
									}
									if (m_RotorGraph3.m_Type == 1)
									{
										CCurve *pCurve = m_RotorGraph3.AddCurve();

										pCurve->ShowPoints(g_dmsdataStore.at(k)->m_bShowPoints);
										pCurve->SetStyle(g_dmsdataStore.at(k)->m_Style);
										pCurve->SetColor(g_dmsdataStore.at(k)->m_Color);
										pCurve->SetWidth(g_dmsdataStore.at(k)->m_Width);
										FillRotorCurve(pCurve, g_dmsdataStore.at(k)->m_DData.at(l), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
										pCurve->SetTitle(g_dmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
									}

								}
							}
						}
					}
				}
			}
			else
			{
				for (int j=0; j< m_pDMSData->m_DData.size();j++)
				{
					CCurve *pCurve = m_RotorGraph3.AddCurve();

					pCurve->ShowPoints(m_pDMSData->m_DData.at(j)->m_bShowPoints);
					pCurve->SetStyle(m_pDMSData->m_DData.at(j)->m_Style);
					pCurve->SetColor(m_pDMSData->m_DData.at(j)->m_Color);
					if (m_pDMSData->m_DData.at(j)==m_pDData)
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width+4);
					}
					else
					{
						pCurve->SetWidth(m_pDMSData->m_DData.at(j)->m_Width);
					}
					FillRotorCurve(pCurve, m_pDMSData->m_DData.at(j), m_RotorGraph3.GetXVariable(), m_RotorGraph3.GetYVariable());
					pCurve->SetTitle(m_pDMSData->m_DData.at(j)->m_DMSName);
				}
			}
		}
	}


	if (m_pDMSData && selected_lambda >= 0 && m_bShowOpPoint)
	{

		if (m_pDMSData->m_bIsVisible && m_RotorGraph1.m_Type == 0)
		{
			CCurve* pRotorCurve = m_RotorGraph1.AddCurve();
			pRotorCurve->ShowPoints(true);
			pRotorCurve->SetWidth(m_pDMSData->m_Width+3);
			pRotorCurve->SetColor(m_pDMSData->m_Color);
			QList <double> *X = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph1.GetXVariable());
			QList <double> *Y = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph1.GetYVariable());
			pRotorCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
		}

		if (m_pDMSData->m_bIsVisible && m_RotorGraph2.m_Type == 0)
		{
			CCurve* pRotorCurve = m_RotorGraph2.AddCurve();
			pRotorCurve->ShowPoints(true);
			pRotorCurve->SetWidth(m_pDMSData->m_Width+3);
			pRotorCurve->SetColor(m_pDMSData->m_Color);
			QList <double> *X = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph2.GetXVariable());
			QList <double> *Y = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph2.GetYVariable());
			pRotorCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
		}

		if (m_pDMSData->m_bIsVisible && m_RotorGraph3.m_Type == 0)
		{
			CCurve* pRotorCurve = m_RotorGraph3.AddCurve();
			pRotorCurve->ShowPoints(true);
			pRotorCurve->SetWidth(m_pDMSData->m_Width+3);
			pRotorCurve->SetColor(m_pDMSData->m_Color);
			QList <double> *X = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph3.GetXVariable());
			QList <double> *Y = (QList <double> *) GetRotorVariable(m_pDMSData, m_RotorGraph3.GetYVariable());
			pRotorCurve->AddPoint(X->at(selected_lambda),Y->at(selected_lambda));
		}

	}


	UpdateView();
}


void QDMS::CreateCharacteristicsCurves()
{
	CCurve *pCurve;
	
	float ***pX;
	float ***pY;
	
	m_CharGraph1.DeleteCurves();
	m_CharGraph2.DeleteCurves();
	m_CharGraph3.DeleteCurves();
	m_CharGraph4.DeleteCurves();
	
//	m_CharGraph1.SetAuto(true);
//	m_CharGraph2.SetAuto(true);
//	m_CharGraph3.SetAuto(true);
//	m_CharGraph4.SetAuto(true);
	
	if(m_pCDMSData && m_pCDMSData->simulated)
	{
		int all = -1;
		
		if (m_CharGraph1.m_Param == 0) all = m_pCDMSData->windtimes;
		if (m_CharGraph1.m_Param == 1) all = m_pCDMSData->rottimes;
		if (m_CharGraph1.m_Param == 2) all = m_pCDMSData->pitchtimes;
		if (m_CharGraph1.m_Param == -1) all = 1;
		
		for (int i=0; i<all;i++)
		{
			pCurve = m_CharGraph1.AddCurve();
			
			pCurve->ShowPoints(m_pCDMSData->m_bShowPoints);
			pCurve->SetStyle(m_pCDMSData->m_Style);
			if (all == 1) pCurve->SetColor(m_pCDMSData->m_Color);
			else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
			pCurve->SetWidth(m_pCDMSData->m_Width);
			FillCharacteristicCurve(pCurve, m_pCDMSData, m_CharGraph1.GetXVariable(), m_CharGraph1.GetYVariable(),i);
			pCurve->SetTitle(m_pCDMSData->m_SimName);
		}
		
		if (m_bShowOpPoint)
		{
			pCurve = m_CharGraph1.AddCurve();
			pCurve->ShowPoints(true);
			pCurve->SetWidth(m_pCDMSData->m_Width+3);
			if (all == 1)
			{
				pCurve->SetColor(m_pCDMSData->m_Color);
			}
			else
			{
				if (m_CharGraph1.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
				if (m_CharGraph1.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
				if (m_CharGraph1.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
			}
			pX = GetCharVariable(m_pCDMSData, m_CharGraph1.GetXVariable());
			pY = GetCharVariable(m_pCDMSData, m_CharGraph1.GetYVariable());
			pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
		}
		
		if (m_CharGraph2.m_Param == 0) all = m_pCDMSData->windtimes;
		if (m_CharGraph2.m_Param == 1) all = m_pCDMSData->rottimes;
		if (m_CharGraph2.m_Param == 2) all = m_pCDMSData->pitchtimes;
		if (m_CharGraph2.m_Param == -1) all = 1;
		
		for (int i=0; i<all;i++)
		{
			pCurve = m_CharGraph2.AddCurve();
			
			pCurve->ShowPoints(m_pCDMSData->m_bShowPoints);
			pCurve->SetStyle(m_pCDMSData->m_Style);
			if (all == 1) pCurve->SetColor(m_pCDMSData->m_Color);
			else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
			pCurve->SetWidth(m_pCDMSData->m_Width);
			FillCharacteristicCurve(pCurve, m_pCDMSData, m_CharGraph2.GetXVariable(), m_CharGraph2.GetYVariable(),i);
			pCurve->SetTitle(m_pCDMSData->m_SimName);
		}
		if (m_bShowOpPoint)
		{
			pCurve = m_CharGraph2.AddCurve();
			pCurve->ShowPoints(true);
			pCurve->SetWidth(m_pCDMSData->m_Width+3);
			if (all == 1)
			{
				pCurve->SetColor(m_pCDMSData->m_Color);
			}
			else
			{
				if (m_CharGraph2.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
				if (m_CharGraph2.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
				if (m_CharGraph2.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
			}
			pX =  GetCharVariable(m_pCDMSData, m_CharGraph2.GetXVariable());
			pY =  GetCharVariable(m_pCDMSData, m_CharGraph2.GetYVariable());
			pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
		}
		
		if (m_CharGraph3.m_Param == 0) all = m_pCDMSData->windtimes;
		if (m_CharGraph3.m_Param == 1) all = m_pCDMSData->rottimes;
		if (m_CharGraph3.m_Param == 2) all = m_pCDMSData->pitchtimes;
		if (m_CharGraph3.m_Param == -1) all = 1;
		
		for (int i=0; i<all;i++)
		{
			pCurve = m_CharGraph3.AddCurve();
			
			pCurve->ShowPoints(m_pCDMSData->m_bShowPoints);
			pCurve->SetStyle(m_pCDMSData->m_Style);
			if (all == 1) pCurve->SetColor(m_pCDMSData->m_Color);
			else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
			pCurve->SetWidth(m_pCDMSData->m_Width);
			FillCharacteristicCurve(pCurve, m_pCDMSData, m_CharGraph3.GetXVariable(), m_CharGraph3.GetYVariable(),i);
			pCurve->SetTitle(m_pCDMSData->m_SimName);
			
			
		}
		
		if (m_bShowOpPoint)
		{
			pCurve = m_CharGraph3.AddCurve();
			pCurve->ShowPoints(true);
			pCurve->SetWidth(m_pCDMSData->m_Width+3);
			if (all == 1)
			{
				pCurve->SetColor(m_pCDMSData->m_Color);
			}
			else
			{
				if (m_CharGraph3.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
				if (m_CharGraph3.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
				if (m_CharGraph3.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
			}
			pX=  GetCharVariable(m_pCDMSData, m_CharGraph3.GetXVariable());
			pY=  GetCharVariable(m_pCDMSData, m_CharGraph3.GetYVariable());
			pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
		}
		
		if (m_CharGraph4.m_Param == 0) all = m_pCDMSData->windtimes;
		if (m_CharGraph4.m_Param == 1) all = m_pCDMSData->rottimes;
		if (m_CharGraph4.m_Param == 2) all = m_pCDMSData->pitchtimes;
		if (m_CharGraph4.m_Param == -1) all = 1;
		
		for (int i=0; i<all;i++)
		{
			pCurve = m_CharGraph4.AddCurve();
			
			pCurve->ShowPoints(m_pCDMSData->m_bShowPoints);
			pCurve->SetStyle(m_pCDMSData->m_Style);
			if (all == 1) pCurve->SetColor(m_pCDMSData->m_Color);
			else pCurve->SetColor(g_mainFrame->m_crColors[i%24]);
			pCurve->SetWidth(m_pCDMSData->m_Width);
			FillCharacteristicCurve(pCurve, m_pCDMSData, m_CharGraph4.GetXVariable(), m_CharGraph4.GetYVariable(),i);
			pCurve->SetTitle(m_pCDMSData->m_SimName);
			
			
		}
		
		if (m_bShowOpPoint)
		{
			pCurve = m_CharGraph4.AddCurve();
			pCurve->ShowPoints(true);
			pCurve->SetWidth(m_pCDMSData->m_Width+3);
			if (all == 1)
			{
				pCurve->SetColor(m_pCDMSData->m_Color);
			}
			else
			{
				if (m_CharGraph4.m_Param == 0) pCurve->SetColor(g_mainFrame->m_crColors[selected_wind%24]);
				if (m_CharGraph4.m_Param == 1) pCurve->SetColor(g_mainFrame->m_crColors[selected_rot%24]);
				if (m_CharGraph4.m_Param == 2) pCurve->SetColor(g_mainFrame->m_crColors[selected_pitch%24]);
			}
			pX=  GetCharVariable(m_pCDMSData, m_CharGraph4.GetXVariable());
			pY=  GetCharVariable(m_pCDMSData, m_CharGraph4.GetYVariable());
			pCurve->AddPoint(pX[selected_wind][selected_rot][selected_pitch],pY[selected_wind][selected_rot][selected_pitch]);
		}
		
	}
	
	UpdateView();
}

void QDMS::CreatePowerCurves()
{

    m_PowerGraph1.DeleteCurves();
    m_PowerGraph2.DeleteCurves();
    m_PowerGraph3.DeleteCurves();

   

	if (m_PowerGraph1.m_Type != 2 && m_PowerGraph2.m_Type != 2 && m_PowerGraph3.m_Type != 2)
	{
		m_DMSToolBar->m_turbineheightComboBox->setEnabled(false);
    }
	else if (m_DMSToolBar->m_turbineheightComboBox->count()>=1)
    {
		m_DMSToolBar->m_turbineheightComboBox->setEnabled(true);
    }

	for (int i=0;i<g_tdmsdataStore.size();i++)
    {
        if (m_PowerGraph1.m_Type == 0) {
			if (g_tdmsdataStore.at(i)->m_bIsVisible) {
				CCurve *pCurve = m_PowerGraph1.AddCurve();
	
				pCurve->ShowPoints(g_tdmsdataStore.at(i)->m_bShowPoints);
				pCurve->SetStyle(g_tdmsdataStore.at(i)->m_Style);
				pCurve->SetColor(g_tdmsdataStore.at(i)->m_Color);
				pCurve->SetWidth(g_tdmsdataStore.at(i)->m_Width);
				FillPowerCurve(pCurve, g_tdmsdataStore.at(i), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
				pCurve->SetTitle(g_tdmsdataStore.at(i)->m_TurbineName);
            }
        } else if (m_pTDMSData && m_pTurbineDData) {
			same_height = selected_height;

			if (m_PowerGraph1.m_Type == 3) {
				for (int k=0; k<g_tdmsdataStore.size();k++) {
					if (g_tdmsdataStore.at(k)->m_bIsVisible) {
						CCurve *pCurve = m_PowerGraph1.AddCurve();

						pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
						pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
						pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
						pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);

						FillWeibullCurve(pCurve, g_tdmsdataStore.at(k), m_PowerGraph1.GetXVariable());
						pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DMSName);
					}
				}
			} else {

				if(m_bIsolateBladeCurve)
				{

					if (m_pTDMSData->m_bIsVisible)
					{
					CCurve *pCurve = m_PowerGraph1.AddCurve();

					pCurve->ShowPoints(m_pTDMSData->m_bShowPoints);
					pCurve->SetStyle(m_pTDMSData->m_Style);
					pCurve->SetColor(m_pTDMSData->m_Color);
					pCurve->SetWidth(m_pTDMSData->m_Width);
					FillPowerCurve(pCurve, m_pTurbineDData, m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
					pCurve->SetTitle(m_pTurbineDData->m_DMSName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tdmsdataStore.size();k++)
							{
							for (int l=0;l<g_tdmsdataStore.at(k)->m_DData.size();l++)
							 {
							 if (g_tdmsdataStore.at(k)->m_DData.at(l)->windspeedStr == m_pTurbineDData->windspeedStr)
								{
								if (g_tdmsdataStore.at(k)->m_bIsVisible)
								 {
									for (int z=0; z<g_tdmsdataStore.at(k)->m_DData.at(l)->elements;z++)
									{
										if (fabs(g_tdmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pTurbineDData->m_zeta.at(selected_height)) < 0.001)
										{
												same_height = z;
												CCurve *pCurve = m_PowerGraph1.AddCurve();

												pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
												pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
												pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
												pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
												FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
												pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}
									}
									if (m_PowerGraph1.m_Type == 1)
									{
										CCurve *pCurve = m_PowerGraph1.AddCurve();

										pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
										pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
										pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
										pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
										FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
										pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
									}

								 }
								}
							 }
							}
					}
				}
				else
				{
					for (int j=0; j< m_pTDMSData->m_DData.size();j++)
					{

						CCurve *pCurve = m_PowerGraph1.AddCurve();

						pCurve->ShowPoints(m_pTDMSData->m_DData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTDMSData->m_DData.at(j)->m_Style);
						pCurve->SetColor(m_pTDMSData->m_DData.at(j)->m_Color);
						if (m_pTDMSData->m_DData.at(j)==m_pTurbineDData)
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTDMSData->m_DData.at(j), m_PowerGraph1.GetXVariable(), m_PowerGraph1.GetYVariable());
						pCurve->SetTitle(m_pTDMSData->m_DData.at(j)->m_DMSName);
					}
				}

			}

		}
	}

	for (int i=0;i<g_tdmsdataStore.size();i++)
    {
        if (m_PowerGraph2.m_Type == 0)
        {

			if (g_tdmsdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_PowerGraph2.AddCurve();

			pCurve->ShowPoints(g_tdmsdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_tdmsdataStore.at(i)->m_Style);
			pCurve->SetColor(g_tdmsdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_tdmsdataStore.at(i)->m_Width);
			FillPowerCurve(pCurve, g_tdmsdataStore.at(i), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
			pCurve->SetTitle(g_tdmsdataStore.at(i)->m_TurbineName);
            }
        }

        else if (m_pTDMSData && m_pTurbineDData)
        {
			same_height = selected_height;

			if (m_PowerGraph2.m_Type == 3)
			{
				for (int k=0; k<g_tdmsdataStore.size();k++)
				{
					if (g_tdmsdataStore.at(k)->m_bIsVisible)
					{
						CCurve *pCurve = m_PowerGraph2.AddCurve();

						pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
						pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
						pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
						pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);

						FillWeibullCurve(pCurve, g_tdmsdataStore.at(k), m_PowerGraph2.GetXVariable());
						pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DMSName);

					}
				}

			}

			else
			{
				if(m_bIsolateBladeCurve)
				{
					CCurve *pCurve = m_PowerGraph2.AddCurve();

					if (m_pTDMSData->m_bIsVisible)
					{
						pCurve->ShowPoints(m_pTDMSData->m_bShowPoints);
						pCurve->SetStyle(m_pTDMSData->m_Style);
						pCurve->SetColor(m_pTDMSData->m_Color);
						pCurve->SetWidth(m_pTDMSData->m_Width);
						FillPowerCurve(pCurve, m_pTurbineDData, m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
						pCurve->SetTitle(m_pTurbineDData->m_DMSName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tdmsdataStore.size();k++)
						 {
							for (int l=0;l<g_tdmsdataStore.at(k)->m_DData.size();l++)
							{
								if (g_tdmsdataStore.at(k)->m_DData.at(l)->windspeedStr == m_pTurbineDData->windspeedStr)
								{
									if (g_tdmsdataStore.at(k)->m_bIsVisible)
									{
										for (int z=0; z<g_tdmsdataStore.at(k)->m_DData.at(l)->elements;z++)
										{
											if (fabs(g_tdmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pTurbineDData->m_zeta.at(selected_height)) < 0.001)
											{
													same_height = z;
													CCurve *pCurve = m_PowerGraph2.AddCurve();

													pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
													pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
													pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
													pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
													FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
													pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
											}
										}
										if (m_PowerGraph2.m_Type == 1)
										{
											CCurve *pCurve = m_PowerGraph2.AddCurve();

											pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
											pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
											pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
											pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
											FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
											pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}

									}
								}
							}
						 }
					}
				}
				else
				{
					for (int j=0; j< m_pTDMSData->m_DData.size();j++)
					{
						CCurve *pCurve = m_PowerGraph2.AddCurve();

						pCurve->ShowPoints(m_pTDMSData->m_DData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTDMSData->m_DData.at(j)->m_Style);
						pCurve->SetColor(m_pTDMSData->m_DData.at(j)->m_Color);
						if (m_pTDMSData->m_DData.at(j)==m_pTurbineDData)
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTDMSData->m_DData.at(j), m_PowerGraph2.GetXVariable(), m_PowerGraph2.GetYVariable());
						pCurve->SetTitle(m_pTDMSData->m_DData.at(j)->m_DMSName);
					}
				}

			}

        }
    }

	for (int i=0;i<g_tdmsdataStore.size();i++)
    {
        if (m_PowerGraph3.m_Type == 0)
        {

			if (g_tdmsdataStore.at(i)->m_bIsVisible)
            {

            CCurve *pCurve = m_PowerGraph3.AddCurve();

			pCurve->ShowPoints(g_tdmsdataStore.at(i)->m_bShowPoints);
			pCurve->SetStyle(g_tdmsdataStore.at(i)->m_Style);
			pCurve->SetColor(g_tdmsdataStore.at(i)->m_Color);
			pCurve->SetWidth(g_tdmsdataStore.at(i)->m_Width);
			FillPowerCurve(pCurve, g_tdmsdataStore.at(i), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
			pCurve->SetTitle(g_tdmsdataStore.at(i)->m_TurbineName);
            }
        }

        else if (m_pTDMSData && m_pTurbineDData)
        {
			same_height = selected_height;

			if (m_PowerGraph3.m_Type == 3)
			{
				for (int k=0; k<g_tdmsdataStore.size();k++)
				{
					if (g_tdmsdataStore.at(k)->m_bIsVisible)
					{
						CCurve *pCurve = m_PowerGraph3.AddCurve();

						pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
						pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
						pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
						pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);

						FillWeibullCurve(pCurve, g_tdmsdataStore.at(k), m_PowerGraph3.GetXVariable());
						pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DMSName);

					}
				}
			}

			else
			{
				if(m_bIsolateBladeCurve)
				{
					if (m_pTDMSData->m_bIsVisible)
					{
						CCurve *pCurve = m_PowerGraph3.AddCurve();

						pCurve->ShowPoints(m_pTDMSData->m_bShowPoints);
						pCurve->SetStyle(m_pTDMSData->m_Style);
						pCurve->SetColor(m_pTDMSData->m_Color);
						pCurve->SetWidth(m_pTDMSData->m_Width);
						FillPowerCurve(pCurve, m_pTurbineDData, m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
						pCurve->SetTitle(m_pTurbineDData->m_DMSName);
					}

					if (m_bCompareBladeCurve)
					{
						 for (int k=0; k<g_tdmsdataStore.size();k++)
						 {
							for (int l=0;l<g_tdmsdataStore.at(k)->m_DData.size();l++)
							{
								if (g_tdmsdataStore.at(k)->m_DData.at(l)->windspeedStr == m_pTurbineDData->windspeedStr)
								{
									if (g_tdmsdataStore.at(k)->m_bIsVisible)
									{
										for (int z=0; z<g_tdmsdataStore.at(k)->m_DData.at(l)->elements;z++)
										{
											if (fabs(g_tdmsdataStore.at(k)->m_DData.at(l)->m_zeta.at(z)-m_pTurbineDData->m_zeta.at(selected_height)) < 0.001)
											{
													same_height = z;
													CCurve *pCurve = m_PowerGraph3.AddCurve();

													pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
													pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
													pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
													pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
													FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
													pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
											}
										}
										if (m_PowerGraph3.m_Type == 1)
										{
											CCurve *pCurve = m_PowerGraph3.AddCurve();

											pCurve->ShowPoints(g_tdmsdataStore.at(k)->m_bShowPoints);
											pCurve->SetStyle(g_tdmsdataStore.at(k)->m_Style);
											pCurve->SetColor(g_tdmsdataStore.at(k)->m_Color);
											pCurve->SetWidth(g_tdmsdataStore.at(k)->m_Width);
											FillPowerCurve(pCurve, g_tdmsdataStore.at(k)->m_DData.at(l), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
											pCurve->SetTitle(g_tdmsdataStore.at(k)->m_DData.at(l)->m_DMSName);
										}
									}
								}
							 }
						 }
					}
				}
				else
				{
					for (int j=0; j< m_pTDMSData->m_DData.size();j++)
					{

						CCurve *pCurve = m_PowerGraph3.AddCurve();

						pCurve->ShowPoints(m_pTDMSData->m_DData.at(j)->m_bShowPoints);
						pCurve->SetStyle(m_pTDMSData->m_DData.at(j)->m_Style);
						pCurve->SetColor(m_pTDMSData->m_DData.at(j)->m_Color);
						if (m_pTDMSData->m_DData.at(j)==m_pTurbineDData)
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width+4);
						}
						else
						{
							pCurve->SetWidth(m_pTDMSData->m_DData.at(j)->m_Width);
						}
						FillPowerCurve(pCurve, m_pTDMSData->m_DData.at(j), m_PowerGraph3.GetXVariable(), m_PowerGraph3.GetYVariable());
						pCurve->SetTitle(m_pTDMSData->m_DData.at(j)->m_DMSName);
					}
				}
			}
        }
    }



    if (m_pTDMSData && selected_windspeed >= 0 && m_bShowOpPoint)
    {

        if (m_pTDMSData->m_bIsVisible && m_PowerGraph1.m_Type == 0)
        {
            CCurve* pPowerCurve = m_PowerGraph1.AddCurve();
            pPowerCurve->ShowPoints(true);
            pPowerCurve->SetWidth(m_pTDMSData->m_Width+3);
            pPowerCurve->SetColor(m_pTDMSData->m_Color);
            QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph1.GetXVariable());
            QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph1.GetYVariable());
            pPowerCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
        }

        if (m_pTDMSData->m_bIsVisible && m_PowerGraph2.m_Type == 0)
        {
            CCurve* pPowerCurve = m_PowerGraph2.AddCurve();
            pPowerCurve->ShowPoints(true);
            pPowerCurve->SetWidth(m_pTDMSData->m_Width+3);
            pPowerCurve->SetColor(m_pTDMSData->m_Color);
            QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph2.GetXVariable());
            QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph2.GetYVariable());
            pPowerCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
        }

        if (m_pTDMSData->m_bIsVisible && m_PowerGraph3.m_Type == 0)
        {
            CCurve* pPowerCurve = m_PowerGraph3.AddCurve();
            pPowerCurve->ShowPoints(true);
            pPowerCurve->SetWidth(m_pTDMSData->m_Width+3);
            pPowerCurve->SetColor(m_pTDMSData->m_Color);
            QList <double> *X = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph3.GetXVariable());
            QList <double> *Y = (QList <double> *) GetTurbineRotorVariable(m_pTDMSData, m_PowerGraph3.GetYVariable());
            pPowerCurve->AddPoint(X->at(selected_windspeed),Y->at(selected_windspeed));
        }

    }


    UpdateView();
}

void QDMS::DisableAllButtons()
{
	m_DMSToolBar->DisableAll();

	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

    g_mainFrame->BEMViewMenu->setEnabled(false);
    g_mainFrame->BEM360PolarMenu->setEnabled(false);
    g_mainFrame->BEMBladeMenu->setEnabled(false);
    g_mainFrame->BEMCtxMenu->setEnabled(false);
    g_mainFrame->BEMTurbine->setEnabled(false);
    g_mainFrame->optionsMenu->setEnabled(false);
    g_mainFrame->fileMenu->setEnabled(false);

	g_mainFrame->m_pctrlMainToolBar->setEnabled(false);


	pSimuWidgetDMS->m_pctrlDefineTurbineSim->setEnabled(false);
	pSimuWidgetDMS->m_pctrlStartTurbineSim->setEnabled(false);
	pSimuWidgetDMS->m_pctrlWind1->setEnabled(false);
	pSimuWidgetDMS->m_pctrlWind2->setEnabled(false);
	pSimuWidgetDMS->m_pctrlWindDelta->setEnabled(false);
    pSimuWidgetDMS->m_pctrlDeleteTDMS->setEnabled(false);


	pSimuWidgetDMS->m_pctrlCreateDMS->setEnabled(false);
	pSimuWidgetDMS->m_pctrlStartDMS->setEnabled(false);
	pSimuWidgetDMS->m_pctrlLSLineEdit->setEnabled(false);
	pSimuWidgetDMS->m_pctrlLELineEdit->setEnabled(false);
	pSimuWidgetDMS->m_pctrlLDLineEdit->setEnabled(false);
    pSimuWidgetDMS->m_pctrlWindspeed->setEnabled(false);
    pSimuWidgetDMS->m_pctrlDeleteDMS->setEnabled(false);


}


void QDMS::EnableAllButtons()
{
	m_DMSToolBar->EnableAll();

    g_mainFrame->BEMViewMenu->setEnabled(true);
    g_mainFrame->BEM360PolarMenu->setEnabled(true);
    g_mainFrame->BEMBladeMenu->setEnabled(true);
    g_mainFrame->BEMCtxMenu->setEnabled(true);
    g_mainFrame->BEMTurbine->setEnabled(true);

    g_mainFrame->m_pctrlMainToolBar->setEnabled(true);

    g_mainFrame->fileMenu->setEnabled(true);
    g_mainFrame->optionsMenu->setEnabled(true);

}


void QDMS::FillComboBoxes(bool bEnable)
{

	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

        if(!bEnable)
        {
				pSimuWidgetDMS->m_pctrlCurveColor->setEnabled(false);
				pSimuWidgetDMS->m_pctrlCurveStyle->setEnabled(false);
				pSimuWidgetDMS->m_pctrlCurveWidth->setEnabled(false);
				pSimuWidgetDMS->m_pctrlShowCurve->setEnabled(false);
				pSimuWidgetDMS->m_pctrlShowPoints->setEnabled(false);
				pSimuWidgetDMS->m_pctrlShowOpPoint->setEnabled(false);
        }
        else
        {
				pSimuWidgetDMS->m_pctrlCurveColor->setEnabled(true);
				pSimuWidgetDMS->m_pctrlCurveStyle->setEnabled(true);
				pSimuWidgetDMS->m_pctrlCurveWidth->setEnabled(true);
				pSimuWidgetDMS->m_pctrlShowCurve->setEnabled(true);
				pSimuWidgetDMS->m_pctrlShowPoints->setEnabled(true);
				pSimuWidgetDMS->m_pctrlShowOpPoint->setEnabled(true);
        }

        int LineWidth[5];
        for (int i=0; i<5;i++) LineWidth[i] = m_CurveWidth;
		pSimuWidgetDMS->m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles
		pSimuWidgetDMS->m_pStyleDelegate->SetLineColor(m_CurveColor);

        int LineStyle[5];
        for (int i=0; i<5;i++) LineStyle[i] = m_CurveStyle;
		pSimuWidgetDMS->m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
		pSimuWidgetDMS->m_pWidthDelegate->SetLineColor(m_CurveColor);

		pSimuWidgetDMS->m_pctrlCurveStyle->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);
		pSimuWidgetDMS->m_pctrlCurveWidth->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);

		pSimuWidgetDMS->m_pctrlCurveColor->SetColor(m_CurveColor);
		pSimuWidgetDMS->m_pctrlCurveColor->SetStyle(m_CurveStyle);
		pSimuWidgetDMS->m_pctrlCurveColor->SetWidth(m_CurveWidth);

		pSimuWidgetDMS->m_pctrlCurveStyle->update();
		pSimuWidgetDMS->m_pctrlCurveWidth->update();
		pSimuWidgetDMS->m_pctrlCurveColor->update();

		pSimuWidgetDMS->m_pctrlCurveStyle->setCurrentIndex(m_CurveStyle);
		pSimuWidgetDMS->m_pctrlCurveWidth->setCurrentIndex(m_CurveWidth-1);

}

void QDMS::OnLengthHeightChanged(){

    if (!m_pBlade) return;

    if (m_heightLengthGroup->button(0)->isChecked()) m_bisHeight = true;
    else m_bisHeight = false;

    QString str;
    GetLengthUnit(str, g_mainFrame->m_LengthUnit);

    if (!m_bisHeight){
        hubEdit->setEnabled(true);
        m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Length(")+str+")");
        m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Angle(deg)"));
    }
    else{
        hubEdit->setEnabled(false);
        m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Height(")+str+")");
        m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Radius(")+str+")");
    }

    for (int i=0; i<=m_pBlade->m_NPanel;i++){
        FillTableRow(i);
    }

}


void QDMS::FillTableRow(int row)
{
        QModelIndex ind;


        if (m_bisHeight){
            ind = m_pWingModel->index(row, 0, QModelIndex());
            m_pWingModel->setData(ind, m_pBlade->m_TPos[row] * g_mainFrame->m_mtoUnit);
        }
        else{
            ind = m_pWingModel->index(row, 0, QModelIndex());
            if (row == 0){
                m_pWingModel->setData(ind, m_pBlade->m_TPos[row] * g_mainFrame->m_mtoUnit);
            }
            else{
                double angle = atan((m_pBlade->m_TOffsetX[row]-m_pBlade->m_TOffsetX[row-1])/(m_pBlade->m_TPos[row]-m_pBlade->m_TPos[row-1])) /PI *180.0;
                double length = (m_pBlade->m_TPos[row]-m_pBlade->m_TPos[row-1]) / cos (angle/180.0*PI);
                m_pWingModel->setData(ind, length * g_mainFrame->m_mtoUnit);
            }
        }

        ind = m_pWingModel->index(row, 1, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_TChord[row] * g_mainFrame->m_mtoUnit);

        if (m_bisHeight){
            ind = m_pWingModel->index(row, 2, QModelIndex());
            m_pWingModel->setData(ind, m_pBlade->m_TOffsetX[row] * g_mainFrame->m_mtoUnit);
        }
        else{
            ind = m_pWingModel->index(row, 2, QModelIndex());
            if (row == 0){
                m_pWingModel->setData(ind, 0);
            }
            else{
                double angle = atan((m_pBlade->m_TOffsetX[row]-m_pBlade->m_TOffsetX[row-1])/(m_pBlade->m_TPos[row]-m_pBlade->m_TPos[row-1])) /PI *180.0;
                m_pWingModel->setData(ind, angle * g_mainFrame->m_mtoUnit);
            }
        }

        ind = m_pWingModel->index(row, 3, QModelIndex());
        m_pWingModel->setData(ind, 90-m_pBlade->m_TTwist[row]);

        ind = m_pWingModel->index(row, 4, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_TCircAngle[row]);

        ind = m_pWingModel->index(row, 5, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_TFoilPAxisX[row]);

        ind = m_pWingModel->index(row, 6, QModelIndex());
        m_pWingModel->setData(ind, m_pBlade->m_Airfoils[row]);

        ind = m_pWingModel->index(row, 7, QModelIndex());

        if (m_pBlade->m_bisSinglePolar) m_pWingModel->setData(ind, m_pBlade->m_Polar.at(row));
        else m_pWingModel->setData(ind, m_pBlade->m_Range.at(row));

}


void QDMS::FillRotorCurve(CCurve *pCurve, void *Data, int XVar, int YVar)
{
        QList <double> *pX;
        QList <double> *pY;
        Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

        if (pGraph->m_Type == 0)
        {
        pX = (QList <double> *) GetRotorVariable(Data, XVar);
        pY = (QList <double> *) GetRotorVariable(Data, YVar);
        }
        else if (pGraph->m_Type == 1)
        {
        pX = (QList <double> *) GetBladeVariable(Data, XVar);
        pY = (QList <double> *) GetBladeVariable(Data, YVar);
        }
        else
        {					
        pX = (QList <double> *) GetAziVariable(Data, XVar);
        pY = (QList <double> *) GetAziVariable(Data, YVar);
        }

        pCurve->SetSelected(-1);

        for (int i=0;i<pX->size();i++)
        {
            pCurve->AddPoint(pX->at(i),pY->at(i));
        }

}


void QDMS::FillCharacteristicCurve(CCurve *pCurve, void *Data, int XVar, int YVar, int num_param)
{
    float *** pX;
    float *** pY;
	Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

    pX =   GetCharVariable(Data, XVar);
    pY =   GetCharVariable(Data, YVar);

	pCurve->SetSelected(-1);

	if( pGraph->m_MainVar==0)
	{
		for (int i=0;i<m_pCDMSData->windtimes;i++)
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
		for (int i=0;i<m_pCDMSData->rottimes;i++)
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
		for (int i=0;i<m_pCDMSData->pitchtimes;i++)
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


void QDMS::FillPowerCurve(CCurve *pCurve, void *Data, int XVar, int YVar)
{
        QList <double> *pX;
        QList <double> *pY;
        Graph *pGraph = (Graph *) pCurve->m_pParentGraph;

        if(pGraph->m_Type == 0)
        {
        pX = (QList <double> *) GetTurbineRotorVariable(Data, XVar);
        pY = (QList <double> *) GetTurbineRotorVariable(Data, YVar);
        }
        else if(pGraph->m_Type == 1)
        {
        pX = (QList <double> *) GetTurbineBladeVariable(Data, XVar);
        pY = (QList <double> *) GetTurbineBladeVariable(Data, YVar);
        }
        else
        {
        pX = (QList <double> *) GetTurbineAziVariable(Data, XVar);
        pY = (QList <double> *) GetTurbineAziVariable(Data, YVar);
        }

        pCurve->SetSelected(-1);

        for (int i=0;i<pX->size();i++)
        {
            pCurve->AddPoint(pX->at(i),pY->at(i));
        }

}


CBlade * QDMS::GetWing(QString WingName)
{
        int i;
        CBlade* pWing;
        for (i=0; i < g_verticalRotorStore.size(); i++)
        {
                pWing = g_verticalRotorStore.at(i);
                if (pWing->getName() == WingName) return pWing;
        }
        return NULL;
}

void QDMS::GLCallViewLists() {
	if(m_bSurfaces) {
		if(m_pBlade)
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
			for (int i=1;i<m_pBlade->m_blades;i++) {
				glRotated(360.0/m_pBlade->m_blades,0,1,0);
				if(m_bSurfaces) {
					if(m_pBlade)
						glCallList(WINGSURFACES);
				}
				if(m_bOutline) {
					if(m_pBlade)
						glCallList(WINGOUTLINE);
				}
				if(m_iSection>=0) {
					glCallList(SECTIONHIGHLIGHT);
				}
			}
		}
	}
}


void* QDMS::GetRotorVariable(void *Data, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        void * pVar;
        DMSData *pDMSData = (DMSData *) Data;

        switch (iVar){
                case 0:
                        pVar = &pDMSData->m_Cp;
                        break;
                case 1:
                        pVar = &pDMSData->m_Cp1;
                        break;
                case 2:
                        pVar = &pDMSData->m_Cp2;
                        break;
				case 3:
                        pVar = &pDMSData->m_Cm;
                        break;
				case 4:
                        pVar = &pDMSData->m_Cm1;
                        break;
				case 5:
                        pVar = &pDMSData->m_Cm2;
                        break;
				case 6:
                        pVar = &pDMSData->m_Kp;
                        break;
				case 7:
                        pVar = &pDMSData->m_Lambda;
                        break;
				case 8:
                        pVar = &pDMSData->m_one_over_Lambda;
                        break;
                case 9:
                        pVar = &pDMSData->m_P;
                        break;
                case 10:
                        pVar = &pDMSData->m_Thrust;
                        break;
                case 11:
                        pVar = &pDMSData->m_T;
                        break;
                case 12:
                        pVar = &pDMSData->m_Omega;
                        break;
                case 13:
                        pVar = &pDMSData->m_V;
                        break;


                default:
                        pVar = &pDMSData->m_Lambda;
                        break;
        }
        return pVar;
}


void * QDMS::GetBladeVariable(void *Data, int iVar)
{
    // returns a pointer to the variable array defined by its index iVar
    void * pVar;
    DData *pDData = (DData *) Data;

    switch (iVar){
        case 0:
                pVar = &pDData->m_zeta;
                break;

        case 1:
                pVar = &pDData->m_eta;
                break;

        case 2:
                pVar = &pDData->m_c_local;
                break;

        case 3:
                pVar = &pDData->m_delta;
                break;

        case 4:
                pVar = &pDData->m_u_up;
                break;

        case 5:
                pVar = &pDData->m_u_down;
                break;

        case 6:
                pVar = &pDData->m_a_up;
                break;

        case 7:
                pVar = &pDData->m_a_down;
                break;

        case 8:
                pVar = &pDData->m_velocity_inf;
                break;

        case 9:
                pVar = &pDData->m_velocity_up;
                break;

        case 10:
                pVar = &pDData->m_velocity_equil;
                break;

        case 11:
                pVar = &pDData->m_velocity_down;
                break;

        case 12:
                pVar = &pDData->m_velocity_wake;
                break;

        case 13:
                pVar = &pDData->m_lambda_up;
                break;

        case 14:
                pVar = &pDData->m_lambda_down;
                break;

        case 15:
                pVar = &pDData->m_area;
                break;

        case 16:
                pVar = &pDData->m_Ftip_up;
                break;

        case 17:
                pVar = &pDData->m_Ftip_dw;
                break;

        case 18:
                pVar = &pDData->m_it_up;
                break;

        case 19:
                pVar = &pDData->m_it_dw;
                break;

        default:
                pVar = &pDData->m_zeta;
                break;

    }
    return pVar;
}


void * QDMS::GetAziVariable(void *Data, int iVar)
{
    // returns a pointer to the variable array defined by its index iVar
    void * pVar;
    DData *pDData = (DData *) Data;

    switch (iVar){
        case 0:
                pVar = &pDData->m_theta_plot;
                break;

        case 1:
				pVar = &pDData->m_iterations[same_height];
                break;

        case 2:
				pVar = &pDData->m_u[same_height];
                break;

        case 3:
				pVar = &pDData->m_V[same_height];
                break;

        case 4:
				pVar = &pDData->m_vrel[same_height];
                break;

        case 5:
				pVar = &pDData->m_Re[same_height];
                break;
        case 6:
                pVar = &pDData->m_DeltaRe[same_height];
                break;

        case 7:
				pVar = &pDData->m_Ftip[same_height];
                break;

        case 8:
				pVar = &pDData->m_alpha_deg[same_height];
                break;

        case 9:
				pVar = &pDData->m_CL[same_height];
                break;

        case 10:
				pVar = &pDData->m_CD[same_height];
                break;

        case 11:
				pVar = &pDData->m_LD[same_height];
                break;

        case 12:
				pVar = &pDData->m_Cn[same_height];
                break;

        case 13:
				pVar = &pDData->m_Ct[same_height];
                break;
        case 14:
                pVar = &pDData->m_FT;
                break;
        case 15:
				pVar = &pDData->m_CF_length;
                break;
        case 16:
				pVar = &pDData->m_CF_cross;
                break;
        case 17:
                pVar = &pDData->m_FT_tot;
                break;
        case 18:
				pVar = &pDData->m_CF_length_tot;
                break;
        case 19:
				pVar = &pDData->m_CF_cross_tot;
                break;

        default:
                pVar = &pDData->m_theta_plot;
                break;

    }
    return pVar;

}


float *** QDMS::GetCharVariable(void *Data, int iVar)
{
		// returns a pointer to the variable array defined by its index iVar
        float *** pVar;
		CDMSData *pCDMSData = (CDMSData *) Data;

		switch (iVar){
				case 0:

                        pVar = pCDMSData->m_P;
						break;
				case 1:

                        pVar = pCDMSData->m_Torque;
						break;
				case 2:

                        pVar = pCDMSData->m_V;
						break;
				case 3:

                        pVar = pCDMSData->m_Lambda;
						break;
				case 4:

                        pVar = pCDMSData->m_one_over_Lambda;
						break;
				case 5:

                        pVar = pCDMSData->m_w;
						break;
				case 6:

                        pVar = pCDMSData->m_Pitch;
						break;
				case 7:

                        pVar = pCDMSData->m_Cp;
						break;
				case 8:

                        pVar = pCDMSData->m_Cp1;
						break;
				case 9:

                        pVar = pCDMSData->m_Cp2;
						break;
				case 10:

                        pVar = pCDMSData->m_Cm;
						break;
				case 11:

                        pVar = pCDMSData->m_Cm1;
						break;
				case 12:

                        pVar = pCDMSData->m_Cm2;
						break;
				case 13:

                        pVar = pCDMSData->m_Kp;
						break;
				default:

                        pVar = pCDMSData->m_P;
						break;
		}
		return pVar;
}


void * QDMS::GetTurbineRotorVariable(void *Data, int iVar)
{
        // returns a pointer to the variable array defined by its index iVar
        void * pVar;
        TDMSData *pTDMSData = (TDMSData *) Data;

        switch (iVar){
        case 0:
                pVar = &pTDMSData->m_P;
                break;
		case 1:
                pVar = &pTDMSData->m_T;
                break;
		case 2:
                pVar = &pTDMSData->m_V;
                break;
		case 3:
                pVar = &pTDMSData->m_Lambda;
                break;
		case 4:
                pVar = &pTDMSData->m_one_over_Lambda;
                break;
		case 5:
                pVar = &pTDMSData->m_Omega;
                break;
		case 6:
				pVar = &pTDMSData->m_Cp;
				break;
		case 7:
				pVar = &pTDMSData->m_Cp1;
				break;
		case 8:
				pVar = &pTDMSData->m_Cp2;
				break;
//		case 7:
//                pVar = &pTDMSData->m_Ct;
//                break;
//		case 8:
//                pVar = &pTDMSData->m_Ct1;
//                break;
//		case 9:
//                pVar = &pTDMSData->m_Ct2;
//                break;
		case 9:
                pVar = &pTDMSData->m_Cm;
                break;
		case 10:
                pVar = &pTDMSData->m_Cm1;
                break;
		case 11:
                pVar = &pTDMSData->m_Cm2;
                break;
		case 12:
                pVar = &pTDMSData->m_Kp;
                break;
		case 13:
                pVar = &pTDMSData->m_P_loss;
                break;
		case 14:
                pVar = &pTDMSData->m_Cp_loss;
                break;
		case 15:
                pVar = &pTDMSData->m_Weibull;
                break;
//		case 16:
//                pVar = &pTDMSData->m_WeibullV3;
//                break;
        default:

                pVar = &pTDMSData->m_Lambda;
                break;
        }
        return pVar;
}


void * QDMS::GetTurbineBladeVariable(void *Data, int iVar)
{
    void * pVar;
    DData *pDData = (DData *) Data;

    switch (iVar){

    case 0:
            pVar = &pDData->m_pos;
            break;

    case 1:
            pVar = &pDData->m_radius_local;
            break;

    case 2:
            pVar = &pDData->m_c_local;
            break;

    case 3:
            pVar = &pDData->m_delta;
            break;

    case 4:
            pVar = &pDData->m_u_up;
            break;

    case 5:
            pVar = &pDData->m_u_down;
            break;

    case 6:
            pVar = &pDData->m_a_up;
            break;

    case 7:
            pVar = &pDData->m_a_down;
            break;

    case 8:
            pVar = &pDData->m_velocity_inf;
            break;

    case 9:
            pVar = &pDData->m_velocity_up;
            break;

    case 10:
            pVar = &pDData->m_velocity_equil;
            break;

    case 11:
            pVar = &pDData->m_velocity_down;
            break;

    case 12:
            pVar = &pDData->m_velocity_wake;
            break;

    case 13:
            pVar = &pDData->m_lambda_up;
            break;

    case 14:
            pVar = &pDData->m_lambda_down;
            break;

    case 15:
            pVar = &pDData->m_area;
            break;

    case 16:
            pVar = &pDData->m_Ftip_up;
            break;

    case 17:
            pVar = &pDData->m_Ftip_dw;
            break;

    case 18:
            pVar = &pDData->m_it_up;
            break;

    case 19:
            pVar = &pDData->m_it_dw;
            break;

    default:
            pVar = &pDData->m_pos;
            break;
    }
    return pVar;
}


void * QDMS::GetTurbineAziVariable(void *Data, int iVar)
{
    void * pVar;
    DData *pDData = (DData *) Data;

    switch (iVar){
    case 0:
            pVar = &pDData->m_theta_plot;
            break;

    case 1:
			pVar = &pDData->m_iterations[same_height];
            break;

    case 2:
			pVar = &pDData->m_u[same_height];
            break;

    case 3:
			pVar = &pDData->m_V[same_height];
            break;

    case 4:
			pVar = &pDData->m_vrel[same_height];
            break;

    case 5:
			pVar = &pDData->m_Re[same_height];
            break;

    case 6:
			pVar = &pDData->m_Ftip[same_height];
            break;

    case 7:
			pVar = &pDData->m_alpha_deg[same_height];
            break;

    case 8:
			pVar = &pDData->m_CL[same_height];
            break;

    case 9:
			pVar = &pDData->m_CD[same_height];
            break;

    case 10:
			pVar = &pDData->m_LD[same_height];
            break;

    case 11:
			pVar = &pDData->m_Cn[same_height];
            break;

    case 12:
			pVar = &pDData->m_Ct[same_height];
            break;

//    case 13:
//            pVar = &pDData->m_FN;
//            break;

	case 13:
            pVar = &pDData->m_FT;
            break;
	case 14:
			pVar = &pDData->m_CF_length;
            break;
	case 15:
			pVar = &pDData->m_CF_cross;
            break;
	case 16:
            pVar = &pDData->m_T;
            break;
	case 17:
			pVar = &pDData->m_F_length;
			break;
	case 18:
			pVar = &pDData->m_F_cross;
			break;

//    case 18:
//            pVar = &pDData->m_FN_tot;
//            break;

	case 19:
            pVar = &pDData->m_FT_tot;
            break;
	case 20:
			pVar = &pDData->m_CF_length_tot;
            break;
	case 21:
			pVar = &pDData->m_CF_cross_tot;
            break;

	case 22:
            pVar = &pDData->m_T_tot;
            break;
	case 23:
			pVar = &pDData->m_F_length_tot;
			break;
	case 24:
			pVar = &pDData->m_F_cross_tot;
			break;

    default:
            pVar = &pDData->m_theta_plot;
            break;
    }
    return pVar;

}

void * QDMS::GetWeibullXVariable(int iVar)
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

void * QDMS::GetWeibullYVariable(void *Data, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;
	TDMSData *pTDMSData = (TDMSData *) Data;

	switch (iVar){
			case 0:
					pVar = &pTDMSData->m_aepk;
					break;
			case 1:
					pVar = &pTDMSData->m_aepA;
					break;
			default:
					pVar = &pTDMSData->m_aepk;
					break;
	}
	return pVar;

}


DData* QDMS::GetBladeData(QString Lambda)
{

    if (!m_pDMSData->m_DData.size()) return NULL;

    for (int i=0; i<m_pDMSData->m_DData.size(); i++)
    {
        if (m_pDMSData->m_DData.at(i)->lambdaglobal==Lambda)
        {
            return m_pDMSData->m_DData.at(i);
        }
    }

    return NULL;
}

DData* QDMS::GetTurbineBladeData(QString Windspeed)
{

    if (!m_pTDMSData->m_DData.size()) return NULL;

    for (int i=0; i<m_pTDMSData->m_DData.size(); i++)
    {
        if (m_pTDMSData->m_DData.at(i)->windspeedStr==Windspeed)
        {
            return m_pTDMSData->m_DData.at(i);
        }
    }

    return NULL;
}

void QDMS::OnBladeColor()
{
    if(!m_pBlade) return;

    QColor WingColor = QColorDialog::getColor(m_pBlade->m_WingColor,
                                      this, "Select the wing color", QColorDialog::ShowAlphaChannel);
    if(WingColor.isValid()) m_pBlade->m_WingColor = WingColor;

    m_pctrlWingColor->SetColor(m_pBlade->m_WingColor);
    m_bResetglGeom = true;
    ComputeGeometry(true);
    UpdateView();

}

void QDMS::OnSectionColor()
{
    if(!m_pBlade) return;

    QColor SectionColor = QColorDialog::getColor(m_OutlineColor,
                                      this, "Select the section color", QColorDialog::ShowAlphaChannel);
    if(SectionColor.isValid()) m_OutlineColor = SectionColor;

    m_pctrlSectionColor->SetColor(m_OutlineColor);
    m_bResetglGeom = true;
    ComputeGeometry(true);
    UpdateView();

}

void QDMS::OnDeleteSection()
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
                m_pBlade->m_TLength[k] = m_pBlade->m_TLength[k+1];
                m_pBlade->m_TCircAngle[k] = m_pBlade->m_TCircAngle[k+1];

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
        ComputeGeometry(true);
        ReadParams(true);
}


bool QDMS::InitDialog(CBlade *pWing)
{
        m_bResetglSectionHighlight = true;

        QString str;
        m_iSection = 0;

        if (m_pWingModel) delete m_pWingModel;
        if (m_pBladeDelegate) delete m_pBladeDelegate;

        m_pBlade = pWing;

        if(!m_pBlade) return false;

        CheckButtons();

        m_pBlade->CreateSurfaces(true);

        m_pctrlSave->setEnabled(false);
        m_pctrlOptimize->setEnabled(false);

        ComputeGeometry(true);

        GetLengthUnit(str, g_mainFrame->m_LengthUnit);

        m_pctrlWingName->setText(m_pBlade->getName());

        m_pWingModel = new QStandardItemModel;
        m_pWingModel->setRowCount(100);//temporary
        m_pWingModel->setColumnCount(8);

        m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Height(")+str+")");
        m_pWingModel->setHeaderData(1, Qt::Horizontal, tr("Chord(")+str+")");
        m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Radius(")+str+")");
        m_pWingModel->setHeaderData(3, Qt::Horizontal, tr("Twist"));
        m_pWingModel->setHeaderData(4, Qt::Horizontal, tr("Circ Ang"));
        m_pWingModel->setHeaderData(5, Qt::Horizontal, tr("P Axis"));
        m_pWingModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Foil"));
        m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar"));

        m_pctrlBladeTable->setModel(m_pWingModel);

        OnResize();

        QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pWingModel);
        m_pctrlBladeTable->setSelectionModel(selectionModel);
        connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));

        m_pBladeDelegate = new BladeDelegate2(m_pBlade, this);
        m_pctrlBladeTable->setItemDelegate(m_pBladeDelegate);
        connect(m_pBladeDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged()));

        int  *precision = new int[5];
        precision[0] = 3;
        precision[1] = 3;
        precision[2] = 3;
        precision[3] = 2;
        precision[4] = 2;
        precision[5] = 2;


        m_pBladeDelegate->SetPointers(precision,&m_pBlade->m_NPanel);

        m_pBladeDelegate->itemmodel = m_pWingModel;

        FillDataTable();

        hubEdit->setValue(m_pBlade->m_TOffsetX[0] * g_mainFrame->m_mtoUnit);

        m_pctrlBladeTable->selectRow(m_iSection);
        SetCurrentSection(m_iSection);
        m_bResetglGeom = true;
        UpdateView();
        return true;
}


void QDMS::InitBladeSimulationParams(DMSData *data)
{
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;
    QString strong;

    if(data)
    {
	pSimuWidgetDMS->m_pctrlRhoVal->setText(strong.number(data->rho,'f',4));
	pSimuWidgetDMS->m_pctrlElementsVal->setText(strong.number(data->elements,'f',0));
	pSimuWidgetDMS->m_pctrlIterationVal->setText(strong.number(data->iterations,'f',0));
	pSimuWidgetDMS->m_pctrlEpsilonVal->setText(strong.number(data->epsilon,'f',4));
	pSimuWidgetDMS->m_pctrlRelaxVal->setText(strong.number(data->relax,'f',1));
	pSimuWidgetDMS->m_pctrlViscVal->setText(strong.number(data->visc,'e',8));
	pSimuWidgetDMS->m_pctrlTipLoss->setChecked(data->m_bTipLoss);
	pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(data->m_bAspectRatio);
	pSimuWidgetDMS->m_pctrlVariable->setChecked(data->m_bVariable);

    if (data->m_bConstant)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("const");
    else if (data->m_bPowerLaw)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("pow");
    else if (data->m_bLogarithmic)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("log");
    /*
	pSimuWidgetDMS->m_pctrlRootLoss->setChecked(data->m_bRootLoss);
	pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(data->m_b3DCorrection);
	pSimuWidgetDMS->m_pctrlInterpolation->setChecked(data->m_bInterpolation);
	pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(data->m_bNewRootLoss);
	pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(data->m_bNewTipLoss);
    */
    }
    if(!data)
    {
	pSimuWidgetDMS->m_pctrlRhoVal->setText("");
	pSimuWidgetDMS->m_pctrlElementsVal->setText("");
	pSimuWidgetDMS->m_pctrlIterationVal->setText("");
	pSimuWidgetDMS->m_pctrlEpsilonVal->setText("");
	pSimuWidgetDMS->m_pctrlRelaxVal->setText("");
	pSimuWidgetDMS->m_pctrlViscVal->setText("");
	pSimuWidgetDMS->m_pctrlTipLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(false);
	pSimuWidgetDMS->m_pctrlVariable->setChecked(false);
	pSimuWidgetDMS->m_pctrlWindprofileVal->setText("");
    /*
	pSimuWidgetDMS->m_pctrlRootLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(false);
	pSimuWidgetDMS->m_pctrlInterpolation->setChecked(false);
	pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(false);
    */
    }
}


void QDMS::InitCharSimulationParams(CDMSData *bladedata)
{
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;
	QString strong;

	if(bladedata)
	{
		pSimuWidgetDMS->m_pctrlRhoVal->setText(strong.number(bladedata->rho,'f',4));
		pSimuWidgetDMS->m_pctrlElementsVal->setText(strong.number(bladedata->elements,'f',0));
		pSimuWidgetDMS->m_pctrlIterationVal->setText(strong.number(bladedata->iterations,'f',0));
		pSimuWidgetDMS->m_pctrlEpsilonVal->setText(strong.number(bladedata->epsilon,'f',4));
		pSimuWidgetDMS->m_pctrlRelaxVal->setText(strong.number(bladedata->relax,'f',1));
		pSimuWidgetDMS->m_pctrlViscVal->setText(strong.number(bladedata->visc,'e',8));
		pSimuWidgetDMS->m_pctrlTipLoss->setChecked(bladedata->m_bTipLoss);
		pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(bladedata->m_bAspectRatio);
		pSimuWidgetDMS->m_pctrlVariable->setChecked(bladedata->m_bVariable);
		if (bladedata->m_bConstant)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("const");
		else if (bladedata->m_bPowerLaw)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("pow");
		else if (bladedata->m_bLogarithmic)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("log");
		/*
		pSimuWidgetDMS->m_pctrlRootLoss->setChecked(bladedata->m_bRootLoss);
		pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(bladedata->m_b3DCorrection);
		pSimuWidgetDMS->m_pctrlInterpolation->setChecked(bladedata->m_bInterpolation);
		pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(bladedata->m_bNewRootLoss);
		pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(bladedata->m_bNewTipLoss);
		pSimuWidgetDMS->m_pctrlCdReynolds->setChecked(bladedata->m_bCdReynolds);
		*/
		if (bladedata->m_bConstant)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("const");
		else if (bladedata->m_bPowerLaw)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("pow");
		else if (bladedata->m_bLogarithmic)
			pSimuWidgetDMS->m_pctrlWindprofileVal->setText("log");
	}
	if(!bladedata)
	{
		pSimuWidgetDMS->m_pctrlRhoVal->setText("");
		pSimuWidgetDMS->m_pctrlElementsVal->setText("");
		pSimuWidgetDMS->m_pctrlIterationVal->setText("");
		pSimuWidgetDMS->m_pctrlEpsilonVal->setText("");
		pSimuWidgetDMS->m_pctrlRelaxVal->setText("");
		pSimuWidgetDMS->m_pctrlViscVal->setText("");
		pSimuWidgetDMS->m_pctrlTipLoss->setChecked(false);
		pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(false);
		pSimuWidgetDMS->m_pctrlVariable->setChecked(false);
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("");
		/*
		pSimuWidgetDMS->m_pctrlRootLoss->setChecked(false);
		pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(false);
		pSimuWidgetDMS->m_pctrlInterpolation->setChecked(false);
		pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(false);
		pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(false);
		pSimuWidgetDMS->m_pctrlCdReynolds->setChecked(false);
		*/

	}

}


void QDMS::InitTurbineSimulationParams(TDMSData *data)
{

	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

    QString strong;

    if(data)
    {
	pSimuWidgetDMS->m_pctrlRhoVal->setText(strong.number(data->rho,'f',4));
	pSimuWidgetDMS->m_pctrlElementsVal->setText(strong.number(data->elements,'f',0));
	pSimuWidgetDMS->m_pctrlIterationVal->setText(strong.number(data->iterations,'f',0));
	pSimuWidgetDMS->m_pctrlEpsilonVal->setText(strong.number(data->epsilon,'f',4));
	pSimuWidgetDMS->m_pctrlRelaxVal->setText(strong.number(data->relax,'f',1));
	pSimuWidgetDMS->m_pctrlViscVal->setText(strong.number(data->visc,'e',8));
	pSimuWidgetDMS->m_pctrlTipLoss->setChecked(data->m_bTipLoss);
	pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(data->m_bAspectRatio);
	pSimuWidgetDMS->m_pctrlVariable->setChecked(data->m_bVariable);

    if (data->m_bConstant)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("const");
    else if (data->m_bPowerLaw)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("pow");
    else if (data->m_bLogarithmic)
		pSimuWidgetDMS->m_pctrlWindprofileVal->setText("log");
    /*
	pSimuWidgetDMS->m_pctrlRootLoss->setChecked(data->m_bRootLoss);
	pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(data->m_b3DCorrection);
	pSimuWidgetDMS->m_pctrlInterpolation->setChecked(data->m_bInterpolation);
	pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(data->m_bNewRootLoss);
	pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(data->m_bNewTipLoss);
    */
    }
    if(!data)
    {
	pSimuWidgetDMS->m_pctrlRhoVal->setText("");
	pSimuWidgetDMS->m_pctrlElementsVal->setText("");
	pSimuWidgetDMS->m_pctrlIterationVal->setText("");
	pSimuWidgetDMS->m_pctrlEpsilonVal->setText("");
	pSimuWidgetDMS->m_pctrlRelaxVal->setText("");
	pSimuWidgetDMS->m_pctrlViscVal->setText("");
	pSimuWidgetDMS->m_pctrlTipLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrlAspectRatio->setChecked(false);
	pSimuWidgetDMS->m_pctrlVariable->setChecked(false);
	pSimuWidgetDMS->m_pctrlWindprofileVal->setText("");
    /*
	pSimuWidgetDMS->m_pctrlRootLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrl3DCorrection->setChecked(false);
	pSimuWidgetDMS->m_pctrlInterpolation->setChecked(false);
	pSimuWidgetDMS->m_pctrlNewRootLoss->setChecked(false);
	pSimuWidgetDMS->m_pctrlNewTipLoss->setChecked(false);
    */
    }

    if(data)
    {
	//m_pctrlWA->setValue(data->A);
	//m_pctrlWk->setValue(data->K);
		m_pctrlWA2->setEnabled(true);
		m_pctrlWk2->setEnabled(true);
		m_pctrlPMA2->setEnabled(true);
		m_pctrlPMk2->setEnabled(true);
    }
    if(!data)
    {
	//m_pctrlWA->setValue(9);
	//m_pctrlWk->setValue(2);
	m_pctrlWA2->setEnabled(false);
	m_pctrlWk2->setEnabled(false);
	m_pctrlPMA2->setEnabled(false);
	m_pctrlPMk2->setEnabled(false);
    m_pctrlYield->setText("");
    }
}


void QDMS::InitTurbineData(TData *pTData)
{
   
    QString strong, str;

    if (pTData)
    {
//        if (pTData->isStall) Type->setText("Stall");
//        if (pTData->isPitch) Type->setText("Pitch");
        if (pTData->isFixed) Trans->setText("Single");
//        if (pTData->is2Step) Trans->setText("2 Step");
        if (pTData->isVariable) Trans->setText("Variable");
        if (pTData->isPrescribedRot) Trans->setText("Prescribed");
//        if (pTData->isPrescribedPitch) Type->setText("Prescribed Pitch");


        Rot1->setText(strong.number(pTData->Rot1,'f',2));
        Rot2->setText(strong.number(pTData->Rot2,'f',2));
        Lambda0->setText(strong.number(pTData->Lambda0,'f',2));
//        Generator->setText(strong.number(pTData->Generator*g_mainFrame->m_WtoUnit,'f',2));
        CutIn->setText(strong.number(pTData->CutIn*g_mainFrame->m_mstoUnit,'f',2));
        CutOut->setText(strong.number(pTData->CutOut*g_mainFrame->m_mstoUnit,'f',2));
//        Switch->setText(strong.number(pTData->Switch*g_mainFrame->m_mstoUnit,'f',2));
        FixedLosses->setText(strong.number(pTData->m_fixedLosses*g_mainFrame->m_WtoUnit,'f',2));
        VariableLosses->setText(strong.number(pTData->VariableLosses,'f',3));
        Blade->setText(pTData->m_WingName);
//        FixedPitch->setText(strong.number(pTData->FixedPitch,'f',2));

        TurbineOffset->setText(strong.number(pTData->Offset*g_mainFrame->m_mtoUnit,'f',2));
        GetLengthUnit(str, g_mainFrame->m_LengthUnit);
        Length0->setText(str);

        RotorHeight->setText(strong.number(pTData->RHeight*g_mainFrame->m_mtoUnit,'f',2));
        Length1->setText(str);

        TurbineRadius->setText(strong.number(pTData->MaxRadius*g_mainFrame->m_mtoUnit,'f',2));
        Length2->setText(str);

        TurbineHeight->setText(strong.number(pTData->THeight*g_mainFrame->m_mtoUnit,'f',2));
        Length3->setText(str);

        TurbineSweptArea->setText(strong.number(pTData->SweptArea*g_mainFrame->m_m2toUnit,'f',2));
        GetAreaUnit(str, g_mainFrame->m_AreaUnit);
        Area1->setText(str);


        GetLengthUnit(str, g_mainFrame->m_LengthUnit);
        Length1->setText(str);
        GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
        Speed1->setText(str);
        Speed2->setText(str);
//        Speed3->setText(str);
        GetPowerUnit(str, g_mainFrame->m_PowerUnit);
//        Power1->setText(str);
        Power2->setText(str);



        GeneratorTypeLabel->show();
        Trans->show();
        CutInLabel->show();
        CutIn->show();
        Speed1->show();
        CutOutLabel->show();
        CutOut->show();
        Speed2->show();
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
        TurbineOffsetLabel->show();
        TurbineOffset->show();
        Length0->show();
        TurbineHeightLabel->show();
        TurbineHeight->show();
        Length3->show();
        RotorHeightLabel->show();
        RotorHeight->show();
        Length1->show();
        TurbineRadiusLabel->show();
        TurbineRadius->show();
        Length2->show();
        TurbineSweptAreaLabel->show();
        TurbineSweptArea->show();
        Area1->show();
        VariableLossesLabel->show();
        VariableLosses->show();
        FixedLossesLabel->show();
        FixedLosses->show();
        Power2->show();



//        if (pTData->isPitch)
//        {
//            Generator->show();
//            GeneratorLabel->show();
//            Power1->show();

//        }
//        else
//        {
//            Generator->hide();
//            GeneratorLabel->hide();
//            Power1->hide();

//        }

//        if(pTData->isPrescribedPitch)
//        {
//            Generator->hide();
//            GeneratorLabel->hide();
//            Power1->hide();
//            FixedPitch->hide();
//            FixedPitchLabel->hide();

//        }

        if(pTData->isPrescribedRot)
        {
            Rot1Label->hide();
            Rot2Label->setText(tr(""));
//            SwitchLabel->setText("");
            LambdaLabel->setText("");
            Rot1->hide();
            Rotspeed1->hide();
            Rot2->hide();
            Rotspeed2->hide();
//            Switch->hide();
//            Speed3->hide();
            Lambda0->hide();
        }


        if (pTData->isVariable)
        {
            Rot1Label->setText(tr("Rotational Speed Min"));
            Rot2Label->setText(tr("Rotational Speed Max"));
//            SwitchLabel->setText("");
            LambdaLabel->setText("TSR at Design Point");
            Rot1->show();
            Speed1->show();
            Rot2->show();
            Speed2->show();
//            Switch->hide();
//            Speed3->show();
            Lambda0->show();

        }
//        if (pTData->is2Step)
//        {
//            Rot1Label->setText(tr("Rotational Speed 1"));
//            Rot2Label->setText(tr("Rotational Speed 2"));
//            SwitchLabel->setText("V Switch");
//            LambdaLabel->setText("");
//            Rot1->show();
//            Rotspeed1->show();
//            Rot2->show();
//            Rotspeed2->show();
//            Switch->show();
//            Speed3->show();
//            Lambda0->hide();
//        }
        if (pTData->isFixed)
        {
            Rot1Label->setText(tr("Rotational Speed"));
            Rot2Label->setText(tr(""));
//            SwitchLabel->setText("");
            LambdaLabel->setText("");
            Rot1->show();
            Rotspeed1->show();
            Rot2->hide();
            Rotspeed2->hide();
//            Switch->hide();
//            Speed3->hide();
            Lambda0->hide();
        }
    }
    else
    {
        GeneratorTypeLabel->hide();
        Trans->hide();
        CutInLabel->hide();
        CutIn->hide();
        Speed1->hide();
        CutOutLabel->hide();
        CutOut->hide();
        Speed2->hide();
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
        TurbineOffsetLabel->hide();
        TurbineOffset->hide();
        Length0->hide();
        TurbineHeightLabel->hide();
        TurbineHeight->hide();
        Length3->hide();
        RotorHeightLabel->hide();
        RotorHeight->hide();
        Length1->hide();
        TurbineRadiusLabel->hide();
        TurbineRadius->hide();
        Length2->hide();
        TurbineSweptAreaLabel->hide();
        TurbineSweptArea->hide();
        Area1->hide();
        VariableLossesLabel->hide();
        VariableLosses->hide();
        FixedLossesLabel->hide();
        FixedLosses->hide();
        Power2->hide();
    }


}

void QDMS::OnResize()
{
    m_pctrlBladeTable->setMaximumWidth(0.9*g_mainFrame->m_pctrlDMSWidget->width());
    m_pctrlBladeTable->setMinimumWidth(0.9*g_mainFrame->m_pctrlDMSWidget->width());
    int unitwidth = (int)((m_pctrlBladeTable->width())/9.0);
    m_pctrlBladeTable->setColumnWidth(0,unitwidth);
    m_pctrlBladeTable->setColumnWidth(1,unitwidth);
    m_pctrlBladeTable->setColumnWidth(2,unitwidth);
    m_pctrlBladeTable->setColumnWidth(3,unitwidth);
    m_pctrlBladeTable->setColumnWidth(4,unitwidth);
    m_pctrlBladeTable->setColumnWidth(5,unitwidth);
    m_pctrlBladeTable->setColumnWidth(6,1.5*unitwidth);
    m_pctrlBladeTable->setColumnWidth(7,1.5*unitwidth);

    m_pctrlBladeTableView->setMaximumWidth(0.9*g_mainFrame->m_pctrlDMSWidget->width());
    m_pctrlBladeTableView->setMinimumWidth(0.9*g_mainFrame->m_pctrlDMSWidget->width());
    unitwidth = (int)((m_pctrlBladeTableView->width())/9.0);
    m_pctrlBladeTableView->setColumnWidth(0,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(1,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(2,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(3,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(4,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(5,unitwidth);
    m_pctrlBladeTableView->setColumnWidth(6,1.5*unitwidth);
    m_pctrlBladeTableView->setColumnWidth(7,1.5*unitwidth);
}


void QDMS::InitBladeTable()
{
    m_bResetglSectionHighlight = true;

    if (m_pBlade)
    {
    delete m_pWingModel;

    m_pctrlWingNameLabel->setText(m_pBlade->getName());
    if (m_pBlade->m_bisSinglePolar) m_pctrlSingleMultiLabel->setText("");
    else m_pctrlSingleMultiLabel->setText("Multi Reynolds Number Polars");

    QString str;
    GetLengthUnit(str, g_mainFrame->m_LengthUnit);

    QString text, blades;//height;
    blades.sprintf("%.0f",double(m_pBlade->m_blades));

    //height.sprintf("%.2f",fabs(m_pBlade->m_TPos[m_pBlade->m_NPanel]-m_pBlade->m_TPos[0])*g_mainFrame->m_mtoUnit);
    //text = "Rotor has "+blades+" blades and "+height+" "+str+" Height";
    text = "Rotor has "+blades+" blades";
    if (m_pBlade->m_bIsInverted) text += "; airfoil suction side facing outwards";
    else text += "; airfoil suction side facing inwards";

    m_pctrlBladesAndHeightLabel->setText(text);

    m_pWingModel = new QStandardItemModel;
    m_pWingModel->setRowCount(100);//temporary
    m_pWingModel->setColumnCount(8);

    m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("Height(")+str+")");
    m_pWingModel->setHeaderData(1, Qt::Horizontal, tr("Chord(")+str+")");
    m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("Radius(")+str+")");
    m_pWingModel->setHeaderData(3, Qt::Horizontal, tr("Twist"));
    m_pWingModel->setHeaderData(4, Qt::Horizontal, tr("Circ Ang"));
    m_pWingModel->setHeaderData(5, Qt::Horizontal, tr("P Axis"));
    m_pWingModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Foil"));
    m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar"));
    if (m_pBlade->m_bisSinglePolar) m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar"));
    else m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Polar Range"));

    m_pctrlBladeTableView->setModel(m_pWingModel);

    OnResize();

    m_iSection = -1;
    FillDataTable();
    ComputeGeometry(true);
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

void QDMS::PaintPowerGraphs(QPainter &painter)
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

void QDMS::PaintRotorGraphs(QPainter &painter)
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

void QDMS::PaintCharacteristicsGraphs(QPainter &painter)
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

void QDMS::PaintCharLegend(QPoint place, int bottom, QPainter &painter)
{
        int LegendSize, LegendWidth, ypos, x1;
        QColor color;
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

        for (j=0; j < g_verticalRotorStore.size(); j++)
        {
                CBlade *pWing = g_verticalRotorStore.at(j);
				for (i=0; i<g_dmsdataStore.size(); i++)
                {
						if (g_dmsdataStore.at(i)->m_WingName == pWing->getName() && g_dmsdataStore.at(i)->m_Cp.size())
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
				for (l=0; l < g_dmsdataStore.size(); l++)
                {
						if (g_dmsdataStore.at(l)->m_Cp.size() &&
								g_dmsdataStore.at(l)->m_Cp.length() &&
								g_dmsdataStore.at(l)->m_bIsVisible &&
								g_dmsdataStore.at(l)->m_WingName == str.at(k))
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
				for (nc=0; nc < g_dmsdataStore.size(); nc++)
                {
						if(str.at(k) == g_dmsdataStore.at(nc)->m_WingName)
                        {
								if (g_dmsdataStore.at(nc)->m_Cp.size() && g_dmsdataStore.at(nc)->m_WingName.length() && g_dmsdataStore.at(nc)->m_bIsVisible)

                                {
                                        //is there anything to draw ?
										LegendPen.setColor(g_dmsdataStore.at(nc)->m_Color);
										LegendPen.setStyle(GetStyle(g_dmsdataStore.at(nc)->m_Style));
										LegendPen.setWidth(g_dmsdataStore.at(nc)->m_Width);
                                        painter.setPen(LegendPen);

                                        painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
                                                                         place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
										if(g_dmsdataStore.at(nc)->m_bShowPoints)
                                        {
                                                x1 = place.x() + (int)(1.5*LegendSize);
                                                painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
                                        }

                                        painter.setPen(TextPen);
                                        painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
																		 g_dmsdataStore.at(nc)->m_DMSName);
                                        ny++ ;
                                }
                        }
                }
                if (FoilPlrs) ny++;
        }

}

void QDMS::PaintView(QPainter &painter)
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

void QDMS::PaintPowerLegend(QPoint place, int bottom, QPainter &painter)
{
        int LegendSize, LegendWidth, ypos, x1;
        QColor color;
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


        QStringList str; // we need to make an inventory of foils

		for (j=0; j<g_verttdataStore.size(); j++)
        {

				for (i=0; i<g_tdmsdataStore.size(); i++)
                {
						if (g_tdmsdataStore.at(i)->m_TurbineName == g_verttdataStore.at(j)->m_TurbineName && g_tdmsdataStore.at(i)->m_P.size())
                        {
								str.append(g_verttdataStore.at(j)->m_TurbineName);
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
				for (l=0; l < g_tdmsdataStore.size(); l++)
                {
						if (g_tdmsdataStore.at(l)->m_P.size() &&
								g_tdmsdataStore.at(l)->m_P.length() &&
								g_tdmsdataStore.at(l)->m_bIsVisible &&
								g_tdmsdataStore.at(l)->m_TurbineName == str.at(k))
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
				for (nc=0; nc < g_tdmsdataStore.size(); nc++)
                {
						if(str.at(k) == g_tdmsdataStore.at(nc)->m_TurbineName)
                        {
								if (g_tdmsdataStore.at(nc)->m_P.size() && g_tdmsdataStore.at(nc)->m_TurbineName.length() && g_tdmsdataStore.at(nc)->m_bIsVisible)

                                {
                                        //is there anything to draw ?
										LegendPen.setColor(g_tdmsdataStore.at(nc)->m_Color);
										LegendPen.setStyle(GetStyle(g_tdmsdataStore.at(nc)->m_Style));
										LegendPen.setWidth(g_tdmsdataStore.at(nc)->m_Width);
                                        painter.setPen(LegendPen);

                                        painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
                                                                         place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
										if(g_tdmsdataStore.at(nc)->m_bShowPoints)
                                        {
                                                x1 = place.x() + (int)(1.5*LegendSize);
                                                painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
                                        }

                                        painter.setPen(TextPen);
                                        painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
																		 g_tdmsdataStore.at(nc)->m_SimName);
                                        ny++ ;
                                }
                        }
                }
                if (FoilPlrs) ny++;
        }

}


void QDMS::LoadSettings(QSettings *pSettings)
{
    pSettings->beginGroup("XDMS");
	{

    dlg_lambda      =       pSettings->value("Lambda").toDouble();
    dlg_epsilon     =       pSettings->value("Epsilon").toDouble();
	dlg_iterations  =       pSettings->value("Interations").toInt();
    dlg_elements    =       pSettings->value("Elements").toInt();
    dlg_rho         =       pSettings->value("Rho").toDouble();
    dlg_relax       =       pSettings->value("Relax").toDouble();
    dlg_tiploss     =       pSettings->value("TipLoss").toBool();
	dlg_aspectratio =		pSettings->value("AspectRatio").toBool();
	dlg_variable    =       pSettings->value("Variable").toBool();

    dlg_lambdastart =       pSettings->value("lambdastart").toDouble();
    dlg_lambdastart =       pSettings->value("lambdastart").toDouble();
    dlg_windspeed   =       pSettings->value("tsrwindspeed").toDouble();

    dlg_lambdaend   =       pSettings->value("lambdaend").toDouble();
    dlg_lambdadelta =       pSettings->value("lambdadelta").toDouble();
    dlg_windstart   =       pSettings->value("windstart").toDouble();
    dlg_windend     =       pSettings->value("windend").toDouble();
	dlg_winddelta   =       pSettings->value("winddelta").toDouble();
	dlg_pitchstart  =       pSettings->value("pitchstart").toDouble();
	dlg_pitchend    =       pSettings->value("pitchend").toDouble();
	dlg_pitchdelta  =       pSettings->value("pitchdelta").toDouble();
	dlg_rotstart    =       pSettings->value("rotstart").toDouble();
	dlg_rotend      =       pSettings->value("rotend").toDouble();
	dlg_rotdelta    =       pSettings->value("rotdelta").toDouble();
	dlg_windstart2  =       pSettings->value("windstartt").toDouble();
	dlg_windend2    =       pSettings->value("windendt").toDouble();
	dlg_winddelta2  =       pSettings->value("winddeltat").toDouble();
    dlg_visc        =       pSettings->value("visc").toDouble();
    dlg_powerlaw    =       pSettings->value("powerlaw").toBool();
    dlg_constant    =       pSettings->value("constant").toBool();
    dlg_logarithmic =       pSettings->value("logarithmic").toBool();
    dlg_exponent    =       pSettings->value("exponent").toDouble();
    dlg_roughness   =       pSettings->value("roughness").toDouble();
    }
    pSettings->endGroup();

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
	SetWeibullGraphTitles(&m_PowerGraph1);
	SetWeibullGraphTitles(&m_PowerGraph2);
	SetWeibullGraphTitles(&m_PowerGraph3);
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


void QDMS::OnBladeView()
{

	g_mainFrame->setIView(BLADEVIEW,DMS);// new JW //m_iView = BLADEVIEW;
	g_mainFrame->setIApp(DMS);  // NM vorher: g_mainFrame->m_iApp = DMS;

    if (!m_WingEdited) UpdateBlades();

    OnCenterScene();

	g_mainFrame->OnDMS();

    CheckButtons();
	g_mainFrame->SetCentralWidget();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    g_mainFrame->m_pctrlDMSWidget->setMinimumWidth(width/m_widthfrac*2);

    UpdateView();
    configureGL();
}


void QDMS::OnRotorsimView()
{

    g_mainFrame->setIView(BEMSIMVIEW,DMS);
    g_mainFrame->setIApp(DMS);

    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_CharLegendOffset.rx() = 10;
    m_CharLegendOffset.ry() = h2 + 30;

    if(!m_WingEdited) UpdateBlades();
	g_mainFrame->OnDMS();

    CheckButtons();
	g_mainFrame->SetCentralWidget();
    UpdateView();
	g_mainFrame->setIView(BEMSIMVIEW,DMS);// new JW, important for size change!
}


void QDMS::OnCharView()
{
	g_mainFrame->setIView(CHARSIMVIEW,DMS);// new JW //m_iView = CHARSIMVIEW;
	g_mainFrame->setIApp(DMS);

	int h  = m_rCltRect.height();
	int h2  = (int)(h/2);

	m_CharLegendOffset.rx() = 10;
	m_CharLegendOffset.ry() = h2 + 30;

	if(!m_WingEdited) UpdateBlades();

	g_mainFrame->OnDMS();

	CheckButtons();
	g_mainFrame->SetCentralWidget();
	UpdateView();
	g_mainFrame->setIView(CHARSIMVIEW,DMS);// new JW, important for size change!
}


void QDMS::OnTurbineView()
{
    g_mainFrame->setIView(TURBINEVIEW,DMS);
    g_mainFrame->setIApp(DMS);

    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

	m_PowerLegendOffset.rx() = 10;
    m_PowerLegendOffset.ry() = h2 + 30;

    if (!m_TurbineEdited) UpdateTurbines();

	g_mainFrame->OnDMS();

    CheckButtons();
	g_mainFrame->SetCentralWidget();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    g_mainFrame->m_pctrlDMSWidget->setFixedWidth(width/m_widthfrac);

    UpdateView();
}

void QDMS::OnNewBlade()
{
    if (!g_360PolarStore.size()) return;

    ObjectIsEdited = true;

	m_WingEdited = true;

	DisableAllButtons();
		
	CBlade *pWing = new CBlade;

    m_pBlade=pWing;

    m_pBlade->m_Airfoils.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Airfoils.append("-----");

    m_pBlade->m_Polar.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Polar.append("-----");

    m_pBlade->m_Range.append("-----");  // NM dirty fix. First two entries are needed for the table in the dock
    m_pBlade->m_Range.append("-----");

    for (int i=0;i<2;i++) m_pBlade->m_TPos[i]=0+i*0.5;
    for (int i=0;i<2;i++) m_pBlade->m_TChord[i]=0.2;
    for (int i=0;i<2;i++) m_pBlade->m_TOffsetX[i]=0.2;
    for (int i=0;i<2;i++) m_pBlade->m_TTwist[i]=90;
    for (int i=0;i<2;i++) m_pBlade->m_TDihedral[i]=0;
    for (int i=0;i<2;i++) m_pBlade->m_TFoilPAxisX[i]=0.5;

    m_pBlade->setName("New Blade");

    QString newname(m_pBlade->getName());
    for (int i=0;i<g_verticalRotorStore.size();i++){
        if (newname == g_verticalRotorStore.at(i)->getName()){
            newname = makeNameWithHigherNumber(newname);
            i = 0;
        }
    }

    m_pBlade->setName(newname);

    m_pctrlInvertBox->setChecked(false);
	
    m_SingleMultiGroup->button(0)->setChecked(m_pBlade->m_bisSinglePolar);
	
    OnCenterScene();
    InitDialog(m_pBlade);

    m_pctrlBlades->setValue(3);


	bladeWidget->setCurrentIndex(1);
	mainWidget->setCurrentIndex(0);
}


void QDMS::OnCellChanged()
{
        ReadParams(true);
        double max=0;

        if (m_pBlade)
        {
        for (int i=0;i<=m_pBlade->m_NPanel;i++)
        {
            if (m_pBlade->m_TOffsetX[i]>max)
                max=m_pBlade->m_TOffsetX[i];
        }
        m_pBlade->m_MaxRadius=max;
        }
}

void QDMS::OnShowOpPoint()
{
        //user has toggled visible switch
		SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

		m_bShowOpPoint = pSimuWidgetDMS->m_pctrlShowOpPoint->isChecked();


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

void QDMS::OnGraphSettings()
{
        QGraph *pGraph = NULL;
        GraphDlg dlg;
        dlg.move(g_mainFrame->m_DlgPos);

        pGraph = m_pCurGraph;
        if(!pGraph) return;

		if (g_mainFrame->m_iView == BEMSIMVIEW && m_pCurGraph->m_Type == 0)  dlg.m_iGraphType = 103;
		if (g_mainFrame->m_iView == BEMSIMVIEW && m_pCurGraph->m_Type == 1)  dlg.m_iGraphType = 104;
		if (g_mainFrame->m_iView == BEMSIMVIEW && m_pCurGraph->m_Type == 2)  dlg.m_iGraphType = 105;

		if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 0) dlg.m_iGraphType = 106;
		if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 1) dlg.m_iGraphType = 107;
		if (g_mainFrame->m_iView == TURBINEVIEW && m_pCurGraph->m_Type == 2) dlg.m_iGraphType = 108;

		if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCurGraph->m_MainVar == 0) dlg.m_iGraphType = 109;
		if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCurGraph->m_MainVar == 1) dlg.m_iGraphType = 110;
		if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCurGraph->m_MainVar == 2) dlg.m_iGraphType = 111;

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
								SetWeibullGraphTitles(&m_PowerGraph1);
                        }
                        else if(&m_PowerGraph2 == pGraph)
                        {
                                SetPowerGraphTitles(&m_PowerGraph2);
								SetWeibullGraphTitles(&m_PowerGraph2);
                        }
                        else if(&m_PowerGraph3 == pGraph)
                        {
                                SetPowerGraphTitles(&m_PowerGraph3);
								SetWeibullGraphTitles(&m_PowerGraph3);
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
                        CreatePolarCurve();
						CreateCharacteristicsCurves();

        }
        else
        {
                pGraph->CopySettings(&graph);
        }
        UpdateView();
}


void QDMS::OnHideAllRotorCurves()
{
	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		for (int i=0; i< g_dmsdataStore.size();i++)
        {
			g_dmsdataStore.at(i)->m_bIsVisible = false;
            if (m_pDMSData) m_pDMSData->m_bIsVisible = true;
        }
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {

		for (int i=0;i<g_tdmsdataStore.size();i++)
        {
			g_tdmsdataStore.at(i)->m_bIsVisible = false;
            if (m_pTDMSData) m_pTDMSData->m_bIsVisible = true;
        }

    }

    CreateRotorCurves();
    CreatePowerCurves();
    SetCurveParams();
}


void QDMS::OnInsertBefore()
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

        m_pBlade->m_Airfoils.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Polar.append("-----");// add new dummy station to temporarily store values
        m_pBlade->m_Range.append("-----");// add new dummy station to temporarily store values


        n  = m_iSection;
        for (k=m_pBlade->m_NPanel; k>=n; k--)
        {
                m_pBlade->m_TPos[k+1]      = m_pBlade->m_TPos[k];
                m_pBlade->m_TChord[k+1]    = m_pBlade->m_TChord[k];
                m_pBlade->m_TOffsetX[k+1]   = m_pBlade->m_TOffsetX[k];
                m_pBlade->m_TTwist[k+1]     = m_pBlade->m_TTwist[k];
                m_pBlade->m_TDihedral[k+1]  = m_pBlade->m_TDihedral[k];
                m_pBlade->m_NXPanels[k+1]   = m_pBlade->m_NXPanels[k];
                m_pBlade->m_NYPanels[k+1]   = m_pBlade->m_NYPanels[k];
                m_pBlade->m_Airfoils[k+1]      = m_pBlade->m_Airfoils[k];
                m_pBlade->m_Polar[k+1]      = m_pBlade->m_Polar[k];
                m_pBlade->m_Range[k+1]      = m_pBlade->m_Range[k];
                m_pBlade->m_TPAxisX[k+1] =   m_pBlade->m_TPAxisX[k];
                m_pBlade->m_TOffsetZ[k+1] =   m_pBlade->m_TOffsetZ[k];
                m_pBlade->m_TFoilPAxisX[k+1] = m_pBlade->m_TFoilPAxisX[k];
                m_pBlade->m_TFoilPAxisZ[k+1] = m_pBlade->m_TFoilPAxisZ[k];
                m_pBlade->m_TCircAngle[k+1] = m_pBlade->m_TCircAngle[k];

        }

        ny = m_pBlade->m_NYPanels[n-1];
        m_pBlade->m_TPos[n]    = (m_pBlade->m_TPos[n+1]    + m_pBlade->m_TPos[n-1])   /2.0;
        m_pBlade->m_TChord[n]  = (m_pBlade->m_TChord[n+1]  + m_pBlade->m_TChord[n-1]) /2.0;
        m_pBlade->m_TOffsetX[n] = (m_pBlade->m_TOffsetX[n+1] + m_pBlade->m_TOffsetX[n-1])/2.0;
        m_pBlade->m_TPAxisX[n] = (m_pBlade->m_TPAxisX[n+1]+m_pBlade->m_TPAxisX[n-1]) /2.0;
        m_pBlade->m_TOffsetZ[n] = (m_pBlade->m_TOffsetZ[n+1]+m_pBlade->m_TOffsetZ[n-1]) / 2.0;
        m_pBlade->m_TFoilPAxisX[n] = (m_pBlade->m_TFoilPAxisX[n+1]+ m_pBlade->m_TFoilPAxisX[n-1]) /2.0;
        m_pBlade->m_TFoilPAxisZ[n] = (m_pBlade->m_TFoilPAxisZ[n+1] + m_pBlade->m_TFoilPAxisZ[n-1]) / 2.0;
        m_pBlade->m_TCircAngle[n] = (m_pBlade->m_TCircAngle[n+1] + m_pBlade->m_TCircAngle[n-1]) / 2.0;


        m_pBlade->m_NXPanels[n]   = m_pBlade->m_NXPanels[n-1];
        m_pBlade->m_NYPanels[n]   = (int)(ny/2);
        m_pBlade->m_NYPanels[n-1] = ny-m_pBlade->m_NYPanels[n];
        if(m_pBlade->m_NYPanels[n]==0)   m_pBlade->m_NYPanels[n]++;
        if(m_pBlade->m_NYPanels[n-1]==0) m_pBlade->m_NYPanels[n-1]++;

        m_pBlade->m_NPanel++;

        m_iSection++;

        FillDataTable();
        ComputeGeometry(true);

        //SetWingData();

        m_bResetglSectionHighlight = true;
        ReadParams(true);
}


void QDMS::OnInsertAfter()
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
                m_pBlade->m_TPos[k]       = m_pBlade->m_TPos[k-1];
                m_pBlade->m_TChord[k]     = m_pBlade->m_TChord[k-1];
                m_pBlade->m_TOffsetX[k]    = m_pBlade->m_TOffsetX[k-1];
                m_pBlade->m_TTwist[k]     = m_pBlade->m_TTwist[k-1];
                m_pBlade->m_TDihedral[k]  = m_pBlade->m_TDihedral[k-1];
                m_pBlade->m_NXPanels[k]   = m_pBlade->m_NXPanels[k-1];
                m_pBlade->m_NYPanels[k]   = m_pBlade->m_NYPanels[k-1];
                m_pBlade->m_Airfoils[k]      = m_pBlade->m_Airfoils[k-1];
                m_pBlade->m_Polar[k]      = m_pBlade->m_Polar[k-1];
                m_pBlade->m_Range[k]      = m_pBlade->m_Range[k-1];
                m_pBlade->m_TPAxisX[k] =   m_pBlade->m_TPAxisX[k-1];
                m_pBlade->m_TOffsetZ[k] =   m_pBlade->m_TOffsetZ[k-1];
                m_pBlade->m_TFoilPAxisX[k] = m_pBlade->m_TFoilPAxisX[k-1];
                m_pBlade->m_TFoilPAxisZ[k] = m_pBlade->m_TFoilPAxisZ[k-1];
                m_pBlade->m_TCircAngle[k] = m_pBlade->m_TCircAngle[k-1];

        }

        if(n<m_pBlade->m_NPanel)
        {
                m_pBlade->m_TPos[n+1]    = (m_pBlade->m_TPos[n]    + m_pBlade->m_TPos[n+2])   /2.0;
                m_pBlade->m_TChord[n+1]  = (m_pBlade->m_TChord[n]  + m_pBlade->m_TChord[n+2]) /2.0;
                m_pBlade->m_TOffsetX[n+1] = (m_pBlade->m_TOffsetX[n] + m_pBlade->m_TOffsetX[n+2])/2.0;
                m_pBlade->m_TPAxisX[n+1] = (m_pBlade->m_TPAxisX[n]+m_pBlade->m_TPAxisX[n+2]) /2.0;
                m_pBlade->m_TOffsetZ[n+1] = (m_pBlade->m_TOffsetZ[n]+m_pBlade->m_TOffsetZ[n+2]) / 2.0;
                m_pBlade->m_TFoilPAxisX[n+1] = (m_pBlade->m_TFoilPAxisX[n]+ m_pBlade->m_TFoilPAxisX[n+2]) /2.0;
                m_pBlade->m_TFoilPAxisZ[n+1] = (m_pBlade->m_TFoilPAxisZ[n] + m_pBlade->m_TFoilPAxisZ[n+2]) / 2.0;
                m_pBlade->m_TCircAngle[n+1] = (m_pBlade->m_TCircAngle[n] + m_pBlade->m_TCircAngle[n+2]) / 2.0;

        }
        else
        {
                m_pBlade->m_TPos[n+1]     = m_pBlade->m_TPos[n+1]*1.1;
                m_pBlade->m_TChord[n+1]   = m_pBlade->m_TChord[n+1];
                m_pBlade->m_TOffsetX[n+1]  = m_pBlade->m_TOffsetX[n];
                m_pBlade->m_TTwist[n+1]     = m_pBlade->m_TTwist[n];


                //m_pBlade->m_TOffsetX[n+1]  = 0;//m_pBlade->m_TOffsetX[n+1] + m_pBlade->m_TChord[n] - m_pBlade->m_TChord[n+1] ;
        }

        m_pBlade->m_TDihedral[n+1]  = m_pBlade->m_TDihedral[n];
        m_pBlade->m_Airfoils[n+1]      = m_pBlade->m_Airfoils[n];
        m_pBlade->m_Polar[n+1]      = m_pBlade->m_Polar[n];
        m_pBlade->m_Range[n+1]      = m_pBlade->m_Range[n];

        m_pBlade->m_NXPanels[n+1]   = m_pBlade->m_NXPanels[n];
        m_pBlade->m_NYPanels[n+1]   = m_pBlade->m_NYPanels[n];
        m_pBlade->m_NYPanels[n+1] = qMax(1,(int)(ny/2));
        m_pBlade->m_NYPanels[n]   = qMax(1,ny-m_pBlade->m_NYPanels[n+1]);

        m_pBlade->m_NPanel++;

        FillDataTable();
        ComputeGeometry(true);
        ReadParams(true);
        double max=0;
        for (int i=0;i<=m_pBlade->m_NPanel;i++)
        {
            if (m_pBlade->m_TOffsetX[i]>max)
                max=m_pBlade->m_TOffsetX[i];
        }
        m_pBlade->m_MaxRadius=max;

}

void QDMS::OnOptimize()
{
   

    OptimizeDlgVAWT OptDlg(this);
    OptDlg.resize(400,150);
    OptDlg.move(g_mainFrame->m_DlgPos);
    OptDlg.InitDialog();
    if (OptDlg.exec())
    {
    }

}


void QDMS::OnScaleBlade()
{

   

    BladeScaleDlgVAWT dlg;
    dlg.move(g_mainFrame->m_DlgPos);
    dlg.InitDialog(fabs(m_pBlade->m_TPos[m_pBlade->m_NPanel]-m_pBlade->m_TPos[0]), m_pBlade->m_TPos[0], m_pBlade->m_TChord[m_iSection], m_pBlade->m_TOffsetX[m_iSection], m_pBlade->m_TTwist[m_iSection]);

    if(QDialog::Accepted == dlg.exec())
    {
            if (dlg.m_bSpan || dlg.m_bShift || dlg.m_bChord || dlg.m_bOffset || dlg.m_bTwist)
            {
                    if(m_pBlade)
                    {
                        if(dlg.m_bSpan)   ScaleSpan(dlg.m_NewSpan);
                        if(dlg.m_bShift)  ShiftSpan(dlg.m_NewShift);
                        if(dlg.m_bChord)  ScaleChord(dlg.m_NewChord);
                        if(dlg.m_bOffset) SetOffset(dlg.m_NewOffset);
                        if(dlg.m_bTwist)  SetTwist(dlg.m_NewTwist);
                    }

            }



            FillDataTable();
            OnCellChanged();
            m_bResetglGeom = true;
            m_bResetglSectionHighlight = true;
            ComputeGeometry(true);
            UpdateView();

    }

}


void QDMS::OnSaveBlade()
{
    ReadParams(true);

    m_pBlade->m_blades = m_pctrlBlades->value();

    m_pBlade->m_bIsInverted = m_pctrlInvertBox->isChecked();


    // adds all polars from the blade as parent
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

    // adds all polars from the struts as parent
    for (int i=0; i<m_pBlade->m_StrutList.size();i++){
        m_pBlade->addParent(m_pBlade->m_StrutList.at(i)->getPolar());
        if (m_pBlade->m_StrutList.at(i)->isMulti){
            for (int j=0;j<m_pBlade->m_StrutList.at(i)->m_MultiPolars.size();j++){
                m_pBlade->addParent(m_pBlade->m_StrutList.at(i)->m_MultiPolars.at(j));
            }
        }
    }

    double max = 0;
    for (int i=0;i<=m_pBlade->m_NPanel;i++)
    {
        if (m_pBlade->m_TOffsetX[i]>max)
            max=m_pBlade->m_TOffsetX[i];
    }
    m_pBlade->m_MaxRadius=max;

    if (!g_verticalRotorStore.add(m_pBlade)) m_pBlade = NULL;

    ObjectIsEdited = false;

    m_WingEdited = false;

    UpdateBlades();

    EnableAllButtons();

    CheckButtons();
}


void QDMS::OnSelChangeWing(int /*i*/)
{
        m_pBlade = m_DMSToolBar->m_rotorComboBox->currentObject();
        OnCenterScene();

            InitBladeTable();
            SetScale();
            UpdateBlades();
            CheckButtons();

}

void QDMS::OnSelChangeTurbine(int /*i*/)
{
	m_pTData = m_DMSToolBar->m_verttdataComboBox->currentObject();

	InitTurbineData(m_pTData);
	UpdateTurbines();
	CheckButtons();
}


void QDMS::OnSelChangeBladeData(int i)
{

    QString strong;
   

    CheckButtons();

	if (i>=0) strong = m_DMSToolBar->m_tsrComboBox->itemText(i);

    m_pDData = GetBladeData(strong);

    UpdateBladeData();
    CheckButtons();


}


void QDMS::OnSelChangeHeightData(int i)
{

    selected_height = i;

    CreateRotorCurves();
}


void QDMS::OnSelChangeTurbineHeightData(int i)
{

    selected_height = i;

	if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
		OnSetWeibull();
	else
		CreatePowerCurves();

}


void QDMS::OnSelChangeRotorSimulation() {
    CheckButtons();

    m_pDMSData = m_DMSToolBar->m_dmsdataComboBox->currentObject();

    UpdateRotorSimulation();
    SetCurveParams();
    CheckButtons();
}


void QDMS::OnSelChangeCharSimulation() {
    m_pCDMSData = m_DMSToolBar->m_cdmsdataComboBox->currentObject();

	UpdateCharacteristicsSimulation();

	CheckButtons();
}


void QDMS::OnSelChangeTurbineSimulation() {
    CheckButtons();

    m_pTDMSData = m_DMSToolBar->m_tdmsdataComboBox->currentObject();

    UpdateTurbineSimulation();
    SetCurveParams();
    CheckButtons();
}


void QDMS::OnSelChangeTurbineBladeData(int i)
{

    QString strong;
   
	if (i>=0) strong = m_DMSToolBar->m_turbinewindspeedComboBox->itemText(i);

    m_pTurbineDData = GetTurbineBladeData(strong);

    UpdateTurbineBladeData();

    CheckButtons();

}


void QDMS::OnDeleteBlade()
{
    if (!m_pBlade) return;

    QMessageBox msgBox;
	msgBox.setText(tr("Do you want to Delete this Blade/Rotor?"));
	msgBox.setInformativeText(tr("This will delete associated Simulation Data too!!!"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {
        CBlade *pBlade = m_pBlade;



        m_pBlade         = NULL;
        m_pDData        = NULL;
        m_pDMSData      = NULL;
        m_pTurbineDData = NULL;
        m_pTData        = NULL;
        m_pTDMSData     = NULL;

        g_verticalRotorStore.remove(pBlade);

        UpdateBlades();
        UpdateTurbines();
        CheckButtons();

    }
    if (msgBox.clickedButton() == cancelButton)
    {
        return;
    }

}


void QDMS::OnEditBlade()
{

        if (!m_pBlade) return;

        DisableAllButtons();

		if (g_mainFrame->m_iView != BLADEVIEW) OnBladeView();// new JW, old: m_iView
       
        ObjectIsEdited = true;

		m_WingEdited = true;


        CBlade *pWing = new CBlade;

        pWing->Duplicate(m_pBlade);

        m_SingleMultiGroup->button(0)->setChecked(pWing->m_bisSinglePolar);
        m_SingleMultiGroup->button(1)->setChecked(!pWing->m_bisSinglePolar);

        m_pctrlInvertBox->setChecked(pWing->m_bIsInverted);
        m_SingleMultiGroup->button(0)->setChecked(pWing->m_bisSinglePolar);
        m_SingleMultiGroup->button(1)->setChecked(!pWing->m_bisSinglePolar);

        double max=0;
        for (int i=0;i<=m_pBlade->m_NPanel;i++)
        {
            if (m_pBlade->m_TOffsetX[i]>max)
                max=m_pBlade->m_TOffsetX[i];
        }
        pWing->m_MaxRadius=max;

        InitDialog(pWing);

        m_pctrlBlades->setValue(pWing->m_blades);

        OnSingleMultiPolarChanged();

		mainWidget->setCurrentIndex(0);
//		SimWidget->show();
//		PowWidget->hide();

        bladeWidget->setCurrentIndex(1);

}


void QDMS::OnRotorGraph()
{
    m_pCurGraph->m_Type = 0;

    m_pCurGraph->SetAutoX(true);
    m_pCurGraph->SetAutoY(true);

	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		m_pCurGraph->SetXVariable(7);
        m_pCurGraph->SetYVariable(0);
        SetRotorGraphTitles(m_pCurGraph);
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {
		m_pCurGraph->SetXVariable(2);
        m_pCurGraph->SetYVariable(0);
        SetPowerGraphTitles(m_pCurGraph);
		SetWeibullGraphTitles(m_pCurGraph);
    }

    m_pCurGraph->SetAuto(true);


    CreateRotorCurves();
    CreatePowerCurves();

}


void QDMS::OnBladeGraph()
{
    m_pCurGraph->m_Type = 1;
    m_pCurGraph->SetAutoX(true);
    m_pCurGraph->SetAutoY(true);

	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		m_pCurGraph->SetXVariable(4);
        m_pCurGraph->SetYVariable(0);
        SetRotorGraphTitles(m_pCurGraph);
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {
		m_pCurGraph->SetXVariable(4);
		m_pCurGraph->SetYVariable(0);
		SetPowerGraphTitles(m_pCurGraph);
		SetWeibullGraphTitles(m_pCurGraph);
    }

    m_pCurGraph->SetAuto(true);


    CreateRotorCurves();
    CreatePowerCurves();

}



void QDMS::OnAziGraph()
{
    m_pCurGraph->m_Type = 2;
	m_pCurGraph->SetXVariable(0);
	m_pCurGraph->SetYVariable(7);

    m_pCurGraph->SetAutoX(true);
    m_pCurGraph->SetAutoY(true);

    selected_height = 0;

	SetRotorGraphTitles(m_pCurGraph);
	SetPowerGraphTitles(m_pCurGraph);
	SetWeibullGraphTitles(m_pCurGraph);

    CreateRotorCurves();
	CreatePowerCurves();
	CreatePolarCurve();
}


void QDMS::OnNewTurbine()
{

        QString str;
       

        rotwindspeeds.clear();
        rotspeeds.clear();

        GetPowerUnit(str,g_mainFrame->m_PowerUnit);
        //power1->setText(str);
        power2->setText(str);
        GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
        speed1->setText(str);
        speed2->setText(str);
        //speed3->setText(str);
        GetLengthUnit(str, g_mainFrame->m_LengthUnit);
        length0->setText(str);

        m_TurbineEdited = true;

        CheckButtons();
        DisableAllButtons();

        //m_pctrlStall->setChecked(true);
        m_pctrlFixed->setChecked(true);
        CheckTurbineButtons();

        QString newname("New Turbine");
        for (int i=0;i<g_verttdataStore.size();i++){
            if (newname == g_verttdataStore.at(i)->getName()){
                newname = makeNameWithHigherNumber(newname);
                i = 0;
            }
        }

        m_pctrlTurbineName->setText(newname);

        CBlade *pWing;
        WingSelection->clear();
        for (int i=0; i < g_verticalRotorStore.size(); i++)
        {
            pWing = g_verticalRotorStore.at(i);
            WingSelection->addItem(pWing->getName());
        }


}


void QDMS::OnEditTurbine()
{
QString str;

rotspeeds = m_pTData->rotspeeds;
rotwindspeeds = m_pTData->rotwindspeeds;


GetPowerUnit(str,g_mainFrame->m_PowerUnit);
//power1->setText(str);
power2->setText(str);

GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
speed1->setText(str);
speed2->setText(str);
//speed3->setText(str);

GetLengthUnit(str, g_mainFrame->m_LengthUnit);
length0->setText(str);

if (!m_pTData) return;

if (g_mainFrame->m_iView != TURBINEVIEW) OnTurbineView();// new JW, old: m_iView

m_TurbineEdited = true;


m_pctrlTurbineName->setText(m_pTData->m_TurbineName);

//m_pctrlStall->setChecked(m_pTData->isStall);
//m_pctrlPitch->setChecked(m_pTData->isPitch);
m_pctrlFixed->setChecked(m_pTData->isFixed);
//m_pctrl2Step->setChecked(m_pTData->is2Step);
m_pctrlVariable->setChecked(m_pTData->isVariable);
m_pctrlPrescribedRot->setChecked(m_pTData->isPrescribedRot);

m_pctrlCutIn->setValue(m_pTData->CutIn*g_mainFrame->m_mstoUnit);
m_pctrlCutOut->setValue(m_pTData->CutOut*g_mainFrame->m_mstoUnit);
//m_pctrlSwitch->SetValue(m_pTData->Switch*g_mainFrame->m_mstoUnit);

m_pctrlRot1->setValue(m_pTData->Rot1);
m_pctrlRot2->setValue(m_pTData->Rot2);
m_pctrlLambda->setValue(m_pTData->Lambda0);
//m_pctrlGenerator->SetValue(m_pTData->Generator*g_mainFrame->m_WtoUnit);

m_pctrlVariableLosses->setValue(m_pTData->VariableLosses);
m_pctrlFixedLosses->setValue(m_pTData->m_fixedLosses*g_mainFrame->m_WtoUnit);


CBlade *pWing;
WingSelection->clear();
for (int i=0; i < g_verticalRotorStore.size(); i++)
{
    pWing = g_verticalRotorStore.at(i);
    WingSelection->addItem(pWing->getName());
}

int pos = WingSelection->findText(m_pTData->m_WingName);
WingSelection->setCurrentIndex(pos);

CheckTurbineButtons();
CheckButtons();
DisableAllButtons();

}


void QDMS::OnDeleteTurbine()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to Delete this Turbine?"));
    msgBox.setInformativeText(tr("This will delete associated Simulation Data too!!"));
    QPushButton *okButton = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);


    msgBox.exec();

    if (msgBox.clickedButton() == okButton)
    {

        g_verttdataStore.remove(m_pTData);

        m_pTDMSData = NULL;
        m_pTData = NULL;

        UpdateTurbines();
        CheckButtons();
    }
    if (msgBox.clickedButton() == cancelButton)
    {
        return;
    }

}


void QDMS::OnSaveTurbine()
{
       
        TData *pTData = new TData;
        CBlade *pWing;

        pWing=GetWing(WingSelection->currentText());

        pTData->turbtype=1;
        pTData->m_TurbineName = m_pctrlTurbineName->text();
		pTData->setName(m_pctrlTurbineName->text());
        pTData->m_WingName = pWing->getName();
//        pTData->setParentName(pWing->getName());  // NM REPLACE
		pTData->setSingleParent(pWing);
        //pTData->OuterRadius = pWing->m_TPos[pWing->m_NPanel];
        //pTData->Generator = m_pctrlGenerator->Value()/g_mainFrame->m_WtoUnit;
        pTData->CutIn = m_pctrlCutIn->getValue()/g_mainFrame->m_mstoUnit;
        pTData->CutOut = m_pctrlCutOut->getValue()/g_mainFrame->m_mstoUnit;
        pTData->Lambda0 = m_pctrlLambda->getValue();
        pTData->Rot1 = m_pctrlRot1->getValue();
        pTData->Rot2 = m_pctrlRot2->getValue();

//        pTData->Switch = m_pctrlSwitch->Value()/g_mainFrame->m_mstoUnit;
//        pTData->is2Step = m_pctrl2Step->isChecked();
        pTData->isFixed = m_pctrlFixed->isChecked();
        pTData->isVariable = m_pctrlVariable->isChecked();
//        pTData->isPitch = m_pctrlPitch->isChecked();
//        pTData->isStall = m_pctrlStall->isChecked();
        pTData->isPrescribedRot = m_pctrlPrescribedRot->isChecked();

        pTData->m_fixedLosses = m_pctrlFixedLosses->getValue()/g_mainFrame->m_WtoUnit;
        pTData->VariableLosses = m_pctrlVariableLosses->getValue();
        pTData->Offset = m_pctrlOffset->getValue()/g_mainFrame->m_mtoUnit;
        pTData->RHeight = fabs(pWing->m_TPos[pWing->m_NPanel]-pWing->m_TPos[0]);
        pTData->THeight = fabs(pWing->m_TPos[pWing->m_NPanel]);
        pTData->MaxRadius = pWing->m_MaxRadius;
        pTData->SweptArea = pWing->m_sweptArea;

        pTData->rotwindspeeds = rotwindspeeds;
        pTData->rotspeeds = rotspeeds;


        if (!g_verttdataStore.add(pTData)) pTData = NULL;

        m_pTData = pTData;

        InitTurbineData(m_pTData);
        m_TurbineEdited = false;

        EnableAllButtons();
        UpdateTurbines();
        CheckButtons();


}


void QDMS::OnShowAllRotorCurves()
{
	if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
    {
		for (int i=0; i< g_dmsdataStore.size();i++)
        {
			g_dmsdataStore.at(i)->m_bIsVisible = true;
        }
    }
	if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
    {

		for( int i=0;i<g_tdmsdataStore.size();i++)
        {
			g_tdmsdataStore.at(i)->m_bIsVisible = true;
        }

    }

    CreateRotorCurves();
    CreatePowerCurves();
    SetCurveParams();
}


void QDMS::OnCreateRotorSimulation()
{

	QString strong, num;

	CreateDMSDlg pDMSDlg(this);

    strong = m_pBlade->getName() + " Simulation";

	int j=1;

	for (int i=0;i<g_dmsdataStore.size();i++)
	{
		   if (strong == g_dmsdataStore.at(i)->m_DMSName)
		   {
		   j++;
		   num.sprintf("%1.0f",double(j));
           strong = m_pBlade->getName() + " Simulation" + " ("+num+")";
		   i=0;
		   }
	}

	pDMSDlg.SimName->setText(strong);

	if (pDMSDlg.exec())
	{
		DMSData *pDMSData = new DMSData;

		pDMSData->m_DMSName = pDMSDlg.SimName->text();
		pDMSData->setName(pDMSDlg.SimName->text());

		pDMSData->elements = pDMSDlg.ElementsEdit->getValue();
		pDMSData->iterations = pDMSDlg.IterationsEdit->getValue();
		pDMSData->epsilon = pDMSDlg.EpsilonEdit->getValue();

		pDMSData->exponent = pDMSDlg.ExpEdit->getValue();
		pDMSData->roughness = pDMSDlg.RoughEdit->getValue();
		pDMSData->m_bPowerLaw = pDMSDlg.PowerLawRadio->isChecked();
		pDMSData->m_bConstant = pDMSDlg.ConstantRadio->isChecked();
		pDMSData->m_bLogarithmic = pDMSDlg.LogarithmicRadio->isChecked();
		pDMSData->m_bTipLoss = pDMSDlg.TipLossBox->isChecked();
		pDMSData->m_bAspectRatio = pDMSDlg.AspectRatioBox->isChecked();
		pDMSData->m_bVariable = pDMSDlg.VariableBox->isChecked();

		pDMSData->relax = pDMSDlg.RelaxEdit->getValue();
		pDMSData->m_Color = g_mainFrame->GetColor(13);
        pDMSData->m_WingName = m_pBlade->getName();
        pDMSData->setSingleParent(m_pBlade);
		pDMSData->visc = pDMSDlg.ViscEdit->getValue();
		pDMSData->rho = pDMSDlg.RhoEdit->getValue();




        if (!g_dmsdataStore.add(pDMSData)) pDMSData = NULL;


		m_pDMSData = pDMSData;



		//// set selected values as default values for next simulation
		dlg_rho = pDMSData->rho;
		dlg_visc = pDMSData->visc;
		dlg_relax = pDMSData->relax;
		dlg_iterations = pDMSData->iterations;
		dlg_elements = pDMSData->elements;
		dlg_epsilon = pDMSData->epsilon;
		dlg_powerlaw = pDMSData->m_bPowerLaw;
		dlg_constant = pDMSData->m_bConstant;
		dlg_logarithmic = pDMSData->m_bLogarithmic;
		dlg_exponent = pDMSData->exponent;
		dlg_roughness = pDMSData->roughness;
		dlg_tiploss = pDMSData->m_bTipLoss;
		dlg_aspectratio = pDMSData->m_bAspectRatio;
		dlg_variable = pDMSData->m_bVariable;


		/*
		dlg_rootloss = pDMSData->m_bRootLoss;
		dlg_3dcorrection = pDMSData->m_b3DCorrection;
		dlg_interpolation = pDMSData->m_bInterpolation;
		dlg_newrootloss = pDMSData->m_bNewRootLoss;
		dlg_newtiploss = pDMSData->m_bNewTipLoss;
		*/
		CheckButtons();
		UpdateRotorSimulation();
	}

}


void QDMS::OnStartRotorSimulation()
{

   
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

    double lstart, lend, ldelta, inflowspeed;
    int times;

	lstart  =   pSimuWidgetDMS->m_pctrlLSLineEdit->getValue();
	lend    =   pSimuWidgetDMS->m_pctrlLELineEdit->getValue();
	ldelta  =   pSimuWidgetDMS->m_pctrlLDLineEdit->getValue();
    inflowspeed = pSimuWidgetDMS->m_pctrlWindspeed->getValue();
    times   =   int((lend-lstart)/ldelta);

	dlg_lambdastart = pSimuWidgetDMS->m_pctrlLSLineEdit->getValue();
	dlg_lambdaend   = pSimuWidgetDMS->m_pctrlLELineEdit->getValue();
	dlg_lambdadelta = pSimuWidgetDMS->m_pctrlLDLineEdit->getValue();
    dlg_windspeed   = pSimuWidgetDMS->m_pctrlWindspeed->getValue();


    m_pDMSData->Clear();

    QProgressDialog progress("", "Abort DMS", 0, times, this);
    progress.setMinimumDuration(1000);
    progress.setModal(true);

    for (int i=0;i<=times;i++)
    {
    if (progress.wasCanceled()) break;


    QString curlambda;
    curlambda.sprintf("%.2f",lstart+i*ldelta);
    QString text = "Compute DMS for Lambda " + curlambda;
    progress.setLabelText(text);
    progress.setValue(i);



	DData *data = new DData (m_pDMSData->getName());
    m_pDMSData->Compute(data,m_pBlade,lstart+i*ldelta, inflowspeed);

    if (!data->m_bBackflow)
    {
        data->m_Color = g_mainFrame->GetColor(15);

        m_pDData = m_pDMSData->m_DData[0];

        selected_lambda = -1;
        selected_height = 0;
        CreateRotorCurves();
    }

    }


    UpdateBlades();
    SetCurveParams();
    FillComboBoxes();

}


void QDMS::OnCreateCharacteristicSimulation()
{

 QString strong, num;

 CreateDMSDlg pDMSDlg(this);

 strong = m_pBlade->getName() + " Simulation";
 int j=1;

 for (int i=0;i<g_cdmsdataStore.size();i++)
 {
		if (strong == g_cdmsdataStore.at(i)->m_SimName)
		{
		j++;
		num.sprintf("%1.0f",double(j));
        strong = m_pBlade->getName() + " Simulation" + " ("+num+")";
		i=0;
		}
 }


 pDMSDlg.SimName->setText(strong);

 if (pDMSDlg.exec())
 {
 CDMSData *pCDMSData = new CDMSData;

 pCDMSData->m_SimName = pDMSDlg.SimName->text();
 pCDMSData->setName(pDMSDlg.SimName->text());



 pCDMSData->elements = pDMSDlg.ElementsEdit->getValue();
 pCDMSData->iterations = pDMSDlg.IterationsEdit->getValue();
 pCDMSData->epsilon = pDMSDlg.EpsilonEdit->getValue();
 pCDMSData->m_bTipLoss = pDMSDlg.TipLossBox->isChecked();
 pCDMSData->m_bAspectRatio = pDMSDlg.AspectRatioBox->isChecked();
 pCDMSData->m_bVariable = pDMSDlg.VariableBox->isChecked();
 pCDMSData->m_bPowerLaw = pDMSDlg.PowerLawRadio->isChecked();
 pCDMSData->m_bConstant = pDMSDlg.ConstantRadio->isChecked();
 pCDMSData->m_bLogarithmic = pDMSDlg.LogarithmicRadio->isChecked();
 pCDMSData->exponent = pDMSDlg.ExpEdit->getValue();
 pCDMSData->roughness = pDMSDlg.RoughEdit->getValue();
 pCDMSData->relax = pDMSDlg.RelaxEdit->getValue();
 pCDMSData->m_Color = g_mainFrame->GetColor(12);
 pCDMSData->m_WingName = m_pBlade->getName();
// pCDMSData->setParentName(m_pBlade->getName());  // NM REPLACE
 pCDMSData->setSingleParent(m_pBlade);
 pCDMSData->rho = pDMSDlg.RhoEdit->getValue();
 pCDMSData->visc = pDMSDlg.ViscEdit->getValue();

 if (!g_cdmsdataStore.add(pCDMSData)) pCDMSData = NULL;

 m_pCDMSData = pCDMSData;

 //////set the selected values as standart values for next definition of a simulation///
 dlg_rho = pCDMSData->rho;
 dlg_relax = pCDMSData->relax;
 dlg_iterations = pCDMSData->iterations;
 dlg_elements = pCDMSData->elements;
 dlg_epsilon = pCDMSData->epsilon;
 dlg_visc = pCDMSData->visc;
 dlg_powerlaw = pCDMSData->m_bPowerLaw;
 dlg_constant = pCDMSData->m_bConstant;
 dlg_logarithmic = pCDMSData->m_bLogarithmic;
 dlg_exponent = pCDMSData->exponent;
 dlg_roughness = pCDMSData->roughness;
 dlg_tiploss = pCDMSData->m_bTipLoss;
 dlg_aspectratio = pCDMSData->m_bAspectRatio;
 dlg_variable = pCDMSData->m_bVariable;
 ////

 CheckButtons();
 UpdateCharacteristicsSimulation();
 }

}


void QDMS::OnStartCharacteristicSimulation()
{

	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

	DData *pDData;
	pDData = NULL;

	double vstart, vend, vdelta, windspeed;
	double rotstart, rotend, rotdelta, rot;
	double pitchstart, pitchend, pitchdelta, pitch;
	double lambda;
	int vtimes, rottimes, pitchtimes, times;

    m_pCDMSData->DeleteArrays(); //// if the simulation was run previously the old arrays are deleted

	vstart  = pSimuWidgetDMS->WindStart->getValue()/g_mainFrame->m_mstoUnit;;
	m_pCDMSData->windstart = vstart;
	vend    = pSimuWidgetDMS->WindEnd->getValue()/g_mainFrame->m_mstoUnit;;
	m_pCDMSData->windend = vend;
	vdelta  = pSimuWidgetDMS->WindDelta->getValue()/g_mainFrame->m_mstoUnit;;
	m_pCDMSData->winddelta = vdelta;
	vtimes = int((vend-vstart)/vdelta)+1;
	if (pSimuWidgetDMS->WindFixed->isChecked()) vtimes = 1;
	m_pCDMSData->windtimes = vtimes;

	rotstart    = pSimuWidgetDMS->RotStart->getValue();
	m_pCDMSData->rotstart = rotstart;
	rotend      = pSimuWidgetDMS->RotEnd->getValue();
	m_pCDMSData->rotend = rotend;
	rotdelta    = pSimuWidgetDMS->RotDelta->getValue();
	m_pCDMSData->rotdelta = rotdelta;
	rottimes = int((rotend-rotstart)/rotdelta)+1;
	if (pSimuWidgetDMS->RotFixed->isChecked()) rottimes = 1;
	m_pCDMSData->rottimes = rottimes;

	pitchstart  = pSimuWidgetDMS->PitchStart->getValue();
	m_pCDMSData->pitchstart = pitchstart;
	pitchend    = pSimuWidgetDMS->PitchEnd->getValue();
	m_pCDMSData->pitchend = pitchend;
	pitchdelta  = pSimuWidgetDMS->PitchDelta->getValue();
	m_pCDMSData->pitchdelta = pitchdelta;
	pitchtimes = int((pitchend-pitchstart)/pitchdelta)+1;
	if (pSimuWidgetDMS->PitchFixed->isChecked()) pitchtimes = 1;
	m_pCDMSData->pitchtimes = pitchtimes;

	times = (rottimes)*(vtimes)*(pitchtimes);

	dlg_windstart2  = pSimuWidgetDMS->WindStart->getValue();
	dlg_windend2    = pSimuWidgetDMS->WindEnd->getValue();
	dlg_winddelta2  = pSimuWidgetDMS->WindDelta->getValue();

	dlg_pitchstart  = pSimuWidgetDMS->PitchStart->getValue();
	dlg_pitchend    = pSimuWidgetDMS->PitchEnd->getValue();
	dlg_pitchdelta  = pSimuWidgetDMS->PitchDelta->getValue();

	dlg_rotstart    = pSimuWidgetDMS->RotStart->getValue();
	dlg_rotend      = pSimuWidgetDMS->RotEnd->getValue();
	dlg_rotdelta    = pSimuWidgetDMS->RotDelta->getValue();

	////////get wing and associated polars;
	CBlade *pWing;
    pWing = m_pBlade;

	QProgressDialog progress("", "Abort DMS", 0, times, this);
	progress.setMinimumDuration(1000);
	progress.setModal(true);

    m_pCDMSData->initArrays(vtimes,rottimes,pitchtimes);

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

				if (pDData) delete pDData;
				pDData = new DData (m_pCDMSData->getName());


				if (progress.wasCanceled())
				{
					m_pCDMSData->simulated = false;
					m_pCDMSData->DeleteArrays();
					UpdateCharacteristicsSimulation();
					return;
				}

				QString curwind;
				curwind.sprintf("%3.1f",windspeed);
				QString currot;
				currot.sprintf("%4.1f",rot);
				QString curpitch;
				curpitch.sprintf("%3.1f",pitch);
				QString text = "Compute DMS for Windspeed " + curwind + ", Rotational Speed " + currot + ", Pitch " + curpitch +"deg";
				progress.setLabelText(text);
				count++;
				progress.setValue(count);

				lambda = (rot*pWing->m_MaxRadius/60*2*PI)/windspeed;
				//lambda = pWing->m_TPos[pWing->m_NPanel]*2*PI/60/windspeed*rot;

				m_pCDMSData->Compute(pDData,pWing,lambda,pitch,windspeed);

				if (!pDData->m_bBackflow)
				{
					m_pCDMSData->m_P[i][j][k]=0.5*m_pCDMSData->rho*pow(windspeed,3)*pDData->sweptArea*pDData->cp;
					m_pCDMSData->m_Torque[i][j][k]=0.5*m_pCDMSData->rho*pow(windspeed,3)*pDData->sweptArea*pDData->cp/(rot/60*2*PI);

					m_pCDMSData->m_Lambda[i][j][k]=(lambda);
					m_pCDMSData->m_one_over_Lambda[i][j][k]=(1/lambda);

					m_pCDMSData->m_V[i][j][k]=(windspeed);
					m_pCDMSData->m_w[i][j][k]=(rot);
					m_pCDMSData->m_Pitch[i][j][k]=(pitch);

					m_pCDMSData->m_Cp[i][j][k]=(pDData->cp);
					m_pCDMSData->m_Cp1[i][j][k]=(pDData->cp1);
					m_pCDMSData->m_Cp2[i][j][k]=(pDData->cp2);
					m_pCDMSData->m_Kp[i][j][k]=(pDData->cp/pow(lambda,3));

					m_pCDMSData->m_Cm[i][j][k]=(pDData->cm);
					m_pCDMSData->m_Cm1[i][j][k]=(pDData->cm1);
					m_pCDMSData->m_Cm2[i][j][k]=(pDData->cm2);
				}
			}

		}

	}

	m_pCDMSData->simulated = true;




	UpdateCharacteristicsSimulation();
	SetCurveParams();
	FillComboBoxes();

}


void QDMS::OnCreateTurbineSimulation()
{
    QString strong, num;

    strong = m_pTData->m_TurbineName + " Simulation";

    int j=1;

	for (int i=0;i<g_tdmsdataStore.size();i++)
    {
		   if (strong == g_tdmsdataStore.at(i)->m_SimName)
           {
           j++;
           num.sprintf("%1.0f",double(j));
           strong = m_pTData->m_TurbineName + " Simulation" + " ("+num+")";
           i=0;
           }
    }


    CreateDMSDlg pDMSDlg(this);

    pDMSDlg.SimName->setText(strong);

    if (pDMSDlg.exec())
    {

    TDMSData *pTDMSData = new TDMSData;

    pTDMSData->m_SimName = pDMSDlg.SimName->text();
	pTDMSData->setName(pDMSDlg.SimName->text());

    pTDMSData->m_TurbineName = m_pTData->m_TurbineName;
//	pTDMSData->setParentName(m_pTData->m_TurbineName);  // NM REPLACE
	pTDMSData->setSingleParent(m_pTData);
    pTDMSData->relax = pDMSDlg.RelaxEdit->getValue();
    pTDMSData->elements = pDMSDlg.ElementsEdit->getValue();
    pTDMSData->iterations = pDMSDlg.IterationsEdit->getValue();
    pTDMSData->epsilon = pDMSDlg.EpsilonEdit->getValue();
    pTDMSData->m_bTipLoss = pDMSDlg.TipLossBox->isChecked();
    pTDMSData->m_bAspectRatio = pDMSDlg.AspectRatioBox->isChecked();
    pTDMSData->m_bVariable = pDMSDlg.VariableBox->isChecked();

    pTDMSData->exponent = pDMSDlg.ExpEdit->getValue();
    pTDMSData->roughness = pDMSDlg.RoughEdit->getValue();
    pTDMSData->m_bPowerLaw = pDMSDlg.PowerLawRadio->isChecked();
    pTDMSData->m_bConstant = pDMSDlg.ConstantRadio->isChecked();
    pTDMSData->m_bLogarithmic = pDMSDlg.LogarithmicRadio->isChecked();

    pTDMSData->rho = pDMSDlg.RhoEdit->getValue();
    pTDMSData->visc = pDMSDlg.ViscEdit->getValue();
    pTDMSData->m_Color = g_mainFrame->GetColor(14);

    if (!g_tdmsdataStore.add(pTDMSData)) pTDMSData = NULL;

    m_pTDMSData = pTDMSData;

    selected_windspeed = -1;

    dlg_rho = pTDMSData->rho;
    dlg_visc = pTDMSData->visc;
    dlg_relax = pTDMSData->relax;
    dlg_iterations = pTDMSData->iterations;
    dlg_elements = pTDMSData->elements;
    dlg_tiploss = pTDMSData->m_bTipLoss;
    dlg_variable = pTDMSData->m_bVariable;
    dlg_aspectratio = pTDMSData->m_bAspectRatio;
    dlg_epsilon = pTDMSData->epsilon;
    dlg_powerlaw = pTDMSData->m_bPowerLaw;
    dlg_constant = pTDMSData->m_bConstant;
    dlg_logarithmic = pTDMSData->m_bLogarithmic;
    dlg_exponent = pTDMSData->exponent;
    dlg_roughness = pTDMSData->roughness;
    /*
    dlg_rootloss = pTDMSData->m_bRootLoss;
    dlg_3dcorrection = pTDMSData->m_b3DCorrection;
    dlg_interpolation = pTDMSData->m_bInterpolation;
    dlg_newrootloss = pTDMSData->m_bNewRootLoss;
    dlg_newtiploss = pTDMSData->m_bNewTipLoss;
    */

    CheckButtons();
    UpdateTurbines();
    InitTurbineSimulationParams(m_pTDMSData);
    }
}


void QDMS::OnStartTurbineSimulation()
{
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;
	DData *data;
	double vstart, vend, vdelta, windspeed, lambda, rot, Toff;
	int times;
	
	vstart  =   pSimuWidgetDMS->m_pctrlWind1->getValue()/g_mainFrame->m_mstoUnit;
	vend    =   pSimuWidgetDMS->m_pctrlWind2->getValue()/g_mainFrame->m_mstoUnit;;
	vdelta  =   pSimuWidgetDMS->m_pctrlWindDelta->getValue()/g_mainFrame->m_mstoUnit;;
	times   =   int((vend-vstart)/vdelta);
	
	dlg_windstart   = pSimuWidgetDMS->m_pctrlWind1->getValue();
	dlg_windend     = pSimuWidgetDMS->m_pctrlWind2->getValue();
	dlg_winddelta   = pSimuWidgetDMS->m_pctrlWindDelta->getValue();
	
	m_pTDMSData->Clear();
	
	CBlade *pWing = g_verticalRotorStore.getObjectByNameOnly(m_pTData->m_WingName);
	
	QProgressDialog progress("", "Abort DMS", 0, times, this);
	progress.setMinimumDuration(1000);
	progress.setModal(true);
	
	for (int i=0;i<=times;i++)
	{
		if (progress.wasCanceled()) break;
		
		windspeed = vstart+vdelta*i;
		
		//// check which rotational speed is used (for fixed, 2step and variable)////
		if (m_pTData->isFixed) rot = m_pTData->Rot1;
		
		if (m_pTData->isVariable)
		{
			rot = m_pTData->Lambda0*windspeed*60/2/PI/m_pTData->MaxRadius;
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
		
		QString curwind;
		curwind.sprintf("%.2f",windspeed);
		QString text = "Compute DMS for Windspeed " + curwind;
		progress.setLabelText(text);
		progress.setValue(i);
		
		lambda = (rot*m_pTData->MaxRadius/60*2*PI)/windspeed;
		//lambda = m_pTData->OuterRadius*2*PI/60/windspeed*rot;
		
		Toff = m_pTData->Offset;
		
		if (windspeed>=m_pTData->CutIn && windspeed<=m_pTData->CutOut)
		{
			data= new DData (m_pTDMSData->getName());
			m_pTDMSData->Compute(data,pWing,lambda,0,Toff,windspeed);
			
			if (!data->m_bBackflow)
			{
				// fill turbine data
				m_pTDMSData->m_Omega.append(rot);
				m_pTDMSData->m_V.append(windspeed);
				m_pTDMSData->m_DData.append(data);
				
				double P = data->power;
				m_pTDMSData->m_P.append(P);
				
				double Thrust = data->thrust;
				m_pTDMSData->m_Thrust.append(Thrust);
				
				double T = data->torque;
				m_pTDMSData->m_T.append(T);
				
				double P_loss = (1-m_pTData->VariableLosses) * P - m_pTData->m_fixedLosses;
				if (P_loss > 0)
				{
					m_pTDMSData->m_P_loss.append(P_loss);
					m_pTDMSData->m_Cp_loss.append(P_loss/P);
				}
				else
				{
					m_pTDMSData->m_P_loss.append(0);
					m_pTDMSData->m_Cp_loss.append(0);
				}
				
//				m_pTDMSData->m_S.append(pow(m_pTData->OuterRadius,2)*PI*m_pTDMSData->rho/2*pow(windspeed,2)*data->cm);
				
				m_pTDMSData->m_Cp.append(data->cp);
				m_pTDMSData->m_Cp1.append(data->cp1);
				m_pTDMSData->m_Cp2.append(data->cp2);
				m_pTDMSData->m_Cm.append(data->cm);
				m_pTDMSData->m_Cm1.append(data->cm1);
				m_pTDMSData->m_Cm2.append(data->cm2);
				m_pTDMSData->m_w.append(data->w);
				m_pTDMSData->m_Lambda.append(lambda);
				m_pTDMSData->m_one_over_Lambda.append(1/data->lambda_global);
				m_pTDMSData->m_Kp.append(data->cp/pow(data->lambda_global,3));
				m_pTDMSData->m_Weibull.append(0);
				
				data->m_Color = g_mainFrame->GetColor(16);
				m_pTurbineDData = m_pTDMSData->m_DData[0];
			}
			
			selected_windspeed = -1;
			
			if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
				OnSetWeibull();
			else
				CreatePowerCurves();
		}
	}
	UpdateTurbines();
	SetCurveParams();
	FillComboBoxes();
}


void QDMS::OnDeleteRotorSim()
{
    if(m_pDMSData)
    {
               g_dmsdataStore.remove(m_pDMSData);
               m_pDMSData = NULL;
               m_pDData = NULL;
               UpdateBlades();
               CheckButtons();
    }

}


void QDMS::OnDeleteCharSim()
{
	if(m_pCDMSData)
	{		
               g_cdmsdataStore.remove(m_pCDMSData);
			   m_pCDMSData	 = NULL;
			   UpdateBlades();
			   CheckButtons();
	}

}


void QDMS::OnDeleteTurbineSim()
{
    if(m_pTDMSData)
    {
               g_tdmsdataStore.remove(m_pTDMSData);
               m_pTurbineDData = NULL;
               m_pTDMSData = NULL;
               m_pDData = NULL;
               UpdateTurbines();
    }
}


void QDMS::OnSetWeibull()
{
	double A,k,PMk,PMA,V,f,energy;

	// get A and k value
	k=m_pctrlWk2->value();
	A=m_pctrlWA2->value();

	//get plus minus range of k and A
	PMA=m_pctrlPMA2->value();
	PMk=m_pctrlPMk2->value();

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
	if (m_pTDMSData)
	{
		// for all turbine simulations
		for (int i=0; i< g_tdmsdataStore.size(); i++)
		{
			g_tdmsdataStore.at(i)->m_Weibull.clear();
			// reset selected A and k
			A = m_A[50];
			k = m_k[50];

			// calc weibull probability for selected A and k
			for (int j=0;j<g_tdmsdataStore.at(i)->m_V.size();j++)
			{
			   V=g_tdmsdataStore.at(i)->m_V.at(j);

			   f=k/A*pow((V/A),(k-1))*exp(-pow((V/A),k));
			   g_tdmsdataStore.at(i)->m_Weibull.append(f);
			}


			// calc aep for A range
			g_tdmsdataStore.at(i)->m_aepA.clear();

			for (int j=0; j<m_A.size(); j++)
			{
				A = m_A[j];
				k = m_k[50];

				// sum up annual yield for temp A
				energy=0;
				for (int m=0;m<g_tdmsdataStore.at(i)->m_V.size()-1;m++)
				{
					f = exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m+1)/A,k));

					energy = energy + 0.5*(g_tdmsdataStore.at(i)->m_P.at(m)+g_tdmsdataStore.at(i)->m_P.at(m+1))*f*8760;
				}
				g_tdmsdataStore.at(i)->m_aepA.append(energy);
			}

			// calc aep for k range
			g_tdmsdataStore.at(i)->m_aepk.clear();

			for (int j=0; j<m_k.size(); j++)
			{
				A = m_A[50];
				k = m_k[j];

				// sum up annual yield for temp k
				energy=0;
				for (int m=0;m<g_tdmsdataStore.at(i)->m_V.size()-1;m++)
				{
					f = exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m+1)/A,k));

					energy = energy + 0.5*(g_tdmsdataStore.at(i)->m_P.at(m)+g_tdmsdataStore.at(i)->m_P.at(m+1))*f*8760;
				}
				g_tdmsdataStore.at(i)->m_aepk.append(energy);
			}

			/*
			g_tdmsdataStore.at(i)->m_aep.clear();
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
					for (int m=0;m<g_tdmsdataStore.at(i)->m_V.size()-1;m++)
					{
						f = exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m)/A,k))-exp(-pow(g_tdmsdataStore.at(i)->m_V.at(m+1)/A,k));

						energy = energy + 0.5*(g_tdmsdataStore.at(i)->m_P.at(m)+g_tdmsdataStore.at(i)->m_P.at(m+1))*f*8760;
					}
					aep.append(energy/1000.0);
				}

				g_tdmsdataStore.at(i)->m_aep.append(aep);
			}
			*/

		}

		QString yield,str;
		GetPowerUnit(str, g_mainFrame->m_PowerUnit);
		yield.sprintf("%.0f",m_pTDMSData->m_aepk.at(50)*g_mainFrame->m_WtoUnit);
		m_pctrlYield->setText(yield+" "+str+"h");

		CreatePowerCurves();
	}
}

void QDMS::OnCenterScene()
{
	if(!m_pBlade) return;
	if (g_mainFrame->m_iApp != DMS) return;
	
	if (m_pctrlShowTurbine->isChecked())
	{
        m_pGLWidget->setSceneRadius(1.4*float(max(m_pBlade->m_MaxRadius,m_pBlade->m_TPos[m_pBlade->m_NPanel])));
		m_pGLWidget->setSceneCenter(qglviewer::Vec(0,m_pBlade->m_TPos[m_pBlade->m_NPanel]/2,0));
	}
	else
	{
        m_pGLWidget->setSceneRadius(float(m_pBlade->getRotorRadius()/2.0*1.4));
        m_pGLWidget->setSceneCenter(qglviewer::Vec(m_pBlade->m_MaxRadius/2,m_pBlade->m_TPos[m_pBlade->m_NPanel]/2,0));
	}
	m_pGLWidget->showEntireScene();
	m_pGLWidget->updateGL();
}

void QDMS::ReadSectionData(int sel)
{
        if(sel>=m_pWingModel->rowCount()) return;
        double d;

        //    for (int i=0; i< m_pWingModel->rowCount();  i++) {

        //        if (i==0) qDebug() << "length" << m_pBlade->m_TPos[i] << "angle" << "0";
        //        else{
        //            double angle = atan((m_pBlade->m_TOffsetX[i]-m_pBlade->m_TOffsetX[i-1])/(m_pBlade->m_TPos[i]-m_pBlade->m_TPos[i-1])) /PI *180.0;
        //            double length = (m_pBlade->m_TPos[i]-m_pBlade->m_TPos[i-1]) / cos (angle/180.0*PI);
        //            qDebug() << "length" << length << "angle" << angle;
        //        }

        //    }

        bool bOK;
        QString strong;
        QStandardItem *pItem;

        if (m_bisHeight){
            pItem = m_pWingModel->item(sel,0);
            strong =pItem->text();
            strong.replace(" ","");
            d =strong.toDouble(&bOK);
            if(bOK) m_pBlade->m_TPos[sel] =d / g_mainFrame->m_mtoUnit;
        }
        else{
            pItem = m_pWingModel->item(sel,0);
            strong =pItem->text();
            strong.replace(" ","");
            d =strong.toDouble(&bOK);
            if (sel == 0){
                if(bOK) m_pBlade->m_TPos[sel] = d / g_mainFrame->m_mtoUnit;
            }
            else{
                pItem = m_pWingModel->item(sel,2);
                strong =pItem->text();
                strong.replace(" ","");
                double angle =strong.toDouble(&bOK);
                if(bOK) m_pBlade->m_TPos[sel] = (d*cos(angle/180.0*PI) / g_mainFrame->m_mtoUnit + m_pBlade->m_TPos[sel-1]);
            }
        }

        pItem = m_pWingModel->item(sel,1);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TChord[sel] = d / g_mainFrame->m_mtoUnit;

        if (m_bisHeight){
            pItem = m_pWingModel->item(sel,2);
            strong =pItem->text();
            strong.replace(" ","");
            d =strong.toDouble(&bOK);
            if(bOK) m_pBlade->m_TOffsetX[sel] =d / g_mainFrame->m_mtoUnit;
            if (sel == 0) hubEdit->setValue(m_pBlade->m_TOffsetX[sel] * g_mainFrame->m_mtoUnit);
        }
        else{
            pItem = m_pWingModel->item(sel,2);
            strong =pItem->text();
            strong.replace(" ","");
            d =strong.toDouble(&bOK);
            if (sel == 0){
            if(bOK) m_pBlade->m_TOffsetX[sel] = hubEdit->getValue() / g_mainFrame->m_mtoUnit;
            }
            else{
                pItem = m_pWingModel->item(sel,0);
                strong =pItem->text();
                strong.replace(" ","");
                if(bOK) m_pBlade->m_TOffsetX[sel] = tan(d * PI / 180) * (m_pBlade->m_TPos[sel]-m_pBlade->m_TPos[sel-1]) + m_pBlade->m_TOffsetX[sel-1];
            }
        }

        pItem = m_pWingModel->item(sel,3);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TTwist[sel] =90-d;

        pItem = m_pWingModel->item(sel,4);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TCircAngle[sel] =d;

        pItem = m_pWingModel->item(sel,5);
        strong =pItem->text();
        strong.replace(" ","");
        d =strong.toDouble(&bOK);
        if(bOK) m_pBlade->m_TFoilPAxisX[sel] =d;

        pItem = m_pWingModel->item(sel,6);
        strong =pItem->text();
        m_pBlade->m_Airfoils[sel] = strong;

        QModelIndex ind;
        ind = m_pWingModel->index(sel, 7, QModelIndex());

        pItem = m_pWingModel->item(sel,7);
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


void QDMS::SaveSettings(QSettings *pSettings)
{
        pSettings->beginGroup("XDMS");
        {
            pSettings->setValue("Lambda", dlg_lambda);
            pSettings->setValue("Epsilon", dlg_epsilon);
            pSettings->setValue("Interations", dlg_iterations);
            pSettings->setValue("Elements", dlg_elements);
            pSettings->setValue("Rho", dlg_rho);
            pSettings->setValue("Relax", dlg_relax);
            pSettings->setValue("TipLoss", dlg_tiploss);
            pSettings->setValue("AspectRatio", dlg_aspectratio);
            pSettings->setValue("Variable", dlg_variable);

            pSettings->setValue("tsrwindspeed", dlg_windspeed);
            pSettings->setValue("lambdastart", dlg_lambdastart);
            pSettings->setValue("lambdaend", dlg_lambdaend);
            pSettings->setValue("lambdadelta", dlg_lambdadelta);
            pSettings->setValue("windstart", dlg_windstart);
            pSettings->setValue("windend", dlg_windend);
			pSettings->setValue("winddelta", dlg_winddelta);
			pSettings->setValue("pitchstart", dlg_pitchstart);
			pSettings->setValue("pitchend", dlg_pitchend);
			pSettings->setValue("pitchdelta", dlg_pitchdelta);
			pSettings->setValue("rotstart", dlg_rotstart);
			pSettings->setValue("rotend", dlg_rotend);
			pSettings->setValue("rotdelta", dlg_rotdelta);
			pSettings->setValue("windstartt", dlg_windstart2);
			pSettings->setValue("windendt", dlg_windend2);
			pSettings->setValue("winddeltat", dlg_winddelta2);
            pSettings->setValue("visc", dlg_visc);
            pSettings->setValue("powerlaw", dlg_powerlaw);
            pSettings->setValue("constant", dlg_constant);
            pSettings->setValue("logarithmic", dlg_logarithmic);
            pSettings->setValue("exponent", dlg_exponent);
            pSettings->setValue("roughness", dlg_roughness);
        }
        pSettings->endGroup();

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

}


void QDMS::ScaleSpan(double NewSpan)
{
        // Scales the wing span-wise to the NewSpan value
        double ratio = NewSpan/(fabs(m_pBlade->m_TPos[m_pBlade->m_NPanel]-m_pBlade->m_TPos[0]));
        for (int i=0; i<=m_pBlade->m_NPanel; i++)
        {
                m_pBlade->m_TPos[i]      *= ratio;
                m_pBlade->m_TLength[i]   *= ratio;
        }
        m_pBlade->ComputeGeometry();
}


void QDMS::ShiftSpan(double NewShift)
{
        // shifts the wing spanwise
        double shift = NewShift-m_pBlade->m_TPos[0];
        for (int i=0; i<=m_pBlade->m_NPanel; i++)
        {
                m_pBlade->m_TPos[i]      += shift;
        }
        m_pBlade->ComputeGeometry();
}


void QDMS::ScaleChord(double NewChord)
{
        // Scales the wing chord-wise so that the root chord reaches the NewChord value
        double ratio = NewChord/m_pBlade->m_TChord[m_iSection];
        for (int i=0; i<=m_pBlade->m_NPanel; i++)
        {
                m_pBlade->m_TChord[i]    *= ratio;
        }
        m_pBlade->ComputeGeometry();
}


void QDMS::SetOffset(double NewChord)
{
        double ratio = NewChord/m_pBlade->m_TOffsetX[m_iSection];
        for (int i=0; i<=m_pBlade->m_NPanel; i++)
        {
                m_pBlade->m_TOffsetX[i]    *= ratio;
        }
        m_pBlade->ComputeGeometry();
}


void QDMS::SetTwist(double Twist)
{

        //set each panel's twist
        for(int i=0; i<=m_pBlade->m_NPanel; i++){
                m_pBlade->m_TTwist[i] = Twist;

        }

        m_pBlade->ComputeGeometry();
}


void QDMS::SetRotorGraphTitles(Graph* pGraph)
{
        if(!pGraph) return;
        // rotor graph x
        if (pGraph->m_Type == 0)
        {
            switch (pGraph->GetXVariable())
            {
                    case 0:
                            pGraph->SetXTitle(tr("Cp"));
                            break;
                    case 1:
                            pGraph->SetXTitle(tr("Cp1"));
                            break;
                    case 2:
                            pGraph->SetXTitle(tr("Cp2"));
                            break;
					case 3:
                            pGraph->SetXTitle(tr("Cm"));
                            break;
					case 4:
                            pGraph->SetXTitle(tr("Cm1"));
                            break;
					case 5:
                            pGraph->SetXTitle(tr("Cm2"));
                            break;
					case 6:
                            pGraph->SetXTitle(tr("Kp"));
                            break;
					case 7:
                            pGraph->SetXTitle(tr("TSR"));
                            break;
					case 8:
                            pGraph->SetXTitle(tr("1/TSR"));
                            break;
                    case 9:
                            pGraph->SetXTitle(tr("Power"));
                            break;
                    case 10:
                            pGraph->SetXTitle(tr("Thrust"));
                            break;
                    case 11:
                            pGraph->SetXTitle(tr("Torque"));
                            break;
                    case 12:
                            pGraph->SetXTitle(tr("Rotational Speed"));
                            break;
                    case 13:
                            pGraph->SetXTitle(tr("Windspeed"));
                            break;
                    default:
                            pGraph->SetXTitle(tr("TSR"));
                            break;
            }
        }
        // blade graph x
        if (pGraph->m_Type == 1)
        {
            switch (pGraph->GetXVariable())
            {
                    case 0:
                            pGraph->SetXTitle(tr("h/H"));
                            break;
                    case 1:
                            pGraph->SetXTitle(tr("r/R"));
                            break;
                    case 2:
                            pGraph->SetXTitle(tr("c"));
                            break;
                    case 3:
                            pGraph->SetXTitle(tr("delta"));
                            break;
                    case 4:
                            pGraph->SetXTitle(tr("u_up"));
                            break;
                    case 5:
                            pGraph->SetXTitle(tr("u_dw"));
                            break;
                    case 6:
                            pGraph->SetXTitle(tr("a_up"));
                            break;
                    case 7:
                            pGraph->SetXTitle(tr("a_dw"));
                            break;
                    case 8:
                            pGraph->SetXTitle(tr("V_inf"));
                            break;
                    case 9:
                            pGraph->SetXTitle(tr("V_up"));
                            break;
                    case 10:
                            pGraph->SetXTitle(tr("V_e"));
                            break;
                    case 11:
                            pGraph->SetXTitle(tr("V_dw"));
                            break;
                    case 12:
                            pGraph->SetXTitle(tr("V_wake"));
                            break;
                    case 13:
                            pGraph->SetXTitle(tr("TSR_up"));
                            break;
                    case 14:
                            pGraph->SetXTitle(tr("TSR_dw"));
                            break;
                    case 15:
                            pGraph->SetXTitle(tr("A"));
                            break;
                    case 16:
                            pGraph->SetXTitle(tr("F_up"));
                            break;
                    case 17:
                            pGraph->SetXTitle(tr("F_dw"));
                            break;
                    case 18:
                            pGraph->SetXTitle(tr("it_up"));
                            break;
                    case 19:
                            pGraph->SetXTitle(tr("it_dw"));
                            break;
                    default:
                            pGraph->SetXTitle(tr("h/H"));
                            break;
            }
        }
        // azimuthal graph x
        if (pGraph->m_Type == 2)
        {
            switch (pGraph->GetXVariable())
            {
                    case 0:
                            pGraph->SetXTitle(tr("theta"));
                            break;
                    case 1:
                            pGraph->SetXTitle(tr("it"));
                            break;
                    case 2:
                            pGraph->SetXTitle(tr("u"));
                            break;
                    case 3:
                            pGraph->SetXTitle(tr("V"));
                            break;
                    case 4:
                            pGraph->SetXTitle(tr("W"));
                            break;
                    case 5:
                            pGraph->SetXTitle(tr("Re"));
                            break;
                    case 6:
                            pGraph->SetXTitle(tr("Delta Re"));
                            break;
                    case 7:
                            pGraph->SetXTitle(tr("F"));
                            break;
                    case 8:
                            pGraph->SetXTitle(tr("alpha"));
                            break;
                    case 9:
                            pGraph->SetXTitle(tr("Cl"));
                            break;
                    case 10:
                            pGraph->SetXTitle(tr("Cd"));
                            break;
                    case 11:
                            pGraph->SetXTitle(tr("Cl/Cd"));
                            break;
                    case 12:
                            pGraph->SetXTitle(tr("Cn"));
                            break;
                    case 13:
                            pGraph->SetXTitle(tr("Ct"));
                            break;
                    case 14:
							pGraph->SetXTitle(tr("CF_t_bla"));
                            break;
                    case 15:
							pGraph->SetXTitle(tr("CF_x_bla"));
                            break;
                    case 16:
							pGraph->SetXTitle(tr("CF_y_bla"));
                            break;
                    case 17:
							pGraph->SetXTitle(tr("CF_t_rot"));
                            break;
                    case 18:
							pGraph->SetXTitle(tr("CF_x_rot"));
                            break;
                    case 19:
							pGraph->SetXTitle(tr("CF_y_rot"));
                            break;
                    default:
                            pGraph->SetXTitle(tr("theta"));
                            break;
            }
        }

        //rotor graph y
        if (pGraph->m_Type == 0)
        {
            switch (pGraph->GetYVariable())
            {
                    case 0:
                            pGraph->SetYTitle(tr("Cp"));
                            break;
                    case 1:
                            pGraph->SetYTitle(tr("Cp1"));
                            break;
                    case 2:
                            pGraph->SetYTitle(tr("Cp2"));
                            break;
					case 3:
                            pGraph->SetYTitle(tr("Cm"));
                            break;
					case 4:
                            pGraph->SetYTitle(tr("Cm1"));
                            break;
					case 5:
                            pGraph->SetYTitle(tr("Cm2"));
                            break;
					case 6:
                            pGraph->SetYTitle(tr("Kp"));
                            break;
					case 7:
                            pGraph->SetYTitle(tr("TSR"));
                            break;
					case 8:
                            pGraph->SetYTitle(tr("1/TSR"));
                            break;
                    case 9:
                            pGraph->SetYTitle(tr("Power"));
                            break;
                    case 10:
                            pGraph->SetYTitle(tr("Thrust"));
                            break;
                    case 11:
                            pGraph->SetYTitle(tr("Torque"));
                            break;
                    case 12:
                            pGraph->SetYTitle(tr("Rotational Speed"));
                            break;
                    case 13:
                            pGraph->SetYTitle(tr("Windspeed"));
                            break;
                    default:
                            pGraph->SetYTitle(tr("TSR"));
                            break;
            }
        }
        // blade graph y
        if (pGraph->m_Type == 1)
        {
            switch (pGraph->GetYVariable())
            {
                    case 0:
                            pGraph->SetYTitle(tr("h/H"));
                            break;
                    case 1:
                            pGraph->SetYTitle(tr("r/R"));
                            break;
                    case 2:
                            pGraph->SetYTitle(tr("c"));
                            break;
                    case 3:
                            pGraph->SetYTitle(tr("delta"));
                            break;
                    case 4:
                            pGraph->SetYTitle(tr("u_up"));
                            break;
                    case 5:
                            pGraph->SetYTitle(tr("u_dw"));
                            break;
                    case 6:
                            pGraph->SetYTitle(tr("a_up"));
                            break;
                    case 7:
                            pGraph->SetYTitle(tr("a_dw"));
                            break;
                    case 8:
                            pGraph->SetYTitle(tr("V_inf"));
                            break;
                    case 9:
                            pGraph->SetYTitle(tr("V_up"));
                            break;
                    case 10:
                            pGraph->SetYTitle(tr("V_e"));
                            break;
                    case 11:
                            pGraph->SetYTitle(tr("V_dw"));
                            break;
                    case 12:
                            pGraph->SetYTitle(tr("V_wake"));
                            break;
                    case 13:
                            pGraph->SetYTitle(tr("TSR_up"));
                            break;
                    case 14:
                            pGraph->SetYTitle(tr("TSR_dw"));
                            break;
                    case 15:
                            pGraph->SetYTitle(tr("A"));
                            break;
                    case 16:
                            pGraph->SetYTitle(tr("F_up"));
                            break;
                    case 17:
                            pGraph->SetYTitle(tr("F_dw"));
                            break;
                    case 18:
                            pGraph->SetYTitle(tr("it_up"));
                            break;
                    case 19:
                            pGraph->SetYTitle(tr("it_dw"));
                            break;
                    default:
                            pGraph->SetYTitle(tr("h/H"));
                            break;
            }
        }
        // azimuthal graph y
        if (pGraph->m_Type == 2)
        {
            switch (pGraph->GetYVariable())
            {
                    case 0:
                            pGraph->SetYTitle(tr("theta"));
                            break;
                    case 1:
                            pGraph->SetYTitle(tr("it"));
                            break;
                    case 2:
                            pGraph->SetYTitle(tr("u"));
                            break;
                    case 3:
                            pGraph->SetYTitle(tr("V"));
                            break;
                    case 4:
                            pGraph->SetYTitle(tr("W"));
                            break;
                    case 5:
                            pGraph->SetYTitle(tr("Re"));
                            break;
                    case 6:
                            pGraph->SetYTitle(tr("Delta Re"));
                            break;
                    case 7:
                            pGraph->SetYTitle(tr("F"));
                            break;
                    case 8:
                            pGraph->SetYTitle(tr("alpha"));
                            break;
                    case 9:
                            pGraph->SetYTitle(tr("Cl"));
                            break;
                    case 10:
                            pGraph->SetYTitle(tr("Cd"));
                            break;
                    case 11:
                            pGraph->SetYTitle(tr("Cl/Cd"));
                            break;
                    case 12:
                            pGraph->SetYTitle(tr("Cn"));
                            break;
                    case 13:
                            pGraph->SetYTitle(tr("Ct"));
                            break;
                    case 14:
							pGraph->SetYTitle(tr("CF_t_bla"));
                            break;
                    case 15:
							pGraph->SetYTitle(tr("CF_x_bla"));
                            break;
                    case 16:
							pGraph->SetYTitle(tr("CF_y_bla"));
                            break;
                    case 17:
							pGraph->SetYTitle(tr("CF_t_rot"));
                            break;
                    case 18:
							pGraph->SetYTitle(tr("CF_x_rot"));
                            break;
                    case 19:
							pGraph->SetYTitle(tr("CF_y_rot"));
                            break;
                    default:
                            pGraph->SetYTitle(tr("theta"));
                            break;
            }
        }
}



void QDMS::SetCharGraphTitles(Graph* pGraph)
{

	if(!pGraph) return;
	{
		switch (pGraph->GetXVariable())
		{
			case 0:
					pGraph->SetXTitle(tr("P [W]"));
					break;
			case 1:
					pGraph->SetXTitle(tr("T [Nm]"));
					break;
			case 2:
					pGraph->SetXTitle(tr("V [m/s]"));
					break;
			case 3:
					pGraph->SetXTitle(tr("TSR"));
					break;
			case 4:
					pGraph->SetXTitle(tr("1/TSR"));
					break;
			case 5:
					pGraph->SetXTitle(tr("Omega [rpm]"));
					break;
			case 6:
					pGraph->SetXTitle(tr("Pitch [deg]"));
					break;
			case 7:
					pGraph->SetXTitle(tr("Cp"));
					break;
			case 8:
					pGraph->SetXTitle(tr("Cp1"));
					break;
			case 9:
					pGraph->SetXTitle(tr("Cp2"));
					break;
			case 10:
					pGraph->SetXTitle(tr("Cm"));
					break;
			case 11:
					pGraph->SetXTitle(tr("Cm1"));
					break;
			case 12:
					pGraph->SetXTitle(tr("Cm2"));
					break;
			case 13:
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
					pGraph->SetYTitle(tr("T [Nm]"));
					break;
			case 2:
					pGraph->SetYTitle(tr("V [m/s]"));
					break;
			case 3:
					pGraph->SetYTitle(tr("TSR"));
					break;
			case 4:
					pGraph->SetYTitle(tr("1/TSR"));
					break;
			case 5:
					pGraph->SetYTitle(tr("Omega [rpm]"));
					break;
			case 6:
					pGraph->SetYTitle(tr("Pitch [deg]"));
					break;
			case 7:
					pGraph->SetYTitle(tr("Cp"));
					break;
			case 8:
					pGraph->SetYTitle(tr("Cp1"));
					break;
			case 9:
					pGraph->SetYTitle(tr("Cp2"));
					break;
			case 10:
					pGraph->SetYTitle(tr("Cm"));
					break;
			case 11:
					pGraph->SetYTitle(tr("Cm1"));
					break;
			case 12:
					pGraph->SetYTitle(tr("Cm2"));
					break;
			case 13:
					pGraph->SetYTitle(tr("Kp"));
					break;
			default:
					pGraph->SetYTitle(tr("P [W]"));
					break;
		}

	}
}


void QDMS::SetPowerGraphTitles(Graph* pGraph)
{
    if(!pGraph) return;

    //turbine graph x
	if (pGraph->m_Type == 0)
    {
        switch (pGraph->GetXVariable())
        {
            case 0:
                    pGraph->SetXTitle(tr("P [W]"));
                    break;
			case 1:
                    pGraph->SetXTitle(tr("T [Nm]"));
                    break;
			case 2:
                    pGraph->SetXTitle(tr("V [m/s]"));
                    break;
			case 3:
                    pGraph->SetXTitle(tr("TSR"));
                    break;
			case 4:
                    pGraph->SetXTitle(tr("1/TSR"));
                    break;
			case 5:
                    pGraph->SetXTitle(tr("Omega [rpm]"));
                    break;
			case 6:
                    pGraph->SetXTitle(tr("Cp"));
                    break;
			case 7:
                    pGraph->SetXTitle(tr("Cp1"));
                    break;
			case 8:
                    pGraph->SetXTitle(tr("Cp2"));
                    break;
//            case 10:
//                    pGraph->SetXTitle(tr("Ct"));
//                    break;
//            case 11:
//                    pGraph->SetXTitle(tr("Ct1"));
//                    break;
//            case 12:
//                    pGraph->SetXTitle(tr("Ct2"));
//                    break;
			case 9:
                    pGraph->SetXTitle(tr("Cm"));
                    break;
			case 10:
                    pGraph->SetXTitle(tr("Cm1"));
                    break;
			case 11:
                    pGraph->SetXTitle(tr("Cm2"));
                    break;
			case 12:
                    pGraph->SetXTitle(tr("Kp"));
                    break;
			case 13:
                    pGraph->SetXTitle(tr("P_loss"));
                    break;
			case 14:
                    pGraph->SetXTitle(tr("Cp_loss"));
                    break;
			case 15:
                    pGraph->SetXTitle(tr("f Wei"));
                    break;
			case 16:
                    pGraph->SetXTitle(tr("f x V^3"));
                    break;        
            default:
                    pGraph->SetXTitle(tr("TSR"));
                    break;
        }
    }

    // blade graph x
    if (pGraph->m_Type == 1)
    {
        switch (pGraph->GetXVariable())
        {
            case 0:
                    pGraph->SetXTitle(tr("h"));
                    break;
            case 1:
                    pGraph->SetXTitle(tr("r"));
                    break;
            case 2:
                    pGraph->SetXTitle(tr("c"));
                    break;
            case 3:
                    pGraph->SetXTitle(tr("delta"));
                    break;
            case 4:
                    pGraph->SetXTitle(tr("u_up"));
                    break;
            case 5:
                    pGraph->SetXTitle(tr("u_dw"));
                    break;
            case 6:
                    pGraph->SetXTitle(tr("a_up"));
                    break;
            case 7:
                    pGraph->SetXTitle(tr("a_dw"));
                    break;
            case 8:
                    pGraph->SetXTitle(tr("V_inf"));
                    break;
            case 9:
                    pGraph->SetXTitle(tr("V_up"));
                    break;
            case 10:
                    pGraph->SetXTitle(tr("V_e"));
                    break;
            case 11:
                    pGraph->SetXTitle(tr("V_dw"));
                    break;
            case 12:
                    pGraph->SetXTitle(tr("V_wake"));
                    break;
            case 13:
                    pGraph->SetXTitle(tr("TSR_up"));
                    break;
            case 14:
                    pGraph->SetXTitle(tr("TSR_dw"));
                    break;
            case 15:
                    pGraph->SetXTitle(tr("A"));
                    break;
            case 16:
                    pGraph->SetXTitle(tr("F_up"));
                    break;
            case 17:
                    pGraph->SetXTitle(tr("F_dw"));
                    break;
            case 18:
                    pGraph->SetXTitle(tr("it_up"));
                    break;
            case 19:
                    pGraph->SetXTitle(tr("it_dw"));
                    break;
            default:
                    pGraph->SetXTitle(tr("h"));
                    break;
        }
    }

    // azimuthal graph x
    if (pGraph->m_Type == 2)
    {
        switch (pGraph->GetXVariable())
            {
            case 0:
                    pGraph->SetXTitle(tr("theta"));
                    break;
            case 1:
                    pGraph->SetXTitle(tr("it"));
                    break;
            case 2:
                    pGraph->SetXTitle(tr("u"));
                    break;
            case 3:
                    pGraph->SetXTitle(tr("V"));
                    break;
            case 4:
                    pGraph->SetXTitle(tr("W"));
                    break;
            case 5:
                    pGraph->SetXTitle(tr("Re"));
                    break;
            case 6:
                    pGraph->SetXTitle(tr("F"));
                    break;
            case 7:
                    pGraph->SetXTitle(tr("alpha"));
                    break;
            case 8:
                    pGraph->SetXTitle(tr("Cl"));
                    break;
            case 9:
                    pGraph->SetXTitle(tr("Cd"));
                    break;
            case 10:
                    pGraph->SetXTitle(tr("Cl/Cd"));
                    break;
            case 11:
                    pGraph->SetXTitle(tr("Cn"));
                    break;
            case 12:
                    pGraph->SetXTitle(tr("Ct"));
                    break;
//            case 13:
//                    pGraph->SetXTitle(tr("F_n"));
//                    break;
			case 13:
					pGraph->SetXTitle(tr("CF_t_bla"));
                    break;
			case 14:
					pGraph->SetXTitle(tr("CF_x_bla"));
                    break;
			case 15:
					pGraph->SetXTitle(tr("CF_y_bla"));
                    break;
			case 16:
					pGraph->SetXTitle(tr("Tq_bla [Nm]"));
                    break;
			case 17:
					pGraph->SetXTitle(tr("F_x_bla [N]"));
					break;
			case 18:
					pGraph->SetXTitle(tr("F_y_bla [N]"));
					break;
//            case 18:
//                    pGraph->SetXTitle(tr("F_nTot"));
//                    break;
			case 19:
					pGraph->SetXTitle(tr("CF_t_rot"));
                    break;
			case 20:
					pGraph->SetXTitle(tr("CF_x_rot"));
                    break;
			case 21:
					pGraph->SetXTitle(tr("CF_y_rot"));
                    break;
			case 22:
					pGraph->SetXTitle(tr("Tq_rot [Nm]"));
                    break;
			case 23:
					pGraph->SetXTitle(tr("F_x_rot [N]"));
					break;
			case 24:
					pGraph->SetXTitle(tr("F_y_rot [N]"));
					break;
            default:
                    pGraph->SetXTitle(tr("theta"));
                    break;
        }
    }

    // turbine graph y
    if (pGraph->m_Type == 0)
    {
        switch (pGraph->GetYVariable())
        {
            case 0:
                    pGraph->SetYTitle(tr("P [W]"));
                    break;
			case 1:
                    pGraph->SetYTitle(tr("T [Nm]"));
                    break;
			case 2:
                    pGraph->SetYTitle(tr("V [m/s]"));
                    break;
			case 3:
                    pGraph->SetYTitle(tr("TSR"));
                    break;
			case 4:
                    pGraph->SetYTitle(tr("1/TSR"));
                    break;
			case 5:
                    pGraph->SetYTitle(tr("Omega [rpm]"));
                    break;
			case 6:
                    pGraph->SetYTitle(tr("Cp"));
                    break;
			case 7:
                    pGraph->SetYTitle(tr("Cp1"));
                    break;
			case 8:
                    pGraph->SetYTitle(tr("Cp2"));
                    break;
//            case 10:
//                    pGraph->SetYTitle(tr("Ct"));
//                    break;
//            case 11:
//                    pGraph->SetYTitle(tr("Ct1"));
//                    break;
//            case 12:
//                    pGraph->SetYTitle(tr("Ct2"));
//                    break;
			case 9:
                    pGraph->SetYTitle(tr("Cm"));
                    break;
			case 10:
                    pGraph->SetYTitle(tr("Cm1"));
                    break;
			case 11:
                    pGraph->SetYTitle(tr("Cm2"));
                    break;
			case 12:
                    pGraph->SetYTitle(tr("Kp"));
                    break;
			case 13:
                    pGraph->SetYTitle(tr("P_loss"));
                    break;
			case 14:
                    pGraph->SetYTitle(tr("Cp_loss"));
                    break;
			case 15:
                    pGraph->SetYTitle(tr("f Wei"));
                    break;
			case 16:
                    pGraph->SetYTitle(tr("f x V^3"));
                    break;
            default:
                    pGraph->SetYTitle(tr("TSR"));
                    break;
        }
    }

    // blade graph y
    if (pGraph->m_Type == 1)
    {
        switch (pGraph->GetYVariable())
        {
            case 0:
                    pGraph->SetYTitle(tr("h"));
                    break;
            case 1:
                    pGraph->SetYTitle(tr("r"));
                    break;
            case 2:
                    pGraph->SetYTitle(tr("c"));
                    break;
            case 3:
                    pGraph->SetYTitle(tr("delta"));
                    break;
            case 4:
                    pGraph->SetYTitle(tr("u_up"));
                    break;
            case 5:
                    pGraph->SetYTitle(tr("u_dw"));
                    break;
            case 6:
                    pGraph->SetYTitle(tr("a_up"));
                    break;
            case 7:
                    pGraph->SetYTitle(tr("a_dw"));
                    break;
            case 8:
                    pGraph->SetYTitle(tr("V_inf"));
                    break;
            case 9:
                    pGraph->SetYTitle(tr("V_up"));
                    break;
            case 10:
                    pGraph->SetYTitle(tr("V_e"));
                    break;
            case 11:
                    pGraph->SetYTitle(tr("V_dw"));
                    break;
            case 12:
                    pGraph->SetYTitle(tr("V_wake"));
                    break;
            case 13:
                    pGraph->SetYTitle(tr("TSR_up"));
                    break;
            case 14:
                    pGraph->SetYTitle(tr("TSR_dw"));
                    break;
            case 15:
                    pGraph->SetYTitle(tr("A"));
                    break;
            case 16:
                    pGraph->SetYTitle(tr("F_up"));
                    break;
            case 17:
                    pGraph->SetYTitle(tr("F_dw"));
                    break;
            case 18:
                    pGraph->SetYTitle(tr("it_up"));
                    break;
            case 19:
                    pGraph->SetYTitle(tr("it_dw"));
                    break;
            default:
                    pGraph->SetYTitle(tr("h"));
                    break;
        }
    }

    // azimuthal graph y
    if (pGraph->m_Type == 2)
    {
        switch (pGraph->GetYVariable())
        {
            case 0:
                    pGraph->SetYTitle(tr("theta"));
                    break;
            case 1:
                    pGraph->SetYTitle(tr("it"));
                    break;
            case 2:
                    pGraph->SetYTitle(tr("u"));
                    break;
            case 3:
                    pGraph->SetYTitle(tr("V"));
                    break;
            case 4:
                    pGraph->SetYTitle(tr("W"));
                    break;
            case 5:
                    pGraph->SetYTitle(tr("Re"));
                    break;
            case 6:
                    pGraph->SetYTitle(tr("F"));
                    break;
            case 7:
                    pGraph->SetYTitle(tr("alpha"));
                    break;
            case 8:
                    pGraph->SetYTitle(tr("Cl"));
                    break;
            case 9:
                    pGraph->SetYTitle(tr("Cd"));
                    break;
            case 10:
                    pGraph->SetYTitle(tr("Cl/Cd"));
                    break;
            case 11:
                    pGraph->SetYTitle(tr("Cn"));
                    break;
            case 12:
                    pGraph->SetYTitle(tr("Ct"));
                    break;
//            case 13:
//                    pGraph->SetYTitle(tr("F_n"));
//                    break;
			case 13:
					pGraph->SetYTitle(tr("CF_t_bla"));
                    break;
			case 14:
					pGraph->SetYTitle(tr("CF_x_bla"));
                    break;
			case 15:
					pGraph->SetYTitle(tr("CF_y_bla"));
                    break;
			case 16:
					pGraph->SetYTitle(tr("Tq_bla [Nm]"));
					break;
			case 17:
					pGraph->SetYTitle(tr("F_x_bla [N]"));
					break;
			case 18:
					pGraph->SetYTitle(tr("F_y_bla [N]"));
					break;
	//            case 18:
	//                    pGraph->SetXTitle(tr("F_nTot"));
	//                    break;
			case 19:
					pGraph->SetYTitle(tr("CF_t_rot"));
					break;
			case 20:
					pGraph->SetYTitle(tr("CF_x_rot"));
					break;
			case 21:
					pGraph->SetYTitle(tr("CF_y_rot"));
					break;
			case 22:
					pGraph->SetYTitle(tr("Tq_rot [Nm]"));
					break;
			case 23:
					pGraph->SetYTitle(tr("F_x_rot [N]"));
					break;
			case 24:
					pGraph->SetYTitle(tr("F_y_rot [N]"));
					break;
            default:
                    pGraph->SetYTitle(tr("theta"));
                    break;

        }
    }
}


void QDMS::SetupLayout()
{

    //--------------------Wing Table Layout--------------//

    QVBoxLayout *BottomLayout = new QVBoxLayout;
    QHBoxLayout *EditNewLayout = new QHBoxLayout;

    m_pctrlWingNameLabel = new QLabel;
    m_pctrlBladesAndHeightLabel = new QLabel;
    m_pctrlBladeTableView = new QTableView;
    m_pctrlSingleMultiLabel = new QLabel;


    m_pctrlBladeTableView->setSelectionMode(QAbstractItemView::NoSelection);
	m_pctrlBladeTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//	m_pctrlBladeTableView->setFixedHeight(550);
//    m_pctrlBladeTableView->setFixedWidth(505);

    m_pctrlEditWing = new QPushButton(tr("Edit"));
    m_pctrlNewWing = new QPushButton(tr("New"));
    m_pctrlDeleteWing = new QPushButton(tr("Delete"));

    QHBoxLayout *MultiNameLayout = new QHBoxLayout;
    MultiNameLayout->addWidget(m_pctrlWingNameLabel);
    MultiNameLayout->addWidget(m_pctrlSingleMultiLabel);


    BottomLayout->addLayout(MultiNameLayout);
    BottomLayout->addWidget(m_pctrlBladesAndHeightLabel);
    BottomLayout->addWidget(m_pctrlBladeTableView);


    EditNewLayout->addWidget(m_pctrlNewWing);
    EditNewLayout->addWidget(m_pctrlEditWing);
    EditNewLayout->addWidget(m_pctrlDeleteWing);

    BottomLayout->addLayout(EditNewLayout);

    QGroupBox *WingDataBox = new QGroupBox(tr("Blade Data"));
    WingDataBox->setLayout(BottomLayout);


    //-----------Wing Edit Layout-------------//
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
    m_pctrlSectionColor->SetColor(QColor(0,0,255));
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
    m_pctrlBladesLabel = new QLabel("Number of Blades");
    m_pctrlSolidityLabel = new QLabel;
    m_pctrlInvertBox = new QCheckBox(tr("Invert Airfoils"));

    ParamLayout->addWidget(m_pctrlBladesLabel);
    ParamLayout->addWidget(m_pctrlBlades);

    QLabel *lab = new QLabel(tr("Show as:"));
    ParamLayout->addWidget(lab);

    m_heightLengthGroup = new QButtonGroup;

    radioButton = new QRadioButton (tr("Height/Radius"));
    ParamLayout->addWidget(radioButton);
    m_heightLengthGroup->addButton(radioButton, 0);

    radioButton = new QRadioButton (tr("Length/Angle"));
    ParamLayout->addWidget(radioButton);
    m_heightLengthGroup->addButton(radioButton, 1);

    m_heightLengthGroup->button(0)->setChecked(true);

    lab = new QLabel(tr("R Hub:"));
    ParamLayout->addWidget(lab);

    hubEdit = new NumberEdit;
    hubEdit->setMinimum(0);
    hubEdit->setAutomaticPrecision(3);
    hubEdit->setValue(0);

    ParamLayout->addWidget(hubEdit);

    ParamLayout->addWidget(m_pctrlInvertBox);


    m_pctrlBlades->setMinimum(1);


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
    //m_pctrlAdvancedDesign = new QPushButton(tr("Advanced Design"));

    QHBoxLayout *OptScale = new QHBoxLayout;
    QHBoxLayout *BackSave = new QHBoxLayout;

    OptScale->addWidget(m_pctrlScale);
    OptScale->addWidget(m_pctrlOptimize);
    QGroupBox *OptBox = new QGroupBox(tr("Modify Shape"));
    OptBox->setLayout(OptScale);

    BackSave->addWidget(m_pctrlBack);
    //BackSave->addWidget(m_pctrlAdvancedDesign);
    BackSave->addWidget(m_pctrlSave);

    //EditLayout->addLayout(ViewLayout222);
    EditLayout->addLayout(NameLayout);
    EditLayout->addLayout(ParamLayout);
    EditLayout->addWidget(m_pctrlSolidityLabel);
    EditLayout->addLayout(InsertLayout);
    EditLayout->addWidget(m_pctrlBladeTable);
    EditLayout->addWidget(OptBox);
    EditLayout->addLayout(BackSave);

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

    QGroupBox *ViewControl = new QGroupBox(tr("3D View Control"));
    ViewControl->setLayout(ViewLayout);

    QHBoxLayout *ViewLayout2 = new QHBoxLayout;
    ViewLayout2->addWidget(ViewControl);


            //---------------Turbine Edit Layout------------------//

            m_pctrlTurbineName = new QLineEdit;
            QGridLayout *TNameLayout = new QGridLayout;
            TNameLayout->addWidget(m_pctrlTurbineName);
            QGroupBox *TNameGroup = new QGroupBox(tr("Turbine Name"));
            TNameGroup->setLayout(TNameLayout);

            m_pctrlGeneratorTypeLabel = new QLabel(tr("Rotational Speed"));
            m_pctrlFixed = new QRadioButton(tr("Single"));
            m_pctrlVariable = new QRadioButton(tr("Variable (Optimal TSR)"));
            m_pctrlPrescribedRot = new QRadioButton(tr("Prescribed"));


            QButtonGroup *Group1 = new QButtonGroup;

            Group1->addButton(m_pctrlFixed);
            Group1->addButton(m_pctrlVariable);
            Group1->addButton(m_pctrlPrescribedRot);

            QGridLayout *TypeLayout = new QGridLayout;
            TypeLayout->addWidget(m_pctrlGeneratorTypeLabel,1,1);
            TypeLayout->addWidget(m_pctrlFixed,2,1);
            TypeLayout->addWidget(m_pctrlVariable,2,2);
            TypeLayout->addWidget(m_pctrlPrescribedRot,3,1);

            QGroupBox *TypeGroup = new QGroupBox(tr("Turbine Type"));
            TypeGroup->setLayout(TypeLayout);


            m_pctrlRotList = new QPushButton(tr("Prescribe Rot. Speed"));
            m_pctrlLambda = new NumberEdit;
            m_pctrlLambdaLabel = new QLabel(tr("TSR at Design Point"));
            m_pctrlLambda->setValue(0);


            m_pctrlCutIn = new NumberEdit;
            m_pctrlCutInLabel = new QLabel(tr("V Cut In"));

            m_pctrlRot1  = new NumberEdit;
            m_pctrlRot1->setValue(0.00001);
            m_pctrlRot1->setMinimum(0.00001);
            m_pctrlRot1Label = new QLabel(tr("Rot. Speed Min"));


            m_pctrlCutOut = new NumberEdit;
            m_pctrlCutOutLabel = new QLabel(tr("V Cut Out"));
            m_pctrlRot2  = new NumberEdit;
            m_pctrlRot2->setValue(0.00001);
            m_pctrlRot2->setMinimum(0.00001);
            m_pctrlRot2Label = new QLabel(tr("Rotational Speed"));
            m_pctrlOffset  = new NumberEdit;
            m_pctrlOffset->setMinimum(0);
            m_pctrlOffset->setAutomaticPrecision(3);
            m_pctrlOffsetLabel = new QLabel(tr("Turbine Offset"));

            m_pctrlVariableLossesLabel = new QLabel(tr("Loss Factor"));
            m_pctrlFixedLossesLabel = new QLabel(tr("Fixed Losses"));
            m_pctrlVariableLosses = new NumberEdit;
            m_pctrlFixedLosses = new NumberEdit;
            m_pctrlVariableLosses->setMinimum(0);
            m_pctrlVariableLosses->setMaximum(1);
            m_pctrlVariableLosses->setAutomaticPrecision(3);

            m_pctrlVariableLosses->setValue(0);
            m_pctrlFixedLosses->setValue(0);
            m_pctrlOffset->setValue(0);
            m_pctrlRot2->setValue(0);
            m_pctrlCutIn->setValue(0);
            m_pctrlCutOut->setValue(0);

            speed1 = new QLabel;
            speed2 = new QLabel;
            rotspeed1 = new QLabel(tr("rpm"));
            rotspeed2 = new QLabel(tr("rpm"));
            power2 = new QLabel();
            length0 = new QLabel();

            QGridLayout *SpeciLayout = new QGridLayout;
            SpeciLayout->addWidget(m_pctrlCutInLabel,1,1);
            SpeciLayout->addWidget(m_pctrlCutIn,1,2);
            SpeciLayout->addWidget(speed1,1,3);
            SpeciLayout->addWidget(m_pctrlCutOutLabel,2,1);
            SpeciLayout->addWidget(m_pctrlCutOut,2,2);
            SpeciLayout->addWidget(speed2,2,3);
            SpeciLayout->addWidget(m_pctrlRot1Label,3,1);
            SpeciLayout->addWidget(m_pctrlRot1,3,2);
            SpeciLayout->addWidget(rotspeed1,3,3);
            SpeciLayout->addWidget(m_pctrlRot2Label,4,1);
            SpeciLayout->addWidget(m_pctrlRot2,4,2);
            SpeciLayout->addWidget(rotspeed2,4,3);
            SpeciLayout->addWidget(m_pctrlLambdaLabel,5,1);
            SpeciLayout->addWidget(m_pctrlLambda,5,2);
            SpeciLayout->addWidget(m_pctrlRotList,6,2);
            SpeciLayout->addWidget(m_pctrlOffsetLabel,7,1);
            SpeciLayout->addWidget(m_pctrlOffset,7,2);
            SpeciLayout->addWidget(length0,7,3);
            SpeciLayout->addWidget(m_pctrlVariableLossesLabel,8,1);
            SpeciLayout->addWidget(m_pctrlVariableLosses,8,2);
            SpeciLayout->addWidget(m_pctrlFixedLossesLabel,9,1);
            SpeciLayout->addWidget(m_pctrlFixedLosses,9,2);
            SpeciLayout->addWidget(power2,9,3);

            QGroupBox *SpeciGroup = new QGroupBox(tr("Turbine Specification"));
            SpeciGroup->setLayout(SpeciLayout);

            WingSelection = new QComboBox;
            QGridLayout *WingLayout = new QGridLayout;
            WingLayout->addWidget(WingSelection);
            QGroupBox *WingGroup = new QGroupBox(tr("Turbine Blade"));
            WingGroup->setLayout(WingLayout);


            m_pctrlSaveTurbine = new QPushButton(tr("Save"));
			m_pctrlDiscardTurbine = new QPushButton(tr("Cancel"));

            QHBoxLayout *SaveOrBackLayout = new QHBoxLayout;
            SaveOrBackLayout->addWidget(m_pctrlDiscardTurbine);
            SaveOrBackLayout->addWidget(m_pctrlSaveTurbine);


            //----------------Turbine Show Layout----------//

            CutInLabel = new QLabel(tr("V Cut In"));
            CutOutLabel = new QLabel(tr("V Cut Out"));
            Rot2Label = new QLabel(tr("Rotational Speed Max"));
            Rot1Label = new QLabel(tr("Rotational Speed Min"));

            GeneratorTypeLabel = new QLabel(tr("Transmission"));

            FixedLossesLabel = new QLabel(tr("Fixed Losses"));
            VariableLossesLabel = new QLabel(tr("VariableLosses"));

            TurbineOffsetLabel = new QLabel(tr("Turbine Offset"));
            TurbineHeightLabel = new QLabel(tr("Turbine Height"));
            RotorHeightLabel = new QLabel(tr("Rotor Height"));
            TurbineRadiusLabel = new QLabel(tr("Rotor Max Radius"));
            TurbineSweptAreaLabel = new QLabel(tr("Rotor Swept Area"));
            BladeLabel = new QLabel(tr("Turbine Blade"));
            LambdaLabel = new QLabel(tr("Tip Speed Ratio at Design Point"));



            Trans = new QLabel;
            Trans->setAlignment(Qt::AlignRight);
            Speed1 = new QLabel;
            Speed2 = new QLabel;
            Length0 = new QLabel;
            Length1 = new QLabel;
            Length2 = new QLabel;
            Length3 = new QLabel;
            Area1 = new QLabel;
            Rotspeed1 = new QLabel(tr("rpm"));
            Rotspeed2 = new QLabel(tr("rpm"));
            Power2 = new QLabel;

            CutIn = new QLabel;
            CutIn->setAlignment(Qt::AlignRight);
            CutOut = new QLabel;
            CutOut->setAlignment(Qt::AlignRight);
            Rot1 = new QLabel;
            Rot1->setAlignment(Qt::AlignRight);
            Rot2 = new QLabel;
            Rot2->setAlignment(Qt::AlignRight);
            Lambda0 = new QLabel;
            Lambda0->setAlignment(Qt::AlignRight);

            FixedLosses = new QLabel;
            FixedLosses->setAlignment(Qt::AlignRight);
            VariableLosses = new QLabel;
            VariableLosses->setAlignment(Qt::AlignRight);

            TurbineOffset = new QLabel;
            TurbineOffset->setAlignment(Qt::AlignRight);
            TurbineHeight = new QLabel;
            TurbineHeight->setAlignment(Qt::AlignRight);
            RotorHeight = new QLabel;
            RotorHeight->setAlignment(Qt::AlignRight);
            TurbineRadius = new QLabel;
            TurbineRadius->setAlignment(Qt::AlignRight);
            TurbineSweptArea = new QLabel;
            TurbineSweptArea->setAlignment(Qt::AlignRight);
            Blade = new QLabel;
            Blade->setAlignment(Qt::AlignRight);


            QGridLayout *TurbineDataLayout = new QGridLayout;
            TurbineDataLayout->addWidget(GeneratorTypeLabel,1,1);
            TurbineDataLayout->addWidget(Trans,1,2);
            TurbineDataLayout->addWidget(CutInLabel,2,1);
            TurbineDataLayout->addWidget(CutIn,2,2);
            TurbineDataLayout->addWidget(Speed1,2,3);
            TurbineDataLayout->addWidget(CutOutLabel,3,1);
            TurbineDataLayout->addWidget(CutOut,3,2);
            TurbineDataLayout->addWidget(Speed2,3,3);
            TurbineDataLayout->addWidget(Rot1Label,4,1);
            TurbineDataLayout->addWidget(Rot1,4,2);
            TurbineDataLayout->addWidget(Rotspeed1,4,3);
            TurbineDataLayout->addWidget(Rot2Label,5,1);
            TurbineDataLayout->addWidget(Rot2,5,2);
            TurbineDataLayout->addWidget(Rotspeed2,5,3);
            TurbineDataLayout->addWidget(LambdaLabel,6,1);
            TurbineDataLayout->addWidget(Lambda0,6,2);
            TurbineDataLayout->addWidget(BladeLabel,8,1);
            TurbineDataLayout->addWidget(Blade,8,2);
            TurbineDataLayout->addWidget(TurbineOffsetLabel,9,1);
            TurbineDataLayout->addWidget(TurbineOffset,9,2);
            TurbineDataLayout->addWidget(Length0,9,3);
            TurbineDataLayout->addWidget(TurbineHeightLabel,10,1);
            TurbineDataLayout->addWidget(TurbineHeight,10,2);
            TurbineDataLayout->addWidget(Length3,10,3);
            TurbineDataLayout->addWidget(RotorHeightLabel,11,1);
            TurbineDataLayout->addWidget(RotorHeight,11,2);
            TurbineDataLayout->addWidget(Length1,11,3);
            TurbineDataLayout->addWidget(TurbineRadiusLabel,12,1);
            TurbineDataLayout->addWidget(TurbineRadius,12,2);
            TurbineDataLayout->addWidget(Length2,12,3);
            TurbineDataLayout->addWidget(TurbineSweptAreaLabel,13,1);
            TurbineDataLayout->addWidget(TurbineSweptArea,13,2);
            TurbineDataLayout->addWidget(Area1,13,3);
            TurbineDataLayout->addWidget(VariableLossesLabel,14,1);
            TurbineDataLayout->addWidget(VariableLosses,14,2);
            TurbineDataLayout->addWidget(FixedLossesLabel,15,1);
            TurbineDataLayout->addWidget(FixedLosses,15,2);
            TurbineDataLayout->addWidget(Power2,15,3);

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
			m_pctrlWk2 = new QDoubleSpinBox;
			m_pctrlWk2->setMinimum(0.01);
			m_pctrlWk2->setSingleStep(0.05);
			m_pctrlWk2->setValue(2);
			m_pctrlWkLabel = new QLabel(tr("k"));
			m_pctrlWA2 = new QDoubleSpinBox;
			m_pctrlWA2->setMinimum(0.01);
			m_pctrlWA2->setSingleStep(0.05);
			m_pctrlWA2->setValue(9);
			m_pctrlWALabel = new QLabel(tr("A"));

			m_pctrlPMk2 = new QDoubleSpinBox;
			m_pctrlPMk2->setMinimum(0.01);
			m_pctrlPMk2->setSingleStep(0.05);
			m_pctrlPMk2->setValue(3);
			m_pctrlPMkLabel = new QLabel(tr("+-"));
			m_pctrlPMA2 = new QDoubleSpinBox;
			m_pctrlPMA2->setMinimum(0.01);
			m_pctrlPMA2->setSingleStep(0.05);
			m_pctrlPMA2->setValue(3);
			m_pctrlPMALabel = new QLabel(tr("+-"));

			m_pctrlYield = new QLabel;
			m_pctrlYield->setText("");
			m_pctrlYieldLabel = new QLabel(tr("Annual Yield"));
			POwer = new QLabel;

			WeibullLayout->addWidget(m_pctrlWkLabel,1,2);
			WeibullLayout->addWidget(m_pctrlWk2,1,1);
			WeibullLayout->addWidget(m_pctrlPMkLabel,2,2);
			WeibullLayout->addWidget(m_pctrlPMk2,2,1);
			WeibullLayout->addWidget(m_pctrlWALabel,1,4);
			WeibullLayout->addWidget(m_pctrlWA2,1,3);
			WeibullLayout->addWidget(m_pctrlPMALabel,2,4);
			WeibullLayout->addWidget(m_pctrlPMA2,2,3);
			WeibullLayout->addWidget(m_pctrlYieldLabel,3,1);
			WeibullLayout->addWidget(m_pctrlYield,3,2);
			WeibullLayout->addWidget(POwer,3,3);
			QGroupBox *WeibullGroup = new QGroupBox(tr("Weibull Settings"));
			WeibullGroup->setLayout(WeibullLayout);


            //---------------------------------------------//

            QVBoxLayout *SimLayout = new QVBoxLayout;

            QVBoxLayout *PowerLayout = new QVBoxLayout;
            PowerLayout->addWidget(TurbineDataGroup);
            PowerLayout->addWidget(SDGroup);
            PowerLayout->addWidget(WeibullGroup);
			PowerLayout->addStretch(1000);

			QVBoxLayout *PowerEditLayout = new QVBoxLayout;
            PowerEditLayout->addWidget(TNameGroup);
            PowerEditLayout->addWidget(TypeGroup);
            PowerEditLayout->addWidget(SpeciGroup);
            PowerEditLayout->addWidget(WingGroup);
            PowerEditLayout->addLayout(SaveOrBackLayout);
			PowerEditLayout->addStretch(1000);

            EditWidget = new QWidget;
            EditWidget->setLayout(EditLayout);
            PowerEditWidget = new QWidget;
//			PowerEditWidget->setMaximumWidth(300);
            PowerEditWidget->setLayout(PowerEditLayout);
            PowerWidget = new QWidget;
//			PowerWidget->setMaximumWidth(300);
            PowerWidget->setLayout(PowerLayout);
            //AdvancedEditWidget = new QWidget;
            //AdvancedEditWidget->setLayout(AdvancedEditLayout);

            bladeWidget = new QStackedWidget;
            bladeWidget->addWidget(WingDataBox);
            bladeWidget->addWidget(EditWidget);
            //bladeWidget->addWidget(AdvancedEditWidget);

            SimLayout->addLayout(ViewLayout2);
            SimLayout->addWidget(bladeWidget);
			SimWidget  = new QWidget;
			SimWidget->setLayout(SimLayout);

//			PowWidget = new QStackedWidget;
//			PowWidget->addWidget(PowerWidget);
//			PowWidget->addWidget(PowerEditWidget);
			mainWidget = new QStackedWidget;
			mainWidget->addWidget(SimWidget);
			mainWidget->addWidget(PowerWidget);
			mainWidget->addWidget(PowerEditWidget);

//            QVBoxLayout *mainLayout = new QVBoxLayout;
//            mainLayout->addWidget(mainWidget);

//            setLayout(mainLayout);

            mainWidget->setMinimumWidth(100);


}



void QDMS::SetCurveParams()
{

	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

		if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if(m_pDMSData)
                {
						if(m_pDMSData->m_bIsVisible)  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(true);  else  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(false);
						if(m_pDMSData->m_bShowPoints) pSimuWidgetDMS->m_pctrlShowPoints->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowPoints->setChecked(false);
						if(m_bShowOpPoint) pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(false);


                        m_CurveColor = m_pDMSData->m_Color;
                        m_CurveStyle = m_pDMSData->m_Style;
                        m_CurveWidth = m_pDMSData->m_Width;
                        FillComboBoxes();
                }
                else
                {
                        FillComboBoxes(false);
                }
        }

		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
		{
				if(m_pTDMSData)
				{
						if(m_pTDMSData->m_bIsVisible)  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(true);  else  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(false);
						if(m_pTDMSData->m_bShowPoints) pSimuWidgetDMS->m_pctrlShowPoints->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowPoints->setChecked(false);
						if(m_bShowOpPoint) pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(false);

						m_CurveColor = m_pTDMSData->m_Color;
						m_CurveStyle = m_pTDMSData->m_Style;
						m_CurveWidth = m_pTDMSData->m_Width;
						FillComboBoxes();
				}
				else
				{
						FillComboBoxes(false);
				}
		}

		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
		{
				if(m_pCDMSData)
				{
						if(m_pCDMSData->m_bIsVisible)  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(true);  else  pSimuWidgetDMS->m_pctrlShowCurve->setChecked(false);
						if(m_pCDMSData->m_bShowPoints) pSimuWidgetDMS->m_pctrlShowPoints->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowPoints->setChecked(false);
						if(m_bShowOpPoint) pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(true); else  pSimuWidgetDMS->m_pctrlShowOpPoint->setChecked(false);

						m_CurveColor = m_pCDMSData->m_Color;
						m_CurveStyle = m_pCDMSData->m_Style;
						m_CurveWidth = m_pCDMSData->m_Width;
						FillComboBoxes();
				}
				else
				{
						FillComboBoxes(false);
				}
		}


}


void QDMS::OnShowPoints()
{
		SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if (m_pDMSData)
                {
						m_pDMSData->m_bShowPoints = pSimuWidgetDMS->m_pctrlShowPoints->isChecked();
                }
                CreateRotorCurves();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
            if (m_pTDMSData)
            {
				m_pTDMSData->m_bShowPoints = pSimuWidgetDMS->m_pctrlShowPoints->isChecked();
            }
                CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
		{
			if (m_pCDMSData)
			{
				m_pCDMSData->m_bShowPoints = pSimuWidgetDMS->m_pctrlShowPoints->isChecked();
			}
				CreateCharacteristicsCurves();
		}

        g_mainFrame->SetSaveState(false);
        UpdateView();
}


void QDMS::OnShowCurve()
{
        //user has toggled visible switch
		SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;

		if(g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: m_iView
        {
                if (m_pDMSData)
                {
						m_pDMSData->m_bIsVisible = pSimuWidgetDMS->m_pctrlShowCurve->isChecked();

                }
                CreateRotorCurves();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: m_iView
        {
            if (m_pTDMSData)
            {
				m_pTDMSData->m_bIsVisible = pSimuWidgetDMS->m_pctrlShowCurve->isChecked();
            }
                CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW)// new JW, old: m_iView
		{
			if (m_pCDMSData)
			{
				m_pCDMSData->m_bIsVisible = pSimuWidgetDMS->m_pctrlShowCurve->isChecked();
			}
				CreateCharacteristicsCurves();
		}
        g_mainFrame->SetSaveState(false);
        UpdateView();
}


void QDMS::UpdateBladeData() {
	// fills combobox with blade associated to XDMS's current blade

	int i/*, size*/, pos;
	QString strong;

	m_DMSToolBar->m_tsrComboBox->clear();
	m_DMSToolBar->m_heightComboBox->clear();
	bool exists=false;

	if(!m_pDMSData || !m_pDMSData->m_DData.size()) {
		m_DMSToolBar->m_tsrComboBox->setEnabled(false);
		selected_lambda = -1;

		m_DMSToolBar->m_heightComboBox->setEnabled(false);
	}


	//count the number of simulations associated to the current blade
	//		size = 0;
	if (m_pDMSData)
	{
		for (i=0; i<m_pDMSData->m_DData.size(); i++)
		{
			m_DMSToolBar->m_tsrComboBox->addItem(m_pDMSData->m_DData.at(i)->lambdaglobal);
			exists=true;
		}

		// if any
		if (exists)
		{
			m_DMSToolBar->m_tsrComboBox->setEnabled(true);
			m_DMSToolBar->m_heightComboBox->setEnabled(true);

			for (i=0; i<m_pDMSData->m_DData.at(0)->m_zeta.size(); i++)
			{
				m_DMSToolBar->m_heightComboBox->addItem(strong.number((m_pDMSData->m_DData.at(0)->m_zeta.at(i)+1)/2,
																	  'f',3));
			}
			m_DMSToolBar->m_heightComboBox->setCurrentIndex(selected_height);

			if(m_pDData)
			{
				pos = m_DMSToolBar->m_tsrComboBox->findText(m_pDData->lambdaglobal);
				if(pos>=0)
				{
					m_DMSToolBar->m_tsrComboBox->setCurrentIndex(pos);
					strong = m_DMSToolBar->m_tsrComboBox->itemText(pos);
					m_pDData = GetBladeData(strong);
					selected_lambda = pos;
				}
				else
				{
					m_DMSToolBar->m_tsrComboBox->setCurrentIndex(0);
					strong = m_DMSToolBar->m_tsrComboBox->itemText(0);
					m_pDData = GetBladeData(strong);
					selected_lambda = -1;
				}
			}
			else
			{
				m_DMSToolBar->m_tsrComboBox->setCurrentIndex(0);
				strong = m_DMSToolBar->m_tsrComboBox->itemText(0);
				m_pDData = GetBladeData(strong);
				selected_lambda = -1;
			}

		}

		// otherwise disable control
		if (!exists) {
			m_DMSToolBar->m_tsrComboBox->setEnabled(false);
			m_DMSToolBar->m_heightComboBox->setEnabled(false);
			m_pDData = NULL;
			selected_lambda = -1;
		}

	}

	CreateRotorCurves();
}


void QDMS::UpdateCurve()
{
		if(g_mainFrame->m_iView == BEMSIMVIEW && m_pDMSData)// new JW, old: m_iView
        {
                m_pDMSData->m_Color = m_CurveColor;
                m_pDMSData->m_Style = m_CurveStyle;
                m_pDMSData->m_Width = (int)m_CurveWidth;
                CreateRotorCurves();
        }
		else if (g_mainFrame->m_iView == TURBINEVIEW && m_pTDMSData)// new JW, old: m_iView
        {
                m_pTDMSData->m_Color = m_CurveColor;
                m_pTDMSData->m_Style = m_CurveStyle;
                m_pTDMSData->m_Width = (int)m_CurveWidth;
				if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
					OnSetWeibull();
				else
					CreatePowerCurves();
        }
		else if (g_mainFrame->m_iView == CHARSIMVIEW && m_pCDMSData)// new JW, old: m_iView
		{
				m_pCDMSData->m_Color = m_CurveColor;
				m_pCDMSData->m_Style = m_CurveStyle;
				m_pCDMSData->m_Width = (int)m_CurveWidth;
				CreateCharacteristicsCurves();
		}

        UpdateView();
        g_mainFrame->SetSaveState(false);
}


void QDMS::UpdateCharacteristicsSimulation()

{

	if (m_DMSToolBar->m_cdmsdataComboBox->count())
	{
		if (m_pCDMSData)
		{
			int pos = m_DMSToolBar->m_cdmsdataComboBox->findText(m_pCDMSData->getName());
			if(pos>=0)
			{
				m_DMSToolBar->m_cdmsdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_DMSToolBar->m_cdmsdataComboBox->setCurrentIndex(0);
				m_pCDMSData =  m_DMSToolBar->m_cdmsdataComboBox->currentObject();
			}
		}
		else
		{
			m_DMSToolBar->m_cdmsdataComboBox->setCurrentIndex(0);
			m_pCDMSData =  m_DMSToolBar->m_cdmsdataComboBox->currentObject();
		}
	}
	else
	{
		m_pCDMSData = NULL;
	}

		CreateCharacteristicsCurves();
		UpdateCharacteristicsMatrix();

		if (g_mainFrame->m_iView == CHARSIMVIEW) InitCharSimulationParams(m_pCDMSData);// new JW, old: m_iView

		if (m_pCDMSData) SetCurveParams();
		else FillComboBoxes(false);



}


void QDMS::UpdateCharacteristicsMatrix()
{
	QString strong;
	double windspeed, rot, pitch;


	m_DMSToolBar->m_windspeedComboBox->clear();
	m_DMSToolBar->m_rotComboBox->clear();
	m_DMSToolBar->m_pitchComboBox->clear();

	if (m_pCDMSData && m_pCDMSData->simulated)
	{
		for (int i=0;i<m_pCDMSData->windtimes;i++)
		{
			windspeed = m_pCDMSData->windstart+m_pCDMSData->winddelta*i;
			m_DMSToolBar->m_windspeedComboBox->addItem(strong.number(windspeed,'f',2));
		}

		for (int j=0;j<m_pCDMSData->rottimes;j++)
		{
			rot = m_pCDMSData->rotstart+m_pCDMSData->rotdelta*j;
			m_DMSToolBar->m_rotComboBox->addItem(strong.number(rot,'f',2));
		}

		for (int k=0;k<m_pCDMSData->pitchtimes;k++)
		{
		pitch = m_pCDMSData->pitchstart+m_pCDMSData->pitchdelta*k;
		m_DMSToolBar->m_pitchComboBox->addItem(strong.number(pitch,'f',2));
		}
		m_DMSToolBar->m_windspeedComboBox->setEnabled(true);
		m_DMSToolBar->m_rotComboBox->setEnabled(true);
		m_DMSToolBar->m_pitchComboBox->setEnabled(true);

	}
	else
	{
		m_DMSToolBar->m_windspeedComboBox->setEnabled(false);
		m_DMSToolBar->m_rotComboBox->setEnabled(false);
		m_DMSToolBar->m_pitchComboBox->setEnabled(false);
	}

	selected_wind = 0;
	selected_rot = 0;
	selected_pitch = 0;

	CreateCharacteristicsCurves();

}


void QDMS::UpdateRotorSimulation() {
	if (m_DMSToolBar->m_dmsdataComboBox->count())
	{
		if (m_pDMSData)
		{
			int pos = m_DMSToolBar->m_dmsdataComboBox->findText(m_pDMSData->getName());
			if(pos>=0)
			{
				m_DMSToolBar->m_dmsdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_DMSToolBar->m_dmsdataComboBox->setCurrentIndex(0);
				m_pDMSData =  m_DMSToolBar->m_dmsdataComboBox->currentObject();
			}
		}
		else
		{
			m_DMSToolBar->m_dmsdataComboBox->setCurrentIndex(0);
			m_pDMSData =  m_DMSToolBar->m_dmsdataComboBox->currentObject();
		}
	}
	else
	{
		m_pDMSData = NULL;
	}

	selected_height = 0;
	UpdateBladeData();

	InitBladeSimulationParams(m_pDMSData);

	if (m_pDMSData) SetCurveParams();
	else FillComboBoxes(false);
}


void QDMS::UpdateTurbineSimulation()

{
	if (m_DMSToolBar->m_tdmsdataComboBox->count())
	{
		if (m_pTDMSData)
		{
			int pos = m_DMSToolBar->m_tdmsdataComboBox->findText(m_pTDMSData->getName());
			if(pos>=0)
			{
				m_DMSToolBar->m_tdmsdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_DMSToolBar->m_tdmsdataComboBox->setCurrentIndex(0);
				m_pTDMSData =  m_DMSToolBar->m_tdmsdataComboBox->currentObject();
			}
		}
		else
		{
			m_DMSToolBar->m_tdmsdataComboBox->setCurrentIndex(0);
			m_pTDMSData =  m_DMSToolBar->m_tdmsdataComboBox->currentObject();
		}
	}
	else
	{
		m_pTDMSData = NULL;
	}

        InitTurbineSimulationParams(m_pTDMSData);

        if (m_pTDMSData) SetCurveParams();
        else FillComboBoxes(false);

        selected_height = 0;
        UpdateTurbineBladeData();
}


void QDMS::UpdateTurbineBladeData()

{

		int i/*, size*/, pos;
		QString strong;

		m_DMSToolBar->m_turbinewindspeedComboBox->clear();
		m_DMSToolBar->m_turbineheightComboBox->clear();
		bool exists=false;



		if(!m_pTDMSData || !m_pTDMSData->m_DData.size())
		{
				m_DMSToolBar->m_turbinewindspeedComboBox->setEnabled(false);
				selected_windspeed = -1;

				m_DMSToolBar->m_turbineheightComboBox->setEnabled(false);
		}

//		size = 0;

		if (m_pTDMSData)
		{
		//count the number of simulations associated to the current wing
		for (i=0; i<m_pTDMSData->m_DData.size(); i++)
		{
		m_DMSToolBar->m_turbinewindspeedComboBox->addItem(m_pTDMSData->m_DData.at(i)->windspeedStr);
		exists=true;
		}

		if (exists)
		{

				// if any
				m_DMSToolBar->m_turbinewindspeedComboBox->setEnabled(true);
				m_DMSToolBar->m_turbineheightComboBox->setEnabled(true);

				for (i=0; i<m_pTDMSData->m_DData.at(0)->m_zeta.size(); i++)
				{
					m_DMSToolBar->m_turbineheightComboBox->addItem(strong.number((m_pTDMSData->m_DData.at(0)->m_zeta.at(i)+1)/2,'f',3));
				}
				m_DMSToolBar->m_turbineheightComboBox->setCurrentIndex(selected_height);

				if(m_pTurbineDData)
				{

						pos = m_DMSToolBar->m_turbinewindspeedComboBox->findText(m_pTurbineDData->windspeedStr);
						if(pos>=0)
						{
							m_DMSToolBar->m_turbinewindspeedComboBox->setCurrentIndex(pos);
							strong = m_DMSToolBar->m_turbinewindspeedComboBox->itemText(pos);
							m_pTurbineDData = GetTurbineBladeData(strong);
							selected_windspeed = pos;
						}
						else
						{
							m_DMSToolBar->m_turbinewindspeedComboBox->setCurrentIndex(0);
							strong = m_DMSToolBar->m_turbinewindspeedComboBox->itemText(0);
							m_pTurbineDData = GetTurbineBladeData(strong);
							selected_windspeed = -1;
						}
				}
				else
				{
					m_DMSToolBar->m_turbinewindspeedComboBox->setCurrentIndex(0);
					strong = m_DMSToolBar->m_turbinewindspeedComboBox->itemText(0);
					m_pTurbineDData = GetTurbineBladeData(strong);
					selected_windspeed = -1;
				}

		}

	}

		if (!exists)
		{


				// otherwise disable control
				m_DMSToolBar->m_turbinewindspeedComboBox->setEnabled(false);
				m_DMSToolBar->m_turbinewindspeedComboBox->setEnabled(false);
				m_pTurbineDData = NULL;
				selected_windspeed = -1;
		}

		if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
			OnSetWeibull();
		else
			CreatePowerCurves();
}


void QDMS::UpdateUnits()
{
   
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS * ) m_pSimuWidgetDMS;
    QString str;
    GetSpeedUnit(str, g_mainFrame->m_SpeedUnit);
	pSimuWidgetDMS->speed1->setText(str);
	pSimuWidgetDMS->speed2->setText(str);
	pSimuWidgetDMS->speed3->setText(str);

	GetLengthUnit(str, g_mainFrame->m_LengthUnit);

    InitTurbineData(m_pTData);
	if (m_pTDMSData) OnSetWeibull();

}


void QDMS::UpdateBlades()
{

   m_pBlade = m_DMSToolBar->m_rotorComboBox->currentObject();

   if (g_mainFrame->m_iView==BEMSIMVIEW)
	   UpdateRotorSimulation();
   else if (g_mainFrame->m_iView==CHARSIMVIEW)
	   UpdateCharacteristicsSimulation();

   InitBladeTable();
   OnLengthHeightChanged();
}

TData * QDMS::GetTurbine(QString m_TurbineName)
{
	for (int i=0;i<g_verttdataStore.size();i++)
	{
		if (g_verttdataStore.at(i)->m_TurbineName == m_TurbineName) return g_verttdataStore.at(i);
	}
	return NULL;
}

void QDMS::UpdateTurbines()
{
	if (m_DMSToolBar->m_verttdataComboBox->count())
	{
		if (m_pTData)
		{
			int pos = m_DMSToolBar->m_verttdataComboBox->findText(m_pTData->getName());
			if(pos>=0)
			{
				m_DMSToolBar->m_verttdataComboBox->setCurrentIndex(pos);
			}
			else
			{
				m_DMSToolBar->m_verttdataComboBox->setCurrentIndex(0);
				m_pTData =  m_DMSToolBar->m_verttdataComboBox->currentObject();
			}
		}
		else
		{
			m_DMSToolBar->m_verttdataComboBox->setCurrentIndex(0);
			m_pTData =  m_DMSToolBar->m_verttdataComboBox->currentObject();
		}
	}
	else
	{
		m_pTData = NULL;
	}



	InitTurbineData(m_pTData);



	if (m_PowerGraph1.m_Type==3||m_PowerGraph2.m_Type==3||m_PowerGraph3.m_Type==3)
		OnSetWeibull();
	else
		CreatePowerCurves();

	UpdateTurbineSimulation();
}

void QDMS::onModuleChanged() {

	if (g_mainFrame->m_iApp == DMS) {

		m_DMSToolBar->hide();

		g_mainFrame->m_pctrlDMSWidget->hide();
		g_mainFrame->m_pctrlSimuWidgetDMS->hide();
		
		g_mainFrame->OnRotorViewAct2->setChecked(false);
		g_mainFrame->OnTurbineViewAct2->setChecked(false);
		g_mainFrame->OnBladeViewAct2->setChecked(false);
		g_mainFrame->OnCharacteristicViewAct2->setChecked(false);
		
		glPopAttrib();  // restores the saved GL settings		
	}
}

void QDMS::drawGL() {
	if (!m_pBlade) {  // NM TODO this should not be possible! Assumption needs validation.
		return;
	}
	
	GLWidget *glWidget = g_mainFrame->getGlWidget();

	GLDraw3D();
    GLRenderView();
	
	if (m_pctrlAxes->isChecked()) {
		m_pBlade->drawCoordinateAxes();
	}

	if (m_pctrlFoilNames->isChecked()) {
		glWidget->setOverpaintFont(QFont(g_mainFrame->m_TextFont.family(), 10));
		for (int i = 0; i <= m_pBlade->m_NPanel; ++i) {
			glWidget->overpaintText(m_pBlade->m_PlanformSpan/10 + m_pBlade->m_TOffsetX[i], m_pBlade->m_TPos[i], 0.0,
									m_pBlade->m_Airfoils[i]);
		}
	}
	
	if (m_pctrlPositions->isChecked()) {
		glWidget->setOverpaintFont(QFont(g_mainFrame->m_TextFont.family(), 10));
        for (int i = 0; i <= m_pBlade->m_NPanel; ++i) {
			glWidget->overpaintText(-m_pBlade->m_PlanformSpan/10 + m_pBlade->m_TOffsetX[i], m_pBlade->m_TPos[i], 0.0,
									QString("%1 m").arg(m_pBlade->m_TPos[i], 7, 'f', 3, ' '));
        }
    }
}


QDMS *g_qdms;
