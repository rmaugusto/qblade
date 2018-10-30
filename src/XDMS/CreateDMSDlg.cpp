/****************************************************************************

    CreateDMSDlg Class
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

#include "CreateDMSDlg.h"
#include "DMS.h"
#include "../Globals.h"


CreateDMSDlg::CreateDMSDlg(void *pParent)
{
setWindowTitle(tr("Define DMS Parameters"));

m_pParent = pParent;


SetupLayout();
Connect();

}


void CreateDMSDlg::SetupLayout()
{
QDMS* pDMS = (QDMS *) m_pParent;
QGridLayout *VarLayout = new QGridLayout;

RhoEdit = new NumberEdit;
ViscEdit = new NumberEdit;
ElementsEdit = new NumberEdit;
IterationsEdit = new NumberEdit;
EpsilonEdit = new NumberEdit;
RelaxEdit = new NumberEdit;

SimName = new QLineEdit;

RhoEdit->setValue(pDMS->dlg_rho);
RhoEdit->setAutomaticPrecision(4);
ViscEdit->setValue(pDMS->dlg_visc);
ViscEdit->setAutomaticPrecision(8);
ElementsEdit->setValue(pDMS->dlg_elements);
ElementsEdit->setAutomaticPrecision(0);
ElementsEdit->setMinimum(10);
IterationsEdit->setValue(pDMS->dlg_iterations);
IterationsEdit->setAutomaticPrecision(0);
IterationsEdit->setMinimum(1);
EpsilonEdit->setValue(pDMS->dlg_epsilon);
EpsilonEdit->setMinimum(0.000000001);
RelaxEdit->setValue(pDMS->dlg_relax);
RelaxEdit->setAutomaticPrecision(2);

RhoEditLabel = new QLabel(tr("Rho"));
ViscEditLabel = new QLabel(tr("Viscosity"));
ElementsLabel = new QLabel("Discretize Blade into N Elements");
IterationsLabel = new QLabel(tr("Max Number of Iterations"));
EpsilonLabel = new QLabel(tr("Max Epsilon for Convergence"));
RelaxEditLabel = new QLabel(tr("Relax. Factor"));

RhoUnitLabel = new QLabel;
ViscUnitLabel = new QLabel;
RhoUnitLabel->setText("kg/m^3");
ViscUnitLabel->setText("kg/ms");
VarLayout->addWidget(RhoEditLabel,1,1);
VarLayout->addWidget(RhoEdit,1,2);
VarLayout->addWidget(RhoUnitLabel,1,3);
VarLayout->addWidget(ViscEditLabel,2,1);
VarLayout->addWidget(ViscEdit,2,2);
VarLayout->addWidget(ViscUnitLabel,2,3);
VarLayout->addWidget(ElementsLabel,3,1);
VarLayout->addWidget(ElementsEdit,3,2);
VarLayout->addWidget(IterationsLabel,4,1);
VarLayout->addWidget(IterationsEdit,4,2);
VarLayout->addWidget(EpsilonLabel,5,1);
VarLayout->addWidget(EpsilonEdit,5,2);
VarLayout->addWidget(RelaxEditLabel,6,1);
VarLayout->addWidget(RelaxEdit,6,2);

QGroupBox *NameGroup = new QGroupBox(tr("Simulation Name"));
QHBoxLayout *NameLayout = new QHBoxLayout;
NameLayout->addWidget(SimName);
NameGroup->setLayout(NameLayout);

QGroupBox *VarGroup = new QGroupBox(tr("Variables"));
VarGroup->setLayout(VarLayout);

QGridLayout *CorLayout = new QGridLayout;

TipLossBox = new QCheckBox(tr("Tip Loss"));
TipLossBox->setChecked(pDMS->dlg_tiploss);
VariableBox = new QCheckBox(tr("Variable Induction Factors"));
VariableBox->setChecked(pDMS->dlg_variable);
AspectRatioBox = new QCheckBox(tr("Finite Aspect Ratio Correction"));
AspectRatioBox->setChecked(pDMS->dlg_aspectratio);
CorLayout->addWidget(TipLossBox,1,1);
//CorLayout->addWidget(AspectRatioBox,2,1);
CorLayout->addWidget(VariableBox,3,1);


/*
RootLossBox = new QCheckBox(tr("Prandtl Root Loss"));
RootLossBox->setChecked(RootLoss);

ThreeDBox = new QCheckBox(tr("3D Correction"));
ThreeDBox->setChecked(ThreeDCorrection);

InterpolationBox = new QCheckBox(tr("Foil Interpolation"));
InterpolationBox->setChecked(Interpolation);

NewTipLossBox = new QCheckBox(tr("New Tip Loss"));
NewTipLossBox->setChecked(NewTipLoss);

NewRootLossBox = new QCheckBox(tr("New Root Loss"));
NewRootLossBox->setChecked(NewRootLoss);

CorLayout->addWidget(NewTipLossBox,2,1);
CorLayout->addWidget(RootLossBox,3,1);
CorLayout->addWidget(NewRootLossBox,4,1);
CorLayout->addWidget(InterpolationBox,5,1);
CorLayout->addWidget(ThreeDBox,6,1);
*/

