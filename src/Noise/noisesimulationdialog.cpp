#include "noisesimulationdialog.h"
#include "ui_noisesimulationdialog.h"

#include "noisesimulation.h"
#include "../Store.h"
#include "../MainFrame.h"

#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
#include <QListWidgetItem>
#include <QMessageBox>

NoiseSimulationDialog::NoiseSimulationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoiseSimulationDialog)
{
    ui->setupUi(this);
    //connect (this, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));
    //connect();

    pXDirect = (QXDirect *) g_mainFrame->m_pXDirect;

    initComponents();

}

NoiseSimulationDialog::~NoiseSimulationDialog()
{
    delete ui;
}

void NoiseSimulationDialog::readWindowParams(NoiseParameter *nParam)
{

    //TODO: VERIFICAR SE ESSE CAMPO É INPUT MESMO OU CALCULO
    //param->setReynoldsBasedDisplacement(  );

    param->setChordBasedReynolds( ui->textChordBasedReynold->text().toDouble() );
    param->setDistanceObsever( ui->textDistanceObserver->text().toDouble() );
    param->setOriginalChordLength( ui->textOriginalAirfoilChordLength->text().toDouble() );
    param->setDStarChordStation( ui->textDChordStation->text().toDouble() );
    param->setDStarScalingFactor( ui->textDScalingFactor->text().toDouble() );
    param->setOriginalMach( ui->textOriginalMachFlow->text().toDouble() );
    param->setOriginalVelocity( ui->textOriginalFlowVelocity->text().toDouble() );
    param->setEddyConvectionMach( ui->textEddyConvectionMach->text().toDouble() );
    param->setWettedLength( ui->textLengthWetted->text().toDouble() );

    param->setDirectivityGreek( ui->textDirectivityGreek->text().toDouble() );
    param->setDirectivityPhi( ui->textDirectivityPhi->text().toDouble() );
    param->setHighFreq( ui->checkHighFrequency->isChecked() );
    param->setLowFreq( ui->checkLowFrequency->isChecked() );
    nParam->setInterpolationLinear( ui->checkLinear->isChecked() );
    nParam->setInterpolationLagranges( ui->checkLagranges->isChecked() );
    nParam->setInterpolationNewtons( ui->checkNewtons->isChecked() );
    nParam->setInterpolationSpline( ui->checkSpline->isChecked() );

    param->setSeparatedFlow( ui->checkBoxSourceSPLa->isChecked() );
    param->setSuctionSide( ui->checkBoxSourceSPLs->isChecked() );
    param->setPressureSide( ui->checkBoxSourceSPLs->isChecked() );

}

void NoiseSimulationDialog::on_NoiseSimulationDialog_accepted()
{

    NoiseSimulation * nSim = new NoiseSimulation();
    NoiseCalculation * nCalc = new NoiseCalculation();
    NoiseParameter *nParam = new NoiseParameter();

    readWindowParams(nParam);

    nCalc->setNoiseParam(nParam);
    nSim->setNoiseCalculation(nCalc);
    nSim->setName(ui->textSimulationName->text());
    g_NoiseSimulationStore.add(nSim);

}

void NoiseSimulationDialog::on_deltaSourceXFoil_toggled(bool checked)
{
    if(checked){
        ui->widgetGridBPM->hide();
        ui->widgetGridXFoilCalc->show();
    }
}

void NoiseSimulationDialog::on_deltaSourceBPM_toggled(bool checked)
{

    if(checked){
        ui->widgetGridXFoilCalc->hide();
        ui->widgetGridBPM->show();
    }

}

void NoiseSimulationDialog::initComponents()
{
    ui->widgetGridBPM->hide();


    //Resize window and center on the screen
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->resize(740,595);
    this->adjustSize();


    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        ));
}

void NoiseSimulationDialog::loadComponents()
{

    //Get all Operational Points and add to the list
    if(pXDirect){

        std::list<XFoil *> lstOPs = pXDirect->GetXFoilPoints();
        std::list<XFoil *>::const_iterator opIterator;

        //XFoil analisis is mandatory
        if(lstOPs.size() == 0){
            QMessageBox::warning(NULL, tr("Not available"), "Analize XFoil before !");
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        }

        for(opIterator=lstOPs.begin(); opIterator!=lstOPs.end(); opIterator++)
        {

            XFoil * pXFoil = *opIterator;

            if(pXFoil){

                QString strItem = QString("Alpha = %1\370 (%2 rad)")
                                               .arg(pXFoil->alfa*180./PI,5,'f',2).arg(pXFoil->alfa,5,'f',5);

               QListWidgetItem *item = new QListWidgetItem(NULL,QListWidgetItem::UserType);
                item->setText(strItem);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                item->setCheckState(Qt::Unchecked );
                ui->listOpPoints->insertItem( ui->listOpPoints->count(),item);

            }

        }
    }
}

void NoiseSimulationDialog::showEvent(QShowEvent *sw)
{
    loadComponents();

}
