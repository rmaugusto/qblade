/****************************************************************************

    BladeScaleDlg Class
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

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>
#include "BladeScaleDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"


BladeScaleDlg::BladeScaleDlg(void */*pParent*/)
{
        setWindowTitle(tr("Scale HAWT Blade Dlg")); // JW modification
        m_bSpan = m_bChord = m_bTwist = false;

        m_NewChord = m_NewTwist = m_NewSpan = false;
        m_RefChord = m_RefTwist = m_RefSpan = 1.0;
        SetupLayout();
}


void BladeScaleDlg::SetupLayout()
{
        QGridLayout *ScaleLayout = new QGridLayout;
        m_pctrlSpan  = new QCheckBox(tr("Pos Scaling"));
        m_pctrlChord = new QCheckBox(tr("Chord Scaling"));
        m_pctrlTwist = new QCheckBox(tr("Twist Scaling"));

        m_pctrlNewSpan  = new NumberEdit(NumberEdit::Standard, 3);
        m_pctrlNewChord = new NumberEdit(NumberEdit::Standard, 3);
        m_pctrlNewTwist = new NumberEdit(NumberEdit::Standard, 3);

        m_pctrlRefSpan  = new QLabel("0.000");
        m_pctrlRefChord = new QLabel("0.000");
        m_pctrlRefTwist = new QLabel("0.000");
        m_pctrlRefSpan->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_pctrlRefChord->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_pctrlRefTwist->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        m_pctrlSpanRatio  = new QLabel("1.000");
        m_pctrlChordRatio = new QLabel("1.000");
        m_pctrlTwistRatio = new QLabel("1.000");
        m_pctrlSpanRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_pctrlChordRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_pctrlTwistRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QLabel *lab11 = new QLabel(tr("Reference"));
        QLabel *lab12 = new QLabel(tr("New"));
        QLabel *lab13 = new QLabel(tr("Ratio"));
        lab11->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        lab12->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        lab13->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ScaleLayout->addWidget(lab11, 1,2);
        ScaleLayout->addWidget(lab12, 1,3);
        ScaleLayout->addWidget(lab13, 1,5);

        ScaleLayout->addWidget(m_pctrlSpan,      2,1);
        ScaleLayout->addWidget(m_pctrlRefSpan,   2,2);
        ScaleLayout->addWidget(m_pctrlNewSpan,   2,3);
        ScaleLayout->addWidget(m_pctrlSpanRatio, 2,5);

        ScaleLayout->addWidget(m_pctrlChord,     3,1);
        ScaleLayout->addWidget(m_pctrlRefChord,  3,2);
        ScaleLayout->addWidget(m_pctrlNewChord,  3,3);
        ScaleLayout->addWidget(m_pctrlChordRatio,3,5);

        ScaleLayout->addWidget(m_pctrlTwist,     4,1);
        ScaleLayout->addWidget(m_pctrlRefTwist,  4,2);
        ScaleLayout->addWidget(m_pctrlNewTwist,  4,3);
        ScaleLayout->addWidget(m_pctrlTwistRatio,4,5);

        m_pctrlUnit20 = new QLabel;
        m_pctrlUnit21 = new QLabel;
        ScaleLayout->addWidget(m_pctrlUnit20, 2,4);
        ScaleLayout->addWidget(m_pctrlUnit21, 3,4);

        QHBoxLayout *CommandButtons = new QHBoxLayout;
        QPushButton *OKButton = new QPushButton(tr("OK"));
        QPushButton *CancelButton = new QPushButton(tr("Cancel"));
        CommandButtons->addStretch(1);
        CommandButtons->addWidget(OKButton);
        CommandButtons->addStretch(1);
        CommandButtons->addWidget(CancelButton);
        CommandButtons->addStretch(1);

        QHBoxLayout *StretchLayout = new QHBoxLayout;
        StretchLayout->addStretch(1);
        StretchLayout->addLayout(ScaleLayout);
        StretchLayout->addStretch(1);

        QVBoxLayout *MainLayout = new QVBoxLayout;
        MainLayout->addStretch(1);
        MainLayout->addLayout(StretchLayout);
        MainLayout->addStretch(1);
        MainLayout->addLayout(CommandButtons);
        MainLayout->addStretch(1);

        setLayout(MainLayout);

        connect(OKButton, SIGNAL(clicked()), this, SLOT(OnOK()));
        connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

        connect(m_pctrlSpan,  SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
        connect(m_pctrlChord, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
        connect(m_pctrlTwist, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));

        connect(m_pctrlNewSpan,  SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
        connect(m_pctrlNewChord, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
        connect(m_pctrlNewTwist, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}


void BladeScaleDlg::InitDialog(double const &RefSpan, double const &RefChord, double const &RefTwist)
{
        QString len;
		GetLengthUnit(len, g_mainFrame->m_LengthUnit);
        m_pctrlUnit20->setText(len);
        m_pctrlUnit21->setText(len);

        m_RefSpan  = RefSpan;
        m_RefChord = RefChord;
        m_RefTwist = RefTwist;

        m_NewSpan  = RefSpan;
        m_NewChord = RefChord;
        m_NewTwist = RefTwist;

        m_pctrlSpan->setChecked(m_bSpan);//(false)
        m_pctrlChord->setChecked(m_bChord);
        m_pctrlTwist->setChecked(m_bTwist);


        QString strong;

		strong = QString("%1").arg(m_RefSpan * g_mainFrame->m_mtoUnit,8,'f',3);
        m_pctrlRefSpan->setText(strong);

		strong = QString("%1").arg(m_RefChord * g_mainFrame->m_mtoUnit,8,'f',3);
        m_pctrlRefChord->setText(strong);


        strong = QString("%1").arg(m_RefTwist,8,'f',2);
        m_pctrlRefTwist->setText(strong);

		m_pctrlNewSpan->setValue(m_NewSpan * g_mainFrame->m_mtoUnit);
		m_pctrlNewChord->setValue(m_NewChord * g_mainFrame->m_mtoUnit);
        m_pctrlNewTwist->setValue(m_NewTwist);

        SetResults();
        EnableControls();
}



void BladeScaleDlg::OnClickedCheckBox()
{
        ReadData();
        EnableControls();
}


void BladeScaleDlg::OnOK()
{
        ReadData();
        accept();
}

void BladeScaleDlg::OnEditingFinished()
{
        ReadData();
        SetResults();
}



void BladeScaleDlg::EnableControls()
{
        m_pctrlNewSpan->setEnabled(m_bSpan);
        m_pctrlNewChord->setEnabled(m_bChord);
        m_pctrlNewTwist->setEnabled(m_bTwist);
}


void BladeScaleDlg::ReadData()
{

        m_bSpan  = m_pctrlSpan->isChecked();
        m_bChord = m_pctrlChord->isChecked();
        m_bTwist = m_pctrlTwist->isChecked();

		m_NewSpan  = m_pctrlNewSpan->getValue()  / g_mainFrame->m_mtoUnit;
		m_NewChord = m_pctrlNewChord->getValue() / g_mainFrame->m_mtoUnit;
        m_NewTwist = m_pctrlNewTwist->getValue();
}


void BladeScaleDlg::SetResults()
{
        QString strong;

        if(m_RefSpan>0.0)  strong = QString("%1").arg(m_NewSpan/m_RefSpan, 6,'f',3);
        else               strong =" 1.000";
        m_pctrlSpanRatio->setText(strong);

        if(m_RefChord>0.0) strong = QString("%1").arg(m_NewChord/m_RefChord, 6,'f',3);
        else               strong =" 1.000";
        m_pctrlChordRatio->setText(strong);

        if(m_RefTwist>0.0) strong = QString("%1").arg(m_NewTwist/m_RefTwist, 6,'f',3);
        else               strong =" 1.000";
        m_pctrlTwistRatio->setText(strong);

}






