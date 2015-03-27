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

NoiseSimulationDialog::NoiseSimulationDialog(QWidget *parent, NoiseSimulation *sim) :
    QDialog(parent),
    ui(new Ui::NoiseSimulationDialog)
{
    ui->setupUi(this);
    //connect (this, SIGNAL(moduleChanged()), this, SLOT(onModuleChanged()));

    m_NSCreated = false;
    m_NS = sim;

    //If it is a new simulation
    if(!m_NS){

        NoiseSimulation * nSim = new NoiseSimulation();
        NoiseCalculation * nCalc = new NoiseCalculation();
        NoiseParameter *nParam = new NoiseParameter();

        nCalc->setNoiseParam(nParam);
        nSim->setCalculation(nCalc);

        m_NS = nSim;
        m_NSCreated = true;

    }else{
        //Can not edit simulation name here
        ui->textSimulationName->setEnabled(false);
    }

    pXDirect = (QXDirect *) g_mainFrame->m_pXDirect;

    initComponents();
    readCalculationParams();

}

NoiseSimulationDialog::~NoiseSimulationDialog()
{
    delete ui;
}

void NoiseSimulationDialog::readWindowParams()
{
    NoiseParameter *param = m_NS->Calculation()->NoiseParam();

    m_NS->setName(ui->textSimulationName->text());

    param->setWettedLength( ui->textWettedLength->text().toDouble() );
    param->setDistanceObsever( ui->textDistanceObsever->text().toDouble() );
    param->setOriginalVelocity( ui->textOriginalVelocity->text().toDouble() );
    param->setOriginalChordLength( ui->textOriginalChordLength->text().toDouble() );
    param->setOriginalMach( ui->textOriginalMach->text().toDouble() );
    param->setDStarChordStation( ui->textDStarChordStation->text().toDouble() );
    param->setDStarScalingFactor( ui->textDStarScalingFactor->text().toDouble() );
    param->setEddyConvectionMach( ui->textEddyConvectionMach->text().toDouble() );

    //TODO: CARREGAR A LISTA DE OPPOINTS SELECIONADA
    //param->setChordBasedReynolds( ui->textChordBasedReynold->text().toDouble() );

    param->setDirectivityGreek( ui->textDirectivityGreek->text().toDouble() );
    param->setDirectivityPhi( ui->textDirectivityPhi->text().toDouble() );

    param->setHighFreq( ui->checkHighFrequency->isChecked() );
    param->setLowFreq( ui->checkLowFrequency->isChecked() );

    param->setInterpolationLinear( ui->checkLinear->isChecked() );
    param->setInterpolationLagranges( ui->checkLagranges->isChecked() );
    param->setInterpolationNewtons( ui->checkNewtons->isChecked() );
    param->setInterpolationSpline( ui->checkSpline->isChecked() );

    param->setSeparatedFlow( ui->checkBoxSourceSPLa->isChecked() );
    param->setSuctionSide( ui->checkBoxSourceSPLs->isChecked() );
    param->setPressureSide( ui->checkBoxSourceSPLs->isChecked() );

    param->OpPoints().clear();

    for (int i = 0; i < ui->listOpPoints->count(); ++i) {

        //TODO: ARRUMAR A OBTENCAO DO OPPOINT
        //Recover NoiseOpPoint from checked list item
        QListWidgetItem * pLstItem = ui->listOpPoints->item(i);
        QVariant pQv = pLstItem->data(Qt::UserRole);
        ListItemNoiseOpPoint pLinopp = pQv.value<ListItemNoiseOpPoint>();
        NoiseOpPoint * nop = new NoiseOpPoint(pLinopp.Reynolds(),pLinopp.Alpha(),pLinopp.DStar());

        //Add NoiseOpPoint to parameters
        param->OpPoints().push_back(nop);
    }

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

bool NoiseSimulationDialog::validateDoubleValue(QLineEdit *txt)
{
    bool doubleValidation = false;
    txt->text().toDouble(&doubleValidation);

    if(!doubleValidation){
        QMessageBox::warning(this, tr("Warning"), tr("Input a valid double value."));
        txt->setFocus();
    }

    return doubleValidation;
}

void NoiseSimulationDialog::readCalculationParams()
{
    if(m_NS->Calculation() && m_NS->Calculation()->NoiseParam()){
        NoiseParameter * param = m_NS->Calculation()->NoiseParam();

        ui->textWettedLength->setText(QString::number(param->WettedLength()));
        ui->textDistanceObsever->setText(QString::number(param->DistanceObsever()));
        ui->textOriginalVelocity->setText(QString::number(param->OriginalVelocity()));
        ui->textOriginalChordLength->setText(QString::number(param->OriginalChordLength()));
        ui->textOriginalMach->setText(QString::number(param->OriginalMach()));
        ui->textDStarChordStation->setText(QString::number(param->DStarChordStation()));
        ui->textDStarScalingFactor->setText(QString::number(param->DStarScalingFactor()));
        ui->textEddyConvectionMach->setText(QString::number(param->EddyConvectionMach()));

        //TODO: LER DO OPPOINT
        //ui->textChordBasedReynold->setText(QString::number(param->ChordBasedReynolds()));
        ui->textDirectivityGreek->setText(QString::number(param->DirectivityGreek()));
        ui->textDirectivityPhi->setText(QString::number(param->DirectivityPhi()));


        ui->checkHighFrequency->setChecked(param->HighFreq());
        ui->checkLowFrequency->setChecked(param->LowFreq());
        ui->checkLinear->setChecked(param->InterpolationLinear());
        ui->checkLagranges->setChecked(param->InterpolationLagranges());
        ui->checkNewtons->setChecked(param->InterpolationSpline());
        ui->checkSpline->setChecked(param->InterpolationSpline());
        ui->checkBoxSourceSPLa->setChecked(param->SeparatedFlow());
        ui->checkBoxSourceSPLs->setChecked(param->SuctionSide());
        ui->checkBoxSourceSPLs->setChecked(param->PressureSide());

        ui->textSimulationName->setText( m_NS->getName() );

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

                ListItemNoiseOpPoint linop;
                QVariant      qv;
                QString strItem = QString("Alpha = %1° (%2 rad)")
                                               .arg(pXFoil->alfa*180./PI,5,'f',2).arg(pXFoil->alfa,5,'f',5);

                QListWidgetItem *item = new QListWidgetItem(NULL,QListWidgetItem::UserType);
                item->setText(strItem);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

                item->setCheckState(Qt::Unchecked );

                for (unsigned int x = 0; x < m_NS->Calculation()->NoiseParam()->OpPoints().size() ; ++x) {
                    NoiseOpPoint * npp = m_NS->Calculation()->NoiseParam()->OpPoints().at(x);
                    if(npp->Alpha() == pXFoil->alfa){
                        item->setCheckState(Qt::Checked );
                    }
                }

                linop.setAlpha(pXFoil->alfa);
                linop.setDStar(pXFoil->reinf);
                qv.setValue(linop);
                item->setData(Qt::UserRole,qv);

                ui->listOpPoints->insertItem( ui->listOpPoints->count(),item);

            }

        }
    }
}

