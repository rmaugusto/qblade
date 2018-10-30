/****************************************************************************

    BladeScaleDlgVAWT Class
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

#ifndef BLADESCALEDLGVAWT_H
#define BLADESCALEDLGVAWT_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "../Misc/NumberEdit.h"

class BladeScaleDlgVAWT : public QDialog
{
    Q_OBJECT

    friend class MainFrame;

public:
    BladeScaleDlgVAWT(void *pParent = NULL);
    void InitDialog(double const &RefSpan, double const &RefShift, double const &RefChord, double const &RefOffset, double const &RefTwist);

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

    QCheckBox *m_pctrlSpan, *m_pctrlChord, *m_pctrlOffset, *m_pctrlTwist, *m_pctrlShift;
    NumberEdit  *m_pctrlNewSpan, *m_pctrlNewChord, *m_pctrlNewOffset, *m_pctrlNewTwist, *m_pctrlNewShift;
    QLabel *m_pctrlRefSpan, *m_pctrlRefChord, *m_pctrlRefOffset, *m_pctrlRefTwist, *m_pctrlRefShift;
    QLabel *m_pctrlSpanRatio, *m_pctrlChordRatio, *m_pctrlOffsetRatio, *m_pctrlShiftRatio;
    QLabel *m_pctrlUnit20, *m_pctrlUnit21, *m_pctrlUnit22, *m_pctrlUnit23;

    bool  m_bSpan, m_bChord, m_bOffset, m_bTwist, m_bShift;
    double  m_NewChord, m_NewTwist, m_NewOffset, m_NewSpan, m_NewShift;
    double  m_RefChord, m_RefTwist, m_RefOffset, m_RefSpan, m_RefShift;

};

#endif // BLADESCALEDLGVAWT_H
