#ifndef NOISEDOCK_H
#define NOISEDOCK_H

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
#include "NoiseModule.h"
#include "noisesimulationdialog.h"
#include "../StoreAssociatedComboBox_include.h"

#include "../XGlobals.h"
#include "../GLWidget.h"
#include "../TwoDWidget.h"

class NoiseSimulation;

class NoiseDock : public ScrolledDock
{
    Q_OBJECT
public:
    explicit NoiseDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, NoiseModule *module);
    ~NoiseDock();
    void initView();

signals:

public slots:

private:

    NoiseModule *m_Module;
    QPushButton *m_newButton;
    QPushButton *m_editCopyButton;
    QPushButton *m_deleteButton;
    QPushButton *m_renameButton;
    QLabel * m_simulationsLabel;
    NoiseSimulationComboBox * m_NoiseSimulationComboBox;

private slots:
    void onRenameButtonClicked();
    void onEditCopyButtonClicked ();
    void onDeleteButtonClicked();
    void onNewButtonClicked ();


};

#endif // NOISEDOCK_H
