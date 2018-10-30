#include "ExportGeomDlg.h"

ExportGeomDlg::ExportGeomDlg()
{
    setWindowTitle(tr("Export Geometry: Specify Resolution"));


    QVBoxLayout *mainLayout = new QVBoxLayout;
    QGridLayout *layout = new QGridLayout;


    QLabel *chord = new QLabel(tr("Chordwise Points (per Side)"));
    Chordwise = new NumberEdit;
    Chordwise->setMinimum(2);
    Chordwise->setMaximum(300);


    layout->addWidget(chord,1,1);
    layout->addWidget(Chordwise,1,2);

    QLabel *span = new QLabel(tr("Spanwise Points"));
    Spanwise = new NumberEdit;
    Spanwise->setMinimum(2);
    Spanwise->setMaximum(3000);

    layout->addWidget(span,2,1);
    layout->addWidget(Spanwise,2,2);

    QHBoxLayout *exportlayout = new QHBoxLayout;
    Export = new QPushButton(tr("Export"));
    Cancel = new QPushButton(tr("Cancel"));
    exportlayout->addWidget(Export);
    exportlayout->addWidget(Cancel);


    mainLayout->addLayout(layout);
    mainLayout->addLayout(exportlayout);

    connect(Export, SIGNAL(clicked()),SLOT(accept()));
    connect(Cancel, SIGNAL(clicked()),SLOT(reject()));


    setLayout(mainLayout);
}


