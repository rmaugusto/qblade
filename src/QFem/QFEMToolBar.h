#ifndef QFEMTOOLBAR_H
#define QFEMTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include "QFEMModule.h"
#include "../XBEM/Blade.h"
#include "BladeStructure.h"
#include "../StoreAssociatedComboBox_include.h"


class QFEMToolBar : public QToolBar
{
	friend class QFEMModule;
    friend class QFEMDock;
	Q_OBJECT

public:
	QFEMToolBar (QMainWindow *parent, QFEMModule *module);
    QAction *TwoDView, *HideWidgetsAct, *GLView;

private:
    QFEMModule *m_module;

	RotorComboBox *m_rotorComboBox;
	BladeStructureComboBox *m_BladeStructureComboBox;
    BladeStructureLoadingComboBox *m_BladeStructureLoadingComboBox;

};

#endif // QFEMTOOLBAR_H
