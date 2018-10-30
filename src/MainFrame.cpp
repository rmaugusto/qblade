/****************************************************************************
  
	MainFrame  Class
	Copyright (C) 2008-2010 Andre Deperrois adeperrois@xflr5.com
	
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

#include "MainFrame.h"
#include "Globals.h"
#include "Design/AFoil.h"
#include "Misc/AboutQ5.h"
#include "Misc/ObjectPropsDlg.h"
#include "Misc/LinePickerDlg.h"
#include "Misc/TranslatorDlg.h"
#include "Graph/GraphDlg.h"
#include "XDirect/XDirect.h"
#include "XDirect/BatchDlg.h"
#include "XDirect/BatchThreadDlg.h"
#include "XDirect/NacaFoilDlg.h"
#include "XDirect/InterpolateFoilsDlg.h"
#include "XDirect/CAddDlg.h"
#include "XDirect/TwoDPanelDlg.h"
#include "XDirect/FoilCoordDlg.h"
#include "XDirect/FoilGeomDlg.h"
#include "XDirect/TEGapDlg.h"
#include "XDirect/LEDlg.h"
#include "XInverse/XInverse.h"
#include <QDesktopWidget>
#include <QtWidgets>
#include "TwoDWidget.h"
#include "StoreAssociatedComboBox.h"
#include "XLLT/QLLTToolBar.h"
#include "XLLT/QLLTSimulation.h"
#include "GLWidget.h"
#include "QBladeApplication.h"


////////////////////////////////////////new code DM//////////////////////
#include "XBEM/BEM.h"
#include "XBEM/BData.h"
#include "XBEM/SimuWidget.h"
#include "XBEM/AboutBEM.h"
#include "XBEM/360Polar.h"
/////////////////////////////////end new code DM////////////////////

////////////////////////////////////////new code JW//////////////////
#include "XDMS/DMS.h"
#include "XDMS/SimuWidgetDMS.h"
//#include "Dock.h"
#include "XBEM/BEMDock.h"
#include "XBEM/BEMSimDock.h"
#include "XDMS/DMSDock.h"
#include "XDMS/DMSSimDock.h"
#include <QDebug>
/////////////////////////////////end new code JW////////////////////

/////////////// new NM ///////////////
#include "XUnsteadyBEM/WindFieldDock.h"
#include "XBEM/BEMToolbar.h"
#include "Serializer.h"
/////////// end new NM ///////////////

#include "QFem/QFEMModule.h"
#include "XLLT/QLLTModule.h"
#include "XUnsteadyBEM/WindFieldModule.h"
#include "XUnsteadyBEM/FASTModule.h"
#include "XUnsteadyBEM/AboutFAST.h"
#include "XDMS/TurDmsModule.h"
#include "XDMS/MultiDmsModule.h"
#include "Noise/NoiseModule.h"
#include "XDMS/RotDmsModule.h"


bool ObjectIsEdited = false;


MainFrame::MainFrame (QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow (parent, flags)
{
	g_mainFrame = this;  // set the global pointer to the mainFrame
	
	connect (this, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));
	
	m_currentModule = NULL;
	m_twoDWidgetInterface = NULL;
	setContextMenuPolicy(Qt::NoContextMenu);  // NM deactivate the default context menu
	/////////// end new NM ///////////////
	
    m_VersionName = QString::fromLatin1("QBlade v0.96 64bit");
	
	setWindowTitle(m_VersionName);
	setWindowIcon(QIcon(":/images/blank_logo.png"));
	
	if(!QGLFormat::hasOpenGL())
	{
		QMessageBox::warning(this, tr("Warning"), tr("Your system does not provide support for OpenGL.\nQBlade will not operate correctly."));
	}
	m_bMaximized = true;
	m_LengthUnit  = 3;
	m_AreaUnit    = 3;
	m_WeightUnit  = 1;
	m_SpeedUnit   = 0;
	m_ForceUnit   = 0;
	m_MomentUnit  = 0;
	m_PowerUnit   = 1;
	m_WORK_GROUPS = 32;
	
	
	CreateDockWindows();
	m_ProjectName = "";
	
	m_BorderClr       = QColor(255, 255, 255);
	m_BackgroundColor = QColor(255,255,255);
	m_TextColor       = QColor(0, 0, 0);
	
	m_TextFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);
	m_TextFont.setFamily(m_TextFont.defaultFamily());
	m_TextFont.setFamily("Century Gothic");
	m_TextFont.setPointSize(10);
	m_TextFont.setStyleStrategy(QFont::OpenGLCompatible);
	
	m_RefGraph.SetGraphName("Reference Graph");
	
	m_ImageFormat = 2;
	m_ExportFileType = 1;
	m_bReverseZoom = false;
	m_bAlphaChannel = false;
	m_bSaveSettings = true;
	
	m_GraphExportFilter = "Comma Separated Values (*.csv)";
	
	m_LastDirName = QDir::homePath();
	
	QDesktopWidget desktop;
	QRect r = desktop.screenGeometry();
	m_DlgPos = QPoint((int)(r.width()/3), (int)(r.height()/3));
	
	QAFoil *pAFoil       = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	/////////////new code DM////////////
	QBEM *pBEM = (QBEM *) m_pBEM;
	////////////end new code DM/////////
	/////////////new code JW////////////
	QDMS *pDMS = (QDMS *) m_pDMS;
	////////////end new code JW/////////
	
	if(LoadSettings()) {
		QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
		
		m_RefGraph.LoadSettings(&settings);
		
		pAFoil->LoadSettings(&settings);
		pXDirect->LoadSettings(&settings);
		pXInverse->LoadSettings(&settings);
		pBEM->LoadSettings(&settings);
		pDMS->LoadSettings(&settings);
	}
	
	SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit, m_PowerUnit,
			 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit, m_WtoUnit);
	
	
	pXDirect->SetAnalysisParams();
	CreateActions();
	CreateMenus();
	CreateToolbars();
	CreateStatusBar();
	
//	m_pctrlMainToolBar->addSeparator();
//	g_rotDmsModule = new RotDmsModule (this, m_pctrlMainToolBar);
//	g_multiDmsModule = new MultiDmsModule (this, m_pctrlMainToolBar);
//	g_turDmsModule = new TurDmsModule (this, m_pctrlMainToolBar);
	
	m_pctrlMainToolBar->addSeparator();
	g_windFieldModule = new WindFieldModule (this, m_pctrlMainToolBar);
	g_QLLTModule = new QLLTModule (this, m_pctrlMainToolBar);
	
	m_pctrlMainToolBar->addSeparator();
	g_QFEMModule = new QFEMModule (this, m_pctrlMainToolBar);
	g_fastModule = new FASTModule (this, m_pctrlMainToolBar);
	
    BEMViewMenu->addSeparator();  // TODO NM Why is this here?!
    BEMViewMenu->addAction(saveViewToImageFileAct);


	// second line
	m_crColors[0] = QColor(255,0,0),
			m_crColors[1] = QColor(0,255,0),
			m_crColors[2] = QColor(0,0,255),
			m_crColors[3] = QColor(0,0,0),
			m_crColors[4] = QColor(255,0,255),
			m_crColors[5] = QColor(0,255,255),
			
			// third line
			m_crColors[6]  = QColor(200,60,60),
			m_crColors[7]  = QColor(0,160,0),
			m_crColors[8]  = QColor(100,100,240),
			m_crColors[9]  = QColor(170,170,0),
			m_crColors[10] = QColor(130,0,130),
			m_crColors[11] = QColor(0,130,130),
			
			// fourth line
			m_crColors[12] = QColor(255,128,128),
			m_crColors[13] = QColor(0,255,128),
			m_crColors[14] = QColor(64,200,255),
			m_crColors[15] = QColor(170,170,100),
			m_crColors[16] = QColor(190,100,190),
			m_crColors[17] = QColor(100,170,170),
			
			// fifth line
			m_crColors[18] = QColor(228,150,70),
			m_crColors[19] = QColor(170,255,170),
			m_crColors[20] = QColor(120,120,255),
			m_crColors[21] = QColor(228,228,128),
			m_crColors[22] = QColor(255,170,255),
			m_crColors[23] = QColor(170,255,255),
			
			// first line
			m_crColors[24] = QColor(50,50,50),
			m_crColors[25] = QColor(90,90,90),
			m_crColors[26] = QColor(130,130,130),
			m_crColors[27] = QColor(170,170,170),
			m_crColors[28] = QColor(210,210,210),
			m_crColors[29] = QColor(255,255,255),
			
			g_pCurFoil = NULL;
	
	m_bSaved     = true;
	m_bHighlightOpp = m_bHighlightWOpp = false;
	
	m_iApp = 0;
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	
	////////////////////new code DM/////////////////////
	m_PowerUnit = 1;
	m_WtoUnit = 1.0;
	SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit,m_ForceUnit,m_MomentUnit,m_PowerUnit,m_mtoUnit,m_m2toUnit,m_mstoUnit,m_kgtoUnit,m_NtoUnit,m_NmtoUnit,m_WtoUnit);
	//////////end new code DM//////////////////		
	
	SetMenus();
	ConnectStores();
	OnHAWTView();
}

MainFrame::~MainFrame() {
	delete g_QFEMModule;
	delete g_QLLTModule;
	delete g_windFieldModule;
	delete g_fastModule;
	delete g_turDmsModule;
	delete g_multiDmsModule;
	delete g_rotDmsModule;
	delete g_qbem;
	delete g_qdms;
	delete g_noiseModule;
}

void MainFrame::ConnectStores()
{
	connect(&g_windFieldStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_windFieldStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_rotorStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_rotorStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_360PolarStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_360PolarStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));

    connect(&g_noiseSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
    connect(&g_noiseSimulationStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));

    connect(&g_StrutStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
    connect(&g_StrutStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_verticalRotorStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_verticalRotorStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_bladeStructureStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_bladeStructureStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_FASTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_FASTSimulationStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_QLLTHAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_QLLTHAWTSimulationStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_QLLTVAWTSimulationStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_QLLTVAWTSimulationStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_QLLTCutPlaneStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_QLLTCutPlaneStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_foilStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_foilStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_polarStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_polarStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_tdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_tdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_bemdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_bemdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_tbemdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_tbemdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_cbemdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_cbemdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_dmsdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_dmsdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_tdmsdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_tdmsdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_cdmsdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_cdmsdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_verttdataStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_verttdataStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
	
	connect(&g_oppointStore, SIGNAL(objectListChanged(bool)), this, SLOT(SetSaveStateFalse()));
	connect(&g_oppointStore, SIGNAL(objectRenamed(QString, QString)), this, SLOT(SetSaveStateFalse()));
}

void MainFrame::AboutQFLR5()
{
	AboutQ5 dlg(this);
	dlg.exec();
}

void MainFrame::onAboutPnoise() {
	QDialog *dialog = new QDialog (this);
	
	dialog->setWindowTitle("About PNoise");
	dialog->setModal(true);
	
	QVBoxLayout *vBox = new QVBoxLayout;
	dialog->setLayout(vBox);
	QHBoxLayout *hBox = new QHBoxLayout;
	vBox->addLayout(hBox);
	
	QLabel *label = new QLabel;
	label->setPixmap(QPixmap(":/images/Poli_logo_3.jpg").scaledToHeight(100, Qt::SmoothTransformation));
	label->setAlignment(Qt::AlignCenter);
	hBox->addWidget(label, Qt::AlignCenter);
	label = new QLabel;
	label->setPixmap(QPixmap(":/images/Poli_USP.png").scaledToHeight(55, Qt::SmoothTransformation));
	label->setAlignment(Qt::AlignCenter);
	hBox->addWidget(label, Qt::AlignCenter);
	label = new QLabel;
	label->setPixmap(QPixmap(":/images/TUBerlin.png").scaledToHeight(80, Qt::SmoothTransformation));
	label->setAlignment(Qt::AlignCenter);
	hBox->addWidget(label, Qt::AlignCenter);
	vBox->addSpacing(20);
	
	QScrollArea *scroll = new QScrollArea;
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scroll->setWidgetResizable(true);
	vBox->addWidget(scroll);
	
	label = new QLabel ("Copyright (C) Joseph Youssif Saab Junior (contact joseph.saab@usp.br).\n"
						"\n"
						"This airfoil noise prediction module is distributed integrated to the QBlade(C) software, "
						"under the GNU General Public License, without any warranty, explicit or implied, of "
						"merchantability or fitness for any particular purpose.\n"
						"\n"
						"The PNoise tool was developed under a collaboration among the Escola Politécnica da "
						"Universidade de São Paulo (Poli-USP) and the HF Institute of TU-Berlin.\n"
						"\n"
						"The tool is based on the NASA BPM (Brooks, Pope, Marcolini, 1989) TE noise model, with "
						"vertical turbulence scale (displacement thickness) being provided either by the original "
						"BPM correlations or by a specific flow around any airfoil geometry, calculated by the XFLR5 "
						"(Drela & Youngren, 2000), (Scherrer, 2004), (Deperrois, 2003-2012).\n"
						"\n"
						"For model validity range, please check PNoise -> Noise Simulation -> Model Validity Hint\n"
						"For code validation, please check the manual.\n"
						"\n"
						"For visualization convenience only, any negative SPL contributions from sources will be "
						"displayed as zero SPL in the graphical output.\n"
						"\n"
						"If you want to reference PNoise inside a report or publication, please make reference "
						"to the validation report provided.\n"
						"\n"
						"Please, report any bugs to joseph.saab@usp.br\n"
						"\n"
						"Future versions planned under the collaboration include additional self-noise sources, "
						"inflow noise source and a quasi-3D rotor noise tool.\n"
						"\n"
						"List of Contributors:\n"
						"Development: Joseph Saab, Marcos Pimenta, David Marten, George Pechlivanoglou.\n"
						"Programming: Ricardo Marques Augusto, Nikolai Moesus.");
	label->setWordWrap(true);
	scroll->setWidget(label);
	
	QPushButton *button = new QPushButton ("Ok");
	connect(button, SIGNAL(clicked()), dialog, SLOT(accept()));
	vBox->addWidget(button, 0, Qt::AlignRight);
	
	dialog->exec();
	
	dialog->deleteLater();
}

void MainFrame::AboutQBlade()
{
	AboutBEM dlg(this);
	dlg.exec();
}

void MainFrame::AboutFASTAerodyn()
{
	AboutFAST dlg(this);
	dlg.exec();
}

void MainFrame::AddRecentFile(const QString &PathName)
{
	m_RecentFiles.removeAll(PathName);
	m_RecentFiles.prepend(PathName);
	while (m_RecentFiles.size() > MAXRECENTFILES)
		m_RecentFiles.removeLast();
	
	updateRecentFileActions();
}

void MainFrame::CreateDMSActions() {
	QDMS *pDMS = (QDMS *) m_pDMS;
	
	AziGraphAct = new QAction(tr("Set as Azimuthal Graph"), this);
	AziGraphAct->setCheckable(true);
	AziGraphAct->setStatusTip(tr("Set as Azimuthal Graph"));
	
	OnBladeViewAct2 = new QAction(QIcon(":/images/vblade.png"), tr("VAWT Rotorblade Design"), this);
	OnBladeViewAct2->setCheckable(true);
	OnBladeViewAct2->setStatusTip(tr("Design a Rotorblade"));
	connect(OnBladeViewAct2, SIGNAL(triggered()), pDMS, SLOT(OnBladeView()));
	
	OnRotorViewAct2 = new QAction(QIcon(":/images/vrotor.png"), tr("Rotor DMS Simulation"), this);
	OnRotorViewAct2->setCheckable(true);
	OnRotorViewAct2->setStatusTip(tr("Simulate a Rotor"));
	connect(OnRotorViewAct2, SIGNAL(triggered()), pDMS, SLOT(OnRotorsimView()));
	
	OnCharacteristicViewAct2 = new QAction(QIcon(":/images/char.png"), tr("Multi Parameter DMS Simulation"), this);
	OnCharacteristicViewAct2->setCheckable(true);
	OnCharacteristicViewAct2->setStatusTip(tr("Vary Simulations over Pitch, Wind and Rotational Speed"));
	connect(OnCharacteristicViewAct2, SIGNAL(triggered()), pDMS, SLOT(OnCharView()));
	
	OnTurbineViewAct2 = new QAction(QIcon(":/images/vturb.png"), tr("Turbine DMS Simulation"), this);
	OnTurbineViewAct2->setCheckable(true);
	OnTurbineViewAct2->setStatusTip(tr("Define and simulate a Wind Turbine"));
	connect(OnTurbineViewAct2, SIGNAL(triggered()), pDMS, SLOT(OnTurbineView()));
}


void MainFrame::CreateDMSMenus()
{
	
}


void MainFrame::ConnectDMSActions()
{
	QDMS *pDMS = (QDMS *) m_pDMS;
	
	
	connect(BladeGraphAct, SIGNAL(triggered()), pDMS, SLOT(OnBladeGraph()));
	connect(RotorGraphAct, SIGNAL(triggered()), pDMS, SLOT(OnRotorGraph()));
	connect(WeibullGraphAct, SIGNAL(triggered()), pDMS, SLOT(OnWeibullGraph()));
	connect(ShowAllRotorCurvesAct, SIGNAL(triggered()), pDMS, SLOT(OnShowAllRotorCurves()));
	connect(HideAllRotorCurvesAct, SIGNAL(triggered()), pDMS, SLOT(OnHideAllRotorCurves()));
	connect(IsolateCurrentBladeCurveAct, SIGNAL(triggered()), pDMS, SLOT(OnIsolateCurrentBladeCurve()));
	connect(CompareCurrentBladeCurveAct, SIGNAL(triggered()), pDMS, SLOT(OnCompareIsolatedBladeCurves()));
	connect(DeleteCurrentBladeAct, SIGNAL(triggered()), pDMS, SLOT(OnDeleteBlade()));
	connect(DeleteCurrentTurbineAct, SIGNAL(triggered()), pDMS, SLOT(OnDeleteTurbine()));
	connect(DeleteCurrentRotorSimAct, SIGNAL(triggered()), pDMS, SLOT(OnDeleteRotorSim()));
	connect(DeleteCurrentTurbineSimAct, SIGNAL(triggered()), pDMS, SLOT(OnDeleteTurbineSim()));
	connect(ExportBladeGeomAct, SIGNAL(triggered()), pDMS, SLOT(OnExportBladeGeometry()));
	
	connect(EditCurrentTurbineAct, SIGNAL(triggered()), pDMS, SLOT(OnEditTurbine()));
	connect(EditCurrentBladeAct, SIGNAL(triggered()), pDMS, SLOT(OnEditBlade()));
	connect(BEMGraphDlg, SIGNAL(triggered()), pDMS, SLOT(OnGraphSettings()));
	connect(SingleGraphAct, SIGNAL(triggered()), pDMS, SLOT(OnSingleGraphs()));
	connect(HideWidgetsAct, SIGNAL(triggered()), pDMS, SLOT(OnHideWidgets()));
	connect(Edit360PolarAct, SIGNAL(triggered()), pDMS, SLOT(OnEditCur360Polar()));
    connect(DeleteCurrent360PolarAct, SIGNAL(triggered()), pDMS, SLOT(OnDelete360Polar()));
//	connect(LoadCylindricFoilAct, SIGNAL(triggered()), pDMS, SLOT(OnLoadCylindricFoil()));
	
	connect(MainWindAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharMainWind()));
	connect(MainRotAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharMainRot()));
	connect(MainPitchAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharMainPitch()));
	connect(ParamWindAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharParamWind()));
	connect(ParamRotAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharParamRot()));
	connect(ParamPitchAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharParamPitch()));
	connect(ParamNoneAct, SIGNAL(triggered()), pDMS, SLOT(OnSetCharParamNone()));
	connect(DeleteCurrentCharSimAct, SIGNAL(triggered()), pDMS, SLOT(OnDeleteCharSim()));
	
	// connection of DMS actions
	connect(AziGraphAct, SIGNAL(triggered()), pDMS, SLOT(OnAziGraph()));
}
/////////////////////end new code JW/////////////////////


void MainFrame::closeEvent (QCloseEvent * event)
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot close QBlade while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			event->ignore();
			return;}
	}
	
	if(!m_bSaved)
	{
		int resp = QMessageBox::question(this, tr("Exit"), tr("Save the project before exit ?"),
										 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
										 QMessageBox::Yes);
		if(resp == QMessageBox::Yes)
		{
			if(!SaveProject(m_FileName))
			{
				event->ignore();
				return;
			}
			AddRecentFile(m_FileName);
		}
		else if (resp==QMessageBox::Cancel)
		{
			event->ignore();
			return;
		}
	}
	DeleteProject();
	
	SaveSettings();
	event->accept();//continue closing
}

void MainFrame::CreateActions() {
	newProjectAct = new QAction(QIcon(":/images/new.png"), tr("New Project"), this);
	newProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	newProjectAct->setStatusTip(tr("Save and close the current project, create a new project"));
	connect(newProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));
	
	closeProjectAct = new QAction(QIcon(":/images/new.png"), tr("Close the Project"), this);
	closeProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	closeProjectAct->setStatusTip(tr("Save and close the current project"));
	connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));
	
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open Project..."), this);
	openAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(OnLoadFile()));
	
	OnAFoilAct = new QAction(QIcon(":/images/foil.png"), tr("&Airfoil Design"), this);
	OnAFoilAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	OnAFoilAct->setCheckable(true);
	OnAFoilAct->setStatusTip(tr("Open Foil Design application"));
	connect(OnAFoilAct, SIGNAL(triggered()), this, SLOT(OnAFoil()));
	
	OnXInverseAct = new QAction(QIcon(":/images/inverse.png"), tr("&XFOIL Inverse Analysis"), this);
	OnXInverseAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	OnXInverseAct->setCheckable(true);
	OnXInverseAct->setStatusTip(tr("Open XFoil inverse analysis application"));
	connect(OnXInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverse()));
	
	OnMixedInverseAct = new QAction(tr("&XFoil Mixed Inverse Design"), this);
	OnMixedInverseAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	OnMixedInverseAct->setStatusTip(tr("Open XFoil Mixed Inverse analysis application"));
	connect(OnMixedInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverseMixed()));
	
	OnXDirectAct = new QAction(QIcon(":/images/analysis.png"), tr("XFOIL Direct Analysis"), this);
	OnXDirectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	OnXDirectAct->setCheckable(true);
	OnXDirectAct->setStatusTip(tr("Open XFoil direct analysis application"));
	connect(OnXDirectAct, SIGNAL(triggered()), this, SLOT(OnXDirect()));
	
	saveAct = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
	saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	saveAct->setStatusTip(tr("Save the project to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(OnSaveProject()));
	
	saveProjectAsAct = new QAction(tr("Save Project As..."), this);
	saveProjectAsAct->setStatusTip(tr("Save the current project under a new name"));
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(OnSaveProjectAs()));
	
	unitsAct = new QAction(tr("Units..."), this);
	unitsAct->setStatusTip(tr("Define the units for this project"));
	connect(unitsAct, SIGNAL(triggered()), this, SLOT(OnUnits()));
	
	languageAct = new QAction(tr("Language..."), this);
	languageAct->setStatusTip(tr("Define the default language for the application"));
	connect(languageAct, SIGNAL(triggered()), this, SLOT(OnLanguage()));
	
	restoreToolbarsAct	 = new QAction(tr("Restore toolbars"), this);
	restoreToolbarsAct->setStatusTip(tr("Restores the toolbars to their original state"));
	connect(restoreToolbarsAct, SIGNAL(triggered()), this, SLOT(OnRestoreToolbars()));
	
	saveViewToImageFileAct = new QAction(tr("Save View to Image File"), this);
	saveViewToImageFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	saveViewToImageFileAct->setStatusTip(tr("Saves the current view to a file on disk"));
	connect(saveViewToImageFileAct, SIGNAL(triggered()), this, SLOT(OnSaveViewToImageFile()));
	
	
	resetSettingsAct = new QAction(tr("Reset Default Settings"), this);
	resetSettingsAct->setStatusTip(tr("will revert to default settings at the next session"));
	connect(resetSettingsAct, SIGNAL(triggered()), this, SLOT(OnResetSettings()));
	
	saveSettingsAct = new QAction(tr("Save Current Settings"), this);
	saveSettingsAct->setStatusTip(tr("will save the current settings"));
	connect(saveSettingsAct, SIGNAL(triggered()), this, SLOT(OnSaveSettings()));
	
	HAWTToolbarView = new QAction(QIcon(":/images/hawt.png"),tr("HAWT Mode"), this);
	HAWTToolbarView->setStatusTip(tr("Change the MainToolBar to HAWT Mode"));
	HAWTToolbarView->setCheckable(true);
	connect(HAWTToolbarView, SIGNAL(triggered()), this, SLOT(OnHAWTView()));
	
	VAWTToolbarView = new QAction(QIcon(":/images/vawt.png"),tr("VAWT Mode"), this);
	VAWTToolbarView->setStatusTip(tr("Change the MainToolBar to VAWT Mode"));
	VAWTToolbarView->setCheckable(true);
	connect(VAWTToolbarView, SIGNAL(triggered()), this, SLOT(OnVAWTView()));
	
	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
	
	styleAct = new QAction(tr("General Display Settings"), this);
	styleAct->setStatusTip(tr("Define the color and font options for all views and graphs"));
	connect(styleAct, SIGNAL(triggered()), this, SLOT(OnStyle()));
	
	
	exportCurGraphAct = new QAction(tr("Export Graph"), this);
	exportCurGraphAct->setStatusTip(tr("Export the current graph data to a text file"));
	connect(exportCurGraphAct, SIGNAL(triggered()), this, SLOT(OnExportCurGraph()));
	
	resetCurGraphScales = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales")+"\t(R)", this);
	resetCurGraphScales->setStatusTip(tr("Restores the graph's x and y scales"));
	connect(resetCurGraphScales, SIGNAL(triggered()), this, SLOT(OnResetCurGraphScales()));

    autoResetCurGraphScales = new QAction(tr("No Automatic Reset"), this);
    autoResetCurGraphScales->setStatusTip(tr("Disables the graphs auto-reset"));
    autoResetCurGraphScales->setCheckable(true);
    connect(autoResetCurGraphScales, SIGNAL(triggered()), this, SLOT(OnAutomaticResetCurGraph()));
	
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	guidelinesAct = new QAction(tr("&Guidelines"), this);
	guidelinesAct->setStatusTip(tr("Show the guidelines for some help"));
	connect(guidelinesAct, SIGNAL(triggered()), this, SLOT(OnGuidelines()));
	
	aboutAct = new QAction(tr("&About XFLR"), this);
	aboutAct->setStatusTip(tr("More information about XFLR5"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(AboutQFLR5()));
	
	aboutQtAct = new QAction(tr("About Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	ShowPolarProps = new QAction(tr("Properties"), this);
	ShowPolarProps->setStatusTip(tr("Show the properties of the currently selected polar"));
	ShowPolarProps->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Return));
	connect(ShowPolarProps, SIGNAL(triggered()), this, SLOT(OnPolarProps()));
	
	ShowWOppProps = new QAction(tr("Properties"), this);
	ShowWOppProps->setStatusTip(tr("Show the properties of the currently selected operating point"));
	ShowWOppProps->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_Return));
	connect(ShowWOppProps, SIGNAL(triggered()), this, SLOT(OnWOppProps()));
	
	
	CreateAFoilActions();
	CreateXDirectActions();
	CreateXInverseActions();
	
	AboutBEMAct = new QAction(tr("&About QBlade"), this);
	AboutBEMAct->setStatusTip(tr("More information about QBlade"));
	connect(AboutBEMAct, SIGNAL(triggered()), this, SLOT(AboutQBlade()));
	
	CreateBEMActions();
	
	AboutFASTAct = new QAction(tr("&About FAST and Aerodyn"), this);
	AboutFASTAct->setStatusTip(tr("More information about FAST and AeroDyn"));
	connect(AboutFASTAct, SIGNAL(triggered()), this, SLOT(AboutFASTAerodyn()));
	
	CreateDMSActions();
}


void MainFrame::CreateAFoilActions()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	
	AFoilGridAct= new QAction(tr("Grid Options"), this);
	AFoilGridAct->setStatusTip(tr("Define the grid settings for the view"));
	connect(AFoilGridAct, SIGNAL(triggered()), pAFoil, SLOT(OnGrid()));
	
	SplinesAct= new QAction(tr("Use Splines"), this);
	SplinesAct->setStatusTip(tr("Define a foil using one B-Spline for each foil side"));
	connect(SplinesAct, SIGNAL(triggered()), pAFoil, SLOT(OnSplines()));
	
	storeSplineAct= new QAction(QIcon(":/images/OnStoreFoil.png"), tr("Store Splines as Foil"), this);
	storeSplineAct->setStatusTip(tr("Store the current splines in the foil database"));
	connect(storeSplineAct, SIGNAL(triggered()), pAFoil, SLOT(OnStoreSplines()));
	
	splineControlsAct= new QAction(tr("Splines Params"), this);
	splineControlsAct->setStatusTip(tr("Define parameters for the splines : degree, number of out points"));
	connect(splineControlsAct, SIGNAL(triggered()), pAFoil, SLOT(OnSplineControls()));
	
	exportSplinesToFileAct= new QAction(tr("Export Splines To File"), this);
	exportSplinesToFileAct->setStatusTip(tr("Define parameters for the splines : degree, number of out points"));
	connect(exportSplinesToFileAct, SIGNAL(triggered()), pAFoil, SLOT(OnExportSplinesToFile()));
	
	newSplinesAct= new QAction(tr("New Splines"), this);
	newSplinesAct->setStatusTip(tr("Reset the splines"));
	connect(newSplinesAct, SIGNAL(triggered()), pAFoil, SLOT(OnNewSplines()));
	
	InsertSplinePt = new QAction(tr("Insert Control Point")+"\tShift+Click", this);
	connect(InsertSplinePt, SIGNAL(triggered()), pAFoil, SLOT(OnInsertCtrlPt()));
	
	RemoveSplinePt = new QAction(tr("Remove Control Point")+"\tCtrl+Click", this);
	connect(RemoveSplinePt, SIGNAL(triggered()), pAFoil, SLOT(OnRemoveCtrlPt()));
	
	ResetXScaleAct= new QAction(QIcon(":/images/OnResetXScale.png"), tr("Reset X Scale"), this);
	ResetXScaleAct->setStatusTip(tr("Resets the scale to fit the current screen width"));
	connect(ResetXScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetXScale()));
	
	ResetXYScaleAct= new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset Scales")+"\t(R)", this);
	ResetXYScaleAct->setStatusTip(tr("Resets the x and y scales to screen size"));
	connect(ResetXYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetScales()));
	
	
	UndoAFoilAct= new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
	UndoAFoilAct->setShortcut(Qt::CTRL + Qt::Key_Z);
	UndoAFoilAct->setStatusTip(tr("Cancels the last modifiction made to the splines"));
	connect(UndoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnUndo()));
	
	RedoAFoilAct= new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
	RedoAFoilAct->setShortcut(Qt::CTRL + Qt::Key_Y);
	RedoAFoilAct->setStatusTip(tr("Restores the last cancelled modifiction made to the splines"));
	connect(RedoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnRedo()));
	
	ShowAllFoils= new QAction(tr("Show All Foils"), this);
	connect(ShowAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnShowAllFoils()));
	HideAllFoils= new QAction(tr("Hide All Foils"), this);
	connect(HideAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnHideAllFoils()));
	
	
	AFoilDelete = new QAction(tr("Delete..."), this);
	connect(AFoilDelete, SIGNAL(triggered()), pAFoil, SLOT(OnDelete()));
	
	AFoilRename = new QAction(tr("Rename..."), this);
	AFoilRename->setShortcut(Qt::Key_F2);
	connect(AFoilRename, SIGNAL(triggered()), pAFoil, SLOT(OnRenameFoil()));
	
	AFoilExport = new QAction(tr("Export..."), this);
	connect(AFoilExport, SIGNAL(triggered()), pAFoil, SLOT(OnExportCurFoil()));
	
	AFoilImport = new QAction(tr("Import Foil"), this);
	connect(AFoilImport, SIGNAL(triggered()), pAFoil, SLOT(OnImportFoil()));
	
	ShowCurrentFoil= new QAction(tr("Show Current Foil"), this);
	connect(ShowCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnShowCurrentFoil()));
	
	HideCurrentFoil= new QAction(tr("Hide Current Foil"), this);
	connect(HideCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnHideCurrentFoil()));
	
	ResetYScaleAct= new QAction(tr("Reset Y Scale"), this);
	connect(ResetYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetYScale()));
	
	zoomInAct= new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
	zoomInAct->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));
	connect(zoomInAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomIn()));
	
	zoomLessAct= new QAction(QIcon(":/images/OnZoomLess.png"), tr("Zoom Less"), this);
	zoomLessAct->setStatusTip(tr("Zoom Less"));
	connect(zoomLessAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomLess()));
	
	zoomYAct= new QAction(QIcon(":/images/OnZoomYScale.png"), tr("Zoom Y Scale Only"), this);
	zoomYAct->setStatusTip(tr("Zoom Y scale Only"));
	connect(zoomYAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomYOnly()));
	
	AFoilDerotateFoil = new QAction(tr("De-rotate the Foil"), this);
	connect(AFoilDerotateFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilDerotateFoil()));
	
	AFoilNormalizeFoil = new QAction(tr("Normalize the Foil"), this);
	connect(AFoilNormalizeFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilNormalizeFoil()));
	
	AFoilRefineLocalFoil = new QAction(tr("Refine Locally"), this);
	connect(AFoilRefineLocalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilCadd()));
	
	AFoilRefineGlobalFoil = new QAction(tr("Refine Globally"), this);
	connect(AFoilRefineGlobalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilPanels()));
	
	AFoilEditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(AFoilEditCoordsFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilCoordinates()));
	
	AFoilScaleFoil = new QAction(tr("Scale camber and thickness"), this);
	connect(AFoilScaleFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilGeom()));
	
	AFoilSetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(AFoilSetTEGap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetTEGap()));
	
	AFoilSetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(AFoilSetLERadius, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetLERadius()));
	
	AFoilLECircle = new QAction(tr("Show LE Circle"), this);
	connect(AFoilLECircle, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilLECircle()));
	
	m_pShowLegend = new QAction(tr("Show Legend"), this);
	m_pShowLegend->setCheckable(true);
	connect(m_pShowLegend, SIGNAL(triggered()), pAFoil, SLOT(OnShowLegend()));
	
	AFoilSetFlap = new QAction(tr("Set Flap"), this);
	connect(AFoilSetFlap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetFlap()));
	
	AFoilInterpolateFoils = new QAction(tr("Interpolate Foils"), this);
	connect(AFoilInterpolateFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilInterpolateFoils()));
	
	AFoilNacaFoils = new QAction(tr("Naca Foils"), this);
	connect(AFoilNacaFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilNacaFoils()));
	
	AFoilTableColumns = new QAction(tr("Set Table Columns"), this);
	connect(AFoilTableColumns, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilTableColumns()));
	
	AFoilTableColumnWidths = new QAction(tr("Reset column widths"), this);
	connect(AFoilTableColumnWidths, SIGNAL(triggered()), pAFoil, SLOT(OnColumnWidths()));
	
	AFoilLoadImage = new QAction(tr("Load background image")   +"\tCtrl+Shift+I", this);
	connect(AFoilLoadImage, SIGNAL(triggered()), pAFoil, SLOT(OnLoadBackImage()));
	AFoilClearImage = new QAction(tr("Clear background image") +"\tCtrl+Shift+I", this);
	
	connect(AFoilClearImage, SIGNAL(triggered()), pAFoil, SLOT(OnClearBackImage()));
}

//
void MainFrame::CreateAFoilMenus()
{
	AFoilViewMenu = menuBar()->addMenu(tr("&View"));
	AFoilViewMenu->addAction(ShowCurrentFoil);
	AFoilViewMenu->addAction(HideCurrentFoil);
	AFoilViewMenu->addAction(ShowAllFoils);
	AFoilViewMenu->addAction(HideAllFoils);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(zoomInAct);
	AFoilViewMenu->addAction(zoomLessAct);
	AFoilViewMenu->addAction(ResetXScaleAct);
	AFoilViewMenu->addAction(ResetYScaleAct);
	AFoilViewMenu->addAction(ResetXYScaleAct);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(m_pShowLegend);
	AFoilViewMenu->addAction(AFoilLECircle);
	AFoilViewMenu->addAction(AFoilGridAct);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(AFoilLoadImage);
	AFoilViewMenu->addAction(AFoilClearImage);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(saveViewToImageFileAct);
	
	AFoilDesignMenu = menuBar()->addMenu(tr("F&oil"));
	AFoilDesignMenu->addAction(AFoilRename);
	AFoilDesignMenu->addAction(AFoilDelete);
	AFoilDesignMenu->addAction(AFoilImport);
	AFoilDesignMenu->addAction(AFoilExport);
	AFoilDesignMenu->addAction(DuplicateFoil);
	AFoilDesignMenu->addSeparator();
	AFoilDesignMenu->addAction(HideAllFoils);
	AFoilDesignMenu->addAction(ShowAllFoils);
	AFoilDesignMenu->addSeparator();
	AFoilDesignMenu->addAction(AFoilNormalizeFoil);
	AFoilDesignMenu->addAction(AFoilDerotateFoil);
	AFoilDesignMenu->addAction(AFoilRefineLocalFoil);
	AFoilDesignMenu->addAction(AFoilRefineGlobalFoil);
	AFoilDesignMenu->addAction(AFoilEditCoordsFoil);
	AFoilDesignMenu->addAction(AFoilScaleFoil);
	AFoilDesignMenu->addAction(AFoilSetTEGap);
	AFoilDesignMenu->addAction(AFoilSetLERadius);
	AFoilDesignMenu->addAction(AFoilSetFlap);
	AFoilDesignMenu->addSeparator();
	AFoilDesignMenu->addAction(AFoilInterpolateFoils);
	AFoilDesignMenu->addAction(LoadCylindricFoilAct);
	AFoilDesignMenu->addAction(AFoilNacaFoils);
	AFoilDesignMenu->addAction(ManageFoilsAct);
	
	
	AFoilSplineMenu = menuBar()->addMenu(tr("&Splines"));
	AFoilSplineMenu->addAction(InsertSplinePt);
	AFoilSplineMenu->addAction(RemoveSplinePt);
	AFoilSplineMenu->addSeparator();
	AFoilSplineMenu->addAction(UndoAFoilAct);
	AFoilSplineMenu->addAction(RedoAFoilAct);
	AFoilSplineMenu->addSeparator();
	AFoilSplineMenu->addAction(newSplinesAct);
	AFoilSplineMenu->addAction(splineControlsAct);
	AFoilSplineMenu->addAction(storeSplineAct);
	AFoilSplineMenu->addAction(exportSplinesToFileAct);
	
	//AFoil Context Menu
	AFoilCtxMenu = new QMenu(tr("Context Menu"),this);
	AFoilCtxMenu->addMenu(AFoilDesignMenu);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addMenu(AFoilSplineMenu);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(ShowAllFoils);
	AFoilCtxMenu->addAction(HideAllFoils);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(ResetXScaleAct);
	AFoilCtxMenu->addAction(ResetYScaleAct);
	AFoilCtxMenu->addAction(ResetXYScaleAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(m_pShowLegend);
	AFoilCtxMenu->addAction(AFoilLECircle);
	AFoilCtxMenu->addAction(AFoilGridAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(AFoilLoadImage);
	AFoilCtxMenu->addAction(AFoilClearImage);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(saveViewToImageFileAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(AFoilTableColumns);
	AFoilCtxMenu->addAction(AFoilTableColumnWidths);
	
	//Context menu to be displayed when user right clicks on a foil in the table
	AFoilTableCtxMenu = new QMenu(tr("Foil Actions"),this);
	AFoilTableCtxMenu->addAction(AFoilRename);
	AFoilTableCtxMenu->addAction(AFoilDelete);
	AFoilTableCtxMenu->addAction(AFoilExport);
	AFoilTableCtxMenu->addAction(DuplicateFoil);
	AFoilTableCtxMenu->addSeparator();
	AFoilTableCtxMenu->addAction(AFoilNormalizeFoil);
	AFoilTableCtxMenu->addAction(AFoilDerotateFoil);
	AFoilTableCtxMenu->addAction(AFoilRefineLocalFoil);
	AFoilTableCtxMenu->addAction(AFoilRefineGlobalFoil);
	AFoilTableCtxMenu->addAction(AFoilEditCoordsFoil);
	AFoilTableCtxMenu->addAction(AFoilScaleFoil);
	AFoilTableCtxMenu->addAction(AFoilSetTEGap);
	AFoilTableCtxMenu->addAction(AFoilSetLERadius);
	AFoilTableCtxMenu->addAction(AFoilSetFlap);
	AFoilTableCtxMenu->addSeparator();
	AFoilTableCtxMenu->addAction(AFoilTableColumns);
	AFoilTableCtxMenu->addAction(AFoilTableColumnWidths);
}


void MainFrame::CreateAFoilToolbar()
{
	m_pctrlZoomY = new QToolButton;
	m_pctrlZoomY->setDefaultAction(zoomYAct);
	m_pctrlZoomY->setCheckable(true);
	
	m_pctrlZoomIn = new QToolButton;
	m_pctrlZoomIn->setDefaultAction(zoomInAct);
	m_pctrlZoomIn->setCheckable(true);
	
	m_pctrlAFoilToolBar = addToolBar(tr("Foil"));
	m_pctrlAFoilToolBar->setObjectName("FoilToolbar");
	m_pctrlAFoilToolBar->addAction(OnXInverseAct);
	m_pctrlAFoilToolBar->addWidget(m_pctrlZoomIn);
	m_pctrlAFoilToolBar->addAction(zoomLessAct);
	m_pctrlAFoilToolBar->addAction(ResetXYScaleAct);
	m_pctrlAFoilToolBar->addAction(ResetXScaleAct);
	m_pctrlAFoilToolBar->addWidget(m_pctrlZoomY);
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(UndoAFoilAct);
	m_pctrlAFoilToolBar->addAction(RedoAFoilAct);
	
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(storeSplineAct);

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    m_pctrlAFoilToolBar->setIconSize(QSize(width*0.025,width*0.025));
}



void MainFrame::CreateDockWindows()
{
	ObjectPropsDlg::s_pMainFrame   = this;
	XFoilAnalysisDlg::s_pMainFrame = this;
	BatchDlg::s_pMainFrame         = this;
	BatchThreadDlg::s_pMainFrame   = this;
	GraphDlg::s_pMainFrame         = this;
	FoilPolarDlg::s_pMainFrame     = this;
	
	m_pctrlXDirectWidget = new QDockWidget("XDirect", this);
	m_pctrlXDirectWidget->setObjectName("XDirect");
	m_pctrlXDirectWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXDirectWidget);
	
	m_pctrlXInverseWidget = new QDockWidget("XInverse", this);
	m_pctrlXInverseWidget->setObjectName("XInverse");
	m_pctrlXInverseWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXInverseWidget);
	
	m_pctrlAFoilWidget = new QDockWidget("AFoil", this);
	m_pctrlAFoilWidget->setObjectName("AFoil");
	m_pctrlAFoilWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, m_pctrlAFoilWidget);
	
	m_twoDWidget = new TwoDWidget(this);
	m_glWidget = new GLWidget(this);
	m_glWidget->updateGL();  // this seems to initialize openGL which must be done before a project is loaded,
	// because e.g. WindField needs glGenLists to work properly
	
	////////////////////////////////////////new code DM//////////////////////////////////
	m_pctrlBEMWidget = new BEMDock("HAWT", this, 0);
	m_pctrlSimuWidget = new BEMSimDock("HAWT Simulation Parameters", this, 0);
	
	m_pctrlBEMWidget->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	m_pctrlSimuWidget->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	
	m_pctrlBEMWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	m_pctrlSimuWidget->setAllowedAreas(Qt::RightDockWidgetArea);
	
	
	
	QBEM *pBEM = (QBEM*)m_pBEM;
	m_twoDWidget->m_pBEM = pBEM;
	
	pBEM->m_pGLWidget = m_glWidget;
	pBEM->m_p2DWidget = m_twoDWidget;
	SimuWidget *pSimuWidget = (SimuWidget *) m_pSimuWidget;
	pBEM->m_pSimuWidget = pSimuWidget;
	//////////////////////////////////////end new code DM//////////////
	
	////////////////////////////////////////new code JW//////////////////////////////////
	
	//		m_pctrlDMSWidget = new QDockWidget("VAWT", this);
	//		m_pctrlDMSWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//		addDockWidget(Qt::LeftDockWidgetArea, m_pctrlDMSWidget);
	
	//      QSizePolicy szPolicyMinimum;
	//      szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	//      szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);
	//		m_pctrlDMSWidget->setSizePolicy(szPolicyMinimum);
	
	//		m_pctrlSimuWidgetDMS = new QDockWidget("VAWT Simulation Parameters", this);
	//		m_pctrlSimuWidgetDMS->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	//		addDockWidget(Qt::RightDockWidgetArea, m_pctrlSimuWidgetDMS);
	
	
	m_pctrlDMSWidget = new DMSDock("VAWT", this, 0);
	
	m_pctrlDMSWidget->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	
	m_pctrlDMSWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	
	//		m_pDMS = new QDMS(this);
	//      QDMS *pDMS = (QDMS*)m_pDMS;
	//      pDMS->SetupLayout();
	//      pDMS->Connect();
	//      pDMS->setAttribute(Qt::WA_DeleteOnClose, false);
	//		m_pctrlDMSWidget->setWidget(pDMS);
	//		m_pctrlDMSWidget->setVisible(false);
	
	
	m_pctrlSimuWidgetDMS = new DMSSimDock("VAWT Simulation Parameters", this, 0);
	
	m_pctrlSimuWidgetDMS->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	
	m_pctrlSimuWidgetDMS->setAllowedAreas(Qt::LeftDockWidgetArea);
	
	
	//		m_pSimuWidgetDMS = new SimuWidgetDMS(this);
	//      SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;
	//      pSimuWidgetDMS->setAttribute(Qt::WA_DeleteOnClose, false);
	//		pSimuWidgetDMS->m_pDMS = pDMS;
	//		m_pctrlSimuWidgetDMS->setWidget(pSimuWidgetDMS);
	//		m_pctrlSimuWidgetDMS->setVisible(false);
	
	QDMS *pDMS = (QDMS*)m_pDMS;
	m_twoDWidget->m_pDMS=pDMS;
	
	pDMS->m_pGLWidget = m_glWidget;
	pDMS->m_p2DWidget = m_twoDWidget;
	SimuWidgetDMS *pSimuWidgetDMS = (SimuWidgetDMS *) m_pSimuWidgetDMS;
	pDMS->m_pSimuWidgetDMS = pSimuWidgetDMS;
	////////////////////////////////////////end new code JW//////////////////////////////
	
	m_pXDirect = new QXDirect(this);
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXDirectWidget->setWidget(pXDirect);
	m_pctrlXDirectWidget->setVisible(false);
	//	m_pctrlXDirectWidget->setFloating(true);
	
	m_pXInverse = new QXInverse(this);
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	pXInverse->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXInverseWidget->setWidget(pXInverse);
	m_pctrlXInverseWidget->setVisible(false);
	//	m_pctrlXInverseWidget->setFloating(true);
	
	m_centralWidget = new QStackedWidget;
	m_centralWidget->addWidget(m_twoDWidget);
	m_centralWidget->addWidget(m_glWidget);
	
	setCentralWidget(m_centralWidget);
	
	m_pAFoil  = new QAFoil(this);
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlAFoilWidget->setWidget(pAFoil);
	m_pctrlAFoilWidget->setVisible(false);
	
	m_twoDWidget->m_pXDirect   = pXDirect;
	m_twoDWidget->m_pXInverse  = pXInverse;
	m_twoDWidget->m_pAFoil     = pAFoil;
	
	//////////////new code DM///////////
	pBEM->s_pXDirect = pXDirect;
	pBEM->s_pAFoil = pAFoil;
	/////////end new code DM////////////
	//////////////new code JW///////////
	pDMS->s_pXDirect = pXDirect;
	pDMS->s_pAFoil = pAFoil;
	/////////end new code JW////////////
	
	QSizePolicy sizepol;
	sizepol.setHorizontalPolicy(QSizePolicy::Expanding);
	sizepol.setVerticalPolicy(QSizePolicy::Expanding);
	m_twoDWidget->setSizePolicy(sizepol);
	
	pXDirect->m_pMainFrame             = this;
	pXDirect->m_p2DWidget              = m_twoDWidget;
	pXDirect->m_pCpGraph->m_pParent    = m_twoDWidget;
	pXDirect->m_pPolarGraph->m_pParent = m_twoDWidget;
	pXDirect->m_pTrGraph->m_pParent    = m_twoDWidget;
	pXDirect->m_pCzGraph->m_pParent    = m_twoDWidget;
	pXDirect->m_pCmGraph->m_pParent    = m_twoDWidget;
	pXDirect->m_pUserGraph->m_pParent  = m_twoDWidget;
	
	pAFoil->m_pMainFrame = this;
	pAFoil->m_p2DWidget  = m_twoDWidget;
	pAFoil->m_pXFoil     = pXDirect->m_pXFoil;
	
	pXInverse->m_pMainFrame       = this;
	pXInverse->m_pXFoil           = pXDirect->m_pXFoil;
	pXInverse->m_p2DWidget        = m_twoDWidget;
	
	XFoilAnalysisDlg::s_pXDirect = m_pXDirect;
	NacaFoilDlg::s_pXFoil         = pXDirect->m_pXFoil;
	InterpolateFoilsDlg::s_pXFoil = pXDirect->m_pXFoil;
	CAddDlg::s_pXFoil             = pXDirect->m_pXFoil;
	TwoDPanelDlg::s_pXFoil        = pXDirect->m_pXFoil;
	FoilGeomDlg::s_pXFoil         = pXDirect->m_pXFoil;
	TEGapDlg::s_pXFoil            = pXDirect->m_pXFoil;
	LEDlg::s_pXFoil               = pXDirect->m_pXFoil;
	BatchDlg::s_pXFoil            = pXDirect->m_pXFoil;
	BatchDlg::s_pXDirect          = m_pXDirect;
	BatchThreadDlg::s_pXFoil      = pXDirect->m_pXFoil;
	BatchThreadDlg::s_pXDirect    = m_pXDirect;
	FoilPolarDlg::s_pXDirect      = m_pXDirect;
	
	GraphDlg::s_ActivePage = 0;
}

void MainFrame::CreateMenus()
{
	// Create common File, View and Help menus
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveProjectAsAct);
	
	CreateBEMMenus();
	
	
	separatorAct = fileMenu->addSeparator();
	for (int i = 0; i < MAXRECENTFILES; ++i)
		fileMenu->addAction(recentFileActs[i]);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	updateRecentFileActions();
	
	optionsMenu = menuBar()->addMenu(tr("Options"));
	optionsMenu->addSeparator();
	optionsMenu->addAction(languageAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(unitsAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(styleAct);
	//	optionsMenu->addSeparator();
	//	optionsMenu->addAction(saveOptionsAct);
	//	optionsMenu->addSeparator();
	//	optionsMenu->addAction(restoreToolbarsAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(saveSettingsAct);
	optionsMenu->addAction(resetSettingsAct);
	
	helpMenu = menuBar()->addMenu(tr("&?"));
	//        helpMenu->addAction(guidelinesAct);
	helpMenu->addAction(AboutBEMAct);
	helpMenu->addAction("About PNoise", this, SLOT(onAboutPnoise()));
	helpMenu->addAction(AboutFASTAct);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
	
	
	//Create Application-Specific Menus
	CreateXDirectMenus();
	CreateXInverseMenus();
	CreateAFoilMenus();
}

///////////////new code DM/////////////
void MainFrame::CreateBEMToolbar() {
	g_qbem->m_BEMToolBar = new BEMToolbar (this);
	g_qdms->m_DMSToolBar = new DMSToolbar (this);    
}


void MainFrame::CreateBEMActions()
{
	QBEM *pBEM = (QBEM *) m_pBEM;
	
	OnBladeViewAct = new QAction(QIcon(":/images/blade.png"), tr("HAWT Rotorblade Design"), this);
	OnBladeViewAct->setCheckable(true);
	OnBladeViewAct->setStatusTip(tr("Design a Rotorblade"));
	connect(OnBladeViewAct, SIGNAL(triggered()), pBEM, SLOT(OnBladeView()));
	
	On360ViewAct = new QAction(QIcon(":/images/extra.png"), tr("Polar Extrapolation to 360"), this);
	On360ViewAct->setCheckable(true);
	On360ViewAct->setStatusTip(tr("Extrapolate an XFOIl Polar to 360 AoA"));
	connect(On360ViewAct, SIGNAL(triggered()), pBEM, SLOT(On360View()));
	
	OnRotorViewAct = new QAction(QIcon(":/images/rotor.png"), tr("Rotor BEM Simulation"), this);
	OnRotorViewAct->setCheckable(true);
	OnRotorViewAct->setStatusTip(tr("Simulate a Rotor"));
	connect(OnRotorViewAct, SIGNAL(triggered()), pBEM, SLOT(OnRotorsimView()));
	OnTurbineViewAct = new QAction(QIcon(":/images/WKA.png"), tr("Turbine BEM Simulation"), this);
	OnTurbineViewAct->setCheckable(true);
	OnTurbineViewAct->setStatusTip(tr("Define and simulate a Wind Turbine"));
	connect(OnTurbineViewAct, SIGNAL(triggered()), pBEM, SLOT(OnTurbineView()));
	
	OnCharacteristicViewAct = new QAction(QIcon(":/images/char.png"), tr("Multi Parameter BEM Simulation"), this);
	OnCharacteristicViewAct->setCheckable(true);
	OnCharacteristicViewAct->setStatusTip(tr("Vary Simulations over Pitch, Wind and Rotational Speed"));
	connect(OnCharacteristicViewAct, SIGNAL(triggered()), pBEM, SLOT(OnCharView()));
	
	RotorGraphAct = new QAction(tr("Set as Rotor Graph"), this);
	RotorGraphAct->setCheckable(true);
	RotorGraphAct->setStatusTip(tr("Set as Rotor Graph"));
	
	BladeGraphAct = new QAction(tr("Set as Blade Graph"), this);
	BladeGraphAct->setCheckable(true);
	BladeGraphAct->setStatusTip(tr("Set as Blade Graph"));
	
	WeibullGraphAct = new QAction(tr("Set as Weibull Graph"), this);
	WeibullGraphAct->setCheckable(true);
	WeibullGraphAct->setStatusTip(tr("Set as Weibull Graph"));
	
	ShowAllRotorCurvesAct = new QAction(tr("Show all Rotor Curves"), this);
	ShowAllRotorCurvesAct->setCheckable(false);
	ShowAllRotorCurvesAct->setStatusTip(tr("Show all Rotor Curves"));
	
	HideAllRotorCurvesAct = new QAction(tr("Hide other Rotor Curves"), this);
	HideAllRotorCurvesAct->setCheckable(false);
	HideAllRotorCurvesAct->setStatusTip(tr("Hide all Rotor Curves"));
	
	ShowAllPolarsAct = new QAction(tr("Show all 360 Polars"), this);
	ShowAllPolarsAct->setCheckable(false);
	ShowAllPolarsAct->setStatusTip(tr("Show all 360 Polars"));
	
	HideAllPolarsAct = new QAction(tr("Hide other 360 Polars"), this);
	HideAllPolarsAct->setCheckable(false);
	HideAllPolarsAct->setStatusTip(tr("Hide all 360 Polars"));
	
	IsolateCurrentBladeCurveAct = new QAction(tr("Isolate Blade Curve"), this);
	IsolateCurrentBladeCurveAct->setCheckable(true);
	IsolateCurrentBladeCurveAct->setStatusTip(tr("Isolates The Current Blade Curve"));
	
	CompareCurrentBladeCurveAct = new QAction(tr("Compare isolated Blade Curve"), this);
	CompareCurrentBladeCurveAct->setCheckable(true);
	CompareCurrentBladeCurveAct->setStatusTip(tr("Compares the Blade Curves of different Blades or Turbines"));
	
	DeleteCurrentBladeAct = new QAction(tr("Delete Current Blade/Rotor"), this);
	DeleteCurrentBladeAct->setCheckable(false);
	DeleteCurrentBladeAct->setStatusTip(tr("Deletes the currently selected Blade/Rotor"));
	
	/////////////// new code NM ///////////////
	ExportCurrentRotorAct = new QAction(tr("Export Current Blade and Polars to WT_Perf Format"), this);
	ExportCurrentRotorAct->setCheckable(false);
	ExportCurrentRotorAct->setStatusTip(tr("Exports the currently selected Rotor and all associated Polars to WT_Perf (*.wtp"));
	/////////// end new code NM ///////////////
	
	ExportCurrentRotorAeroDynAct = new QAction(tr("Export Current Blade and Polars to AeroDyn Format"), this);
	ExportCurrentRotorAeroDynAct->setCheckable(false);
	ExportCurrentRotorAeroDynAct->setStatusTip(tr("Exports the currently selected Rotor and all associated Polars to AeroDyn (*.ipt)"));
	
	connect(ExportCurrentRotorAeroDynAct, SIGNAL(triggered()), pBEM, SLOT(OnExportRotorToAeroDyn()));  // new code NM
	
	
	exportAll360PolarsNRELAct = new QAction(tr("Export all 360 Polars to AeroDyn (NREL) format"), this);
	connect(exportAll360PolarsNRELAct, SIGNAL(triggered()), pBEM, SLOT(OnExportAll360PolarsNREL()));
	
	DeleteCurrent360PolarAct = new QAction(tr("Delete Current 360 Polar"), this);
	DeleteCurrent360PolarAct->setCheckable(false);
	DeleteCurrent360PolarAct->setStatusTip(tr("Deletes the currently selected 360 Polar"));
    connect(DeleteCurrent360PolarAct, SIGNAL(triggered()), pBEM, SLOT(OnDelete360Polar()));

	
	DeleteCurrentTurbineAct = new QAction(tr("Delete Current Turbine"), this);
	DeleteCurrentTurbineAct->setCheckable(false);
	DeleteCurrentTurbineAct->setStatusTip(tr("Deletes the currently selected Turbine"));
	
	DeleteCurrentRotorSimAct = new QAction(tr("Delete Current Rotor Sim"), this);
	DeleteCurrentRotorSimAct->setCheckable(false);
	DeleteCurrentRotorSimAct->setStatusTip(tr("Deletes the currently selected Rotor Simulation"));
	
	DeleteCurrentTurbineSimAct = new QAction(tr("Delete Current Turbine Sim"), this);
	DeleteCurrentTurbineSimAct->setCheckable(false);
	DeleteCurrentTurbineSimAct->setStatusTip(tr("Deletes the currently selected Turbine Simulation"));
	
	LoadCylindricFoilAct = new QAction(tr("Generate a Circular Foil"), this);
	LoadCylindricFoilAct->setCheckable(false);
	LoadCylindricFoilAct->setStatusTip(tr("Loads a Circular Foil"));
    connect(LoadCylindricFoilAct, SIGNAL(triggered()), g_qbem, SLOT(OnLoadCylindricFoil()));

	
	ExportBladeGeomAct = new QAction(tr("Export 3D Blade geometry in STL or text format"), this);
	ExportBladeGeomAct->setCheckable(false);
	ExportBladeGeomAct->setStatusTip(tr("Export 3D Blade geometry in STL format"));
	
	OnImport360PolarAct = new QAction(tr("Import 360 Polar in plain text, NREL or XFOIL format"), this);
	OnImport360PolarAct->setCheckable(false);
	OnImport360PolarAct->setStatusTip(tr("Import 360 Polar in plain text, NREL or XFOIL format"));
	connect(OnImport360PolarAct, SIGNAL(triggered()), pBEM, SLOT(OnImport360Polar()));
	
	OnImportBladeGeometry = new QAction(tr("Import Blade geometry in QBlade, Aerodyn or WT_Perf format"), this);
	OnImportBladeGeometry->setCheckable(false);
	OnImportBladeGeometry->setStatusTip(tr("Import Blade geometry in QBlade, Aerodyn or WT_Perf format"));
	connect(OnImportBladeGeometry, SIGNAL(triggered()), pBEM, SLOT(OnImportBladeGeometry()));
	
	
	Export360PolarAct = new QAction(tr("Export 360 Polar into AeroDyn (NREL) Format"), this);
	Export360PolarAct->setCheckable(false);
	Export360PolarAct->setStatusTip(tr("Export 360 Polar into AeroDyn (NREL) Format"));
	
	ImportPolarAct = new QAction(tr("Import Polar in plain text, NREL or XFOIL format"), this);
	ImportPolarAct->setCheckable(false);
	ImportPolarAct->setStatusTip(tr("Import Polar in plain text, NREL or XFOIL format"));
	
	ExportBladeTableAct = new QAction(tr("Export Rotorblade Table"), this);
	ExportBladeTableAct->setCheckable(false);
	ExportBladeTableAct->setStatusTip(tr("Exports the Rotorblade to a Simple Text File"));
	
	Edit360PolarAct = new QAction(tr("Edit Current 360 Polar"), this);
	Edit360PolarAct->setCheckable(false);
	Edit360PolarAct->setStatusTip(tr("Edit the Current 360 Polar"));
    connect(Edit360PolarAct, SIGNAL(triggered()), pBEM, SLOT(OnEditCur360Polar()));

	
	EditCurrentTurbineAct = new QAction(tr("Edit current Turbine"), this);
	EditCurrentTurbineAct->setCheckable(false);
	EditCurrentTurbineAct->setStatusTip(tr("Edit current Turbine"));
	
	EditCurrentBladeAct = new QAction(tr("Edit current Blade"), this);
	EditCurrentBladeAct->setCheckable(true);
	EditCurrentBladeAct->setStatusTip(tr("Edit current Blade"));
	
	SingleGraphAct = new QAction(QIcon(":/images/single.png"), tr("Single Graph View"), this);
	SingleGraphAct->setCheckable(true);
	SingleGraphAct->setStatusTip(tr("Single Graph View"));
	
	HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
	HideWidgetsAct->setCheckable(true);
	HideWidgetsAct->setStatusTip(tr("Hide Widgets"));
	
	BEMGraphDlg = new QAction(tr("Define Graph Settings"), this);
	
	
	MainWindAct = new QAction(tr("Windspeed"), this);
	MainWindAct->setCheckable(true);
	MainWindAct->setStatusTip(tr("Main Variable: Windspeed"));
	
	
	MainRotAct = new QAction(tr("Rotational Speed"), this);
	MainRotAct->setCheckable(true);
	MainRotAct->setStatusTip(tr("Main Variable: Rotational Speed"));
	
	MainPitchAct = new QAction(tr("Pitch Angle"), this);
	MainPitchAct->setCheckable(true);
	MainPitchAct->setStatusTip(tr("Main Variable: Pitch Angle"));
	
	
	ParamWindAct = new QAction(tr("Windspeed"), this);
	ParamWindAct->setCheckable(true);
	ParamWindAct->setStatusTip(tr("Parameter: Windspeed"));
	
	ParamRotAct = new QAction(tr("Rotational Speed"), this);
	ParamRotAct->setCheckable(true);
	ParamRotAct->setStatusTip(tr("Parameter: Rotational Speed"));
	
	ParamPitchAct = new QAction(tr("Pitch Angle"), this);
	ParamPitchAct->setCheckable(true);
	ParamPitchAct->setStatusTip(tr("Parameter: Pitch Angle"));
	
	ParamNoneAct = new QAction(tr("No Parameter"), this);
	ParamNoneAct->setCheckable(true);
	ParamNoneAct->setStatusTip(tr("No Parameter"));
	
	
	DeleteCurrentCharSimAct = new QAction(tr("Delete Current Multi Parameter Simulation"), this);
	DeleteCurrentCharSimAct->setCheckable(false);
	DeleteCurrentCharSimAct->setStatusTip(tr("Deletes the currently selected Multi Parameter Simulation"));
	
	connect(ExportBladeGeomAct, SIGNAL(triggered()), pBEM, SLOT(OnExportBladeGeometry()));
	
	connect(Export360PolarAct, SIGNAL(triggered()), pBEM, SLOT(OnExport360PolarNREL()));
	connect(ImportPolarAct, SIGNAL(triggered()), pBEM, SLOT(OnImportPolar()));
	connect(ExportBladeTableAct, SIGNAL(triggered()), pBEM, SLOT(OnExportBladeTable()));
	
}

void MainFrame::CreateBEMMenus()
{
	//QBEM foil Context Menu
	BEMCtxMenu = new QMenu(tr("Context Menu"),this);
	currentBladeMenu2 = BEMCtxMenu->addMenu(tr("Current Rotor"));
	currentBladeMenu2->addAction(EditCurrentBladeAct);
	currentBladeMenu2->addAction(ExportBladeGeomAct);
	currentBladeMenu2->addSeparator();
	currentBladeMenu2->addAction(DeleteCurrentRotorSimAct);
	currentBladeMenu2->addAction(DeleteCurrentCharSimAct);
	currentBladeMenu2->addAction(DeleteCurrentBladeAct);
	BEMCtxMenu->addSeparator()->setText(tr("Current Graph"));
	BEMCtxMenu->addAction(resetCurGraphScales);
	BEMCtxMenu->addAction(exportCurGraphAct);
	BEMCtxMenu->addSeparator();
	BEMCtxMenu->addAction(ShowAllRotorCurvesAct);
	BEMCtxMenu->addAction(HideAllRotorCurvesAct);
	BEMCtxMenu->addSeparator();
	BEMCtxMenu->addAction(IsolateCurrentBladeCurveAct);
	BEMCtxMenu->addAction(CompareCurrentBladeCurveAct);
	BEMCtxMenu->addSeparator();
	BEMCtxMenu->addAction(RotorGraphAct);
	BEMCtxMenu->addAction(BladeGraphAct);
	BEMCtxMenu->addAction(AziGraphAct);// new code JW
	BEMCtxMenu->addSeparator();
	BEMCtxMenu->addAction(saveViewToImageFileAct);
	
	
	TurbineCtxMenu = new QMenu(tr("Context Menu"),this);
	
	currentTurbineMenu = TurbineCtxMenu->addMenu(tr("Current Turbine"));
	currentTurbineMenu->addAction(EditCurrentTurbineAct);
	currentTurbineMenu->addSeparator();
	currentTurbineMenu->addAction(DeleteCurrentTurbineSimAct);
	currentTurbineMenu->addAction(DeleteCurrentTurbineAct);
	
	
	TurbineCtxMenu->addSeparator()->setText(tr("Current Graph"));
	TurbineCtxMenu->addAction(resetCurGraphScales);
	TurbineCtxMenu->addAction(exportCurGraphAct);
	TurbineCtxMenu->addSeparator();
	TurbineCtxMenu->addAction(ShowAllRotorCurvesAct);
	TurbineCtxMenu->addAction(HideAllRotorCurvesAct);
	TurbineCtxMenu->addSeparator();
	TurbineCtxMenu->addAction(IsolateCurrentBladeCurveAct);
	TurbineCtxMenu->addAction(CompareCurrentBladeCurveAct);
	TurbineCtxMenu->addSeparator();
	TurbineCtxMenu->addAction(RotorGraphAct);
	TurbineCtxMenu->addAction(BladeGraphAct);
	TurbineCtxMenu->addAction(AziGraphAct);// new code JW
	TurbineCtxMenu->addAction(WeibullGraphAct);// new code JW
	TurbineCtxMenu->addSeparator();
	TurbineCtxMenu->addAction(saveViewToImageFileAct);
	
	
	
	PolarCtxMenu = new QMenu(tr("Context Menu"),this);
	
	
	current360PolarMenu = PolarCtxMenu->addMenu(tr("Current 360 Polar"));
	current360PolarMenu->addAction(Export360PolarAct);
	current360PolarMenu->addAction(DeleteCurrent360PolarAct);
	current360PolarMenu->addAction(Edit360PolarAct);
	
	PolarCtxMenu->addSeparator()->setText(tr("Current Graph"));
	PolarCtxMenu->addAction(resetCurGraphScales);
    PolarCtxMenu->addAction(autoResetCurGraphScales);
	PolarCtxMenu->addAction(exportCurGraphAct);
	PolarCtxMenu->addSeparator();
	PolarCtxMenu->addAction(ShowAllPolarsAct);
	PolarCtxMenu->addAction(HideAllPolarsAct);
	PolarCtxMenu->addSeparator();
	PolarCtxMenu->addAction(saveViewToImageFileAct);
	
	
	
	CharCtxMenu = new QMenu(tr("Context Menu"), this);
	currentBladeMenu = CharCtxMenu->addMenu(tr("Current Rotor"));
	currentBladeMenu->addAction(EditCurrentBladeAct);
	currentBladeMenu->addAction(ExportBladeGeomAct);
	currentBladeMenu->addAction(ExportCurrentRotorAct);  // new code NM	
	currentBladeMenu->addAction(ExportCurrentRotorAeroDynAct);  // new code NM
	currentBladeMenu->addSeparator();
	currentBladeMenu->addAction(DeleteCurrentRotorSimAct);
	currentBladeMenu->addAction(DeleteCurrentCharSimAct);
	currentBladeMenu->addAction(DeleteCurrentBladeAct);
	
	
	CharCtxMenu->addSeparator()->setText(tr("Current Graph"));
	CharCtxMenu->addAction(resetCurGraphScales);
	CharCtxMenu->addAction(exportCurGraphAct);
	
	CharCtxMenu->addSeparator()->setText(tr("Main Variable"));
	CharCtxMenu->addAction(MainWindAct);
	CharCtxMenu->addAction(MainRotAct);
	CharCtxMenu->addAction(MainPitchAct);
	
	CharCtxMenu->addSeparator()->setText(tr("Parameter"));
	CharCtxMenu->addAction(ParamNoneAct);
	CharCtxMenu->addAction(ParamWindAct);
	CharCtxMenu->addAction(ParamRotAct);
	CharCtxMenu->addAction(ParamPitchAct);
	
	CharCtxMenu->addSeparator();
	CharCtxMenu->addAction(saveViewToImageFileAct);
	
	
	
	//QBEM General Menu
	BEMViewMenu = menuBar()->addMenu(tr("&View"));
	BEMViewMenu->addSeparator();
	BEMViewMenu->addAction(OnAFoilAct);
	//	BEMViewMenu->addAction(OnXInverseAct);
	BEMViewMenu->addAction(OnXDirectAct);
	BEMViewMenu->addSeparator();
	BEMViewMenu->addAction(On360ViewAct);
	BEMViewMenu->addSeparator();
	BEMViewMenu->addAction(OnBladeViewAct);
	BEMViewMenu->addAction(OnRotorViewAct);
	BEMViewMenu->addAction(OnCharacteristicViewAct);
	BEMViewMenu->addAction(OnTurbineViewAct);
	//	BEMViewMenu->addSeparator();
	BEMViewMenu->addAction(OnBladeViewAct2);
	BEMViewMenu->addAction(OnRotorViewAct2);
	BEMViewMenu->addAction(OnCharacteristicViewAct2);
	BEMViewMenu->addAction(OnTurbineViewAct2);
    BEMViewMenu->addSeparator();
	
	//BEMViewMenu->addSeparator();
	//BEMViewMenu->addAction();// todo NM: insert windfield action
	//BEMViewMenu->addAction(unitsAct);
	//BEMViewMenu->addAction(styleAct);
	
	//////////////////// end JW modification ////////////////
	
	BEMBladeMenu = menuBar()->addMenu(tr("&Blade/Rotor"));
	BEMBladeMenu->addAction(OnImportBladeGeometry);
	BEMBladeMenu->addSeparator();
	BEMBladeMenu->addAction(ExportBladeGeomAct);
	BEMBladeMenu->addAction(ExportBladeTableAct);  // new code NM
	BEMBladeMenu->addAction(ExportCurrentRotorAct);  // new code NM
	BEMBladeMenu->addAction(ExportCurrentRotorAeroDynAct);  // new code NM
	BEMBladeMenu->addSeparator();
	BEMBladeMenu->addAction(DeleteCurrentBladeAct);
	BEMBladeMenu->addAction(DeleteCurrentRotorSimAct);
	BEMBladeMenu->addAction(DeleteCurrentCharSimAct);
	//BEMBladeMenu->addAction(saveViewToImageFileAct);// JW modification
	
	BEM360PolarMenu = menuBar()->addMenu(tr("360 &Polar"));
	current360PolarMenu = BEM360PolarMenu->addMenu(tr("Current 360 Polar"));
	current360PolarMenu->addAction(Export360PolarAct);
	current360PolarMenu->addAction(DeleteCurrent360PolarAct);
	current360PolarMenu->addAction(Edit360PolarAct);
	
	BEM360PolarMenu->addAction(OnImport360PolarAct);
	BEM360PolarMenu->addSeparator();
	BEM360PolarMenu->addAction(exportAll360PolarsNRELAct);
	BEM360PolarMenu->addSeparator();
	BEM360PolarMenu->addAction(LoadCylindricFoilAct);
	
	
	BEMTurbine = menuBar()->addMenu(tr("&Turbine"));
	BEMTurbine->addAction(DeleteCurrentTurbineAct);
	BEMTurbine->addAction(DeleteCurrentTurbineSimAct);
	
}
////////////end new code DM////////////////

////////////////////////new code JW///////////////////////////////
void MainFrame::ConnectBEMActions()
{
	QBEM *pBEM = (QBEM *) m_pBEM;
	
	connect(ShowAllPolarsAct, SIGNAL(triggered()), pBEM, SLOT(OnShowAllRotorCurves()));
	connect(HideAllPolarsAct, SIGNAL(triggered()), pBEM, SLOT(OnHideAllRotorCurves()));
	connect(RotorGraphAct, SIGNAL(triggered()), pBEM, SLOT(OnRotorGraph()));
	connect(BladeGraphAct, SIGNAL(triggered()), pBEM, SLOT(OnBladeGraph()));
	connect(WeibullGraphAct, SIGNAL(triggered()), pBEM, SLOT(OnWeibullGraph()));
	connect(ShowAllRotorCurvesAct, SIGNAL(triggered()), pBEM, SLOT(OnShowAllRotorCurves()));
	connect(HideAllRotorCurvesAct, SIGNAL(triggered()), pBEM, SLOT(OnHideAllRotorCurves()));
	connect(IsolateCurrentBladeCurveAct, SIGNAL(triggered()), pBEM, SLOT(OnIsolateCurrentBladeCurve()));
	connect(CompareCurrentBladeCurveAct, SIGNAL(triggered()), pBEM, SLOT(OnCompareIsolatedBladeCurves()));
	connect(DeleteCurrentBladeAct, SIGNAL(triggered()), pBEM, SLOT(OnDeleteBlade()));
	connect(ExportCurrentRotorAct, SIGNAL(triggered()), pBEM, SLOT(OnExportRotorToWT_Perf()));  // new code NM
	connect(DeleteCurrentTurbineAct, SIGNAL(triggered()), pBEM, SLOT(OnDeleteTurbine()));
	connect(DeleteCurrentRotorSimAct, SIGNAL(triggered()), pBEM, SLOT(OnDeleteRotorSim()));
	connect(DeleteCurrentTurbineSimAct, SIGNAL(triggered()), pBEM, SLOT(OnDeleteTurbineSim()));
//	connect(LoadCylindricFoilAct, SIGNAL(triggered()), pBEM, SLOT(OnLoadCylindricFoil()));
	connect(ExportBladeGeomAct, SIGNAL(triggered()), pBEM, SLOT(OnExportBladeGeometry()));
	
	connect(EditCurrentTurbineAct, SIGNAL(triggered()), pBEM, SLOT(OnEditTurbine()));
	connect(EditCurrentBladeAct, SIGNAL(triggered()), pBEM, SLOT(OnEditBlade()));
	connect(SingleGraphAct, SIGNAL(triggered()), pBEM, SLOT(OnSingleGraphs()));
	connect(HideWidgetsAct, SIGNAL(triggered()), pBEM, SLOT(OnHideWidgets()));
	
	connect(BEMGraphDlg, SIGNAL(triggered()), pBEM, SLOT(OnGraphSettings()));
	connect(MainWindAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharMainWind()));
	connect(MainRotAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharMainRot()));
	connect(MainPitchAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharMainPitch()));
	connect(ParamWindAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharParamWind()));
	connect(ParamRotAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharParamRot()));
	connect(ParamPitchAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharParamPitch()));
	connect(ParamNoneAct, SIGNAL(triggered()), pBEM, SLOT(OnSetCharParamNone()));
	connect(DeleteCurrentCharSimAct, SIGNAL(triggered()), pBEM, SLOT(OnDeleteCharSim()));
	
}

void MainFrame::DisconnectBEMActions()
{
	// delete connections of simultaneous BEM/DMS actions
	
	disconnect(RotorGraphAct, SIGNAL(triggered()), 0, 0);
	disconnect(BladeGraphAct, SIGNAL(triggered()), 0, 0);
	disconnect(WeibullGraphAct, SIGNAL(triggered()), 0, 0);
	disconnect(ShowAllRotorCurvesAct, SIGNAL(triggered()), 0, 0);
	disconnect(HideAllRotorCurvesAct, SIGNAL(triggered()), 0, 0);
	disconnect(IsolateCurrentBladeCurveAct, SIGNAL(triggered()), 0, 0);
	disconnect(CompareCurrentBladeCurveAct, SIGNAL(triggered()), 0, 0);
	disconnect(DeleteCurrentBladeAct, SIGNAL(triggered()), 0, 0);
	disconnect(ExportCurrentRotorAct, SIGNAL(triggered()), 0, 0);  // new code NM
	disconnect(DeleteCurrentTurbineAct, SIGNAL(triggered()), 0, 0);
	disconnect(DeleteCurrentRotorSimAct, SIGNAL(triggered()), 0, 0);
	disconnect(DeleteCurrentTurbineSimAct, SIGNAL(triggered()), 0, 0);
	disconnect(ExportBladeGeomAct, SIGNAL(triggered()), 0, 0);
	disconnect(EditCurrentTurbineAct, SIGNAL(triggered()), 0, 0);
	disconnect(EditCurrentBladeAct, SIGNAL(triggered()), 0, 0);
	disconnect(BEMGraphDlg, SIGNAL(triggered()), 0, 0);
	disconnect(SingleGraphAct, SIGNAL(triggered()), 0, 0);
	disconnect(HideWidgetsAct, SIGNAL(triggered()), 0, 0);
	disconnect(MainWindAct, SIGNAL(triggered()), 0,0);
	disconnect(MainRotAct, SIGNAL(triggered()), 0,0);
	disconnect(MainPitchAct, SIGNAL(triggered()), 0,0);
	disconnect(ParamWindAct, SIGNAL(triggered()), 0,0);
	disconnect(ParamRotAct, SIGNAL(triggered()), 0,0);
	disconnect(ParamPitchAct, SIGNAL(triggered()), 0,0);
	disconnect(ParamNoneAct, SIGNAL(triggered()), 0,0);
	disconnect(DeleteCurrentCharSimAct, SIGNAL(triggered()), 0,0);

}
////////////////////////end new code JW///////////////////////////////

void MainFrame::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
	m_pctrlProjectName = new QLabel(" ");
	m_pctrlProjectName->setMinimumWidth(200);
	statusBar()->addPermanentWidget(m_pctrlProjectName);
}

void MainFrame::CreateMainToolbar() {
	m_pctrlMainToolBar = addToolBar(tr("MainToolBar"));
	m_pctrlMainToolBar->setObjectName("MainToolbar");
	m_pctrlMainToolBar->addAction(newProjectAct);
	m_pctrlMainToolBar->addAction(openAct);
	m_pctrlMainToolBar->addAction(saveAct);
	m_pctrlMainToolBar->addSeparator();
	m_pctrlMainToolBar->addAction(HAWTToolbarView);
	m_pctrlMainToolBar->addAction(VAWTToolbarView);
	m_pctrlMainToolBar->addSeparator();
	m_pctrlMainToolBar->addAction(OnAFoilAct);
	//	m_pctrlMainToolBar->addAction(OnXInverseAct);
	m_pctrlMainToolBar->addAction(OnXDirectAct);
	m_pctrlMainToolBar->addAction(On360ViewAct);
    m_pctrlMainToolBar->addSeparator();
    g_noiseModule = new NoiseModule (this, m_pctrlMainToolBar);
	m_pctrlMainToolBar->addSeparator();
	m_pctrlMainToolBar->addAction(OnBladeViewAct);
	m_pctrlMainToolBar->addAction(OnRotorViewAct);
	m_pctrlMainToolBar->addAction(OnCharacteristicViewAct);
	m_pctrlMainToolBar->addAction(OnTurbineViewAct);
	m_pctrlMainToolBar->addAction(OnBladeViewAct2);
	m_pctrlMainToolBar->addAction(OnRotorViewAct2);
	m_pctrlMainToolBar->addAction(OnCharacteristicViewAct2);
	m_pctrlMainToolBar->addAction(OnTurbineViewAct2);
	
	QRect rec = QApplication::desktop()->screenGeometry();
	int width = rec.width();
	m_pctrlMainToolBar->setIconSize(QSize(width*0.025,width*0.025));
	
}


void MainFrame::CreateToolbars() {
	CreateMainToolbar();
	addToolBarBreak(Qt::TopToolBarArea);
	
	CreateXDirectToolbar();
	CreateXInverseToolbar();
	CreateAFoilToolbar();
	CreateBEMToolbar();
}


void MainFrame::CreateXDirectToolbar()
{
	m_pctrlPolarView = new QToolButton;
	m_pctrlPolarView->setDefaultAction(PolarsAct);
	m_pctrlPolarView->setCheckable(true);
	m_pctrlOppView = new QToolButton;
	m_pctrlOppView->setDefaultAction(OpPointsAct);
	m_pctrlOppView->setCheckable(true);
	
	setIconSize(QSize(36,36));
	
	
	QGroupBox *FoilBox = new QGroupBox (tr("Airfoils"));
	QVBoxLayout *vBox = new QVBoxLayout ();
	FoilBox->setLayout(vBox);
	m_pctrlFoil = new FoilComboBox (&g_foilStore);
	m_pctrlFoil->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlFoil->setMinimumWidth(170);
	vBox->addWidget(m_pctrlFoil);
	QGroupBox *PolarBox = new QGroupBox (tr("Polars"));
	vBox = new QVBoxLayout ();
	PolarBox->setLayout(vBox);
	m_pctrlPolar = new PolarComboBox (&g_polarStore);
	m_pctrlPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlPolar->setMinimumWidth(170);
	m_pctrlPolar->setParentBox(m_pctrlFoil);
	vBox->addWidget(m_pctrlPolar);
	QGroupBox *OpPointBox = new QGroupBox (tr("Operational Points"));
	vBox = new QVBoxLayout ();
	OpPointBox->setLayout(vBox);
	m_pctrlOpPoint = new OpPointComboBox (&g_oppointStore);
	m_pctrlOpPoint->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlOpPoint->setMinimumWidth(170);
	m_pctrlOpPoint->setParentBox(m_pctrlPolar);
	vBox->addWidget(m_pctrlOpPoint);
	
	m_pctrlXDirectToolBar = addToolBar(tr("Foil"));
	m_pctrlXDirectToolBar->setObjectName("XDirectToolbar");
	m_pctrlXDirectToolBar->addWidget(m_pctrlOppView);
	m_pctrlXDirectToolBar->addWidget(m_pctrlPolarView);
	m_pctrlXDirectToolBar->addSeparator();
	m_pctrlXDirectToolBar->addWidget(FoilBox);
	m_pctrlXDirectToolBar->addWidget(PolarBox);
	m_pctrlXDirectToolBar->addWidget(OpPointBox);

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    m_pctrlXDirectToolBar->setIconSize(QSize(width*0.025,width*0.025));
	
	connect(m_pctrlFoil,    SIGNAL(activated(int)), this, SLOT(OnSelChangeFoil(int)));
	connect(m_pctrlPolar,   SIGNAL(activated(int)), this, SLOT(OnSelChangePolar(int)));
	connect(m_pctrlOpPoint, SIGNAL(activated(int)), this, SLOT(OnSelChangeOpp(int)));
}



void MainFrame::CreateXDirectActions()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	OpPointsAct = new QAction(QIcon(":/images/OnOpPointView.png"), tr("OpPoint view")+"\tF5", this);
	OpPointsAct->setCheckable(true);
	OpPointsAct->setStatusTip(tr("Show Operating point view"));
	connect(OpPointsAct, SIGNAL(triggered()), pXDirect, SLOT(OnOpPoints()));
	
	PolarsAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar view")+"\tF8", this);
	PolarsAct->setCheckable(true);
	PolarsAct->setStatusTip(tr("Show Polar view"));
	connect(PolarsAct, SIGNAL(triggered()), pXDirect, SLOT(OnPolars()));
	
	defineCpGraphSettings = new QAction(tr("Define Cp Graph Settings")+"\t(G)", this);
	connect(defineCpGraphSettings, SIGNAL(triggered()), pXDirect, SLOT(OnCpGraphSettings()));
	
	XDirectPolarFilter = new QAction(tr("Polar Filter"), this);
	connect(XDirectPolarFilter, SIGNAL(triggered()), pXDirect, SLOT(OnPolarFilter()));
	
	allPolarGraphsSettingsAct = new QAction(tr("All Polar Graph Settings"), this);
	allPolarGraphsSettingsAct->setStatusTip("Modifies the setting for all polar graphs simultaneously");
	connect(allPolarGraphsSettingsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphsSetting()));
	
	allPolarGraphsScales = new QAction(tr("Reset All Polar Graph Scales"), this);
	connect(allPolarGraphsScales, SIGNAL(triggered()), pXDirect, SLOT(OnResetAllPolarGraphsScales()));
	
	resetGraphLegend = new QAction(tr("Reset Legend Position"), this);
	connect(resetGraphLegend, SIGNAL(triggered()), pXDirect, SLOT(OnResetGraphLegend()));
	
	TwoPolarGraphsAct = new QAction(tr("Two Polar Graphs")+"\t(T)", this);
	TwoPolarGraphsAct->setCheckable(true);
	connect(TwoPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnCouplePolarGraphs()));
	
	AllPolarGraphsAct = new QAction(tr("All Polar Graphs")+"\t(A)", this);
	AllPolarGraphsAct->setCheckable(true);
	connect(AllPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphs()));
	
	for (int i = 0; i < 5; ++i)
	{
		PolarGraphAct[i] = new QAction(this);
		PolarGraphAct[i]->setData(i);
		PolarGraphAct[i]->setCheckable(true);
		connect(PolarGraphAct[i], SIGNAL(triggered()), pXDirect, SLOT(OnSinglePolarGraph()));
	}
	PolarGraphAct[0]->setText(tr("Cl vs. Cd") +"\t(1)");
	PolarGraphAct[1]->setText(tr("Cl vs.Alpha")+ "\t(2)");
	PolarGraphAct[2]->setText(tr("Cl vs. Xtr.")+ "\t(3)");
	PolarGraphAct[3]->setText(tr("Cm vs.Alpha")+ "\t(4)");
	PolarGraphAct[4]->setText(tr("Glide ratio vs. alpha")+ "\t(5)");
	
	highlightOppAct	 = new QAction(tr("Highlight Current OpPoint")+"\t(Ctrl+H)", this);
	highlightOppAct->setCheckable(true);
	highlightOppAct->setStatusTip(tr("Highlights on the polar curve the currently selected operating point"));
	connect(highlightOppAct, SIGNAL(triggered()), pXDirect, SLOT(OnHighlightOpp()));
	
	
	deleteCurFoil = new QAction(tr("Delete..."), this);
	connect(deleteCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteCurFoil()));
	
	renameCurFoil = new QAction(tr("Rename...")+"\tF2", this);
	connect(renameCurFoil, SIGNAL(triggered()), this, SLOT(OnRenameCurFoil()));
	
	exportCurFoil = new QAction(tr("Export..."), this);
	connect(exportCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurFoil()));
	
	setCurFoilStyle = new QAction(tr("Set Style..."), this);
	connect(setCurFoilStyle, SIGNAL(triggered()), this, SLOT(OnCurFoilStyle()));
	
	deleteFoilPolars = new QAction(tr("Delete associated polars"), this);
	deleteFoilPolars->setStatusTip(tr("Delete all the polars associated to this foil"));
	connect(deleteFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilPolars()));
	
	showFoilPolars = new QAction(tr("Show associated polars"), this);
	connect(showFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilPolars()));
	
	hideFoilPolars = new QAction(tr("Hide associated polars"), this);
	connect(hideFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnHideFoilPolars()));
	
	hidePolarOpps = new QAction(tr("Hide associated OpPoints"), this);
	connect(hidePolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnHidePolarOpps()));
	
	showPolarOpps = new QAction(tr("Show associated OpPoints"), this);
	connect(showPolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnShowPolarOpps()));
	
	deletePolarOpps = new QAction(tr("Delete associated OpPoints"), this);
	connect(deletePolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnDeletePolarOpps()));
	
	hideFoilOpps = new QAction(tr("Hide associated OpPoints"), this);
	connect(hideFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnHideFoilOpps()));
	
	showFoilOpps = new QAction(tr("Show associated OpPoints"), this);
	connect(showFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilOpps()));
	
	deleteFoilOpps = new QAction(tr("Delete associated OpPoints"), this);
	connect(deleteFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilOpps()));
	
	definePolar = new QAction(tr("Define an Analysis")+"\tF6", this);
	definePolar->setStatusTip(tr("Defines a single analysis/polar"));
	connect(definePolar, SIGNAL(triggered()), pXDirect, SLOT(OnNewPolar()));
	
	defineBatch = new QAction(tr("Batch Analysis")+"\tShift+F6", this);
	defineBatch->setStatusTip(tr("Launches a batch of analysis calculation for a specified range or list of Reynolds numbers"));
	connect(defineBatch, SIGNAL(triggered()), pXDirect, SLOT(OnBatchAnalysis()));
	
	MultiThreadedBatchAct = new QAction(tr("Multi-threaded Batch Analysis")+"\tCtrl+F6", this);
	MultiThreadedBatchAct->setStatusTip(tr("Launches a batch of analysis calculation using all available computer CPU cores"));
	connect(MultiThreadedBatchAct, SIGNAL(triggered()), pXDirect, SLOT(OnMultiThreadedBatchAnalysis()));
	
	deletePolar = new QAction(tr("Delete"), this);
	deletePolar->setStatusTip(tr("Deletes the currently selected polar"));
	connect(deletePolar, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteCurPolar()));
	
	resetCurPolar = new QAction(tr("Reset"), this);
	resetCurPolar->setStatusTip(tr("Deletes the contents of the currently selected polar"));
	connect(resetCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnResetCurPolar()));
	
    editCurPolar = new QAction(tr("Edit Points"), this);
	editCurPolar->setStatusTip(tr("Remove the unconverged or erroneaous points of the currently selected polar"));
	connect(editCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnEditCurPolar()));
	
	exportCurPolar = new QAction(tr("Export Polar to XFOIL format"), this);
	connect(exportCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurPolar()));
	
	exportAllPolars = new QAction(tr("Export all Polars to XFOIL format"), this);
	connect(exportAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnExportAllPolars()));
	
	exportAllPolarsNRELAct = new QAction(tr("Export all Polars to AeroDyn (NREL) format"), this);
	connect(exportAllPolarsNRELAct, SIGNAL(triggered()), pXDirect, SLOT(OnExportAllPolarsNREL()));
	
	ExportPolarAct = new QAction(tr("Export Polar to AeroDyn (NREL) Format"), this);
	ExportPolarAct->setCheckable(false);
	ExportPolarAct->setStatusTip(tr("Export Polar to AeroDyn (NREL) Format"));
	connect(ExportPolarAct, SIGNAL(triggered()), pXDirect, SLOT(OnExportPolarNREL()));
	
	XDirectStyleAct = new QAction(tr("Define Styles"), this);
	XDirectStyleAct->setStatusTip(tr("Define the style for the boundary layer and the pressure arrows"));
	connect(XDirectStyleAct, SIGNAL(triggered()), pXDirect, SLOT(OnXDirectStyle()));
	
	ManageFoilsAct = new QAction(tr("Manage Foils"), this);
	ManageFoilsAct->setShortcut(Qt::Key_F7);
	connect(ManageFoilsAct, SIGNAL(triggered()), pXDirect, SLOT(OnManageFoils()));
	
	RenamePolarAct = new QAction(tr("Rename"), this);
	connect(RenamePolarAct, SIGNAL(triggered()), pXDirect, SLOT(OnRenamePolar()));
	
	showPanels = new QAction(tr("Show Panels"), this);
	showPanels->setCheckable(true);
	showPanels->setStatusTip(tr("Show the foil's panels"));
	connect(showPanels, SIGNAL(triggered()), pXDirect, SLOT(OnShowPanels()));
	
	resetFoilScale = new QAction(tr("Reset Foil Scale"), this);
	resetFoilScale->setStatusTip(tr("Resets the foil's scale to original size"));
	connect(resetFoilScale, SIGNAL(triggered()), pXDirect, SLOT(OnResetFoilScale()));
	
	showInviscidCurve = new QAction(tr("Show Inviscid Curve"), this);
	showInviscidCurve->setCheckable(true);
	showInviscidCurve->setStatusTip(tr("Display the Opp's inviscid curve"));
	connect(showInviscidCurve, SIGNAL(triggered()), pXDirect, SLOT(OnCpi()));
	
	showNeutralLine = new QAction(tr("Neutral Line"), this);
	showNeutralLine->setCheckable(true);
	connect(showNeutralLine, SIGNAL(triggered()), pXDirect, SLOT(OnShowNeutralLine()));
	
	showAllPolars = new QAction(tr("Show All Polars"), this);
	connect(showAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnShowAllPolars()));
	
	hideAllPolars = new QAction(tr("Hide Other Polars"), this);
	connect(hideAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnHideAllPolars()));
	
	showCurOppOnly = new QAction(tr("Show Current Opp Only"), this);
	showCurOppOnly->setCheckable(true);
	connect(showCurOppOnly, SIGNAL(triggered()), pXDirect, SLOT(OnCurOppOnly()));
	
	showAllOpPoints = new QAction(tr("Show All Opps"), this);
	showAllOpPoints->setCheckable(true);
	connect(showAllOpPoints, SIGNAL(triggered()), pXDirect, SLOT(OnShowAllOpps()));
	
	hideAllOpPoints = new QAction(tr("Hide Other Opps"), this);
	connect(hideAllOpPoints, SIGNAL(triggered()), pXDirect, SLOT(OnHideAllOpps()));
	
	exportCurOpp = new QAction(tr("Export"), this);
	connect(exportCurOpp, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurOpp()));
	
	deleteCurOpp = new QAction(tr("Delete"), this);
	connect(deleteCurOpp, SIGNAL(triggered()), pXDirect, SLOT(OnDelCurOpp()));
	
	resetXFoil = new QAction(tr("Reset XFoil"), this);
	
	
	
	connect(resetXFoil, SIGNAL(triggered()), pXDirect, SLOT(OnResetXFoil()));
	
	viewXFoilAdvanced = new QAction(tr("XFoil Advanced Settings"), this);
	defineBatch->setStatusTip(tr("Tip : you don't want to use that option..."));
	connect(viewXFoilAdvanced, SIGNAL(triggered()), pXDirect, SLOT(OnXFoilAdvanced()));
	
	viewLogFile = new QAction(tr("View Log File")+"\t(L)", this);
	connect(viewLogFile, SIGNAL(triggered()), this, SLOT(OnLogFile()));
	
	DerotateFoil = new QAction(tr("De-rotate the Foil"), this);
	connect(DerotateFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDerotateFoil()));
	
	NormalizeFoil = new QAction(tr("Normalize the Foil"), this);
	connect(NormalizeFoil, SIGNAL(triggered()), pXDirect, SLOT(OnNormalizeFoil()));
	
	RefineLocalFoil = new QAction(tr("Refine Locally"), this);
	connect(RefineLocalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnCadd()));
	
	RefineGlobalFoil = new QAction(tr("Refine Globally"), this);
	connect(RefineGlobalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnPanels()));
	
	EditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(EditCoordsFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilCoordinates()));
	
	ScaleFoil = new QAction(tr("Scale camber and thickness"), this);
	connect(ScaleFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilGeom()));
	
	SetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(SetTEGap, SIGNAL(triggered()), pXDirect, SLOT(OnSetTEGap()));
	
	SetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(SetLERadius, SIGNAL(triggered()), pXDirect, SLOT(OnSetLERadius()));
	
	SetFlap = new QAction(tr("Set Flap"), this);
	connect(SetFlap, SIGNAL(triggered()), pXDirect, SLOT(OnSetFlap()));
	
	InterpolateFoils = new QAction(tr("Interpolate Foils"), this);
	connect(InterpolateFoils, SIGNAL(triggered()), pXDirect, SLOT(OnInterpolateFoils()));
	
	NacaFoils = new QAction(tr("Naca Foils"), this);
	connect(NacaFoils, SIGNAL(triggered()), pXDirect, SLOT(OnNacaFoils()));
	
	DuplicateFoil = new QAction(tr("Duplicate"), this);
	connect(DuplicateFoil, SIGNAL(triggered()), this, SLOT(OnDuplicateFoil()));
	
	setCpVarGraph = new QAction(tr("Cp Variable"), this);
	setCpVarGraph->setCheckable(true);
	setCpVarGraph->setStatusTip(tr("Sets Cp vs. chord graph"));
	connect(setCpVarGraph, SIGNAL(triggered()), pXDirect, SLOT(OnCpGraph()));
	
	setQVarGraph = new QAction(tr("Q Variable"), this);
	setQVarGraph->setCheckable(true);
	
	setQVarGraph->setStatusTip(tr("Sets Speed vs. chord graph"));
	connect(setQVarGraph, SIGNAL(triggered()), pXDirect, SLOT(OnQGraph()));
	
	CurXFoilResExport = new QAction(tr("Export Cur. XFoil Results"), this);
	CurXFoilResExport->setStatusTip(tr("Sets Speed vs. chord graph"));
	connect(CurXFoilResExport, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurXFoilResults()));
	
	CurXFoilCtPlot = new QAction(tr("Max. Shear Coefficient"), this);
	CurXFoilCtPlot->setCheckable(true);
	connect(CurXFoilCtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCtPlot()));
	
	CurXFoilDbPlot = new QAction(tr("Bottom Side D* and Theta"), this);
	CurXFoilDbPlot->setCheckable(true);
	connect(CurXFoilDbPlot, SIGNAL(triggered()), pXDirect, SLOT(OnDbPlot()));
	
	CurXFoilDtPlot = new QAction(tr("Top Side D* and Theta"), this);
	CurXFoilDtPlot->setCheckable(true);
	connect(CurXFoilDtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnDtPlot()));
	
	CurXFoilRtLPlot = new QAction(tr("Log(Re_Theta)"), this);
	CurXFoilRtLPlot->setCheckable(true);
	connect(CurXFoilRtLPlot, SIGNAL(triggered()), pXDirect, SLOT(OnRtLPlot()));
	
	CurXFoilRtPlot = new QAction(tr("Re_Theta"), this);
	CurXFoilRtPlot->setCheckable(true);
	connect(CurXFoilRtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnRtPlot()));
	
	CurXFoilNPlot = new QAction(tr("Amplification Ratio"), this);
	CurXFoilNPlot->setCheckable(true);
	connect(CurXFoilNPlot, SIGNAL(triggered()), pXDirect, SLOT(OnNPlot()));
	
	CurXFoilCdPlot = new QAction(tr("Dissipation Coefficient"), this);
	CurXFoilCdPlot->setCheckable(true);
	connect(CurXFoilCdPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCdPlot()));
	
	CurXFoilCfPlot = new QAction(tr("Skin Friction Coefficient"), this);
	CurXFoilCfPlot->setCheckable(true);
	connect(CurXFoilCfPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCfPlot()));
	
	CurXFoilUePlot = new QAction(tr("Edge Velocity"), this);
	CurXFoilUePlot->setCheckable(true);
	connect(CurXFoilUePlot, SIGNAL(triggered()), pXDirect, SLOT(OnUePlot()));
	
	CurXFoilHPlot = new QAction(tr("Kinematic Shape Parameter"), this);
	CurXFoilHPlot->setCheckable(true);
	connect(CurXFoilHPlot, SIGNAL(triggered()), pXDirect, SLOT(OnHPlot()));
	
	m_pImportXFoilPolar = new QAction(tr("Import XFoil Polar"), this);
	connect(m_pImportXFoilPolar, SIGNAL(triggered()), pXDirect, SLOT(OnImportXFoilPolar()));
}


void MainFrame::CreateXDirectMenus()
{
	//MainMenu for XDirect Application
	XDirectViewMenu = menuBar()->addMenu(tr("&View"));
	XDirectViewMenu->addAction(OpPointsAct);
	XDirectViewMenu->addAction(PolarsAct);
	XDirectViewMenu->addSeparator();
	XDirectViewMenu->addAction(saveViewToImageFileAct);
	
	FoilMenu = menuBar()->addMenu(tr("&Foil"));
	FoilMenu->addAction(ManageFoilsAct);
	FoilMenu->addSeparator();
	currentFoilMenu = FoilMenu->addMenu(tr("Current Foil"));
	currentFoilMenu->addAction(setCurFoilStyle);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(exportCurFoil);
	currentFoilMenu->addAction(renameCurFoil);
	currentFoilMenu->addAction(deleteCurFoil);
	currentFoilMenu->addAction(DuplicateFoil);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(showFoilPolars);
	currentFoilMenu->addAction(hideFoilPolars);
	currentFoilMenu->addAction(deleteFoilPolars);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(showFoilOpps);
	currentFoilMenu->addAction(hideFoilOpps);
	currentFoilMenu->addAction(deleteFoilOpps);
	FoilMenu->addSeparator();
	FoilMenu->addAction(resetFoilScale);
	FoilMenu->addAction(showPanels);
	FoilMenu->addAction(showNeutralLine);
	FoilMenu->addAction(XDirectStyleAct);
	
	DesignMenu = menuBar()->addMenu(tr("&Design"));
	DesignMenu->addAction(NormalizeFoil);
	DesignMenu->addAction(DerotateFoil);
	DesignMenu->addAction(RefineLocalFoil);
	DesignMenu->addAction(RefineGlobalFoil);
	DesignMenu->addAction(EditCoordsFoil);
	DesignMenu->addAction(ScaleFoil);
	DesignMenu->addAction(SetTEGap);
	DesignMenu->addAction(SetLERadius);
	DesignMenu->addAction(SetFlap);
	DesignMenu->addSeparator();
	DesignMenu->addAction(InterpolateFoils);
	DesignMenu->addAction(NacaFoils);
	
	XFoilAnalysisMenu = menuBar()->addMenu(tr("Analysis"));
	XFoilAnalysisMenu->addAction(definePolar);
	XFoilAnalysisMenu->addAction(defineBatch);
	XFoilAnalysisMenu->addAction(MultiThreadedBatchAct);
	XFoilAnalysisMenu->addSeparator();
	XFoilAnalysisMenu->addAction(resetXFoil);
	XFoilAnalysisMenu->addAction(viewXFoilAdvanced);
	XFoilAnalysisMenu->addAction(viewLogFile);
	
	PolarMenu = menuBar()->addMenu(tr("&Polars"));
	currentPolarMenu = PolarMenu->addMenu(tr("Current Polar"));
	currentPolarMenu->addAction(ShowPolarProps);
	currentPolarMenu->addAction(editCurPolar);
	currentPolarMenu->addAction(resetCurPolar);
	currentPolarMenu->addAction(deletePolar);
	currentPolarMenu->addAction(RenamePolarAct);
	currentPolarMenu->addAction(ExportPolarAct);
	currentPolarMenu->addAction(exportCurPolar);
	currentPolarMenu->addSeparator();
	currentPolarMenu->addAction(showPolarOpps);
	currentPolarMenu->addAction(hidePolarOpps);
	currentPolarMenu->addAction(deletePolarOpps);
	PolarMenu->addSeparator();
	PolarMenu->addAction(ImportPolarAct);
	PolarMenu->addAction(m_pImportXFoilPolar);
	PolarMenu->addSeparator();
	PolarMenu->addAction(exportAllPolarsNRELAct);
	PolarMenu->addAction(exportAllPolars);
	PolarMenu->addSeparator();
	//	PolarMenu->addAction(XDirectPolarFilter);
	//	PolarMenu->addSeparator();
	PolarMenu->addAction(showAllPolars);
	PolarMenu->addAction(hideAllPolars);
	PolarMenu->addSeparator();
	GraphPolarMenu = PolarMenu->addMenu(tr("Polar Graphs"));
	GraphPolarMenu->addAction(allPolarGraphsSettingsAct);
	GraphPolarMenu->addAction(allPolarGraphsScales);
	GraphPolarMenu->addAction(resetGraphLegend);
	GraphPolarMenu->addSeparator();
	GraphPolarMenu->addAction(AllPolarGraphsAct);
	GraphPolarMenu->addAction(TwoPolarGraphsAct);
	GraphPolarMenu->addSeparator();
	for(int i=0; i<5; i++)
		GraphPolarMenu->addAction(PolarGraphAct[i]);
	GraphPolarMenu->addSeparator();
	GraphPolarMenu->addAction(highlightOppAct);
	
	
	OpPointMenu = menuBar()->addMenu(tr("Operating Points"));
	currentOppMenu = OpPointMenu->addMenu(tr("Current OpPoint"));
	currentOppMenu->addAction(ShowWOppProps);
	currentOppMenu->addAction(exportCurOpp);
	currentOppMenu->addAction(deleteCurOpp);
	CpGraphMenu = OpPointMenu->addMenu(tr("Cp Graph"));
	CpGraphMenu->addAction(setCpVarGraph);
	CpGraphMenu->addAction(setQVarGraph);
	CpGraphMenu->addSeparator();
	CpGraphMenu->addAction(showInviscidCurve);
	CpGraphMenu->addSeparator();
	CurXFoilResults = CpGraphMenu->addMenu(tr("Current XFoil Results"));
	CurXFoilResults->addAction(CurXFoilResExport);
	CurXFoilResults->addSeparator();
	CurXFoilResults->addAction(CurXFoilCtPlot);
	CurXFoilResults->addAction(CurXFoilDbPlot);
	CurXFoilResults->addAction(CurXFoilDtPlot);
	CurXFoilResults->addAction(CurXFoilRtLPlot);
	CurXFoilResults->addAction(CurXFoilRtPlot);
	CurXFoilResults->addAction(CurXFoilNPlot);
	CurXFoilResults->addAction(CurXFoilCdPlot);
	CurXFoilResults->addAction(CurXFoilCfPlot);
	CurXFoilResults->addAction(CurXFoilUePlot);
	CurXFoilResults->addAction(CurXFoilHPlot);
	CpGraphMenu->addSeparator();
	CpGraphMenu->addAction(resetCurGraphScales);
	CpGraphMenu->addAction(defineCpGraphSettings);
	CpGraphMenu->addAction(exportCurGraphAct);
	OpPointMenu->addSeparator();
	OpPointMenu->addAction(showCurOppOnly);
	OpPointMenu->addAction(hideAllOpPoints);
	OpPointMenu->addAction(showAllOpPoints);
	
	//XDirect foil Context Menu
	OperFoilCtxMenu = new QMenu(tr("Context Menu"),this);
	OperFoilCtxMenu->addMenu(currentFoilMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addMenu(currentPolarMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addMenu(DesignMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	CurOppCtxMenu = OperFoilCtxMenu->addMenu(tr("Current OpPoint"));
	CurOppCtxMenu->addAction(ShowWOppProps);
	CurOppCtxMenu->addAction(exportCurOpp);
	CurOppCtxMenu->addAction(deleteCurOpp);
	
	OperFoilCtxMenu->addSeparator();//_______________
	//	CurGraphCtxMenu = OperFoilCtxMenu->addMenu(tr("Cp graph"));
	OperFoilCtxMenu->addMenu(CpGraphMenu);
	
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(definePolar);
	OperFoilCtxMenu->addAction(defineBatch);
	OperFoilCtxMenu->addAction(MultiThreadedBatchAct);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(showAllPolars);
	OperFoilCtxMenu->addAction(hideAllPolars);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(showCurOppOnly);
	OperFoilCtxMenu->addAction(showAllOpPoints);
	OperFoilCtxMenu->addAction(hideAllOpPoints);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(resetFoilScale);
	OperFoilCtxMenu->addAction(showPanels);
	OperFoilCtxMenu->addAction(showNeutralLine);
	OperFoilCtxMenu->addAction(XDirectStyleAct);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(saveViewToImageFileAct);
	//End XDirect foil Context Menu
	
	
	//XDirect polar Context Menu
	OperPolarCtxMenu = new QMenu(tr("Context Menu"),this);
	OperPolarCtxMenu->addMenu(currentFoilMenu);
	OperPolarCtxMenu->addMenu(currentPolarMenu);
	OperPolarCtxMenu->addSeparator();//__
	OperPolarCtxMenu->addSeparator()->setText(tr("Current Graph"));
	OperPolarCtxMenu->addAction(resetCurGraphScales);
	OperPolarCtxMenu->addAction(exportCurGraphAct);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addMenu(GraphPolarMenu);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(definePolar);
	OperPolarCtxMenu->addAction(defineBatch);
	OperPolarCtxMenu->addAction(MultiThreadedBatchAct);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(showAllPolars);
	OperPolarCtxMenu->addAction(hideAllPolars);
	OperPolarCtxMenu->addAction(showAllOpPoints);
	OperPolarCtxMenu->addAction(hideAllOpPoints);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(saveViewToImageFileAct);
	
	//End XDirect polar Context Menu
}


void MainFrame::CreateXInverseActions()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	
	SaveFoil = new QAction(QIcon(":/images/OnStoreFoil.png"), tr("Store Foil"), this);
	SaveFoil->setStatusTip(tr("Store Foil in database"));
	connect(SaveFoil, SIGNAL(triggered()), pXInverse, SLOT(OnStoreFoil()));
	
	ExtractFoil = new QAction(QIcon(":/images/OnExtractFoil.png"),tr("Extract Foil"), this);
	ExtractFoil->setStatusTip(tr("Extract a Foil from the database for modification"));
	connect(ExtractFoil, SIGNAL(triggered()), pXInverse, SLOT(OnExtractFoil()));
	
	InverseStyles = new QAction(tr("Define Styles"), this);
	InverseStyles->setStatusTip(tr("Define the styles for this view"));
	connect(InverseStyles, SIGNAL(triggered()), pXInverse, SLOT(OnInverseStyles()));
	
	InverseResetScale = new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset foil scale")+"\t(R)", this);
	InverseResetScale->setStatusTip(tr("Resets the scale to fit the screen size"));
	connect(InverseResetScale, SIGNAL(triggered()), pXInverse, SLOT(OnResetFoilScale()));
	
	InverseInsertCtrlPt = new QAction(tr("Insert Control Point")+"\tShift+Click", this);
	connect(InverseInsertCtrlPt, SIGNAL(triggered()), pXInverse, SLOT(OnInsertCtrlPt()));
	
	InverseRemoveCtrlPt = new QAction(tr("Remove Control Point")+"\tCtrl+Click", this);
	connect(InverseRemoveCtrlPt, SIGNAL(triggered()), pXInverse, SLOT(OnRemoveCtrlPt()));
	
	InvQInitial = new QAction(tr("Show Q-Initial"), this);
	InvQInitial->setCheckable(true);
	connect(InvQInitial, SIGNAL(triggered()), pXInverse, SLOT(OnQInitial()));
	
	InvQSpec = new QAction(tr("Show Q-Spec"), this);
	InvQSpec->setCheckable(true);
	connect(InvQSpec, SIGNAL(triggered()), pXInverse, SLOT(OnQSpec()));
	
	InvQViscous = new QAction(tr("Show Q-Viscous"), this);
	InvQViscous->setCheckable(true);
	connect(InvQViscous, SIGNAL(triggered()), pXInverse, SLOT(OnQViscous()));
	
	InvQPoints = new QAction(tr("Show Points"), this);
	InvQPoints->setCheckable(true);
	connect(InvQPoints, SIGNAL(triggered()), pXInverse, SLOT(OnQPoints()));
	
	InvQReflected = new QAction(tr("Show Reflected"), this);
	InvQReflected->setCheckable(true);
	
	connect(InvQReflected, SIGNAL(triggered()), pXInverse, SLOT(OnQReflected()));
	XInverseGraphDlg = new QAction(tr("Define Graph Settings")+"\t(G)", this);
	connect(XInverseGraphDlg, SIGNAL(triggered()), pXInverse, SLOT(OnGraphSettings()));
	
	InverseZoomIn = new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
	InverseZoomIn->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));
	connect(InverseZoomIn, SIGNAL(triggered()), pXInverse, SLOT(OnZoomIn()));
	
	InverseZoomX = new QAction(QIcon(":/images/OnZoomGraphX.png"), tr("Zoom X Scale"), this);
	InverseZoomX->setStatusTip(tr("Zoom X Scale Only"));
	connect(InverseZoomX, SIGNAL(triggered()), pXInverse, SLOT(OnZoomX()));
	
	InverseZoomY = new QAction(QIcon(":/images/OnZoomGraphY.png"), tr("Zoom Y Scale"), this);
	InverseZoomY->setStatusTip(tr("Zoom Y Scale Only"));
	connect(InverseZoomY, SIGNAL(triggered()), pXInverse, SLOT(OnZoomY()));
}


void MainFrame::CreateXInverseMenus()
{
	//MainMenu for XInverse Application
	XInverseViewMenu = menuBar()->addMenu(tr("&View"));
	XInverseViewMenu->addAction(InverseStyles);
	XInverseViewMenu->addAction(XInverseGraphDlg);
	XInverseViewMenu->addSeparator();
	XInverseViewMenu->addAction(saveViewToImageFileAct);
	
	InverseGraphMenu = menuBar()->addMenu(tr("&Graph"));
	InverseGraphMenu->addAction(XInverseGraphDlg);
	InverseGraphMenu->addAction(resetCurGraphScales);
	InverseGraphMenu->addAction(exportCurGraphAct);
	
	InverseFoilMenu = menuBar()->addMenu(tr("&Foil"));
	InverseFoilMenu->addAction(SaveFoil);
	InverseFoilMenu->addAction(ExtractFoil);
	InverseFoilMenu->addAction(InverseResetScale);
	InverseFoilMenu->addSeparator();
	InverseFoilMenu->addAction(InvQInitial);
	InverseFoilMenu->addAction(InvQSpec);
	InverseFoilMenu->addAction(InvQViscous);
	InverseFoilMenu->addAction(InvQPoints);
	InverseFoilMenu->addAction(InvQReflected);
	
	//Context Menu for XInverse Application
	InverseContextMenu = new QMenu(tr("Context Menu"),this);
	InverseContextMenu->addAction(InverseStyles);
	InverseContextMenu->addAction(InverseResetScale);
	InverseContextMenu->addAction(resetCurGraphScales);
	InverseContextMenu->addAction(XInverseGraphDlg);
	InverseContextMenu->addSeparator();
	InverseContextMenu->addAction(InverseInsertCtrlPt);
	InverseContextMenu->addAction(InverseRemoveCtrlPt);
	InverseContextMenu->addSeparator();
	InverseContextMenu->addAction(InvQInitial);
	InverseContextMenu->addAction(InvQSpec);
	InverseContextMenu->addAction(InvQViscous);
	InverseContextMenu->addAction(InvQPoints);
	InverseContextMenu->addAction(InvQReflected);
}



void MainFrame::CreateXInverseToolbar()
{
	m_pctrlFullInverse  = new QRadioButton(tr("Full Inverse"));
	m_pctrlMixedInverse = new QRadioButton(tr("Mixed Inverse"));
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	connect(m_pctrlFullInverse,  SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));
	connect(m_pctrlMixedInverse, SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));
	
	m_pctrlInvZoomX = new QToolButton;
	m_pctrlInvZoomX->setDefaultAction(InverseZoomX);
	m_pctrlInvZoomX->setCheckable(true);
	
	m_pctrlInvZoomY = new QToolButton;
	m_pctrlInvZoomY->setDefaultAction(InverseZoomY);
	m_pctrlInvZoomY->setCheckable(true);
	
	m_pctrlInvZoomIn = new QToolButton;
	m_pctrlInvZoomIn->setDefaultAction(InverseZoomIn);
	m_pctrlInvZoomIn->setCheckable(true);
	
	m_pctrlXInverseToolBar = addToolBar(tr("XInverse"));
	m_pctrlXInverseToolBar->setObjectName("XInverseToolbar");
	m_pctrlXInverseToolBar->addAction(OnAFoilAct);
	m_pctrlXInverseToolBar->addWidget(m_pctrlFullInverse);
	m_pctrlXInverseToolBar->addWidget(m_pctrlMixedInverse);
	m_pctrlXInverseToolBar->addSeparator();
	m_pctrlXInverseToolBar->addAction(ExtractFoil);
	m_pctrlXInverseToolBar->addAction(SaveFoil);
	m_pctrlXInverseToolBar->addSeparator();
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomIn);
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomX);
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomY);
	m_pctrlXInverseToolBar->addAction(resetCurGraphScales);
	m_pctrlXInverseToolBar->addAction(InverseResetScale);
}



bool MainFrame::DeleteFoil(CFoil *pFoil, bool bAsk)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	if(!pFoil || !pFoil->getName().length()) return false;
	QString strong;
	
	if(bAsk)
	{
		strong = tr("Are you sure you want to delete")  +"\n"+ pFoil->getName() +"\n";
		strong+= tr("and all associated Polars, Blades and Simulations ?");
		
		int resp = QMessageBox::question(this,tr("Question"), strong,  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if(resp != QMessageBox::Yes) return false;
	}
	
	g_foilStore.remove(pFoil);
	
	// now we set all the pretty pointers to NULL, just in case
	g_qbem->m_pCurPolar = NULL;
	g_qbem->m_pCur360Polar = NULL;
	g_qbem->m_pCurFoil = NULL;
	
	
	g_qbem->m_pBlade = NULL;
	g_qbem->m_pBData = NULL;
	g_qbem->m_pTurbineBData = NULL;
	g_qbem->m_pBEMData = NULL;
	g_qbem->m_pTData = NULL;
	g_qbem->m_pTBEMData = NULL;
	g_qbem->m_pCBEMData = NULL;
	
	g_qdms->m_pBlade = NULL;
	g_qdms->m_pDData = NULL;
	g_qdms->m_pTurbineDData = NULL;
	g_qdms->m_pDMSData = NULL;
	g_qdms->m_pTData = NULL;
	g_qdms->m_pTDMSData = NULL;
	g_qdms->m_pCDMSData = NULL;
	
	pXDirect->m_pCurOpp = NULL;
	pXDirect->m_pCurPolar = NULL;
	
	g_pCurFoil = NULL;
	
	pXDirect->SetControls();
	SetSaveState(false);
	
	return true;
}



void MainFrame::DeleteProject()
{
	
	// clear everything
	int i;
	
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->m_pXFoil->m_FoilName = "";
	g_pCurFoil  = NULL;
	pXDirect->m_pCurPolar = NULL;
	pXDirect->m_pCurOpp   = NULL;
	pXDirect->SetFoil();
	
	g_QFEMModule->CleanUp();
	
	////new code DM ////
	
	if (m_iApp == BEM && m_iView == BLADEVIEW && g_qbem->m_WingEdited){
		g_qbem->m_WingEdited = false;
		delete g_qbem->m_pBlade;
		g_qbem->m_pBlade = NULL;
		g_qbem->CheckButtons();
	}
	
	g_qbem->m_pBData          = NULL;
	g_qbem->m_pTurbineBData   = NULL;
	g_qbem->m_pBEMData        = NULL;
	g_qbem->m_pTData          = NULL;
	g_qbem->m_pTBEMData       = NULL;
	g_qbem->m_pCBEMData       = NULL;
	g_qbem->m_pBlade          = NULL;
	g_qbem->m_pCurFoil        = NULL;
	g_qbem->m_pCur360Polar    = NULL;
	g_qbem->m_pCurPolar       = NULL;
	
	if (m_iApp == DMS && m_iView == BLADEVIEW && g_qdms->m_WingEdited){
		g_qdms->m_WingEdited = false;
		delete g_qdms->m_pBlade;
		g_qdms->m_pBlade = NULL;
		g_qdms->CheckButtons();
	}
	
	
	g_qdms->m_pDData          = NULL;
	g_qdms->m_pTurbineDData   = NULL;
	g_qdms->m_pDMSData        = NULL;
	g_qdms->m_pTData          = NULL;
	g_qdms->m_pTDMSData       = NULL;
	g_qdms->m_pBlade          = NULL;
	g_qdms->m_pCurFoil        = NULL;
	g_qdms->m_pCur360Polar    = NULL;
	g_qdms->m_pCurPolar       = NULL;
	g_qdms->m_pCDMSData       = NULL;
	
	g_QLLTCutPlaneStore.disableSignal();
	for (i=g_QLLTCutPlaneStore.size()-1; i>=0; i--)
	{
		g_QLLTCutPlaneStore.removeAt(i);
	}
	g_QLLTCutPlaneStore.enableSignal();
	g_QLLTCutPlaneStore.emitObjectListChanged(false);
	
	g_QLLTHAWTSimulationStore.disableSignal();
	for (i=g_QLLTHAWTSimulationStore.size()-1; i>=0; i--)
	{
		g_QLLTHAWTSimulationStore.removeAt(i);
	}
	g_QLLTHAWTSimulationStore.enableSignal();
	g_QLLTHAWTSimulationStore.emitObjectListChanged(false);
	
	
	g_QLLTVAWTSimulationStore.disableSignal();
	for (i=g_QLLTVAWTSimulationStore.size()-1; i>=0; i--)
	{
		g_QLLTVAWTSimulationStore.removeAt(i);
	}
	g_QLLTVAWTSimulationStore.enableSignal();
	g_QLLTVAWTSimulationStore.emitObjectListChanged(false);
	
	
	for (i=g_FASTSimulationStore.size()-1; i>=0; i--)
	{
		g_FASTSimulationStore.removeAt(i);
	}
	
	for (i=g_windFieldStore.size()-1; i>=0; i--)
	{
		g_windFieldStore.removeAt(i);
	}
	
	for (i = g_bladestructureloadingStore.size()-1; i >= 0; i--)
	{
		g_bladestructureloadingStore.removeAt(i);
	}
	
	for (i = g_bladeStructureStore.size()-1; i >= 0; i--)
	{
		g_bladeStructureStore.removeAt(i);
	}
	
	for (i=g_tdmsdataStore.size()-1; i>=0 ;i--)
	{
		g_tdmsdataStore.removeAt(i);
	}
	
	for (i=g_cdmsdataStore.size()-1; i>=0 ;i--)
	{
		g_cdmsdataStore.removeAt(i);
	}
	
	for (i=g_verttdataStore.size()-1; i>=0;i--)
	{
		g_verttdataStore.removeAt(i);
	}
	
	for (i=g_dmsdataStore.size()-1; i>=0;i--)
	{
		g_dmsdataStore.removeAt(i);
	}
	
	for (i = g_verticalRotorStore.size()-1; i>=0;i--)
	{
		g_verticalRotorStore.removeAt(i);
	}
	
	for (i=g_tbemdataStore.size()-1; i>=0 ;i--)
	{
		g_tbemdataStore.removeAt(i);
	}
	
	for (i=g_cbemdataStore.size()-1; i>=0 ;i--)
	{
		g_cbemdataStore.removeAt(i);
	}
	
	for (i=g_tdataStore.size()-1; i>=0;i--)
	{
		g_tdataStore.removeAt(i);
	}
	
	for (i=g_bemdataStore.size()-1; i>=0;i--)
	{
		g_bemdataStore.removeAt(i);
	}
	
	for (i = g_rotorStore.size()-1; i >= 0; i--)
	{
		g_rotorStore.removeAt(i);
	}

    for (i = g_StrutStore.size()-1; i>=0;i--)
    {
        g_StrutStore.removeAt(i);
    }
	
	for (i = g_360PolarStore.size()-1; i>=0;i--)
	{
		g_360PolarStore.removeAt(i);
	}
	
	for (i=g_oppointStore.size()-1; i>=0; i--)
	{
		g_oppointStore.removeAt(i);
	}
	
	for (i=g_polarStore.size()-1; i>=0; i--)
	{
		g_polarStore.removeAt(i);
	}
	
	for (i=g_foilStore.size()-1; i>=0; i--)
	{
		g_foilStore.removeAt(i);
	}
	for (i=g_noiseSimulationStore.size()-1; i>=0; i--)
	{
		g_noiseSimulationStore.removeAt(i);
	}
	
	
	
	
	if (m_iApp == BEM)
	{
		g_qbem->UpdateFoils();
		g_qbem->UpdateBlades();
		g_qbem->UpdateTurbines();
		
		g_qbem->CreatePolarCurve();
		g_qbem->CreatePowerCurves();
		g_qbem->CreateRotorCurves();
		g_qbem->CreateCharacteristicsCurves();
		
		g_qbem->CheckButtons();
		g_qbem->UpdateView();
	}
	
	if (m_iApp == DMS)
	{
		//            pDMS->UpdateFoils();
		g_qdms->UpdateBlades();
		g_qdms->UpdateTurbines();
		
		g_qdms->CreatePolarCurve();
		g_qdms->CreatePowerCurves();
		g_qdms->CreateRotorCurves();
		g_qdms->CheckButtons();
		g_qdms->UpdateView();
	}
	
	UpdateFoils();
	if(pXDirect->m_bPolar) pXDirect->CreatePolarCurves();
	else                   pXDirect->CreateOppCurves();
	
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->FillFoilTable();
	pAFoil->SelectFoil();
	
	QXInverse *pXInverse =(QXInverse*)m_pXInverse;
	pXInverse->Clear();
	
	SetProjectName("");
	SetSaveState(true);
}

QColor MainFrame::GetColor(int type)
{
	//type
	// 0=Foil
	// 1=Polar
	// 2=Opp
	// 3=Wing (unused)
	// 4=WPolar
	// 5=WOpp
	// 6=POpp
	
	// JW comment
	
	// 7=Rotor Data HAWT
	// 8=Turbine Data HAWT
	// 9=Rotor Blade Data HAWT
	// 10=Turbine Blade Data HAWT
	// 11=360 Polar
	// 12=Characteristic Data HAWT
	
	// 13=Rotor Data VAWT
	// 14=Turbine Data VAWT
	// 15=Rotor Blade Data VAWT
	// 16=Turbine Blade Data VAWT
	
	// end JW comment
	
	QColor color = QColor(255,0,0);
	int i,j;
	bool bFound = false;
	switch (type)
	{
	case 0:
	{
		CFoil *pFoil;
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_foilStore.size(); i++)
			{
				pFoil = g_foilStore.at(i);
				bFound = false;
				if(pFoil->m_FoilColor == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) return m_crColors[j];
		}
		return m_crColors[g_foilStore.size()%24];
		break;
	}
	case 1:
	{
		CPolar *pPolar;
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_polarStore.size(); i++)
			{
				pPolar = (CPolar*)g_polarStore.at(i);
				bFound = false;
				if(pPolar->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) return m_crColors[j];
		}
		return m_crColors[g_polarStore.size()%24];
		break;
	}
	case 2:
	{
		OpPoint *pOpPoint;
		for (j=0; j<24; j++){
			for (i=0; i<g_oppointStore.size(); i++)
			{
				pOpPoint = g_oppointStore.at(i);
				bFound = false;
				if(pOpPoint->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) return m_crColors[j];
		}
		return m_crColors[g_oppointStore.size()%24];
		break;
	}
	case 4:
	{
		return m_crColors[0];
		break;
	}
	case 5:
	{
		return m_crColors[0];
		break;
	}
	case 6:
	{
		return m_crColors[0];
		break;
	}
		/////// new code DM/////
	case 7:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_bemdataStore.size(); i++)
			{
				bFound = false;
				if(g_bemdataStore.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_bemdataStore.size()%24];
		break;
	}
		
	case 8:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_tbemdataStore.size(); i++)
			{
				bFound = false;
				if(g_tbemdataStore.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_tbemdataStore.size()%24];
		break;
	}
	case 9:
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		for (j=0; j<24; j++)
		{
			for (i=0; i<pBEM->m_pBEMData->m_BData.size(); i++)
			{
				bFound = false;
				if(pBEM->m_pBEMData->m_BData.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[pBEM->m_pBEMData->m_BData.size()%24];
		break;
	}
		
	case 10:
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		for (j=0; j<24; j++)
		{
			for (i=0; i<pBEM->m_pTBEMData->m_BData.size(); i++)
			{
				bFound = false;
				if(pBEM->m_pTBEMData->m_BData.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[pBEM->m_pTBEMData->m_BData.size()%24];
		break;
	}
	case 11:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_360PolarStore.size(); i++)
			{
				bFound = false;
				C360Polar *pPolar = (C360Polar *) g_360PolarStore.at(i);
				if(pPolar->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_360PolarStore.size()%24];
		break;
	}
	case 12:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_cbemdataStore.size(); i++)
			{
				bFound = false;
				CBEMData *pBEMData = g_cbemdataStore.at(i);
				if(pBEMData->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_cbemdataStore.size()%24];
		break;
	}
		//// end new code DM///
		
		/////// new code JW /////
	case 13:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_dmsdataStore.size(); i++)
			{
				bFound = false;
				if(g_dmsdataStore.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_dmsdataStore.size()%24];
		break;
	}
		
	case 14:
	{
		for (j=0; j<24; j++)
		{
			for (i=0; i<g_tdmsdataStore.size(); i++)
			{
				bFound = false;
				if(g_tdmsdataStore.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[g_tdmsdataStore.size()%24];
		break;
	}
		
	case 15:
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		for (j=0; j<24; j++)
		{
			for (i=0; i<pDMS->m_pDMSData->m_DData.size(); i++)
			{
				bFound = false;
				if(pDMS->m_pDMSData->m_DData.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[pDMS->m_pDMSData->m_DData.size()%24];
		break;
	}
		
	case 16:
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		for (j=0; j<24; j++)
		{
			for (i=0; i<pDMS->m_pTDMSData->m_DData.size(); i++)
			{
				bFound = false;
				if(pDMS->m_pTDMSData->m_DData.at(i)->m_Color == m_crColors[j])
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				return m_crColors[j];
		}
		return m_crColors[pDMS->m_pTDMSData->m_DData.size()%24];
		break;
	}
		
		
		//// end new code JW ///
		
	default:
	{
		return QColor(255,0,0);
	}
	}
	return color;
}



CFoil* MainFrame::GetFoil(QString strFoilName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strFoilName.length()) return NULL;
	CFoil* pFoil;
	for (int i=0; i<g_foilStore.size(); i++)
	{
		pFoil = g_foilStore.at(i);
		if (pFoil->getName() == strFoilName)
		{
			return pFoil;
		}
	}
	
	return NULL;
}

OpPoint *MainFrame::GetOpp(double Alpha)
{
	OpPoint* pOpPoint;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	CPolar *pCurPolar = pXDirect->m_pCurPolar;
	if(!pCurPolar) return NULL;
	
	for (int i=0; i<g_oppointStore.size(); i++)
	{
		if(!pCurPolar) return NULL;
		pOpPoint = g_oppointStore.at(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == g_pCurFoil->getName())
		{
			if (pOpPoint->m_strPlrName == pCurPolar->getName())
			{
				if(pCurPolar->m_PolarType != FIXEDAOAPOLAR)
				{
					if(fabs(pOpPoint->Alpha - Alpha) <0.001)
					{
						return pOpPoint;
					}
				}
				else
				{
					if(fabs(pOpPoint->Reynolds - Alpha) <0.1)
					{
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}


void MainFrame::keyPressEvent(QKeyEvent *event)
{
	if(m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyPressEvent(event);
	}
	else if(m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyPressEvent(event);
	}
	else if(m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyPressEvent(event);
	}
	else
	{
		switch (event->key())
		{
		case Qt::Key_L:
		{
			OnLogFile();
			break;
		}
		case Qt::Key_Control:
		{
			UpdateView();
			break;
		}	
		default:
			event->ignore();
		}
	}
}


void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
	if(m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyReleaseEvent(event);
	}
	else if(m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyReleaseEvent(event);
	}
	else if(m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyReleaseEvent(event);
	}
}

bool MainFrame::LoadSettings()
{
	QPoint pt;
	bool bFloat;
	int SettingsFormat;
	QSize size;
	
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
	
	settings.beginGroup("MainFrame");
	{
		SettingsFormat = settings.value("SettingsFormat").toInt();
		if(SettingsFormat != SETTINGSFORMAT) return false;
		
		m_GraphExportFilter = settings.value("GraphExportFilter",".csv").toString();
		
		m_LanguageFilePath = settings.value("LanguageFilePath").toString();
		
		bFloat  = settings.value("XDirect_Float").toBool();
		pt.rx() = settings.value("XDirect_x").toInt();
		pt.ry() = settings.value("XDirect_y").toInt();
		size    = settings.value("XDirectSize").toSize();
		m_pctrlXDirectWidget->setFloating(bFloat);
		if(bFloat) m_pctrlXDirectWidget->move(pt);
		m_pctrlXDirectWidget->resize(size);
		
		bFloat  = settings.value("AFoil_Float").toBool();
		pt.rx() = settings.value("AFoil_x").toInt();
		pt.ry() = settings.value("AFoil_y").toInt();
		size    = settings.value("AFoilSize").toSize();
		m_pctrlAFoilWidget->setFloating(bFloat);
		if(bFloat) m_pctrlAFoilWidget->move(pt);
		m_pctrlAFoilWidget->resize(size);
		
		bFloat  = settings.value("XInverse_Float").toBool();
		pt.rx() = settings.value("XInverse_x").toInt();
		pt.ry() = settings.value("XInverse_y").toInt();
		size    = settings.value("XInverseSize").toSize();
		m_pctrlXInverseWidget->setFloating(bFloat);
		if(bFloat) m_pctrlXInverseWidget->move(pt);
		m_pctrlXInverseWidget->resize(size);
		
		//////////////new code DM////////////////////
		bFloat  = settings.value("XBEM_Float").toBool();
		pt.rx() = settings.value("XBEM_x").toInt();
		pt.ry() = settings.value("XBEM_y").toInt();
		m_pctrlBEMWidget->setFloating(bFloat);
		if(bFloat) m_pctrlBEMWidget->move(pt);
		
		m_WORK_GROUPS = settings.value("OPENCL_WORK_GROUPS").toInt();
		
		bFloat  = settings.value("Simu_Float").toBool();
		pt.rx() = settings.value("Simu_x").toInt();
		pt.ry() = settings.value("Simu_y").toInt();
		m_pctrlSimuWidget->setFloating(bFloat);
		if(bFloat) m_pctrlSimuWidget->move(pt);
		m_PowerUnit = settings.value("powerunit").toInt();
		/////////////end new code DM///////////////
		
		//////////////new code JW////////////////////
		bFloat  = settings.value("XDMS_Float").toBool();
		pt.rx() = settings.value("XDMS_x").toInt();
		pt.ry() = settings.value("XDMS_y").toInt();
		size    = settings.value("XDMSSize").toSize();
		m_pctrlDMSWidget->setFloating(bFloat);
		if(bFloat) m_pctrlDMSWidget->move(pt);
		m_pctrlDMSWidget->resize(size);
		
		bFloat  = settings.value("SimuDMS_Float").toBool();
		pt.rx() = settings.value("SimuDMS_x").toInt();
		pt.ry() = settings.value("SimuDMS_y").toInt();
		size    = settings.value("SimuDMSSize").toSize();
		m_pctrlSimuWidgetDMS->setFloating(bFloat);
		if(bFloat) m_pctrlSimuWidgetDMS->move(pt);
		m_pctrlSimuWidgetDMS->resize(size);
		/////////////end new code JW///////////////
		
		m_LastDirName = settings.value("LastDirName").toString();
		m_ImageDirName = settings.value("ImageDirName").toString();
		m_ExportLastDirName = settings.value("ExportLastDirName").toString();
		m_LengthUnit  = settings.value("LengthUnit").toInt();
		m_AreaUnit    = settings.value("AreaUnit").toInt();
		m_WeightUnit  = settings.value("WeightUnit").toInt();
		m_SpeedUnit   = settings.value("SpeedUnit").toInt();
		m_ForceUnit   = settings.value("ForceUnit").toInt();
		m_MomentUnit  = settings.value("MomentUnit").toInt();
		m_BackgroundColor.setRed(settings.value("BackgroundColorRed").toInt());
		m_BackgroundColor.setGreen(settings.value("BackgroundColorGreen").toInt());
		m_BackgroundColor.setBlue(settings.value("BackgroundColorBlue").toInt());
		m_TextColor.setRed(settings.value("TextColorRed").toInt());
		m_TextColor.setGreen(settings.value("TextColorGreen").toInt());
		m_TextColor.setBlue(settings.value("TextColorBlue").toInt());
		
		m_TextFont.setFamily(settings.value("TextFontFamily", "Courier").toString());
		m_TextFont.setPointSize(settings.value("TextFontPointSize", 10).toInt());
		m_TextFont.setStyleStrategy(QFont::OpenGLCompatible);
		
		m_ImageFormat = settings.value("ImageFormat").toInt();
		m_DlgPos.rx() = settings.value("DlgPos_x").toInt();
		m_DlgPos.ry() = settings.value("DlgPos_y").toInt();
		
		QString RecentF,strange;
		m_RecentFiles.clear();
		int n=0;
		do
		{
			RecentF = QString("RecentFile_%1").arg(n);
			strange = settings.value(RecentF).toString();
			if(strange.length())
			{
				m_RecentFiles.append(strange);
				n++;
			}
			else break;
		}while(n<MAXRECENTFILES);
		
		m_bReverseZoom = settings.value("ReverseZoom").toBool();
		m_bAlphaChannel = settings.value("AlphaChannel").toBool();
	}
	
	return true;
}

int MainFrame::loadQBladeProject(QString PathName)
{
	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = tr("Could not read the file\n")+PathName;
		QMessageBox::information(window(), tr("Info"), strange);
		return 0;
	}
	QXDirect * pXDirect = (QXDirect*)m_pXDirect;
	QString end;
	//QFileInfo fileinfo(XFile);
	end = PathName.right(4);
	end = end.toLower();
	//QString dir = fileinfo.canonicalPath();
	
	int pos = PathName.lastIndexOf("/");
	if(pos>0) m_LastDirName = PathName.left(pos);
	
	if(end==".wpa")
	{
		if(!m_bSaved)
		{
			QString strong = tr("Save the current project ?");
			int resp =  QMessageBox::question(0,tr("Save"), strong,  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
			if(resp==QMessageBox::Cancel)
			{
				XFile.close();
				return 0;
			}
			else if (resp==QMessageBox::Yes)
			{
				if(!SaveProject(m_FileName))
				{
					XFile.close();
					return 0;
				}
			}
		}
		
		DeleteProject();
		
		QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
		ar.setVersion(QDataStream::Qt_4_5);
#endif
		ar.setByteOrder(QDataStream::LittleEndian);
//		if(SerializeProject(ar, false, 1))
//		{
//			g_pCurFoil = pXDirect->SetFoil();
//			UpdateFoils();
//			UpdateView();
//			QApplication::restoreOverrideCursor();
//		}
//		else
//		{
//			QApplication::restoreOverrideCursor();
//			QMessageBox::warning(this,tr("Warning"), tr("Error reading the file"));
//			DeleteProject();
//		}
		try {
			SerializeProject(ar, false);
			g_pCurFoil = pXDirect->SetFoil();
			UpdateFoils();
			UpdateView();
		} catch (Serializer::Exception &error) {
			DeleteProject();
			QApplication::restoreOverrideCursor();			
			QMessageBox::critical(this, "Serialization error", error.message);
		}

		AddRecentFile(PathName);
		SetSaveState(true);
		SetProjectName(PathName);
		
		XFile.close();
		
		
		pXDirect->SetControls();
		
		return XFOILANALYSIS;
	}
	
	XFile.close();
	
	return 0;
}

void MainFrame::OnAFoil()
{
	setIApp (DIRECTDESIGN);  // NM vorher: m_iApp = DIRECTDESIGN;
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlAFoilToolBar->show();
	
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->hide();
	m_pctrlAFoilWidget->show();
	
	////////////new code DM/////////////
	OnAFoilAct->setChecked(true);
	OnXDirectAct->setChecked(false);
	OnXInverseAct->setChecked(false);
	////////end new code DM//////////	
	
	SetCentralWidget();
	SetMenus();
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->SetParams();
}

void MainFrame::OnBEM()
{
	// NM the new toolBar
	g_qbem->m_BEMToolBar->show();
	switch (g_mainFrame->m_iView) {
	case BLADEVIEW:
		g_qbem->m_BEMToolBar->setState(BEMToolbar::BLADEVIEW_STATE);
		break;
	case BEMSIMVIEW:
		g_qbem->m_BEMToolBar->setState(BEMToolbar::ROTORVIEW_STATE);
		break;
	case CHARSIMVIEW:
		g_qbem->m_BEMToolBar->setState(BEMToolbar::CHARACTERISTICVIEW_STATE);
		break;
	case TURBINEVIEW:
		g_qbem->m_BEMToolBar->setState(BEMToolbar::TURBINEVIEW_STATE);
		break;
	case POLARVIEW:
		g_qbem->m_BEMToolBar->setState(BEMToolbar::POLARVIEW_STATE);
		break;
	}
	
	DisconnectBEMActions();
	ConnectBEMActions();
	
	OnRotorViewAct->setChecked(g_mainFrame->m_iView==BEMSIMVIEW);
	OnTurbineViewAct->setChecked(g_mainFrame->m_iView==TURBINEVIEW);
	On360ViewAct->setChecked(g_mainFrame->m_iView==POLARVIEW);
	OnBladeViewAct->setChecked(g_mainFrame->m_iView==BLADEVIEW);
	OnCharacteristicViewAct->setChecked(g_mainFrame->m_iView==CHARSIMVIEW);
	
	g_qbem->UpdateBlades();
	SetMenus();
	
	g_qbem->configureGL();
}

void MainFrame::OnVAWTView () {
	g_qdms->CheckButtons();
	
	VAWTToolbarView->setChecked(true);
	HAWTToolbarView->setChecked(false);
	
	OnBladeViewAct->setVisible(false);
	OnRotorViewAct->setVisible(false);
	OnCharacteristicViewAct->setVisible(false);
	OnTurbineViewAct->setVisible(false);
	
	OnBladeViewAct2->setVisible(true);
	OnRotorViewAct2->setVisible(true);
	OnCharacteristicViewAct2->setVisible(true);
	OnTurbineViewAct2->setVisible(true);
	
	g_QFEMModule->setToolbarVisibility(false);
	g_QLLTModule->setToolbarVisibility(true);
	g_QLLTModule->ChangeHAWTVAWTView(true);
	g_windFieldModule->setToolbarVisibility(true);
	g_fastModule->setToolbarVisibility(false);
	g_QLLTModule->setActionIcon(":/images/LLT_VAWT.png");
}

void MainFrame::OnHAWTView () {
	g_qbem->CheckButtons();
	
	VAWTToolbarView->setChecked(false);
	HAWTToolbarView->setChecked(true);
	
	OnBladeViewAct->setVisible(true);
	OnRotorViewAct->setVisible(true);
	OnCharacteristicViewAct->setVisible(true);
	OnTurbineViewAct->setVisible(true);
	
	OnBladeViewAct2->setVisible(false);
	OnRotorViewAct2->setVisible(false);
	OnCharacteristicViewAct2->setVisible(false);
	OnTurbineViewAct2->setVisible(false);
	
	g_QFEMModule->setToolbarVisibility(true);
	g_QLLTModule->setToolbarVisibility(true);
	g_QLLTModule->ChangeHAWTVAWTView(false);
	g_windFieldModule->setToolbarVisibility(true);
	g_fastModule->setToolbarVisibility(true);
	g_QLLTModule->setActionIcon(":/images/LLT.png");
	
}

////////new code JW//////
void MainFrame::OnDMS()
{
	//// DM the new toolBar
	g_qdms->m_DMSToolBar->show();
	switch (g_mainFrame->m_iView) {
	case BLADEVIEW:
		g_qdms->m_DMSToolBar->setState(DMSToolbar::BLADEVIEW_STATE);
		break;
	case BEMSIMVIEW:
		g_qdms->m_DMSToolBar->setState(DMSToolbar::ROTORVIEW_STATE);
		break;
	case CHARSIMVIEW:
		g_qdms->m_DMSToolBar->setState(DMSToolbar::CHARACTERISTICVIEW_STATE);
		break;
	case TURBINEVIEW:
		g_qdms->m_DMSToolBar->setState(DMSToolbar::TURBINEVIEW_STATE);
		break;
	}
	//// DM end
	
	//// new code JW ////////
	DisconnectBEMActions();
	ConnectDMSActions();
	//// end new code JW ////
	
	QDMS *pDMS = (QDMS*) m_pDMS;
	
	//setIApp(DMS);  // NM vorher: m_iApp = DMS;
	
	OnBladeViewAct2->setChecked(g_mainFrame->m_iView==BLADEVIEW);// new JW, old: pDMS->m_iView
	OnRotorViewAct2->setChecked(g_mainFrame->m_iView==BEMSIMVIEW);// new JW, old: pDMS->m_iView
	OnTurbineViewAct2->setChecked(g_mainFrame->m_iView==TURBINEVIEW);// new JW, old: pDMS->m_iView
	OnCharacteristicViewAct2->setChecked(g_mainFrame->m_iView==CHARSIMVIEW);// new JW, old: pDMS->m_iView
	
	pDMS->UpdateBlades();
	SetMenus();
	
	g_qdms->configureGL();	
}
////////end new code JW/////


void MainFrame::OnCurFoilStyle()
{
	if(!g_pCurFoil) return;
	
	LinePickerDlg dlg;
	dlg.InitDialog(g_pCurFoil->m_nFoilStyle, g_pCurFoil->m_nFoilWidth, g_pCurFoil->m_FoilColor);
	dlg.move(m_DlgPos);
	
	if(QDialog::Accepted==dlg.exec())
	{
		g_pCurFoil->m_FoilColor  = dlg.GetColor();
		g_pCurFoil->m_nFoilStyle = dlg.GetStyle();
		g_pCurFoil->m_nFoilWidth = dlg.GetWidth();
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->m_BufferFoil.m_FoilColor  = g_pCurFoil->m_FoilColor;
		pXDirect->m_BufferFoil.m_nFoilStyle = g_pCurFoil->m_nFoilStyle;
		pXDirect->m_BufferFoil.m_nFoilWidth = g_pCurFoil->m_nFoilWidth;
		SetSaveState(false);
	}
	m_DlgPos = dlg.pos();
	UpdateView();
}


void MainFrame::OnExportCurGraph()
{
	QGraph *pGraph = NULL;
	
	switch(m_iApp)
	{
	case XFOILANALYSIS:
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pGraph = pXDirect->m_pCurGraph;
		break;
	}
		///new code DM///
	case BEM:
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		pGraph = pBEM->m_pCurGraph;
		
		break;
	}
		///end new code DM///
		
		///new code JW///
	case DMS:
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		pGraph = pDMS->m_pCurGraph;
		break;
	}
		///end new code JW///
	}
	if(!pGraph) return;
	
	QString FileName;
	
	pGraph->GetGraphName(FileName);
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Graph"), m_ExportLastDirName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&m_GraphExportFilter);
	if(!FileName.length()) return;
	
	int pos, type;
	pos = FileName.lastIndexOf("/");
	if(pos>0) m_ExportLastDirName = FileName.left(pos);
	
	QFile XFile(FileName);
	
	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
	pos = FileName.indexOf(".csv");
	if(pos>0) type=2; else type=1;
	
	pGraph->ExportToFile(XFile, type);
}


void MainFrame::OnGuidelines()
{
	QString FileName;
#ifdef Q_WS_MAC
	
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
												  kCFURLPOSIXPathStyle);
	QString bundlePath(CFStringGetCStringPtr(macPath,
											 CFStringGetSystemEncoding()));
	QFileInfo fileInfo(bundlePath);
	QDir dir = fileInfo.dir();
	FileName = dir.absoluteFilePath("Guidelines.pdf");
	CFRelease(appUrlRef);
	CFRelease(macPath);
	
	
#endif
#ifdef Q_WS_WIN
	QDir dir(qApp->applicationDirPath());
	FileName = dir.canonicalPath() + "/Guidelines.pdf" ;
#endif
#ifdef Q_WS_X11
	QDir dir("/usr/share/xflr5");
	FileName = dir.canonicalPath() + "/Guidelines.pdf" ;
#endif
	QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
}

void MainFrame::OnLanguage()
{
	TranslatorDlg dlg;
	QDir TranslationsDir;
	
#ifdef Q_WS_MAC
	TranslationsDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_WS_WIN
	TranslationsDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_WS_X11
	TranslationsDir.setPath("/usr/share/xflr5");
#endif
	
	dlg.m_TranslationDirPath = TranslationsDir.canonicalPath() + "/translations" ;
	dlg.m_LanguageFilePath = m_LanguageFilePath;
	dlg.InitDialog();
	dlg.move(m_DlgPos);
	if(dlg.exec()==QDialog::Accepted)
	{
		m_LanguageFilePath = dlg.m_LanguageFilePath;
	}
	m_DlgPos = dlg.pos();
}





void MainFrame::OnLoadFile()
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot load a Project while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			return;}
	}
	
	QString PathName;
	
	PathName = QFileDialog::getOpenFileName(this, tr("Open Project File"),
											m_LastDirName,
											tr("QBlade Project File (*.wpa)"));
	if(!PathName.length()) return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) m_LastDirName = PathName.left(pos);
	
	loadQBladeProject(PathName);
	
}


void MainFrame::OnLogFile()
{
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	// 20090605 Francesco Meschia
	QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
}

void MainFrame::OnNewProject()
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot clear the Project while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			return;}
	}
	
	if(!m_bSaved)
	{
		int resp = QMessageBox::question(window(), tr("Question"), tr("Save the current project ?"),
										 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		
		if (QMessageBox::Cancel == resp)
		{
			return;
		}
		else if (QMessageBox::Yes == resp)
		{
			if(SaveProject(m_FileName))
			{
				SetSaveState(true);
				statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
			}
			else return; //save failed, don't close
		}
		else if (QMessageBox::No == resp)
		{
			DeleteProject();
		}
	}
	else
	{
		DeleteProject();
	}
	
	UpdateView();
}


void MainFrame::OnResetCurGraphScales()
{
	QGraph *pGraph = NULL;
	switch(m_iApp)
	{
	case XFOILANALYSIS:
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		if(!pXDirect->m_bPolar) pGraph = pXDirect->m_pCpGraph;
		else pGraph = pXDirect->m_pCurGraph;
		break;
	}
	case INVERSEDESIGN:
	{
		QXInverse *pXInverse = (QXInverse*)m_pXInverse;
		pGraph = &pXInverse->m_QGraph;
		pXInverse->ReleaseZoom();
		break;
	}
		////new code DM///////
	case BEM:
	{
		QBEM *pBEM = (QBEM *)m_pBEM;
		pGraph = pBEM->m_pCurGraph;
		break;
	}
		/////////end new code DM////////////
		////new code JW///////
	case DMS:
	{
		QDMS *pDMS = (QDMS *)m_pDMS;
		pGraph = pDMS->m_pCurGraph;
		break;
	}
		//////////end new code JW////////////
	}
	
	if(!pGraph) return;
	
	pGraph->SetAuto(true);
	UpdateView();
}

void MainFrame::OnAutomaticResetCurGraph()
{
    QGraph *pGraph = NULL;
    switch(m_iApp)
    {
    case XFOILANALYSIS:
    {
        QXDirect *pXDirect = (QXDirect*)m_pXDirect;
        if(!pXDirect->m_bPolar) pGraph = pXDirect->m_pCpGraph;
        else pGraph = pXDirect->m_pCurGraph;
        break;
    }
    case INVERSEDESIGN:
    {
        QXInverse *pXInverse = (QXInverse*)m_pXInverse;
        pGraph = &pXInverse->m_QGraph;
        pXInverse->ReleaseZoom();
        break;
    }
        ////new code DM///////
    case BEM:
    {
        QBEM *pBEM = (QBEM *)m_pBEM;
        pGraph = pBEM->m_pCurGraph;
        break;
    }
        /////////end new code DM////////////
        ////new code JW///////
    case DMS:
    {
        QDMS *pDMS = (QDMS *)m_pDMS;
        pGraph = pDMS->m_pCurGraph;
        break;
    }
        //////////end new code JW////////////
    }

    if(!pGraph) return;

    if (pGraph->m_autoReset){
        pGraph->m_autoReset = false;
    }
    else{
        pGraph->m_autoReset = true;
    }
    UpdateView();
}



void MainFrame::OnRenameCurFoil()
{
	if(!g_pCurFoil) return;
	RenameFoil(g_pCurFoil);
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->SetFoil(g_pCurFoil);
	UpdateFoils();
	UpdateView();
}


void MainFrame::OnResetSettings()
{
	int resp = QMessageBox::question(this, tr("Default Settings"), tr("Are you sure you want to reset the default settings ?"),
									 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(resp == QMessageBox::Yes)
	{
		QMessageBox::warning(this,tr("Default Settings"), tr("The settings will be reset at the next session."));
#ifdef Q_WS_MAC
		QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#else
		QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#endif
		settings.clear();
		m_LastDirName = QDir::homePath();
		// do not save on exit
		m_bSaveSettings = false;
	}
}


// new code JW ///
void MainFrame::OnSaveSettings()
{
	int resp = QMessageBox::question(this, tr("Save Current Settings"), tr("Are you sure you want to save the current settings for your next session?"),
									 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(resp == QMessageBox::Yes)
	{
		QMessageBox::warning(this,tr("Save Current Settings"), tr("The current settings will be resumed at the next session."));
		
		m_bSaveSettings = true;
		SaveSettings();
		// do not save on exit
		m_bSaveSettings = false;
	}
}
// end new code JW ///


void MainFrame::OnRestoreToolbars()
{
	if(m_iApp==XFOILANALYSIS)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
		
		m_pctrlAFoilWidget->hide();
		m_pctrlXInverseWidget->hide();
		
		m_pctrlXDirectToolBar->show();
		m_pctrlXDirectWidget->show();
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		
		m_pctrlXDirectWidget->hide();
		m_pctrlXInverseWidget->hide();
		
		m_pctrlAFoilToolBar->show();
		m_pctrlAFoilWidget->show();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		m_pctrlAFoilToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		
		m_pctrlAFoilWidget->hide();
		m_pctrlXDirectWidget->hide();
		
		m_pctrlXInverseToolBar->show();
		m_pctrlXInverseWidget->show();
	}
}


void MainFrame::OnSaveProject()
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot save the Project while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			return;}
	}
	
	if (!m_ProjectName.length() || m_ProjectName=="*")
	{
		OnSaveProjectAs();
		return;
	}
	if(SaveProject(m_FileName))
	{
		SetSaveState(true);
		statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
	}
}



bool MainFrame::OnSaveProjectAs()
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot save the Project while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			return false;}
	}
	if(SaveProject())
	{
		SetProjectName(m_FileName);
		AddRecentFile(m_FileName);
		statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
		SetSaveState(true);
	}
	return true;
}

void MainFrame::OnSaveViewToImageFile()
{
	QSize sz(m_twoDWidget->geometry().width(), m_twoDWidget->geometry().height());
	QImage img(sz, QImage::Format_RGB32);
	QPainter painter(&img);
	QString FileName, Filter;
	switch(m_ImageFormat)
	{
	case 0 :
	{
		Filter = "Portable Network Graphics (*.png)";
		break;
	}
	case 1 :
	{
		Filter = "JPEG (*.jpg)";
		break;
	}
	case 2 :
	{
		Filter = "Windows Bitmap (*.bmp)";
		break;
	}
	}
	
	FileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
											m_ImageDirName,
											"Portable Network Graphics (*.png);;JPEG (*.jpg);;Windows Bitmap (*.bmp)",
											&Filter);
	
	if(!FileName.length()) return;
	
	int pos = FileName.lastIndexOf("/");
	if(pos>0) m_ImageDirName = FileName.left(pos);
	
	if(Filter == "Portable Network Graphics (*.png)")
	{
		if(FileName.right(4)!=".png") FileName+= ".png";
		m_ImageFormat = 0;
	}
	else if(Filter == "JPEG (*.jpg)")
	{
		if(FileName.right(4)!=".jpg") FileName+= ".jpg";
		m_ImageFormat = 1;
	}
	else if(Filter == "Windows Bitmap (*.bmp)")
	{
		if(FileName.right(4)!=".bmp") FileName+= ".bmp";
		m_ImageFormat = 2;
	}
	
	switch(m_iApp)
	{
	case XFOILANALYSIS:
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->PaintView(painter);
		break;
	}
	case DIRECTDESIGN:
	{
		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->PaintView(painter);
		break;
	}
	case INVERSEDESIGN:
	{
		QXInverse *pXInverse = (QXInverse*)m_pXInverse;
		pXInverse->PaintView(painter);
		break;
	}
		////////////new code DM//////////
	case BEM:
	{ 
		QBEM *pBEM = (QBEM*) m_pBEM;
		if(g_mainFrame->m_iView==BLADEVIEW)// new JW, old: pBEM->m_iView
		{
			pBEM->UpdateView();
			pBEM->SnapClient(FileName);
			return;
		}
		else
		{
			pBEM->PaintView(painter);
			break;
		}
	}
	case QFEMMODULE:
	{
		if(g_mainFrame->m_iView==QFEMSTRUCTVIEW || g_mainFrame->m_iView==QFEMLOADINGVIEW)// new JW, old: pBEM->m_iView
		{
			g_qbem->UpdateView();
			g_qbem->SnapClient(FileName);
			return;
		}
		else
		{
			g_QFEMModule->m_graph[0]->drawGraph(painter);
			g_QFEMModule->m_graph[1]->drawGraph(painter);
			g_QFEMModule->m_graph[2]->drawGraph(painter);
			g_QFEMModule->m_graph[3]->drawGraph(painter);
			break;
		}
	}
		
	case WINDFIELDMODULE:
	{
		g_qbem->UpdateView();
		g_qbem->SnapClient(FileName);
		return;
	}
		
	case FASTMODULE:
	{
		g_fastModule->m_graph[0]->drawGraph(painter);
		g_fastModule->m_graph[1]->drawGraph(painter);
		g_fastModule->m_graph[2]->drawGraph(painter);
		g_fastModule->m_graph[3]->drawGraph(painter);
		break;
	}
		//////////end new code DM/////////
		////////////new code JW//////////
	case DMS:
	{
		QDMS *pDMS = (QDMS*) m_pDMS;
		if(g_mainFrame->m_iView==BLADEVIEW)// new JW, old: pDMS->m_iView
		{
			pDMS->UpdateView();
			pDMS->SnapClient(FileName);
			return;
		}
		else
		{
			pDMS->PaintView(painter);
			break;
		}
	}
		//////////end new code JW/////////
	}
	img.save(FileName);
}

void MainFrame::OnSelChangeFoil(int /*i*/)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	// Gets the new selected foil name and notifies XDirect
	// sets a polar, if any
	// then updates the polar combobox
	
	g_pCurFoil = m_pctrlFoil->currentObject();
	pXDirect->SetFoil(g_pCurFoil);
	pXDirect->SetPolar();
	m_iApp = XFOILANALYSIS;
	UpdatePolars();
	pXDirect->SetControls();
	UpdateView();
}


