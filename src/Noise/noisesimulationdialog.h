#ifndef NOISESIMULATIONDIALOG_H
#define NOISESIMULATIONDIALOG_H

#include "../XDirect/XDirect.h"
#include "noisecalculation.h"
#include "noisesimulation.h"
#include "noiseoppoint.h"

#include <QDialog>
#include <QMetaType>

namespace Ui{
    class NoiseSimulationDialog;
}

class ListItemNoiseOpPoint : public NoiseOpPoint
{
};
Q_DECLARE_METATYPE(ListItemNoiseOpPoint)

class NoiseSimulationDialog : public QDialog
{
    Q_OBJECT

friend class OpPoint;

class QXDirect;

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

    NoiseSimulation * GetNoiseSimulation();
private slots:

    void on_deltaSourceXFoil_toggled(bool checked);

    void on_deltaSourceBPM_toggled(bool checked);

    void on_buttonOK_clicked();

    void on_buttonCancel_clicked();
    void closeEvent(QCloseEvent * event);

private:
    bool validateDoubleValue(QLineEdit * txt);
    void readCalculationParams();
    void readWindowParams();
    bool validateInputs();
    void loadLinearInterpolate();
    void CreateXBL(OpPoint * cur_pOpPoint, double xs[IVX][3], int &nside1, int &nside2);


    Ui::NoiseSimulationDialog *ui;
    NoiseSimulation * m_NS;
    bool m_NSCreated;
    void showEvent(QShowEvent * sw);
    QXDirect *pXDirect;
};

#endif // NOISESIMULATIONDIALOG_H
