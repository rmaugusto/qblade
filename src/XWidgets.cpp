#include "XWidgets.h"

#include "Store.h"
#include "StorableObject_heirs.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QDebug>


template <class T>
RenameDialog<T>::RenameDialog (T *objectToRename, Store<T> *associatedStore) {
	QVBoxLayout *vBox = new QVBoxLayout ();
	setLayout(vBox);
		QLabel *label = new QLabel (tr("The chosen name does already exist!<br>In order to save this object, "
									   "it needs an unique name.<br>Please enter a new name:"));
		vBox->addWidget(label);
		m_newNameEdit = new QLineEdit (objectToRename->getName());
		vBox->addWidget(m_newNameEdit);
		label = new QLabel (tr("Existing names:"));
		vBox->addWidget(label);
		m_namesListWidget = new QListWidget ();
		connect (m_namesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onSelectedRowChanges()));
		connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onSelectedRowChanges()));
		connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onOkButtonClicked()));
		vBox->addWidget(m_namesListWidget);
		QHBoxLayout *buttonHBox = new QHBoxLayout ();
		vBox->addLayout(buttonHBox);
			buttonHBox->addStretch();
			QPushButton *button = new QPushButton (tr("Don't save"));
			connect(button, SIGNAL(clicked()), this, SLOT(onDontSaveButtonClicked()));
			buttonHBox->addWidget(button);
			button = new QPushButton (tr("Ok"));
			button->setDefault(true);
			connect(button, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
			buttonHBox->addWidget(button);
			
	for (int i = 0; i < associatedStore->size(); ++i) {
		if (!associatedStore->isSameNameAllowed() || associatedStore->at(i)->getParent() == objectToRename->getParent()) {
			new QListWidgetItem (associatedStore->at(i)->getName(), m_namesListWidget);
		}
	}
	
	setWindowTitle("Rename Object");
	m_newNameEdit->selectAll();
}

template <class T>
void RenameDialog<T>::reject() {
	done (RenameDialog<T>::DontSave);
}

template <class T>
void RenameDialog<T>::onSelectedRowChanges() {
	m_newNameEdit->setText(m_namesListWidget->currentItem()->text());
	m_newNameEdit->selectAll();
}

template <class T>
void RenameDialog<T>::onOkButtonClicked() {
	if (m_newNameEdit->text() == "") {
		QMessageBox::warning(this, tr("Warning"), tr("A name must be chosen."));
		return;
	}
	
	/* check if chosen name already exists */
	bool nameAlreadyExists = false;
	int i = 0;
	while (m_namesListWidget->item(i) != 0) {
		if (m_namesListWidget->item(i)->text() == m_newNameEdit->text()) {
			nameAlreadyExists = true;
			break;
		}
		i++;
	}
	
	if (nameAlreadyExists) {
		QMessageBox::StandardButton response;
		response = QMessageBox::question (this, tr("Question"),
										  QString (tr("Do you wish to overwrite ") + m_newNameEdit->text() + " ?"),
										  QMessageBox::Yes|QMessageBox::Cancel);
		if (response == QMessageBox::Yes) {
			done (RenameDialog<T>::Overwrite);
		}
	} else {
		done (RenameDialog<T>::Ok);
	}
}

template <class T>
void RenameDialog<T>::onDontSaveButtonClicked() {
	done (RenameDialog<T>::DontSave);
}

/* to make it possible to separate the template header and implementation, all needed types have to be
 * instantiated here. For more information see:
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 * */
template class RenameDialog<WindField>;
template class RenameDialog<CBlade>;
template class RenameDialog<BladeStructure>;
template class RenameDialog<FASTSimulation>;
template class RenameDialog<CPolar>;
template class RenameDialog<CFoil>;
template class RenameDialog<C360Polar>;
template class RenameDialog<BEMData>;
template class RenameDialog<TBEMData>;
template class RenameDialog<CBEMData>;
template class RenameDialog<TData>;
template class RenameDialog<DMSData>;
template class RenameDialog<TDMSData>;
template class RenameDialog<CDMSData>;
template class RenameDialog<OpPoint>;
template class RenameDialog<BladeStructureLoading>;
template class RenameDialog<QLLTSimulation>;
template class RenameDialog<QLLTCutPlane>;
template class RenameDialog<NoiseSimulation>;
template class RenameDialog<Strut>;