void MainFrame::OnSelChangePolar(int /*i*/)
{
	
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	// Gets the new selected polar name and notifies XDirect
	
	m_iApp = XFOILANALYSIS;
	pXDirect->SetPolar(m_pctrlPolar->currentObject());
	UpdateOpps();
	pXDirect->SetControls();
	UpdateView();
}


void MainFrame::OnSelChangeOpp(int /*i*/)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	// Gets the new selected Opp name and notifies XDirect
	
	pXDirect->SetOpp(m_pctrlOpPoint->currentObject());
	UpdateView();
}




void MainFrame::OnStyle()
{
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	
	m_DisplaySettingsDlg.m_pMainFrame = this;
	m_DisplaySettingsDlg.move(m_DlgPos);
	m_DisplaySettingsDlg.m_BackgroundColor = m_BackgroundColor;
	m_DisplaySettingsDlg.m_TextColor       = m_TextColor;
	m_DisplaySettingsDlg.m_TextFont        = m_TextFont;
	m_DisplaySettingsDlg.m_pRefGraph       = &m_RefGraph;
	m_DisplaySettingsDlg.m_bReverseZoom    = m_bReverseZoom;
	m_DisplaySettingsDlg.m_bAlphaChannel   = m_bAlphaChannel;
	m_DisplaySettingsDlg.InitDialog();
	
	if(m_DisplaySettingsDlg.exec() ==QDialog::Accepted)
	{
		m_BackgroundColor = m_DisplaySettingsDlg.m_BackgroundColor;
		m_TextColor       = m_DisplaySettingsDlg.m_TextColor;
		m_TextFont        = m_DisplaySettingsDlg.m_TextFont;
		m_bReverseZoom    = m_DisplaySettingsDlg.m_pctrlReverseZoom->isChecked();
		m_bAlphaChannel   = m_DisplaySettingsDlg.m_pctrlAlphaChannel->isChecked();
		
		if(m_DisplaySettingsDlg.m_bIsGraphModified)
		{
			SetGraphSettings(&m_RefGraph);
		}
	}
	m_DlgPos = m_DisplaySettingsDlg.pos();
	
	pXDirect->m_pCpGraph->SetInverted(true);
	UpdateView();
}


