/****************************************************************************

    QFEMToolBar Class
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

#include "QFEMToolBar.h"
#include <QAction>
#include <QGroupBox>
#include <QLayout>
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"


QFEMToolBar::QFEMToolBar(QMainWindow* parent, QFEMModule* module)
{
	setObjectName("FEMToolbar");
	
    m_module = module;
//    HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
//    HideWidgetsAct->setCheckable(true);
//    HideWidgetsAct->setStatusTip(tr("Hide Widgets"));
//    //connect(HideWidgetsAct, SIGNAL(triggered()), this, SLOT(OnHideWidgets()));
//    addAction(HideWidgetsAct);
    HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
    HideWidgetsAct->setCheckable(true);
    HideWidgetsAct->setStatusTip(tr("Hide Widgets"));

    addAction(HideWidgetsAct);

    connect(HideWidgetsAct, SIGNAL(triggered()), m_module, SLOT(OnHideWidgets()));

    GLView = new QAction(QIcon(":/images/3dview.png"), tr("3D OpenGL View"), this);
    GLView->setCheckable(true);
    GLView->setStatusTip(tr("3D OpenGL View"));

    TwoDView = new QAction(QIcon(":/images/graph.png"), tr("Plot Results in a Graph"), this);
    TwoDView->setCheckable(true);
    TwoDView->setStatusTip(tr("Plot Results in a Graph"));

    connect (GLView, SIGNAL(triggered(bool)), m_module, SLOT(OnGLView()));
    connect (TwoDView, SIGNAL(triggered(bool)), m_module, SLOT(OnTwoDView()));


    addAction(GLView);
    addAction(TwoDView);
    addSeparator();

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));

    QGroupBox *groupBox = new QGroupBox (tr("Rotor Blade"));
	m_rotorComboBox = new RotorComboBox (&g_rotorStore);
	m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_rotorComboBox->setMinimumWidth(170);

	connect (m_rotorComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeRotor()));

    QGridLayout *grid = new QGridLayout ();
	grid->addWidget(m_rotorComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

    groupBox = new QGroupBox (tr("Structural Model"));
	m_BladeStructureComboBox = new BladeStructureComboBox (&g_bladeStructureStore);
	m_BladeStructureComboBox->setParentBox(m_rotorComboBox);
	m_BladeStructureComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_BladeStructureComboBox->setMinimumWidth(170);
    connect (m_BladeStructureComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeBladeStructure()));
    grid = new QGridLayout ();
	grid->addWidget(m_BladeStructureComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

    groupBox = new QGroupBox (tr("Blade Static Loading"));
    m_BladeStructureLoadingComboBox = new BladeStructureLoadingComboBox(&g_bladestructureloadingStore);
    m_BladeStructureLoadingComboBox->setParentBox(m_BladeStructureComboBox);
    m_BladeStructureLoadingComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_BladeStructureLoadingComboBox->setMinimumWidth(170);
    connect (m_BladeStructureLoadingComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeLoading()));
    grid = new QGridLayout ();
    grid->addWidget(m_BladeStructureLoadingComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

	parent->addToolBar(this);
    hide();

}
