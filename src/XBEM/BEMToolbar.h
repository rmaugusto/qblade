#ifndef BEMTOOLBAR_H
#define BEMTOOLBAR_H

#include <QToolBar>
#include <QAction>
class QComboBox;

#include "../StoreAssociatedComboBox_include.h"


class BEMToolbar : public QToolBar
{
    friend class QBEM;
	Q_OBJECT
	
public:
	enum ToolBarState {BLADEVIEW_STATE, ROTORVIEW_STATE, CHARACTERISTICVIEW_STATE, TURBINEVIEW_STATE, POLARVIEW_STATE};
	
	BEMToolbar(QMainWindow *parent);
	void setState (ToolBarState newState);
	void EnableAll();
	void DisableAll();
	
private:
	QAction *m_SingleGraphAct, *m_HideWidgetsAct, *m_rotorBox, *m_rotorSimulationBox, *m_tipSpeedRationBox, *m_multiParameterSimulationBox, *m_windspeedBox,
			*m_rotationalSpeedBox, *m_pitchBox, *m_turbineBox, *m_turbineSimulationBox, *m_foilBox, *m_polarBox, *m_polar360Box, *m_turbinewindspeedBox;
	QComboBox *m_rotComboBox, *m_pitchComboBox, *m_tsrComboBox, *m_windspeedComboBox, *m_turbinewindspeedComboBox;
	RotorComboBox *m_rotorComboBox;
	BEMDataComboBox *m_bemdataComboBox;
	TBEMDataComboBox *m_tbemdataComboBox;
	CBEMDataComboBox *m_cbemdataComboBox;
	TDataComboBox *m_tdataComboBox;
	PolarComboBox *m_polarComboBox;
	Polar360ComboBox *m_polar360ComboBox;
	FoilComboBox *m_foilComboBox;

	
};

#endif // BEMTOOLBAR_H