ExportRotorDialog::ExportRotorDialog (double rho, double kinVisc) {
	QVBoxLayout *vBox = new QVBoxLayout ();
	setLayout(vBox);
		QGroupBox *groupBox = new QGroupBox (tr("WT_perf parameters"));
		vBox->addWidget(groupBox);
			QGridLayout *grid = new QGridLayout ();
			groupBox->setLayout(grid);
				m_tipLossCheckBox = new QCheckBox (tr("TipLoss"));
				m_tipLossCheckBox->setToolTip(tr("Use the Prandtl tip-loss model?"));
				grid->addWidget (m_tipLossCheckBox, 0, 0, 1, 2);
				m_hubLossCheckBox = new QCheckBox (tr("HubLoss"));
				m_hubLossCheckBox->setToolTip(tr("Use the Prandtl hub-loss model?"));
				grid->addWidget (m_hubLossCheckBox, 1, 0, 1, 2);
				QLabel *label = new QLabel (tr("Yaw:"));
				grid->addWidget (label, 2, 0);
				m_yawEdit = new NumberEdit ();
				m_yawEdit->setToolTip(tr("Yaw error [deg]"));
				grid->addWidget (m_yawEdit, 2, 1);
				label = new QLabel (tr("Rho:"));
				grid->addWidget (label, 3, 0);
				m_rhoEdit = new NumberEdit ();
				m_rhoEdit->setToolTip(tr("Air density [mass/volume]"));
				grid->addWidget (m_rhoEdit, 3, 1);
				label = new QLabel (tr("KinVisc:"));
				grid->addWidget (label, 4, 0);
				m_kinViscEdit = new NumberEdit ();
				m_kinViscEdit->setToolTip(tr("Kinematic air viscosity"));
				grid->addWidget (m_kinViscEdit, 4, 1);
		QHBoxLayout *buttonHBox = new QHBoxLayout ();
		vBox->addLayout(buttonHBox);
			QPushButton *button = new QPushButton (tr("Ok"));
			connect(button, SIGNAL(clicked()), this, SLOT(accept()));
			buttonHBox->addWidget(button);
			button = new QPushButton (tr("Cancel"));
			connect(button, SIGNAL(clicked()), this, SLOT(reject()));
			buttonHBox->addWidget(button);	
	m_rhoEdit->setValue(rho);
	m_kinViscEdit->setValue(kinVisc);
	m_yawEdit->setValue(0);
}

ConnectToAirfoilDialog::ConnectToAirfoilDialog (CPolar *polar) {
    m_Polar = polar;
    QVBoxLayout *vBox = new QVBoxLayout ();
    setLayout(vBox);
        QLabel *label = new QLabel(tr("Enter a name for the imported Polar:"));
        vBox->addWidget(label);
        m_newNameEdit = new QLineEdit ("");
        vBox->addWidget(m_newNameEdit);
        label = new QLabel(tr("Enter Reynolds Number"));
        m_ReynoldsEdit = new NumberEdit;
        m_ReynoldsEdit->setMinimum(0);
        m_ReynoldsEdit->setValue(1000000);
        vBox->addWidget(label);
        vBox->addWidget(m_ReynoldsEdit);
        label = new QLabel (tr("Select an Airfoil with which you want to connect the imported Polar"));
        vBox->addWidget(label);
        vBox->addWidget(label);
        label = new QLabel (tr("Existing Airfoils:"));
        vBox->addWidget(label);
        m_namesListWidget = new QListWidget ();
        connect (m_namesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onSelectedRowChanges()));
        connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onSelectedRowChanges()));
        connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onOkButtonClicked()));
        vBox->addWidget(m_namesListWidget);
        QHBoxLayout *buttonHBox = new QHBoxLayout ();
        vBox->addLayout(buttonHBox);
            buttonHBox->addStretch();
            QPushButton *button = new QPushButton (tr("Don't save"));
            connect(button, SIGNAL(clicked()), this, SLOT(onDontSaveButtonClicked()));
            buttonHBox->addWidget(button);
            button = new QPushButton (tr("Ok"));
            button->setDefault(true);
            connect(button, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
            buttonHBox->addWidget(button);

    for (int i = 0; i < g_foilStore.size(); ++i) {
            new QListWidgetItem (g_foilStore.at(i)->getName(), m_namesListWidget);
    }

    setWindowTitle("Import Polar");
    m_newNameEdit->selectAll();
}

