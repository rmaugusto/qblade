#include "NoiseCreatorDialog.h"

#include <QGroupBox>
#include <QScrollArea>
#include <QRadioButton>
#include <QButtonGroup>
#include <algorithm>
#include <QMessageBox>

#include "../ParameterGrid.h"
#include "NoiseSimulation.h"
#include "../Store.h"
#include "NoiseModule.h"
#include "../Objects/OpPoint.h"
#include "../Objects/Polar.h"
#include "../Objects/Foil.h"
#include "NoiseException.h"


typedef Parameter::NoiseSimulation P;

NoiseCreatorDialog::NoiseCreatorDialog(NoiseSimulation *presetSimulation, NoiseModule *module)
	: m_module(module),
	  m_editedSimulation(presetSimulation),
	  m_opPointViewWidget(NULL)
{
	setWindowTitle("2D Noise Simulation");	
	
	QTabWidget *tabWidget = new QTabWidget;
	m_contentVBox->insertWidget(0, tabWidget);
	
	QWidget *widget = new QWidget;
	tabWidget->addTab(widget, "Parameters");
		QHBoxLayout *hBox = new QHBoxLayout;
		widget->setLayout(hBox);
			QGroupBox *groupBox = new QGroupBox ("Simulation Parameters");
			hBox->addWidget(groupBox);
				ParameterGrid<P> *pGrid = new ParameterGrid<P>(this);
				groupBox->setLayout(pGrid);
					pGrid->addEdit(P::Name, LineEdit, new QLineEdit, "Name of Simulation:", "Noise Simulation");
					get<QLineEdit>(P::Name)->setMinimumWidth(150);
					pGrid->addEdit(P::WettedLength, NumberEditType, new NumberEdit(),
								  "Length of wetted Trailing-Edge (L) []:", 1, LENGTH);
					pGrid->addEdit(P::DistanceObsever, NumberEditType, new NumberEdit(),
								  "Distance from observer to TE (re) []:", 1.22, LENGTH);
					pGrid->addEdit(P::OriginalVelocity, NumberEditType, new NumberEdit(),
								  "Original flow velocity (U) []:", 13, SPEED);
					pGrid->addEdit(P::OriginalChordLength, NumberEditType, new NumberEdit(),
								  "Original airfoil Chord length (C) []:", 1, LENGTH);
					pGrid->addEdit(P::OriginalMach, NumberEditType, new NumberEdit(),
								  "Original flow Mach Number (M):", 0.21);
					pGrid->addEdit(P::DStarChordStation, NumberEditType, new NumberEdit(),
								  "D* at chord station:", 0.98);
					pGrid->addEdit(P::DStarScalingFactor, NumberEditType, new NumberEdit(),
								  "D* scaling factor:", 1);
					pGrid->addEdit(P::EddyConvectionMach, NumberEditType, new NumberEdit(),
								  "Eddy Convection Mach number []:", 0.8, PERCENT);
					pGrid->addEdit(P::DirectivityTheta, NumberEditType, new NumberEdit(),
								  "Directivity angle θe [deg]:", 90);
					pGrid->addEdit(P::DirectivityPhi, NumberEditType, new NumberEdit(),
								  "Directivity angle ψe [deg]:", 90);
					
			QVBoxLayout *vBox = new QVBoxLayout;
			hBox->addLayout(vBox);
				QLabel *imageLabel = new QLabel;
				imageLabel->setPixmap(QPixmap(":/images/noise_3d_plate.png"));
				vBox->addWidget(imageLabel, 0, Qt::AlignHCenter);
				groupBox = new QGroupBox ("TE noise source contributions");
				vBox->addWidget(groupBox);
					pGrid = new ParameterGrid<P>(this);
					groupBox->setLayout(pGrid);
						pGrid->addEdit(P::SeparatedFlow, CheckBox, new QCheckBox ("enable"),
									   "Separated flow on the suction side (high Reynolds flow):", true);
						pGrid->addEdit(P::SuctionSide, CheckBox, new QCheckBox ("enable"),
									   "Suction side of airfoil (attached flow):", true);
						pGrid->addEdit(P::PressureSide, CheckBox, new QCheckBox ("enable"),
									   "Pressure side of airfoil (attached flow):", true);
				vBox->addStretch();
	
	
	widget = new QWidget;
	tabWidget->addTab(widget, "Op. Points");
		hBox = new QHBoxLayout;
		widget->setLayout(hBox);
			groupBox = new QGroupBox ("Operational Points");
			groupBox->setMinimumHeight(300);
			hBox->addWidget(groupBox);
				QGridLayout *grid = new QGridLayout;
				grid->setColumnStretch(5, 1);
				groupBox->setLayout(grid);
					QLabel *label = new QLabel ("Select operational points from");
					grid->addWidget(label, 0, 0, 1, 1);
					m_selectFromButtons = new QButtonGroup;
					connect(m_selectFromButtons, SIGNAL(buttonClicked(int)), this, SLOT(onSelectButtonsClicked(int)));
					QRadioButton *radioButton = new QRadioButton ("this polar:");
					m_selectFromButtons->addButton(radioButton, NoiseParameter::OnePolar);
					grid->addWidget(radioButton, 0, 1, 1, 1);
					m_airfoilComboBox = new FoilComboBox (&g_foilStore);
					grid->addWidget(m_airfoilComboBox, 0, 2, 1, 1);
					m_polarComboBox = new PolarComboBox (&g_polarStore);
					m_polarComboBox->setParentBox(m_airfoilComboBox);
					connect(m_polarComboBox, SIGNAL(valueChanged(int)), this, SLOT(onPolarBoxChange()));
					grid->addWidget(m_polarComboBox, 0, 3, 1, 1);
					radioButton = new QRadioButton ("all polars");
					m_selectFromButtons->addButton(radioButton, NoiseParameter::MultiplePolars);
					grid->addWidget(radioButton, 1, 1, 1, 3);
					radioButton = new QRadioButton ("original BPM δ* correlations");
					m_selectFromButtons->addButton(radioButton, NoiseParameter::OriginalBpm);
					grid->addWidget(radioButton, 2, 1, 1, 3);
					
					m_opPointScrollArea = new QScrollArea;
					grid->addWidget(m_opPointScrollArea, 3, 0, 1, 4);
						// scroll area is filled in NoiseCreatorDialog::fillOpPointView
					
					m_originalBpmWidget = new QWidget;
					grid->addWidget(m_originalBpmWidget, 3, 0, 1, 4, Qt::AlignLeft | Qt::AlignTop);
						pGrid = new ParameterGrid<P>(this);
						m_originalBpmWidget->setLayout(pGrid);
							pGrid->addEdit(P::Aoa, NumberEditType, new NumberEdit, "AOA (α) [deg]:", 0);
							pGrid->addEdit(P::ChordBasedReynolds, NumberEditType, new NumberEdit,
										   "Chord based Reynolds number (Rc):", 100000);
							pGrid->addComboBox(P::Transition, "Type of Transition:", NoiseParameter::TransitionFlow,
											   QStringList()<<"Fully turbulent"<<"Transition flow");
						
	setUnitContainingLabels();
	initView();			
}

