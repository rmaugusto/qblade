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

    param->setDeltaSouce( ui->deltaSourceXFoil->isChecked() ? Noise::XFoilCalculation : Noise::OriginalBPM );
    param->setDirectivityGreekDeg( ui->textDirectivityGreek->text().toDouble() );
    param->setDirectivityPhiDeg( ui->textDirectivityPhi->text().toDouble() );

    param->setHighFreq( ui->checkHighFrequency->isChecked() );
    param->setLowFreq( ui->checkLowFrequency->isChecked() );

    param->setInterpolationLinear( ui->checkLinear->isChecked() );
    param->setInterpolationLagranges( ui->checkLagranges->isChecked() );
    param->setInterpolationNewtons( ui->checkNewtons->isChecked() );
    param->setInterpolationSpline( ui->checkSpline->isChecked() );

    if(ui->comboTransitionType->currentIndex() == 0){
        param->setTransition( Noise::FullyTurbulent );
    }else{
        param->setTransition( Noise::TransitionFlow );
    }

    param->setSeparatedFlow( ui->checkBoxSourceSPLa->isChecked() );
    param->setSuctionSide( ui->checkBoxSourceSPLs->isChecked() );
    param->setPressureSide( ui->checkBoxSourceSPLp->isChecked() );

    param->OpPoints().clear();

    if(param->DeltaSouce() ==  Noise::XFoilCalculation){
        for (int i = 0; i < ui->listOpPoints->count(); ++i) {
            //Recover NoiseOpPoint from checked list item
            QListWidgetItem * pLstItem = ui->listOpPoints->item(i);
            if(pLstItem->checkState() == Qt::Checked){                
                QVariant qv = pLstItem->data(Qt::UserRole);
                NoiseOpPoint componentNop = qv.value<NoiseOpPoint>();
                NoiseOpPoint * nop = new NoiseOpPoint(componentNop.getOpPoint());

                //Calculate all sides before use NOP
                nop->calculateXSides();

                param->OpPoints().push_back(nop);

//                ListItemNoiseOpPoint pLinopp = pQv.value<ListItemNoiseOpPoint>();
//                NoiseOpPoint * nop = new NoiseOpPoint(pLinopp.Reynolds(),pLinopp.Alpha());

//                //Add NoiseOpPoint to parameters
//                param->OpPoints().push_back(nop);
            }
        }
    }else{

        //When BPM add only one OpPoint always
        //Degree to radians
        //double aoa = ui->textAoa->text().toDouble()*(PI/180.);
        double aoa = ui->textAoa->text().toDouble();
        NoiseOpPoint * nop = new NoiseOpPoint(ui->textChordBasedReynold->text().toDouble(),aoa);
        param->OpPoints().push_back(nop);

    }

}

void NoiseSimulationDialog::on_deltaSourceXFoil_toggled(bool checked)
{
    if(checked){
        ui->widgetGridBPM->hide();
        ui->widgetGridXFoilCalc->show();
        ui->listOpPoints->setEnabled(true);
    }
}

