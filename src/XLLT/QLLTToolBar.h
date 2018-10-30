/****************************************************************************

    QLLTToolBar Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef QLLTTOOLBAR_H
#define QLLTTOOLBAR_H
#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
class QLabel;
#include "QLLTModule.h"
#include "QLLTSimulation.h"
#include "../StoreAssociatedComboBox_include.h"


class QLLTToolBar : public QToolBar
{
    friend class QLLTModule;
    friend class QLLTDock;
    Q_OBJECT

private:
    class QLLTThread : public QThread
    {
    public:
        QLLTSimulation *simulation;

    private:
        void run () {simulation->onStartReplay(); }
    };


public:
    QLLTThread *m_QLLTThread;
    QLLTToolBar(QMainWindow *parent, QLLTModule *module);
    void OnHAWTView();
    void OnVAWTView();
    void DisableBoxes();
    void EnableBoxes();
    bool isReplayRunning() { if (!m_QLLTThread) return false; else return m_QLLTThread->isRunning();}
    int getCurrentBladeSection();
    void setShownTimeForAllSimulations();
    QSlider* GetTimeStepSlider() { return m_Timesteps; }
    double GetDelay(){ return m_DelayBox->value();}

    LLTSimulationComboBox *m_LLTHAWTSimulationComboBox;
    LLTSimulationComboBox *m_LLTVAWTSimulationComboBox;

    QSlider *m_Timesteps;
    QLabel *m_TimeLabel;
    QPushButton *m_startReplay;
    QDoubleSpinBox *m_DelayBox;


private:
    QLLTModule *m_module;

    QAction *TwoDView, *GLView;
    QAction* VAWTBox;
    QAction* HAWTBox;

    void useLLTSimulation (QLLTSimulation *newShownLLTSimulation);

private slots:
    void OnTimeSliderChanged();
    void onStartReplay();
    void onStopReplay();

public slots:
    void SetTimeStepSlider(int i) { m_Timesteps->setValue(i); }

};

#endif // QLLTTOOLBAR_H
