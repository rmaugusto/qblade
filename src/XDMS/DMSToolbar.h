#ifndef DMSTOOLBAR_H
#define DMSTOOLBAR_H

#include <QToolBar>
#include <QAction>
#include "../XWidgets.h"
#include "../StoreAssociatedComboBox_include.h"


class DMSToolbar : public QToolBar
{
	friend class QDMS;
	Q_OBJECT

public:
	enum ToolBarState {BLADEVIEW_STATE, ROTORVIEW_STATE, CHARACTERISTICVIEW_STATE, TURBINEVIEW_STATE};

	DMSToolbar(QMainWindow *parent);
	void setState (ToolBarState newState);
	void EnableAll();
	void DisableAll();
    RotorComboBox* getRotorBox(){ return m_rotorComboBox; }

private:
	QAction *m_SingleGraphAct, *m_HideWidgetsAct, *m_rotorBox, *m_rotorSimulationBox, *m_tipSpeedRationBox, *m_multiParameterSimulationBox, *m_windspeedBox, *m_heightBox,
			*m_rotationalSpeedBox, *m_pitchBox, *m_turbineBox, *m_turbineSimulationBox, *m_turbinewindspeedBox, *m_turbineheightBox;
	QComboBox *m_rotComboBox, *m_pitchComboBox, *m_tsrComboBox, *m_windspeedComboBox, *m_heightComboBox, *m_turbinewindspeedComboBox, *m_turbineheightComboBox;
	RotorComboBox *m_rotorComboBox;
	TDataComboBox *m_verttdataComboBox;
	DMSDataComboBox *m_dmsdataComboBox;
	TDMSDataComboBox *m_tdmsdataComboBox;
	CDMSDataComboBox *m_cdmsdataComboBox;

};

#endif // DMSTOOLBAR_H
