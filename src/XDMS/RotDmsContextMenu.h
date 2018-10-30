#ifndef ROTDMSCONTEXTMENU_H
#define ROTDMSCONTEXTMENU_H

#include "../TwoDContextMenu.h"
#include "RotDmsModule.h"


class RotDmsContextMenu : public TwoDContextMenu
{
	Q_OBJECT

public:
	RotDmsContextMenu(QWidget *parent, RotDmsModule *module);

	void setCurrentGraphType (NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti = NewGraph::None);

private:
	RotDmsModule *m_module;
	QAction *m_setGraphTypeRotorAction, *m_setGraphTypeBladeAction, *m_setGraphTypeAzimuthalAction,
			*m_setGraphTypeLegendAction;
	QAction *m_setOneCurveOnly, *m_setForEveryTipSpeed, *m_setForEverySimulation;

private slots:
	void changeToRotorGraph () { m_module->changeGraphType(NewGraph::TurbineRotor); }
	void changeToBladeGraph () { m_module->changeGraphType(NewGraph::TurbineBlade); }
	void changeToAzimuthalGraph () { m_module->changeGraphType(NewGraph::TurbineAzimuthal); }
	void changeToLegendGraph () { m_module->changeGraphType(NewGraph::RotorLegend); }
	void changeToOneCurve () { m_module->changeGraphTypeMulti(NewGraph::None); }
	void changeToForTipSpeed () { m_module->changeGraphTypeMulti(NewGraph::RotorTipSpeed); }
	void changeToForSimulations () { m_module->changeGraphTypeMulti(NewGraph::RotorAllSimulations); }
};

#endif // ROTDMSCONTEXTMENU_H
