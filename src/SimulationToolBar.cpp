#include "SimulationToolBar.h"

#include <QAction>
#include <QGroupBox>
#include <QBoxLayout>

#include "SimulationModule.h"
#include "StoreAssociatedComboBox.h"
class TData;


template <class Parent, class Simulation>
void SimulationToolBar<Parent,Simulation>::constructSimulationToolBar(QString parentName, Store<Parent> *parentStore,
														  QString simulationName, Store<Simulation> *simulationStore) {

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));

	m_hideDocksAction = new QAction(QIcon(":/images/hidden.png"), tr("Hide Docks"), this);
	m_hideDocksAction->setCheckable(true);
	m_hideDocksAction->setStatusTip(tr("Hide Docks"));
	connect(m_hideDocksAction, SIGNAL(toggled(bool)), module(), SLOT(onHideDocks(bool)));
	addAction(m_hideDocksAction);
	
	QGroupBox *groupBox = new QGroupBox (parentName);
	addWidget(groupBox);
		QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
            m_parentComboBox = new StoreAssociatedComboBox<Parent> (parentStore);
			m_parentComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_parentComboBox->setMinimumWidth(170);
			vBox->addWidget(m_parentComboBox);
	groupBox = new QGroupBox (simulationName);
	addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
			m_simulationComboBox = new StoreAssociatedComboBox<Simulation> (simulationStore);
			m_simulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_simulationComboBox->setMinimumWidth(170);
			m_simulationComboBox->setParentBox(m_parentComboBox);
			vBox->addWidget(m_simulationComboBox);
}

template <class Parent, class Simulation>
void SimulationToolBar<Parent,Simulation>::setShownSimulationParent(Parent *newParent) {
	m_parentComboBox->setCurrentObject(newParent);
}


template class SimulationToolBar<TData, TDMSData>;
template class SimulationToolBar<CBlade, CDMSData>;
template class SimulationToolBar<CBlade, DMSData>;
