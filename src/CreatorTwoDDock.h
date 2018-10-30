#ifndef CREATORTWODDOCK_H
#define CREATORTWODDOCK_H

#include <QCheckBox>
#include <QLabel>

#include "CreatorDock.h"
#include "Misc/LineStyleButton.h"
#include "TwoDWidgetInterface.h"
#include "Noise/NoiseSimulation.h"


/* To access derived members within this class' functions, it is necessary to specify with the 'this' pointer.
 * This issue is the result of a template class inheriting a template class. More information at:
 * http://stackoverflow.com/questions/1120833/derived-template-class-access-to-base-class-member-data
 * */

template <class Object>
class CreatorTwoDDock : public CreatorDock<Object>
{
//	Q_OBJECT  // incompatible
		
public:
	CreatorTwoDDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, bool useSelectionDot = true);
	virtual ~CreatorTwoDDock() { }
	virtual void setShownObject (Object *newObject);	
	void adjustShowCheckBox();
	bool isSelectShown();
	
protected:
	virtual TwoDWidgetInterface* twoDInterface() = 0;
	virtual QList<NewGraph::GraphType> affectedGraphTypes() = 0;  // which graphs need to be rescaled after show/hide
	
	QVBoxLayout *m_visualizationVBox;
	QCheckBox *m_showCheckBox, *m_showCurveCheckBox, *m_showPointsCheckBox, *m_showSelectCheckBox;
	LineStyleButton *m_styleButton;	
	
//protected slots:
	void onShowChanged ();
	void onShowCurveChanged ();
	void onShowPointsChanged ();
	void onShowSelectChanged ();
	void onStyleChanged ();
};


template <class Object>
CreatorTwoDDock<Object>::CreatorTwoDDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags,
										 bool useSelectionDot)
	: CreatorDock<Object>(title, parent, flags)
{
	QGroupBox *groupBox = new QGroupBox ("Visualization");
	this->m_contentVBox->addWidget(groupBox);
		m_visualizationVBox = new QVBoxLayout;
		groupBox->setLayout(m_visualizationVBox);
			QHBoxLayout *hBox = new QHBoxLayout;
			m_visualizationVBox->addLayout(hBox);
				m_showCheckBox = new QCheckBox ("Show");
				this->connect(m_showCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowChanged()));
				hBox->addWidget(m_showCheckBox);
				m_showCurveCheckBox = new QCheckBox ("Curve");
				this->connect(m_showCurveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowCurveChanged()));
				hBox->addWidget(m_showCurveCheckBox);
				m_showPointsCheckBox = new QCheckBox ("Points");
				this->connect(m_showPointsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowPointsChanged()));
				hBox->addWidget(m_showPointsCheckBox);
				hBox->addStretch();
			if (useSelectionDot) {
				m_showSelectCheckBox = new QCheckBox ("Show Selection Dot");
				this->connect(m_showSelectCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onShowSelectChanged()));
				m_visualizationVBox->addWidget(m_showSelectCheckBox);
			}
			hBox = new QHBoxLayout ();
			m_visualizationVBox->addLayout(hBox);
				QLabel *label = new QLabel("Style:");
				hBox->addWidget(label);
				m_styleButton = new LineStyleButton ();
				this->connect(m_styleButton, SIGNAL(styleChanged()), this, SLOT(onStyleChanged()));
				hBox->addWidget(m_styleButton);
				hBox->addStretch();
}

template <class Object>
void CreatorTwoDDock<Object>::setShownObject(Object *newObject) {
	CreatorDock<Object>::setShownObject(newObject);
	
	m_styleButton->setEnabled(this->m_shownObject);
	m_showCheckBox->setEnabled(this->m_shownObject);
	m_showCurveCheckBox->setEnabled(this->m_shownObject);
	m_showPointsCheckBox->setEnabled(this->m_shownObject);
	if (this->m_shownObject) {
		m_styleButton->setPen(this->m_shownObject->getPen());
		m_showCheckBox->setChecked(this->m_shownObject->isShownInGraph());
		m_showCurveCheckBox->setChecked(this->m_shownObject->isDrawCurve());
		m_showPointsCheckBox->setChecked(this->m_shownObject->isDrawPoints());
	} else {
		m_styleButton->setPen(QPen());
		m_showCheckBox->setChecked(false);
		m_showCurveCheckBox->setChecked(false);
		m_showPointsCheckBox->setChecked(false);
	}
}

template <class Object>
void CreatorTwoDDock<Object>::adjustShowCheckBox() {
	m_showCheckBox->setChecked(this->m_shownObject && this->m_shownObject->isShownInGraph());
}

template <class Object>
bool CreatorTwoDDock<Object>::isSelectShown() {
	return m_showSelectCheckBox->isChecked();
}

template <class Object>
void CreatorTwoDDock<Object>::onShowChanged() {
	if (this->m_shownObject) {  // in case the boxes are changed programmatically when no simulation is shown
		this->m_shownObject->setShownInGraph(m_showCheckBox->isChecked());
//		if (m_showCheckBox->isChecked()) {  // hiding doesn't require a reload  // NM yes it does...
			// would be more performant to load only the newly shown curve but this is not performance critical
			foreach(NewGraph::GraphType type, affectedGraphTypes()) {
				twoDInterface()->reloadForGraphType(type);
			}
//		} else {
//			twoDInterface()->update();
//		}
	}
}

template <class Object>
void CreatorTwoDDock<Object>::onShowCurveChanged() {
	if (this->m_shownObject) {  // in case the boxes are changed programmatically when no simulation is shown
		this->m_shownObject->setDrawCurve(m_showCurveCheckBox->isChecked());
		twoDInterface()->update();
	}
}

template <class Object>
void CreatorTwoDDock<Object>::onShowPointsChanged() {
	if (this->m_shownObject) {  // in case the boxes are changed programmaticly when no simulation is shown
		this->m_shownObject->setDrawPoints(m_showPointsCheckBox->isChecked());
		twoDInterface()->update();
	}
}

template <class Object>
void CreatorTwoDDock<Object>::onShowSelectChanged() {
	twoDInterface()->update();
}

template <class Object>
void CreatorTwoDDock<Object>::onStyleChanged() {
	this->m_shownObject->setPen(m_styleButton->getPen());
	twoDInterface()->update();
}


#endif // CREATORTWODDOCK_H
