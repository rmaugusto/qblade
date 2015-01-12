#include "QLLTDock.h"
#include "QLLTModule.h"
#include "QLLTCreatorDialog.h"
#include <QMessageBox>

QLLTDock::QLLTDock(const QString & title, QMainWindow * parent, Qt::WindowFlags flags, QLLTModule *module)
    : ScrolledDock (title, parent, flags) {

    setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea);
    setVisible(false);

    m_module = module;
    m_shownLLTSimulation = NULL;


    QGroupBox *groupBox = new QGroupBox ("LLT Controls");
    m_contentVBox->addWidget(groupBox);
        QGridLayout *grid = new QGridLayout ();
        groupBox->setLayout(grid);
            m_startSimulationButton = new QPushButton (tr("Start Simulation"));
            connect(m_startSimulationButton, SIGNAL(clicked()), this, SLOT(onStartSimulationButtonClicked()));
            grid->addWidget(m_startSimulationButton, 0, 0, 1, 2);
            m_renameButton = new QPushButton (tr("Rename"));
            connect(m_renameButton, SIGNAL(clicked()), this, SLOT(onRenameButtonClicked()));
            grid->addWidget (m_renameButton, 1, 0);
            m_editCopyButton = new QPushButton (tr("Edit/Copy"));
            connect(m_editCopyButton, SIGNAL(clicked()), this, SLOT(onEditCopyButtonClicked()));
            grid->addWidget (m_editCopyButton, 1, 1);
            m_deleteButton = new QPushButton (tr("Delete"));
            connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
            grid->addWidget (m_deleteButton, 2, 0);
            m_newButton = new QPushButton (tr("New"));
            connect(m_newButton, SIGNAL(clicked()), this, SLOT(onNewButtonClicked()));
            grid->addWidget (m_newButton, 2, 1);


    addScrolledDock(Qt::LeftDockWidgetArea , parent);

    setShownLLTSimulation(NULL);




}

void QLLTDock::initView(){

}

void QLLTDock::onStartSimulationButtonClicked(){

    if (!m_module->GetQLLTSim()) return;
    QThread *thread = new QThread();
    m_module->GetQLLTSim()->moveToThread(thread);
    connect(thread, SIGNAL(started()), m_module->GetQLLTSim(), SLOT(OnStartAnalysis()));
    thread->start();
}

void QLLTDock::onNewButtonClicked() {
    /* check first, if all requirements are fullfilled */
    QString errorMessage ("");
    if (g_rotorStore.size() == 0) {
        errorMessage.append(tr("\n - No rotor in database"));
    }
    if (g_windFieldStore.size() == 0) {
        errorMessage.append(tr("\n - No windfield in database"));
    }
    if (errorMessage != "") {
        QMessageBox::critical(this, tr("Create New Simulation"), QString(tr("The following error(s) occured:\n") + errorMessage), QMessageBox::Ok);
        return;
    }

    QLLTCreatorDialog *creatorDialog = new QLLTCreatorDialog (NULL, m_module);
    creatorDialog->exec();
    delete creatorDialog;
}

void QLLTDock::setShownLLTSimulation(QLLTSimulation *newSimulation) {
    m_shownLLTSimulation = newSimulation;

    /* set all labels to correct values */
    if (m_shownLLTSimulation) {
        m_startSimulationButton->setEnabled(!m_shownLLTSimulation->hasLLTResults());
    } else {
        m_startSimulationButton->setEnabled(false);

    }
    m_deleteButton->setEnabled(m_shownLLTSimulation);
    m_editCopyButton->setEnabled(m_shownLLTSimulation);
    m_renameButton->setEnabled(m_shownLLTSimulation);
}

void QLLTDock::onRenameButtonClicked() {
    g_QLLTHAWTSimulationStore.rename(m_shownLLTSimulation);
}

void QLLTDock::onEditCopyButtonClicked() {
    QLLTCreatorDialog *creatorDialog = new QLLTCreatorDialog (m_shownLLTSimulation, m_module);
    creatorDialog->exec();
    delete creatorDialog;
}

void QLLTDock::onDeleteButtonClicked() {
    g_QLLTHAWTSimulationStore.remove(m_shownLLTSimulation);
}

