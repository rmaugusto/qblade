#ifndef NOISETOOLBAR_H
#define NOISETOOLBAR_H

#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include "NoiseModule.h"
#include "../StoreAssociatedComboBox_include.h"

class NoiseModule;

class NoiseToolBar : public QToolBar
{
    friend class NoiseModule;
    friend class NoiseDock;
    Q_OBJECT
public:
    explicit NoiseToolBar(QMainWindow *parent, NoiseModule *module);
    ~NoiseToolBar();

private:
    NoiseModule *m_module;
    QAction *TwoDView, *GLView;

signals:

public slots:

};

#endif // NOISETOOLBAR_H
