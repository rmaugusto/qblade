#include "AboutFAST.h"

#include <QtWidgets>
#include <QBitmap>



AboutFAST::AboutFAST(void */*parent*/)
{
        setWindowTitle(tr("About FAST and AeroDyn"));


        SetupLayout();
        connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
}

AboutFAST::~AboutFAST()
{
}

void AboutFAST::SetupLayout()
{
        QHBoxLayout *LogoLayout = new QHBoxLayout;
        m_NRELIcon = new QLabel;
        m_NRELIcon->setObjectName("iconNREL");
        m_NRELIcon->setGeometry(QRect(20, 0, 61, 71));
        m_NRELIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/nrel.png")));


        QLabel *lab1  = new QLabel("FAST (v7.02.00d-bjj)");
        lab1->setAlignment(Qt::AlignLeft);
        LogoLayout->addWidget(m_NRELIcon);




        QLabel *lab2  = new QLabel(tr("By Jason Jonkman and David J. Laino"));
        QLabel *lab3  = new QLabel(tr("Supported by Bonnie Jonkman"));
        QLabel *lab4  = new QLabel(tr("QBlade is coupled with FAST / AeroDyn (v7.02.00d-bjj) from the National Renewable Energy Laboratory (NREL)"));

        QLabel *lab6  = new QLabel(tr("FAST is an aeroelastic computer-aided engineering (CAE) tool for horizontal axis wind turbines"));
        QLabel *lab7  = new QLabel(tr("AeroDyn is a software library for aerodynamics analysis routines for horizontal-axis wind"));
        QLabel *lab8  = new QLabel(tr("turbines and performs the aerodynamic simulations within the FAST framework"));


        QLabel *lab10  = new QLabel(tr("FAST is developed by Jason Jonkman"));
        QLabel *lab11  = new QLabel(tr("Aerodyn is developed by David J. Laino"));
        QLabel *lab12  = new QLabel(tr("With Support by Bonnie Jonkman"));

        QLabel *lab9  = new QLabel(tr("More Information on FAST and AeroDyn can be found On the NWTC Information Portal under the following links "));





        m_pXFLR5Link = new QLabel;
        m_pXFLR5Link->setText("<a href=http://wind.nrel.gov/designcodes/simulators/aerodyn>wind.nrel.gov/designcodes/simulators/aerodyn</a>");
        m_pXFLR5Link->setOpenExternalLinks(true);
        m_pXFLR5Link->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        m_pQBladeLink = new QLabel;
        m_pQBladeLink->setText("<a href=http://wind.nrel.gov/designcodes/simulators/fast>wind.nrel.gov/designcodes/simulators/fast</a>");
        m_pQBladeLink->setOpenExternalLinks(true);
        m_pQBladeLink->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        QLabel *m_pDisclaimer = new QLabel;
        m_pDisclaimer->setText("<a href=http://wind.nrel.gov/designcodes/disclaimer.html>wind.nrel.gov/designcodes/disclaimer</a>");
        m_pDisclaimer->setOpenExternalLinks(true);
        m_pDisclaimer->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        QVBoxLayout *LinkLayout = new QVBoxLayout;
        LinkLayout->addWidget(m_pQBladeLink);
        LinkLayout->addStretch(1);
        LinkLayout->addWidget(m_pXFLR5Link);
        LinkLayout->addStretch(1);
        LinkLayout->addWidget(m_pDisclaimer);


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
        MainLayout->addWidget(lab3);
        MainLayout->addSpacing(20);
        MainLayout->addWidget(lab4);
        MainLayout->addSpacing(20);
        MainLayout->addWidget(lab6);
        MainLayout->addWidget(lab7);
        MainLayout->addWidget(lab8);
        MainLayout->addSpacing(20);


        MainLayout->addWidget(lab10);
        MainLayout->addWidget(lab11);
        MainLayout->addWidget(lab12);
        MainLayout->addSpacing(20);
        MainLayout->addWidget(lab9);
        MainLayout->addSpacing(20);
        MainLayout->addLayout(LinkLayout);
        MainLayout->addSpacing(20);
        MainLayout->addLayout(OKLayout);
        setLayout(MainLayout);
        setMinimumHeight(400);
}

