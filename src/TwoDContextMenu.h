#ifndef TWODCONTEXTMENU_H
#define TWODCONTEXTMENU_H

#include <QMenu>
class QAction;

#include "TwoDWidgetEventInterface.h"


class TwoDContextMenu : public QMenu
{
	Q_OBJECT

public:
	TwoDContextMenu(QWidget *parent, TwoDWidgetInterface *module);

	void setAutoReset (bool status) { m_autoResetAction->setChecked(status); }
	
private:
	TwoDWidgetInterface *m_module;
	QAction *m_resetScaleAction, *m_autoResetAction, *m_exportGraphAction, *m_showAllAction, *m_hideAllAction;
	
private slots:
	void callShowAll () { m_module->showAll(); }
	void callHideAll () { m_module->hideAll(); }
	void callResetScale () { m_module->resetScale(); }  // can't be connected directly: TwoDWidgetInterface is not a QObject
	void callAutoResetSwitch (bool status) { m_module->autoResetSwitch(status); }
	void callExportGraph () { m_module->exportGraph(); }
};

#endif // TWODCONTEXTMENU_H