NoiseSimulation *NoiseSimulationDialog::GetNoiseSimulation()
{
    return m_NS;
}

void NoiseSimulationDialog::showEvent(QShowEvent *sw)
{
    loadComponents();

}

bool NoiseSimulationDialog::validateInputs()
{

    if(!validateDoubleValue(ui->textWettedLength)) return false;
    if(!validateDoubleValue(ui->textDistanceObsever)) return false;
    if(!validateDoubleValue(ui->textOriginalVelocity)) return false;
    if(!validateDoubleValue(ui->textOriginalChordLength)) return false;
    if(!validateDoubleValue(ui->textOriginalMach)) return false;
    if(!validateDoubleValue(ui->textDStarChordStation)) return false;
    if(!validateDoubleValue(ui->textDStarScalingFactor)) return false;
    if(!validateDoubleValue(ui->textEddyConvectionMach)) return false;

    if(ui->deltaSourceBPM->isChecked()){
        if(!validateDoubleValue(ui->textChordBasedReynold)) return false;
        if(!validateDoubleValue(ui->textAoa )) return false;
    }

    if(!validateDoubleValue(ui->textDirectivityGreek)) return false;
    if(!validateDoubleValue(ui->textDirectivityPhi)) return false;

    //Validate if there is at least one checked OpPoint when XFoil
    //is selected
    if(ui->deltaSourceXFoil->isChecked()){
        bool checkedItemFound = false;
        for (unsigned int i = 0; i < ui->listOpPoints->count(); ++i) {

            QListWidgetItem * pLstItem = ui->listOpPoints->item(i);
            if(pLstItem->checkState() == Qt::Checked){
                checkedItemFound = true;
                break;
            }
        }

        if(!checkedItemFound){
            QMessageBox::warning(this, tr("Warning"), tr("Select an OpPoint."));
            return false;
        }
    }

    return true;

}

void NoiseSimulationDialog::on_buttonOK_clicked()
{

    if(validateInputs()){

        //Update the values on NoiseParameter
        readWindowParams();

        //If it is a new simulation add to the store
        if(m_NSCreated){
            g_NoiseSimulationStore.add( m_NS );
        }

        accept();

    }
}

void NoiseSimulationDialog::on_buttonCancel_clicked()
{
    //If it is a new simulation free
    if(m_NSCreated){
        delete m_NS;
    }

    reject();
}

void NoiseSimulationDialog::closeEvent(QCloseEvent *event)
{
    //If it is a new simulation free
    if(m_NSCreated){
        delete m_NS;
    }

    reject();

}