void MainFrame::OnUnits()
{
	m_UnitsDlg.move(m_DlgPos);
	m_UnitsDlg.m_Length = m_LengthUnit;
	m_UnitsDlg.m_Area   = m_AreaUnit;
	m_UnitsDlg.m_Weight = m_WeightUnit;
	m_UnitsDlg.m_Speed  = m_SpeedUnit;
	m_UnitsDlg.m_Force  = m_ForceUnit;
	m_UnitsDlg.m_Moment = m_MomentUnit;
	/////////new code DM//////////
	m_UnitsDlg.m_Power = m_PowerUnit;
	////////////end new code DM/////////
	m_UnitsDlg.InitDialog();
	
	if(m_UnitsDlg.exec()==QDialog::Accepted)
	{
		m_LengthUnit = m_UnitsDlg.m_Length;
		m_AreaUnit   = m_UnitsDlg.m_Area;
		m_WeightUnit = m_UnitsDlg.m_Weight;
		m_SpeedUnit  = m_UnitsDlg.m_Speed;
		m_ForceUnit  = m_UnitsDlg.m_Force;
		m_MomentUnit = m_UnitsDlg.m_Moment;
		/////////new code DM//////////
		m_PowerUnit = m_UnitsDlg.m_Power;
		////////////end new code DM/////////
		
		SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit, m_PowerUnit,
				 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit, m_WtoUnit);
		
		SetSaveState(false);
		
		///////////new code DM/////////
		if(m_iApp==BEM)
		{
			QBEM *pBEM= (QBEM*)m_pBEM;
			pBEM->UpdateUnits();
		}
		////////////end new code DM////////
		
		///////////new code JW/////////
		if(m_iApp==DMS)
		{
			QDMS *pDMS= (QDMS*)m_pDMS;
			pDMS->UpdateUnits();
		}
		////////////end new code JW////////
		
		emit unitsChanged();
	}
	m_DlgPos = m_UnitsDlg.pos();
}


