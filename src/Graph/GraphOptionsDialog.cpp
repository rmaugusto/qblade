#include "GraphOptionsDialog.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QColorDialog>
#include <QCheckBox>
#include <QFontDialog>

#include "NewGraph.h"
#include "ShowAsGraphInterface.h"
#include "../MainFrame.h"
#include "../TwoDWidget.h"
#include "../Misc/NumberEdit.h"
#include "../Misc/LineButton.h"
#include "../Misc/LinePickerDlg.h"
#include "../Misc/ColorButton.h"


GraphOptionsDialog::GraphOptionsDialog(NewGraph *graph) {
	m_graph = graph;
	
	setWindowTitle(tr("Graph Options"));
	
	QVBoxLayout *dialogVBox = new QVBoxLayout ();
	setLayout(dialogVBox);
		QTabWidget *tabWidget = new QTabWidget ();
		dialogVBox->addWidget(tabWidget);
		QHBoxLayout *hBox = new QHBoxLayout ();
		dialogVBox->addLayout(hBox);
			hBox->addStretch();
			QPushButton *button = new QPushButton (tr("Restore default Style"));
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onRestoreButtonClicked()));
			hBox->addWidget (button);
			button = new QPushButton (tr("Apply"));
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onApplyButtonClicked()));
			hBox->addWidget (button);
			button = new QPushButton (tr("Cancel"));
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));
			hBox->addWidget (button);
			button = new QPushButton (tr("Ok"));
			button->setDefault(true);
			connect(button, SIGNAL(clicked(bool)), this, SLOT(onOkButtonClicked()));
			hBox->addWidget (button);
			
	/* the variables tab */
	QWidget *widget = new QWidget ();
	tabWidget->addTab(widget, "Variables");
		QGridLayout *grid = new QGridLayout ();
		widget->setLayout(grid);
			hBox = new QHBoxLayout ();
			grid->addLayout(hBox, 0, 0, 1, 2);
				QLabel *label = new QLabel (tr("Title of graph: "));
				hBox->addWidget(label);
				m_graphTitleEdit = new QLineEdit ();
				hBox-> addWidget(m_graphTitleEdit);
			label = new QLabel (tr("y-Axis"));
			grid->addWidget(label, 1, 0, 1, 1, Qt::AlignHCenter);
			label = new QLabel (tr("x-Axis"));
			grid->addWidget(label, 1, 1, 1, 1, Qt::AlignHCenter);
			m_yVariableList = new QListWidget ();
			connect(m_yVariableList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onOkButtonClicked()));
			grid->addWidget(m_yVariableList, 2, 0);
			m_xVariableList = new QListWidget ();
			connect(m_xVariableList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onOkButtonClicked()));
			grid->addWidget(m_xVariableList, 2, 1);
			
	/* the styles and axes tab */
	widget = new QWidget ();
	tabWidget->addTab(widget, "Styles and Axes");
		hBox = new QHBoxLayout ();
		widget->setLayout(hBox);
			QGroupBox *groupBox = new QGroupBox (tr("Axes"));
			groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			hBox->addWidget(groupBox);
				grid = new QGridLayout ();
				groupBox->setLayout(grid);
					int gridRowCount = 0;
					grid->setVerticalSpacing(10);
					grid->setHorizontalSpacing(10);
					label = new QLabel(tr("y-Axis"));
					grid->addWidget(label, gridRowCount, 1, 1, 1, Qt::AlignHCenter);
					label = new QLabel(tr("x-Axis"));
					grid->addWidget(label, gridRowCount++, 2, 1, 1, Qt::AlignHCenter);
					label = new QLabel (tr("Automatic: "));
					grid->addWidget(label, gridRowCount, 0);
					QHBoxLayout *autoHBox = new QHBoxLayout ();
					grid->addLayout(autoHBox, gridRowCount, 1);
						m_yLimitsManuelCheckBox = new QCheckBox (tr("limits"));
						autoHBox->addWidget(m_yLimitsManuelCheckBox);
						m_yTickManuelCheckBox = new QCheckBox (tr("tick"));
						autoHBox->addWidget(m_yTickManuelCheckBox);
					autoHBox = new QHBoxLayout ();
					grid->addLayout(autoHBox, gridRowCount++, 2);
						m_xLimitsManuelCheckBox = new QCheckBox (tr("limits"));
						autoHBox->addWidget(m_xLimitsManuelCheckBox);
						m_xTickManuelCheckBox = new QCheckBox (tr("tick"));
						autoHBox->addWidget(m_xTickManuelCheckBox);
					label = new QLabel (tr("Low limit: "));
					grid->addWidget(label, gridRowCount, 0);
					m_yLowLimitEdit = new NumberEdit (NumberEdit::Standard, 6);
					connect(m_yLowLimitEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_yLowLimitEdit, gridRowCount, 1);
					m_xLowLimitEdit = new NumberEdit (NumberEdit::Standard, 6);
					connect(m_xLowLimitEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_xLowLimitEdit, gridRowCount++, 2);
					label = new QLabel (tr("High limit: "));
					grid->addWidget(label, gridRowCount, 0);
					m_yHighLimitEdit = new NumberEdit (NumberEdit::Standard);
					connect(m_yHighLimitEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_yHighLimitEdit, gridRowCount, 1);
					m_xHighLimitEdit = new NumberEdit (NumberEdit::Standard);
					connect(m_xHighLimitEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_xHighLimitEdit, gridRowCount++, 2);
					label = new QLabel (tr("Tick size: "));
					grid->addWidget(label, gridRowCount, 0);
					m_yTickSizeEdit = new NumberEdit (NumberEdit::Scientific, 2, 0);
					connect(m_yTickSizeEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_yTickSizeEdit, gridRowCount, 1);
					m_xTickSizeEdit = new NumberEdit (NumberEdit::Scientific, 2, 0);
					connect(m_xTickSizeEdit, SIGNAL(valueChanged(double)), this, SLOT(onAxisValueChanged()));
					grid->addWidget(m_xTickSizeEdit, gridRowCount++, 2);
					label = new QLabel (tr("Grid style: "));
					grid->addWidget(label, gridRowCount, 0);
					m_yGridStyleButton = new LineButton ();
					connect(m_yGridStyleButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
					grid->addWidget(m_yGridStyleButton, gridRowCount, 1);
					m_xGridStyleButton = new LineButton ();
					connect(m_xGridStyleButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
					grid->addWidget(m_xGridStyleButton, gridRowCount++, 2);
					label = new QLabel (tr("Axis style: "));
					grid->addWidget(label, gridRowCount, 0);
					m_mainAxisStyleButton = new LineButton ();
					connect(m_mainAxisStyleButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
					grid->addWidget(m_mainAxisStyleButton, gridRowCount++, 1, 1, 2);
					setTabOrder(m_yLimitsManuelCheckBox, m_yLowLimitEdit);
					setTabOrder(m_yLowLimitEdit, m_yHighLimitEdit);
					setTabOrder(m_yHighLimitEdit, m_yTickSizeEdit);
					setTabOrder(m_yTickSizeEdit, m_xLimitsManuelCheckBox);
					setTabOrder(m_xLimitsManuelCheckBox, m_xLowLimitEdit);
					setTabOrder(m_xLowLimitEdit, m_xHighLimitEdit);
					setTabOrder(m_xHighLimitEdit, m_xTickSizeEdit);
			groupBox = new QGroupBox (tr("Styles"));
			groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			hBox->addWidget(groupBox);
				grid = new QGridLayout ();
				grid->setVerticalSpacing(10);
				grid->setHorizontalSpacing(10);
//				grid->setColumnMinimumWidth(1, 100);
				groupBox->setLayout(grid);
					gridRowCount = 0;
					label = new QLabel (tr("Border style: "));
					grid->addWidget(label, gridRowCount, 0);
					m_borderStyleButton = new LineButton ();
					connect(m_borderStyleButton, SIGNAL(clicked()), this, SLOT(onLineButtonClicked()));
					grid->addWidget(m_borderStyleButton, gridRowCount++, 1);
					label = new QLabel (tr("Background: "));
					grid->addWidget(label, gridRowCount, 0);
					m_backgroundColorButton = new ColorButton ();
					connect(m_backgroundColorButton, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
					grid->addWidget(m_backgroundColorButton, gridRowCount++, 1);
					label = new QLabel (tr("Title font: "));
					grid->addWidget(label, gridRowCount, 0);
					m_titleFontButton = new QPushButton ();
					m_titleFontButton->setFixedWidth (120);
					connect(m_titleFontButton, SIGNAL(clicked()), this, SLOT(onFontButtonClicked()));
					grid->addWidget(m_titleFontButton, gridRowCount++, 1);
					label = new QLabel (tr("Title color: "));
					grid->addWidget(label, gridRowCount, 0);
					m_titleColorButton = new ColorButton ();
					connect(m_titleColorButton, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
					grid->addWidget(m_titleColorButton, gridRowCount++, 1);
					label = new QLabel (tr("Label font: "));
					grid->addWidget(label, gridRowCount, 0);
					m_tickFontButton = new QPushButton ();
					m_tickFontButton->setFixedWidth (120);
					connect(m_tickFontButton, SIGNAL(clicked()), this, SLOT(onFontButtonClicked()));
					grid->addWidget(m_tickFontButton, gridRowCount++, 1);
					label = new QLabel (tr("Label color: "));
					grid->addWidget(label, gridRowCount, 0);
					m_tickColorButton = new ColorButton ();
					connect(m_tickColorButton, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));
					grid->addWidget(m_tickColorButton, gridRowCount++, 1);
			hBox->addStretch();
		
	initView();
}

void GraphOptionsDialog::initView(bool asDefault) {
	NewGraph *graphToLoad;
	if (asDefault) {
		graphToLoad = new NewGraph("__default", NewGraph::FastSimulation, NULL);
	} else {
		graphToLoad = m_graph;
	}
	
	m_backgroundColorButton->SetColor(graphToLoad->getBackgroundColor());
	m_titleColorButton->SetColor(graphToLoad->getTitleColor());
	m_tickColorButton->SetColor(graphToLoad->getTickColor());
	m_titleFont = graphToLoad->getTitleFont();
	setFontButtonsText(m_titleFontButton, m_titleFont.family());
	m_tickFont = graphToLoad->getTickFont();
	setFontButtonsText(m_tickFontButton, m_tickFont.family());
	m_mainAxisStyleButton->setPen(graphToLoad->getMainAxesPen());
	m_xGridStyleButton->setPen(graphToLoad->getXGridPen());
	m_yGridStyleButton->setPen(graphToLoad->getYGridPen());
	m_borderStyleButton->SetColor(graphToLoad->getBorderColor());
	m_borderStyleButton->SetWidth(graphToLoad->getBorderWidth());
	
	if (asDefault) {
		delete graphToLoad;
	}	
	
	if (!asDefault) {  // the following parameters are not restored to default
		m_xVariableList->addItems(m_graph->getAvailableVariables());
		m_xVariableList->setCurrentRow(m_graph->getAvailableVariables().indexOf(m_graph->getShownXVariable()));
		m_yVariableList->addItems(m_graph->getAvailableVariables());
		m_yVariableList->setCurrentRow(m_graph->getAvailableVariables().indexOf(m_graph->getShownYVariable()));
		
		m_graphTitleEdit->setText(m_graph->getTitle());
		m_xLowLimitEdit->setValue(m_graph->getXLowLimit());
		m_xHighLimitEdit->setValue(m_graph->getXHighLimit());
		m_xTickSizeEdit->setValue(m_graph->getXTickSize());
		m_yLowLimitEdit->setValue(m_graph->getYLowLimit());
		m_yHighLimitEdit->setValue(m_graph->getYHighLimit());
		m_yTickSizeEdit->setValue(m_graph->getYTickSize());
		m_xLimitsManuelCheckBox->setChecked(true);
		m_xTickManuelCheckBox->setChecked(true);
		m_yLimitsManuelCheckBox->setChecked(true);
		m_yTickManuelCheckBox->setChecked(true);
	}
}

void GraphOptionsDialog::onCancelButtonClicked() {
	reject();
}

void GraphOptionsDialog::onRestoreButtonClicked() {
	initView(true);
}

void GraphOptionsDialog::onApplyButtonClicked() {
	m_graph->setTitle(m_graphTitleEdit->text());
	m_graph->setShownVariables((m_xVariableList->currentItem() == NULL ? "" : m_xVariableList->currentItem()->text()),
							   (m_yVariableList->currentItem() == NULL ? "" : m_yVariableList->currentItem()->text()));
	
	/* silent error prevention by ignoring invalid values */
	if (!m_xLimitsManuelCheckBox->isChecked() && m_xLowLimitEdit->getValue() < m_xHighLimitEdit->getValue()) {
		m_graph->setXLimits(m_xLowLimitEdit->getValue(), m_xHighLimitEdit->getValue());
	} else {
		m_graph->setOptimalXLimits();
	}
	if (!m_yLimitsManuelCheckBox->isChecked() && m_yLowLimitEdit->getValue() < m_yHighLimitEdit->getValue()) {
		m_graph->setYLimits(m_yLowLimitEdit->getValue(), m_yHighLimitEdit->getValue());
	} else {
		m_graph->setOptimalYLimits();
	}
	if (!m_xTickManuelCheckBox->isChecked() && m_xTickSizeEdit->getValue() > 0) {
		m_graph->setXTickSize(m_xTickSizeEdit->getValue());
	}
	if (!m_yTickManuelCheckBox->isChecked() && m_yTickSizeEdit->getValue() > 0) {
		m_graph->setYTickSize(m_yTickSizeEdit->getValue());
	}
	m_graph->setXGridPen(m_xGridStyleButton->getPen());
	m_graph->setYGridPen(m_yGridStyleButton->getPen());
	m_graph->setMainAxesPen(m_mainAxisStyleButton->getPen());
	m_graph->setBorderColor(m_borderStyleButton->GetColor());
	m_graph->setBorderWidth(m_borderStyleButton->GetWidth());
	m_graph->setBackgroundColor(m_backgroundColorButton->GetColor());
	m_graph->setTickColor(m_tickColorButton->GetColor());
	m_graph->setTitleColor(m_titleColorButton->GetColor());
	m_graph->setTickFont(m_tickFont);
	m_graph->setTitleFont(m_titleFont);
	
	g_mainFrame->getTwoDWidget()->update();
}

void GraphOptionsDialog::onOkButtonClicked() {
	onApplyButtonClicked();
	accept();
}

void GraphOptionsDialog::onLineButtonClicked() {
	LineButton *clickedButton = dynamic_cast<LineButton*> (QObject::sender());
	
	LinePickerDlg linePicker;
	if (clickedButton == m_borderStyleButton) {
		linePicker.disableLineStyle();
	}
	linePicker.InitDialog(clickedButton->GetStyle(), clickedButton->GetWidth(), clickedButton->GetColor());
	if(linePicker.exec() == QDialog::Accepted) 	{
		clickedButton->SetStyle(linePicker.GetStyle());
		clickedButton->SetWidth(linePicker.GetWidth());
		clickedButton->SetColor(linePicker.GetColor());
	}	
}

void GraphOptionsDialog::onColorButtonClicked() {
	ColorButton *clickedButton = dynamic_cast<ColorButton*> (QObject::sender());
	
	QColor chosenColor = QColorDialog::getColor(clickedButton->GetColor());
	if (chosenColor.isValid()) {  // if user cancels, the color is not valid
		clickedButton->SetColor(chosenColor);
	}
}

void GraphOptionsDialog::onFontButtonClicked() {
	QPushButton *clickedButton = dynamic_cast<QPushButton*> (QObject::sender());
	QFont *font = (clickedButton == m_tickFontButton) ? &m_tickFont : &m_titleFont;
	*font = QFontDialog::getFont(NULL, *font);
	setFontButtonsText(clickedButton, font->family());
}

void GraphOptionsDialog::setFontButtonsText(QPushButton *button, QString fontName) {
	QString buttonText = fontName;
	QFont defaultFont;  // NM the empty constructor provides the default font
	QFontMetrics metrics (defaultFont);
	if (metrics.width(buttonText) > button->width()-20) {
		const int dotsLength = metrics.width("...");
		do {
			buttonText.chop(1);
		} while (metrics.width(buttonText) + dotsLength > button->width()-20);
		buttonText.append("...");
		button->setToolTip(fontName);
	} else {
		button->setToolTip("");
	}
	button->setText(buttonText);
}

void GraphOptionsDialog::onAxisValueChanged() {
	NumberEdit *changedEdit = dynamic_cast<NumberEdit*> (QObject::sender());
	
	m_yLimitsManuelCheckBox->setChecked(false);		
	m_xLimitsManuelCheckBox->setChecked(false);		
	if (changedEdit == m_yTickSizeEdit) {
		m_yTickManuelCheckBox->setChecked(false);
	} else if (changedEdit == m_xTickSizeEdit) {
		m_xTickManuelCheckBox->setChecked(false);
	} 
}
