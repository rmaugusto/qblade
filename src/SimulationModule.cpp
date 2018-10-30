#include "SimulationModule.h"

#include "SimulationDock.h"
#include "SimulationToolBar.h"
class TData;


template <class Parent, class Simulation>
SimulationModule<Parent,Simulation>::SimulationModule()
{
	m_shownSimulationParent = NULL;
	m_shownSimulation = NULL;
}

template <class Parent, class Simulation>
SimulationModule<Parent,Simulation>::~SimulationModule() {
	
}

template <class Parent, class Simulation>
void SimulationModule<Parent,Simulation>::onHideDocks(bool hide) {
	simulationDock()->setVisible(!hide);
}

template <class Parent, class Simulation>
void SimulationModule<Parent,Simulation>::setShownSimulationParent(Parent *newParent, bool forceReload) {
	if (forceReload || m_shownSimulationParent != newParent) {
		m_shownSimulationParent = newParent;
		toolBar()->setShownSimulationParent(m_shownSimulationParent);
	}
}

template <class Parent, class Simulation>
void SimulationModule<Parent,Simulation>::setShownSimulation(Simulation *newSimulation, bool forceReload) {
	if (forceReload || m_shownSimulation != newSimulation) {
		m_shownSimulation = newSimulation;
		simulationDock()->setShownObject(m_shownSimulation);
		toolBar()->setShownSimulation(m_shownSimulation);
	}
}


/* to make it possible to separate the template header and implementation, all needed types have to be
 * instantiated here. For more information see:
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 * */
template class SimulationModule<TData, TDMSData>;
template class SimulationModule<CBlade, CDMSData>;
template class SimulationModule<CBlade, DMSData>;
