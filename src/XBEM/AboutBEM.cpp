/****************************************************************************

    AboutBEM Class
        Copyright (C) 2010 David Marten qblade@web.de

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

#include <QtWidgets>
#include "AboutBEM.h"
#include "../XBEM/BEM.h"
#include <QBitmap>

AboutBEM::AboutBEM(void *parent)
{
        setWindowTitle(tr("About QBlade"));

        m_pBEM = parent;

        SetupLayout();
        connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
}

AboutBEM::~AboutBEM()
{
}

void AboutBEM::SetupLayout()
{
        QBEM *pBEM = (QBEM*)m_pBEM;
        QHBoxLayout *LogoLayout = new QHBoxLayout;
        m_IconQ5 = new QLabel;
        m_IconQ5->setObjectName("iconQBLADE");
        m_IconQ5->setGeometry(QRect(20, 0, 134, 100));
        m_IconQ5->setPixmap(QPixmap(QString::fromUtf8(":/images/TUBerlin.png")));

        m_Rotor = new QLabel;
        m_Rotor->setObjectName("iconRotor");
        m_Rotor->setGeometry(QRect(20, 0, 100, 100));
        m_Rotor->setPixmap(QPixmap(QString::fromUtf8(":/images/blank_logo.png")));

        m_HFI = new QLabel;
        m_HFI->setObjectName("iconHFI");
        m_HFI->setGeometry(QRect(20, 0, 100, 100));
        m_HFI->setPixmap(QPixmap(QString::fromUtf8(":/images/hfi.png")));

        QLabel* m_dfg = new QLabel;
        m_dfg->setObjectName("icon^DFG");
        m_dfg->setGeometry(QRect(20, 0, 100, 100));
        m_dfg->setPixmap(QPixmap(QString::fromUtf8(":/images/dfg.png")));

        QLabel *lab1  = new QLabel(pBEM->m_VersionName);
        lab1->setAlignment(Qt::AlignLeft);
        LogoLayout->addWidget(m_Rotor);
        LogoLayout->addStretch(1);
        LogoLayout->addWidget(m_HFI);
        LogoLayout->addStretch(1);
        LogoLayout->addWidget(m_IconQ5);
        LogoLayout->addStretch(1);
        LogoLayout->addWidget(m_dfg);



        QLabel *lab2  = new QLabel(tr("Copyright (C) David Marten (Contact: david.marten@tu-berlin.de) ")+pBEM->m_VersionName);
        QLabel *lab3  = new QLabel(tr("This program is distributed in the hope that it will be useful,"));
        QLabel *lab4  = new QLabel(tr("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
        QLabel *lab5  = new QLabel(tr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));

        QLabel *lab9  = new QLabel(tr("The QBlade project is managed and developed by David Marten with continuous support and technical consulting <br/>from George Pechlivanoglou, who also initiated the project in 2010. The QBlade team is a part of the <br/>Wind Energy Group of the Berlin Institute of Technology (TU Berlin) situated at the Insitute <br/>of Fluid Dynamics and Technical Acoustics (ISTA), led by Oliver Paschreit and Navid Nayeri. <br/>The code development is partially funded through the German Science Foundation (DFG) project PAK780 <br/>QBlade is open source and distributed under the terms of the GNU General Public License <a href=\"https://www.gnu.org/licenses/gpl.txt\">Link </a><br/>If you want to contribute to QBlade please contact one of the authors. The source code can be found at <a href=\"http://sourceforge.net/projects/qblade/\">Link</a>"));
        lab9->setTextFormat(Qt::RichText);
        lab9->setOpenExternalLinks(true);
        lab9->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);




        QLabel *lab16  = new QLabel(tr("If you want to reference QBlade inside a report or publication please use the following reference:"));
        QLabel *lab17  = new QLabel(tr("D. Marten, J. Wendler, G. Pechlivanoglou, C. N. Nayeri und C. O. Paschereit\n\"QBlade: An Open Source Tool for Design and Simulation of Horizontal and Vertical Axis Wind Turbines\"\nInternational Journal of Emerging Technology and Advanced Engineering (IJETAE), 3 (Special Issue 3):264-269\nFebruary  2013, ISSN: 2250-2459"));


        lab17->setOpenExternalLinks(true);
        lab17->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);



        QLabel *lab10  = new QLabel(tr("List of Contributions - final projects at the ISTA, TU Berlin:"));
        QLabel *lab11  = new QLabel(tr("David Marten - Blade Design and BEM Modules (2010)"));
        QLabel *lab12  = new QLabel(tr("Guido Weinzierl - Initiation of Windfield Generator (2011)"));
        QLabel *lab13  = new QLabel(tr("Juliane Wendler - VAWT module (2012)"));
        QLabel *lab14  = new QLabel(tr("Nikolai Moesus - Windfield and FAST modules, GUI and database development (2013)"));
        QLabel *lab15  = new QLabel(tr("Matthew Lennie - QFEM Module (2013)"));



        m_pXFLR5Link = new QLabel;
        m_pXFLR5Link->setText("<a href=http://fd.tu-berlin.de>http://fd.tu-berlin.de</a>");
        m_pXFLR5Link->setOpenExternalLinks(true);
        m_pXFLR5Link->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        m_pQBladeLink = new QLabel;
        m_pQBladeLink->setText("<a href=http://q-blade.org>http://q-blade.org</a>");
        m_pQBladeLink->setOpenExternalLinks(true);
        m_pQBladeLink->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        QHBoxLayout *LinkLayout = new QHBoxLayout;
        LinkLayout->addWidget(m_pQBladeLink);
        LinkLayout->addStretch(1);
        LinkLayout->addWidget(m_pXFLR5Link);

        OKButton = new QPushButton(tr("OK"));
        QHBoxLayout *OKLayout = new QHBoxLayout;
        OKLayout->addStretch(1);
        OKLayout->addWidget(OKButton);
        OKLayout->addStretch(1);

        QVBoxLayout *MainLayout = new QVBoxLayout;
        MainLayout->addLayout(LogoLayout);
        MainLayout->addSpacing(20);
//        MainLayout->addWidget(lab1);
        MainLayout->addWidget(lab2);
        MainLayout->addSpacing(20);
        MainLayout->addWidget(lab3);
        MainLayout->addWidget(lab4);
        MainLayout->addWidget(lab5);
        MainLayout->addSpacing(20);
        MainLayout->addWidget(lab9);

        MainLayout->addSpacing(20);


        MainLayout->addWidget(lab16);
        MainLayout->addSpacing(5);
        MainLayout->addWidget(lab17);
        MainLayout->addSpacing(20);



        MainLayout->addWidget(lab10);
        MainLayout->addWidget(lab11);
        MainLayout->addWidget(lab12);
        MainLayout->addWidget(lab13);
        MainLayout->addWidget(lab14);
        MainLayout->addWidget(lab15);
        MainLayout->addSpacing(20);
        MainLayout->addLayout(LinkLayout);
        MainLayout->addSpacing(20);
        MainLayout->addLayout(OKLayout);
        setLayout(MainLayout);
        setMinimumHeight(400);
}
