#ifndef POLARSELECTIONDIALOG_H
#define POLARSELECTIONDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include "../src/XBEM/Blade.h"
#include "../src/Objects/Foil.h"

class PolarSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    PolarSelectionDialog(CFoil *pFoil, CBlade *pBlade);
    PolarSelectionDialog(CFoil *pFoil, QVector<C360Polar *> list);
    QStringList GetPolarNamesList(){ return clickedPolars; }
    double GetMin(){ return min; }
    double GetMax(){ return max; }

private:
    QList<QCheckBox *> boxList;
    QList<QString> namesList;
    QList<double> reynoldsList;

    QList<QString> clickedPolars;
    QPushButton *accept;
    QLabel *warning;
    double min, max;

private slots:
    void OnCheckBoxClicked();
};

#endif // POLARSELECTIONDIALOG_H
