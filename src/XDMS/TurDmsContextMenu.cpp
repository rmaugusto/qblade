#include "TurDmsContextMenu.h"

#include "TurDmsModule.h"


TurDmsContextMenu::TurDmsContextMenu(QWidget *parent, TurDmsModule *module) 
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
	m_setGraphTypeWeibullAction = new QAction (tr("Weibull Graph"), this);
	m_setGraphTypeWeibullAction->setCheckable(true);
	connect(m_setGraphTypeWeibullAction, SIGNAL(triggered()), this, SLOT(changeToWeibullGraph()));
	addAction(m_setGraphTypeWeibullAction);
	m_setGraphTypeLegendAction = new QAction (tr("Show Legend"), this);
	m_setGraphTypeLegendAction->setCheckable(true);
	connect(m_setGraphTypeLegendAction, SIGNAL(triggered()), this, SLOT(changeToLegendGraph()));
	addAction(m_setGraphTypeLegendAction);
}

void TurDmsContextMenu::setCurrentGraphType(NewGraph::GraphType graphType, NewGraph::GraphType /*graphTypeMulti*/) {
	m_setGraphTypeRotorAction->setChecked(graphType == NewGraph::TurbineRotor);
	m_setGraphTypeBladeAction->setChecked(graphType == NewGraph::TurbineBlade);
	m_setGraphTypeAzimuthalAction->setChecked(graphType == NewGraph::TurbineAzimuthal);
	m_setGraphTypeWeibullAction->setChecked(graphType == NewGraph::TurbineWeibull);
	m_setGraphTypeLegendAction->setChecked(graphType == NewGraph::TurbineLegend);
}
