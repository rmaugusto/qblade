/****************************************************************************

    Edit360PolarDlg Class
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

#ifndef EDIT360POLARDLG_H
#define EDIT360POLARDLG_H

#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QDoubleSpinBox>
#include <QLabel>

#include "../XBEM/360Polar.h"
#include "../Graph/Curve.h"


class Edit360PolarDlg : public QDialog
{
        Q_OBJECT


        friend class QBEM;

public:
        Edit360PolarDlg();
        void InitDialog();

private slots:
        void OnDeletePoint();
        void UpdateSpinBox(int row);
        void ClChanged(double val);
        void CdChanged(double val);
        void CreateGraphs(int row);





private:
        void SetupLayout();
        void FillTable();
        void keyPressEvent(QKeyEvent *event);

private:
        QPushButton *m_pctrlDeletePoint;
        QPushButton *OKButton, *CancelButton;

        QListWidget *m_pctrlAlphaList;
        QDoubleSpinBox *m_pctrlClBox, *m_pctrlCdBox;
        QLabel *m_pctrlClLabel, *m_pctrlCdLabel;
		C360Polar *m_pPolar;

        void *m_pBEM;

        int curIndex;

};

#endif // EDIT360POLARDLG_H
