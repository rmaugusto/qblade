#include "TurDmsTurbineCreatorDialog.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QDebug>

#include "../Misc/NumberEdit.h"
#include "../StoreAssociatedComboBox.h"
#include "../Store.h"
#include "../XBEM/TData.h"
#include "../MainFrame.h"
#include "TurDmsModule.h"
#include "../XBEM/PrescribedValuesDlg.h"
#include "../Globals.h"
#include "../ParameterGrid.h"

typedef Parameter::TData P;


TurDmsTurbineCreatorDialog::TurDmsTurbineCreatorDialog(TData *presetTurbine, TurDmsModule *module)
	: m_module(module),
	  m_editedTurbine(presetTurbine)
{
	setWindowTitle(tr("DMS Turbine"));	
	
	QGroupBox *groupBox = new QGroupBox ("Turbine Specification");
	m_contentVBox->insertWidget(0, groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::Name, LineEdit, new QLineEdit (), "Name of Turbine:", "VAWT Turbine");
			grid->addEdit(P::Blade, RotorBox, new RotorComboBox(&g_verticalRotorStore), "Turbine Blade:", (quintptr)NULL);
			grid->addEdit(P::VCutIn, NumberEditType, new NumberEdit(), "V Cut In []:", 0, SPEED);
			grid->addEdit(P::VCutOut, NumberEditType, new NumberEdit(), "V Cut Out []:", 0, SPEED);
			grid->addEdit(P::TurbineOffset, NumberEditType, new NumberEdit(), "Turbine Offset []:", 0, LENGTH);
			grid->addEdit(P::LossFactor, NumberEditType, new NumberEdit(), "Loss Factor:", 0);
			grid->addEdit(P::FixedLosses, NumberEditType, new NumberEdit(), "Fixed Losses []:", 0, POWER);
	groupBox = new QGroupBox ("Rotational Speed");
	m_contentVBox->insertWidget(1, groupBox);
		grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			int gridRowCount = 0;
			QLabel *label = new QLabel ("Transmission:");
			grid->addWidget(label, gridRowCount, 0);
			m_transmissionButtonGroup = new QButtonGroup ();
			connect(m_transmissionButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(onTransmissionChanged(int,bool)));
			QRadioButton *radioButton = new QRadioButton ("Single");
			m_transmissionButtonGroup->addButton(radioButton, 0);
			grid->addWidget(radioButton, gridRowCount++, 1);
			radioButton = new QRadioButton ("Variable (Optimal TSR)");
			m_transmissionButtonGroup->addButton(radioButton, 1);
			grid->addWidget(radioButton, gridRowCount++, 1);
			radioButton = new QRadioButton ("Prescribed");
			m_transmissionButtonGroup->addButton(radioButton, 2);
			grid->addWidget(radioButton, gridRowCount++, 1);
			
			m_singleWidget = new QWidget ();
			grid->addWidget(m_singleWidget, gridRowCount++, 0, 1, 2);
				ParameterGrid<P> *innerGrid = new ParameterGrid<P>(this);
				m_singleWidget->setLayout(innerGrid);
					innerGrid->addEdit(P::RotationalSpeed, NumberEditType, new NumberEdit(), "Rot. Speed [rpm]:", 0);
			
			m_variableWidget = new QWidget ();
			grid->addWidget(m_variableWidget, gridRowCount++, 0, 1, 2);
				innerGrid = new ParameterGrid<P>(this);
				m_variableWidget->setLayout(innerGrid);
					innerGrid->addEdit(P::RotationalSpeedMin, NumberEditType, new NumberEdit(), "Rot. Speed Min [rpm]:", 0);
					innerGrid->addEdit(P::RotationalSpeedMax, NumberEditType, new NumberEdit(), "Rot. Speed Max [rpm]:", 0);
					innerGrid->addEdit(P::TSR, NumberEditType, new NumberEdit(), "TSR at Design Point:", 0);
					
			m_prescribedWidget = new QWidget ();
			grid->addWidget(m_prescribedWidget, gridRowCount++, 0, 1, 2);
				innerGrid = new ParameterGrid<P>(this);
				m_prescribedWidget->setLayout(innerGrid);
					m_prescribeSpeedButton = new QPushButton("Prescribe Rot. Speed");
					connect(m_prescribeSpeedButton, SIGNAL(clicked()), this, SLOT(onPrescribeButtonClicked()));
					innerGrid->addWidget(m_prescribeSpeedButton, 0, 1);
			
	setUnitContainingLabels();
	initView();
}

