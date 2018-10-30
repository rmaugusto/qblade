#include "MultiDmsContextMenu.h"


MultiDmsContextMenu::MultiDmsContextMenu(QWidget *parent, MultiDmsModule *module) 
	: TwoDContextMenu (parent, module),
	  m_module(module)
{
	addSeparator();
	m_setGraphTypeWindspeedAction = new QAction ("Windspeed Curves", this);
	m_setGraphTypeWindspeedAction->setCheckable(true);
	connect(m_setGraphTypeWindspeedAction, SIGNAL(triggered()), this, SLOT(changeToWindspeedGraph()));
	addAction(m_setGraphTypeWindspeedAction);
	m_setGraphTypeRotationalAction = new QAction (tr("Rotational Speed Curves"), this);
	m_setGraphTypeRotationalAction->setCheckable(true);
	connect(m_setGraphTypeRotationalAction, SIGNAL(triggered()), this, SLOT(changeToRotationalGraph()));
	addAction(m_setGraphTypeRotationalAction);
	m_setGraphTypePitchAction = new QAction ("Pitch Angle Curves", this);
	m_setGraphTypePitchAction->setCheckable(true);
	connect(m_setGraphTypePitchAction, SIGNAL(triggered()), this, SLOT(changeToPitchGraph()));
	addAction(m_setGraphTypePitchAction);
	
	addSeparator();
	m_setOneCurveOnly = new QAction ("Single Curve", this);
	m_setOneCurveOnly->setCheckable(true);
	connect(m_setOneCurveOnly, SIGNAL(triggered()), this, SLOT(changeToOneCurve()));
	addAction(m_setOneCurveOnly);
	m_setForEveryWindspeed = new QAction ("For Every Windspeed", this);
	m_setForEveryWindspeed->setCheckable(true);
	connect(m_setForEveryWindspeed, SIGNAL(triggered()), this, SLOT(changeToForWindspeed()));
	addAction(m_setForEveryWindspeed);
	m_setForEveryRotational = new QAction ("For Every Rotational Speed", this);
	m_setForEveryRotational->setCheckable(true);
	connect(m_setForEveryRotational, SIGNAL(triggered()), this, SLOT(changeToForRotational()));
	addAction(m_setForEveryRotational);
	m_setForEveryPitch = new QAction ("For Every Pitch Angle", this);
	m_setForEveryPitch->setCheckable(true);
	connect(m_setForEveryPitch, SIGNAL(triggered()), this, SLOT(changeToForPitch()));
	addAction(m_setForEveryPitch);
}

void MultiDmsContextMenu::setCurrentGraphType(NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti) {
	m_setGraphTypeWindspeedAction->setChecked(graphType == NewGraph::MultiWindspeed);
	m_setGraphTypeRotationalAction->setChecked(graphType == NewGraph::MultiRotational);
	m_setGraphTypePitchAction->setChecked(graphType == NewGraph::MultiPitch);
	
	m_setOneCurveOnly->setChecked(graphTypeMulti == NewGraph::None);
	m_setForEveryWindspeed->setChecked(graphTypeMulti == NewGraph::MultiWindspeed);
	m_setForEveryWindspeed->setEnabled(graphType != NewGraph::MultiWindspeed);
	m_setForEveryRotational->setChecked(graphTypeMulti == NewGraph::MultiRotational);
	m_setForEveryRotational->setEnabled(graphType != NewGraph::MultiRotational);
	m_setForEveryPitch->setChecked(graphTypeMulti == NewGraph::MultiPitch);
	m_setForEveryPitch->setEnabled(graphType != NewGraph::MultiPitch);
}
