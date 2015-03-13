#ifndef NOISETOOLBAR_H
#define NOISETOOLBAR_H

#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include "NoiseModule.h"

class NoiseModule;

class NoiseToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit NoiseToolBar(QMainWindow *parent, NoiseModule *module);
    ~NoiseToolBar();

private:
    NoiseModule *m_Module;
    QAction *BpmteViewAction, *Q3dViewAction;

signals:

private slots:
    void OnQ3dView();
    void OnBpmteView();

public slots:

};

#endif // NOISETOOLBAR_H
