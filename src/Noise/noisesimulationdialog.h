#ifndef NOISESIMULATIONDIALOG_H
#define NOISESIMULATIONDIALOG_H

#include "../XDirect/XDirect.h"
#include "noisecalculation.h"

#include <QDialog>

namespace Ui{
    class NoiseSimulationDialog;
}

class NoiseSimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseSimulationDialog(QWidget *parent = 0);
    ~NoiseSimulationDialog();

    /**
     * @brief initComponents Initializate/configure the components
     */
    void initComponents();
    /**
     * @brief loadComponents Load the components data
     */
    void loadComponents();
    void readWindowParams(NoiseParameter *nParam);
private slots:
    void on_NoiseSimulationDialog_accepted();

    void on_deltaSourceXFoil_toggled(bool checked);

    void on_deltaSourceBPM_toggled(bool checked);

private:
    Ui::NoiseSimulationDialog *ui;
    void showEvent(QShowEvent * sw);
    QXDirect *pXDirect;
};

#endif // NOISESIMULATIONDIALOG_H
