#ifndef MULTIDMSSIMULATIONDOCK_H
#define MULTIDMSSIMULATIONDOCK_H

#include "../SimulationDock.h"
#include "../ParameterViewer.h"
class MultiDmsModule;
class CBlade;
class CDMSData;


class MultiDmsSimulationDock : public SimulationDock<CBlade,CDMSData>, public ParameterViewer<Parameter::CDMSData>
{
	Q_OBJECT
	
public:
	MultiDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, MultiDmsModule *module);
	~MultiDmsSimulationDock();
	
	virtual SimulationModule<CBlade,CDMSData>* module();
	virtual QList<NewGraph::GraphType> affectedGraphTypes();
	virtual void setShownObject (CDMSData *newSimulation);
	
private:	
	MultiDmsModule *m_module;
	
private slots:
	virtual void onUnitsChanged();
	virtual void onEditCopyButtonClicked ();
	virtual void onRenameButtonClicked ();
	virtual void onDeleteButtonClicked ();
	virtual void onNewButtonClicked ();	
};

#endif // MULTIDMSSIMULATIONDOCK_H
