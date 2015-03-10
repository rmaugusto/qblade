#include "noisesimulationdialog.h"
#include "ui_noisesimulationdialog.h"

#include "noisesimulation.h"
#include "../Store.h"

//#include "../Store_include.h"
//#include "../Store.h"
//#include "../Params.h"
//#include "noisesimulation.h"
//#include "../StorableObject.h"

//#include <QDialog>
//#include <QPushButton>
//#include <QLineEdit>
//#include "../XWidgets.h"
//#include "../Misc/NumberEdit.h"
//#include "noisemodule.h"
//#include <QCheckBox>
//#include "../StoreAssociatedComboBox_include.h"

//#include <QGridLayout>
//#include <QLabel>
//#include "../XGlobals.h"
//#include <QButtonGroup>
//#include <QTabWidget>
//#include <QVBoxLayout>
//#include "../StoreAssociatedComboBox.h"
//#include "../XUnsteadyBEM/WindField.h"
//#include <QGroupBox>
//#include <QRadioButton>
//#include <QScrollArea>
//#include "../XBEM/Blade.h"
//#include <QDebug>
//#include <QMessageBox>



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



}
