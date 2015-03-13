#ifndef NOISEWARNINGDIALOG_H
#define NOISEWARNINGDIALOG_H

#include <QDialog>

namespace Ui {
class NoiseWarningDialog;
}

class NoiseWarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseWarningDialog(QWidget *parent = 0);
    ~NoiseWarningDialog();

private:
    Ui::NoiseWarningDialog *ui;
};

#endif // NOISEWARNINGDIALOG_H
