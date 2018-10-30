#include "FASTMenu.h"
#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>

#include "../MainFrame.h"
#include "FASTModule.h"
#include "FASTSimulation.h"


FASTMenu::FASTMenu(QMainWindow *parent, FASTModule *module)
    : QMenu (parent)
{
	setTitle (tr("FAST Simulation"));
    m_module = module;

    connect (this, SIGNAL(aboutToShow()), SLOT(onAboutToShow()));


    m_writeFASTFiles = new QAction(tr("Export the current FAST simulation setup files (no results)"), this);
    m_writeFASTFiles->setStatusTip(tr("Exports the FAST simulation"));
    connect(m_writeFASTFiles, SIGNAL(triggered()), this, SLOT(onWriteFASTFiles()));
    addAction(m_writeFASTFiles);
}

void FASTMenu::onWriteFASTFiles(){
    QString directoryName = QFileDialog::getExistingDirectory (g_mainFrame, tr("Choose directory for export"),
                                                                                g_mainFrame->m_LastDirName);
	if (!directoryName.isEmpty()) {
		QDir Dir = QDir(directoryName);
		m_module->getShownFASTSimulation()->writeAllFiles(Dir);
	}
}

void FASTMenu::onAboutToShow() {
    m_writeFASTFiles->setEnabled(m_module->getShownFASTSimulation());
}

