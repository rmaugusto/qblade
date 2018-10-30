#ifndef ABOUTFAST_H
#define ABOUTFAST_H

#include <QDialog>
#include <QLabel>

class AboutFAST : public QDialog
{
    Q_OBJECT

public:
        explicit AboutFAST(void *parent = 0);
        virtual ~AboutFAST();


private:
        void SetupLayout();

        QPushButton *OKButton;
        QLabel * m_pXFLR5Link, *m_pQBladeLink;
        QLabel *m_NRELIcon;
};

#endif // ABOUTFAST_H
