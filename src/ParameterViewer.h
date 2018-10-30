#ifndef PARAMETERVIEWER_H
#define PARAMETERVIEWER_H

#include <QHash>
#include <QPair>
#include <QVariant>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>

#include "Globals.h"
#include "ParameterObject.h"
#include "Misc/NumberEdit.h"
#include "StoreAssociatedComboBox.h"
#include "Misc/FixedSizeLabel.h"
#include "StorableObject.h"


template <class ParameterGroup>
class ParameterViewer
{
public:
	typedef typename ParameterGroup::Key Key;
	enum Type {Unknown, CheckBox, NumberEditType, LineEdit, Label, NumberLabel, BoxLabel, RotorBox, ComboBox};
	
	void addView (Key key, Type type, QWidget *widget, QVariant defaultValue) {
		m_views.insert(key, ViewTuple(type, widget, defaultValue));
	}
	void addUnitLabel (Key key, Unit unit, QLabel *label) { m_units.insert(key, UnitPair(unit, label)); }
	void loadObject (ParameterObject<ParameterGroup> *object);
	void storeObject (ParameterObject<ParameterGroup> *object);
	
protected:
	class ViewTuple {
	public:
		ViewTuple(ParameterViewer::Type t, QWidget *w, QVariant v) : type(t), widget(w), defaultValue(v) { }
		ParameterViewer::Type type; QWidget *widget; QVariant defaultValue;
	};
	typedef QPair<Unit,QLabel*> UnitPair;
	QHash<Key,ViewTuple> m_views;
	QHash<Key,UnitPair> m_units;
	
	ParameterViewer() { }
	virtual ~ParameterViewer () { }
	
	void setParameter (Key key, QVariant value);
	QVariant getParameter (Key key);
	QString prepareNumberString (QVariant variant);
	QString prepareObjectString(QVariant value);
	template <class Object>
	void setObjectBox(QWidget *widget, QVariant value);
	template <class Object>
	QVariant getObjectBox(QWidget *widget);
	
	template <class Widget>
	Widget* get (Key key);
	QLabel* getUnitLabel (Key key);
	
	virtual void onUnitsChanged () = 0;  // should be implemented as slot
	void setUnitContainingLabels ();
};


/* implementation */
template <class ParameterGroup>
template <class Widget>
Widget* ParameterViewer<ParameterGroup>::get (Key key) {
	Widget* widget = dynamic_cast<Widget*>(m_views.value(key, ViewTuple(Unknown, NULL, QVariant())).widget);
	if (!widget) {
		qDebug() << "Invalid cast or parameter in ParameterViewer::get";
	}
	return widget;
}

template <class ParameterGroup>
QLabel *ParameterViewer<ParameterGroup>::getUnitLabel(Key key) {
	QLabel* label = m_units.value(key, UnitPair(NONE, NULL)).second;
	if (!label) {
		qDebug() << "Invalid parameter in ParameterViewer::getUnitLabel";
	}
	return label;
}

template <class ParameterGroup>
void ParameterViewer<ParameterGroup>::setParameter(Key key, QVariant value) {
	ViewTuple view = m_views.value(key, ViewTuple(Unknown, NULL, QVariant()));
	
	if (m_units.contains(key) && value.toString() != "") {
		const double unitFactor = getUnitFactor(m_units.value(key).first);
		const double number = value.toDouble();
		value = QVariant(number * unitFactor);
	}

	switch(view.type) {
	case CheckBox: static_cast<QCheckBox*>(view.widget)->setChecked(value.toBool()); break;
	case NumberEditType: static_cast<NumberEdit*>(view.widget)->setValue(value.toDouble()); break;
	case LineEdit: static_cast<QLineEdit*>(view.widget)->setText(value.toString()); break;
	case Label: static_cast<QLabel*>(view.widget)->setText(value.toString()); break;
	case NumberLabel: static_cast<QLabel*>(view.widget)->setText(prepareNumberString(value)); break;
	case BoxLabel: static_cast<FixedSizeLabel*>(view.widget)->setText(prepareObjectString(value)); break;
	case RotorBox: setObjectBox<CBlade>(view.widget, value); break;
	case ComboBox: static_cast<QComboBox*>(view.widget)->setCurrentIndex(value.toInt()); break;
	case Unknown: qDebug() << "Attempt to set invalid parameter" << key << "in ParameterViewer!";
	}
}

