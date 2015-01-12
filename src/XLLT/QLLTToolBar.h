#ifndef QLLTTOOLBAR_H
#define QLLTTOOLBAR_H

#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include "QLLTModule.h"
#include "../StoreAssociatedComboBox_include.h"

class QLLTToolBar : public QToolBar
{
    friend class QLLTModule;
    friend class QLLTDock;
    Q_OBJECT

public:
    QLLTToolBar(QMainWindow *parent, QLLTModule *module);

private:
    QLLTModule *m_module;

    QAction *TwoDView, *GLView;


    LLTSimulationComboBox *m_LLTSimulationComboBox;
    void useLLTSimulation (QLLTSimulation *newShownLLTSimulation);

};

#endif // QLLTTOOLBAR_H
