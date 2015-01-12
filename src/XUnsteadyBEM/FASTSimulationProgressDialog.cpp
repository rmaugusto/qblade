#include "FASTSimulationProgressDialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QDir>
#include <QFile>


FASTSimulationProgressDialog::FASTSimulationProgressDialog()
{
	setMinimumWidth(750);
	setMinimumHeight(450);
	
	QVBoxLayout *vBox = new QVBoxLayout ();
	setLayout(vBox);
		m_textEdit = new QTextEdit ();
		m_textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
		m_textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);
		vBox->addWidget(m_textEdit);
		QHBoxLayout *hBox = new QHBoxLayout ();
		vBox->addLayout(hBox);
			hBox->addStretch();
			m_cancelButton = new QPushButton ("Cancel");
			connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
			hBox->addWidget(m_cancelButton);
			m_finishButton = new QPushButton ("Finished");
			m_finishButton->setDefault(true);
			m_finishButton->setEnabled(false);
			connect(m_finishButton, SIGNAL(clicked()), this, SLOT(onFinishedButtonClicked()));
			hBox->addWidget(m_finishButton);
}

void FASTSimulationProgressDialog::startProcess(QString FASTInputDirectory) {
	QFile FASTBinary (QString(QDir::currentPath() + QDir::separator() + "FAST_bin" + QDir::separator() + "FAST"));
	if (! FASTBinary.exists()) {
		throw QString("Can't find FAST binary: " + FASTBinary.fileName());
	}
	QFile FASTInputFile (QString(FASTInputDirectory + QDir::separator() + "fast.fst"));
	m_FASTProcess = new QProcess (this);
	connect(m_FASTProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onNewProcessOutput()));
	connect(m_FASTProcess, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    QStringList arg;
    arg.append(FASTInputFile.fileName());
    m_FASTProcess->start(QString(FASTBinary.fileName()),arg);
}

void FASTSimulationProgressDialog::onNewProcessOutput() {
	m_textEdit->moveCursor(QTextCursor::End);
	m_textEdit->insertPlainText(QString(m_FASTProcess->readAllStandardOutput()));
}

void FASTSimulationProgressDialog::onProcessFinished(int exitCode) {
	m_textEdit->moveCursor(QTextCursor::End);
	if (exitCode == 0) {  // assuming that FAST never returns 0 if there was an error...
		m_finishButton->setEnabled(true);
	}
}

void FASTSimulationProgressDialog::onFinishedButtonClicked() {
	accept();
}

void FASTSimulationProgressDialog::onCancelButtonClicked() {
	reject();
}
