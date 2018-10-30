/****************************************************************************

    BladeScale Class
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

#ifndef BLADESCALEDLG_H
#define BLADESCALEDLG_H


#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "../Misc/NumberEdit.h"


class BladeScaleDlg : public QDialog
{
        Q_OBJECT
	
        friend class MainFrame;
public:
        BladeScaleDlg(void *pParent = NULL);
        void InitDialog(double const &RefSpan, double const &RefChord, double const &RefTwist);
private:
        void SetupLayout();
        void ReadData();
        void SetResults();
        void EnableControls();

private slots:
        void OnOK();
        void OnClickedCheckBox();
        void OnEditingFinished();


public:

        QCheckBox *m_pctrlSpan, *m_pctrlChord, *m_pctrlTwist;
        NumberEdit  *m_pctrlNewSpan, *m_pctrlNewChord, *m_pctrlNewTwist;
        QLabel *m_pctrlRefSpan, *m_pctrlRefChord, *m_pctrlRefTwist;
        QLabel *m_pctrlSpanRatio, *m_pctrlChordRatio, *m_pctrlTwistRatio;
        QLabel *m_pctrlUnit20, *m_pctrlUnit21;

        bool  m_bSpan, m_bChord, m_bTwist;
        double  m_NewChord, m_NewTwist, m_NewSpan;
        double  m_RefChord, m_RefTwist, m_RefSpan;
};




#endif // BLADESCALEDLG_H
