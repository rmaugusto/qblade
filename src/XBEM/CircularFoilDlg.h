#ifndef CIRCULARFOILDLG_H
#define CIRCULARFOILDLG_H

#include <QDialog>
#include <QLabel>
#include "../Objects/Foil.h"
#include "../Misc/NumberEdit.h"
#include <QLineEdit>


class CircularFoilDlg : public QDialog
{
    Q_OBJECT

    friend class XBEM;



private slots:
    void OnOK();

public:
    CircularFoilDlg();
    void SetupLayout();
    void keyPressEvent(QKeyEvent *event);

    QPushButton *OKButton, *CancelButton;
    QLabel * m_pctrlMessage;
    QLineEdit * m_CircleName;
    NumberEdit * m_CircularDrag;


};


#endif // CIRCULARFOILDLG_H
