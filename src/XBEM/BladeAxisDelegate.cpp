/****************************************************************************

    BladeDelegate Class
        Copyright (C) 2012 David Marten david.marten@tu-berlin.de

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


#include <QtWidgets>
#include "BEM.h"
#include "BladeAxisDelegate.h"


QWidget *BladeAxisDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,const QModelIndex & index ) const
{
        NumberEdit *editor = new NumberEdit();
		editor->setParent(parent);
        editor->setAlignment(Qt::AlignRight);
        editor->setAutomaticPrecision(m_Precision[index.column()]);

        if(index.column()==0 /*|| index.column()==3*/)
        {
        editor->setEnabled(false);
        }

        return editor;
}



void BladeAxisDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

        double value = index.model()->data(index, Qt::EditRole).toDouble();
        NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
        floatEdit->setValue(value);

        if(index.column()==3 /*|| index.column()==3*/)
        {
        floatEdit->setMaximum(1.0);
        floatEdit->setMinimum(-1.0);
        }

}

void BladeAxisDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

        NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
        double value = floatEdit->getValue(true);
        model->setData(index, value, Qt::EditRole);

}

void BladeAxisDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QString strong;
        QStyleOptionViewItem myOption = option;

        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble(),0,'f', m_Precision[index.column()]);

        drawDisplay(painter, myOption, myOption.rect, strong);
        drawFocus(painter, myOption, myOption.rect);
}


void BladeAxisDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
        editor->setGeometry(option.rect);
}

void BladeAxisDelegate::SetPointers(int *PrecisionTable, int *pNPanels)
{
        m_Precision = PrecisionTable;
        m_pNPanels = pNPanels;
}













