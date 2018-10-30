#ifndef TURDMSCONTEXTMENU_H
#define TURDMSCONTEXTMENU_H

#include "../TwoDContextMenu.h"
#include "TurDmsModule.h"


class TurDmsContextMenu : public TwoDContextMenu
{
	Q_OBJECT
	
public:
	TurDmsContextMenu(QWidget *parent, TurDmsModule *module);
	
	void setCurrentGraphType (NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti = NewGraph::None);
	
private:
	TurDmsModule *m_module;
	QAction *m_setGraphTypeRotorAction, *m_setGraphTypeBladeAction, *m_setGraphTypeAzimuthalAction,
	        *m_setGraphTypeWeibullAction, *m_setGraphTypeLegendAction;
	
private slots:
	void changeToRotorGraph () { m_module->changeGraphType(NewGraph::TurbineRotor); }
	void changeToBladeGraph () { m_module->changeGraphType(NewGraph::TurbineBlade); }
	void changeToAzimuthalGraph () { m_module->changeGraphType(NewGraph::TurbineAzimuthal); }
	void changeToWeibullGraph () { m_module->changeGraphType(NewGraph::TurbineWeibull); }
	void changeToLegendGraph () { m_module->changeGraphType(NewGraph::TurbineLegend); }
};

#endif // TURDMSCONTEXTMENU_H
