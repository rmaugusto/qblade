/****************************************************************************

    OptimizeDlg Class
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

#ifndef OPTIMIZEDLG_H
#define OPTIMIZEDLG_H


#include "BEM.h"
#include <QtWidgets>
#include "../Misc/NumberEdit.h"

class OptimizeDlg : public QDialog
{

    Q_OBJECT

    friend class XBEM;

private slots:


    void CheckButtons();
    void OnOptimize();


public:

    OptimizeDlg(void *pParent);
    void SetupLayout();
    void InitDialog();
    void Connect();
	C360Polar* Get360Polar(QString m_FoilName, QString PolarName);








    void *m_pParent;

    NumberEdit *FromChord, *ToChord, *Lambda0, *LambdaStall, *FromTwist, *ToTwist, *DeltaTwist;
    QComboBox *FromPosChord, *ToPosChord;
    QRadioButton *OptSchmitz, *OptBetz, *OptLinear, *OptNone, *OptNone2, *OptStall, *OptGlide;
    QLabel *Lambda0Label, *FromPosChordLabel, *ToPosChordLabel, *FromChordLabel, *ToChordLabel;
    QPushButton *Optimize, *Done;
    QLabel *LambdaStallLabel, *FromTwistLabel, *ToTwistLabel, *DeltaTwistLabel;
    QRadioButton *OptLinear2;
    QLabel *LT1, *LT2, *LT3, *LT4, *LC1, *LC2;

public slots:


};


#endif // OPTIMIZEDLG_H
