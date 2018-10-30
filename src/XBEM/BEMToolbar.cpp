#include "BEMToolbar.h"

#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include "../StoreAssociatedComboBox.h"
#include "../Store.h"
#include "BEM.h"


BEMToolbar::BEMToolbar(QMainWindow *parent)
{
	setObjectName("BEMToolbar");
	
    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));


	m_SingleGraphAct = new QAction(QIcon(":/images/single.png"), tr("Single Graph View"), this);
	m_SingleGraphAct->setCheckable(true);
	m_SingleGraphAct->setStatusTip(tr("Single Graph View"));

	m_HideWidgetsAct = new QAction(QIcon(":/images/hidden.png"), tr("Hide Widgets"), this);
	m_HideWidgetsAct->setCheckable(true);
	m_HideWidgetsAct->setStatusTip(tr("Hide Widgets"));

	addAction(m_SingleGraphAct);
	addAction(m_HideWidgetsAct);

	QGroupBox *groupBox = new QGroupBox (tr("Airfoils"));
	m_foilBox = addWidget(groupBox);
	QVBoxLayout *vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_foilComboBox = new FoilComboBox (&g_foilStore);
		m_foilComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_foilComboBox->setMinimumWidth(170);
		vBox->addWidget(m_foilComboBox);
	groupBox = new QGroupBox (tr("Polars"));
	m_polarBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_polarComboBox = new PolarComboBox (&g_polarStore);
		m_polarComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_polarComboBox->setMinimumWidth(170);
		m_polarComboBox->setParentBox(m_foilComboBox);
		vBox->addWidget(m_polarComboBox);
	groupBox = new QGroupBox (tr("360 Polars"));
	m_polar360Box = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_polar360ComboBox = new Polar360ComboBox (&g_360PolarStore);
		m_polar360ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_polar360ComboBox->setMinimumWidth(170);
		m_polar360ComboBox->setParentBox(m_polarComboBox);
		vBox->addWidget(m_polar360ComboBox);
	groupBox = new QGroupBox (tr("Rotor Blade"));
	m_rotorBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_rotorComboBox = new RotorComboBox (&g_rotorStore);
		m_rotorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_rotorComboBox->setMinimumWidth(170);
		vBox->addWidget(m_rotorComboBox);
	groupBox = new QGroupBox (tr("Rotor Simulation"));
	m_rotorSimulationBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_bemdataComboBox = new BEMDataComboBox (&g_bemdataStore);
		m_bemdataComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_bemdataComboBox->setMinimumWidth(170);
		m_bemdataComboBox->setParentBox(m_rotorComboBox);
		vBox->addWidget(m_bemdataComboBox);
	groupBox = new QGroupBox (tr("Tip Speed Ratio"));
	m_tipSpeedRationBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_tsrComboBox = new QComboBox();
		m_tsrComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_tsrComboBox->setMinimumWidth(170);
		vBox->addWidget(m_tsrComboBox);
	groupBox = new QGroupBox (tr("Multi Parameter BEM Simulation"));
	m_multiParameterSimulationBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_cbemdataComboBox = new CBEMDataComboBox (&g_cbemdataStore);
		m_cbemdataComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_cbemdataComboBox->setMinimumWidth(170);
		m_cbemdataComboBox->setParentBox(m_rotorComboBox);
		vBox->addWidget(m_cbemdataComboBox);
	groupBox = new QGroupBox (tr("Turbine"));
	m_turbineBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_tdataComboBox = new TDataComboBox (&g_tdataStore);
		m_tdataComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_tdataComboBox->setMinimumWidth(170);
		vBox->addWidget(m_tdataComboBox);
	groupBox = new QGroupBox (tr("Turbine Simulation"));
	m_turbineSimulationBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_tbemdataComboBox = new TBEMDataComboBox (&g_tbemdataStore);
		m_tbemdataComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_tbemdataComboBox->setMinimumWidth(170);
		m_tbemdataComboBox->setParentBox(m_tdataComboBox);
		vBox->addWidget(m_tbemdataComboBox);
	groupBox = new QGroupBox (tr("Windspeed [m/s]"));
	m_turbinewindspeedBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_turbinewindspeedComboBox = new QComboBox ();
		m_turbinewindspeedComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_turbinewindspeedComboBox->setMinimumWidth(170);
		vBox->addWidget(m_turbinewindspeedComboBox);
	groupBox = new QGroupBox (tr("Windspeed [m/s]"));
	m_windspeedBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_windspeedComboBox = new QComboBox ();
		m_windspeedComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_windspeedComboBox->setMinimumWidth(170);
		vBox->addWidget(m_windspeedComboBox);
	groupBox = new QGroupBox (tr("Rotational Speed [1/min]"));
	m_rotationalSpeedBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_rotComboBox = new QComboBox();
		m_rotComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_rotComboBox->setMinimumWidth(170);
		vBox->addWidget(m_rotComboBox);
	groupBox = new QGroupBox (tr("Pitch [deg]"));
	m_pitchBox = addWidget(groupBox);
		vBox = new QVBoxLayout ();
		groupBox->setLayout(vBox);
		m_pitchComboBox = new QComboBox();
		m_pitchComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		m_pitchComboBox->setMinimumWidth(170);
		vBox->addWidget(m_pitchComboBox);
			
	connect(m_SingleGraphAct, SIGNAL(triggered()), g_qbem, SLOT(OnSingleGraphs()));
	connect(m_HideWidgetsAct, SIGNAL(triggered()), g_qbem, SLOT(OnHideWidgets()));
    connect(m_rotorComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeWing(int)));
    connect(m_foilComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeFoil(int)));
    connect(m_polarComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangePolar(int)));
    connect(m_polar360ComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChange360Polar(int)));
	connect(m_bemdataComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeRotorSimulation()));
    connect(m_tdataComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeTurbine(int)));
	connect(m_tbemdataComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeTurbineSimulation()));
	connect(m_cbemdataComboBox,SIGNAL(valueChanged(int)), g_qbem, SLOT(OnSelChangeCharSimulation()));
	connect(m_tsrComboBox,SIGNAL(activated(int)), g_qbem, SLOT(OnSelChangeBladeData(int)));
	connect(m_windspeedComboBox,SIGNAL(activated(int)), g_qbem, SLOT(OnSelChangeWind(int)));
	connect(m_pitchComboBox,SIGNAL(activated(int)), g_qbem, SLOT(OnSelChangePitch(int)));
	connect(m_rotComboBox,SIGNAL(activated(int)), g_qbem, SLOT(OnSelChangeRot(int)));
    connect(m_turbinewindspeedComboBox,SIGNAL(activated(int)), g_qbem, SLOT(OnSelChangeTurbineBladeData(int)));



	parent->addToolBar(this);
	hide();

}

