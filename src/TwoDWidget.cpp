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
#include "Miarex/Miarex.h"
#include "Design/AFoil.h"
#include "XInverse/XInverse.h"
#include "XBEM/BEM.h"
#include "XDMS/DMS.h"
#include "XUnsteadyBEM/FASTModule.h"


TwoDWidget::TwoDWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pXDirect   = NULL;
	m_pMiarex    = NULL;
	m_pAFoil     = NULL;
	m_pBEM       = NULL;
	m_pDMS       = NULL;
	
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyPressEvent(event);
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyReleaseEvent(event);
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mousePressEvent(event);
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseReleaseEvent(event);
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseMoveEvent(event);
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseDoubleClickEvent(event );
	}
	else if(g_mainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
//		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
//		pAFoil->SetScale(rect());
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
	QMiarex   *pMiarex   = (QMiarex*)m_pMiarex;
	QAFoil    *pAFoil    = (QAFoil*)m_pAFoil;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	
	if(m_pXDirect)
	{
		pXDirect->SetFoilScale(rect());
		pXDirect->SetPolarLegendPos();
	}
	if(m_pMiarex)
	{
		pMiarex->m_bIs2DScaleSet = false;
		pMiarex->Set2DScale();
		if(pMiarex->m_iView==WOPPVIEW)   pMiarex->SetWingLegendPos();
		if(pMiarex->m_iView==WPOLARVIEW) pMiarex->SetWPlrLegendPos();
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
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->wheelEvent(event);
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
	
	if(g_mainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{	
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->PaintView(painter);
	}
	else if(g_mainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->PaintView(painter);
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
	else
	{
		painter.fillRect(rect(), g_mainFrame->m_BackgroundColor);
	}
}

void TwoDWidget::contextMenuEvent (QContextMenuEvent *event)
{
	QPoint ScreenPt = event->globalPos();
	QPoint CltPt = event->pos();
	switch(g_mainFrame->m_iApp)
	{
	case MIAREX:
	{
		QMiarex *pMiarex = (QMiarex*)m_pMiarex;
		if(event->reason()==QContextMenuEvent::Keyboard)
		{
			ScreenPt.rx() = pMiarex->m_LastPoint.x()+g_mainFrame->pos().x()+geometry().x();
			ScreenPt.ry() = pMiarex->m_LastPoint.y()+g_mainFrame->pos().y()+geometry().y();
		}
		
		pMiarex->m_pCurGraph = pMiarex->GetGraph(CltPt);
		if(pMiarex->m_iView==WOPPVIEW)         g_mainFrame->WOppCtxMenu->exec(ScreenPt);
		else if (pMiarex->m_iView==WPOLARVIEW) g_mainFrame->WPlrCtxMenu->exec(ScreenPt);
		else if (pMiarex->m_iView==WCPVIEW)    g_mainFrame->WCpCtxMenu->exec(ScreenPt);
		else if(pMiarex->m_iView==WSTABVIEW)
		{
			if(pMiarex->m_iStabilityView==STABTIMEVIEW)       g_mainFrame->WTimeCtxMenu->exec(ScreenPt);
			else if(pMiarex->m_iStabilityView==STABPOLARVIEW) g_mainFrame->WPlrCtxMenu->exec(ScreenPt);
		}
		break;
	}
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