void ConnectToAirfoilDialog::onSelectedRowChanges() {
    m_newNameEdit->setText(m_namesListWidget->currentItem()->text()+"_Imported_Polar");
    m_newNameEdit->selectAll();
}

void ConnectToAirfoilDialog::reject() {
    done (ConnectToAirfoilDialog::DontSave);
}

void ConnectToAirfoilDialog::onOkButtonClicked() {
    if (m_namesListWidget->currentRow()<0){
        QString strange = tr("An Airfoil to connect with has to be choosen!\n");
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    if (!m_newNameEdit->text().length()){
        QString strange = tr("The Polar needs to have a name!\n");
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    m_Polar->m_Reynolds = m_ReynoldsEdit->getValue();
    m_Polar->setName(m_newNameEdit->text());
    m_Polar->setSingleParent(g_mainFrame->GetFoil(m_namesListWidget->currentItem()->text()));
    done (ConnectToAirfoilDialog::Ok);
}

void ConnectToAirfoilDialog::onDontSaveButtonClicked() {
    done (ConnectToAirfoilDialog::DontSave);
}


ConnectToPolarDialog::ConnectToPolarDialog (C360Polar *polar) {
    m_Polar = polar;
    QVBoxLayout *vBox = new QVBoxLayout ();
    setLayout(vBox);
        QLabel *label = new QLabel(tr("Enter a name for the imported 360 Polar:"));
        vBox->addWidget(label);
        m_newNameEdit = new QLineEdit ("");
        vBox->addWidget(m_newNameEdit);
        label = new QLabel (tr("Select a Polar with which you want to connect the imported 360 Polar"));
        vBox->addWidget(label);
        vBox->addWidget(label);
        label = new QLabel (tr("Existing Polars:"));
        vBox->addWidget(label);
        m_namesListWidget = new QListWidget ();
        connect (m_namesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onSelectedRowChanges()));
        connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onSelectedRowChanges()));
        connect (m_namesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onOkButtonClicked()));
        vBox->addWidget(m_namesListWidget);
        QHBoxLayout *buttonHBox = new QHBoxLayout ();
        vBox->addLayout(buttonHBox);
            buttonHBox->addStretch();
            QPushButton *button = new QPushButton (tr("Don't save"));
            connect(button, SIGNAL(clicked()), this, SLOT(onDontSaveButtonClicked()));
            buttonHBox->addWidget(button);
            button = new QPushButton (tr("Ok"));
            button->setDefault(true);
            connect(button, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
            buttonHBox->addWidget(button);

    for (int i = 0; i < g_polarStore.size(); ++i) {
            new QListWidgetItem (g_polarStore.at(i)->getParent()->getName()+" - "+g_polarStore.at(i)->getName(), m_namesListWidget);
    }

    setWindowTitle("Import Polar");
    m_newNameEdit->selectAll();
}

void ConnectToPolarDialog::onSelectedRowChanges() {
    m_newNameEdit->setText(g_polarStore.at(m_namesListWidget->currentRow())->getParent()->getName()+"_Imported_360Polar");
    m_newNameEdit->selectAll();
}

void ConnectToPolarDialog::reject() {
    done (ConnectToAirfoilDialog::DontSave);
}

void ConnectToPolarDialog::onOkButtonClicked() {
    if (m_namesListWidget->currentRow()<0){
        QString strange = tr("A Polar to connect with has to be choosen!\n");
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    if (!m_newNameEdit->text().length()){
        QString strange = tr("The 360 Polar needs to have a name!\n");
        QMessageBox::warning(g_mainFrame, tr("Warning"), strange);
        return;
    }

    m_Polar->setName(m_newNameEdit->text());
    m_Polar->setSingleParent(g_polarStore.at(m_namesListWidget->currentRow()));
    m_Polar->m_airfoil = g_foilStore.getObjectByNameOnly(m_Polar->getParent()->getParent()->getName());
    m_Polar->reynolds = g_polarStore.at(m_namesListWidget->currentRow())->m_Reynolds;

    done (ConnectToAirfoilDialog::Ok);
}

void ConnectToPolarDialog::onDontSaveButtonClicked() {
    done (ConnectToAirfoilDialog::DontSave);
}

