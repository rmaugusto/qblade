/****************************************************************************

	WindFieldCreator Class
		Copyright (C) 2012 Nikolai Moesus

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef WINDFIELDCREATOR_H
#define WINDFIELDCREATOR_H

#include <QObject>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressDialog>
#include <QThread>
#include <QDebug>
#include <QStackedWidget>
#include "src/GLWidget.h"
#include "../XWidgets.h"
#include "../Misc/NumberEdit.h"
#include "../ScrolledDock.h"
#include "WindField.h"
class WindFieldCreatorDock;  // forward declaration

/* Worker Thread explanation  // TODO deprecated
 * The extra thread is used to keep the main thread with the GUI active while calculation. Otherwise the
 * cancel button on the progress dialog could not be used.
 * When create windfield button is clicked a WindFieldThread is constructed. All parameters for the
 * WindField are passed to the thread that constructs the WindField. The threads run method starts
 * the windfield calculation.
 * As soon as the last onUpdateProgress signal is processed, the progress dialog emits a finshed signal to
 * the creator dockWidget. There the fully calculated windfield is passed to the WindFielModule and the
 * thread is marked for delition. In its destructor it marks the progress dialog for delition.
 * */
class WindFieldCreatorDock : public ScrolledDock
{
	Q_OBJECT
	
private:
	class WindFieldThread : public QThread
	{
	public:
		WindField *windField;
		
	private:
		void run () { windField->calculateWindField(); }
	};
	
public:
	WindFieldCreatorDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags);
	
	void initView ();
	void useWindField (WindField *newShownWindField);

private:
	WindField *m_shownWindField;  // the currently shown windfield
	
	QStackedWidget *m_stackedWidget;  // to switch between view and create mode
	
	/* widgets for view layout */
	QLabel *m_nameLabel;
	QLabel *m_shearLayerLabel;
	QPushButton *m_newButton, *m_editCopyButton, *m_deleteButton;
	QLabel *m_windSpeedLabel, *m_rotorRadiusLabel, *m_hubHeightLabel;
	QLabel *m_roughnessLabel, *m_measurementHeightLabel;
	QLabel *m_turbulenceLabel, *m_simulationTimeLabel, *m_timestepsLabel, *m_pointsLabel,
			*m_highestFrequencyLabel, *m_lowestFrequencyLabel, *m_resolutionLabel;
	
	/* widgets for create layout */
	QLineEdit *m_nameEdit;  // defines the windfields name
	QCheckBox *m_shearLayerCheckBox;  // to (de)activate options
	QPushButton *m_createButton, *m_renameButton, *m_cancelButton;  // the button at the bottom
	NumberEdit *m_windSpeedEdit, *m_rotorRadiusEdit, *m_hubHeightEdit;
	NumberEdit *m_roughnessEdit, *m_measurementHeightEdit;
	NumberEdit *m_turbulenceEdit, *m_simulationTimeEdit, *m_timestepsEdit, *m_pointsEdit,
			*m_highestFrequencyEdit, *m_lowestFrequencyEdit, *m_resolutionEdit;
	
	WindFieldThread *m_windFieldThread;
	QProgressDialog *m_progressDialog;  // showing the windfield calculations progress
	int m_progressStep;  // how often the updateProgress signal has been send already
	int m_progressStepShown;  // how often the updateProgress signal has been processed
	bool m_cancelCalculation;  // windField looks at this bool while calculation to possibly cancel

public slots:
	void onWindFieldRenamed (QString oldName, QString newName);
	void onShearLayerCheckBoxChanged (int state);  // enables/disables the corresponding LineEdits
	void onTimeEditsChange ();  // the calculation of frequencies must be redone
	void onPointsEditChange ();  // show another spatial resolution
	void onNewButtonClicked ();
	void onEditCopyButtonClicked ();
	void onDeleteButtonClicked ();
	void onCreateButtonClicked ();  // starts WindField calculation in separete thread
	void onRenameButtonClicked ();
	void onCancelButtonClicked ();
	void onWindFieldProgress ();  // update the progress dialog
	void onWindFieldFinished ();  // shows the WindField when it's calculation is finished
	void onProgressDialogCanceled ();  // stops windfield calculation
	void onBadAlloc ();  // shows message Dialog and quits programm
};

#endif // WINDFIELDCREATOR_H