void BEMToolbar::DisableAll()
{
	m_rotorComboBox->setEnabled(false);
	m_rotComboBox->setEnabled(false);
	m_pitchComboBox->setEnabled(false);
	m_tsrComboBox->setEnabled(false);
	m_windspeedComboBox->setEnabled(false);
	m_turbinewindspeedComboBox->setEnabled(false);
	m_bemdataComboBox->setEnabled(false);
	m_tbemdataComboBox->setEnabled(false);
	m_cbemdataComboBox->setEnabled(false);
	m_tdataComboBox->setEnabled(false);
	m_polarComboBox->setEnabled(false);
	m_polar360ComboBox->setEnabled(false);
	m_foilComboBox->setEnabled(false);
}

void BEMToolbar::EnableAll()
{
    m_rotorComboBox->setEnabled(m_rotorComboBox->count());
    m_rotComboBox->setEnabled(m_rotComboBox->count());
    m_pitchComboBox->setEnabled(m_pitchComboBox->count());
    m_tsrComboBox->setEnabled(m_tsrComboBox->count());
    m_windspeedComboBox->setEnabled(m_windspeedComboBox->count());
    m_turbinewindspeedComboBox->setEnabled(m_turbinewindspeedComboBox->count());
    m_bemdataComboBox->setEnabled(m_bemdataComboBox->count());
    m_tbemdataComboBox->setEnabled(m_tbemdataComboBox->count());
    m_cbemdataComboBox->setEnabled(m_cbemdataComboBox->count());
    m_tdataComboBox->setEnabled(m_tdataComboBox->count());
    m_polarComboBox->setEnabled(m_polarComboBox->count());
    m_polar360ComboBox->setEnabled(m_polar360ComboBox->count());
    m_foilComboBox->setEnabled(m_foilComboBox->count());
}

void BEMToolbar::setState(ToolBarState newState) {
	m_rotorBox->setVisible(false);
	m_rotorSimulationBox->setVisible(false);
	m_tipSpeedRationBox->setVisible(false);
	m_multiParameterSimulationBox->setVisible(false);
	m_windspeedBox->setVisible(false);
	m_rotationalSpeedBox->setVisible(false);
	m_pitchBox->setVisible(false);
	m_turbineBox->setVisible(false);
	m_turbineSimulationBox->setVisible(false);
	m_foilBox->setVisible(false);
	m_polarBox->setVisible(false);
	m_polar360Box->setVisible(false);
	m_SingleGraphAct->setVisible(false);
	m_HideWidgetsAct->setVisible(false);
	m_turbinewindspeedBox->setVisible(false);
	
	switch (newState) {
	case BLADEVIEW_STATE:
		m_rotorBox->setVisible(true);
		m_HideWidgetsAct->setVisible(true);

		break;
	case ROTORVIEW_STATE:
		m_HideWidgetsAct->setVisible(true);
		m_SingleGraphAct->setVisible(true);
		m_rotorBox->setVisible(true);
		m_rotorSimulationBox->setVisible(true);
		m_tipSpeedRationBox->setVisible(true);
		break;
	case CHARACTERISTICVIEW_STATE:
		m_HideWidgetsAct->setVisible(true);
		m_SingleGraphAct->setVisible(true);
		m_rotorBox->setVisible(true);
		m_multiParameterSimulationBox->setVisible(true);
		m_windspeedBox->setVisible(true);
		m_rotationalSpeedBox->setVisible(true);
		m_pitchBox->setVisible(true);
		break;
	case TURBINEVIEW_STATE:
		m_HideWidgetsAct->setVisible(true);
		m_SingleGraphAct->setVisible(true);
		m_turbineBox->setVisible(true);
		m_turbineSimulationBox->setVisible(true);
		m_turbinewindspeedBox->setVisible(true);
		break;
	case POLARVIEW_STATE:
		m_HideWidgetsAct->setVisible(true);
		m_SingleGraphAct->setVisible(true);
		m_foilBox->setVisible(true);
		m_polarBox->setVisible(true);
		m_polar360Box->setVisible(true);
		break;
	}
}
