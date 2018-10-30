#include "PolarSelectionDialog.h"
#include "../Store.h"
#include "../src/XBEM/360Polar.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QVBoxLayout>

PolarSelectionDialog::PolarSelectionDialog(CFoil *pFoil, CBlade *pBlade)
{

    setWindowTitle(tr("Select Reynolds Number Range"));
    QLabel *lab;
    QCheckBox *box;
    QGridLayout *grid = new QGridLayout;
    QVBoxLayout *QV = new QVBoxLayout;
    int gridrow = 0;
    warning = new QLabel(tr(""));
    accept = new QPushButton(tr("Accept"));
    connect(accept, SIGNAL(clicked()), this, SLOT(accept()));
    accept->setFocus();

    QV->addLayout(grid);

    lab = new QLabel("360 Polar Name");
    grid->addWidget(lab,gridrow,0);
    lab = new QLabel("Reynolds Number");
    grid->addWidget(lab,gridrow,1);
    lab = new QLabel("Select");
    grid->addWidget(lab,gridrow,2);
    gridrow++;
    lab = new QLabel("");
    grid->addWidget(lab,gridrow,0);
    gridrow++;

    for (int i=0; i<g_360PolarStore.size();i++){
        if (pFoil->getName() == g_360PolarStore.at(i)->m_airfoil->getName()){

            if (reynoldsList.size()){
                for (int j=0;j<reynoldsList.size();j++){
                    if (g_360PolarStore.at(i)->reynolds <= reynoldsList.at(j)){
                        reynoldsList.insert(j, g_360PolarStore.at(i)->reynolds);
                        namesList.insert(j,g_360PolarStore.at(i)->getName());
                        break;
                    }
                    else if(j < reynoldsList.size()-1){
                        if (g_360PolarStore.at(i)->reynolds >= reynoldsList.at(j) && g_360PolarStore.at(i)->reynolds <= reynoldsList.at(j+1)){
                            reynoldsList.insert(j+1, g_360PolarStore.at(i)->reynolds);
                            namesList.insert(j+1,g_360PolarStore.at(i)->getName());
                            break;
                        }
                    }
                    if ( j == reynoldsList.size()-1 && g_360PolarStore.at(i)->reynolds >= reynoldsList.at(reynoldsList.size()-1)){
                        reynoldsList.append(g_360PolarStore.at(i)->reynolds);
                        namesList.append(g_360PolarStore.at(i)->getName());
                        break;
                    }
                }
            }
            else{
            namesList.append(g_360PolarStore.at(i)->getName());
            reynoldsList.append(g_360PolarStore.at(i)->reynolds);
            }
        }
    }
    for (int i=0;i<reynoldsList.size();i++){
    lab = new QLabel(namesList.at(i));
    grid->addWidget(lab,gridrow,0);
    lab = new QLabel(QString("%1").arg(reynoldsList.at(i)));
    grid->addWidget(lab,gridrow,1);
    box = new QCheckBox;
    boxList.append(box);

    connect(box, SIGNAL(clicked()), this, SLOT(OnCheckBoxClicked()));
    connect(box, SIGNAL(stateChanged(int)), this, SLOT(OnCheckBoxClicked()));

    box->setChecked(false);
    if (i!=0){
    if (reynoldsList.at(i) > reynoldsList.at(i-1)) box->setChecked(true);
    }
    else box->setChecked(true);

    grid->addWidget(box,gridrow,2);

    gridrow++;
    }

    QV->addWidget(warning);
    QV->addWidget(accept);

    setLayout(QV);

    for (int i=0; i<boxList.size();i++){
        for (int j=0; j<pBlade->m_PolarAssociatedFoils.size();j++){
            if (pBlade->m_PolarAssociatedFoils.at(j) == pFoil->getName()){
                boxList.at(i)->setChecked(false);
                for (int k=0;k<pBlade->m_MultiPolars.at(j).size();k++){
                    if (pBlade->m_MultiPolars.at(j).at(k) == namesList.at(i)) boxList.at(i)->setChecked(true);
                }
            }
        }
    }

}