void MainFrame::OnXDirect()
{
	UpdateFoils();
	
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->SetFoilScale();
	setIApp(XFOILANALYSIS);  // NM vorher: m_iApp = XFOILANALYSIS;
	
	m_pctrlAFoilToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlXDirectToolBar->show();
	
	m_pctrlAFoilWidget->hide();
	m_pctrlXInverseWidget->hide();
	m_pctrlXDirectWidget->show();
	
	////////////new code DM/////////////
	OnAFoilAct->setChecked(false);
	OnXDirectAct->setChecked(true);
	OnXInverseAct->setChecked(false);
	////////end new code DM//////////
	
	SetCentralWidget();
	SetMenus();
	pXDirect->SetPolarLegendPos();
	pXDirect->SetControls();
	pXDirect->UpdateView();
}


void MainFrame::OnXInverse()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	//	pXInverse->SetScale();
	setIApp(INVERSEDESIGN);  // NM vorher: m_iApp = INVERSEDESIGN;
	
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->show();
	
	m_pctrlAFoilWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->show();
	
	////////////new code DM/////////////
	OnAFoilAct->setChecked(false);
	OnXDirectAct->setChecked(false);
	OnXInverseAct->setChecked(true);
	////////end new code DM//////////
	
	SetCentralWidget();
	//	pXInverse->m_bFullInverse = true;
	SetMenus();
	pXInverse->SetParams();
	pXInverse->UpdateView();
}


