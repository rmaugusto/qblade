#include "WindFieldMenu.h"

#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>

#include "../MainFrame.h"
#include "WindFieldModule.h"
#include "WindField.h"


WindFieldMenu::WindFieldMenu(QMainWindow *parent, WindFieldModule *module)
	: QMenu (parent)
{
	m_module = module;
	
	setTitle (tr("Windfield"));
	connect (this, SIGNAL(aboutToShow()), SLOT(onAboutToShow()));
	
	m_writeWindfieldBinaryFileAction = new QAction(tr("Export current Windfield as Binary File"), this);
	connect(m_writeWindfieldBinaryFileAction, SIGNAL(triggered()), this, SLOT(onWriteWindfieldBinaryFile()));
	addAction(m_writeWindfieldBinaryFileAction);
	m_writeWindfieldTxtFileAction = new QAction(tr("Export current Windfield as Text File"), this);
	connect(m_writeWindfieldTxtFileAction, SIGNAL(triggered()), this, SLOT(onWriteWindfieldTxtFile()));
	addAction(m_writeWindfieldTxtFileAction);
    m_importWindfieldBinaryFileAction = new QAction(tr("Import Binary Wind Field File"), this);
    connect(m_importWindfieldBinaryFileAction, SIGNAL(triggered()), this, SLOT(onImportWindfieldBinaryFile()));
    addAction(m_importWindfieldBinaryFileAction);
}

void WindFieldMenu::onAboutToShow() {
	const bool windFieldAvailable = (m_module->getShownWindField() != NULL);
	m_writeWindfieldBinaryFileAction->setEnabled(windFieldAvailable);
	m_writeWindfieldTxtFileAction->setEnabled(windFieldAvailable);
}

void WindFieldMenu::onWriteWindfieldBinaryFile() {
	QString fileName = m_module->getShownWindField()->getName() + ".bts";
	fileName.replace(' ', '_');
	fileName = QFileDialog::getSaveFileName(NULL, tr("Export Windfield"),
											g_mainFrame->m_ExportLastDirName + QDir::separator() + fileName,
											"Binary Windfield File (*.bts)");
	if (!fileName.endsWith(".bts")) {
		fileName.append(".bts");
	}
	
	QFile windfieldFile (fileName);
	g_mainFrame->m_ExportLastDirName = QFileInfo(windfieldFile).absolutePath();
	if (windfieldFile.open(QIODevice::WriteOnly)) {
		QDataStream fileStream (&windfieldFile);
		m_module->getShownWindField()->exportToBinary(fileStream);
	}
	windfieldFile.close();
}

void WindFieldMenu::onImportWindfieldBinaryFile() {
    m_module->OnImportBinaryWindField();
}

void WindFieldMenu::onWriteWindfieldTxtFile() {
	QString fileName = m_module->getShownWindField()->getName() + ".txt";
	fileName.replace(' ', '_');
	fileName = QFileDialog::getSaveFileName(NULL, tr("Export Windfield"),
											g_mainFrame->m_ExportLastDirName + QDir::separator() + fileName,
											"Text File (*.txt)");
	if (!fileName.endsWith(".txt")) {
		fileName.append(".txt");
	}
	
	QFile windfieldFile (fileName);
	g_mainFrame->m_ExportLastDirName = QFileInfo(windfieldFile).absolutePath();
	if (windfieldFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream fileStream (&windfieldFile);
		m_module->getShownWindField()->exportToTxt(fileStream);
	}
	windfieldFile.close();
}
