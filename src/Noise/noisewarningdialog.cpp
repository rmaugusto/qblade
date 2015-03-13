#include "noisewarningdialog.h"
#include "ui_noisewarningdialog.h"

NoiseWarningDialog::NoiseWarningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoiseWarningDialog)
{
    ui->setupUi(this);
}

NoiseWarningDialog::~NoiseWarningDialog()
{
    delete ui;
}
