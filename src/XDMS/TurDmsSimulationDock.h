#ifndef TURDMSSIMULATIONDOCK_H
#define TURDMSSIMULATIONDOCK_H

#include "../SimulationDock.h"
#include "../ParameterViewer.h"
class TurDmsModule;
class TDMSData;


class TurDmsSimulationDock : public SimulationDock<TData,TDMSData>, public ParameterViewer<Parameter::TDMSData>
{
	Q_OBJECT
	
public:
	TurDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, TurDmsModule *module);
	~TurDmsSimulationDock();
	
	virtual SimulationModule<TData,TDMSData>* module();
	virtual QList<NewGraph::GraphType> affectedGraphTypes();
	virtual void setShownObject (TDMSData *newSimulation);
	
private:	
	TurDmsModule *m_module;
	
private slots:
	virtual void onUnitsChanged();
	virtual void onEditCopyButtonClicked ();
	virtual void onRenameButtonClicked ();
	virtual void onDeleteButtonClicked ();
	virtual void onNewButtonClicked ();	
};

#endif // TURDMSSIMULATIONDOCK_H
