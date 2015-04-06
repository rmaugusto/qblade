#include "noisedock.h"

#include "noisesimulation.h"
#include "../GlobalFunctions.h"
#include "../Store.h"
#include "../StoreAssociatedComboBox.h"

#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>


NoiseDock::NoiseDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, NoiseModule *module) :
    ScrolledDock(title, parent, flags)
{

    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);
    setVisible(false);

    m_Module = module;




//    QGroupBox *groupBox = new QGroupBox ("LLT XX");
//    m_contentVBox->addWidget(groupBox);

//    QGridLayout *grid = new QGridLayout ();
//    groupBox->setLayout(grid);

//    QPushButton *m_startSimulationButton = new QPushButton (tr("Start Simulation"));
//    connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT (OnClickX()));
//    grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);



    QGroupBox *groupBox = new QGroupBox ("Trailing TE Noise");
    m_contentVBox->addWidget(groupBox);

    QGridLayout *grid = new QGridLayout ();
    groupBox->setLayout(grid);

    m_simulationsLabel = new QLabel (tr("Simulations:"));
    grid->addWidget (m_simulationsLabel, 0, 0,1,0);

    m_NoiseSimulationComboBox = new NoiseSimulationComboBox(&g_NoiseSimulationStore);
    m_NoiseSimulationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_NoiseSimulationComboBox->setMinimumWidth(170);
    connect (m_NoiseSimulationComboBox, SIGNAL(valueChanged(int)), m_Module, SLOT(OnSelChangeNoiseSimulation(int)));
    grid->addWidget (m_NoiseSimulationComboBox, 1, 0,1,0);



    m_newButton = new QPushButton (tr("New"));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
    grid->addWidget (m_newButton, 2, 0);

    m_renameButton = new QPushButton (tr("Rename"));
    connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
    grid->addWidget (m_renameButton, 2, 1);
    m_editCopyButton = new QPushButton (tr("Edit"));
    connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));
    grid->addWidget (m_editCopyButton, 3, 0);
    m_deleteButton = new QPushButton (tr("Delete"));
    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
    grid->addWidget (m_deleteButton, 3, 1);

    m_exportButton = new QPushButton (tr("Export"));
    connect(m_exportButton, SIGNAL(clicked()), this, SLOT(onExportButtonClicked()));
    grid->addWidget (m_exportButton, 4, 0,1,2);

    QLabel * labelPlotMessage = new QLabel("* Negative SPL contributions will not be plotted to graph");
    labelPlotMessage->setWordWrap(true);
    m_contentVBox->addWidget(labelPlotMessage);


//    groupBox = new QGroupBox ();
//    vBox->addWidget (groupBox);
//    grid = new QGridLayout ();
//    groupBox->setLayout(grid);

//    m_renameButton = new QPushButton (tr("Rename"));
//    connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
//    grid->addWidget (m_renameButton, 0, 0);

//    m_editCopyButton = new QPushButton (tr("Edit/Copy"));
//    connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));
//    grid->addWidget (m_editCopyButton, 0, 1);

//    m_deleteButton = new QPushButton (tr("Delete"));
//    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
//    grid->addWidget (m_deleteButton, 1, 0);

//    m_newButton = new QPushButton (tr("New"));
//    connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
//    grid->addWidget (m_newButton, 1, 1);






    addScrolledDock(Qt::LeftDockWidgetArea , parent);


}

void NoiseDock::initView(){

}

void NoiseDock::onDeleteButtonClicked()
{
    if(m_NoiseSimulationComboBox->currentIndex() != -1){

        QMessageBox::StandardButton response = QMessageBox::question (this, tr("Question"),
                                                                      QString (tr("Do you really want to delete this simulation?")),
                                                                      QMessageBox::Yes|QMessageBox::Cancel);
        if (response == QMessageBox::Yes) {
            g_NoiseSimulationStore.remove(m_NoiseSimulationComboBox->currentObject() );
        }
    }

}

void NoiseDock::onNewButtonClicked()
{
    NoiseSimulationDialog * dialog = new NoiseSimulationDialog();
    int result = dialog->exec();

    if(result == QDialog::Accepted){
        NoiseSimulation * ns  = dialog->GetNoiseSimulation();
        ns->simulate();
        m_Module->reloadAllGraphics();
        //g_mainFrame->getTwoDWidget()->update();
    }

    delete dialog;

}

void NoiseDock::onExportButtonClicked()
{

    if(m_NoiseSimulationComboBox->currentIndex() != -1){

        QString fileName;

        fileName = m_NoiseSimulationComboBox->currentObject()->getName();
        fileName.replace("/", " ");

        fileName = QFileDialog::getSaveFileName(this, tr("Export Noise"),
                            fileName+".txt",
                            tr("Noise prediction (*.txt)"));
        if(!fileName.length()) return;

        QFile qFile(fileName);

        if (!qFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

        QTextStream out(&qFile);

        m_NoiseSimulationComboBox->currentObject()->exportCalculation(out);

        qFile.close();


    }

}

void NoiseDock::onRenameButtonClicked()
{
    if(m_NoiseSimulationComboBox->currentIndex() != -1){
        g_NoiseSimulationStore.rename(m_NoiseSimulationComboBox->currentObject());
    }
}

void NoiseDock::onEditCopyButtonClicked()
{
    if(m_NoiseSimulationComboBox->currentIndex() != -1){

        NoiseSimulationDialog * dialog = new NoiseSimulationDialog(NULL,m_NoiseSimulationComboBox->currentObject());
        int result = dialog->exec();

        if(result == QDialog::Accepted){
            NoiseSimulation * ns  = dialog->GetNoiseSimulation();
            ns->simulate();
            m_Module->reloadAllGraphics();
        }

        delete dialog;

    }
}


NoiseDock::~NoiseDock(){

    delete m_newButton;
    delete m_editCopyButton;
    delete m_deleteButton;
    delete m_renameButton;
    delete m_exportButton;
    delete m_simulationsLabel;
    delete m_NoiseSimulationComboBox;

}
