#include "SimulationCreatorDialog.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QSettings>

#include "ParameterGrid.h"


template <class ParameterGroup>
SimulationCreatorDialog<ParameterGroup>::SimulationCreatorDialog() {
	QHBoxLayout *hBox = new QHBoxLayout ();
	m_contentVBox->insertLayout(0, hBox);
		m_leftContentVBox = new QVBoxLayout ();
		hBox->addLayout(m_leftContentVBox);
		m_rightContentVBox = new QVBoxLayout ();
		hBox->addLayout(m_rightContentVBox);
	
	QPushButton *button = new QPushButton ("Restore Default");
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onRestoreDefaultClicked()));
	m_buttonHBox->insertWidget (1, button);
}

template <class ParameterGroup>
QGroupBox* SimulationCreatorDialog<ParameterGroup>::constructParameterBox(QString defaultName) {
	QGroupBox *groupBox = new QGroupBox ("Parameters");
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::Name, this->LineEdit, new QLineEdit(), "Name of Simulation:", defaultName);
			grid->addEdit(P::Rho, this->NumberEditType, new NumberEdit (), "Rho [kg/m^3]:", 1.225);
			grid->addEdit(P::Viscosity, this->NumberEditType, new NumberEdit (), "Viscosity [kg/(m*s)]:", 0.00001647);
			grid->addEdit(P::Discretize, this->NumberEditType, new NumberEdit (), "Discretize Blade into N Elements:", 40);
			this->template get<NumberEdit>(P::Discretize)->setAutomaticPrecision(0);
			grid->addEdit(P::MaxIterations, this->NumberEditType, new NumberEdit (), "Max Number of Iterations:", 100);
			this->template get<NumberEdit>(P::MaxIterations)->setAutomaticPrecision(0);
			grid->addEdit(P::MaxEpsilon, this->NumberEditType, new NumberEdit (), "Max Epsilon for Convergence:", 0.001);
			grid->addEdit(P::RelaxFactor, this->NumberEditType, new NumberEdit (), "Relax. Factor:", 0.35);
			
	return groupBox;
}

template <class ParameterGroup>
QGroupBox* SimulationCreatorDialog<ParameterGroup>::constructCorrectionsBox() {
	QGroupBox *groupBox = new QGroupBox ("Corrections");
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addEdit(P::TipLoss, this->CheckBox, new QCheckBox("enable"), "Tip Loss:", false);
			grid->addEdit(P::VariableInduction, this->CheckBox, new QCheckBox("enable"), "Variable Induction Factors:", false);
			
			return groupBox;
}

template <class ParameterGroup>
void SimulationCreatorDialog<ParameterGroup>::onRestoreDefaultClicked() {
	this->loadObject(NULL);
}

template <class ParameterGroup>
void SimulationCreatorDialog<ParameterGroup>::loadValuesFromSettings() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");
	
	this->template get<NumberEdit>(P::Rho)->setValue(settings.value("rho", 1.225).toDouble());
	this->template get<NumberEdit>(P::Viscosity)->setValue(settings.value("viscosity", 0.00001647).toDouble());
	this->template get<NumberEdit>(P::Discretize)->setValue(settings.value("discretize", 40).toDouble());
	this->template get<NumberEdit>(P::MaxIterations)->setValue(settings.value("maxIter", 100).toDouble());
	this->template get<NumberEdit>(P::MaxEpsilon)->setValue(settings.value("maxEpsilon", 0.001).toDouble());
	this->template get<NumberEdit>(P::RelaxFactor)->setValue(settings.value("relex", 0.35).toDouble());
	this->template get<QCheckBox>(P::TipLoss)->setChecked(settings.value("tipLoss", false).toBool());
	this->template get<QCheckBox>(P::VariableInduction)->setChecked(settings.value("varInduc", false).toBool());
	
	settings.endGroup();
}

template <class ParameterGroup>
void SimulationCreatorDialog<ParameterGroup>::saveValuesToSettings() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.beginGroup("defaultValues/DmsSimulationCreatorDialog");

	settings.setValue("rho", this->template get<NumberEdit>(P::Rho)->getValue());
	settings.setValue("viscosity", this->template get<NumberEdit>(P::Viscosity)->getValue());
	settings.setValue("discretize", this->template get<NumberEdit>(P::Discretize)->getValue());
	settings.setValue("maxIter", this->template get<NumberEdit>(P::MaxIterations)->getValue());
	settings.setValue("maxEpsilon", this->template get<NumberEdit>(P::MaxEpsilon)->getValue());
	settings.setValue("relex", this->template get<NumberEdit>(P::RelaxFactor)->getValue());
	settings.setValue("tipLoss", this->template get<QCheckBox>(P::TipLoss)->isChecked());
	settings.setValue("varInduc", this->template get<QCheckBox>(P::VariableInduction)->isChecked());
	
	settings.endGroup();
}


template class SimulationCreatorDialog<Parameter::TDMSData>;
template class SimulationCreatorDialog<Parameter::CDMSData>;
template class SimulationCreatorDialog<Parameter::DMSData>;