void TurDmsTurbineCreatorDialog::initView() {
	loadObject(m_editedTurbine);
	
	m_singleWidget->hide();
	m_variableWidget->hide();
	m_prescribedWidget->hide();
	
	if (m_editedTurbine) {
		if (m_editedTurbine->isFixed) {
			m_transmissionButtonGroup->button(0)->setChecked(true);
		} else if (m_editedTurbine->isVariable) {
			m_transmissionButtonGroup->button(1)->setChecked(true);
		} else {
			m_transmissionButtonGroup->button(2)->setChecked(true);
		}
		m_prescribedWindSpeed = m_editedTurbine->rotwindspeeds;
		m_prescribedRotorSpeed = m_editedTurbine->rotspeeds;		
	} else {
		get<QLineEdit>(P::Name)->setText(g_verttdataStore.getNextName("VAWT Turbine"));
		m_transmissionButtonGroup->button(0)->setChecked(true);
		m_prescribedWindSpeed.clear();
		m_prescribedRotorSpeed.clear();
	}
}

void TurDmsTurbineCreatorDialog::onTransmissionChanged(int buttonId, bool checked) {
	switch (buttonId) {
	case 0: m_singleWidget->setVisible(checked); break;
	case 1: m_variableWidget->setVisible(checked); break;
	case 2: m_prescribedWidget->setVisible(checked); break;
	}
}

void TurDmsTurbineCreatorDialog::onPrescribeButtonClicked() {
	// copied from BEM::OnPrescribeRot
	PrescribedValuesDlg *pTableWidget = new PrescribedValuesDlg;
	
	QSizePolicy szPolicyMinimum;
    szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Expanding);
    szPolicyMinimum.setVerticalPolicy(QSizePolicy::Expanding);
	
	pTableWidget->setSizePolicy(szPolicyMinimum);
    pTableWidget->editrot=true;
    pTableWidget->setModal(true);

	pTableWidget->cut_in = getParameter(P::VCutIn).toDouble();
    pTableWidget->cut_out = getParameter(P::VCutOut).toDouble();

	if (m_prescribedWindSpeed.isEmpty()) {
		pTableWidget->OnReset();
	} else {
		pTableWidget->windspeeds = m_prescribedWindSpeed;
        pTableWidget->value = m_prescribedRotorSpeed;
	}

    pTableWidget->InitDialog();

    if(QDialog::Accepted == pTableWidget->exec()) {
        m_prescribedWindSpeed = pTableWidget->windspeeds;
        m_prescribedRotorSpeed = pTableWidget->value;
    }
	
	pTableWidget->deleteLater();
}

void TurDmsTurbineCreatorDialog::onCreateButtonClicked() {
	double Rot1Value;  // needed because of stupid double usage of TData::Rot1
	if (m_transmissionButtonGroup->checkedId() == 0) {
		Rot1Value = getParameter(P::RotationalSpeed).toDouble();
	} else {
		Rot1Value = getParameter(P::RotationalSpeedMin).toDouble();
	}
	
	TData *newTurbine = new TData (this);
	
	newTurbine->rotwindspeeds = m_prescribedWindSpeed;
	newTurbine->rotspeeds = m_prescribedRotorSpeed;
	
	newTurbine->isFixed = (m_transmissionButtonGroup->checkedId() == 0);
	newTurbine->isVariable = (m_transmissionButtonGroup->checkedId() == 1);
	newTurbine->isPrescribedRot = (m_transmissionButtonGroup->checkedId() == 2);
	
	newTurbine->Rot1 = Rot1Value;
	
	if (g_verttdataStore.add(newTurbine)) {
		m_module->setShownSimulationParent(newTurbine);
		accept();  // leave dialog only if adding was successful
	}
}
