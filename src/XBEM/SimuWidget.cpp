/****************************************************************************

    SimuWidget Class
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

#include "SimuWidget.h"
#include <QtWidgets>
#include <QWidget>
#include "../Misc/LineCbBox.h"
#include "../Misc/LineButton.h"
#include "../Misc/LineDelegate.h"

SimuWidget::SimuWidget(QWidget */*parent*/)
{

    SetupLayout();
    Connect();

    m_pctrlCurveColor->setEnabled(false);
    m_pctrlCurveStyle->setEnabled(false);
    m_pctrlCurveWidth->setEnabled(false);
    m_pctrlShowCurve->setEnabled(false);
    m_pctrlShowPoints->setEnabled(false);

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setFixedWidth(width/6);


}




void SimuWidget::SetupLayout()
{

//	setFixedWidth(250);
    /////////top Layout//////////
    m_pctrlRho = new QLabel(tr("Rho"));
    m_pctrlElements = new QLabel(tr("Elements"));
    m_pctrlIteration = new QLabel(tr("max Iterations"));
    m_pctrlEpsilon = new QLabel(tr("Epsilon"));
    m_pctrlRelax = new QLabel(tr("Relax Factor"));
    m_pctrlVisc = new QLabel(tr("Viscosity"));

    m_pctrlRhoVal = new QLabel;
    m_pctrlElementsVal = new QLabel;
    m_pctrlIterationVal = new QLabel;
    m_pctrlEpsilonVal = new QLabel;
    m_pctrlRelaxVal = new QLabel;
    m_pctrlViscVal = new QLabel;





    m_pctrlTipLoss = new QCheckBox(tr("Tip Loss"));
    m_pctrlTipLoss->setEnabled(false);

    m_pctrlRootLoss = new QCheckBox(tr("Root Loss"));
    m_pctrlRootLoss->setEnabled(false);

    m_pctrl3DCorrection = new QCheckBox(tr("3D Correction"));
    m_pctrl3DCorrection->setEnabled(false);

    m_pctrlInterpolation = new QCheckBox(tr("Foil Interpolation"));
    m_pctrlInterpolation->setEnabled(false);

    m_pctrlNewTipLoss = new QCheckBox(tr("New Tip Loss"));
    m_pctrlNewTipLoss->setEnabled(false);

    m_pctrlNewRootLoss = new QCheckBox(tr("New Root Loss"));
    m_pctrlNewRootLoss->setEnabled(false);

    m_pctrlCdReynolds = new QCheckBox(tr("Reynolds Drag Correction"));
    m_pctrlCdReynolds->setEnabled(false);

    QGridLayout *SimuShow = new QGridLayout;

    SimuShow->addWidget(m_pctrlTipLoss,1,1);
    SimuShow->addWidget(m_pctrlNewTipLoss,2,1);
    SimuShow->addWidget(m_pctrlRootLoss,3,1);
    SimuShow->addWidget(m_pctrlNewRootLoss,4,1);
    SimuShow->addWidget(m_pctrlCdReynolds,6,1);
    SimuShow->addWidget(m_pctrl3DCorrection,5,1);
    SimuShow->addWidget(m_pctrlInterpolation,7,1);
    SimuShow->addWidget(m_pctrlRho,1,2);
    SimuShow->addWidget(m_pctrlRhoVal,1,3);
    SimuShow->addWidget(m_pctrlVisc,2,2);
    SimuShow->addWidget(m_pctrlViscVal,2,3);
    SimuShow->addWidget(m_pctrlElements,3,2);
    SimuShow->addWidget(m_pctrlElementsVal,3,3);
    SimuShow->addWidget(m_pctrlIteration,4,2);
    SimuShow->addWidget(m_pctrlIterationVal,4,3);
    SimuShow->addWidget(m_pctrlEpsilon,5,2);
    SimuShow->addWidget(m_pctrlEpsilonVal,5,3);
    SimuShow->addWidget(m_pctrlRelax,6,2);
    SimuShow->addWidget(m_pctrlRelaxVal,6,3);

    QGroupBox *SimuGroup = new QGroupBox(tr("Simulation Parameters"));
    SimuGroup->setLayout(SimuShow);

    //---------------Wing Simulation Control Layout (bottom)------------------//
    QGridLayout *SequenceLayout = new QGridLayout;

    m_pctrlCreateBEM = new QPushButton(tr("Define Simulation"));
    m_pctrlDeleteBEM = new QPushButton(tr("Delete Simulation"));

    m_pctrlLSLabel = new QLabel(tr("Tip Speed Ratio Start:"));
    m_pctrlLSLabel->setAlignment(Qt::AlignLeft);
    m_pctrlLSLineEdit = new NumberEdit;
    m_pctrlLSLineEdit->setAlignment(Qt::AlignRight);
    m_pctrlLSLineEdit->setMinimum(double(0.00001));
    m_pctrlLELabel = new QLabel(tr("Tip Speed Ratio End:"));
    m_pctrlLELabel->setAlignment(Qt::AlignLeft);
    m_pctrlLELineEdit = new NumberEdit;
    m_pctrlLELineEdit->setAlignment(Qt::AlignRight);
    m_pctrlLELineEdit->setMinimum(double(0.01));
    m_pctrlLDLabel = new QLabel(tr("Tip Speed Ratio Delta:"));
    m_pctrlLDLabel->setAlignment(Qt::AlignLeft);
    m_pctrlLDLineEdit = new NumberEdit;
    m_pctrlLDLineEdit->setAlignment(Qt::AlignRight);
    m_pctrlLDLineEdit->setMinimum(double(0.1));

    QLabel *m_pctrlWindSpeedLabel = new QLabel(tr("@ Wind Speed of: ="));
    m_pctrlWindspeed = new NumberEdit;
    m_pctrlWindspeed->setAlignment(Qt::AlignRight);
    m_pctrlWindspeed->setMinimum(double(0.01));

    m_pctrlStartBEM = new QPushButton(tr("Start Simulation"));

    QVBoxLayout *SeqLay = new QVBoxLayout;
    QHBoxLayout *createDeleteLayout = new QHBoxLayout;
    createDeleteLayout->addWidget(m_pctrlCreateBEM);
    createDeleteLayout->addWidget(m_pctrlDeleteBEM);


    SequenceLayout->addWidget(m_pctrlLSLabel,1,1);
    SequenceLayout->addWidget(m_pctrlLELabel,2,1);
    SequenceLayout->addWidget(m_pctrlLDLabel,3,1);
    SequenceLayout->addWidget(m_pctrlLSLineEdit,1,2);
    SequenceLayout->addWidget(m_pctrlLELineEdit,2,2);
    SequenceLayout->addWidget(m_pctrlLDLineEdit,3,2);
    SequenceLayout->addWidget(m_pctrlWindSpeedLabel,4,1);
    SequenceLayout->addWidget(m_pctrlWindspeed,4,2);

    SeqLay->addLayout(createDeleteLayout);
    SeqLay->addLayout(SequenceLayout);
    SeqLay->addWidget(m_pctrlStartBEM);

    QGroupBox *AnalysisGroup = new QGroupBox(tr("Analysis Settings"));
    AnalysisGroup->setLayout(SeqLay);

    /////////-----------Turbine Simulation Control Layout (Bottom) ---------------//

    m_pctrlWind1 = new NumberEdit;
    m_pctrlWind1->setMinimum(double(0.01));
    m_pctrlWind1Label = new QLabel(tr("V Start = "));
    m_pctrlWind2 = new NumberEdit;
    m_pctrlWind2->setMinimum(double(0.01));
    m_pctrlWind2Label = new QLabel(tr("V End = "));
    m_pctrlWindDelta= new NumberEdit;
    m_pctrlWindDelta->setMinimum(double(0.1));

    m_pctrlWindDeltaLabel = new QLabel(tr("V Delta = "));
    m_pctrlDefineTurbineSim = new QPushButton(tr("Define Simulation"));
    m_pctrlStartTurbineSim = new QPushButton(tr("Start Simulation"));
    m_pctrlDeleteTBEM = new QPushButton(tr("Delete Simulation"));

    speed1 = new QLabel;
    speed2 = new QLabel;
    speed3 = new QLabel;

    SeqLay = new QVBoxLayout;
    createDeleteLayout = new QHBoxLayout;
    createDeleteLayout->addWidget(m_pctrlDefineTurbineSim);
    createDeleteLayout->addWidget(m_pctrlDeleteTBEM);

    QGridLayout *WindLayout = new QGridLayout;
    WindLayout->addWidget(m_pctrlWind1Label,1,1);
    WindLayout->addWidget(m_pctrlWind1,1,2);
    WindLayout->addWidget(speed1,1,3);
    WindLayout->addWidget(m_pctrlWind2Label,2,1);
    WindLayout->addWidget(m_pctrlWind2,2,2);
    WindLayout->addWidget(speed2,2,3);
    WindLayout->addWidget(m_pctrlWindDeltaLabel,3,1);
    WindLayout->addWidget(m_pctrlWindDelta,3,2);
    WindLayout->addWidget(speed3,3,3);

    SeqLay->addLayout(createDeleteLayout);
    SeqLay->addLayout(WindLayout);
    SeqLay->addWidget(m_pctrlStartTurbineSim);

    QVBoxLayout *StretchLayout = new QVBoxLayout;
    StretchLayout->addLayout(SeqLay);
    StretchLayout->addStretch(1000);

    QGroupBox *WindGroup = new QGroupBox(tr("Analysis Settings"));
    WindGroup->setLayout(StretchLayout);

    //_____________/////Characteristics Simulation Control///________________

    WindStart = new NumberEdit;
    WindStart->setMinimum(double(0.01));
    QLabel *WindS = new QLabel; WindS->setText(tr("Start = "));
    WindEnd = new NumberEdit;
    WindEnd->setMinimum(double(0.01));
    QLabel *WindE = new QLabel; WindE->setText(tr("End = "));
    WindDelta = new NumberEdit;
    WindDelta->setMinimum(double(0.01));
    QLabel *WindD = new QLabel; WindD->setText(tr("Delta = "));

    RotStart = new NumberEdit;
    RotStart->setMinimum(double(0.01));
    QLabel *RotS = new QLabel; RotS->setText(tr("Start = "));
    RotEnd = new NumberEdit;
    RotEnd->setMinimum(double(0.01));
    QLabel *RotE = new QLabel; RotE->setText(tr("End = "));
    RotDelta = new NumberEdit;
    RotDelta->setMinimum(double(0.01));
    QLabel *RotD = new QLabel; RotD->setText(tr("Delta = "));


    PitchStart = new NumberEdit;
    QLabel *PitchS = new QLabel; PitchS->setText(tr("Start = "));
    PitchEnd = new NumberEdit;
    QLabel *PitchE = new QLabel; PitchE->setText(tr("End = "));
    PitchDelta = new NumberEdit;
    QLabel *PitchD = new QLabel; PitchD->setText(tr("Delta = "));
    PitchDelta->setMinimum(double(0.01));

    WSpeed1 = new QLabel;
    WSpeed2 = new QLabel;
    WSpeed3 = new QLabel;

    WindFixed = new QCheckBox(tr("Fix"));
    RotFixed = new QCheckBox(tr("Fix"));
    PitchFixed = new QCheckBox(tr("Fix"));


    QLabel *RUnit1 = new QLabel; RUnit1->setText((tr("1/min")));
    QLabel *RUnit2 = new QLabel; RUnit2->setText((tr("1/min")));
    QLabel *RUnit3 = new QLabel; RUnit3->setText((tr("1/min")));

    QLabel *PUnit1 = new QLabel; PUnit1->setText((tr("  deg  ")));
    QLabel *PUnit2 = new QLabel; PUnit2->setText((tr("  deg  ")));
    QLabel *PUnit3 = new QLabel; PUnit3->setText((tr("  deg  ")));

    QGridLayout *WindParamsLayout = new QGridLayout;
    WindParamsLayout->addWidget(WindS,1,1);
    WindParamsLayout->addWidget(WindStart,1,2);
    WindParamsLayout->addWidget(WSpeed1,1,3);
    WindParamsLayout->addWidget(WindE,2,1);
    WindParamsLayout->addWidget(WindEnd,2,2);
    WindParamsLayout->addWidget(WSpeed2,2,3);
    WindParamsLayout->addWidget(WindD,3,1);
    WindParamsLayout->addWidget(WindDelta,3,2);
    WindParamsLayout->addWidget(WSpeed3,3,3);
    WindParamsLayout->addWidget(WindFixed,1,4);


	QGroupBox *WUnitGroup = new QGroupBox;
	WUnitGroup->setLayout(WindParamsLayout);
    WUnitGroup->setTitle(tr("Wind Speed Range"));


    QGridLayout *RotParamsLayout = new QGridLayout;
    RotParamsLayout->addWidget(RotS,1,1);
    RotParamsLayout->addWidget(RotStart,1,2);
    RotParamsLayout->addWidget(RUnit1,1,3);
    RotParamsLayout->addWidget(RotE,2,1);
    RotParamsLayout->addWidget(RotEnd,2,2);
    RotParamsLayout->addWidget(RUnit2,2,3);
    RotParamsLayout->addWidget(RotD,3,1);
    RotParamsLayout->addWidget(RotDelta,3,2);
    RotParamsLayout->addWidget(RUnit3,3,3);
    RotParamsLayout->addWidget(RotFixed,1,4);


    QGroupBox *RUnitGroup = new QGroupBox;
    RUnitGroup->setLayout(RotParamsLayout);
    RUnitGroup->setTitle(tr("Rotational Speed Range"));


    QGridLayout *PitchParamsLayout = new QGridLayout;
    PitchParamsLayout->addWidget(PitchS,1,1);
    PitchParamsLayout->addWidget(PitchStart,1,2);
    PitchParamsLayout->addWidget(PUnit1,1,3);
    PitchParamsLayout->addWidget(PitchE,2,1);
    PitchParamsLayout->addWidget(PitchEnd,2,2);
    PitchParamsLayout->addWidget(PUnit2,2,3);
    PitchParamsLayout->addWidget(PitchD,3,1);
    PitchParamsLayout->addWidget(PitchDelta,3,2);
    PitchParamsLayout->addWidget(PUnit3,3,3);
    PitchParamsLayout->addWidget(PitchFixed,1,4);


    QGroupBox *PUnitGroup = new QGroupBox;
    PUnitGroup->setLayout(PitchParamsLayout);
    PUnitGroup->setTitle(tr("Pitch Range"));

    CreateCharSim = new QPushButton(tr("Define Simulation"));
    StartCharSim = new QPushButton(tr("Start Simulation"));
    m_pctrlDeleteCBEM = new QPushButton(tr("Delete Simulation"));

    createDeleteLayout = new QHBoxLayout;
    createDeleteLayout->addWidget(CreateCharSim);
    createDeleteLayout->addWidget(m_pctrlDeleteCBEM);


    QVBoxLayout *CharLayout = new QVBoxLayout;
    CharLayout->addLayout(createDeleteLayout);
    CharLayout->addWidget(WUnitGroup);
    CharLayout->addWidget(RUnitGroup);
    CharLayout->addWidget(PUnitGroup);
    CharLayout->addWidget(StartCharSim);


    QGroupBox *CharGroup = new QGroupBox;
    CharGroup->setLayout(CharLayout);
	CharGroup->setTitle(tr("Analysis Settings"));




    //_______________________Curve Display______________________//

    QHBoxLayout *CurveDisplay = new QHBoxLayout;
    m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
    m_pctrlShowPoints = new QCheckBox(tr("Points"));
    m_pctrlShowOpPoint= new QCheckBox(tr("Selected Op Point"));
    CurveDisplay->addWidget(m_pctrlShowCurve);
    CurveDisplay->addWidget(m_pctrlShowPoints);
    CurveDisplay->addWidget(m_pctrlShowOpPoint);

    QVBoxLayout *CurveGroup = new QVBoxLayout;
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


    CurveGroup->addLayout(CurveDisplay);
    CurveGroup->addLayout(CurveStyleLayout);
    CurveGroup->addStretch(1);

    QGroupBox *CurveBox = new QGroupBox(tr("Graph Curve Settings"));
    CurveBox->setLayout(CurveGroup);

    //////////////final Layout//////////////

    middleControls = new QStackedWidget;
    middleControls->addWidget(AnalysisGroup);
    middleControls->addWidget(WindGroup);
    middleControls->addWidget(CharGroup);






    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(SimuGroup);
    mainLayout->addWidget(middleControls);
    mainLayout->addWidget(CurveBox);
    mainLayout->insertStretch(3,100000);;
    setLayout(mainLayout);
}


