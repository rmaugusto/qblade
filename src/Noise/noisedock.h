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

    NoiseModule *m_module;


private slots:
    void OnClickX();
};

#endif // NOISEDOCK_H
