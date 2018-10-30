#include "TurDmsTurbineDock.h"

#include <QGroupBox>
#include <QMessageBox>

#include "../Store.h"
#include "TurDmsTurbineCreatorDialog.h"
#include "../XBEM/TData.h"
#include "../MainFrame.h"
#include "../Misc/FixedSizeLabel.h"
#include "../ParameterGrid.h"

typedef Parameter::TData P;


TurDmsTurbineDock::TurDmsTurbineDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, TurDmsModule *module)
	: CreatorDock (title, parent, flags),
	  m_module(module)
{
	setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);

	QGroupBox *groupBox = new QGroupBox ("Turbine Specification");
	m_contentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P> (this);
		groupBox->setLayout(grid);
			grid->addEdit(P::Blade, BoxLabel, new FixedSizeLabel("", 150), "Turbine Blade:", "");
			grid->addLabel(P::VCutIn, true, "V Cut In []:", SPEED);
			grid->addLabel(P::VCutOut, true, "V Cut Out []:", SPEED);
			grid->addLabel(P::TurbineOffset, true, "Turbine Offset []:", LENGTH);
			grid->addLabel(P::TurbineHeight, true, "Turbine Height []:", LENGTH);
			grid->addLabel(P::RotorHeight, true, "Rotor Height []:", LENGTH);
			grid->addLabel(P::RotorMaxRadius, true, "Rotor Max Radius []:", LENGTH);
			grid->addLabel(P::RotorSweptArea, true, "Rotor Swept Area []:", AREA);
			grid->addLabel(P::LossFactor, true, "Loss Factor:");
			grid->addLabel(P::FixedLosses, true, "Fixed Losses []:", POWER);
	
	m_singleGroupBox = new QGroupBox ("Rotational Speed");
	m_contentVBox->addWidget(m_singleGroupBox);
		grid = new ParameterGrid<P> (this);
		m_singleGroupBox->setLayout(grid);
			grid->addRow("Transmission:", "Single");
			grid->addLabel(P::RotationalSpeed, true, "Rotational Speed [rpm]:");
		
	m_variableGroupBox = new QGroupBox ("Rotational Speed");
	m_contentVBox->addWidget(m_variableGroupBox);
		grid = new ParameterGrid<P> (this);
		m_variableGroupBox->setLayout(grid);
			grid->addRow("Transmission:", "Variable");
			grid->addLabel(P::RotationalSpeedMin, true, "Rotational Speed Min [rpm]:");
			grid->addLabel(P::RotationalSpeedMax, true, "Rotational Speed Max [rpm]:");
			grid->addLabel(P::TSR, true, "TSR at Design Point::");

	m_prescribedGroupBox = new QGroupBox ("Rotational Speed");
	m_contentVBox->addWidget(m_prescribedGroupBox);
		grid = new ParameterGrid<P> (this);
		m_prescribedGroupBox->setLayout(grid);
			grid->addRow("Transmission:", "Prescribed");
							
	m_contentVBox->addStretch();
		
	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();
	
	addScrolledDock (Qt::LeftDockWidgetArea, parent, 290);	
}

void TurDmsTurbineDock::setShownObject(TData *newTurbine) {
	CreatorDock::setShownObject(newTurbine);
	
	loadObject(m_shownObject);
	
	if (m_shownObject) {
		m_singleGroupBox->setVisible(m_shownObject->isFixed);
		m_variableGroupBox->setVisible(m_shownObject->isVariable);
		m_prescribedGroupBox->setVisible(m_shownObject->isPrescribedRot);
	}
}

void TurDmsTurbineDock::onUnitsChanged() {
	setUnitContainingLabels();
	setShownObject(m_shownObject);  // reload object
}

void TurDmsTurbineDock::onEditCopyButtonClicked() {
	TurDmsTurbineCreatorDialog *creatorDialog = new TurDmsTurbineCreatorDialog (m_shownObject, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void TurDmsTurbineDock::onRenameButtonClicked() {
	g_verttdataStore.rename(m_shownObject);	
}

void TurDmsTurbineDock::onDeleteButtonClicked() {
	g_verttdataStore.remove(m_shownObject);
}

void TurDmsTurbineDock::onNewButtonClicked() {
	/* check first, if all requirements are fullfilled */
    if (g_verticalRotorStore.size() == 0) {
        QMessageBox::critical(this, "Create new DMS Turbine",
							  "The following error(s) occured:\n\n - No VAWT Blade in database", QMessageBox::Ok);
        return;
    }

	TurDmsTurbineCreatorDialog *creatorDialog = new TurDmsTurbineCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}
