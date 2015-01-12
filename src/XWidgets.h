#ifndef XWIDGETS_H
#define XWIDGETS_H

#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>
#include "Misc/NumberEdit.h"
#include "Store.h"





class RenameDialogBase : public QDialog
{
	Q_OBJECT
	
protected slots:
	virtual void onSelectedRowChanges () = 0;
	virtual void onOkButtonClicked () = 0;
	virtual void onDontSaveButtonClicked () = 0;
};

template <class T>
class RenameDialog : public RenameDialogBase
{
public:
	RenameDialog (T *objectToRename, Store<T> *associatedStore);
	enum Response {Ok, DontSave, Overwrite};  // the possible responses of this dialog
	QString getNewName () { return m_newNameEdit->text(); }
	
private:
	QLineEdit *m_newNameEdit;
	QListWidget *m_namesListWidget;

	void reject();  // override the common method to set an adequat response value
	void onSelectedRowChanges ();
	void onOkButtonClicked ();
	void onDontSaveButtonClicked ();
};

class ConnectToAirfoilDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectToAirfoilDialog (CPolar *polar);
    enum Response {Ok, DontSave, Overwrite};  // the possible responses of this dialog
    QString getNewName () { return m_newNameEdit->text(); }
private:
    CPolar *m_Polar;
    QLineEdit *m_newNameEdit;
    NumberEdit *m_ReynoldsEdit;
    QListWidget *m_namesListWidget;

    void reject();  // override the common method to set an adequat response value

private slots:
    void onSelectedRowChanges ();
    void onOkButtonClicked ();
    void onDontSaveButtonClicked ();

};

class ExportRotorDialog : public QDialog
{
	Q_OBJECT
public:
	ExportRotorDialog (double rho, double kinVisc);

	bool getTipLoss () { return m_tipLossCheckBox->checkState(); }
	bool getHubLoss () { return m_hubLossCheckBox->checkState(); }
	double getYaw () { return m_yawEdit->getValue(); }
	double getRho () { return m_rhoEdit->getValue(); }
	double getKinVisc () { return m_kinViscEdit->getValue(); }
	
private:
	QCheckBox *m_tipLossCheckBox, *m_hubLossCheckBox;
	NumberEdit *m_yawEdit, *m_rhoEdit, *m_kinViscEdit;
};

class ConnectToPolarDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectToPolarDialog(C360Polar *polar);
    enum Response {Ok, DontSave, Overwrite};  // the possible responses of this dialog
private:
    C360Polar *m_Polar;
    QLineEdit *m_newNameEdit;
    QListWidget *m_namesListWidget;

    void reject();  // override the common method to set an adequat response value

private slots:
    void onSelectedRowChanges ();
    void onOkButtonClicked ();
    void onDontSaveButtonClicked ();
};

#endif // XWIDGETS_H
