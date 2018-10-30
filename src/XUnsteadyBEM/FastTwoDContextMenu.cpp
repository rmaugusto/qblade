#include "FastTwoDContextMenu.h"

#include <QAction>

#include "FASTModule.h"
#include "../MainFrame.h"


FastTwoDContextMenu::FastTwoDContextMenu(QWidget *parent, FASTModule *module) 
	: TwoDContextMenu (parent, module)
{
	m_module = module;
	
	addSeparator();
	m_setGraphTypeFastAction = new QAction (tr("FAST Graph"), this);
	m_setGraphTypeFastAction->setCheckable(true);
	connect(m_setGraphTypeFastAction, SIGNAL(triggered()), this, SLOT(changeToFastGraph()));
	addAction(m_setGraphTypeFastAction);
	m_setGraphTypeBladeAction = new QAction (tr("Blade Graph"), this);
	m_setGraphTypeBladeAction->setCheckable(true);
	connect(m_setGraphTypeBladeAction, SIGNAL(triggered()), this, SLOT(changeToBladeGraph()));
	addAction(m_setGraphTypeBladeAction);
}

void FastTwoDContextMenu::setCurrentGraphType(NewGraph::GraphType graphType, NewGraph::GraphType /*graphTypeMulti*/) {
	m_setGraphTypeFastAction->setChecked(graphType == NewGraph::FastSimulation);
	m_setGraphTypeBladeAction->setChecked(graphType == NewGraph::BladeGraph);
}