QGroupBox *CorGroup = new QGroupBox(tr("Corrections"));
CorGroup->setLayout(CorLayout);



PowerLawRadio = new QRadioButton(tr("Power Law"));
PowerLawRadio->setEnabled(g_mainFrame->m_iView==TURBINEVIEW);// new JW, old: pDMS->m_iView
PowerLawRadio->setChecked(g_mainFrame->m_iView==TURBINEVIEW && pDMS->dlg_powerlaw);// new JW, old: pDMS->m_iView
if (g_mainFrame->m_iView!=TURBINEVIEW) PowerLawRadio->setChecked(false);// new JW, old: pDMS->m_iView

ConstantRadio = new QRadioButton(tr("Constant"));
ConstantRadio->setChecked(pDMS->dlg_constant || g_mainFrame->m_iView!=TURBINEVIEW);// new JW, old: pDMS->m_iView

LogarithmicRadio = new QRadioButton(tr("Logarithmic"));
LogarithmicRadio->setEnabled(g_mainFrame->m_iView==TURBINEVIEW);// new JW, old: pDMS->m_iView
LogarithmicRadio->setChecked(g_mainFrame->m_iView==TURBINEVIEW && pDMS->dlg_logarithmic);// new JW, old: pDMS->m_iView
if (g_mainFrame->m_iView!=TURBINEVIEW) LogarithmicRadio->setChecked(false);// new JW, old: pDMS->m_iView

ExpEditLabel = new QLabel(tr("exponent"));
ExpEdit = new NumberEdit;
ExpEdit->setValue(pDMS->dlg_exponent);
ExpEdit->setAutomaticPrecision(3);
ExpEdit->setEnabled(pDMS->dlg_powerlaw);
if (g_mainFrame->m_iView!=TURBINEVIEW) ExpEdit->setEnabled(false);// new JW, old: pDMS->m_iView



RoughEditLabel = new QLabel(tr("surface roughness"));
RoughEdit = new NumberEdit;
RoughEdit->setValue(pDMS->dlg_roughness * g_mainFrame->m_mtoUnit);
RoughEdit->setAutomaticPrecision(3);
RoughEdit->setMinimum(0.0002);
RoughEdit->setMaximum(2);
RoughEdit->setEnabled(pDMS->dlg_logarithmic);
if (g_mainFrame->m_iView==BEMSIMVIEW) RoughEdit->setEnabled(false);// new JW, old: pDMS->m_iView

QString str;
GetLengthUnit(str, g_mainFrame->m_LengthUnit);
RoughUnitLabel = new QLabel;
RoughUnitLabel->setText(str);

