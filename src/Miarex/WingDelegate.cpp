/****************************************************************************

	WingDelegate Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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
 

#include "../Objects/Foil.h"
#include "WingDelegate.h"
#include "GL3dWingDlg.h"
#include "../XGlobals.h"
#include "../Store.h"


WingDelegate::WingDelegate(QObject *parent)
 : QItemDelegate(parent)
{
	m_pWingDlg = parent;
}


QWidget *WingDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex & index ) const
{
	if(index.column()!=5 && index.column()!=7 && index.column()!=9)
	{
		NumberEdit *editor = new NumberEdit();
		editor->setParent(parent);
		editor->setAlignment(Qt::AlignRight);

		editor->setAutomaticPrecision(m_Precision[index.column()]);
		if(index.column()==6) editor->setMaximum(MAXCHORDPANELS-1);
		if(index.column()==8) editor->setMaximum(MAXSPANSTATIONS-1);
		return editor;
	}
	else
	{
		QString strong;
		QComboBox *editor = new QComboBox(parent);
		editor->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		//fill comboboxes here
		if(index.column()==5)
		{
			for(int i=0; i< g_foilStore.size(); i++)
			{
				CFoil *pFoil = g_foilStore.at(i);
				pFoil->GetFoilName(strong);
				editor->addItem(strong);
			}
		}
		else if(index.column()==7)
		{
			editor->addItem(tr("Uniform"));
			editor->addItem(tr("Cosine"));
		}
		else if(index.column()==9)
		{
			editor->addItem(tr("Uniform"));
			editor->addItem(tr("Cosine"));
			editor->addItem(tr("Sine"));
			editor->addItem(tr("-Sine"));
		}
		return editor;
	}
	return NULL;
}


void WingDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if(index.column()!=5 && index.column()!=7 && index.column()!=9)
	{
		double value = index.model()->data(index, Qt::EditRole).toDouble();
		NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
		floatEdit->setValue(value);
	}
	else
	{
		QString strong = index.model()->data(index, Qt::EditRole).toString();
		QComboBox *pCbBox = static_cast<QComboBox*>(editor);
		int pos = pCbBox->findText(strong);
		if (pos>=0) pCbBox->setCurrentIndex(pos);
		else        pCbBox->setCurrentIndex(0);
	}
}


void WingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if(index.column()!=5 && index.column()!=7 && index.column()!=9)
	{
		NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
		double value = floatEdit->getValue(true);
		model->setData(index, value, Qt::EditRole);
	}
	else
	{
		QString strong;
		QComboBox *pCbBox = static_cast<QComboBox*>(editor);
		int sel = pCbBox->currentIndex();
		if (sel >=0) strong = pCbBox->itemText(sel);
		model->setData(index, strong, Qt::EditRole);
	}
}


void WingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QString strong;
	QStyleOptionViewItem myOption = option;
	if(index.column()!=5 && index.column()!=7 && index.column()!=9)
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble(),0,'f', m_Precision[index.column()]);
	}
	else
	{
		myOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
		strong = index.model()->data(index, Qt::DisplayRole).toString();
	}

	if(index.row()> *m_pNPanels) strong=" ";
	if(index.row()== *m_pNPanels)
	{
		if(index.column()==3 ||index.column()>=6) strong = " ";
	}
	drawDisplay(painter, myOption, myOption.rect, strong);
	drawFocus(painter, myOption, myOption.rect);
}


void WingDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}


void WingDelegate::SetPointers(int *PrecisionTable, int *pNPanels)
{
	m_Precision = PrecisionTable;
	m_pNPanels = pNPanels;
}