void MainFrame::OnXInverseMixed()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	//	pXInverse->SetScale();
	setIApp(INVERSEDESIGN);  // NM vorher: m_iApp = INVERSEDESIGN;
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->show();
	
	m_pctrlAFoilWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->show();
	
	pXInverse->m_bFullInverse = false;
	SetCentralWidget();
	SetMenus();
	pXInverse->SetParams();
	pXInverse->UpdateView();
}


void MainFrame::openRecentFile()
{
	if (g_QLLTModule->GetQLLTSim()){
		if (g_QLLTModule->GetQLLTSim()->isRunning() || g_QLLTModule->GetQLLTToolBar()->isReplayRunning()){
			QMessageBox::critical(this, tr("Warning"), QString(tr("Cannot load a Project while a LLT simulation or animation is running\n- Please stop the simulation or animation before closing!!!")), QMessageBox::Ok);
			return;}
	}
	
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action) return;
	
	int App = loadQBladeProject(action->data().toString());
	
	if(App==0)
	{
		m_iApp = App;
		QString FileName = action->data().toString();
		m_RecentFiles.removeAll(FileName);
		updateRecentFileActions();
	}
	
}


CFoil* MainFrame::ReadFoilFile(QTextStream &in)
{
	QString Strong;
	QString tempStr;
	QString FoilName;
	
	CFoil* pFoil = NULL;
	int pos, res, i, ip;
	pos = 0;
	double x,y, z,area;
	bool bRead;
	
	
	pFoil = new CFoil();
	if(!pFoil)	return NULL;
	
	while(tempStr.length()==0 && !in.atEnd())
	{
		FoilName = Strong;
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);
		// ignore everything after # (including #)
		if(pos>0)Strong.truncate(pos);
		tempStr = Strong;
		tempStr.remove(" ");
	}
	
	if(!in.atEnd())
	{
		// FoilName contains the last comment
		ReadValues(Strong, res,x,y,z);
		if(res==2)
		{
			//there isn't a name on the first line
			FoilName = "New Foil";
			{
				pFoil->xb[0] = x;
				pFoil->yb[0] = y;
				pFoil->nb=1;
			}
		}
		else FoilName = Strong;
		// remove fore and aft spaces
		FoilName = FoilName.trimmed();
	}
	
	bRead = true;
	do
	{
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);
		// ignore everything after # (including #)
		if(pos>0)Strong.truncate(pos);
		tempStr = Strong;
		tempStr.remove(" ");
		if (!Strong.isNull() && bRead && tempStr.length())
		{
			ReadValues(Strong, res, x,y,z);
			if(res==2)
			{
				pFoil->xb[pFoil->nb] = x;
				pFoil->yb[pFoil->nb] = y;
				pFoil->nb++;
				if(pFoil->nb>IQX)
				{
					QMessageBox::warning(this, tr("Warning"),tr("The maximum for foil coordinates is 302, cant continue to read the foil"));
					delete pFoil;
					return NULL;
				}
			}
			else bRead = false;
		}
	}while (bRead && !Strong.isNull());
	
	pFoil->setName(FoilName);
	
	// Check if the foil was written clockwise or counter-clockwise
	
	area = 0.0;
	for (i=0; i<pFoil->nb; i++)
	{
		if(i==pFoil->nb-1)	ip = 0;
		else				ip = i+1;
		area +=  0.5*(pFoil->yb[i]+pFoil->yb[ip])*(pFoil->xb[i]-pFoil->xb[ip]);
	}
	
	if(area < 0.0)
	{
		//reverse the points order
		double xtmp, ytmp;
		for (int i=0; i<pFoil->nb/2; i++)
		{
			xtmp         = pFoil->xb[i];
			ytmp         = pFoil->yb[i];
			pFoil->xb[i] = pFoil->xb[pFoil->nb-i-1];
			pFoil->yb[i] = pFoil->yb[pFoil->nb-i-1];
			pFoil->xb[pFoil->nb-i-1] = xtmp;
			pFoil->yb[pFoil->nb-i-1] = ytmp;
		}
	}
	
	memcpy(pFoil->x, pFoil->xb, sizeof(pFoil->xb));
	memcpy(pFoil->y, pFoil->yb, sizeof(pFoil->yb));
	pFoil->n=pFoil->nb;
	
	pFoil->m_FoilColor = GetColor(0);
	if (!pFoil->InitFoil()) return NULL;
	
	return pFoil;
}

