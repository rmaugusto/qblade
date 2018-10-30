#ifndef FASTMENU_H
#define FASTMENU_H

#include <QMenu>
#include "FASTModule.h"

class FASTMenu : public QMenu
{
    Q_OBJECT
public:
    FASTMenu(QMainWindow *parent, FASTModule *module);
private:
    QAction *m_writeFASTFiles;
    FASTModule *m_module;

private slots:
    void onAboutToShow();
    void onWriteFASTFiles();
};

#endif // FASTMENU_H