void SimuWidget::Connect()
{

    connect(m_pctrlDeleteCBEM, SIGNAL(clicked()), g_qbem, SLOT(OnDeleteCharSim()));
    connect(m_pctrlDeleteTBEM, SIGNAL(clicked()), g_qbem, SLOT(OnDeleteTurbineSim()));
    connect(m_pctrlDeleteBEM, SIGNAL(clicked()), g_qbem, SLOT(OnDeleteRotorSim()));

    connect(m_pctrlStartBEM, SIGNAL(clicked()), this,SLOT(OnStartBEM()));
    connect(m_pctrlCreateBEM, SIGNAL(clicked()), this,SLOT(OnCreateBEM()));    
    connect(m_pctrlStartTurbineSim, SIGNAL(clicked()), SLOT(OnStartTurbineSimulation()));
    connect(m_pctrlDefineTurbineSim, SIGNAL(clicked()), SLOT(OnSetTurbineSimulationParameters()));

    connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
    connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
    connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
    connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowPoints()));
    connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));
    connect(m_pctrlShowOpPoint, SIGNAL(clicked()), this, SLOT(OnShowOpPoint()));
    connect(CreateCharSim, SIGNAL(clicked()), this, SLOT(OnCreateCharSim()));
    connect(StartCharSim, SIGNAL(clicked()), this, SLOT(OnStartCharSim()));
    connect(WindFixed, SIGNAL(clicked()), this, SLOT(OnFixCLicked()));
    connect(PitchFixed, SIGNAL(clicked()), this, SLOT(OnFixCLicked()));
    connect(RotFixed, SIGNAL(clicked()), this, SLOT(OnFixCLicked()));
    connect(PitchDelta, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(WindDelta, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(RotDelta, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(PitchStart, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(WindStart, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(RotStart, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(PitchEnd, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(WindEnd, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(RotEnd, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlWind1, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlWind2, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlWindDelta, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlLSLineEdit, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlLELineEdit, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));
    connect(m_pctrlLDLineEdit, SIGNAL(editingFinished()), this, SLOT(OnCheckDeltas()));

}

void SimuWidget::OnCheckDeltas()
{
    QBEM *pBEM = (QBEM *) m_pBEM;


    if (WindEnd->getValue()-WindStart->getValue()<=0) WindEnd->setValue(WindStart->getValue()+20);
    if (PitchEnd->getValue()-PitchStart->getValue()<=0) PitchEnd->setValue(PitchStart->getValue()+10);
    if (RotEnd->getValue()-RotStart->getValue()<=0) RotEnd->setValue(RotStart->getValue()+400);

    if (m_pctrlLELineEdit->getValue()-m_pctrlLSLineEdit->getValue()<=0) m_pctrlLELineEdit->setValue(m_pctrlLSLineEdit->getValue()+10);
    if (m_pctrlWind2->getValue()-m_pctrlWind1->getValue()<=0) m_pctrlWind2->setValue(m_pctrlWind1->getValue()+20);


    if (fabs(PitchStart->getValue()-PitchEnd->getValue())/PitchDelta->getValue()>99) PitchDelta->setValue(fabs(PitchStart->getValue()-PitchEnd->getValue())/99);
    if (fabs(RotStart->getValue()-RotEnd->getValue())/RotDelta->getValue()>99) RotDelta->setValue(fabs(RotStart->getValue()-RotEnd->getValue())/99);
    if (fabs(WindStart->getValue()-WindEnd->getValue())/WindDelta->getValue()>99) WindDelta->setValue(fabs(WindStart->getValue()-WindEnd->getValue())/99);

    pBEM->dlg_windstart2 = WindStart->getValue();
    pBEM->dlg_windend2 = WindEnd->getValue();
    pBEM->dlg_winddelta2 = WindDelta->getValue();

    pBEM->dlg_pitchstart = PitchStart->getValue();
    pBEM->dlg_pitchend = PitchEnd->getValue();
    pBEM->dlg_pitchdelta = PitchDelta->getValue();

    pBEM->dlg_rotstart = RotStart->getValue();
    pBEM->dlg_rotend = RotEnd->getValue();
    pBEM->dlg_rotdelta = RotDelta->getValue();

    pBEM->dlg_windstart = m_pctrlWind1->getValue();
    pBEM->dlg_windend = m_pctrlWind2->getValue();
    pBEM->dlg_winddelta = m_pctrlWindDelta->getValue();

    pBEM->dlg_lambdastart = m_pctrlLSLineEdit->getValue();
    pBEM->dlg_lambdaend = m_pctrlLELineEdit->getValue();
    pBEM->dlg_lambdadelta = m_pctrlLDLineEdit->getValue();
    pBEM->dlg_windspeed = m_pctrlWindspeed->getValue();



}

void SimuWidget::OnCreateCharSim()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCreateCharacteristicSimulation();
}

void SimuWidget::OnStartCharSim()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnStartCharacteristicSimulation();
}

void SimuWidget::OnStartBEM()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnStartRotorSimulation();


}

void SimuWidget::OnCreateBEM()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCreateRotorSimulation();
}

void SimuWidget::OnStartTurbineSimulation()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnStartTurbineSimulation();;

}

void SimuWidget::OnSetTurbineSimulationParameters()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCreateTurbineSimulation();
}

void SimuWidget::OnCurveStyle(int index)
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCurveStyle(index);
}

void SimuWidget::OnCurveColor()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCurveColor();
}

void SimuWidget::OnFixCLicked()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->CheckButtons();
}

void SimuWidget::OnCurveWidth(int index)
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnCurveWidth(index);
}

void SimuWidget::OnShowPoints()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnShowPoints();
}

void SimuWidget::OnShowCurve()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnShowCurve();
}

void SimuWidget::OnShowOpPoint()
{
    QBEM *pBEM = (QBEM *) m_pBEM;

    pBEM->OnShowOpPoint();
}


