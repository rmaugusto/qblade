/****************************************************************************

	DisplaySettingsDlg Class
	Copyright (C) 2008-2008 Andre Deperrois adeperrois@xflr5.com

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

#include "DisplaySettingsDlg.h"
#include "../MainFrame.h"
#include "../XDirect/XDirect.h"
#include "../XInverse/XInverse.h"
#include "../Graph/GraphDlg.h"
#include "../QBladeApplication.h"
#include <QGroupBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QStyleFactory>
////////////new code DM//////////
#include "../XBEM/BEM.h"
//////////end new code DM/////////
////////////new code JW//////////
#include "../XDMS/DMS.h"
//////////end new code JW/////////



DisplaySettingsDlg::DisplaySettingsDlg(void *pParent)
{
	setWindowTitle(tr("General Display Settings"));
	m_pMainFrame = pParent;
	m_bIsGraphModified = false;
	m_bReverseZoom = false;
	m_bAlphaChannel = false;
	SetupLayout();

	connect(m_pctrlStyles, SIGNAL(activated(const QString &)),this, SLOT(OnStyleChanged(const QString &)));

	connect(m_pctrlBackColor, SIGNAL(clicked()),this, SLOT(OnBackgroundColor()));
	connect(m_pctrlGraphSettings, SIGNAL(clicked()),this, SLOT(OnGraphSettings()));
	connect(m_pctrlTextClr, SIGNAL(clicked()),this, SLOT(OnTextColor()));
	connect(m_pctrlTextFont, SIGNAL(clicked()),this, SLOT(OnTextFont()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void DisplaySettingsDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlStyles = new QComboBox;
	m_pctrlStyles->addItems(QStyleFactory::keys());

	m_pctrlGraphSettings  = new QPushButton(tr("All Graph Settings"));
	m_pctrlGraphSettings->setMinimumWidth(120);
	QHBoxLayout *GraphLayout = new QHBoxLayout;
	GraphLayout->addWidget(m_pctrlGraphSettings);
	QGroupBox *GraphBox = new QGroupBox(tr("Graph Settings"));
	GraphBox->setLayout(GraphLayout);


	QHBoxLayout *BackLayout = new QHBoxLayout;
	m_pctrlBackColor      = new ColorButton;
	m_pctrlBackColor->setMinimumWidth(120);
	BackLayout->addWidget(m_pctrlBackColor);
	QGroupBox *BackBox = new QGroupBox(tr("Background Color"));
	BackBox->setLayout(BackLayout);

	QHBoxLayout *FontLayout = new QHBoxLayout;
	m_pctrlTextFont       = new QPushButton;
	m_pctrlTextClr        = new QPushButton(tr("Text Color"));
	m_pctrlTextFont->setMinimumWidth(120);
	m_pctrlTextClr->setMinimumWidth(120);
	FontLayout->addWidget(m_pctrlTextFont);
	FontLayout->addWidget(m_pctrlTextClr);
	QGroupBox *FontBox = new QGroupBox(tr("Font"));
	FontBox->setLayout(FontLayout);

	m_pctrlReverseZoom = new QCheckBox(tr("Reverse zoom direction using mouse wheel"));
	m_pctrlAlphaChannel = new QCheckBox(tr("Enable 3D transparency"));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlStyles);
	MainLayout->addStretch(1);
	MainLayout->addWidget(BackBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(FontBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(GraphBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlReverseZoom);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlAlphaChannel);
	MainLayout->addSpacing(20);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void DisplaySettingsDlg::InitDialog()
{
	m_MemGraph.CopySettings(m_pRefGraph);
	m_pctrlBackColor->SetColor(m_BackgroundColor);
	QString FontName = m_TextFont.family() + QString(" %1").arg(m_TextFont.pointSize());
	m_pctrlTextFont->setText(FontName);
	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(dynamic_cast<QBladeApplication*>(qApp)->getApplicationStyle(),
														   Qt::MatchFixedString));
	m_pctrlReverseZoom->setChecked(m_bReverseZoom);
	m_pctrlAlphaChannel->setChecked(m_bAlphaChannel);
	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
//		m_pctrlTextClr->setAutoFillBackground(true);
		m_pctrlTextClr->setFont(m_TextFont);
	}
}

void DisplaySettingsDlg::OnStyleChanged(const QString &StyleName) {
	dynamic_cast<QBladeApplication*>(qApp)->setApplicationStyle(StyleName);
}


void DisplaySettingsDlg::OnBackgroundColor()
{
	QColor Color = QColorDialog::getColor(m_BackgroundColor);
	if(Color.isValid()) m_BackgroundColor = Color;

	m_pctrlBackColor->SetColor(m_BackgroundColor);

	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_BackgroundColor);
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
	}
}


void DisplaySettingsDlg::reject()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect *pXDirect   = (QXDirect*)pMainFrame->m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)pMainFrame->m_pXInverse;

	pXDirect->m_pCpGraph->CopySettings(&m_MemGraph);
	pXDirect->m_pCpGraph->SetInverted(true);

	pXDirect->m_pPolarGraph->CopySettings(&m_MemGraph);
	pXDirect->m_pCmGraph->CopySettings(&m_MemGraph);
	pXDirect->m_pCzGraph->CopySettings(&m_MemGraph);
	pXDirect->m_pTrGraph->CopySettings(&m_MemGraph);
	pXDirect->m_pUserGraph->CopySettings(&m_MemGraph);

	pXInverse->m_QGraph.CopySettings(&m_MemGraph);
	pXInverse->m_QGraph.SetInverted(true);

        /////new code DM////////
        QBEM *pBEM = (QBEM *) pMainFrame->m_pBEM;

        pBEM->m_RotorGraph1.CopySettings(&m_MemGraph);
        pBEM->m_RotorGraph2.CopySettings(&m_MemGraph);
        pBEM->m_RotorGraph3.CopySettings(&m_MemGraph);
        pBEM->m_PowerGraph1.CopySettings(&m_MemGraph);
        pBEM->m_PowerGraph2.CopySettings(&m_MemGraph);
        pBEM->m_PowerGraph3.CopySettings(&m_MemGraph);
        pBEM->m_360Graph2.CopySettings(&m_MemGraph);
        pBEM->m_360Graph1.CopySettings(&m_MemGraph);
        pBEM->m_360Graph3.CopySettings(&m_MemGraph);
        pBEM->m_360Graph4.CopySettings(&m_MemGraph);
        pBEM->m_CharGraph1.CopySettings(&m_MemGraph);
        pBEM->m_CharGraph2.CopySettings(&m_MemGraph);
        pBEM->m_CharGraph3.CopySettings(&m_MemGraph);
        pBEM->m_CharGraph4.CopySettings(&m_MemGraph);
        ///////////end new code DM/////////


        /////new code JW////////
        QDMS *pDMS = (QDMS *) pMainFrame->m_pDMS;

        pDMS->m_RotorGraph1.CopySettings(&m_MemGraph);
        pDMS->m_RotorGraph2.CopySettings(&m_MemGraph);
        pDMS->m_RotorGraph3.CopySettings(&m_MemGraph);
        pDMS->m_PowerGraph1.CopySettings(&m_MemGraph);
        pDMS->m_PowerGraph2.CopySettings(&m_MemGraph);
		pDMS->m_PowerGraph3.CopySettings(&m_MemGraph);
		pDMS->m_CharGraph1.CopySettings(&m_MemGraph);
		pDMS->m_CharGraph2.CopySettings(&m_MemGraph);
		pDMS->m_CharGraph3.CopySettings(&m_MemGraph);
		pDMS->m_CharGraph4.CopySettings(&m_MemGraph);
        ///////////end new code JW/////////

	QDialog::reject();
}

void DisplaySettingsDlg::OnGraphSettings()
{
	if(!m_pRefGraph) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect *pXDirect   = (QXDirect*)pMainFrame->m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)pMainFrame->m_pXInverse;

	GraphDlg dlg;

	dlg.m_GraphArray[0] = pXDirect->m_pCpGraph;
	dlg.m_GraphArray[1] = pXDirect->m_pPolarGraph;
	dlg.m_GraphArray[2] = pXDirect->m_pCmGraph;
	dlg.m_GraphArray[3] = pXDirect->m_pCzGraph;
	dlg.m_GraphArray[4] = pXDirect->m_pTrGraph;
	dlg.m_GraphArray[5] = pXDirect->m_pUserGraph;

	dlg.m_GraphArray[6] = NULL;  // NM these are the former Miarex Graphs
	dlg.m_GraphArray[7] = NULL;
	dlg.m_GraphArray[8] = NULL;
	dlg.m_GraphArray[9] = NULL;
	dlg.m_GraphArray[10] = NULL;
	dlg.m_GraphArray[11] = NULL;
	dlg.m_GraphArray[12] = NULL;
	dlg.m_GraphArray[13] = NULL;
	dlg.m_GraphArray[14] = NULL;
	dlg.m_GraphArray[15] = NULL;
	dlg.m_GraphArray[16] = NULL;
	dlg.m_GraphArray[17] = NULL;
	dlg.m_GraphArray[18] = NULL;
	dlg.m_GraphArray[19] = NULL;
	dlg.m_GraphArray[20] = &pXInverse->m_QGraph;
	//////////////new code DM/////////////////////
	QBEM *pBEM = (QBEM *)pMainFrame->m_pBEM;
	QDMS *pDMS = (QDMS *)pMainFrame->m_pDMS;

	dlg.m_GraphArray[21] = &pBEM->m_RotorGraph1;
	dlg.m_GraphArray[22] = &pBEM->m_RotorGraph2;
	dlg.m_GraphArray[23] = &pBEM->m_RotorGraph3;
	dlg.m_GraphArray[24] = &pBEM->m_PowerGraph1;
	dlg.m_GraphArray[25] = &pBEM->m_PowerGraph2;
	dlg.m_GraphArray[26] = &pBEM->m_PowerGraph3;
	dlg.m_GraphArray[27] = &pBEM->m_360Graph1;
	dlg.m_GraphArray[28] = &pBEM->m_360Graph2;
	dlg.m_GraphArray[29] = &pBEM->m_CharGraph1;
	dlg.m_GraphArray[30] = &pBEM->m_CharGraph2;
	dlg.m_GraphArray[31] = &pBEM->m_CharGraph3;
	dlg.m_GraphArray[32] = &pBEM->m_CharGraph4;
	dlg.m_GraphArray[33] = &pDMS->m_PowerGraph1;
	dlg.m_GraphArray[34] = &pDMS->m_PowerGraph2;
	dlg.m_GraphArray[35] = &pDMS->m_PowerGraph3;
	dlg.m_GraphArray[36] = &pDMS->m_RotorGraph1;
	dlg.m_GraphArray[37] = &pDMS->m_RotorGraph2;
	dlg.m_GraphArray[38] = &pDMS->m_RotorGraph3;
	dlg.m_GraphArray[39] = &pDMS->m_CharGraph1;
	dlg.m_GraphArray[40] = &pDMS->m_CharGraph2;
	dlg.m_GraphArray[41] = &pDMS->m_CharGraph3;
	dlg.m_GraphArray[42] = &pDMS->m_CharGraph4;
    dlg.m_GraphArray[43] = &pBEM->m_360Graph3;
    dlg.m_GraphArray[44] = &pBEM->m_360Graph4;

    dlg.m_NGraph = 45;
	/////////////end new code DM/////////////


	QGraph graph;
	graph.CopySettings(m_pRefGraph);

	dlg.m_pMemGraph = m_pRefGraph;
	dlg.m_pGraph = &graph;

	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_pRefGraph->CopySettings(dlg.m_pGraph);
		m_bIsGraphModified = true;

		pXDirect->m_pCpGraph->SetInverted(true);
		pXInverse->m_QGraph.SetInverted(true);
	}
}


void DisplaySettingsDlg::OnTextColor()
{
	QColor Color = QColorDialog::getColor(m_TextColor);
	if(Color.isValid()) m_TextColor = Color;

	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_BackgroundColor);
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
//		m_pctrlTextClr->setAutoFillBackground(true);
	}
}



void DisplaySettingsDlg::OnTextFont()
{
	bool ok;
	QFont TextFont;
	TextFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);

#ifdef Q_WS_MAC
	//20090604 Mac OS Native font dialog does not work well under QT 4.5.1
	//QFont font = QFontDialog::getFont(&ok, m_TextFont, this);
        //20110324 Works again under QT 4.6, though it loses focus is mouse is moved outside of it (QT bug?)
        //QFont font = QFontDialog::getFont(&ok, m_TextFont, this, "",QFontDialog::DontUseNativeDialog);
	   TextFont = QFontDialog::getFont(&ok, m_TextFont, this);
#else
	TextFont = QFontDialog::getFont(&ok, m_TextFont, this);
#endif

	if (ok)
	{
		m_TextFont = TextFont;
		m_pctrlTextFont->setText(m_TextFont.family());
		m_pctrlTextFont->setFont(m_TextFont);
		m_pctrlTextClr->setFont(m_TextFont);
        qApp->setFont(TextFont);

	}

}