void MainFrame::RemoveOpPoint()
{
	
	QXDirect *pXDirect =(QXDirect*)m_pXDirect;
	
	g_oppointStore.remove(m_pctrlOpPoint->currentObject());
	
	pXDirect->m_pCurOpp = m_pctrlOpPoint->currentObject();
	
	pXDirect->SetControls();
}




void MainFrame::RenameFoil(CFoil *pFoil)
{
	if(!pFoil) return;
	
	QString strong;
	int i;
	QString OldName = pFoil->getName();
	
	g_foilStore.rename(pFoil,pFoil->getName());
	
	strong = pFoil->getName();
	
	for (i=0; i<g_rotorStore.size(); i++)
	{
		for (int j=0; j<g_rotorStore.at(i)->m_Airfoils.size();j++)
		{
			if (g_rotorStore.at(i)->m_Airfoils.at(j) == OldName) g_rotorStore.at(i)->m_Airfoils[j] = strong;
		}
		for (int j=0; j<g_rotorStore.at(i)->m_PolarAssociatedFoils.size();j++)
		{
			if (g_rotorStore.at(i)->m_PolarAssociatedFoils.at(j) == OldName) g_rotorStore.at(i)->m_PolarAssociatedFoils[j] = strong;
		}
		
	}
	
	for (i=0; i<g_verticalRotorStore.size(); i++)
	{
		for (int j=0; j<g_verticalRotorStore.at(i)->m_Airfoils.size();j++)
		{
			if (g_verticalRotorStore.at(i)->m_Airfoils.at(j) == OldName) g_verticalRotorStore.at(i)->m_Airfoils[j] = strong;
		}
		for (int j=0; j<g_verticalRotorStore.at(i)->m_PolarAssociatedFoils.size();j++)
		{
			if (g_verticalRotorStore.at(i)->m_PolarAssociatedFoils.at(j) == OldName) g_verticalRotorStore.at(i)->m_PolarAssociatedFoils[j] = strong;
		}
	}
	
	for (i=0; i<g_oppointStore.size(); i++)
	{
		if(g_oppointStore.at(i)->m_strFoilName == OldName)
		{
			g_oppointStore.at(i)->m_strFoilName = strong;
		}
	}
	SetSaveState(false);
	
}