void NoiseCreatorDialog::initView() {
	loadObject(m_editedSimulation);
	
	if (m_editedSimulation) {
		m_selectFromButtons->button(m_editedSimulation->getSelectFrom())->setChecked(true);
		onSelectButtonsClicked(m_editedSimulation->getSelectFrom()); // needed because no signal if button is yet selected
		
		if (m_editedSimulation->getSelectFrom() == NoiseParameter::OnePolar) {
			if (!m_editedSimulation->getAnalyzedOpPoints().isEmpty()) {
				m_airfoilComboBox->setCurrentObject(
						static_cast<CFoil*>(m_editedSimulation->getAnalyzedOpPoints()[0]->getParent()->getParent()));
				m_polarComboBox->setCurrentObject(
						static_cast<CPolar*>(m_editedSimulation->getAnalyzedOpPoints()[0]->getParent()));
			}
		}
		
		QVector<OpPoint*> analyzedOpPoints = m_editedSimulation->getAnalyzedOpPoints();
		for (int i = 0; i < m_opPointRecords.size(); ++i) {
			if (analyzedOpPoints.contains(m_opPointRecords[i].opPoint)) {
				m_opPointRecords[i].checkBox->setChecked(true);
			}
		}
	} else {
		get<QLineEdit>(P::Name)->setText(g_noiseSimulationStore.getNextName("Noise Simulation"));		
		m_selectFromButtons->button(NoiseParameter::OnePolar)->setChecked(true);
		onSelectButtonsClicked(NoiseParameter::OnePolar);
	}
}

void NoiseCreatorDialog::prepareOpPointRecords(bool allPolars) {
	m_opPointRecords.clear();
	
	for (int i = 0; i < g_oppointStore.size(); ++i) {
		OpPoint *opPoint = g_oppointStore.at(i);
		if (allPolars || static_cast<CPolar*>(opPoint->getParent()) == m_polarComboBox->currentObject()) {
			m_opPointRecords.append(OpPointRecord(opPoint->getParent()->getParent()->getName(),
												  opPoint->getParent()->getName(),
												  opPoint->getName(),
												  opPoint,
												  opPoint->m_readyForNoise));
		}
	}
	
	std::sort(m_opPointRecords.begin(), m_opPointRecords.end(), OpPointRecord::sort);
}

