#ifndef SIMULATIONTOOLBAR_H
#define SIMULATIONTOOLBAR_H

#include <QToolBar>

template <class T>
class Store;
template <class T>
class StoreAssociatedComboBox;
template <class Parent, class Simulation>
class SimulationModule;


template <class Parent, class Simulation>
class SimulationToolBar : public QToolBar
{
//	Q_OBJECT  // incompatible
	
public:
	SimulationToolBar() { }
	virtual ~SimulationToolBar() { }
	
	virtual void setShownSimulationParent (Parent *newParent);
	virtual void setShownSimulation (Simulation *newSimulation) = 0;
	
protected:
	void constructSimulationToolBar(QString parentName, Store<Parent> *parentStore,
									QString simulationName, Store<Simulation> *simulationStore);
	virtual SimulationModule<Parent,Simulation>* module() = 0;
	QAction *m_hideDocksAction;	 // TODO NM dont need this reference? Make anonymous
	StoreAssociatedComboBox<Parent> *m_parentComboBox;
	StoreAssociatedComboBox<Simulation> *m_simulationComboBox;
};

#endif // SIMULATIONTOOLBAR_H
