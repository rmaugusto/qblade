#ifndef NOISEWIDGET_H
#define NOISEWIDGET_H

#include <QtWidgets>
#include <QWidget>
#include "../GLWidget.h"
#include "../MainFrame.h"

class NoiseWidget : public QWidget
{

    friend class MainFrame;
    friend class DisplaySettingsDlg;



    Q_OBJECT
public:
    void LoadSettings(QSettings *pSettings);
    void SaveSettings(QSettings *pSettings);
    void InitView();

    explicit NoiseWidget(QWidget *parent = 0);

    virtual void PaintView(QPainter &painter);

private:

    QGraph m_Noise2dGraph;
    QPoint m_Noise2dOffset;

signals:

public slots:
    virtual void OnNoiseView();

};

#endif // NOISEWIDGET_H
