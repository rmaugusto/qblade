#ifndef QLLTDOCK_H
#define QLLTDOCK_H

#include <QObject>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressDialog>
#include <QThread>
#include <QDebug>
#include <QLabel>
#include <QDockWidget>
#include "../ScrolledDock.h"
#include "../XGlobals.h"
#include "QLLTModule.h"


class QLLTModule;

class QLLTDock : public ScrolledDock
{

    Q_OBJECT

public:
    QLLTDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QLLTModule *module);

    void initView();
    void setShownLLTSimulation(QLLTSimulation *newSimulation);

private:

    QPushButton *m_startSimulationButton;
    QPushButton *m_editCopyButton, *m_renameButton, *m_newButton, *m_deleteButton;

    QLLTModule *m_module;
    QLLTSimulation *m_shownLLTSimulation;


private slots:
    void onStartSimulationButtonClicked();
    void onNewButtonClicked();
    void onEditCopyButtonClicked ();
    void onRenameButtonClicked ();
    void onDeleteButtonClicked ();

};

#endif // QLLTDOCK_H
