#include "RotDmsContextMenu.h"

#include "RotDmsModule.h"


RotDmsContextMenu::RotDmsContextMenu(QWidget *parent, RotDmsModule *module)
	: TwoDContextMenu (parent, module)
{
	m_module = module;

	addSeparator();
	m_setGraphTypeRotorAction = new QAction (tr("Rotor Graph"), this);
	m_setGraphTypeRotorAction->setCheckable(true);
	connect(m_setGraphTypeRotorAction, SIGNAL(triggered()), this, SLOT(changeToRotorGraph()));
	addAction(m_setGraphTypeRotorAction);
	m_setGraphTypeBladeAction = new QAction (tr("Blade Graph"), this);
	m_setGraphTypeBladeAction->setCheckable(true);
	connect(m_setGraphTypeBladeAction, SIGNAL(triggered()), this, SLOT(changeToBladeGraph()));
	addAction(m_setGraphTypeBladeAction);
	m_setGraphTypeAzimuthalAction = new QAction (tr("Azimuthal Graph"), this);
	m_setGraphTypeAzimuthalAction->setCheckable(true);
	connect(m_setGraphTypeAzimuthalAction, SIGNAL(triggered()), this, SLOT(changeToAzimuthalGraph()));
	addAction(m_setGraphTypeAzimuthalAction);
	m_setGraphTypeLegendAction = new QAction (tr("Show Legend"), this);
	m_setGraphTypeLegendAction->setCheckable(true);
	connect(m_setGraphTypeLegendAction, SIGNAL(triggered()), this, SLOT(changeToLegendGraph()));
	addAction(m_setGraphTypeLegendAction);

	addSeparator();
	m_setOneCurveOnly = new QAction ("Single Curve", this);
	m_setOneCurveOnly->setCheckable(true);
	connect(m_setOneCurveOnly, SIGNAL(triggered()), this, SLOT(changeToOneCurve()));
	addAction(m_setOneCurveOnly);
	m_setForEveryTipSpeed = new QAction ("For Every Tip Speed Ratio", this);
	m_setForEveryTipSpeed->setCheckable(true);
	connect(m_setForEveryTipSpeed, SIGNAL(triggered()), this, SLOT(changeToForTipSpeed()));
	addAction(m_setForEveryTipSpeed);
	m_setForEverySimulation = new QAction ("For Every Simulation", this);
	m_setForEverySimulation->setCheckable(true);
	connect(m_setForEverySimulation, SIGNAL(triggered()), this, SLOT(changeToForSimulations()));
	addAction(m_setForEverySimulation);
}

void RotDmsContextMenu::setCurrentGraphType(NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti) {
	m_setGraphTypeRotorAction->setChecked(graphType == NewGraph::TurbineRotor);
	m_setGraphTypeBladeAction->setChecked(graphType == NewGraph::TurbineBlade);
	m_setGraphTypeAzimuthalAction->setChecked(graphType == NewGraph::TurbineAzimuthal);
	m_setGraphTypeLegendAction->setChecked(graphType == NewGraph::RotorLegend);

	m_setOneCurveOnly->setChecked(graphTypeMulti == NewGraph::None);
	m_setOneCurveOnly->setEnabled(graphType != NewGraph::TurbineRotor && graphType != NewGraph::RotorLegend);
	m_setForEveryTipSpeed->setChecked(graphTypeMulti == NewGraph::RotorTipSpeed);
	m_setForEveryTipSpeed->setEnabled(graphType != NewGraph::TurbineRotor && graphType != NewGraph::RotorLegend);
	m_setForEverySimulation->setChecked(graphTypeMulti == NewGraph::RotorAllSimulations);
}
