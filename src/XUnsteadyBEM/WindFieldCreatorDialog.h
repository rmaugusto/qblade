#ifndef WINDFIELDCREATORDIALOG_H
#define WINDFIELDCREATORDIALOG_H

#include <QThread>
class QProgressDialog;

#include "../CreatorDialog.h"
#include "../ParameterViewer.h"
#include "../ParameterKeys.h"
class WindField;
class WindFieldModule;


/* Worker Thread: The extra thread is used to keep the main thread with the GUI active while calculation. Otherwise
 * the cancel button on the progress dialog could not be used.
 * When create windfield button is clicked a WindFieldThread is constructed. The windfield is prepared with all needed
 * parameters and the threads run method starts the windfield calculation.
 * As soon as the worker thread signals finished(), the fully calculated windfield is passed to the WindFielModule
 * and the thread is marked for delition.
 * */

class WindFieldCreatorDialog : public CreatorDialog, public ParameterViewer<Parameter::Windfield>
{
	Q_OBJECT
	
private:
	class WindFieldThread : public QThread {
	public: WindField *windField;
	private: void run ();
	};
	
public:
	WindFieldCreatorDialog(WindField *windfield, WindFieldModule *module);
	
private slots:
	void onWindFieldProgress ();  // update the progress dialog
	void onWindFieldFinished ();  // stores and shows the WindField when it's calculation is finished
	void onProgressDialogCanceled ();  // cancel windfield calculation
	
	void onCreateButtonClicked ();
	void onUnitsChanged () { }  // no need for this
	void onShearLayerChanged (bool state);  // enables/disables the corresponding LineEdits
	
private:
	void initView ();
	
	WindFieldModule *m_module;
	WindField *m_editedWindfield;
	
	WindFieldThread *m_windFieldThread;
	QProgressDialog *m_progressDialog;  // showing the windfield calculations progress
	int m_progressStep;  // how often the updateProgress signal has been send already
	int m_progressStepShown;  // how often the updateProgress signal has been processed
	bool m_cancelCalculation;  // windfield checks out this bool whether it should cancel the calculation
};

#endif // WINDFIELDCREATORDIALOG_H