PolarSelectionDialog::PolarSelectionDialog(CFoil *pFoil, QVector<C360Polar *> list)
{

    setWindowTitle(tr("Select Reynolds Number Range"));
    QLabel *lab;
    QCheckBox *box;
    QGridLayout *grid = new QGridLayout;
    QVBoxLayout *QV = new QVBoxLayout;
    int gridrow = 0;
    warning = new QLabel(tr(""));
    accept = new QPushButton(tr("Accept"));
    connect(accept, SIGNAL(clicked()), this, SLOT(accept()));
    accept->setFocus();

    QV->addLayout(grid);

    lab = new QLabel("360 Polar Name");
    grid->addWidget(lab,gridrow,0);
    lab = new QLabel("Reynolds Number");
    grid->addWidget(lab,gridrow,1);
    lab = new QLabel("Select");
    grid->addWidget(lab,gridrow,2);
    gridrow++;
    lab = new QLabel("");
    grid->addWidget(lab,gridrow,0);
    gridrow++;

    for (int i=0; i<g_360PolarStore.size();i++){
        if (pFoil->getName() == g_360PolarStore.at(i)->m_airfoil->getName()){

            if (reynoldsList.size()){
                for (int j=0;j<reynoldsList.size();j++){
                    if (g_360PolarStore.at(i)->reynolds <= reynoldsList.at(j)){
                        reynoldsList.insert(j, g_360PolarStore.at(i)->reynolds);
                        namesList.insert(j,g_360PolarStore.at(i)->getName());
                        break;
                    }
                    else if(j < reynoldsList.size()-1){
                        if (g_360PolarStore.at(i)->reynolds >= reynoldsList.at(j) && g_360PolarStore.at(i)->reynolds <= reynoldsList.at(j+1)){
                            reynoldsList.insert(j+1, g_360PolarStore.at(i)->reynolds);
                            namesList.insert(j+1,g_360PolarStore.at(i)->getName());
                            break;
                        }
                    }
                    if ( j == reynoldsList.size()-1 && g_360PolarStore.at(i)->reynolds >= reynoldsList.at(reynoldsList.size()-1)){
                        reynoldsList.append(g_360PolarStore.at(i)->reynolds);
                        namesList.append(g_360PolarStore.at(i)->getName());
                        break;
                    }
                }
            }
            else{
            namesList.append(g_360PolarStore.at(i)->getName());
            reynoldsList.append(g_360PolarStore.at(i)->reynolds);
            }
        }
    }
    for (int i=0;i<reynoldsList.size();i++){
    lab = new QLabel(namesList.at(i));
    grid->addWidget(lab,gridrow,0);
    lab = new QLabel(QString("%1").arg(reynoldsList.at(i)));
    grid->addWidget(lab,gridrow,1);
    box = new QCheckBox;
    boxList.append(box);

    connect(box, SIGNAL(clicked()), this, SLOT(OnCheckBoxClicked()));
    connect(box, SIGNAL(stateChanged(int)), this, SLOT(OnCheckBoxClicked()));

    box->setChecked(false);
    if (i!=0){
    if (reynoldsList.at(i) > reynoldsList.at(i-1)) box->setChecked(true);
    }
    else box->setChecked(true);

    grid->addWidget(box,gridrow,2);

    gridrow++;
    }

    QV->addWidget(warning);
    QV->addWidget(accept);

    setLayout(QV);

    if (list.size()){
        C360Polar *polar = list.at(0);
        for (int i=0; i<boxList.size();i++){
                if (polar->getParent()->getParent()->getName() == pFoil->getName()){
                    boxList.at(i)->setChecked(false);
                    for (int k=0;k<list.size();k++){
                        if (list.at(k)->getName() == namesList.at(i)) boxList.at(i)->setChecked(true);
                    }
                }
        }
    }

}

void PolarSelectionDialog::OnCheckBoxClicked(){

    accept->setEnabled(true);
    warning->setText(tr(""));

    bool bOneSelected = false;
    for (int i=0;i<boxList.size();i++){
        if (boxList.at(i)->isChecked()) bOneSelected = true;
    }

    if (!bOneSelected){
        warning->setText("no Polar Selected");
        accept->setEnabled(false);
    }

    bool bDouble = false;
    for (int i=0;i<boxList.size();i++){
        if (boxList.at(i)->isChecked()){
            for (int j=0;j<boxList.size();j++){
                if (i!=j && boxList.at(j)->isChecked() && reynoldsList.at(i) == reynoldsList.at(j)) bDouble = true;
            }
        }
    }

    if (bDouble){
        warning->setText("only one Polar per Reynolds Number");
        accept->setEnabled(false);
    }

    for (int i=0;i<boxList.size();i++){
        if (boxList.at(i)->isChecked()){
            min = reynoldsList.at(i);
            break;
        }
    }

    for (int i=boxList.size()-1;i>=0;i--){
        if (boxList.at(i)->isChecked()){
            max = reynoldsList.at(i);
            break;
        }
    }

    clickedPolars.clear();
    for (int i=0;i<boxList.size();i++){
        if (boxList.at(i)->isChecked()){
            clickedPolars.append(namesList.at(i));
        }
    }
    //IMPORTANT: this List stores the polarnames in ascending order with respect to the reynolds number

}
