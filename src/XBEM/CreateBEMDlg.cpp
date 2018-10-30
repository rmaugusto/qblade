/****************************************************************************

    CreateBEMDlg Class
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

#include "CreateBEMDlg.h"
#include "BEM.h"



CreateBEMDlg::CreateBEMDlg(void *pParent)
{
m_pParent = pParent;

setWindowTitle(tr("Define BEM Parameters"));

SetupLayout();

Connect();

}

void CreateBEMDlg::SetupLayout()
{
QBEM *pBEM = (QBEM *) m_pParent;


QGridLayout *VarLayout = new QGridLayout;

IterationsEdit = new NumberEdit;
EpsilonEdit = new NumberEdit;
ElementsEdit = new NumberEdit;
RhoEdit = new NumberEdit;
RelaxEdit = new NumberEdit;
ViscEdit = new NumberEdit;

SimName = new QLineEdit;

IterationsEdit->setValue(pBEM->dlg_iterations);
IterationsEdit->setAutomaticPrecision(0);
IterationsEdit->setMinimum(1);
EpsilonEdit->setValue(pBEM->dlg_epsilon);
EpsilonEdit->setMinimum(0.000000001);
ElementsEdit->setValue(pBEM->dlg_elements);
ElementsEdit->setAutomaticPrecision(0);
ElementsEdit->setMinimum(5);
ElementsEdit->setMaximum(1000);

RelaxEdit->setValue(pBEM->dlg_relax);
RelaxEdit->setAutomaticPrecision(2);
RhoEdit->setValue(pBEM->dlg_rho);
RhoEdit->setAutomaticPrecision(4);
ViscEdit->setValue(pBEM->dlg_visc);
ViscEdit->setAutomaticPrecision(8);

IterationsLabel = new QLabel(tr("Max Number of Iterations"));
EpsilonLabel = new QLabel(tr("Max Epsilon for Convergence"));
ElementsLabel = new QLabel("Discretize Blade into N Elements");
RhoEditLabel = new QLabel(tr("Rho"));
RelaxEditLabel = new QLabel(tr("Relax. Factor"));
ViscEditLabel = new QLabel(tr("Viscosity"));

VarLayout->addWidget(ElementsEdit,1,1);
VarLayout->addWidget(ElementsLabel,1,2);
VarLayout->addWidget(EpsilonEdit,2,1);
VarLayout->addWidget(EpsilonLabel,2,2);
VarLayout->addWidget(IterationsEdit,3,1);
VarLayout->addWidget(IterationsLabel,3,2);
VarLayout->addWidget(RhoEdit,5,1);
VarLayout->addWidget(RhoEditLabel,5,2);
VarLayout->addWidget(RelaxEdit,4,1);
VarLayout->addWidget(RelaxEditLabel,4,2);
VarLayout->addWidget(ViscEditLabel,6,2);
VarLayout->addWidget(ViscEdit,6,1);


QGroupBox *NameGroup = new QGroupBox(tr("Simulation Name"));
QHBoxLayout *NameLayout = new QHBoxLayout;
NameLayout->addWidget(SimName);
NameGroup->setLayout(NameLayout);

QGroupBox *VarGroup = new QGroupBox(tr("Variables"));
VarGroup->setLayout(VarLayout);

QGridLayout *CorLayout = new QGridLayout;

TipLossBox = new QCheckBox(tr("Prandtl Tip Loss"));
TipLossBox->setChecked(pBEM->dlg_tiploss);

ReynoldsBox = new QCheckBox(tr("Reynolds Drag Correction"));
ReynoldsBox->setChecked(pBEM->dlg_reynolds);

RootLossBox = new QCheckBox(tr("Prandtl Root Loss"));
RootLossBox->setChecked(pBEM->dlg_rootloss);

ThreeDBox = new QCheckBox(tr("3D Correction"));
ThreeDBox->setChecked(pBEM->dlg_3dcorrection);

InterpolationBox = new QCheckBox(tr("Foil Interpolation"));
InterpolationBox->setChecked(pBEM->dlg_interpolation);

NewTipLossBox = new QCheckBox(tr("New Tip Loss"));
NewTipLossBox->setChecked(pBEM->dlg_newtiploss);

NewRootLossBox = new QCheckBox(tr("New Root Loss"));
NewRootLossBox->setChecked(pBEM->dlg_newrootloss);


CorLayout->addWidget(TipLossBox,1,1);
CorLayout->addWidget(NewTipLossBox,2,1);
CorLayout->addWidget(RootLossBox,3,1);
CorLayout->addWidget(NewRootLossBox,4,1);
CorLayout->addWidget(ThreeDBox,5,1);
CorLayout->addWidget(ReynoldsBox,6,1);
CorLayout->addWidget(InterpolationBox,7,1);


QGroupBox *CorGroup = new QGroupBox(tr("Corrections"));
CorGroup->setLayout(CorLayout);


OkButton = new QPushButton(tr("Create"));

QVBoxLayout *mainLayout = new QVBoxLayout;
QHBoxLayout *bottomLayout = new QHBoxLayout;
bottomLayout->addWidget(CorGroup);
bottomLayout->addWidget(VarGroup);
mainLayout->addWidget(NameGroup);
mainLayout->addLayout(bottomLayout);
mainLayout->addWidget(OkButton);

setLayout(mainLayout);


}

void CreateBEMDlg::Connect()
{

    connect(OkButton, SIGNAL(clicked()), SLOT(accept()));
    connect(TipLossBox, SIGNAL(clicked()), this, SLOT(CheckButtons()));
    connect(RootLossBox, SIGNAL(clicked()), this, SLOT(CheckButtons()));
    connect(NewTipLossBox, SIGNAL(clicked()), this, SLOT(CheckButtons()));
    connect(NewRootLossBox, SIGNAL(clicked()), this, SLOT(CheckButtons()));



}

void CreateBEMDlg::CheckButtons()
{
    if (NewTipLossBox->isChecked() || NewRootLossBox->isChecked())
    {
        TipLossBox->setChecked(false);
        RootLossBox->setChecked(false);
    }


}


