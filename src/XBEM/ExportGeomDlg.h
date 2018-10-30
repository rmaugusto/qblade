#ifndef EXPORTGEOMDLG_H
#define EXPORTGEOMDLG_H

#include <QDialog>
#include "../Misc/NumberEdit.h"
#include <QtWidgets>

class ExportGeomDlg : public QDialog
{
    Q_OBJECT
    friend class QBEM;
public:
    ExportGeomDlg();

private:
    NumberEdit *Chordwise, *Spanwise;
    QPushButton *Export, *Cancel;
};

#endif // EXPORTGEOMDLG_H
