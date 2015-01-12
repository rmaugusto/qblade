#include "QLLTToolBar.h"

#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include <QMainWindow>

QLLTToolBar::QLLTToolBar(QMainWindow *parent, QLLTModule *module)
{
    m_module = module;

    GLView = new QAction(QIcon(":/images/3dview.png"), tr("3D OpenGL View"), this);
    GLView->setCheckable(true);
    GLView->setStatusTip(tr("3D OpenGL View"));

    TwoDView = new QAction(QIcon(":/images/graph.png"), tr("Plot Results in a Graph"), this);
    TwoDView->setCheckable(true);
    TwoDView->setStatusTip(tr("Plot Results in a Graph"));

    connect (GLView, SIGNAL(triggered(bool)), m_module, SLOT(OnGLView()));
    connect (TwoDView, SIGNAL(triggered(bool)), m_module, SLOT(OnTwoDView()));


    addAction(GLView);
    addAction(TwoDView);
    addSeparator();

    QGroupBox *groupBox = new QGroupBox (tr("LLT Simulation"));
    m_LLTSimulationComboBox = new LLTSimulationComboBox(&g_QLLTHAWTSimulationStore);
    m_LLTSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_LLTSimulationComboBox->setMinimumWidth(170);
    connect (m_LLTSimulationComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeLLTSimulation()));
    QGridLayout *grid = new QGridLayout ();
    grid->addWidget(m_LLTSimulationComboBox, 0, 0);
    groupBox->setLayout(grid);
    addWidget(groupBox);

    parent->addToolBar(this);
    hide();
}

void QLLTToolBar::useLLTSimulation (QLLTSimulation *newShownLLTSimulation) {
//	m_FASTSimulationComboBox->setCurrentObject(newShownFASTSimulation);

//	/* update the sectionBox */
//	m_enableSectionChange = false;
//	m_sectionComboBox->clear();
//	if (newShownFASTSimulation != NULL) {
//		for (int i = 0; i < newShownFASTSimulation->getAeroDynOutput().size(); ++i) {
//			if (newShownFASTSimulation->getAeroDynOutput().at(i) == true) {
//				m_sectionComboBox->addItem(QString("%1 - %2m - %3").arg(i+1, 2).arg(newShownFASTSimulation->getUsedRotor()->getFASTRadiusAt(i), 6, 'f', 2).arg(newShownFASTSimulation->getUsedRotor()->get360PolarAt(i)->getName()), i+1);
//				if (newShownFASTSimulation->getShownBladeSection() == i+1) {
//					m_sectionComboBox->setCurrentIndex(m_sectionComboBox->count()-1);
//				}
//			}
//		}
//	}
//	m_sectionComboBox->setEnabled(m_sectionComboBox->count());
//	m_enableSectionChange = true;
//	onSectionBoxChanged();

//	/* update the time slider */
//	m_timeEdit->setEnabled(newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults());
//	m_timeSlider->setEnabled(newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults());
//	if (newShownFASTSimulation && newShownFASTSimulation->hasAeroDynResults()) {
//		m_enableTimeChange = false;
//		m_timeSlider->setMaximum(newShownFASTSimulation->getTimeData()->numberOfRows-1);
//		m_timeEdit->setMinimum(newShownFASTSimulation->getTimeData()->data[0]);
//		m_timeEdit->setMaximum(newShownFASTSimulation->getTimeData()->data[newShownFASTSimulation->getTimeData()->numberOfRows-1]);
//		m_timeSlider->setValue(newShownFASTSimulation->getShownTimeIndex());
//		m_timeEdit->setValue(newShownFASTSimulation->getTimeData()->data[newShownFASTSimulation->getShownTimeIndex()]);
//		setShownTimeForAllSimulations();
//		m_enableTimeChange = true;
//	} else {
//		m_enableTimeChange = false;
//		m_timeSlider->setValue(0);
//		m_timeEdit->setValue(0);
//		m_enableTimeChange = true;
//	}
//	m_module->emitUpdateBladeGraphs();
}
