#ifndef PARAMETERGRID_H
#define PARAMETERGRID_H

#include <QGridLayout>

#include "ParameterViewer.h"


template <class ParameterGroup>
class ParameterGrid : public QGridLayout
{
public:
	typedef typename ParameterGroup::Key Key;
	ParameterGrid(ParameterViewer<ParameterGroup> *viewer) : m_viewer(viewer) { }
	
	void addEdit(Key key, typename ParameterViewer<ParameterGroup>::Type type, QWidget *widget, QString displayName,
					QVariant defaultValue, Unit unit = NONE) {
		const int row = rowCount();
		QLabel *label = new QLabel(displayName);
		addWidget(label, row, 0);
		addWidget(widget, row, 1);
		if (unit != NONE) {
			m_viewer->addUnitLabel(key, unit, label);
		}
		m_viewer->addView(key, type, widget, defaultValue);
	}
	
	void addComboBox(Key key, QString displayName, int defaultIndex, QStringList items) {
		const int row = rowCount();
		QLabel *label = new QLabel(displayName);
		addWidget(label, row, 0);
		QComboBox *box = new QComboBox;
		box->addItems(items);
		addWidget(box, row, 1);
		m_viewer->addView(key, ParameterViewer<ParameterGroup>::ComboBox, box, defaultIndex);
	}
	
	void addLabel(Key key, bool isNumber, QString displayName, Unit unit = NONE) {
		const int row = rowCount();
		QLabel *label = new QLabel(displayName);
		addWidget(label, row, 0);
		QLabel *value = new QLabel("");
		addWidget(value, row, 1);
		if (unit != NONE) {
			m_viewer->addUnitLabel(key, unit, label);
		}
		m_viewer->addView(key, (isNumber ? ParameterViewer<ParameterGroup>::NumberLabel :
										   ParameterViewer<ParameterGroup>::Label),
						  value, "");
	}
	
	void addRow(QString name, QString value) {
		const int row = rowCount();
		QLabel *label = new QLabel(name);
		addWidget(label, row, 0);
		label = new QLabel(value);
		addWidget(label, row, 1);
	}
	
private:
	ParameterViewer<ParameterGroup> *m_viewer;
};

#endif // PARAMETERGRID_H
