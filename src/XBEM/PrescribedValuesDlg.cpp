#include "PrescribedValuesDlg.h"
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>


PrescribedValuesDlg::PrescribedValuesDlg()
{
        m_pBEM    = NULL;
        curIndex  = 0;
        editpitch = false;
        editrot = false;

        SetupLayout();
}

void PrescribedValuesDlg::keyPressEvent(QKeyEvent *event)
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
        default:
            event->ignore();
    }
}

void PrescribedValuesDlg::OnInsertPoint()
{

    int sel;
    sel = m_pctrlPitchView->currentIndex().row();


    if(sel<=0) return;


    windspeeds.insert(sel,(windspeeds.at(sel-1) + windspeeds.at(sel))/2.0) ;
    value.insert(sel,(value.at(sel-1) + value.at(sel))/2.0) ;


    FillList();
    SetSelection(sel);

}

void PrescribedValuesDlg::OnDeletePoint()
{

    int  sel;
    QModelIndex index = m_pctrlPitchView->currentIndex();
    sel = index.row();

    if(sel<0) return;

    windspeeds.removeAt(sel);
    value.removeAt(sel);


    FillList();
    SetSelection(sel);

}

void PrescribedValuesDlg::OnCellChanged()
{
    double X,Y;

    int  sel = m_pctrlPitchView->currentIndex().row();

    QModelIndex Xindex = m_pPitchModel->index(sel, 0);
    X = Xindex.data().toDouble();


    if (sel == 0)
    {
        if ( X < windspeeds.at(sel+1))
        {
        windspeeds[sel]=X;
        }
    }
    else if (sel == windspeeds.size()-1)
    {
        if ( X > windspeeds.at(sel-1))
        {
        windspeeds[sel]=X;
        }
    }
    else
    {
        if ( X < windspeeds.at(sel+1)&& X > windspeeds.at(sel-1))
        {
        windspeeds[sel]=X;
        }
    }

    QModelIndex Yindex = m_pPitchModel->index(sel, 1);
    Y = Yindex.data().toDouble();

    if (editpitch)
    {
    value[sel]=Y;
    }
    if (editrot)
    {
        if (Y>0.01)
        {
        value[sel]=Y;
        }
    }


}

void PrescribedValuesDlg::FillList()
{
    m_pPitchModel->setRowCount(windspeeds.size());
    m_pPitchModel->setColumnCount(2);
    for (int i=0; i<windspeeds.size(); i++)
    {
        QModelIndex Xindex = m_pPitchModel->index(i, 0, QModelIndex());
        m_pPitchModel->setData(Xindex, windspeeds.at(i));

        QModelIndex Yindex =m_pPitchModel->index(i, 1, QModelIndex());
        m_pPitchModel->setData(Yindex, value.at(i));
    }
//    QModelIndex Xindex = m_pPitchModel->index(int(cut_out-cut_in)+1, 0, QModelIndex());
//    m_pPitchModel->setData(Xindex, cut_out);
//    QModelIndex Yindex =m_pPitchModel->index(int(cut_out-cut_in)+1, 1, QModelIndex());
//    m_pPitchModel->setData(Yindex, pitch);

    m_pctrlPitchView->resizeRowsToContents();
    m_pctrlPitchView->resizeColumnsToContents();
    m_pctrlPitchView->setWindowTitle(QObject::tr("Pitch Angles at given Windspeeds"));
    m_pctrlPitchView->show();
}

void PrescribedValuesDlg::InitDialog()
{

    if (editpitch) m_pctrlOmega->hide();
    m_pPitchModel = new QStandardItemModel;
    m_pPitchModel->setRowCount(10);//temporary
    m_pPitchModel->setColumnCount(2);
    m_pPitchModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Windspeed [m/s]"));
    if (editpitch) m_pPitchModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Pitch Angle []"));
    if (editrot) m_pPitchModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Rot. Speed [1/min]"));


    m_pctrlPitchView->setModel(m_pPitchModel);

    m_pFloatDelegate = new FloatEditDelegate;
    m_pctrlPitchView->setItemDelegate(m_pFloatDelegate);

    int  *precision = new int[2];
    precision[0] = 5;//five digits for x and y coordinates
    precision[1] = 5;
    m_pFloatDelegate->SetPrecision(precision);

    connect(m_pFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged()));
    connect(m_pctrlDeletePoint, SIGNAL(clicked()),this, SLOT(OnDeletePoint()));
    connect(m_pctrlInsertPoint, SIGNAL(clicked()),this, SLOT(OnInsertPoint()));
    connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_pctrlResetButton, SIGNAL(clicked()), this, SLOT(OnReset()));
    connect(m_pctrlResetButton, SIGNAL(clicked()), this, SLOT(FillList()));



    FillList();
}

void PrescribedValuesDlg::OnReset()
{

    double val;

    if (editpitch) val = 0;
    if (editrot) val = m_pctrlOmega->getValue();

    value.clear();
    windspeeds.clear();


    for (int i=0; i<=cut_out-cut_in; i++)
    {
        windspeeds.append(cut_in+i);
        value.append(val);
    }
    if (windspeeds.at(windspeeds.size()-1)<cut_out)
    {

        windspeeds.append(cut_out);
        value.append(val);

    }

}

void PrescribedValuesDlg::SetupLayout()
{

    if (editpitch) setWindowTitle(tr("Prescribed Pitch Angles"));
    if (editrot) setWindowTitle(tr("Prescribed Rotational Speeds"));


    QVBoxLayout *CommandButtons = new QVBoxLayout;
    m_pctrlInsertPoint	= new QPushButton(tr("Insert Point"));
    m_pctrlDeletePoint	= new QPushButton(tr("Delete Point"));
    OKButton            = new QPushButton(tr("OK"));
    CancelButton        = new QPushButton(tr("Cancel"));
    m_pctrlResetButton  = new QPushButton(tr("Reset Values"));
    m_pctrlOmega        = new NumberEdit;
    m_pctrlOmega->setValue(200);

    CommandButtons->addStretch(1);
    CommandButtons->addWidget(m_pctrlInsertPoint);
    CommandButtons->addWidget(m_pctrlDeletePoint);
    CommandButtons->addStretch(1);
    CommandButtons->addWidget(m_pctrlOmega);
    CommandButtons->addWidget(m_pctrlResetButton);
    CommandButtons->addStretch(2);
    CommandButtons->addWidget(OKButton);
    CommandButtons->addWidget(CancelButton);
    CommandButtons->addStretch(1);

    m_pctrlPitchView = new QTableView(this);
    m_pctrlPitchView->setMinimumHeight(500);
//    m_pctrlPitchView->setMinimumWidth(150);

    QHBoxLayout * MainLayout = new QHBoxLayout(this);
    MainLayout->addWidget(m_pctrlPitchView);
    MainLayout->addLayout(CommandButtons);
    setLayout(MainLayout);


}

void PrescribedValuesDlg::SetSelection(int sel)
{
    if(sel>=0)
    {
        m_pctrlPitchView->selectRow(sel);
    }
}