bool MainFrame::SaveProject(QString PathName)
{
	QString Filter = "QBlade Project File (*.wpa)";
	QString FileName = m_ProjectName;
	
	if(!PathName.length())
	{
		if(FileName.right(1)=="*") 	FileName = FileName.left(FileName.length()-1);
		FileName.replace("/", " ");
		
		
		PathName = QFileDialog::getSaveFileName(this, tr("Save the Project File"),
												m_LastDirName+"/"+FileName,
												tr("QBlade Project File (*.wpa)"),
												&Filter);
		
		if(!PathName.length()) return false;//nothing more to do
		int pos = PathName.indexOf(".wpa", Qt::CaseInsensitive);
		if(pos<0) PathName += ".wpa";
		pos = PathName.lastIndexOf("/");
		if(pos>0) m_LastDirName = PathName.left(pos);
	}
	
	QFile fp(PathName);
	
	if (!fp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
		return false;
	}
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QDataStream ar(&fp);
	ar.setVersion(QDataStream::Qt_4_5);
	ar.setByteOrder(QDataStream::LittleEndian);
	
	try {
		SerializeProject(ar,true);
	} catch (Serializer::Exception &error) {
		QMessageBox::critical(this, "Serialization error", error.message);
	}
	m_FileName = PathName;
	fp.close();
	
	SaveSettings();
	QApplication::restoreOverrideCursor();
	return true;
}


void MainFrame::SaveSettings()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	/////////////////////////new code DM//////////////////
	QBEM *pBEM = (QBEM *) m_pBEM;
	/////////////////////////new code DM//////////////////
	/////////////////////////new code JW//////////////////
	QDMS *pDMS = (QDMS *) m_pDMS;
	/////////////////////////new code JW//////////////////
	
	if(!m_bSaveSettings) return;
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"QBLADE");
#endif
	settings.beginGroup("MainFrame");
	{
		settings.setValue("SettingsFormat", SETTINGSFORMAT);
		settings.setValue("FrameGeometryx", frameGeometry().x());
		settings.setValue("FrameGeometryy", frameGeometry().y());
		settings.setValue("SizeWidth", size().width());
		settings.setValue("SizeHeight", size().height());
		settings.setValue("SizeMaximized", isMaximized());
		settings.setValue("GraphExportFilter", m_GraphExportFilter);
		settings.setValue("LanguageFilePath", m_LanguageFilePath);
		settings.setValue("XDirect_Float", m_pctrlXDirectWidget->isFloating());
		settings.setValue("AFoil_Float", m_pctrlAFoilWidget->isFloating());
		settings.setValue("XInverse_Float", m_pctrlXInverseWidget->isFloating());
		settings.setValue("XDirect_x", m_pctrlXDirectWidget->frameGeometry().x());
		settings.setValue("XDirect_y", m_pctrlXDirectWidget->frameGeometry().y());
		settings.setValue("AFoil_x", m_pctrlAFoilWidget->frameGeometry().x());
		settings.setValue("AFoil_y", m_pctrlAFoilWidget->frameGeometry().y());
		settings.setValue("XInverse_x", m_pctrlXInverseWidget->frameGeometry().x());
		settings.setValue("XInverse_y", m_pctrlXInverseWidget->frameGeometry().y());
		settings.setValue("XDirectSize", m_pctrlXDirectWidget->size());
		settings.setValue("AFoilSize", m_pctrlAFoilWidget->size());
		settings.setValue("XInverseSize", m_pctrlXInverseWidget->size());
		/////////////////////////new code DM//////////////////
		settings.setValue("XBEM_x", m_pctrlBEMWidget->width());
		settings.setValue("XBEM_y", m_pctrlBEMWidget->height());
		settings.setValue("XBEM_Float", m_pctrlBEMWidget->isFloating());
		settings.setValue("Simu_x", m_pctrlSimuWidget->frameGeometry().x());
		settings.setValue("Simu_y", m_pctrlSimuWidget->frameGeometry().y());
		settings.setValue("SimuFloat", m_pctrlSimuWidget->isFloating());
		settings.setValue("powerunit", m_PowerUnit);
		/////////////end new code DM////////////
		/////////////////////////new code JW//////////////////
		settings.setValue("XDMSSize", m_pctrlDMSWidget->size());
		settings.setValue("XDMS_x", m_pctrlDMSWidget->width());
		settings.setValue("XDMS_y", m_pctrlDMSWidget->height());
		settings.setValue("XDMS_Float", m_pctrlDMSWidget->isFloating());
		settings.setValue("SimuDMSSize", m_pctrlSimuWidgetDMS->size());
		settings.setValue("SimuDMS_x", m_pctrlSimuWidgetDMS->frameGeometry().x());
		settings.setValue("SimuDMS_y", m_pctrlSimuWidgetDMS->frameGeometry().y());
		settings.setValue("SimuDMSFloat", m_pctrlSimuWidgetDMS->isFloating());
		/////////////end new code JW////////////
		settings.setValue("LastDirName", m_LastDirName);
		settings.setValue("ImageDirName", m_ImageDirName);
		settings.setValue("ExportLastDirName", m_ExportLastDirName);
		settings.setValue("LengthUnit", m_LengthUnit);
		settings.setValue("AreaUnit", m_AreaUnit);
		settings.setValue("WeightUnit", m_WeightUnit);
		settings.setValue("SpeedUnit", m_SpeedUnit);
		settings.setValue("ForceUnit", m_ForceUnit);
		settings.setValue("MomentUnit", m_MomentUnit);
		settings.setValue("BackgroundColorRed", m_BackgroundColor.red());
		settings.setValue("BackgroundColorGreen", m_BackgroundColor.green());
		settings.setValue("BackgroundColorBlue", m_BackgroundColor.blue());
		settings.setValue("TextColorRed", m_TextColor.red());
		settings.setValue("TextColorGreen", m_TextColor.green());
		settings.setValue("TextColorBlue", m_TextColor.blue());
		settings.setValue("TextFontFamily", m_TextFont.family());
		settings.setValue("TextFontPointSize", m_TextFont.pointSize());
		settings.setValue("ImageFormat", m_ImageFormat);
		settings.setValue("DlgPos_x", m_DlgPos.x());
		settings.setValue("DlgPos_y", m_DlgPos.y());
		settings.setValue("RecentFileSize", m_RecentFiles.size());
		settings.setValue("ReverseZoom", m_bReverseZoom);
		settings.setValue("AlphaChannel", m_bAlphaChannel);
		settings.setValue("OPENCL_WORK_GROUPS", g_QLLTModule->getGroupSize());
		
		QString RecentF;
		for(int i=0; i<m_RecentFiles.size() && i<MAXRECENTFILES; i++)
		{
			RecentF = QString("RecentFile_%1").arg(i);
			if(m_RecentFiles[i].length()) settings.setValue(RecentF, m_RecentFiles.at(i));
			else                          settings.setValue(RecentF, "");
		}
		for(int i=m_RecentFiles.size(); i<MAXRECENTFILES; i++)
		{
			RecentF = QString("RecentFile_%1").arg(i);
			settings.setValue(RecentF, "");
		}
	}
	settings.endGroup();
	
	
	pAFoil->SaveSettings(&settings);
	pXDirect->SaveSettings(&settings);
	pXInverse->SaveSettings(&settings);
	//////////////new code DM////////////
	pBEM->SaveSettings(&settings);
	//////////////end new code DM///////////
	//////////////new code JW////////////
	pDMS->SaveSettings(&settings);
	//////////////end new code JW///////////
	m_RefGraph.SaveSettings(&settings);
}


