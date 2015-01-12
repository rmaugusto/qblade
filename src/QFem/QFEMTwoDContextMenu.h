#ifndef QFEMTWODCONTEXTMENU_H
#define QFEMTWODCONTEXTMENU_H

class QAction;

#include "QFEMModule.h"
#include "../TwoDContextMenu.h"

class QFEMTwoDContextMenu : public TwoDContextMenu
{
    Q_OBJECT

public:
    QFEMTwoDContextMenu(QWidget *parent, QFEMModule *module);

private:
    QFEMModule *m_module;

private slots:
};

#endif // QFEMTWODCONTEXTMENU_H