void NoiseSimulationDialog::on_deltaSourceBPM_toggled(bool checked)
{

    if(checked){
        ui->widgetGridXFoilCalc->hide();
        ui->widgetGridBPM->show();
        ui->listOpPoints->setEnabled(false);
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

        //If it is a new simulation
        if(m_NSCreated){
            ui->textOriginalVelocity->setText("");
        }else{
            ui->textOriginalVelocity->setText(QString::number(param->OriginalVelocity()));
        }
        ui->textOriginalChordLength->setText(QString::number(param->OriginalChordLength()));
        ui->textOriginalMach->setText(QString::number(param->OriginalMach()));
        ui->textDStarChordStation->setText(QString::number(param->DStarChordStation()));
        ui->textDStarScalingFactor->setText(QString::number(param->DStarScalingFactor()));
        ui->textEddyConvectionMach->setText(QString::number(param->EddyConvectionMach()));

        if(param->DeltaSouce() == Noise::OriginalBPM){
            if(param->OpPoints().size() > 0){
                NoiseOpPoint * nop  = param->OpPoints().at(0);
                ui->textChordBasedReynold->setText(QString::number( nop->Reynolds() ));
                ui->textAoa->setText( QString::number( nop->AlphaDeg() ) );
            }
        }

        ui->textDirectivityGreek->setText(QString::number(param->DirectivityGreekDeg()));
        ui->textDirectivityPhi->setText(QString::number(param->DirectivityPhiDeg()));


        ui->checkHighFrequency->setChecked(param->HighFreq());
        ui->checkLowFrequency->setChecked(param->LowFreq());
        ui->checkLinear->setChecked(param->InterpolationLinear());
        ui->checkLagranges->setChecked(param->InterpolationLagranges());
        ui->checkNewtons->setChecked(param->InterpolationSpline());
        ui->checkSpline->setChecked(param->InterpolationSpline());
        ui->checkBoxSourceSPLa->setChecked(param->SeparatedFlow());
        ui->checkBoxSourceSPLs->setChecked(param->SuctionSide());
        ui->checkBoxSourceSPLp->setChecked(param->PressureSide());

        if(param->Transition() == Noise::FullyTurbulent){
            ui->comboTransitionType->setCurrentIndex(0);
        }else{
            ui->comboTransitionType->setCurrentIndex(1);
        }

        if( param->DeltaSouce() == Noise::XFoilCalculation ){
            ui->deltaSourceXFoil->setChecked(true);
        }else{
            ui->deltaSourceBPM->setChecked(true);
        }

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


        //XFoil analisis is mandatory
        if(g_oppointStore.size() == 0){
            QMessageBox::warning(NULL, tr("Not available"), "Load an airfoil and select TBL data source prior to running the Noise Module.");
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        }

        for (int k = 0; k < g_oppointStore.size(); ++k) {
            OpPoint * opPoint = g_oppointStore.at(k);

            if(opPoint && opPoint->getProc()){

                //ListItemNoiseOpPoint linop;
                //NoiseOpPoint nop(opPoint);
                NoiseOpPoint * nop = new NoiseOpPoint(opPoint);

//                linop.setAlphaDeg(opPoint->getAlpha());
//                linop.setReynolds(opPoint->getReynolds());
//                linop.setAirfoilName(opPoint->getParent()->getParent()->getName());
//                linop.setPolarName(opPoint->getParent()->getName());

                QString strItem = QString("%1°, %2, %3 ")
                                               .arg(nop->AlphaDeg())
                                               .arg(nop->PolarName())
                                               .arg(nop->AirfoilName());

                QListWidgetItem *item = new QListWidgetItem(NULL,QListWidgetItem::UserType);
                item->setText(strItem);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

                item->setCheckState(Qt::Unchecked );

                for (unsigned int x = 0; x < m_NS->Calculation()->NoiseParam()->OpPoints().size() ; ++x) {
                    NoiseOpPoint * npp = m_NS->Calculation()->NoiseParam()->OpPoints().at(x);
                    if(npp->AlphaDeg() == opPoint->getAlpha()){
                        item->setCheckState(Qt::Checked );
                    }
                }

                //QVariant      * qv = new QVariant(Qt::UserRole,nop);
                //item->setData(Qt::UserRole,*qv);
                QVariant qv;
                qv.setValue(*nop);
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

    if(ui->textSimulationName->text().isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("Input simulation name."));
        ui->textSimulationName->setFocus();
        return false;
    }

    //Validate if there is at least one checked OpPoint when XFoil
    //is selected
    if(ui->deltaSourceXFoil->isChecked()){
        bool checkedItemFound = false;
        for (int i = 0; i < ui->listOpPoints->count(); ++i) {

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


void NoiseSimulationDialog::loadLinearInterpolate()
{

    //TODO: O DSTR e o X devem ser baseados no alpha selecionado
/*
    double x[IVX][3];
    int nside1, nside2, ibl;

    for (int k = 0; k < g_oppointStore.size(); ++k) {
        OpPoint * opPoint = g_oppointStore.at(k);

        CreateXBL(opPoint,x, nside1, nside2);
        for (ibl=2; ibl<= nside1;ibl++)
        {

            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][1] = x[ibl][1];
            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][1] = x[ibl][1];
            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][1] = x[ibl][1];

            m_NS->Calculation()->NoiseParam()->DStars()[ibl][1] = opPoint->dstr[ibl][1];
            m_NS->Calculation()->NoiseParam()->DStars()[ibl][1] = opPoint->dstr[ibl][1];
            m_NS->Calculation()->NoiseParam()->DStars()[ibl][1] = opPoint->dstr[ibl][1];

        }
        for (ibl=2; ibl<= nside2;ibl++)
        {

            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][2] = x[ibl][2];
            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][2] = x[ibl][2];
            m_NS->Calculation()->NoiseParam()->ChordStations()[ibl][2] = x[ibl][2];

            m_NS->Calculation()->NoiseParam()->DStars()[ibl][2] = opPoint->dstr[ibl][2];
            m_NS->Calculation()->NoiseParam()->DStars()[ibl][2] = opPoint->dstr[ibl][2];
            m_NS->Calculation()->NoiseParam()->DStars()[ibl][2] = opPoint->dstr[ibl][2];
        }
    }

*/

}

void NoiseSimulationDialog::on_buttonOK_clicked()
{

    if(validateInputs()){

        //Update the values on NoiseParameter
        readWindowParams();

        //If it is a new simulation add to the store
        if(m_NSCreated){

            loadLinearInterpolate();
            g_NoiseSimulationStore.add( m_NS );
        }

        freeOnClose(false);
        accept();

    }
}

void NoiseSimulationDialog::on_buttonCancel_clicked()
{
    freeOnClose(true);

    reject();
}

void NoiseSimulationDialog::freeOnClose(bool freeNS)
{

    //If it is a new simulation free
    if(m_NSCreated && freeNS){
        delete m_NS;
    }

    for (int i = 0; i < ui->listOpPoints->count(); ++i) {
        //Recover NoiseOpPoint from checked list item
        QListWidgetItem * pLstItem = ui->listOpPoints->item(i);
        ui->listOpPoints->removeItemWidget(pLstItem);
        delete pLstItem;
    }
}

void NoiseSimulationDialog::closeEvent(QCloseEvent *event)
{

    freeOnClose(true);

    reject();

}



