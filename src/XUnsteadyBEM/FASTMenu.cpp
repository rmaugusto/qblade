#include "FASTMenu.h"
#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>

#include "../MainFrame.h"
#include "FASTModule.h"
#include "FASTSimulation.h"


extern MainFrame *g_mainFrame;

FASTMenu::FASTMenu(QMainWindow *parent, FASTModule *module)
    : QMenu (parent)
{
    m_module = module;

    connect (this, SIGNAL(aboutToShow()), SLOT(onAboutToShow()));

    setTitle (tr("FAST Simulation"));


    m_writeFASTFiles = new QAction(tr("Export the current FAST simulation setup files (no results)"), this);
    m_writeFASTFiles->setStatusTip(tr("Exports the FAST simulation"));
    connect(m_writeFASTFiles, SIGNAL(triggered()), this, SLOT(onWriteFASTFiles()));
    addAction(m_writeFASTFiles);
}

void FASTMenu::onWriteFASTFiles(){

    if (!m_module->getShownFASTSimulation()) return;
    QString directoryName = QFileDialog::getExistingDirectory (g_mainFrame, tr("Choose directory for export"),
                                                                                g_mainFrame->m_LastDirName);
    if (directoryName.isEmpty()) return;
    QDir Dir = QDir(directoryName);
    FASTSimulation *pFASTSim = m_module->getShownFASTSimulation();
    pFASTSim->writeAllFiles(Dir);

}

void FASTMenu::onAboutToShow() {
    bool fastSimAvailable = (m_module->getShownFASTSimulation() != NULL);
    m_writeFASTFiles->setEnabled(fastSimAvailable);
}

