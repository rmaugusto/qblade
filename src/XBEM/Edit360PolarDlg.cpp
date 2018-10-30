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

#include "Edit360PolarDlg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>

#include "BEM.h"
#include "../Graph/Curve.h"



Edit360PolarDlg::Edit360PolarDlg()
{
        setWindowTitle(tr("Polar Points Edition"));
        m_pBEM    = NULL;
        m_pPolar      = NULL;
        curIndex = 0;

        SetupLayout();
}



void Edit360PolarDlg::InitDialog()
{
        FillTable();
}


void Edit360PolarDlg::FillTable()
{
        QString strong;

        m_pctrlAlphaList->clear();
        if(m_pBEM && m_pPolar)
        {
                        for (int i=0; i<m_pPolar->m_Alpha.size(); i++)
                        {
                                strong = QString("%1").arg(m_pPolar->m_Alpha.at(i),8,'f',3);
                                m_pctrlAlphaList->addItem(strong);
                        }

        }
        m_pctrlAlphaList->setCurrentRow(curIndex);

//        if (m_pctrlAlphaList->count())
//        {
//            UpdateSpinBox(m_pctrlAlphaList->currentRow());
//        }



}


void Edit360PolarDlg::keyPressEvent(QKeyEvent *event)
{
        // Prevent Return Key from closing App
        switch (event->key())
        {
                case Qt::Key_Return:
                {
                        if(!OKButton->hasFocus() && !CancelButton->hasFocus())
                        {
                                OKButton->setFocus();
                        }
                        else
                        {
                                QDialog::accept();
                        }
                        break;
                }
                case Qt::Key_Escape:
                {
                        QDialog::reject();
                        return;
                }
                default:
                        event->ignore();
        }
}


void Edit360PolarDlg::OnDeletePoint()
{
        QBEM *pBEM = (QBEM *) m_pBEM;


        curIndex = m_pctrlAlphaList->currentRow();
        if(pBEM)
        {
                m_pPolar->m_Alpha.removeAt(curIndex);
                m_pPolar->m_Cd.removeAt(curIndex);
                m_pPolar->m_Cl.removeAt(curIndex);
                m_pPolar->m_Cm.removeAt(curIndex);
                m_pPolar->m_Cl_att.removeAt(curIndex);
                m_pPolar->m_Cl_sep.removeAt(curIndex);
                m_pPolar->m_fst.removeAt(curIndex);

                FillTable();
                if (curIndex >=0) CreateGraphs(curIndex);
        }

        if(curIndex>=m_pctrlAlphaList->count()-1)
        {
                curIndex = m_pctrlAlphaList->count()-1;
        }
        if (m_pctrlAlphaList->count()) m_pctrlAlphaList->setCurrentRow(curIndex);
}




void Edit360PolarDlg::SetupLayout()
{
        QVBoxLayout *CommandButtons = new QVBoxLayout;
        m_pctrlDeletePoint	= new QPushButton(tr("Delete Point"));
        OKButton            = new QPushButton(tr("OK"));
        CancelButton        = new QPushButton(tr("Cancel"));

        m_pctrlCdBox = new QDoubleSpinBox;
        m_pctrlClBox = new QDoubleSpinBox;
        m_pctrlClLabel = new QLabel(tr("Cl"));
        m_pctrlCdLabel = new QLabel(tr("Cd"));
        
        m_pctrlCdBox->setSingleStep(0.001);
        m_pctrlClBox->setSingleStep(0.001);
        m_pctrlCdBox->setDecimals(3);
        m_pctrlClBox->setDecimals(3);
        m_pctrlClBox->setMinimum(-300);
        m_pctrlCdBox->setMinimum(-300);
        m_pctrlClBox->setMaximum(300);
        m_pctrlCdBox->setMaximum(300);





        

        QGridLayout *BoxLayout = new QGridLayout;

        BoxLayout->addWidget(m_pctrlClLabel,1,1);
        BoxLayout->addWidget(m_pctrlCdLabel,1,2);
        BoxLayout->addWidget(m_pctrlClBox,2,1);
        BoxLayout->addWidget(m_pctrlCdBox,2,2);

        CommandButtons->addLayout(BoxLayout);
        CommandButtons->addStretch(1);
        CommandButtons->addWidget(m_pctrlDeletePoint);
        CommandButtons->addStretch(2);
        CommandButtons->addWidget(OKButton);
        CommandButtons->addWidget(CancelButton);
        CommandButtons->addStretch(1);

        m_pctrlAlphaList = new QListWidget;

        QHBoxLayout * MainLayout = new QHBoxLayout(this);
        MainLayout->addStretch(1);
        MainLayout->addWidget(m_pctrlAlphaList);
        MainLayout->addStretch(1);
        MainLayout->addLayout(CommandButtons);
        MainLayout->addStretch(1);

        setLayout(MainLayout);

        connect(m_pctrlDeletePoint, SIGNAL(clicked()),this, SLOT(OnDeletePoint()));

        connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
        connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
        connect(m_pctrlAlphaList, SIGNAL(currentRowChanged(int)), this, SLOT(UpdateSpinBox(int)));
        connect(m_pctrlClBox, SIGNAL(valueChanged(double)), this, SLOT(ClChanged(double)));
        connect(m_pctrlCdBox, SIGNAL(valueChanged(double)), this, SLOT(CdChanged(double)));

}


