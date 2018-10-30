/****************************************************************************

    CreateDMSDlg Class
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

#ifndef CREATEDMSDLG_H
#define CREATEDMSDLG_H


#include "DMS.h"
#include <QtWidgets>
#include "../Misc/NumberEdit.h"

class CreateDMSDlg : public QDialog
{
    Q_OBJECT
    friend class XDMS;

private slots:



public slots:

    void CheckButtons();
    void OnConst();
    void OnPow();
    void OnLog();


public:

    CreateDMSDlg(void *pParent);
    void SetupLayout();
    void Connect();

    NumberEdit *IterationsEdit, *EpsilonEdit, *ElementsEdit, *RhoEdit, *RelaxEdit, *ViscEdit, *ExpEdit, *RoughEdit;
    QLabel *IterationsLabel, *ElementsLabel, *EpsilonLabel, *RhoEditLabel, *RelaxEditLabel, *ViscEditLabel, *ExpEditLabel, *RoughEditLabel;
    QRadioButton *PowerLawRadio, *ConstantRadio, *LogarithmicRadio;
    QCheckBox *TipLossBox, *VariableBox, *AspectRatioBox;//, *RootLossBox, *ThreeDBox, *InterpolationBox, *NewTipLossBox, *NewRootLossBox;
    QPushButton *OkButton;
    QLineEdit *SimName;
    QLabel *ViscUnitLabel, *RhoUnitLabel, *RoughUnitLabel;
    void *m_pParent;



private:


    /*
    bool RootLoss;
    bool ThreeDCorrection;
    bool Interpolation;
    bool NewTipLoss;
    bool NewRootLoss;
    */

};


#endif // CREATEDMSDLG_H
