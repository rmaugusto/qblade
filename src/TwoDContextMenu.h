#ifndef TWODCONTEXTMENU_H
#define TWODCONTEXTMENU_H

#include <QMenu>
class QAction;

#include "TwoDWidgetInterface.h"


class TwoDContextMenu : public QMenu
{
	Q_OBJECT

public:
	TwoDContextMenu(QWidget *parent, TwoDWidgetInterface *module);

	void setAutoReset (bool status) { m_autoResetAction->setChecked(status); }
	virtual void setCurrentGraphType (NewGraph::GraphType /*graphType*/, NewGraph::GraphType /*graphTypeMulti*/) { }

private:
	TwoDWidgetInterface *m_module;  // NM TODO double storage of module here and in inherited class
	QAction *m_resetScaleAction, *m_autoResetAction, *m_exportGraphAction, *m_showAllAction, *m_hideAllAction;
	
private slots:
	void callShowAll () { m_module->showAll(); }
	void callHideAll () { m_module->hideAll(); }
	void callResetScale () { m_module->resetScale(true); }  // can't be connected directly: TwoDWidgetInterface not a QObject
	void callAutoResetSwitch (bool status) { m_module->autoResetSwitch(status); }
	void callExportGraph () { m_module->exportGraph(); }
};

#endif // TWODCONTEXTMENU_H
