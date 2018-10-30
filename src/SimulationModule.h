#ifndef SIMULATIONMODULE_H
#define SIMULATIONMODULE_H

#include <QDebug>

#include "Module.h"
template<class Parent, class Simulation>
class SimulationToolBar;
template<class Parent, class Simulation>
class SimulationDock;
class CBlade;

/* The module system is a problem of dual inheritance hierarchy. The base classes called Simulation* are interconnected.
 * For each implemented module each of the Simulation* classes is inherited. These inherited classes are interconnected
 * as well. To avoid saving a reference in the base and the derived class each, only the derived classes save a
 * reference to other derived classes of the same module, while the base classes provide a pure virtual function to get
 * access to that reference, which must be implemented by the derived class. This pattern is called
 * "Intelligent Children". Example:
 * virtual SimulationToolBar<Parent,Simulation>* toolBar() = 0;
 * 
 * To avoid code repetition and to provide a constant interface, there are member functions called in the base classes,
 * which should take as argument a reference to a certain class determined by the certain module. As covariance in
 * function parameters is not an option, this is solved by template classes. The base classes are templates, and the
 * deriving classes inherit a certain instantiation of that template. Example:
 * virtual void setShownSimulationParent (Parent *newParent, bool forceReload = false);
 * 
 * As Q_OBJECT is not compatible with template classes, the signals and slots are implemented in Simulation*SignalSlots
 * classes, which can be inherited by the actual base classes. If any type in the Simulation*SignalSlots classes needs
 * to be template like, the containing function must be declared for each needed class instead. Because a deriving class
 * will implement only a certain one of those functions, they cannot be declared pure virtual. The default
 * implementation shoud crash the program instead, to prevent its use. Example:
 * virtual void setShownSimulation (TDMSData *newSimulation, bool forceReload = false) { throw "illegal use"; }
 * */


class SimulationModuleSignalsSlots : public ModuleBase, public TwoDModule
{
	Q_OBJECT
	
public slots:
	virtual void onHideDocks (bool hide) = 0;
	
	/* As QObject does not support class templates, each specialization must be created manually */
	virtual void setShownSimulationParent (CBlade */*newParent*/, bool /*forceReload*/ = false) { throw "illegal use"; }
//	virtual void setShownSimulation (TDMSData */*newSimulation*/, bool /*forceReload*/ = false) { throw "illegal use"; }
//	virtual void setShownSimulation (CDMSData */*newSimulation*/, bool /*forceReload*/ = false) { throw "illegal use"; }
};


template <class Parent, class Simulation>
class SimulationModule : public SimulationModuleSignalsSlots
{
//	Q_OBJECT  // incompatible
	
public:
	SimulationModule();
	virtual ~SimulationModule();
	
	Parent *getShownSimulationParent () { return m_shownSimulationParent; }
	Simulation *getShownSimulation () { return m_shownSimulation; }
	
protected:
	virtual SimulationDock<Parent,Simulation>* simulationDock() = 0;
	virtual SimulationToolBar<Parent,Simulation>* toolBar() = 0;
	
	Parent *m_shownSimulationParent;
	Simulation *m_shownSimulation;
	
public /*slots*/:
	virtual void setShownSimulationParent (Parent *newParent, bool forceReload = false);
	virtual void setShownSimulation (Simulation *newSimulation, bool forceReload = false);	
	virtual void onHideDocks (bool hide);	
};


#endif // SIMULATIONMODULE_H
