#ifndef FASTSIMULATIONPROGRESSDIALOG_H
#define FASTSIMULATIONPROGRESSDIALOG_H

#include <QDialog>
class QTextEdit;
class QPushButton;
class QProcess;


class FASTSimulationProgressDialog : public QDialog
{
	Q_OBJECT
	
public:
	FASTSimulationProgressDialog();
	void startProcess (QString FASTInputDirectory);  // throws QString on error
	
private slots:
	void onFinishedButtonClicked ();
	void onCancelButtonClicked ();
	void onNewProcessOutput ();
	void onProcessFinished (int exitCode);
	
private:
	QProcess *m_FASTProcess;
	QTextEdit *m_textEdit;
	QPushButton *m_finishButton, *m_cancelButton;
};

#endif // FASTSIMULATIONPROGRESSDIALOG_H
