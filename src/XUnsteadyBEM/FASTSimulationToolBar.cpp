#include "FASTSimulationToolBar.h"

#include <QDebug>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QComboBox>
#include <QVariant>

#include "FASTModule.h"
#include "../StoreAssociatedComboBox.h"
#include "../Store.h"
#include "FASTSimulation.h"
#include "../XBEM/Blade.h"
#include "../XBEM/360Polar.h"
#include "../Misc/NumberEdit.h"
#include "FASTResult.h"


FASTSimulationToolBar::FASTSimulationToolBar (QMainWindow *parent, FASTModule *module) {
	setObjectName("FASTSimulationToolbar");
	
	m_module = module;
	m_enableSectionChange = true;
	m_enableTimeChange = true;

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));
	
	QGroupBox *groupBox = new QGroupBox (tr("FAST Simulation"));
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_FASTSimulationComboBox = new FASTSimulationComboBox (&g_FASTSimulationStore);
			m_FASTSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_FASTSimulationComboBox->setMinimumWidth(170);
			connect (m_FASTSimulationComboBox, SIGNAL(valueChanged(FASTSimulation*)),
					 m_module, SLOT(setShownFASTSimulation(FASTSimulation*)));
			vBox->addWidget(m_FASTSimulationComboBox);
	groupBox = new QGroupBox (tr("Blade Section"));
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_sectionComboBox = new QComboBox ();
			m_sectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_sectionComboBox->setMinimumWidth(170);
			connect (m_sectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSectionBoxChanged()));
			vBox->addWidget(m_sectionComboBox);
	groupBox = new QGroupBox (tr("Time"));
	addWidget(groupBox);
		QHBoxLayout *hBox = new QHBoxLayout ();
		groupBox->setLayout(hBox);
			m_timeEdit = new NumberEdit (NumberEdit::Standard, 5);
//			m_timeEdit->setFixedWidth(60);
			connect(m_timeEdit, SIGNAL(editingFinished()), this, SLOT(onTimeEditChange()));
			hBox->addWidget(m_timeEdit);
			m_timeSlider = new QSlider ();
			m_timeSlider->setOrientation(Qt::Horizontal);
			m_timeSlider->setMinimum(0);
			connect(m_timeSlider, SIGNAL(valueChanged(int)), this, SLOT(onTimeSliderChange(int)));
			hBox->addWidget(m_timeSlider);
	
	useFASTSimulation(NULL);
	
	parent->addToolBar(this);
	hide();
}

int FASTSimulationToolBar::getCurrentBladeSection() {
	return m_sectionComboBox->itemData(m_sectionComboBox->currentIndex()).toInt();
}

void FASTSimulationToolBar::useFASTSimulation (FASTSimulation *newShownFASTSimulation) {
	m_FASTSimulationComboBox->setCurrentObject(newShownFASTSimulation);
	
	/* update the sectionBox */
	m_enableSectionChange = false;
	m_sectionComboBox->clear();
	if (newShownFASTSimulation != NULL) {
		for (int i = 0; i < newShownFASTSimulation->getAeroDynOutput().size(); ++i) {
			if (newShownFASTSimulation->getAeroDynOutput().at(i) == true) {
				m_sectionComboBox->addItem(QString("%1 - %2m - %3").arg(i+1, 2).arg(newShownFASTSimulation->getUsedRotor()->getFASTRadiusAt(i), 6, 'f', 2).arg(newShownFASTSimulation->getUsedRotor()->get360PolarAt(i)->getName()), i+1);
				if (newShownFASTSimulation->getShownBladeSection() == i+1) {
					m_sectionComboBox->setCurrentIndex(m_sectionComboBox->count()-1);
				}
			}
		}
	}
	m_sectionComboBox->setEnabled(m_sectionComboBox->count());
	m_enableSectionChange = true;
	onSectionBoxChanged();
	
	/* update the time slider */
	m_timeEdit->setEnabled(newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults());
	m_timeSlider->setEnabled(newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults());
	if (newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults()) {
		m_enableTimeChange = false;
		m_timeSlider->setMaximum(newShownFASTSimulation->getTimeData()->numberOfRows-1);
		m_timeEdit->setMinimum(newShownFASTSimulation->getTimeData()->data[0]);
		m_timeEdit->setMaximum(newShownFASTSimulation->getTimeData()->data[newShownFASTSimulation->getTimeData()->numberOfRows-1]);
		m_timeSlider->setValue(newShownFASTSimulation->getShownTimeIndex());
		m_timeEdit->setValue(newShownFASTSimulation->getTimeData()->data[newShownFASTSimulation->getShownTimeIndex()]);
		setShownTimeForAllSimulations();
		m_enableTimeChange = true;
	} else {
		m_enableTimeChange = false;
		m_timeSlider->setValue(0);
		m_timeEdit->setValue(0);
		m_enableTimeChange = true;
	}
	m_module->reloadBladeGraphs();
}

void FASTSimulationToolBar::onSectionBoxChanged() {
	if (m_sectionComboBox->currentIndex() != -1 && m_enableSectionChange) {
		for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
			if (g_FASTSimulationStore.at(i)->getUsedRotor() == m_FASTSimulationComboBox->currentObject()->getUsedRotor()) {
				g_FASTSimulationStore.at(i)->setShownBladeSection(this->getCurrentBladeSection());
			}
		}
		m_module->reloadFastGraphs();
	}
}

void FASTSimulationToolBar::onTimeSliderChange(int newValue) {
	if (m_enableTimeChange) {
		m_enableTimeChange = false;
		m_FASTSimulationComboBox->currentObject()->setShownTimeIndex(newValue);
		setShownTimeForAllSimulations();
		m_timeEdit->setValue(m_FASTSimulationComboBox->currentObject()->getShownTime());
		m_enableTimeChange = true;

		m_module->reloadBladeGraphs();
	}
}

void FASTSimulationToolBar::onTimeEditChange() {
    if (m_enableTimeChange && m_FASTSimulationComboBox->currentObject()) {
		double newValue = m_timeEdit->getValue();
		m_enableTimeChange = false;
		FASTSimulation *currentSim = m_FASTSimulationComboBox->currentObject();
		float *timeArray = currentSim->getTimeData()->data;
		
		/* calculate which timeIndex matches best to what the user typed */
		int timeIndex = (newValue - currentSim->getTStart()) / currentSim->getAeroTimeStep();
		while (timeArray[timeIndex] < newValue) {
			timeIndex++;
		}
		while (timeArray[timeIndex] > newValue) {  // the chosen timeIndex will be less or equal
			timeIndex--;
		}
		if ((newValue - timeArray[timeIndex] > timeArray[timeIndex+1] - newValue) &&  // round to nearest available value
													timeIndex < currentSim->getTimeData()->numberOfRows-2) {
			timeIndex++;
		}
		
		currentSim->setShownTimeIndex(timeIndex);
		setShownTimeForAllSimulations();
		m_timeEdit->setValue(timeArray[timeIndex]);
		m_timeSlider->setValue(timeIndex);
		m_enableTimeChange = true;
		
		m_module->reloadBladeGraphs();
	}
}

void FASTSimulationToolBar::setShownTimeForAllSimulations() {
	const double shownTime = m_FASTSimulationComboBox->currentObject()->getShownTime();
	for (int i = 0; i < g_FASTSimulationStore.size(); ++i) {
		if (g_FASTSimulationStore.at(i) != m_FASTSimulationComboBox->currentObject()) {  // the correct time is set already
			g_FASTSimulationStore.at(i)->setShownTime(shownTime);
		}
	}
}
