#ifndef FASTTWODCONTEXTMENU_H
#define FASTTWODCONTEXTMENU_H

class QAction;

#include "../TwoDContextMenu.h"
#include "FASTModule.h"


class FastTwoDContextMenu : public TwoDContextMenu
{
	Q_OBJECT
	
public:
	FastTwoDContextMenu(QWidget *parent, FASTModule *module);
	
	void setCurrentGraphType (NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti = NewGraph::None);
	
private:
	FASTModule *m_module;
	QAction *m_setGraphTypeFastAction, *m_setGraphTypeBladeAction;
	
private slots:
	void changeToFastGraph () { m_module->changeGraphType(NewGraph::FastSimulation); }
	void changeToBladeGraph () { m_module->changeGraphType(NewGraph::BladeGraph); }
};

#endif // FASTTWODCONTEXTMENU_H
