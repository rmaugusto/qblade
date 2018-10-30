#ifndef MULTIDMSCONTEXTMENU_H
#define MULTIDMSCONTEXTMENU_H

#include "../TwoDContextMenu.h"
#include "MultiDmsModule.h"


class MultiDmsContextMenu : public TwoDContextMenu
{
	Q_OBJECT
	
public:
	MultiDmsContextMenu(QWidget *parent, MultiDmsModule *module);
	
	void setCurrentGraphType (NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti);
	
private:
	MultiDmsModule *m_module;
	QAction *m_setGraphTypeWindspeedAction, *m_setGraphTypeRotationalAction, *m_setGraphTypePitchAction;
	QAction *m_setOneCurveOnly, *m_setForEveryWindspeed, *m_setForEveryRotational, *m_setForEveryPitch;
	
private slots:
	void changeToWindspeedGraph () { m_module->changeGraphType(NewGraph::MultiWindspeed); }
	void changeToRotationalGraph () { m_module->changeGraphType(NewGraph::MultiRotational); }
	void changeToPitchGraph () { m_module->changeGraphType(NewGraph::MultiPitch); }
	void changeToOneCurve () { m_module->changeGraphTypeMulti(NewGraph::None); }
	void changeToForWindspeed () { m_module->changeGraphTypeMulti(NewGraph::MultiWindspeed); }
	void changeToForRotational () { m_module->changeGraphTypeMulti(NewGraph::MultiRotational); }
	void changeToForPitch () { m_module->changeGraphTypeMulti(NewGraph::MultiPitch); }
};

#endif // MULTIDMSCONTEXTMENU_H
