/****************************************************************************

	DMSDock Class
		Copyright (C) 2013 Juliane Wendler

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

#include "DMSDock.h"
#include "../Globals.h"
#include "../XGlobals.h"
#include "DMS.h"


DMSDock::DMSDock (const QString & title, QMainWindow * parent, Qt::WindowFlags flags)
	: ScrolledDock (title, parent, flags)
{
	g_mainFrame->m_pDMS = new QDMS(g_mainFrame);
	QDMS *pDMS = (QDMS*) g_mainFrame->m_pDMS;
	pDMS->SetupLayout();

	/*
	//--------------------Wing Table Layout--------------//


	m_pctrlWingNameLabel = new QLabel;
	m_pctrlBladesAndHeightLabel = new QLabel;

	m_pctrlBladeTableView = new QTableView;
	m_pctrlBladeTableView->setSelectionMode(QAbstractItemView::NoSelection);
	m_pctrlBladeTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_pctrlEditWing = new QPushButton(tr("Edit Blade"));
	m_pctrlNewWing = new QPushButton(tr("New Blade"));

	QHBoxLayout *EditNewLayout = new QHBoxLayout;
	EditNewLayout->addWidget(m_pctrlEditWing);
	EditNewLayout->addWidget(m_pctrlNewWing);

	QVBoxLayout *BottomLayout = new QVBoxLayout;
	BottomLayout->addWidget(m_pctrlWingNameLabel);
	BottomLayout->addWidget(m_pctrlBladesAndHeightLabel);
	BottomLayout->addWidget(m_pctrlBladeTableView);
	BottomLayout->addLayout(EditNewLayout);

	QGroupBox *WingDataBox = new QGroupBox(tr("Blade Data"));
	WingDataBox->setLayout(BottomLayout);


	//-----------Wing Edit Layout-------------//


	m_pctrlWingName     = new QLineEdit(tr("Blade Name"));
	m_pctrlWingColor    = new ColorButton;
	m_pctrlSectionColor    = new ColorButton;
	m_pctrlSectionColor->SetColor(QColor(0,0,255));
	QHBoxLayout *NameLayout = new QHBoxLayout;
	NameLayout->addWidget(m_pctrlWingColor);
	NameLayout->addWidget(m_pctrlWingName);
	NameLayout->addWidget(m_pctrlSectionColor);

	m_pctrlBladesLabel = new QLabel("Number of Blades");
	m_pctrlBlades = new QSpinBox;
	m_pctrlBlades->setMinimum(1);
	m_pctrlHubRadiusLabel = new QLabel(tr("Radial Offset"));
	m_pctrlHubRadius = new FloatEdit;
	m_pctrlHubRadiusUnitLabel = new QLabel;
	QHBoxLayout *ParamLayout = new QHBoxLayout;
	ParamLayout->addWidget(m_pctrlBladesLabel);
	ParamLayout->addWidget(m_pctrlBlades);
	ParamLayout->addWidget(m_pctrlHubRadiusLabel);
	ParamLayout->addWidget(m_pctrlHubRadius);
	ParamLayout->addWidget(m_pctrlHubRadiusUnitLabel);
	m_pctrlSolidityLabel = new QLabel;

	m_pctrlInsertBefore   = new QPushButton(tr("Insert Before"));
	m_pctrlInsertAfter    = new QPushButton(tr("Insert After"));
	m_pctrlDeleteSection  = new QPushButton(tr("Delete Section"));
	QHBoxLayout *InsertLayout = new QHBoxLayout;
	InsertLayout->addWidget(m_pctrlInsertBefore);
	InsertLayout->addWidget(m_pctrlInsertAfter);
	InsertLayout->addWidget(m_pctrlDeleteSection);

	m_pctrlBladeTable = new QTableView(this);
	m_pctrlBladeTable->setWindowTitle(QObject::tr("Blade definition"));
	m_pctrlBladeTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlBladeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlBladeTable->setEditTriggers(QAbstractItemView::CurrentChanged |
																			  QAbstractItemView::DoubleClicked |
																			  QAbstractItemView::SelectedClicked |
																			  QAbstractItemView::EditKeyPressed);

	m_pctrlSave           = new QPushButton(tr("Save"));
	m_pctrlOptimize       = new QPushButton(tr("Optimize"));
	m_pctrlBack           = new QPushButton(tr("Back"));
	m_pctrlScale          = new QPushButton(tr("Scale"));


	QHBoxLayout *OptScale = new QHBoxLayout;
	OptScale->addWidget(m_pctrlScale);
	OptScale->addWidget(m_pctrlOptimize);
	QGroupBox *OptBox = new QGroupBox(tr("Modify Shape"));
	OptBox->setLayout(OptScale);

	QHBoxLayout *BackSave = new QHBoxLayout;
	BackSave->addWidget(m_pctrlBack);
	BackSave->addWidget(m_pctrlSave);

	QVBoxLayout *EditLayout = new QVBoxLayout;
	EditLayout->addLayout(NameLayout);
	EditLayout->addLayout(ParamLayout);
	EditLayout->addWidget(m_pctrlSolidityLabel);
	EditLayout->addLayout(InsertLayout);
	EditLayout->addWidget(m_pctrlBladeTable);
	EditLayout->addWidget(OptBox);
	EditLayout->addLayout(BackSave);

	*/

	pDMS->Connect();
	pDMS->setAttribute(Qt::WA_DeleteOnClose, false);

	m_contentVBox->addWidget(pDMS->mainWidget);
//	m_contentVBox->addWidget(pDMS->SimWidget);
//	m_contentVBox->addWidget(pDMS->PowWidget);
	setVisible(false);

	addScrolledDock(Qt::LeftDockWidgetArea , parent);
}

void DMSDock::initView()
{
}