void Edit360PolarDlg::UpdateSpinBox(int row)
{


    if (row >=0 && m_pctrlAlphaList->count())
    {
    m_pctrlClBox->setEnabled(true);
    m_pctrlCdBox->setEnabled(true);
    m_pctrlClBox->setValue(m_pPolar->m_Cl.at(row));
    m_pctrlCdBox->setValue(m_pPolar->m_Cd.at(row));

    CreateGraphs(row);
    }
    else
    {
    m_pctrlClBox->setEnabled(false);
    m_pctrlCdBox->setEnabled(false);
    }

}

void Edit360PolarDlg::ClChanged(double val)
{


    if (fabs(val) < 0.000001) val = 0.0001;

    m_pPolar->m_Cl[m_pctrlAlphaList->currentRow()] = val;

    CreateGraphs(m_pctrlAlphaList->currentRow());

}

void Edit360PolarDlg::CdChanged(double val)
{

    if (fabs(val) < 0.000001) val = 0.0001;

    m_pPolar->m_Cd[m_pctrlAlphaList->currentRow()] = val;

    CreateGraphs(m_pctrlAlphaList->currentRow());



}

void Edit360PolarDlg::CreateGraphs(int row)
{
    QBEM *pBEM = (QBEM *) m_pBEM;
    QList <double> ClCd;



    pBEM->CreateSinglePolarCurve(false);

    for (int i=0;i<m_pPolar->m_Cl.size();i++)
    {

        ClCd.append(m_pPolar->m_Cl.at(i)/m_pPolar->m_Cd.at(i));
    }



    CCurve* pPolarCurve = pBEM->m_360Graph1.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(4);
    pPolarCurve->SetColor(QColor(0,255,0));
    QList <double> *X = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph1.GetXVariable());
    QList <double> *Y = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph1.GetYVariable());

    if (pBEM->m_360Graph1.GetXVariable() == 3 && pBEM->m_360Graph1.GetYVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row),ClCd.at(row));
    else if (pBEM->m_360Graph1.GetXVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row), Y->at(row));
    else if (pBEM->m_360Graph1.GetYVariable() == 3) pPolarCurve->AddPoint(X->at(row),ClCd.at(row));
    else pPolarCurve->AddPoint(X->at(row),Y->at(row));



    pPolarCurve = pBEM->m_360Graph2.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(4);
    pPolarCurve->SetColor(QColor(0,255,0));
    X = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph2.GetXVariable());
    Y = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph2.GetYVariable());

    if (pBEM->m_360Graph2.GetXVariable() == 3 && pBEM->m_360Graph2.GetYVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row),ClCd.at(row));
    else if (pBEM->m_360Graph2.GetXVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row), Y->at(row));
    else if (pBEM->m_360Graph2.GetYVariable() == 3) pPolarCurve->AddPoint(X->at(row), ClCd.at(row));
    else pPolarCurve->AddPoint(X->at(row),Y->at(row));


    pPolarCurve = pBEM->m_360Graph3.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(4);
    pPolarCurve->SetColor(QColor(0,255,0));
    X = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph3.GetXVariable());
    Y = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph3.GetYVariable());

    if (pBEM->m_360Graph3.GetXVariable() == 3 && pBEM->m_360Graph3.GetYVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row),ClCd.at(row));
    else if (pBEM->m_360Graph3.GetXVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row), Y->at(row));
    else if (pBEM->m_360Graph3.GetYVariable() == 3) pPolarCurve->AddPoint(X->at(row), ClCd.at(row));
    else pPolarCurve->AddPoint(X->at(row),Y->at(row));


    pPolarCurve = pBEM->m_360Graph4.AddCurve();
    pPolarCurve->ShowPoints(true);
    pPolarCurve->SetWidth(4);
    pPolarCurve->SetColor(QColor(0,255,0));
    X = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph4.GetXVariable());
    Y = (QList <double> *) pBEM->GetVariable(m_pPolar, pBEM->m_360Graph4.GetYVariable());

    if (pBEM->m_360Graph4.GetXVariable() == 3 && pBEM->m_360Graph4.GetYVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row),ClCd.at(row));
    else if (pBEM->m_360Graph4.GetXVariable() == 3) pPolarCurve->AddPoint(ClCd.at(row), Y->at(row));
    else if (pBEM->m_360Graph4.GetYVariable() == 3) pPolarCurve->AddPoint(X->at(row), ClCd.at(row));
    else pPolarCurve->AddPoint(X->at(row),Y->at(row));



    pBEM->UpdateView();
}