void NoiseCreatorDialog::fillOpPointView() {
	if (m_opPointViewWidget) {
		m_opPointViewWidget->deleteLater();
		m_opPointViewWidget = NULL;
	}
	
	if (!m_opPointRecords.isEmpty()) {
		m_opPointViewWidget = new QWidget;
		m_opPointViewWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);	
		m_opPointScrollArea->setWidget(m_opPointViewWidget);
			QGridLayout *grid = new QGridLayout;
			grid->setHorizontalSpacing(20);
			grid->setSizeConstraint(QLayout::SetMinimumSize);
			m_opPointViewWidget->setLayout(grid);
				QPushButton *button = new QPushButton ("All");
				button->setMinimumWidth(QFontMetrics(QFont()).width("All") * 1.8);
				button->setCheckable(true);
				connect(button, &QPushButton::toggled, this, &NoiseCreatorDialog::onAllButtonToggled);
				grid->addWidget(button, 0, 0, 1, 1);
				QLabel *label = new QLabel("<center>Op. Point</center>");
				grid->addWidget(label, 0, 1, 1, 1);
				label = new QLabel("<center>Polar</center>");
				grid->addWidget(label, 0, 2, 1, 1);
				label = new QLabel("<center>Airfoil</center>");
				grid->addWidget(label, 0, 3, 1, 1);
				QFont font ("Monospace");
				for (int i = 0; i < m_opPointRecords.size(); ++i) {
					m_opPointRecords[i].checkBox = new QCheckBox;
					if (!m_opPointRecords[i].readyForNoise) {
						m_opPointRecords[i].checkBox->setEnabled(false);
						m_opPointRecords[i].checkBox->setToolTip("This operational point lacks some data needed for "
																 "noise simulations. Repeat the analysis in the Direct "
																 "Analysis module");
					}
					grid->addWidget(m_opPointRecords[i].checkBox, i+1, 0, 1, 1, Qt::AlignHCenter);
					label = new QLabel (QString("%1").arg(m_opPointRecords[i].name.toDouble(), 6, 'f', 2));
					label->setFont(font);
					grid->addWidget(label, i+1, 1);							
					label = new QLabel (m_opPointRecords[i].polar);
					label->setFont(font);
					grid->addWidget(label, i+1, 2);
					label = new QLabel (m_opPointRecords[i].airfoil);
					label->setFont(font);
					grid->addWidget(label, i+1, 3);
				}
	}
}

void NoiseCreatorDialog::onSelectButtonsClicked(int id) {
	m_airfoilComboBox->setEnabled(id == NoiseParameter::OnePolar);
	m_polarComboBox->setEnabled(id == NoiseParameter::OnePolar);
	m_opPointScrollArea->setHidden(id == NoiseParameter::OriginalBpm);
	m_originalBpmWidget->setVisible(id == NoiseParameter::OriginalBpm);
	
	prepareOpPointRecords(id == NoiseParameter::MultiplePolars);
	fillOpPointView();
}

void NoiseCreatorDialog::onPolarBoxChange() {
	prepareOpPointRecords(false);
	fillOpPointView();
}

void NoiseCreatorDialog::onAllButtonToggled(bool pressed) {
	for (const OpPointRecord &record : m_opPointRecords) {
		record.checkBox->setChecked(pressed);
	}
}

void NoiseCreatorDialog::onCreateButtonClicked() {
	/* check for parameter sanity */
	bool hasOpPoints = (m_selectFromButtons->checkedId() == NoiseParameter::OriginalBpm ? true : false);
	for (const OpPointRecord &record : m_opPointRecords) {
		if (record.checkBox->isChecked()) {
			hasOpPoints = true;
			break;
		}
	}
	if (!hasOpPoints) {
		QMessageBox::critical(this, "Create Noise Simulation",
							  "The following error(s) occured:\n\n - Simulation has no Op. Points", QMessageBox::Ok);
		return;
	}
	
	/* create new simlation */
	NoiseSimulation *newSimulation = new NoiseSimulation (this);
	newSimulation->setSelectFrom(static_cast<NoiseParameter::OpPointSource> (m_selectFromButtons->checkedId()));
	
	QList<OpPoint*> analyzedOpPoints;
	for (int i = 0; i < m_opPointRecords.size(); ++i) {
		if (m_opPointRecords[i].checkBox->isChecked()) {
			analyzedOpPoints.append(m_opPointRecords[i].opPoint);
		}
	}
	newSimulation->setAnalyzedOpPoints(analyzedOpPoints.toVector());
	
	try {
		newSimulation->simulate();
		
		if (g_noiseSimulationStore.add(newSimulation)) {
			m_module->setShownSimulation(newSimulation);
			accept();  // leave dialog only if adding was successful
		}
	} catch (NoiseException &e) {
		delete newSimulation;
		QMessageBox::critical(g_mainFrame, "Simulation Error", e.what());
	}
}
