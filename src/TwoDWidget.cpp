/****************************************************************************

	TwoDWidget Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "TwoDWidget.h"

#include "MainFrame.h"
#include "XDirect/XDirect.h"
#include "Design/AFoil.h"
#include "XInverse/XInverse.h"
#include "XBEM/BEM.h"
#include "XDMS/DMS.h"
#include "XUnsteadyBEM/FASTModule.h"


TwoDWidget::TwoDWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pXDirect   = NULL;
	m_pAFoil     = NULL;
	m_pBEM       = NULL;
	m_pDMS       = NULL;
	
    setAttribute(Qt::WA_AcceptTouchEvents, true);
	setMouseTracking(true);  // NM not really needed. Could be realized much more efficient
	setCursor(Qt::CrossCursor);
}

void TwoDWidget::keyPressEvent(QKeyEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyPressEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyPressEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyPressEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onKeyPressEvent(event);
	}
}

void TwoDWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyReleaseEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyReleaseEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyReleaseEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onKeyReleaseEvent(event);
	}
}

void TwoDWidget::mousePressEvent(QMouseEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mousePressEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mousePressEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mousePressEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onMousePressEvent(event);
	}
}


void TwoDWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseReleaseEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mouseReleaseEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mouseReleaseEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onMouseReleaseEvent(event);
	}
}


void TwoDWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseMoveEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mouseMoveEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mouseMoveEvent(event);
	}
	else if (g_mainFrame->m_iApp == BEM && m_pBEM)
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		pBEM->MouseMoveEvent(event);
	}
	else if (g_mainFrame->m_iApp == DMS && m_pDMS)
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		pDMS->MouseMoveEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onMouseMoveEvent(event);
	}
}

void TwoDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseDoubleClickEvent(event );
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mouseDoubleClickEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mouseDoubleClickEvent(event );
	}
	else if (g_mainFrame->m_iApp == BEM && m_pBEM)
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		pBEM->mouseDoubleClickEvent(event);
	}
	else if (g_mainFrame->m_iApp == DMS && m_pDMS)
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		pDMS->mouseDoubleClickEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onMouseDoubleClickEvent(event);
	}
}

void TwoDWidget::resizeEvent(QResizeEvent */*event*/)
{
	QXDirect  *pXDirect  = (QXDirect*)m_pXDirect;
	QAFoil    *pAFoil    = (QAFoil*)m_pAFoil;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	
	if(m_pXDirect)
	{
		pXDirect->SetFoilScale(rect());
		pXDirect->SetPolarLegendPos();
	}
	if(m_pAFoil)
	{
		pAFoil->SetScale(rect());
	}
	if(m_pXInverse)
	{
		pXInverse->SetScale(rect());
	}
	QBEM *pBEM = (QBEM *) m_pBEM;
	if (m_pBEM)
	{
		pBEM->m_bIs2DScaleSet=false;
		pBEM->SetScale(rect());
        pBEM->SetLegendPos();
	}
	QDMS *pDMS = (QDMS *) m_pDMS;
	if (m_pDMS)
	{
		pDMS->m_bIs2DScaleSet=false;
		pDMS->SetScale(rect());
        pDMS->SetLegendPos();
	}
	
	if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onResizeEvent();
	}
}

void TwoDWidget::wheelEvent(QWheelEvent *event)
{
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->wheelEvent(event);
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->wheelEvent(event);
	}
	else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->wheelEvent(event);
	}
	else if (g_mainFrame->m_iApp == BEM && m_pBEM)
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		pBEM->WheelEvent(event);
	}
	else if (g_mainFrame->m_iApp == DMS && m_pDMS)
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		pDMS->WheelEvent(event);
	} else if (g_mainFrame->getTwoDWidgetInterface()) {
		g_mainFrame->getTwoDWidgetInterface()->onWheelEvent(event);
	}
}

void TwoDWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(rect(), g_mainFrame->m_BackgroundColor);
	
	/* realize the message as overpaint */
	QStringList missingObjectMessages = g_mainFrame->prepareMissingObjectMessage();
	if (!missingObjectMessages.isEmpty()) {
	    const QFont missingObjectFont (g_mainFrame->m_TextFont.family(), 0.02*width());
	    const QFont hintFont (g_mainFrame->m_TextFont.family(), 0.015*width());

		painter.setFont(missingObjectFont);
		const int betweenTextGap = 0.08*height();
		QPoint textPosition = QPoint (0.15*width(), 0.15*height());
		for (int i = 0; i < missingObjectMessages.size() - 1; ++i) {
			painter.drawText(textPosition, missingObjectMessages[i]);
			textPosition.ry() += betweenTextGap;
		}
		painter.setFont(hintFont);
		painter.drawText(textPosition, missingObjectMessages.last());
	} else {
		if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
		{	
			QXDirect* pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->PaintView(painter);
		}
		else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
		{
			QAFoil *pAFoil= (QAFoil*)m_pAFoil;
			pAFoil->PaintView(painter);
		}
		else if(g_mainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
		{
			QXInverse *pXInverse= (QXInverse*)m_pXInverse;
			pXInverse->PaintView(painter);
		}
		else if(g_mainFrame->m_iApp == BEM && m_pBEM)
		{
			QBEM *pBEM = (QBEM *) m_pBEM;
			pBEM->PaintView(painter);
		}
		else if(g_mainFrame->m_iApp == DMS && m_pDMS)
		{
			QDMS *pDMS = (QDMS *) m_pDMS;
			pDMS->PaintView(painter);
		} else if (g_mainFrame->getTwoDWidgetInterface()) {
			g_mainFrame->getTwoDWidgetInterface()->onPaintEvent(event);
		}
        else{
            QImage image(":/images/logo_1000.png");
            int width = this->width();
            int height = this->height();

            painter.drawImage(QRectF(width/2-height/3,height/2-height/3,height/1.5,height/1.5),image);
            QFont font (g_mainFrame->m_TextFont.family(), 0.06*height);
            painter.setFont(font);
            QPoint textPosition = QPoint (width/2-height*0.12, height/2+height/2.5);
            painter.drawText(textPosition, "v0.96");

            QFont font2(g_mainFrame->m_TextFont.family(), 0.03*height);
            textPosition = QPoint (width/2-height*0.52, height/2-height/2.5);
            painter.setFont(font2);
            painter.drawText(textPosition, "Click on the Airfoil Design Module in the Toolbar to Start");
        }
	}
}

void TwoDWidget::contextMenuEvent (QContextMenuEvent *event)
{
	QPoint ScreenPt = event->globalPos();
	QPoint CltPt = event->pos();
	switch(g_mainFrame->m_iApp)
	{
	case XFOILANALYSIS:
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->m_pCurGraph = pXDirect->GetGraph(CltPt);
		if(pXDirect->m_bPolar) g_mainFrame->OperPolarCtxMenu->exec(ScreenPt);
		else                   g_mainFrame->OperFoilCtxMenu->exec(ScreenPt);
		break;
	}
	case DIRECTDESIGN:
	{
		g_mainFrame->AFoilCtxMenu->exec(ScreenPt);
		break;
	}
	case INVERSEDESIGN:
	{
		g_mainFrame->InverseContextMenu->exec(ScreenPt);
		break;
	}
	case BEM:
	{
		QBEM *pBEM = (QBEM *) m_pBEM;
		
		if (g_mainFrame->m_iView==CHARSIMVIEW)// new JW, old: pBEM->m_iView
		{
			pBEM->CheckButtons();
			g_mainFrame->CharCtxMenu->exec(ScreenPt);
		}
		else if (g_mainFrame->m_iView == POLARVIEW)// new JW, old: pBEM->m_iView
		{
			pBEM->CheckButtons();
			g_mainFrame->PolarCtxMenu->exec(ScreenPt);
		}
		else if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: pBEM->m_iView
		{
			pBEM->CheckButtons();
			g_mainFrame->BEMCtxMenu->exec(ScreenPt);
		}
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: pBEM->m_iView
		{
			pBEM->CheckButtons();
			g_mainFrame->TurbineCtxMenu->exec(ScreenPt);
		}
		break;
	}
	case DMS:
	{
		QDMS *pDMS = (QDMS *) m_pDMS;
		
		if (g_mainFrame->m_iView==CHARSIMVIEW)// new JW, old: pDMS->m_iView
		{
			pDMS->CheckButtons();
			g_mainFrame->CharCtxMenu->exec(ScreenPt);
		}
		if (g_mainFrame->m_iView == BEMSIMVIEW)// new JW, old: pDMS->m_iView
		{
			pDMS->CheckButtons();
			g_mainFrame->BEMCtxMenu->exec(ScreenPt);
		}
		else if (g_mainFrame->m_iView == TURBINEVIEW)// new JW, old: pDMS->m_iView
		{
			pDMS->CheckButtons();
			g_mainFrame->TurbineCtxMenu->exec(ScreenPt);
		}
		break;
	}
	default:
		if (g_mainFrame->getTwoDWidgetInterface()) {
			g_mainFrame->getTwoDWidgetInterface()->onContextMenuEvent(event);
		}
		break;
	}
}

void TwoDWidget::enterEvent(QEvent */*event*/) {
	setFocus();
}

void TwoDWidget::leaveEvent(QEvent */*event*/) {
	g_mainFrame->setFocus();
}
