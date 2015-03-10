#ifndef NOISESIMULATIONDIALOG_H
#define NOISESIMULATIONDIALOG_H

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

private slots:
    void on_NoiseSimulationDialog_accepted();

private:
    Ui::NoiseSimulationDialog *ui;
};

#endif // NOISESIMULATIONDIALOG_H
