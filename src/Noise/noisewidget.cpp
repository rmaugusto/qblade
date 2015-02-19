#include "noisewidget.h"

NoiseWidget::NoiseWidget(QWidget *parent) :
    QWidget(parent)
{
    InitView();
}

void NoiseWidget::InitView(){

//    int w,h;

//    h = m_rCltRect.height();
//    w = m_rCltRect.width();
//    int w2  = (int)(w/2);
//    int h34  = (int)(3*h/4);

//    QRect CpRect(0,0,w,h34);


//    m_rSingleRect.setRect(40,10,m_rCltRect.right()-80,m_rCltRect.bottom()-230);


//    QRect Rect1(0,0,w2,m_rCltRect.bottom()-00);
//    QRect Rect2(w2,0,w2,m_rCltRect.bottom()-00);

    m_Noise2dGraph.SetXMajGrid(true, QColor(200,200,200),2,1);
    m_Noise2dGraph.SetYMajGrid(true, QColor(200,200,200),2,1);
    m_Noise2dGraph.SetXMin(-0.0);
    m_Noise2dGraph.SetXMax( 0.1);
    m_Noise2dGraph.SetYMin(-0.01);
    m_Noise2dGraph.SetYMax( 0.01);
    m_Noise2dGraph.SetType(0);
    m_Noise2dGraph.SetMargin(50);
    m_Noise2dGraph.SetXVariable(0);
    m_Noise2dGraph.SetYVariable(1);
    m_Noise2dGraph.SetGraphName(tr("Noise CL Graph"));



    QRect Rect1(0,0,20,20);
    QRect Rect2(0,0,20,20);

    m_Noise2dGraph.SetDrawRect(Rect1);
    m_Noise2dGraph.SetDrawRect(Rect2);

    m_Noise2dGraph.Init();

    m_Noise2dGraph.SetAutoXUnit();


}

void NoiseWidget::LoadSettings(QSettings *pSettings)
{
    m_Noise2dGraph.LoadSettings(pSettings);
}

void NoiseWidget::SaveSettings(QSettings *pSettings)
{
}

void NoiseWidget::PaintView(QPainter &painter)
{
    painter.fillRect(100,100,300,300, QColor(0, 0, 0) );
    painter.drawRect(10,10,200,200);
    //painter.drawText(100, 100, "HHHEYYYY");
}

void NoiseWidget::OnNoiseView(){

    g_mainFrame->setIView(NOISE_2DVIEW,NOISE);// new JW //m_iView = POLARVIEW;
    g_mainFrame->setIApp(NOISE);  // NM vorher: g_mainFrame->m_iApp = BEM;
/*
    int w  = m_rCltRect.width();
    int w2  = (int)(w/2);
    int h  = m_rCltRect.height();
    int h2  = (int)(h/2);

    m_360LegendOffset.rx() = w2 + 10;
    m_360LegendOffset.ry() = h2 + 30;

    if (!m_bNew360Polar) UpdateFoils();
    CheckButtons();
    UpdateView();

*/

    g_mainFrame->OnNoise();

    g_mainFrame->SetCentralWidget();



}