template <class ParameterGroup>
QVariant ParameterViewer<ParameterGroup>::getParameter(Key key) {
	ViewTuple view = m_views.value(key, ViewTuple(Unknown, NULL, QVariant()));
	
	QVariant value;
	switch(view.type) {
	case CheckBox: value = QVariant(static_cast<QCheckBox*>(view.widget)->isChecked()); break;
	case NumberEditType: value = QVariant(static_cast<NumberEdit*>(view.widget)->getValue()); break;
	case LineEdit: value = QVariant(static_cast<QLineEdit*>(view.widget)->text()); break;
	case Label:
	case NumberLabel: value = QVariant(static_cast<QLabel*>(view.widget)->text()); break;
	case BoxLabel: value = QVariant(static_cast<FixedSizeLabel*>(view.widget)->text()); break;
	case RotorBox: value = getObjectBox<CBlade>(view.widget); break;
	case ComboBox: value = QVariant(static_cast<QComboBox*>(view.widget)->currentIndex()); break;
	default: qDebug() << "Attempt to get invalid parameter" << key << "in ParameterViewer!";
	}
	
	if (m_units.contains(key)) {
		const double unitFactor = getUnitFactor(m_units.value(key).first);
		const double number = value.toDouble();
		value = QVariant(number / unitFactor);
	}
	
	return value;
}

template <class ParameterGroup>
QString ParameterViewer<ParameterGroup>::prepareNumberString (QVariant variant) {
	if (variant.toString() == "")  // can happen because "" is the default
		return "";
	
	const double PRECISION = 10e-12;
	double number = variant.toDouble();
	
	/* calculate exponent */
	number = (fabs(number) < PRECISION) ? 0 : number;  // catch rounding errors
	int exponent = (number == 0) ? 0 : floor(log10(fabs(number)+PRECISION));  // log10 can't handle 0; catch rounding errors	
	
	const bool useExponent = (exponent >= 5 || exponent <= -3);
	QString string = (useExponent ? QString::number(number, 'e') : QString::number(number, 'f'));
	
	/* chop zeros */
	if (string.contains('.')) {
		if (useExponent) {
			const int posOfE = string.indexOf('e');
			int posOfFirstZero = posOfE-1;
			while (string[posOfFirstZero] == '0')
				--posOfFirstZero;
			if (string[posOfFirstZero] == '.')
				--posOfFirstZero;
			string.remove(posOfFirstZero+1, posOfE - posOfFirstZero - 1);
		} else {
			while(string.endsWith('0'))
				string.chop(1);
			if (string.endsWith('.'))
				string.chop(1);
		}
	}
	
	return string;
}

template <class ParameterGroup>
template <class Object>
void ParameterViewer<ParameterGroup>::setObjectBox(QWidget *widget, QVariant value) {
	Object *object = reinterpret_cast<Object*>(value.value<quintptr>());
	if (object) {  // happens for defaultValue which is NULL
		static_cast<StoreAssociatedComboBox<Object>*>(widget)->setCurrentObject(object);
	}
}

template <class ParameterGroup>
template <class Object>
QVariant ParameterViewer<ParameterGroup>::getObjectBox(QWidget *widget) {
	return reinterpret_cast<quintptr>(static_cast<StoreAssociatedComboBox<CBlade>*>(widget)->currentObject());
}

template <class ParameterGroup>
QString ParameterViewer<ParameterGroup>::prepareObjectString(QVariant value) {
	if (value.toString() == "") {
		return "";
	} else {
		return reinterpret_cast<StorableObject*>(value.value<quintptr>())->getName();
	}
}

template <class ParameterGroup>
void ParameterViewer<ParameterGroup>::loadObject(ParameterObject<ParameterGroup> *object) {
	typename QHash<Key,ViewTuple>::iterator iter;
	for (iter = m_views.begin(); iter != m_views.end(); ++iter) {
		setParameter(iter.key(), (object ? object->getParameter(iter.key()) : iter.value().defaultValue));
	}
}

template <class ParameterGroup>
void ParameterViewer<ParameterGroup>::storeObject(ParameterObject<ParameterGroup> *object) {
	typename QHash<Key,ViewTuple>::iterator iter;
	for (iter = m_views.begin(); iter != m_views.end(); ++iter) {
		object->setParameter(iter.key(), getParameter(iter.key()));
	}
}

template <class ParameterGroup>
void ParameterViewer<ParameterGroup>::setUnitContainingLabels() {
	typename QHash<Key,UnitPair>::iterator iter;
	for (iter = m_units.begin(); iter != m_units.end(); ++iter) {
		QString text = iter.value().second->text();
		text = text.replace(text.indexOf('[')+1, text.indexOf(']')-text.indexOf('[')-1, getUnitName(iter.value().first));
		iter.value().second->setText(text);
	}
}

#endif // PARAMETERVIEWER_H
