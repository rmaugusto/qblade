#ifndef TURDMSTURBINEDOCK_H
#define TURDMSTURBINEDOCK_H

class QLabel;
class QGroupBox;

#include "../CreatorDock.h"
#include "../ParameterViewer.h"
class TurDmsModule;
class TData;
class FixedSizeLabel;


class TurDmsTurbineDock : public CreatorDock<TData>, public ParameterViewer<Parameter::TData>
{
	Q_OBJECT
	
public:
	TurDmsTurbineDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, TurDmsModule *module);
	
	void setShownObject (TData *newTurbine);
	
private:
	TurDmsModule *m_module;
	
	QGroupBox *m_singleGroupBox, *m_variableGroupBox, *m_prescribedGroupBox;

private slots:
	virtual void onUnitsChanged ();  // should be implemented as slot
	virtual void onEditCopyButtonClicked ();
	virtual void onRenameButtonClicked ();
	virtual void onDeleteButtonClicked ();
	virtual void onNewButtonClicked ();	
};

#endif // TURDMSTURBINEDOCK_H
