/****************************************************************************

    AboutBEM Class
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

#include <QtWidgets>
#include "AboutBEM.h"
#include "../XBEM/BEM.h"
#include <QBitmap>

AboutBEM::AboutBEM(QWidget *parent)
	: QDialog(parent)
{
        setWindowTitle(tr("About QBlade"));

        SetupLayout();
        connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
}

AboutBEM::~AboutBEM()
{
}

void AboutBEM::SetupLayout()
{
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

        QLabel *lab1  = new QLabel(g_mainFrame->m_VersionName);
        lab1->setAlignment(Qt::AlignLeft);
        LogoLayout->addWidget(m_Rotor);
        LogoLayout->addStretch(1);
        LogoLayout->addWidget(m_HFI);
        LogoLayout->addStretch(1);
        LogoLayout->addWidget(m_IconQ5);
//        LogoLayout->addStretch(1);
//        LogoLayout->addWidget(m_dfg);



        QLabel *lab2  = new QLabel(tr("Copyright (C) David Marten (Contact: david.marten@tu-berlin.de) ")+g_mainFrame->m_VersionName);
        QLabel *lab3  = new QLabel(tr("This program is distributed in the hope that it will be useful, but without any warranty; \nwithout even the implied warranty of merchantability or fitness for a particular purpose."));

        QLabel *lab9  = new QLabel(tr("The QBlade project is managed and developed by David Marten with continuous support <br/>and technical consulting from George Pechlivanoglou. The QBlade team is a part of the <br/>Wind Energy Group of the Berlin Institute of Technology (TU Berlin) situated at the Insitute <br/>of Fluid Dynamics and Technical Acoustics (ISTA), led by C.O. Paschreit and C.N. Nayeri. <br/>Code development is partially funded by the German Science Foundation project DFG PAK780"));
        lab9->setTextFormat(Qt::RichText);
        lab9->setOpenExternalLinks(true);
        lab9->setAlignment(Qt::AlignLeft);
        lab9->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);


        QLabel *lab95 = new QLabel("QBlade is distributed under the terms of the GNU General Public License - found here: <a href=\"https://www.gnu.org/licenses/gpl.txt\">GPL</a>");
        QLabel *lab99 = new QLabel("Useful weblinks:");
        QLabel *lab98 = new QLabel("QBlade Forums for Q&A: <a href=\"http://sourceforge.net/p/qblade/discussion/\">QBlade Forums</a>");
        QLabel *lab97 = new QLabel("Developed @ the Chair of FLuid Dynamics: <a href=\"http://fd.tu-berlin.de/en/\">http://fd.tu-berlin.de</a>");
        QLabel *lab96 = new QLabel("Sourceforge project page: <a href=\"http://sourceforge.net/projects/qblade/\">http://sourceforge.net/projects/qblade</a>");


        lab98->setOpenExternalLinks(true);
        lab97->setOpenExternalLinks(true);
        lab96->setOpenExternalLinks(true);
        lab95->setOpenExternalLinks(true);


        QLabel *lab16  = new QLabel(tr("If you want to reference QBlade inside a report or publication, use the following reference:"));
        QLabel *lab17  = new QLabel(tr("D. Marten, M. Lennie, G. Pechlivanoglou, C.N. Nayeri, C.O. Paschereit: \nValidation and Application of an Unsteady Lifting Line Free Vortex Wake \nModule Implemented Within the Open Source Wind Turbine Blade Design \nCode QBlade; J. of Eng. for Gas Turbines and Power, 138(7), Dec 2015"));


        lab17->setOpenExternalLinks(true);
        lab17->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);



        QLabel *lab10  = new QLabel(tr("List of Contributors:"));
        QLabel *lab11  = new QLabel(tr("Nikolai Moesus\tJuliane Wendler\tMatthew Lennie"));
        QLabel *lab12  = new QLabel(tr("Joe Saverin\tGuido Weinzierl\tTobias Weber"));



        OKButton = new QPushButton(tr("OK"));
        QHBoxLayout *OKLayout = new QHBoxLayout;
        OKLayout->addStretch(1);
        OKLayout->addWidget(OKButton);
        OKLayout->addStretch(1);

        QVBoxLayout *MainLayout = new QVBoxLayout;
        MainLayout->addLayout(LogoLayout);
        MainLayout->addSpacing(10);
        MainLayout->addWidget(lab2);
        MainLayout->addSpacing(10);
        MainLayout->addWidget(lab3);
        MainLayout->addSpacing(10);
        MainLayout->addWidget(lab95);
        MainLayout->addSpacing(10);

        MainLayout->addWidget(lab9);

        MainLayout->addSpacing(10);
        MainLayout->addWidget(lab99);
        MainLayout->addWidget(lab98);
        MainLayout->addWidget(lab97);
        MainLayout->addWidget(lab96);
        MainLayout->addSpacing(10);



        MainLayout->addWidget(lab16);
        MainLayout->addSpacing(5);
        MainLayout->addWidget(lab17);
        MainLayout->addSpacing(10);



        MainLayout->addWidget(lab10);
        MainLayout->addWidget(lab11);
        MainLayout->addWidget(lab12);

        MainLayout->addSpacing(20);
        MainLayout->addLayout(OKLayout);
        setLayout(MainLayout);
        setMinimumHeight(650);
}
