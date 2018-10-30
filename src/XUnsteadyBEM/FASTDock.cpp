#include "FASTDock.h"

#include <QGroupBox>
#include <QColorDialog>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QLayout>

#include "FASTSimulationCreatorDialog.h"
#include "FASTSimulationProgressDialog.h"
#include "FASTSimulation.h"
#include "../GlobalFunctions.h"
#include "../Store.h"
#include "WindField.h"
#include "../XBEM/Blade.h"
#include "../QFem/BladeStructure.h"
#include "../Misc/LineButton.h"
#include "../Misc/LinePickerDlg.h"
#include "../Misc/FixedSizeLabel.h"


FASTDock::FASTDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, FASTModule *module)
	: ScrolledDock (title, parent, flags)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);

	m_module = module;
	m_shownFASTSimulation = NULL;
	
    QGroupBox *groupBox = new QGroupBox ("FAST Controls");
	m_contentVBox->addWidget(groupBox);
		QGridLayout *grid = new QGridLayout ();
		groupBox->setLayout(grid);
			m_startSimulationButton = new QPushButton (tr("Start Simulation"));
			connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT(onStartSimulationButtonClicked()));
			grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);
			m_renameButton = new QPushButton (tr("Rename"));
			connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
			grid->addWidget (m_renameButton, 1, 0);
			m_editCopyButton = new QPushButton (tr("Edit/Copy"));
			connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));					
			grid->addWidget (m_editCopyButton, 1, 1);
			m_deleteButton = new QPushButton (tr("Delete"));
			connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
			grid->addWidget (m_deleteButton, 2, 0);
			m_newButton = new QPushButton (tr("New"));
			connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
			grid->addWidget (m_newButton, 2, 1);
	groupBox = new QGroupBox (tr("Styles"));
	m_contentVBox->addWidget(groupBox);
		grid = new QGridLayout ();
		groupBox->setLayout(grid);
			int gridRowCount = 0;
			QLabel *label = new QLabel (tr("Style: "));
			grid->addWidget(label, gridRowCount, 0);
			m_simulationLineButton = new LineButton ();
			connect(m_simulationLineButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
			grid->addWidget(m_simulationLineButton, gridRowCount++, 1);
			QHBoxLayout *hBox = new QHBoxLayout ();
			grid->addLayout(hBox, gridRowCount++, 0, 1, 2);
				m_showCheckBox = new QCheckBox (tr("show"));
				connect(m_showCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCheckBoxCanged()));
				hBox->addWidget(m_showCheckBox);
				m_showCurveCheckBox = new QCheckBox (tr("curve"));
				connect(m_showCurveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCurveCheckBoxCanged()));
				hBox->addWidget(m_showCurveCheckBox);
				m_showPointsCheckBox = new QCheckBox (tr("points"));
				connect(m_showPointsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowPointsCheckBoxCanged()));
				hBox->addWidget(m_showPointsCheckBox);
	groupBox = new QGroupBox (tr("Simulation Parameters"));
	m_contentVBox->addWidget(groupBox);
		grid = new QGridLayout ();
		groupBox->setLayout(grid);
			gridRowCount = 0;
			label = new QLabel (tr("Windfield: "));
			grid->addWidget(label, gridRowCount, 0);
			m_windfieldLabel = new FixedSizeLabel ("", 150);
			grid->addWidget(m_windfieldLabel, gridRowCount++, 1);
			label = new QLabel (tr("Total run time [s]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_totalRuntimeLabel = new QLabel ();
			grid->addWidget(m_totalRuntimeLabel, gridRowCount++, 1);
			label = new QLabel (tr("FAST Time step [s]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_timeStepLabel = new QLabel ();
			grid->addWidget(m_timeStepLabel, gridRowCount++, 1);
			label = new QLabel (tr("Rotor: "));
			grid->addWidget(label, gridRowCount, 0);
			m_rotorLabel = new FixedSizeLabel ("", 150);
			grid->addWidget(m_rotorLabel, gridRowCount++, 1);
			label = new QLabel (tr("Number of Blades: "));
			grid->addWidget(label, gridRowCount, 0);
			m_numberOfBladesLabel = new QLabel ();
			grid->addWidget(m_numberOfBladesLabel, gridRowCount++, 1);
			label = new QLabel (tr("Rotor Speed [rpm]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_rotorSpeedLabel = new QLabel ();
			grid->addWidget(m_rotorSpeedLabel, gridRowCount++, 1);
			label = new QLabel (tr("Nacelle Yaw [deg]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_nacelleYawLabel = new QLabel ();
			grid->addWidget(m_nacelleYawLabel, gridRowCount++, 1);
	groupBox = new QGroupBox (tr("Environment"));
	m_contentVBox->addWidget(groupBox);
		grid = new QGridLayout ();
		groupBox->setLayout(grid);
			gridRowCount = 0;
			label = new QLabel (tr("Gravity [m/s^2]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_gravityLabel = new QLabel ();
			grid->addWidget(m_gravityLabel, gridRowCount++, 1);
			label = new QLabel (tr("AirDens [k/m^3]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_airDensLabel = new QLabel ();
			grid->addWidget(m_airDensLabel, gridRowCount++, 1);
			label = new QLabel (tr("KinVisc [m^2/s]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_kinViscLabel = new QLabel ();
			grid->addWidget(m_kinViscLabel, gridRowCount++, 1);
	groupBox = new QGroupBox (tr("Aerodynamic Parameters"));
	m_contentVBox->addWidget(groupBox);
		grid = new QGridLayout ();
		groupBox->setLayout(grid);
			gridRowCount = 0;
			label = new QLabel (tr("Aero Time Step [s]: "));
			grid->addWidget(label, gridRowCount, 0);
			m_aeroTimeStepLabel = new QLabel ();
			grid->addWidget(m_aeroTimeStepLabel, gridRowCount++, 1);
			label = new QLabel (tr("StallMod: "));
			grid->addWidget(label, gridRowCount, 0);
			m_stallModLabel = new QLabel ();
			grid->addWidget(m_stallModLabel, gridRowCount++, 1);
			label = new QLabel (tr("UseCm: "));
			grid->addWidget(label, gridRowCount, 0);
			m_useCmLabel = new QLabel ();
			grid->addWidget(m_useCmLabel, gridRowCount++, 1);
			label = new QLabel (tr("InfModel: "));
			grid->addWidget(label, gridRowCount, 0);
			m_infModLabel = new QLabel ();
			grid->addWidget(m_infModLabel, gridRowCount++, 1);
			label = new QLabel (tr("IndModel: "));
			grid->addWidget(label, gridRowCount, 0);
			m_indModLabel = new QLabel ();
			grid->addWidget(m_indModLabel, gridRowCount++, 1);
			label = new QLabel (tr("TLModel: "));
			grid->addWidget(label, gridRowCount, 0);
			m_tlModelLabel = new QLabel ();
			grid->addWidget(m_tlModelLabel, gridRowCount++, 1);
			label = new QLabel (tr("HLModel: "));
			grid->addWidget(label, gridRowCount, 0);
			m_hlModelLabel = new QLabel ();
			grid->addWidget(m_hlModelLabel, gridRowCount++, 1);
	groupBox = new QGroupBox (tr("Blade Structure"));
	m_contentVBox->addWidget(groupBox);
		grid = new QGridLayout ();
		groupBox->setLayout(grid);
			gridRowCount = 0;
			label = new QLabel (tr("Blade Structure: "));
			grid->addWidget(label, gridRowCount, 0);
			m_bladeStructureLabel = new FixedSizeLabel ("", 150);
			grid->addWidget(m_bladeStructureLabel, gridRowCount++, 1);
			label = new QLabel (tr("Degrees of Freedom: "));
			grid->addWidget(label, gridRowCount++, 0);
			m_dofLabel = new QLabel ();
			m_dofLabel->setAlignment(Qt::AlignRight);
			grid->addWidget(m_dofLabel, gridRowCount++, 0, 1, 2);
			

	setShownFASTSimulation(NULL);
			
	addScrolledDock (Qt::LeftDockWidgetArea, parent);
}

void FASTDock::onShowCheckBoxCanged () {
	m_shownFASTSimulation->setShownInGraph(m_showCheckBox->isChecked());
	m_module->reloadAllGraphCurves();
}

void FASTDock::onShowPointsCheckBoxCanged () {
	m_shownFASTSimulation->setDrawPoints(m_showPointsCheckBox->isChecked());
	m_module->update();
}

void FASTDock::onShowCurveCheckBoxCanged () {
	m_shownFASTSimulation->setDrawCurve(m_showCurveCheckBox->isChecked());
	m_module->update();
}

void FASTDock::onStartSimulationButtonClicked() {
	FASTSimulationProgressDialog *progressDialog = NULL;
	try {
		QDir savePath (QString(QDir::currentPath() + QDir::separator() + "FAST_simulation"));
		if (!savePath.removeRecursively()) {
			throw QString(tr("Could not completely remove existing directory: ") + savePath.absolutePath());
		}
		if (!savePath.mkpath(savePath.absolutePath())) {
			throw QString(tr("Could not make directory: ") + savePath.absolutePath());
		}
		m_shownFASTSimulation->writeAllFiles(savePath);
	
		progressDialog = new FASTSimulationProgressDialog ();
		progressDialog->startProcess(savePath.absolutePath());
		int response = progressDialog->exec();
		if (response == QDialog::Accepted) {
			m_shownFASTSimulation->readOutputFile(QString(savePath.absolutePath() + QDir::separator() + "fast.out"));
			m_module->reloadAllGraphCurves();
		}
		
		m_module->setShownFASTSimulation(m_shownFASTSimulation, true);  // reload the simulation
	} catch (QString errorMessage) {
		QMessageBox::critical(this, tr("FAST Simulation"), errorMessage, QMessageBox::Ok);
	}
	
	if (progressDialog) {
		delete progressDialog;
	}
}

void FASTDock::onRenameButtonClicked() {
	g_FASTSimulationStore.rename(m_shownFASTSimulation);
}

void FASTDock::onEditCopyButtonClicked() {
	FASTSimulationCreatorDialog *creatorDialog = new FASTSimulationCreatorDialog (m_shownFASTSimulation, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void FASTDock::onDeleteButtonClicked() {
	g_FASTSimulationStore.remove(m_shownFASTSimulation);
}

void FASTDock::onNewButtonClicked() {
    /* check first, if all requirements are fullfilled */
    QString errorMessage ("");
    if (g_rotorStore.size() == 0) {
        errorMessage.append(tr("\n - No rotor in database"));
    }
    if (g_windFieldStore.size() == 0) {
        errorMessage.append(tr("\n - No windfield in database"));
    }
    if (errorMessage != "") {
        QMessageBox::critical(this, tr("Create New Simulation"), QString(tr("The following error(s) occured:\n") + errorMessage), QMessageBox::Ok);
        return;
    }

	FASTSimulationCreatorDialog *creatorDialog = new FASTSimulationCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;	
}

void FASTDock::setShownFASTSimulation(FASTSimulation *newSimulation) {
	m_shownFASTSimulation = newSimulation;
	
	/* set all labels to correct values */
	if (m_shownFASTSimulation) {
		m_simulationLineButton->setPen(m_shownFASTSimulation->getPen());
		m_showCheckBox->setChecked(m_shownFASTSimulation->isShownInGraph());
		m_showPointsCheckBox->setChecked(m_shownFASTSimulation->isDrawPoints());
		m_showCurveCheckBox->setChecked(m_shownFASTSimulation->isDrawCurve());
		m_startSimulationButton->setEnabled(!m_shownFASTSimulation->hasFastResults());
		m_windfieldLabel->setText(m_shownFASTSimulation->getUsedWindField()->getName());
		m_totalRuntimeLabel->setText(QString().setNum(m_shownFASTSimulation->getTotalRunTime()));
		m_timeStepLabel->setText(QString().setNum(m_shownFASTSimulation->getTimeStep()));
		m_rotorLabel->setText(m_shownFASTSimulation->getUsedRotor()->getName());
		m_numberOfBladesLabel->setText(QString().setNum(m_shownFASTSimulation->getNumberOfBlades()));
		m_rotorSpeedLabel->setText(QString().setNum(m_shownFASTSimulation->getRotorSpeed()));
		m_nacelleYawLabel->setText(QString().setNum(m_shownFASTSimulation->getNacelleYaw()));
		m_gravityLabel->setText(QString().setNum(m_shownFASTSimulation->getGravity()));
		m_airDensLabel->setText(QString().setNum(m_shownFASTSimulation->getAirDens()));
		m_kinViscLabel->setText(QString().setNum(m_shownFASTSimulation->getKinVisc()));
		m_aeroTimeStepLabel->setText(QString().setNum(m_shownFASTSimulation->getAeroTimeStep()));
		m_stallModLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getStallMod()]);
		m_useCmLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getUseCm()]);
		m_infModLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getInfModel()]);
		m_indModLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getIndModel()]);
		m_tlModelLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getTlModel()]);
		m_hlModelLabel->setText(FASTSimulation::valueString[(int)m_shownFASTSimulation->getHlModel()]);
		m_bladeStructureLabel->setText(m_shownFASTSimulation->getUsedBladeStructure()->getName());
		QString temp;
		if (m_shownFASTSimulation->getUseFlapDof1()) { temp.append("FlapDOF1, "); }
		if (m_shownFASTSimulation->getUseFlapDof2()) { temp.append("FlapDOF2, "); }
		if (m_shownFASTSimulation->getUseEdgeDof()) { temp.append("EdgeDOF, "); }
		temp.chop(2);
		m_dofLabel->setText(temp);
	} else {
		QPen pen;
		pen.setColor(QColor("lightgrey"));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		m_simulationLineButton->setPen(pen);
		m_startSimulationButton->setEnabled(false);
		m_windfieldLabel->setText("");
		m_totalRuntimeLabel->setText("");
		m_timeStepLabel->setText("");
		m_rotorLabel->setText("");
		m_numberOfBladesLabel->setText("");
		m_rotorSpeedLabel->setText("");
		m_nacelleYawLabel->setText("");
		m_gravityLabel->setText("");
		m_airDensLabel->setText("");
		m_kinViscLabel->setText("");
		m_aeroTimeStepLabel->setText("");
		m_stallModLabel->setText("");
		m_useCmLabel->setText("");
		m_infModLabel->setText("");
		m_indModLabel->setText("");
		m_tlModelLabel->setText("");
		m_hlModelLabel->setText("");
		m_bladeStructureLabel->setText("");
		m_dofLabel->setText("");
	}
	
	m_showCheckBox->setEnabled(m_shownFASTSimulation);
	m_showCurveCheckBox->setEnabled(m_shownFASTSimulation);
	m_showPointsCheckBox->setEnabled(m_shownFASTSimulation);
	m_simulationLineButton->setEnabled(m_shownFASTSimulation);
	m_deleteButton->setEnabled(m_shownFASTSimulation);
	m_editCopyButton->setEnabled(m_shownFASTSimulation);
	m_renameButton->setEnabled(m_shownFASTSimulation);	
}

void FASTDock::adjustShowCheckBox() {
	m_showCheckBox->setChecked(m_shownFASTSimulation->isShownInGraph());
}

void FASTDock::onLineButtonClicked() {
    LinePickerDlg linePicker;
    linePicker.InitDialog(m_simulationLineButton->GetStyle(), m_simulationLineButton->GetWidth(), m_simulationLineButton->GetColor());
    if(linePicker.exec() == QDialog::Accepted) 	{
        m_simulationLineButton->SetStyle(linePicker.GetStyle());
        m_simulationLineButton->SetWidth(linePicker.GetWidth());
        m_simulationLineButton->SetColor(linePicker.GetColor());

        m_shownFASTSimulation->setPen(m_simulationLineButton->getPen());
    }
}
