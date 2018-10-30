#ifndef SIMULATIONDOCK_H
#define SIMULATIONDOCK_H

#include "CreatorTwoDDock.h"
#include "Graph/NewGraph.h"
#include "SimulationModule.h"
#include "XDMS/TDMSData.h"
#include "XDMS/CDMSData.h"


/* To access derived members within this class' functions, it is necessary to specify with the 'this' pointer.
 * This issue is the result of a template class inheriting a template class. More information at:
 * http://stackoverflow.com/questions/1120833/derived-template-class-access-to-base-class-member-data
 * */

template <class Parent, class Simulation>
class SimulationDock : public CreatorTwoDDock<Simulation>
{
//	Q_OBJECT  // incompatible
	
public:
	SimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags);
	virtual ~SimulationDock() { }
	virtual void setShownObject (Simulation *newSimulation);	
	
protected:
	virtual SimulationModule<Parent,Simulation>* module() = 0;
	virtual TwoDWidgetInterface* twoDInterface() { return module(); }
	
	QPushButton *m_startSimulationButton;
	
//protected slots:
	void onStartSimulationButtonClicked ();
};


template <class Parent, class Simulation>
SimulationDock<Parent,Simulation>::SimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags)
	: CreatorTwoDDock<Simulation> (title, parent, flags)
{
	m_startSimulationButton = new QPushButton ("Start Simulation");
	this->connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT(onStartSimulationButtonClicked()));
	this->m_controlsGridLayout->addWidget(m_startSimulationButton, 0, 0, 1, 2);
}

template <class Parent, class Simulation>
void SimulationDock<Parent,Simulation>::setShownObject(Simulation *newSimulation) {
	CreatorTwoDDock<Simulation>::setShownObject(newSimulation);
	
	if (this->m_shownObject) {
		this->m_startSimulationButton->setEnabled(!this->m_shownObject->hasResults());
	} else {
		this->m_startSimulationButton->setEnabled(false);
	}
}

template <class Parent, class Simulation>
void SimulationDock<Parent,Simulation>::onStartSimulationButtonClicked() {
	this->m_shownObject->startSimulation();
	module()->setShownSimulation(this->m_shownObject, true);  // enforce reload
	foreach(NewGraph::GraphType type, this->affectedGraphTypes()) {
		module()->reloadForGraphType(type);
	}
}

#endif // SIMULATIONDOCK_H
