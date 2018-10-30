#ifndef FASTDOCK_H
#define FASTDOCK_H

class QLabel;
class QCheckBox;
class QPushButton;

#include "../ScrolledDock.h"
class FASTSimulation;
class FASTModule;
class LineButton;
class FixedSizeLabel;


class FASTDock : public ScrolledDock
{
	Q_OBJECT
	
public:
	FASTDock (const QString &title, QMainWindow *parent, Qt::WindowFlags flags, FASTModule *module);
	
	void setShownFASTSimulation (FASTSimulation *newSimulation);
	void adjustShowCheckBox ();  // necessary because showAll and hideAll can change the status
	
private:
	FASTModule *m_module;
	FASTSimulation *m_shownFASTSimulation;
	
	QLabel *m_totalRuntimeLabel, *m_timeStepLabel, *m_numberOfBladesLabel, *m_rotorSpeedLabel,
		   *m_nacelleYawLabel, *m_gravityLabel, *m_airDensLabel, *m_kinViscLabel;
	QLabel *m_aeroTimeStepLabel, *m_stallModLabel, *m_useCmLabel, *m_infModLabel, *m_indModLabel,
		   *m_tlModelLabel, *m_hlModelLabel, *m_dofLabel;
	FixedSizeLabel *m_windfieldLabel, *m_rotorLabel, *m_bladeStructureLabel;
	LineButton *m_simulationLineButton;
	QCheckBox *m_showCheckBox, *m_showPointsCheckBox, *m_showCurveCheckBox;
	QPushButton *m_startSimulationButton;
	QPushButton *m_editCopyButton, *m_renameButton, *m_newButton, *m_deleteButton;
	
private slots:
	void onLineButtonClicked ();
	void onShowCheckBoxCanged ();
	void onShowPointsCheckBoxCanged ();
	void onShowCurveCheckBoxCanged ();
	void onStartSimulationButtonClicked ();
	void onEditCopyButtonClicked ();
	void onRenameButtonClicked ();
	void onDeleteButtonClicked ();
	void onNewButtonClicked ();
};

#endif // FASTDOCK_H
