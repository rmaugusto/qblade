/****************************************************************************

    OptimizeDlgVAWT Class
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

#ifndef OPTIMIZEDLGVAWT_H
#define OPTIMIZEDLGVAWT_H

#include "DMS.h"
#include <QtWidgets>
#include "../Misc/NumberEdit.h"

class OptimizeDlgVAWT : public QDialog
{

    Q_OBJECT

    friend class XBEM;
    friend class XDMS;

private slots:

    void CheckButtons();
    void OnOptimize();


public:

    OptimizeDlgVAWT(void *pParent);
    void SetupLayout();
    void InitDialog();
    void Connect();


    void *m_pParent;

    QComboBox *FromPosChord, *ToPosChord;
    QRadioButton *OptNone, *OptTroposk, *OptArcLine, *ArcLine, *OptStraight, *OptHelix;
    QLabel *FromPosChordLabel, *ToPosChordLabel, *MaxDisplLabel, *LR1, *R0Label, *R1Label, *dRLabel, *StraightLabel, *CircAngleFromLabel, *CircAngleToLabel, *LR2;
    NumberEdit *MaxDispl, *R0, *R1, *dR, *Straight, *CircAngleFrom, *CircAngleTo;
    QPushButton *Optimize, *Done;

    QList <double> m_Zt; // troposkien height coordinate
    QList <double> m_Rt; // troposkien radius coordinate
    QList <double> m_Zf; // fitted curve height coordinate
    QList <double> m_Rf; // fitted curve radius coordinate
    CBlade *m_pBlade;

};


#endif // OPTIMIZEDLGVAWT_H
