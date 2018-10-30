#include "NoiseDock.h"

#include <QSettings>
#include <QMessageBox>

#include "../Store.h"
#include "NoiseModule.h"
#include "NoiseCreatorDialog.h"


NoiseDock::NoiseDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, NoiseModule *module)
	: CreatorTwoDDock (title, parent, flags, false),
	  m_module(module)
{
	setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);

	m_colorByOpPoint = new QCheckBox ("Color by Curve");
	connect(m_colorByOpPoint, SIGNAL(toggled(bool)), this, SLOT(onColorByOpPoint()));
	m_visualizationVBox->insertWidget(1, m_colorByOpPoint);
	
	m_contentVBox->addStretch();	
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	m_colorByOpPoint->setChecked(settings.value("modules/NoiseDock/colorByOpPoint", false).toBool());

	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();
	
	addScrolledDock (Qt::LeftDockWidgetArea, parent);	
}


NoiseDock::~NoiseDock() {
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.setValue("modules/NoiseDock/colorByOpPoint", m_colorByOpPoint->isChecked());
}

bool NoiseDock::isColorByOpPoint() {
	return m_colorByOpPoint->isChecked();
}

TwoDWidgetInterface *NoiseDock::twoDInterface() {
	return m_module;
}

QList<NewGraph::GraphType> NoiseDock::affectedGraphTypes() {
	return {NewGraph::Noise};
}

void NoiseDock::onUnitsChanged() {
	setUnitContainingLabels();
	setShownObject(m_shownObject);  // reload object
}

void NoiseDock::onEditCopyButtonClicked() {
	NoiseCreatorDialog *creatorDialog = new NoiseCreatorDialog (m_shownObject, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void NoiseDock::onRenameButtonClicked() {
	g_noiseSimulationStore.rename(m_shownObject);
}

void NoiseDock::onDeleteButtonClicked() {
	g_noiseSimulationStore.remove(m_shownObject);
}

void NoiseDock::onNewButtonClicked() {
	/* check first, if all requirements are fullfilled */
    if (g_polarStore.size() == 0) {
        QMessageBox::critical(this, "Create new Noise Simulation",
							  "The following error(s) occured:\n\n - No Polars in database", QMessageBox::Ok);
        return;
    }

	NoiseCreatorDialog *creatorDialog = new NoiseCreatorDialog (NULL, m_module);
	creatorDialog->exec();
	delete creatorDialog;
}

void NoiseDock::onColorByOpPoint() {
	twoDInterface()->update();
}
