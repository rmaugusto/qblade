#include "WindFieldCreatorDialog.h"

#include <QGroupBox>
#include <QDebug>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>

#include "../ParameterGrid.h"
#include "../Misc/NumberEdit.h"
#include "WindField.h"
#include "../Store.h"
#include "WindFieldModule.h"

typedef Parameter::Windfield P;


void WindFieldCreatorDialog::WindFieldThread::run() {
	 windField->calculateWindField();
}

WindFieldCreatorDialog::WindFieldCreatorDialog(WindField *windfield, WindFieldModule *module)
	: m_module(module),
	  m_editedWindfield(windfield)
{
	setWindowTitle(tr("Windfield"));
	
	QGroupBox *groupBox = new QGroupBox ("Windfield Parameter");
	m_contentVBox->insertWidget(0, groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::Name, LineEdit, new QLineEdit (), "Name:", "Windfield");
			grid->addEdit(P::Time, NumberEditType, new NumberEdit(), "Time [s]:", 60);
			get<NumberEdit>(P::Time)->setMinimum(0.001);
			grid->addEdit(P::Timesteps, NumberEditType, new NumberEdit(NumberEdit::Standard, 0, 1), "Timesteps:", 100);
			grid->addEdit(P::Points, NumberEditType, new NumberEdit(NumberEdit::Standard, 0, 3), "Points per Direction:", 20);
	groupBox = new QGroupBox ("Simulation Parameter");
	m_contentVBox->insertWidget(1, groupBox);
		grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::RotorRadius, NumberEditType, new NumberEdit(), "Rotor Radius []:", 30, LENGTH);
			get<NumberEdit>(P::RotorRadius)->setMinimum(0.001);
			grid->addEdit(P::HubHeight, NumberEditType, new NumberEdit(), "Hub Height []:", 60, LENGTH);
			get<NumberEdit>(P::HubHeight)->setMinimum(0);
			grid->addEdit(P::WindSpeed, NumberEditType, new NumberEdit(), "Mean Wind Speed []:", 13, SPEED);
			get<NumberEdit>(P::WindSpeed)->setMinimum(0);
            grid->addEdit(P::MeasurementHeight, NumberEditType, new NumberEdit(), "Measurement Height []:", 10, LENGTH);
            get<NumberEdit>(P::MeasurementHeight)->setMinimum(0.001);
			grid->addEdit(P::Turbulence, NumberEditType, new NumberEdit(), "Turbulence Intensity [%]:", 10);
			get<NumberEdit>(P::Turbulence)->setRange(0, 100);
			grid->addEdit(P::ShearLayer, CheckBox, new QCheckBox("include"), "Shear Layer:", true);
			connect(get<QCheckBox>(P::ShearLayer), SIGNAL(toggled(bool)), this, SLOT(onShearLayerChanged(bool)));

			grid->addEdit(P::RoughnessLength, NumberEditType, new NumberEdit(), "Roughness Length []:", 0.01, LENGTH);
			
	setUnitContainingLabels();
	initView();
}

void WindFieldCreatorDialog::onCreateButtonClicked() {
	QString errorMessage = "";
	if (get<NumberEdit>(P::RotorRadius)->getValue() >= get<NumberEdit>(P::HubHeight)->getValue()) {
		errorMessage += "\n- Rotor Radius must be less then Hub Height.";
	}
	if (get<NumberEdit>(P::Turbulence)->getValue() == 0) {
		errorMessage += "\n- Turbulence must not be 0.";
	}
	if (get<NumberEdit>(P::RoughnessLength)->getValue() == 0) {
		errorMessage += "\n- Roughness Length must not be 0.";
	}
	if (errorMessage != "") {
		QMessageBox::critical(this, "Windfield", QString("Could not create Windfield due to following error(s):\n" +
														 errorMessage), QMessageBox::Ok);
		return;
	}
	
	m_cancelCalculation = false;
	m_progressStep = 0;
	m_progressStepShown = 0;
	const int progressSteps = pow(get<NumberEdit>(P::Points)->getValue(),2) * get<NumberEdit>(P::Timesteps)->getValue() * 1.5;
	m_progressDialog = new QProgressDialog ("Calculating Windfield... please wait", "Cancel", 0, progressSteps+1);
	m_progressDialog->setModal(true);
	m_progressDialog->setValue(0);
	m_progressDialog->show();
	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(onProgressDialogCanceled()));
	
	WindField *windfield = new WindField (this, &m_cancelCalculation);
	connect(windfield, SIGNAL(updateProgress()), this, SLOT(onWindFieldProgress()));
	
	m_windFieldThread =  new WindFieldThread ();
	m_windFieldThread->windField = windfield;
	connect(m_windFieldThread, SIGNAL(finished()), this, SLOT(onWindFieldFinished()), Qt::QueuedConnection);
	m_windFieldThread->start(QThread::LowPriority);
}

void WindFieldCreatorDialog::onShearLayerChanged(bool state) {
	get<NumberEdit>(P::MeasurementHeight)->setEnabled(state);
	get<NumberEdit>(P::RoughnessLength)->setEnabled(state);
}

void WindFieldCreatorDialog::initView() {
	loadObject(m_editedWindfield);
	if (!m_editedWindfield) {
		get<QLineEdit>(P::Name)->setText(g_windFieldStore.getNextName("Windfield"));
	}
	
	onShearLayerChanged(get<QCheckBox>(P::ShearLayer)->isChecked());
}

void WindFieldCreatorDialog::onWindFieldProgress() {
	m_progressStep++;
	if (m_progressStepShown >= m_progressStep - 30) {  // stackoverflow can occure if the difference is too large
		m_progressDialog->setValue(m_progressStep);
	}
	m_progressStepShown++;
}

void WindFieldCreatorDialog::onWindFieldFinished() {
	WindField *windField = m_windFieldThread->windField;
	
	m_progressDialog->deleteLater();
	m_windFieldThread->deleteLater();
	
	if (windField->isValid()) {
		if (g_windFieldStore.add(windField)) {
			m_module->setShownWindField(windField);
			m_module->reportGLChange();
			accept();
		}
	} else {
		delete windField;  // needed, because WindFieldThread does not care for the WindField
	}
}

void WindFieldCreatorDialog::onProgressDialogCanceled() {
	m_cancelCalculation = true;
}
