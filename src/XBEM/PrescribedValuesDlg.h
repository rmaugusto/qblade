#ifndef PrescribedValuesDlg_H
#define PrescribedValuesDlg_H

#include <QDialog>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QList>
#include "../Misc/FloatEditDelegate.h"
#include "../Misc/NumberEdit.h"
#include "../Objects/Foil.h"


class PrescribedValuesDlg : public QDialog
{
        Q_OBJECT


        friend class QBEM;
        friend class QDMS;


public:
        PrescribedValuesDlg();
        void InitDialog();

public slots:  //private slots:
        void OnDeletePoint();
        void OnInsertPoint();
        void SetSelection(int sel);
        void OnCellChanged();
        void OnReset();
        void FillList();







private:
        void SetupLayout();


//        void FillTable();
        void keyPressEvent(QKeyEvent *event);

public:  //private:
        QPushButton *m_pctrlDeletePoint, *m_pctrlInsertPoint, *m_pctrlResetButton;
        QPushButton *OKButton, *CancelButton;

        FloatEditDelegate *m_pFloatDelegate;
        QTableView *m_pctrlPitchView;
        QStandardItemModel *m_pPitchModel;
        NumberEdit *m_pctrlOmega;
        QList <double> windspeeds;
        QList <double> value;
        void *m_pBEM;
        double cut_in, cut_out;
        bool editpitch;
        bool editrot;

        int curIndex;

};

#endif // PrescribedValuesDlg_H