void MainFrame::SetCentralWidget()
{
	////////////////new code DM//////////////
	if(m_iApp==BEM && g_mainFrame->m_iView == BLADEVIEW)// new JW, old: pBEM->m_iView
	{
		switchToGlWidget();
	}
	// new JW, old: pBEM->m_iView
	else if(m_iApp == BEM && (g_mainFrame->m_iView == POLARVIEW || g_mainFrame->m_iView == BEMSIMVIEW ||  g_mainFrame->m_iView == CHARSIMVIEW ||  g_mainFrame->m_iView == TURBINEVIEW))
	{
		switchToTwoDWidget();
	}
	/////////////end new code DM/////////
	
	////////////////new code JW//////////////
	else if (m_iApp==DMS && g_mainFrame->m_iView == BLADEVIEW)// new JW, old: pDMS->m_iView
	{
		switchToGlWidget();
	}
	// new JW, old: pDMS->m_iView
	else if(m_iApp == DMS && (g_mainFrame->m_iView == POLARVIEW || g_mainFrame->m_iView == BEMSIMVIEW ||  g_mainFrame->m_iView == CHARSIMVIEW ||  g_mainFrame->m_iView == TURBINEVIEW))
	{
		switchToTwoDWidget();
	}
	/////////////end new code JW/////////
	else
	{
		switchToTwoDWidget();
	}
}



void MainFrame::SelectOpPoint(OpPoint *pOpp)
{
	//Selects pOpp in the combobox and returns true
	//On error, selects the first and returns false
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	CPolar *pCurPlr    = pXDirect->m_pCurPolar;
	if(!pOpp || !pCurPlr) return;
	
	double alpha, Re;
	
	for(int i=0; i<m_pctrlOpPoint->count(); i++)
	{
		if(pCurPlr->m_PolarType != FIXEDAOAPOLAR)
		{
			alpha = m_pctrlOpPoint->itemText(i).toDouble();
			if(fabs(alpha-pOpp->Alpha)<0.001)
			{
				m_pctrlOpPoint->setCurrentIndex(i);
			}
		}
		else
		{
			Re = m_pctrlOpPoint->itemText(i).toDouble();
			if(fabs(Re-pOpp->Reynolds)<1.0)
			{
				m_pctrlOpPoint->setCurrentIndex(i);
			}
		}
	}
}

void MainFrame::SerializeProject(QDataStream &ar, bool isStoring) {
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
    const int thisVersion = VERSIONNUMBER;
	
	g_serializer.setDataStream(&ar);
	if (isStoring) {
		g_serializer.setMode(Serializer::WRITE);
        g_serializer.setArchiveFormat(thisVersion);  // added serialization for refHeight
		g_serializer.writeInt(g_serializer.getArchiveFormat());
		g_serializer.writeInt(11229944);  // used to determine if the file actually is a QBlade file
		
		/* older formats:
         * 100054 : changed serialization of dummylines
         * 100053 : added new float vector class
         * 100052 : DMSData serializes ShowAsGraphInterface data
         * 100051 : made vortex strain an option in qlltsimulations
		 * 100050 : added pitch axis to struts
         * 100049 : added power law shear type to QLLTSimulation and blade vortex serialization
         * 100048 : added multi polar definition to VAWT struts
         * 100047 : redordered LLT output variables; nees reordering in serialize of older project
         * 100046 : added code number 11229944 to check if file is a QBlade file
		 * 100045 : added wake counter to switch between different LLW wake counting schemes
		 * 100044 : LLT output variables now stored for all blades
		 * 100043 : added "WakeCoarsening" option to QLLTSimulations
		 * 100042 : removed redundancy from opPoint and added NoiseSimulation
		 * 100041 : added flag to OpPoint whether a noise simulation is possible
		 * 100040 : added some more values to OpPoint needed for noise calculation
		 * 100039 : added vortex stretch factor
		 * 100038 : added core size as a LLT output
		 * 100037 : added LLT EvalPoint
		 * 100036 : added adaptive timestepping for LLT
		 * 100035 : additional data for OpPoint that formerly was thrown away
		 * 100034 : added bound vortex and aoaposition to qlltsimulation object
		 * 100033 : decomposition of 360Polar & dynamic stall variables
		 * 100032 : added windspeed to DMSData
		 * 100031 : Added azistart, rotor mom. of inertia, isStartup, initialrot
		 * 100030 : Added AR Correction for LLTSimulations of VAWT
		 * 100029 : QBlade v0.91 added averaged values for VAWT LLT
		 * 100028 : David: QBlade v0.91 windfield data now stored in vectors
		 * 100027 : QBlade v0.91 some parameters added to various classes
		 * 100026 : QBlade v0.9
		 * 100025 : NM entirely changed behaviour. Introduced Serializer. Added WindField.
		 *
		 * --- cut downward compatibility in 100046
		 *
		 * 100024 : JW modification
		 * 100024 : (new code JW) Weibull functionality changed
		 * 100023 : (new code JW) VAWT Characteristics Simulations included
		 * 100022 : (new code JW) VAWT simulation included
		 * 100021 : (new code DM) Reynolds Drag Correction Added
		 * 100020 : (new code DM) Prescribed Pitch and Rotspeed arrays now saved in TData
		 * 100019 : (new code DM) Prescribed Pitch and Rotspeed added to TData
		 * 100018 : (new code DM) Characteristics Simulations Included
		 * 100017 : (new code DM) New Cm Coefficient
		 * 100016 : (new code DM) New 360Polar parameters
		 * 100015 : (new code DM) Fixed Pitch included
		 * 
		 * --- cut downward compatibility in 100030
		 * 
		 * 100014 : Removed Point Foil
		 * 100013 : Added CoG serialization
		 * 100012 : Added sideslip
		 * 100011 : Added Body serialization
		 * 100010 : Converted to I.S. units
		 * 100009 : added serialization of opps in numbered format
		 * 100008 : Added m_WngAnalysis.m_bTiltedGeom, m_WngAnalysis.m_bWakeRollUp
		 * 100006 : version 2.99Beta001 format
		 * 100005 : version 2.00 format
		 * Archive format introduced in Beta V22
		 * Write units //added in Beta v16
		 * Save analysis settings // added in Beta v22
		 * */
	} else {
		g_serializer.setMode(Serializer::READ);
		g_serializer.setArchiveFormat(g_serializer.readInt());
		
		if (g_serializer.getArchiveFormat() >= 100046 &&
			g_serializer.readInt() != 11229944)  // used to determine if the file actually is a QBlade file
		{
			throw Serializer::Exception ("The loaded file is not a QBlade file!");
		}
		
		if (g_serializer.getArchiveFormat() > thisVersion) {
			throw Serializer::Exception ("The loaded file uses a newer file version! "
										 "Please get the latest release of QBlade.");
		}
	}
	
	if (g_serializer.getArchiveFormat() < 100025) {  // cut downward compatibility
		throw Serializer::Exception (QString("The loaded project file version is too old: %1").arg(
										 g_serializer.getArchiveFormat()));
	}
	
	if (g_serializer.getArchiveFormat() >= 100025) {
		g_serializer.readOrWriteInt (&m_LengthUnit);
		g_serializer.readOrWriteInt (&m_AreaUnit);
		g_serializer.readOrWriteInt (&m_WeightUnit);
		g_serializer.readOrWriteInt (&m_SpeedUnit);
		g_serializer.readOrWriteInt (&m_ForceUnit);
		g_serializer.readOrWriteInt (&m_MomentUnit);
		g_serializer.readOrWriteInt (&m_PowerUnit);

		/* read or write the store contents here */
		g_foilStore.serializeContent();
		g_polarStore.serializeContent();
		g_360PolarStore.serializeContent();
		g_rotorStore.serializeContent();
		g_bemdataStore.serializeContent();
		g_tbemdataStore.serializeContent();
		g_cbemdataStore.serializeContent();
		g_tdataStore.serializeContent();
		g_verticalRotorStore.serializeContent();
		g_dmsdataStore.serializeContent();
		g_tdmsdataStore.serializeContent();
		g_cdmsdataStore.serializeContent();
		g_verttdataStore.serializeContent();
		g_windFieldStore.serializeContent();
		g_FASTSimulationStore.serializeContent();
		g_bladeStructureStore.serializeContent();
		g_oppointStore.serializeContent();
		g_bladestructureloadingStore.serializeContent();
		g_QLLTHAWTSimulationStore.serializeContent();
		g_QLLTVAWTSimulationStore.serializeContent();
		g_QLLTCutPlaneStore.serializeContent();
        g_noiseSimulationStore.serializeContent();
        g_StrutStore.serializeContent();
		
		if (g_serializer.isReadMode()) {
			qDebug() << "Loading the current file version: " << g_serializer.getArchiveFormat();
			
			g_serializer.restoreAllPointers();
			
			g_foilStore.emitObjectListChanged(true);
			g_polarStore.emitObjectListChanged(true);
			g_360PolarStore.emitObjectListChanged(true);
            g_StrutStore.emitObjectListChanged(true);
			g_rotorStore.emitObjectListChanged(true);
			g_bemdataStore.emitObjectListChanged(true);
			g_tbemdataStore.emitObjectListChanged(true);
			g_cbemdataStore.emitObjectListChanged(true);
			g_tdataStore.emitObjectListChanged(true);
			g_verticalRotorStore.emitObjectListChanged(true);
			g_dmsdataStore.emitObjectListChanged(true);
			g_tdmsdataStore.emitObjectListChanged(true);
			g_cdmsdataStore.emitObjectListChanged(true);
			g_verttdataStore.emitObjectListChanged(true);
			g_windFieldStore.emitObjectListChanged(true);
			g_FASTSimulationStore.emitObjectListChanged(true);
			g_bladeStructureStore.emitObjectListChanged(true);
			g_oppointStore.emitObjectListChanged(true);
			g_bladestructureloadingStore.emitObjectListChanged(true);
			g_QLLTHAWTSimulationStore.emitObjectListChanged(true);
			g_QLLTVAWTSimulationStore.emitObjectListChanged(true);
			g_QLLTCutPlaneStore.emitObjectListChanged(true);
			g_noiseSimulationStore.emitObjectListChanged(true);

			
			SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit, m_PowerUnit,
					 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit, m_WtoUnit);
		}
	}	
	
	g_serializer.setDataStream(NULL);
	
	QApplication::restoreOverrideCursor();
}



void MainFrame::SetCurrentFoil(CFoil* pFoil)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil   *pAFoil = (QAFoil*)m_pAFoil;
	pXDirect->SetFoil(pFoil);
	pAFoil->SelectFoil(pFoil);
	g_pCurFoil = pFoil;
    g_qbem->UpdateFoils();
    if (pFoil) m_pctrlFoil->setCurrentObject(pFoil);
}

void MainFrame::SetMenus()
{
	menuBar()->clear();
	menuBar()->addMenu(fileMenu);
	
	// module specific menus
	
	/////////////// new code NM ///////////////
	if (m_currentModule)
	{
		m_currentModule->addMainMenuEntries();
	}
	/////////// end new code NM ///////////////
	else if(m_iApp==XFOILANALYSIS)
	{
		menuBar()->addMenu(XDirectViewMenu);
		menuBar()->addMenu(FoilMenu);
		menuBar()->addMenu(DesignMenu);
		menuBar()->addMenu(XFoilAnalysisMenu);
		menuBar()->addMenu(PolarMenu);
		menuBar()->addMenu(OpPointMenu);
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		menuBar()->addMenu(XInverseViewMenu);
		menuBar()->addMenu(InverseFoilMenu);
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		menuBar()->addMenu(AFoilViewMenu);
		menuBar()->addMenu(AFoilDesignMenu);
		menuBar()->addMenu(AFoilSplineMenu);
	}
	//////////////////////new code DM///////////////////////////
	else if(m_iApp== BEM)
	{
		menuBar()->addMenu(BEMViewMenu);
		if (g_mainFrame->m_iView==POLARVIEW)
			menuBar()->addMenu(BEM360PolarMenu);// JW modification // new JW, old: pBEM->m_iView
		if (g_mainFrame->m_iView==BLADEVIEW||g_mainFrame->m_iView==CHARSIMVIEW||g_mainFrame->m_iView==BEMSIMVIEW)
			menuBar()->addMenu(BEMBladeMenu);// JW modification // new JW, old: pBEM->m_iView
		if (g_mainFrame->m_iView==TURBINEVIEW)
			menuBar()->addMenu(BEMTurbine);// JW modification // new JW, old: pBEM->m_iView
	}
	/////////////////////////end new code DM///////////////////
	
	//////////////////////new code JW///////////////////////////
	else if(m_iApp== DMS)
	{
		menuBar()->addMenu(BEMViewMenu);
		//menuBar()->addMenu(BEMBladeMenu);
		//menuBar()->addMenu(BEM360PolarMenu);
		//menuBar()->addMenu(BEMTurbine);
		if (g_mainFrame->m_iView==BLADEVIEW||g_mainFrame->m_iView==CHARSIMVIEW||g_mainFrame->m_iView==BEMSIMVIEW)
			menuBar()->addMenu(BEMBladeMenu);// JW modification// new JW, old: pDMS->m_iView
		if (g_mainFrame->m_iView==TURBINEVIEW)
			menuBar()->addMenu(BEMTurbine);// JW modification// new JW, old: pDMS->m_iView
	}
	/////////////////////////end new code JW///////////////////
	
	menuBar()->addMenu(optionsMenu);
	menuBar()->addMenu(helpMenu);
}



CFoil* MainFrame::StoreFoil(CFoil* pNewFoil)
{
	// Adds the buffer foil to the Store,
	// and initializes XFoil, comboboxes and everything.
	
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	
	if (!g_foilStore.add(pNewFoil)) pNewFoil = NULL;
    else m_pctrlFoil->setCurrentObject(pNewFoil);

	if (pNewFoil) pNewFoil->InitFoil();
	pAFoil->SelectFoil(pNewFoil);
	pXDirect->SetFoil(pNewFoil);
	g_pCurFoil = pNewFoil;

    g_qbem->UpdateFoils();
    g_qbem->UpdateBlades();
    g_qbem->UpdateTurbines();

    g_qdms->UpdateBlades();
    g_qdms->UpdateTurbines();
	


	return pNewFoil;// foil added
}


void MainFrame::SetProjectName(QString PathName)
{
	m_FileName = PathName;
	int pos = PathName.lastIndexOf("/");
	if (pos>0) m_ProjectName = PathName.right(PathName.length()-pos-1);
	else m_ProjectName = PathName;
	if(m_ProjectName.length()>4)
	{
		m_ProjectName = m_ProjectName.left(m_ProjectName.length()-4);
		m_pctrlProjectName->setText(m_ProjectName);
	}
}


void MainFrame::SetSaveState(bool bSave)
{
	m_bSaved = bSave;
	
	int len = m_ProjectName.length();
	if(m_ProjectName.right(1)=="*") m_ProjectName = m_ProjectName.left(len-1);
	if (!bSave)
	{
		m_ProjectName += "*";
	}
	m_pctrlProjectName->setText(m_ProjectName);
}

void MainFrame::SetSaveStateFalse()
{
	m_bSaved = false;
	
	int len = m_ProjectName.length();
	if(m_ProjectName.right(1)=="*") m_ProjectName = m_ProjectName.left(len-1);
	
	m_ProjectName += "*";
	
	m_pctrlProjectName->setText(m_ProjectName);
}

void MainFrame::SetGraphSettings(Graph *pGraph)
{
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	
	pXDirect->m_pPolarGraph->CopySettings(pGraph, false);
	pXDirect->m_pCpGraph->CopySettings(pGraph, false);
	pXDirect->m_pCmGraph->CopySettings(pGraph, false);
	pXDirect->m_pCzGraph->CopySettings(pGraph, false);
	pXDirect->m_pTrGraph->CopySettings(pGraph, false);
	pXDirect->m_pUserGraph->CopySettings(pGraph, false);
	
	pXInverse->m_QGraph.CopySettings(pGraph, false);
}



QString MainFrame::ShortenFileName(QString &PathName)
{
	QString strong, strange;
	if(PathName.length()>60)
	{
		int pos = PathName.lastIndexOf('/');
		if (pos>0)
		{
			strong = '/'+PathName.right(PathName.length()-pos-1);
			strange = PathName.left(60-strong.length()-6);
			pos = strange.lastIndexOf('/');
			if(pos>0) strange = strange.left(pos)+"/...  ";
			strong = strange+strong;
		}
		else
		{
			strong = PathName.left(40);
		}
	}
	else strong = PathName;
	return strong;
}

void MainFrame::UpdateFoils()
{
	
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	pXDirect->SetFoil(m_pctrlFoil->currentObject());
	
	UpdatePolars();
	
}

void MainFrame::UpdatePolars()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	pXDirect->m_pCurPolar = m_pctrlPolar->currentObject();
	
	UpdateOpps();
}


void MainFrame::UpdateOpps()
{    
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	
	pXDirect->m_pCurOpp = m_pctrlOpPoint->currentObject();
}



void MainFrame::updateRecentFileActions()
{
	int numRecentFiles = qMin(m_RecentFiles.size(), MAXRECENTFILES);
	
	QString text;
	for (int i = 0; i < numRecentFiles; ++i)
	{
		text = tr("&%1 %2").arg(i + 1).arg(ShortenFileName(m_RecentFiles[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(m_RecentFiles[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
		recentFileActs[j]->setVisible(false);
	
	separatorAct->setVisible(numRecentFiles > 0);
}



void MainFrame::UpdateView()
{
	switch(m_iApp)
	{
	case XFOILANALYSIS:
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->UpdateView();
		break;
	}
	case DIRECTDESIGN:
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->UpdateView();
		break;
	}
	case INVERSEDESIGN:
	{
		QXInverse *pXInverse = (QXInverse*)m_pXInverse;
		pXInverse->UpdateView();
		break;
	}
		//////////////new code DM/////////
	case BEM:
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		pBEM->UpdateView();
		break;
	}
		/////////////end new code DM///////////
		//////////////new code JW/////////
	case DMS:
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		pDMS->UpdateView();
		break;
	}
		/////////////end new code JW///////////
	}
}


void MainFrame::OnPolarProps()
{
	if(m_iApp==XFOILANALYSIS)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->OnPolarProps();
	}
}


void MainFrame::OnWOppProps()
{
	if(m_iApp==XFOILANALYSIS)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->OnOpPointProps();
	}
}



void MainFrame::SetDlgPos(QDialog &Dlg)
{
	QPoint Position = m_DlgPos;
	QDesktopWidget desk;
	
	if(Dlg.frameGeometry().width() +m_DlgPos.x()>desk.width())  Position.rx() += desk.width() -(Dlg.frameGeometry().width() +m_DlgPos.x());
	if(Dlg.frameGeometry().height()+m_DlgPos.y()>desk.height()) Position.ry() += desk.height()-(Dlg.frameGeometry().height()+m_DlgPos.y());
	
	if(m_DlgPos.x()<0) Position.rx()=0;
	
	if(Dlg.width()>desk.rect().width())   Position.rx()=0;
	if(Dlg.height()>desk.rect().height()) Position.ry()=0;
	
	Dlg.move(Position);
}



void MainFrame::OnDuplicateFoil()
{
	if(!g_pCurFoil) return;
	CFoil *pNewFoil = new CFoil();
	pNewFoil->CopyFoil(g_pCurFoil);
	pNewFoil->InitFoil();
	
	if(StoreFoil(pNewFoil))
	{
		if(m_iApp==XFOILANALYSIS)
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->m_BufferFoil.CopyFoil(pNewFoil);
			pXDirect->SetFoil();
		}
		else if(m_iApp==DIRECTDESIGN)
		{
			QAFoil *pAFoil= (QAFoil*)m_pAFoil;
			//then duplicate the buffer foil and add it
			CFoil *pNewFoil = new CFoil();
			pNewFoil->CopyFoil(pAFoil->m_pBufferFoil);
			pAFoil->FillFoilTable();
			pAFoil->SelectFoil(pNewFoil);
		}
		
		UpdateFoils();
		UpdateView();
		SetSaveState(false);
	}
}

void MainFrame::setIApp (int iApp) {
	if (m_iApp != iApp) {
		emit moduleChanged ();
		m_iApp = iApp;
	}
	
	if (iApp == XFOILANALYSIS || iApp == DIRECTDESIGN || iApp == INVERSEDESIGN || iApp == BEM || iApp == DMS) {
		setCurrentModule(NULL);  // is needed, because these modules do not inherit class Module
	}
}

void MainFrame::onModuleChanged () {
	if (g_mainFrame->m_currentModule == NULL) {
		m_pctrlXDirectToolBar->hide();
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
		
		m_pctrlXDirectWidget->hide();
		m_pctrlXInverseWidget->hide();
		m_pctrlAFoilWidget->hide();
		
		OnAFoilAct->setChecked(false);
		OnXDirectAct->setChecked(false);
		OnXInverseAct->setChecked(false);
	}
}

void MainFrame::setCurrentModule(ModuleBase *newModule) {
	m_currentModule = newModule;
}

GLModule *MainFrame::getGlModule() {
	return dynamic_cast<GLModule*>(m_currentModule);
}

QStringList MainFrame::prepareMissingObjectMessage() {
	// NM if all modules would inherit ModuleBase, this switch function wouldn't be necessary
	QStringList message;
	if (g_mainFrame->getCurrentModule()) {
		message = g_mainFrame->getCurrentModule()->prepareMissingObjectMessage();
	} else {
		switch (m_iApp) {
		case XFOILANALYSIS:
			message = CPolar::prepareMissingObjectMessage(); break;
		case DIRECTDESIGN:
			message = QStringList(); break;
		case INVERSEDESIGN:
			message = QStringList(); break;
		case BEM:
			message = g_qbem->prepareMissingObjectMessage(); break;
		case DMS:
			message = g_qdms->prepareMissingObjectMessage(); break;
		default:
			message = QStringList(); break;  // no module is chosen after starting QBlade
		}
	}
	
	if (!message.isEmpty()) {
		message.prepend("Missing Objects:");
	}
	return message;
}

void MainFrame::setIView (int newView, int /*newApp*/) {
	m_iView = newView;
}

void MainFrame::switchToTwoDWidget() {
	m_twoDWidgetInterface = dynamic_cast<TwoDWidgetInterface*> (m_currentModule);
	m_centralWidget->setCurrentIndex(0);
}


MainFrame *g_mainFrame;

