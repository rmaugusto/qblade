/****************************************************************************

    BladeDelegate Class
        Copyright (C) 2010 David Marten david.marten@tu-berlin.de

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
#include "../Objects/Polar.h"
#include "BladeDelegate.h"
#include "../Store.h"
#include "BEM.h"
#include "Blade.h"


BladeDelegate::BladeDelegate (CBlade *blade, void *pBEM, QObject *parent) : QItemDelegate(parent){
    m_pBlade = blade;
    m_pBEM = pBEM;
}


QWidget *BladeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,const QModelIndex & index ) const
{
        if(index.column()!=3 && index.column()!=4)
        {
                NumberEdit *editor = new NumberEdit();
				editor->setParent(parent);
                editor->setAlignment(Qt::AlignRight);

                editor->setAutomaticPrecision(m_Precision[index.column()]);

                if(index.column()==0 && index.row()==0) editor->setEnabled(false);

                return editor;
        }
        else if (!(index.column()==4 && !m_pBlade->m_bisSinglePolar))
        {
                QString strong, strong2;
                QModelIndex ind;
                QComboBox *editor = new QComboBox(parent);
                //fill comboboxes here
                if(index.column()==3)
                {
						for(int i=0; i< g_foilStore.size(); i++)
                        {
								CFoil *pFoil = g_foilStore.at(i);
                                pFoil->GetFoilName(strong);
                                editor->addItem(strong);
                        }
                }
                if(index.column()==4)
                {
                    editor->clear();
						for(int i=0; i< g_360PolarStore.size(); i++)
                        {
								C360Polar *pPolar = g_360PolarStore.at(i);
                                QStandardItemModel *model = (QStandardItemModel*) itemmodel;

                                ind = model->index(index.row(),(index.column() -1),QModelIndex());

								strong = pPolar->m_airfoil->getName();
								strong2 = pPolar->getName();


                                if (model->data(ind,Qt::DisplayRole) == strong)
                                {
                                editor->addItem(strong2);
                                }
                        }
                }
                return editor;
        }
        else{
                QBEM *pBEM = (QBEM *) m_pBEM;
                QPushButton *editor = new QPushButton(tr("-----"), parent);
                editor->setFlat(true);
                editor->setStyleSheet("QPushButton { text-align: left; }");
                connect (editor,SIGNAL(pressed()), pBEM, SLOT(OnPolarDialog()));
                return editor;
        }
        return NULL;
}



void BladeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
        if(index.column()!=3 && index.column()!=4)
        {
                double value = index.model()->data(index, Qt::EditRole).toDouble();
                NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
                floatEdit->setValue(value);
        }
        else if (index.column()==3 || (index.column() == 4 && m_pBlade->m_bisSinglePolar))
        {
                QString strong = index.model()->data(index, Qt::EditRole).toString();
                QComboBox *pCbBox = static_cast<QComboBox*>(editor);
                int pos = pCbBox->findText(strong);
                if (pos>=0) pCbBox->setCurrentIndex(pos);
                else        pCbBox->setCurrentIndex(0);
        }
        else if (index.column()==4 && !m_pBlade->m_bisSinglePolar){
                QString strong = index.model()->data(index, Qt::EditRole).toString();
                QPushButton *pButton =  static_cast<QPushButton*>(editor);
                pButton->setText(strong);
        }
}

void BladeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
        if(index.column()!=3 && index.column()!=4)
        {
                NumberEdit *floatEdit = static_cast<NumberEdit*>(editor);
                double value = floatEdit->getValue(true);
                model->setData(index, value, Qt::EditRole);

        }
        else if (index.column()==3 || (index.column() == 4 && m_pBlade->m_bisSinglePolar))
        {
                QString strong;
                QComboBox *pCbBox = static_cast<QComboBox*>(editor);
                int sel = pCbBox->currentIndex();
                if (sel >=0) strong = pCbBox->itemText(sel);
                model->setData(index, strong, Qt::EditRole);
        }
        else if (index.column()==4 && !m_pBlade->m_bisSinglePolar){
                QString strong;
                QPushButton *pButton = static_cast<QPushButton*>(editor);
                strong = pButton->text();
                model->setData(index, strong, Qt::EditRole);
        }
}

void BladeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QString strong;
        QStyleOptionViewItem myOption = option;
        if(index.column()!=3 && index.column()!=4)
        {
                myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
                strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble(),0,'f', m_Precision[index.column()]);
        }
        else
        {
                myOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
                strong = index.model()->data(index, Qt::DisplayRole).toString();
        }


        drawDisplay(painter, myOption, myOption.rect, strong);
        drawFocus(painter, myOption, myOption.rect);
}


void BladeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
        editor->setGeometry(option.rect);
}

void BladeDelegate::SetPointers(int *PrecisionTable, int *pNPanels)
{
        m_Precision = PrecisionTable;
        m_pNPanels = pNPanels;
}












