/****************************************************************************

    QLLTToolBar Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

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

#include "QLLTToolBar.h"
#include <QLabel>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QMainWindow>
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"
#include "QLLTSimulation.h"
#include "QLLTDock.h"


QLLTToolBar::QLLTToolBar(QMainWindow *parent, QLLTModule *module)
{
	setObjectName("QLLTToolbar");
	
    m_module = module;
    m_QLLTThread = NULL;

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));


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

    QGroupBox *groupbox = new QGroupBox (tr("LLT HAWT Simulation"));
    m_LLTHAWTSimulationComboBox = new LLTSimulationComboBox(&g_QLLTHAWTSimulationStore);
    m_LLTHAWTSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_LLTHAWTSimulationComboBox->setMinimumWidth(170);
    connect (m_LLTHAWTSimulationComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeLLTSimulation()));
    QGridLayout *grid = new QGridLayout ();
    grid->addWidget(m_LLTHAWTSimulationComboBox, 0, 0);
    groupbox->setLayout(grid);
    HAWTBox = addWidget(groupbox);

    groupbox = new QGroupBox (tr("LLT VAWT Simulation"));
    m_LLTVAWTSimulationComboBox = new LLTSimulationComboBox(&g_QLLTVAWTSimulationStore);
    m_LLTVAWTSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_LLTVAWTSimulationComboBox->setMinimumWidth(170);
    connect (m_LLTVAWTSimulationComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(OnSelChangeVAWTLLTSimulation()));
    grid = new QGridLayout ();
    grid->addWidget(m_LLTVAWTSimulationComboBox, 0, 0);
    groupbox->setLayout(grid);
    VAWTBox = addWidget(groupbox);

    groupbox = new QGroupBox (tr("Time"));
    m_Timesteps = new QSlider;
    m_Timesteps->setOrientation(Qt::Horizontal);
    m_Timesteps->setEnabled(false);

    m_TimeLabel = new QLabel("");

    grid = new QGridLayout ();

    m_startReplay = new QPushButton("Replay");
    m_DelayBox = new QDoubleSpinBox;
    m_DelayBox->setMinimum(0.0);
    m_DelayBox->setSingleStep(0.01);
    m_DelayBox->setVisible(false);
    grid->addWidget(m_startReplay, 0, 0);
    grid->addWidget(m_DelayBox, 0, 1);
    grid->addWidget(m_TimeLabel, 0, 2);
    grid->addWidget(m_Timesteps, 0, 3);
    groupbox->setLayout(grid);
    addWidget(groupbox);

    parent->addToolBar(this);
    hide();
    connect(m_startReplay, SIGNAL(clicked()), this, SLOT(onStartReplay()));



    connect(&g_QLLTHAWTSimulationStore, SIGNAL(objectListChanged(bool)), m_module, SLOT(OnSelChangeLLTSimulation()));
    connect(&g_QLLTVAWTSimulationStore, SIGNAL(objectListChanged(bool)), m_module, SLOT(OnSelChangeVAWTLLTSimulation()));


}

void QLLTToolBar::onStartReplay(){
    m_DelayBox->setVisible(true);
    m_startReplay->setText("Stop");
    m_module->GetQLLTDock()->onReplayStarted();
    disconnect(m_startReplay, SIGNAL(clicked()), this, SLOT(onStartReplay()));
    connect(m_startReplay, SIGNAL(clicked()), m_module, SLOT(OnStopReplay()));
    connect(m_startReplay, SIGNAL(clicked()), this, SLOT(onStopReplay()));

    m_QLLTThread =  new QLLTThread ();
    m_QLLTThread->simulation = m_module->GetQLLTSim();
    connect(m_QLLTThread, SIGNAL(finished()), m_QLLTThread, SLOT(deleteLater()), Qt::QueuedConnection);
    m_QLLTThread->start();

}

void QLLTToolBar::onStopReplay(){
    m_DelayBox->setVisible(false);
    m_startReplay->setText("Replay");
    m_module->GetQLLTDock()->onReplayStopped();

    connect(m_startReplay, SIGNAL(clicked()), this, SLOT(onStartReplay()));
    disconnect(m_startReplay, SIGNAL(clicked()), m_module, SLOT(OnStopReplay()));
    disconnect(m_startReplay, SIGNAL(clicked()), this, SLOT(onStopReplay()));

    m_QLLTThread = NULL;
}

void QLLTToolBar::OnTimeSliderChanged(){
    if (!m_module->GetQLLTSim()) return;
    if (!m_module->GetQLLTSim()->hasStoredData()) return;

    if (m_module->GetQLLTSim()->getIsVawt())
		m_LLTVAWTSimulationComboBox->currentObject()->setShownTimeIndex(m_Timesteps->value());
    if (!m_module->GetQLLTSim()->getIsVawt())
		m_LLTHAWTSimulationComboBox->currentObject()->setShownTimeIndex(m_Timesteps->value());

    if (m_module->GetQLLTSim()->getStoreWake())
		m_module->forceReRender();

    setShownTimeForAllSimulations();

    QString strong;
    m_TimeLabel->setText(strong.number(m_module->GetQLLTSim()->getResultsArray().at(0)->at(m_Timesteps->value()),'f',3) +
						 " [s]");

	m_module->reloadBladeGraphs();
	
	if (m_module->GetQLLTSim()->getStoreWake()) {
		m_module->OnRenderCutPlanes();
	}
}

void QLLTToolBar::DisableBoxes(){
    m_LLTVAWTSimulationComboBox->setEnabled(false);
    m_LLTHAWTSimulationComboBox->setEnabled(false);
    m_Timesteps->setEnabled(false);
    m_startReplay->setEnabled(false);
}

void QLLTToolBar::EnableBoxes(){
    m_LLTVAWTSimulationComboBox->setEnabled(g_QLLTVAWTSimulationStore.size());
    m_LLTHAWTSimulationComboBox->setEnabled(g_QLLTHAWTSimulationStore.size());
    m_Timesteps->setEnabled(true);
    if (m_module->GetQLLTSim()){
        if (m_module->GetQLLTSim()->getStoreWake() && m_module->GetQLLTSim()->hasStoredData()) m_startReplay->setEnabled(true);
    }
}


void QLLTToolBar::OnHAWTView(){
    HAWTBox->setVisible(true);
    m_LLTHAWTSimulationComboBox->show();
    VAWTBox->setVisible(false);
    m_LLTVAWTSimulationComboBox->hide();
}

void QLLTToolBar::OnVAWTView(){
    HAWTBox->setVisible(false);
    m_LLTHAWTSimulationComboBox->hide();
    VAWTBox->setVisible(true);
    m_LLTVAWTSimulationComboBox->show();
}

void QLLTToolBar::useLLTSimulation (QLLTSimulation *newShownLLTSimulation) {
    if (newShownLLTSimulation){
        if (newShownLLTSimulation->hasStoredData() && newShownLLTSimulation->getShownTimeIndex() == -1) newShownLLTSimulation->setShownTimeIndex(0);
    }

    m_startReplay->setEnabled(false);

    if (!newShownLLTSimulation){
        m_TimeLabel->setText("");
        m_Timesteps->setValue(0);
        m_Timesteps->setEnabled(false);
        return;
    }

    if (!newShownLLTSimulation->getIsVawt()) m_LLTHAWTSimulationComboBox->setCurrentObject(newShownLLTSimulation);
    else m_LLTVAWTSimulationComboBox->setCurrentObject(newShownLLTSimulation);

    if (newShownLLTSimulation->hasStoredData()){
        QString strong;
        disconnect(m_Timesteps, SIGNAL(valueChanged(int)), 0, 0);
        m_Timesteps->setEnabled(true);
        m_Timesteps->setMinimum(0);
        m_Timesteps->setMaximum(newShownLLTSimulation->getNumTimestepsComputed()-1);
        m_Timesteps->setValue(newShownLLTSimulation->getShownTimeIndex());
        m_TimeLabel->setText(strong.number(m_module->GetQLLTSim()->getResultsArray().at(0)->at(m_Timesteps->value()),'f',3)+" [s]");
        connect(m_Timesteps,SIGNAL(valueChanged(int)),this,SLOT(OnTimeSliderChanged()));
        setShownTimeForAllSimulations(); 

		m_module->reloadBladeGraphs();
		
        if (newShownLLTSimulation->getStoreWake())
			m_startReplay->setEnabled(true);
    }
    else{
        m_TimeLabel->setText("");
        m_Timesteps->setValue(0);
        m_Timesteps->setEnabled(false);
    }
}

void QLLTToolBar::setShownTimeForAllSimulations() {
    double shownTime = -1;
    if (m_module->GetQLLTSim()->hasStoredData()) shownTime = m_module->GetQLLTSim()->getShownTime();
    if (m_module->GetQLLTSim()->getIsVawt()){
        for (int i = 0; i < g_QLLTVAWTSimulationStore.size(); ++i) {
            if (g_QLLTVAWTSimulationStore.at(i) != m_LLTVAWTSimulationComboBox->currentObject()) {  // the correct time is set already
                g_QLLTVAWTSimulationStore.at(i)->setShownTime(shownTime);
            }
        }
    }
    else if(!m_module->GetQLLTSim()->getIsVawt()){
        for (int i = 0; i < g_QLLTHAWTSimulationStore.size(); ++i) {
            if (g_QLLTHAWTSimulationStore.at(i) != m_LLTHAWTSimulationComboBox->currentObject()) {  // the correct time is set already
                g_QLLTHAWTSimulationStore.at(i)->setShownTime(shownTime);
            }
        }
    }
}
