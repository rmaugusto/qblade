#ifndef GRAPHOPTIONSDIALOG_H
#define GRAPHOPTIONSDIALOG_H

#include <QDialog>
class QListWidget;
class QLineEdit;
class QCheckBox;

class NewGraph;
class NumberEdit;
class LineStyleButton;
class NewColorButton;


class GraphOptionsDialog : public QDialog
{
	Q_OBJECT
	
public:
	GraphOptionsDialog(NewGraph *graph);
	
private:
	void initView (bool asDefault = false);
	
	NewGraph *m_graph;
	QLineEdit *m_graphTitleEdit;
	QListWidget *m_xVariableList, *m_yVariableList;
	NumberEdit *m_xLowLimitEdit, *m_xHighLimitEdit, *m_yLowLimitEdit, *m_yHighLimitEdit;
	NumberEdit *m_xTickSizeEdit, *m_yTickSizeEdit;
	QCheckBox *m_xLogScale, *m_yLogScale;
	QCheckBox *m_xLimitsManualCheckBox, *m_yLimitsManualCheckBox, *m_xTickManuelCheckBox, *m_yTickManuelCheckBox;
	LineStyleButton *m_xGridStyleButton, *m_yGridStyleButton, *m_mainAxisStyleButton, *m_borderStyleButton;
	NewColorButton *m_backgroundColorButton, *m_tickColorButton, *m_titleColorButton;
	QPushButton *m_tickFontButton, *m_titleFontButton;
	QFont m_tickFont, m_titleFont;
	
private slots:
	void onCancelButtonClicked ();
	void onRestoreButtonClicked ();
	void onApplyButtonClicked ();
	void onOkButtonClicked ();
	void onFontButtonClicked ();  // FontButton is not even implemented...
	void setFontButtonsText (QPushButton *button, QString fontName);
	void onAxisValueChanged ();
	void onLogarithmicChanged ();
};

#endif // GRAPHOPTIONSDIALOG_H
