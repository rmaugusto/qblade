#include "WindFieldToolBar.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QMainWindow>

#include "../StoreAssociatedComboBox.h"
#include "WindFieldModule.h"
#include "WindField.h"
#include "../Misc/NumberEdit.h"
#include "../Store.h"



WindFieldToolBar::WindFieldToolBar(QMainWindow *parent, WindFieldModule *module)
{
	setObjectName("WindfieldToolbar");
	
	m_module = module;
	m_shownWindField = NULL;

    QRect rec = QApplication::desktop()->screenGeometry();
    int width = rec.width();
    setIconSize(QSize(width*0.025,width*0.025));
	
	{
		QGroupBox *groupBox = new QGroupBox (tr("Windfield"));
		addWidget(groupBox); {
			QVBoxLayout *vBox = new QVBoxLayout ();
			groupBox->setLayout(vBox);
			m_windFieldComboBox = new WindFieldComboBox (&g_windFieldStore);
			m_windFieldComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			m_windFieldComboBox->setMinimumWidth(170);
			vBox->addWidget(m_windFieldComboBox);
			connect (m_windFieldComboBox, SIGNAL(valueChanged(int)), m_module, SLOT(onShownWindFieldChanged(int)));
	} } {
		QGroupBox *groupBox = new QGroupBox (tr("Timestep"));
		addWidget(groupBox); {
			QHBoxLayout *hBox = new QHBoxLayout ();
			groupBox->setLayout(hBox); {
				m_timestepEdit = new NumberEdit (NumberEdit::Standard, 0);
//				m_timestepEdit->setFixedWidth(60);
                m_timestepEdit->setMinimum(1);
				m_timestepEdit->setValue(1);
                m_timestepEdit->
				connect (m_timestepEdit, SIGNAL(editingFinished()), this, SLOT(onTimestepEditEdited()));
				hBox->addWidget(m_timestepEdit);
				m_timestepSlider = new QSlider ();
				m_timestepSlider->setOrientation(Qt::Horizontal);
				m_timestepSlider->setMinimum(1);
				connect (m_timestepSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
				hBox->addWidget(m_timestepSlider);
	} } }
	
	useWindField(NULL);
	
	parent->addToolBar(this);
	hide();
}

void WindFieldToolBar::useWindField(WindField *newShownWindField) {
	m_shownWindField = newShownWindField;
	
	if (m_shownWindField) {
		int index = m_windFieldComboBox->findText(m_shownWindField->getName());
		m_windFieldComboBox->setCurrentIndex(index);
		m_timestepSlider->setValue(m_shownWindField->getShownTimestep()+1);  // index shift
		m_timestepSlider->setMaximum (m_shownWindField->getNumberOfTimesteps());
		m_timestepEdit->setMaximum(m_shownWindField->getNumberOfTimesteps());
	} else {
		m_timestepSlider->setValue(1);
		m_timestepSlider->setMaximum(1);
		m_timestepEdit->setMaximum(1);
	}
}

void WindFieldToolBar::onSliderChanged(int newValue) {
	WindField *windField = m_module->getShownWindField();
	if (windField) {
		windField->setShownTimestep(newValue - 1);  // Indexverschiebung
		m_module->reportGLChange();
	}
	m_timestepEdit->setValue(newValue);
}

void WindFieldToolBar::onTimestepEditEdited() {
	m_timestepSlider->setValue(m_timestepEdit->getValue());
}
