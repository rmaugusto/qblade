#include "TwoDContextMenu.h"

TwoDContextMenu::TwoDContextMenu(QWidget *parent, TwoDWidgetInterface *module)
	: QMenu (parent)
{
	m_module = module;

	m_showAllAction = new QAction (tr("Show All"), this);
	connect(m_showAllAction, SIGNAL(triggered()), this, SLOT(callShowAll()));
	addAction(m_showAllAction);
    m_hideAllAction = new QAction (tr("Show Current Only"), this);
	connect(m_hideAllAction, SIGNAL(triggered()), this, SLOT(callHideAll()));
	addAction(m_hideAllAction);
	m_resetScaleAction = new QAction (QString(tr("Reset Graph Scales")+"\t(R)"), this);
	connect(m_resetScaleAction, SIGNAL(triggered()), this, SLOT(callResetScale()));
	addAction(m_resetScaleAction);
	m_autoResetAction = new QAction (tr("No Automatic Reset"), this);
	m_autoResetAction->setCheckable(true);
	connect(m_autoResetAction, SIGNAL(triggered(bool)), this, SLOT(callAutoResetSwitch(bool)));
	addAction(m_autoResetAction);
	m_exportGraphAction = new QAction (QString(tr("Export Graph")), this);
	connect(m_exportGraphAction, SIGNAL(triggered()), this, SLOT(callExportGraph()));
	addAction(m_exportGraphAction);
}
