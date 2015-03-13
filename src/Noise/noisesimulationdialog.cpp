#include "noisesimulationdialog.h"
#include "ui_noisesimulationdialog.h"

#include "noisesimulation.h"
#include "../Store.h"


NoiseSimulationDialog::NoiseSimulationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoiseSimulationDialog)
{
    ui->setupUi(this);
    //connect (this, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));
    //connect();
}

NoiseSimulationDialog::~NoiseSimulationDialog()
{
    delete ui;
}

void NoiseSimulationDialog::on_NoiseSimulationDialog_accepted()
{


    NoiseSimulation * ns = new NoiseSimulation();
    g_NoiseSimulationStore.add(ns);


}
