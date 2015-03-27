#ifndef NOISESIMULATIONDIALOG_H
#define NOISESIMULATIONDIALOG_H

#include "../XDirect/XDirect.h"
#include "noisecalculation.h"
#include "noisesimulation.h"

#include <QDialog>

namespace Ui{
    class NoiseSimulationDialog;
}

class NoiseSimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseSimulationDialog(QWidget *parent = 0,NoiseSimulation * sim=0);
    ~NoiseSimulationDialog();

    /**
     * @brief initComponents Initializate/configure the components
     */
    void initComponents();
    /**
     * @brief loadComponents Load the components data
     */
    void loadComponents();
private slots:

    void on_deltaSourceXFoil_toggled(bool checked);

    void on_deltaSourceBPM_toggled(bool checked);

    void on_buttonOK_clicked();

    void on_buttonCancel_clicked();
    void finished(int result);

private:
    bool validateDoubleValue(QLineEdit * txt);
    void readCalculationParams();
    void readWindowParams();
    bool validateInputs();

    Ui::NoiseSimulationDialog *ui;
    NoiseSimulation * m_NS;
    bool m_NSCreated;
    void showEvent(QShowEvent * sw);
    QXDirect *pXDirect;
};

#endif // NOISESIMULATIONDIALOG_H
