

#include "CircularFoilDlg.h"
#include <QGridLayout>
#include <QPushButton>
#include <QKeyEvent>





CircularFoilDlg::CircularFoilDlg()
{
    setWindowTitle(tr("Add a Circular Foil to the Database"));



    SetupLayout();
    m_CircleName->setText("Circular Foil");
    m_CircularDrag->setValue(1.2);
    m_CircularDrag->setMinimum(0.001);
    m_CircularDrag->setMaximum(10);


}


void CircularFoilDlg::SetupLayout()
{
    QGridLayout *MainGrid = new QGridLayout;
    QLabel *Name   = new QLabel(tr("Foil Name"));
    QLabel *Drag  = new QLabel(tr("Drag Coefficient"));

    m_CircularDrag = new NumberEdit;
    m_CircleName = new QLineEdit;
    m_pctrlMessage = new QLabel();
    m_pctrlMessage->setMinimumWidth(120);


    m_CircleName->setAlignment(Qt::AlignRight);
    m_CircularDrag->setAlignment(Qt::AlignRight);
    MainGrid->addWidget(Name,     1,1, 1,1, Qt::AlignRight);
    MainGrid->addWidget(m_CircleName,  1,2, 1,1, Qt::AlignRight);
    MainGrid->addWidget(m_pctrlMessage, 2,1, 1,2, Qt::AlignRight);
    MainGrid->addWidget(Drag,    3,1, 1,1, Qt::AlignRight);
    MainGrid->addWidget(m_CircularDrag,  3,2, 1,1, Qt::AlignRight);


    QHBoxLayout *CommandButtons = new QHBoxLayout;
    OKButton = new QPushButton(tr("OK"));
    OKButton->setAutoDefault(false);
    CancelButton = new QPushButton(tr("Cancel"));
    CancelButton->setAutoDefault(false);
    CommandButtons->addStretch(1);
    CommandButtons->addWidget(OKButton);
    CommandButtons->addStretch(1);
    CommandButtons->addWidget(CancelButton);
    CommandButtons->addStretch(1);
    connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(MainGrid);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(CommandButtons);

    setLayout(mainLayout);


}



void CircularFoilDlg::keyPressEvent(QKeyEvent *event)
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
            else if (OKButton->hasFocus())
            {
                OnOK();
            }
            return;
        }
        case Qt::Key_Escape:
        {
            reject();
            return;
        }
    }
}


void CircularFoilDlg::OnOK()
{
    accept();
}

