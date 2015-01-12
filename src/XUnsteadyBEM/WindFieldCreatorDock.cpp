/****************************************************************************

	WindFieldCreator Class
		Copyright (C) 2012 Nikolai Moesus

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

#include "WindFieldCreatorDock.h"

#include <QMessageBox>
#include "../GlobalFunctions.h"

//#include "../Globals.h"
//#include "../XGlobals.h"
#include "WindFieldModule.h"
#include "../Store.h"


WindFieldCreatorDock::WindFieldCreatorDock (const QString & title, QMainWindow * parent, Qt::WindowFlags flags)
    : ScrolledDock (title, parent, flags)
{

    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);

	m_shownWindField = NULL;
	
	/* create the layout */
	m_stackedWidget = new QStackedWidget ();
	m_contentVBox->addWidget(m_stackedWidget);
	
	/* create the layout for view */
	QWidget *widget = new QWidget ();
	m_stackedWidget->addWidget(widget);
		QVBoxLayout *vBox = new QVBoxLayout ();
		widget->setLayout(vBox);
			QGroupBox *groupBox = new QGroupBox (tr("Windfield Name"));
			vBox->addWidget(groupBox);
				QGridLayout *grid = new QGridLayout ();
				groupBox->setLayout(grid);
					m_nameLabel = new QLabel ();
					grid->addWidget(m_nameLabel, 0, 0);
			groupBox = new QGroupBox (tr("Geometry Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					QLabel *label = new QLabel (tr("Rotor radius [m]:"));
					grid->addWidget (label, 0, 0);
					m_rotorRadiusLabel = new QLabel ();
					grid->addWidget (m_rotorRadiusLabel, 0, 1);
					label = new QLabel (tr("Hub height [m]:"));
					grid->addWidget (label, 1, 0);
					m_hubHeightLabel = new QLabel ();
					grid->addWidget (m_hubHeightLabel, 1, 1);
			groupBox = new QGroupBox (tr("Wind Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					label = new QLabel (tr("Mean wind speed u [m/s]:"));
					grid->addWidget (label, 0, 0);
					m_windSpeedLabel = new QLabel ();
					grid->addWidget (m_windSpeedLabel, 0, 1);
					label = new QLabel (tr("Turbulence intensity [%]:"));
					grid->addWidget (label, 1, 0);
					m_turbulenceLabel = new QLabel ();
					grid->addWidget (m_turbulenceLabel, 1, 1);
					m_shearLayerLabel = new QLabel (tr("Include Shear Layer"));
					grid->addWidget (m_shearLayerLabel, 2, 0, 1, 2);
					label = new QLabel (tr("Measurement height [m]:"));
					grid->addWidget (label, 3, 0);
					m_measurementHeightLabel = new QLabel ();
					grid->addWidget (m_measurementHeightLabel, 3, 1);
					label = new QLabel (tr("Roughness length z0 [m]:"));
					grid->addWidget (label, 4, 0);
					m_roughnessLabel = new QLabel ();
					grid->addWidget (m_roughnessLabel, 4, 1);
			groupBox = new QGroupBox (tr("Simulation Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					label = new QLabel (tr("Simulation time [s]:"));
					grid->addWidget (label, 0, 0);
					m_simulationTimeLabel = new QLabel ();
					grid->addWidget (m_simulationTimeLabel, 0, 1);
					label = new QLabel (tr("# of timesteps:"));
					grid->addWidget (label, 1, 0);
					m_timestepsLabel = new QLabel ();
					grid->addWidget (m_timestepsLabel, 1, 1);
					label = new QLabel (tr("# of points (in y&z):"));
					grid->addWidget (label, 2, 0);
					m_pointsLabel = new QLabel ();
					grid->addWidget (m_pointsLabel, 2, 1);
					label = new QLabel (tr("Highest Freq. [Hz]:"));
					grid->addWidget (label, 3, 0);
					m_highestFrequencyLabel = new QLabel ();
					grid->addWidget (m_highestFrequencyLabel, 3, 1);
					label = new QLabel (tr("Lowest Freq. [Hz]:"));
					grid->addWidget (label, 4, 0);
					m_lowestFrequencyLabel = new QLabel ();
					grid->addWidget (m_lowestFrequencyLabel, 4, 1);
					label = new QLabel (tr("Spatial stepsize [m]:"));
					grid->addWidget (label, 5, 0);
					m_resolutionLabel = new QLabel ();
					grid->addWidget (m_resolutionLabel, 5, 1);
			groupBox = new QGroupBox ();
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout(grid);
					m_renameButton = new QPushButton (tr("Rename"));
					connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
					grid->addWidget (m_renameButton, 0, 0);
					m_editCopyButton = new QPushButton (tr("Edit/Copy"));
					connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));					
					grid->addWidget (m_editCopyButton, 0, 1);
					m_deleteButton = new QPushButton (tr("Delete"));
					connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
					grid->addWidget (m_deleteButton, 1, 0);
					m_newButton = new QPushButton (tr("New"));
					connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
					grid->addWidget (m_newButton, 1, 1);
			
//				QHBoxLayout *hBox = new QHBoxLayout ();
//				groupBox->setLayout (hBox);
//					m_newButton = new QPushButton (tr("New"));
//					connect(m_newButton, SIGNAL(clicked(bool)), this, SLOT(onNewButtonClicked(bool)));
//					hBox->addWidget (m_newButton);
//					m_editButton = new QPushButton (tr("Edit"));
//					connect(m_editButton, SIGNAL(clicked(bool)), this, SLOT(onEditButtonClicked(bool)));					
//					hBox->addWidget (m_editButton);
//					m_deleteButton = new QPushButton (tr("Delete"));
//					connect(m_deleteButton, SIGNAL(clicked(bool)), this, SLOT(onDeleteButtonClicked(bool)));
//					hBox->addWidget (m_deleteButton);
	
	/* create the layout for create */
	widget = new QWidget ();
	m_stackedWidget->addWidget(widget);
		vBox = new QVBoxLayout ();
		widget->setLayout(vBox);
			groupBox = new QGroupBox (tr("Windfield Name"));
			vBox->addWidget(groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout(grid);
					m_nameEdit = new QLineEdit ();
					grid->addWidget(m_nameEdit, 0, 0);
			groupBox = new QGroupBox (tr("Geometry Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					label = new QLabel (tr("Rotor radius [m]:"));
					grid->addWidget (label, 0, 0);
					m_rotorRadiusEdit = new NumberEdit ();
					connect (m_rotorRadiusEdit, SIGNAL(editingFinished()),
																	this, SLOT(onPointsEditChange()));
					grid->addWidget (m_rotorRadiusEdit, 0, 1);
					label = new QLabel (tr("Hub height [m]:"));
					grid->addWidget (label, 1, 0);
					m_hubHeightEdit = new NumberEdit ();
					grid->addWidget (m_hubHeightEdit, 1, 1);
			groupBox = new QGroupBox (tr("Wind Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					label = new QLabel (tr("Mean wind speed u [m/s]:"));
					grid->addWidget (label, 0, 0);
					m_windSpeedEdit = new NumberEdit ();
					m_windSpeedEdit->setMinimum(0);
					grid->addWidget (m_windSpeedEdit, 0, 1);
					label = new QLabel (tr("Turbulence intensity [%]:"));
					grid->addWidget (label, 1, 0);
					m_turbulenceEdit = new NumberEdit ();
					m_turbulenceEdit->setRange(0, 100);
					grid->addWidget (m_turbulenceEdit, 1, 1);
					m_shearLayerCheckBox = new QCheckBox (tr("Include Shear Layer"));
					connect(m_shearLayerCheckBox, SIGNAL(stateChanged(int)),
															this, SLOT(onShearLayerCheckBoxChanged(int)));
					grid->addWidget (m_shearLayerCheckBox, 2, 0, 1, 2);
					label = new QLabel (tr("Measurement height [m]:"));
					grid->addWidget (label, 3, 0);
					m_measurementHeightEdit = new NumberEdit ();
					grid->addWidget (m_measurementHeightEdit, 3, 1);
					label = new QLabel (tr("Roughness length z0 [m]:"));
					grid->addWidget (label, 4, 0);
					m_roughnessEdit = new NumberEdit ();
					grid->addWidget (m_roughnessEdit, 4, 1);
			groupBox = new QGroupBox (tr("Simulation Parameter"));
			vBox->addWidget (groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout (grid);
					label = new QLabel (tr("Simulation time [s]:"));
					grid->addWidget (label, 0, 0);
					m_simulationTimeEdit = new NumberEdit ();
					connect (m_simulationTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(onTimeEditsChange()));
					grid->addWidget (m_simulationTimeEdit, 0, 1);
					label = new QLabel (tr("# of timesteps:"));
					grid->addWidget (label, 1, 0);
					m_timestepsEdit = new NumberEdit ();
					connect (m_timestepsEdit, SIGNAL(valueChanged(double)), this, SLOT(onTimeEditsChange()));
					grid->addWidget (m_timestepsEdit, 1, 1);
					label = new QLabel (tr("# of points (in y&z):"));
					grid->addWidget (label, 2, 0);
					m_pointsEdit = new NumberEdit ();
					connect (m_pointsEdit, SIGNAL(valueChanged(double)), this, SLOT(onPointsEditChange()));
					grid->addWidget (m_pointsEdit, 2, 1);
					label = new QLabel (tr("Highest Freq. [Hz]:"));
					grid->addWidget (label, 3, 0);
					m_highestFrequencyEdit = new NumberEdit ();
					m_highestFrequencyEdit->setAutomaticPrecision(4);
					grid->addWidget (m_highestFrequencyEdit, 3, 1);
					label = new QLabel (tr("Lowest Freq. [Hz]:"));
					grid->addWidget (label, 4, 0);
					m_lowestFrequencyEdit = new NumberEdit ();
					m_lowestFrequencyEdit->setAutomaticPrecision(4);
					grid->addWidget (m_lowestFrequencyEdit, 4, 1);
					label = new QLabel (tr("Spatial stepsize [m]:"));
					grid->addWidget (label, 5, 0);
					m_resolutionEdit = new NumberEdit ();
					grid->addWidget (m_resolutionEdit, 5, 1);
			groupBox = new QGroupBox ();
			vBox->addWidget (groupBox);
				QHBoxLayout *hBox = new QHBoxLayout ();
				groupBox->setLayout (hBox);
					m_cancelButton = new QPushButton (tr("Cancel"));
					connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
					hBox->addWidget (m_cancelButton);
					m_createButton = new QPushButton (tr("Create"));
					connect(m_createButton, SIGNAL(clicked()), this, SLOT(onCreateButtonClicked()));
					hBox->addWidget (m_createButton);

	/* initialize the sensibility of widgets */
	m_shearLayerCheckBox->setChecked(false);
	m_highestFrequencyEdit->setEnabled(false);
	m_lowestFrequencyEdit->setEnabled(false);
	m_resolutionEdit->setEnabled(false);
	
	m_stackedWidget->setCurrentIndex(0);
	useWindField(NULL);
	
	addScrolledDock (Qt::LeftDockWidgetArea, parent);
}

void WindFieldCreatorDock::initView() {
}

void WindFieldCreatorDock::useWindField(WindField *newShownWindField) {
	m_shownWindField = newShownWindField;
	
	if (m_shownWindField) {
		m_nameLabel->setText(m_shownWindField->getName());
		m_windSpeedLabel->setText(QString().setNum(m_shownWindField->getMeanWindSpeed(),'f',2));
		m_rotorRadiusLabel->setText(QString().setNum(m_shownWindField->getRotorRadius(),'f',2));
		m_hubHeightLabel->setText(QString().setNum(m_shownWindField->getHubheight(),'f',2));
		m_roughnessLabel->setText(QString().setNum(m_shownWindField->getRoughnessLength(),'f',2));
		m_measurementHeightLabel->setText(QString().setNum(m_shownWindField->getWindSpeedMeasurementHeight(),'f',2));
		m_turbulenceLabel->setText(QString().setNum(m_shownWindField->getTurbulenceIntensity(),'f',2));
		m_simulationTimeLabel->setText(QString().setNum(m_shownWindField->getSimulationTime(),'f',2));
		m_timestepsLabel->setText(QString().setNum(m_shownWindField->getNumberOfTimesteps()));
		m_pointsLabel->setText(QString().setNum(m_shownWindField->getPointsPerSide()));
	} else {
		m_nameLabel->setText("< no windfield >");
		m_windSpeedLabel->setText("");
		m_rotorRadiusLabel->setText("");
		m_hubHeightLabel->setText("");
		m_roughnessLabel->setText("");
		m_measurementHeightLabel->setText("");
		m_turbulenceLabel->setText("");
		m_simulationTimeLabel->setText("");
		m_timestepsLabel->setText("");
		m_pointsLabel->setText("");
	}
	
	m_editCopyButton->setEnabled(m_shownWindField);
	m_deleteButton->setEnabled(m_shownWindField);
	m_renameButton->setEnabled(m_shownWindField);
}

void WindFieldCreatorDock::onWindFieldRenamed(QString oldName, QString newName) {
	if (m_nameLabel->text() == oldName) {
		m_nameLabel->setText(newName);
	}
}

void WindFieldCreatorDock::onShearLayerCheckBoxChanged(int state) {
	m_roughnessEdit->setEnabled(state);
	m_measurementHeightEdit->setEnabled(state);
}

void WindFieldCreatorDock::onTimeEditsChange() {
	m_highestFrequencyEdit->setValue(m_timestepsEdit->getValue() / 2 / m_simulationTimeEdit->getValue());
	m_lowestFrequencyEdit->setValue(1 / m_simulationTimeEdit->getValue());
}

void WindFieldCreatorDock::onPointsEditChange() {
	m_resolutionEdit->setValue(m_rotorRadiusEdit->getValue()*2 / (m_pointsEdit->getValue()-1));
}

void WindFieldCreatorDock::onNewButtonClicked() {
	/* set default parameter */

    QString newname("New Windfield");
    for (int i=0;i<g_windFieldStore.size();i++){
        if (newname == g_windFieldStore.at(i)->getName()){
            newname = makeNameWithHigherNumber(newname);
            i=0;
        }
    }

    m_nameEdit->setText(newname);

	m_shearLayerCheckBox->setChecked(true);
	m_rotorRadiusEdit->setValue(20);
	m_hubHeightEdit->setValue(100);
	m_windSpeedEdit->setValue(13);
	m_turbulenceEdit->setValue(10);
	m_measurementHeightEdit->setValue(10);
	m_roughnessEdit->setValue(0.01);
	m_simulationTimeEdit->setValue(60);
	m_timestepsEdit->setValue(100);
	m_pointsEdit->setValue(20);
	
	m_stackedWidget->setCurrentIndex(1);	
}

void WindFieldCreatorDock::onEditCopyButtonClicked() {
	if (m_shownWindField) {
		m_nameEdit->setText(m_shownWindField->getName());
		m_windSpeedEdit->setValue(m_shownWindField->getMeanWindSpeed());
		m_rotorRadiusEdit->setValue(m_shownWindField->getRotorRadius());
		m_hubHeightEdit->setValue(m_shownWindField->getHubheight());
		m_roughnessEdit->setValue(m_shownWindField->getRoughnessLength());
		m_measurementHeightEdit->setValue(m_shownWindField->getWindSpeedMeasurementHeight());
		m_turbulenceEdit->setValue(m_shownWindField->getTurbulenceIntensity());
		m_simulationTimeEdit->setValue(m_shownWindField->getSimulationTime());
		m_timestepsEdit->setValue(m_shownWindField->getNumberOfTimesteps());
		m_pointsEdit->setValue(m_shownWindField->getPointsPerSide());
	}
	
	m_stackedWidget->setCurrentIndex(1);
}

void WindFieldCreatorDock::onDeleteButtonClicked() {
	QMessageBox::StandardButton response = QMessageBox::question (this, tr("Question"),
																  QString (tr("Do you really want to delete this Windfield?")),
																  QMessageBox::Yes|QMessageBox::Cancel);
	if (response == QMessageBox::Yes) {
		g_windFieldStore.remove(m_shownWindField);
	}		
}

void WindFieldCreatorDock::onCreateButtonClicked() {
	QString errorMessage = "";
	if (m_rotorRadiusEdit->getValue() >= m_hubHeightEdit->getValue()) {
		errorMessage += "\n- Rotor Radius must be less then Hub Height.";
	}
	if (m_turbulenceEdit->getValue() == 0) {
		errorMessage += "\n- Turbulence must not be 0.";
	}
	if (m_roughnessEdit->getValue() == 0) {
		errorMessage += "\n- Roughness length must not be 0.";
	}
	if (m_measurementHeightEdit->getValue() == 0) {
		errorMessage += "\n- Measurement height must not be 0.";
	}
	if (errorMessage != "") {
		QMessageBox::critical(this, tr("Windfield"), QString("Could not create Windfield due to following errors:\n" + errorMessage),
							  QMessageBox::Ok);
		return;
	}
	
	m_cancelCalculation = false;
	m_progressStep = 0;
	m_progressStepShown = 0;
	int progressSteps = pow(m_pointsEdit->getValue(),2) * m_timestepsEdit->getValue() * 1.5;
	m_progressDialog = new QProgressDialog ("Creating windField... please wait", "Cancel", 0, progressSteps+1);
	m_progressDialog->setModal(true);
	m_progressDialog->setValue(0);
	m_progressDialog->show();
	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(onProgressDialogCanceled()));
	
	WindField *windField = new WindField (m_nameEdit->text(),
										  m_rotorRadiusEdit->getValue(),
										  m_hubHeightEdit->getValue(),
										  m_windSpeedEdit->getValue(),
										  m_turbulenceEdit->getValue(),
										  m_shearLayerCheckBox->isChecked(),
										  m_measurementHeightEdit->getValue(),
										  m_roughnessEdit->getValue(),
										  m_simulationTimeEdit->getValue(),
										  m_timestepsEdit->getValue(),
										  m_pointsEdit->getValue(),
										  &m_cancelCalculation );
	connect(windField, SIGNAL(updateProgress()), this, SLOT(onWindFieldProgress()));
	connect(windField, SIGNAL(badAlloc()), this, SLOT(onBadAlloc()));
	
	m_windFieldThread =  new WindFieldThread ();
	m_windFieldThread->windField = windField;
	connect(m_windFieldThread, SIGNAL(finished()), this, SLOT(onWindFieldFinished()), Qt::QueuedConnection);
	m_windFieldThread->start(QThread::LowPriority);
}

void WindFieldCreatorDock::onRenameButtonClicked() {
	g_windFieldStore.rename(m_shownWindField);
	m_nameLabel->setText(m_shownWindField->getName());  // might have changed
}

void WindFieldCreatorDock::onCancelButtonClicked() {
	m_stackedWidget->setCurrentIndex(0);
}

void WindFieldCreatorDock::onWindFieldProgress() {
	m_progressStep++;
	//qDebug() << "Update Signal " << m_progressStep << " + " << m_progressStepShown;
	if (m_progressStepShown >= m_progressStep -30) {  // stackoverflow can occure if the difference is too large
		m_progressDialog->setValue(m_progressStep);
	}
	m_progressStepShown++;
	//qDebug() << "Update Signal " << m_progressStep << "ende";
}

void WindFieldCreatorDock::onWindFieldFinished() {
	//qDebug() << "onWindFieldFinished";
	WindField *windField = m_windFieldThread->windField;
	
	if (windField->isValid()) {
		if (g_windFieldStore.add(windField)) {
			g_windFieldModule->setShownWindField(windField);
			g_windFieldModule->reportGLChange();
			m_stackedWidget->setCurrentIndex(0);
		}
	} else {
		delete windField;  // needed, because WindFieldThread does not care for the WindField
	}
	
	m_progressDialog->deleteLater();
	m_windFieldThread->deleteLater();
}

void WindFieldCreatorDock::onProgressDialogCanceled() {
	m_cancelCalculation = true;
}

void WindFieldCreatorDock::onBadAlloc() {
	// TODO never call exit while some thread is still running!!!
	QMessageBox msgBox;
	QString msgText = "bad_alloc exeption in windfield. Not enough memory (RAM) available.";
	msgBox.setText(msgText);
	QPushButton *okButton = msgBox.addButton("OK", QMessageBox::ActionRole);
	msgBox.exec();
	if (msgBox.clickedButton() == okButton) {
		exit(1);  // dirty exit
	}	
}
