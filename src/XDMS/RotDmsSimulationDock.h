#ifndef ROTDMSSIMULATIONDOCK_H
#define ROTDMSSIMULATIONDOCK_H

#include "../SimulationDock.h"
#include "../ParameterViewer.h"
class RotDmsModule;
class CBlade;
class DMSData;


class RotDmsSimulationDock : public SimulationDock<CBlade,DMSData>, public ParameterViewer<Parameter::DMSData>
{
	Q_OBJECT

public:
	RotDmsSimulationDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, RotDmsModule *module);
	~RotDmsSimulationDock();

	virtual SimulationModule<CBlade,DMSData>* module();
	virtual QList<NewGraph::GraphType> affectedGraphTypes();
	virtual void setShownObject (DMSData *newSimulation);

private:
	RotDmsModule *m_module;

private slots:
	virtual void onUnitsChanged();
	virtual void onEditCopyButtonClicked ();
	virtual void onRenameButtonClicked ();
	virtual void onDeleteButtonClicked ();
	virtual void onNewButtonClicked ();
};

#endif // ROTDMSSIMULATIONDOCK_H