/*
QGridLayout *WindLayout = new QGridLayout;
WindLayout->addWidget(PowerLawRadio, 1,1);
WindLayout->addWidget(ExpEditLabel, 2,2);
WindLayout->addWidget(ExpEdit, 2,3);
WindLayout->addWidget(ConstantRadio, 3,1);
WindLayout->addWidget(LogarithmicRadio, 4,1);
WindLayout->addWidget(RoughEditLabel, 5,2);
WindLayout->addWidget(RoughEdit, 5,3);
WindLayout->addWidget(RoughUnitLabel, 5,4);
WindLayout->addWidget(refHEditLabel, 6,2);
WindLayout->addWidget(refHEdit, 6,3);
WindLayout->addWidget(refHUnitLabel, 6,4);
WindLayout->addWidget(refVEditLabel, 7,2);
WindLayout->addWidget(refVEdit, 7,3);
WindLayout->addWidget(refVUnitLabel, 7,4);
*/

QGridLayout *WindLayout = new QGridLayout;

WindLayout->addWidget(ConstantRadio, 1,1);
WindLayout->addWidget(PowerLawRadio, 2,1);
WindLayout->addWidget(ExpEditLabel, 3,2);
WindLayout->addWidget(ExpEdit, 3,3);
WindLayout->addWidget(LogarithmicRadio, 4,1);
WindLayout->addWidget(RoughEditLabel, 5,2);
WindLayout->addWidget(RoughEdit, 5,3);
WindLayout->addWidget(RoughUnitLabel, 5,4);



QGroupBox *WindGroup = new QGroupBox(tr("Wind Profile"));
WindGroup->setLayout(WindLayout);

OkButton = new QPushButton(tr("Create"));

QVBoxLayout *mainLayout = new QVBoxLayout;
QHBoxLayout *bottomLayout = new QHBoxLayout;
bottomLayout->addWidget(CorGroup);
bottomLayout->addWidget(VarGroup);
mainLayout->addWidget(NameGroup);
mainLayout->addLayout(bottomLayout);
//mainLayout->addWidget(WindGroup);
mainLayout->addWidget(OkButton);

setLayout(mainLayout);

}


void CreateDMSDlg::Connect()
{

    connect(OkButton, SIGNAL(clicked()), SLOT(accept()));

    connect(PowerLawRadio, SIGNAL(clicked()), this, SLOT(OnPow()));
    connect(ConstantRadio, SIGNAL(clicked()), this, SLOT(OnConst()));
    connect(LogarithmicRadio, SIGNAL(clicked()), this, SLOT(OnLog()));

    connect(PowerLawRadio, SIGNAL(clicked()), this, SLOT(CheckButtons()));
    connect(ConstantRadio, SIGNAL(clicked()), this, SLOT(CheckButtons()));
    connect(LogarithmicRadio, SIGNAL(clicked()), this, SLOT(CheckButtons()));



}


void CreateDMSDlg::OnPow()
{
    QDMS* pDMS = (QDMS *) m_pParent;

    pDMS->dlg_powerlaw = true;
    pDMS->dlg_logarithmic = false;
    pDMS->dlg_constant = false;
}



void CreateDMSDlg::OnLog()
{
    QDMS* pDMS = (QDMS *) m_pParent;

    pDMS->dlg_logarithmic = true;
    pDMS->dlg_powerlaw = false;
    pDMS->dlg_constant = false;
}


void CreateDMSDlg::OnConst()
{
    QDMS* pDMS = (QDMS *) m_pParent;

    pDMS->dlg_constant = true;
    pDMS->dlg_powerlaw = false;
    pDMS->dlg_logarithmic = false;
}


void CreateDMSDlg::CheckButtons()
{

    QDMS* pDMS = (QDMS *) m_pParent;

    if (pDMS->dlg_powerlaw)
    {
        ExpEdit->setEnabled(true);
        RoughEdit->setEnabled(false);
    }

    if (pDMS->dlg_constant)
    {
        ExpEdit->setEnabled(false);
        RoughEdit->setEnabled(false);
    }

    if (pDMS->dlg_logarithmic)
    {
        ExpEdit->setEnabled(false);
        RoughEdit->setEnabled(true);
    }

}


