#include "QFEMTwoDContextMenu.h"

#include <QAction>

#include "QFEMModule.h"
#include "../MainFrame.h"


QFEMTwoDContextMenu::QFEMTwoDContextMenu(QWidget *parent, QFEMModule *module)
    : TwoDContextMenu (parent, module)
{
    m_module = module;
}
