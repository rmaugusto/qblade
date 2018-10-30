/****************************************************************************

    BladeDelegate2 Class
        Copyright (C) 2012 Juliane Wendler

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

#ifndef BLADEDELEGATE2_H
#define BLADEDELEGATE2_H

#include <QList>
#include <QItemDelegate>
#include "../Misc/NumberEdit.h"

class CBlade;

class BladeDelegate2 : public QItemDelegate
{
        Q_OBJECT

public:
        BladeDelegate2 (CBlade *blade, void *pDMS, QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void SetPointers(int*PrecisionTable, int *pNPanels);


private:
        void *m_pWingDlg;
        int *m_pNPanels;
        int *m_Precision;
        CBlade *m_pBlade;
        void *m_pDMS;

public:
        void *itemmodel;

};

#endif // BLADEDELEGATE2_H
