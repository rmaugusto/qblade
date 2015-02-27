/****************************************************************************

	Additional MainFrame Functions for QBlade
		Copyright (C) 2012 Nikolai Moesus

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

#include "MainFrame.h"
#include <QToolBar>
#include <QDockWidget>
#include <QAction>
#include <QApplication>
#include "XGlobals.h"
#include "ScrolledDock.h"
#include "Module.h"
#include "TwoDWidgetEventInterface.h"


void MainFrame::setIApp (int iApp) {
	if (m_iApp != iApp) {
		emit moduleChanged ();
		m_iApp = iApp;
	}
	
	if (iApp == XFOILANALYSIS || iApp == DIRECTDESIGN || iApp == INVERSEDESIGN || iApp == MIAREX
                                    || iApp == BEM || iApp == DMS || iApp == NOISE_MODULE) {
		setCurrentModule(NULL);  // is needed, because these modules are not of class Module
	}
}

void MainFrame::onModuleChanged () {
	if (g_mainFrame->m_currentModule == NULL) {
		m_pctrlMiarexToolBar->hide();
		m_pctrlStabViewWidget->hide();
		m_pctrlXDirectToolBar->hide();
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
	
		m_pctrlMiarexWidget->hide();
		m_pctrlXDirectWidget->hide();
		m_pctrlXInverseWidget->hide();
		m_pctrlAFoilWidget->hide();
		
		OnAFoilAct->setChecked(false);
		OnXDirectAct->setChecked(false);
		OnMiarexAct->setChecked(false);
		OnXInverseAct->setChecked(false);
	}
}

void MainFrame::setCurrentModule(ModuleBase *newModule) {
	m_currentModule = newModule;
}

void MainFrame::setIView (int newView, int newApp)
{
	emit viewChanged (newView, newApp);
	m_iView = newView;
}

void MainFrame::switchToTwoDWidget() {
	m_twoDWidgetInterface = dynamic_cast<TwoDWidgetInterface*> (m_currentModule);
	m_centralWidget->setCurrentIndex(0);
}

void MainFrame::onViewChanged (int newView, int newApp)
{

	if (m_iView!=newView || (m_iView==newView && m_iApp!=newApp))
	{
		if (m_iView==BLADEVIEW)// old view
		{
			if (m_iApp == BEM)// old App
			{
				m_pctrlBEMWidget->hide();
				BladeDockSize.setWidth(m_pctrlBEMWidget->m_storedSize.width());
                m_pctrlBEMWidget->show();

			}
			else if (m_iApp == DMS)
			{
				m_pctrlDMSWidget->hide();
                VBladeDockSize.setWidth(m_pctrlDMSWidget->m_storedSize.width());
                m_pctrlDMSWidget->show();

			}
		}
		else if (m_iView==TURBINEVIEW)
		{
			if (m_iApp == BEM)
			{
				m_pctrlBEMWidget->hide();
				TurbineDockSize.setWidth(m_pctrlBEMWidget->m_storedSize.width());
                m_pctrlBEMWidget->show();

			}
			else if (m_iApp == DMS)
			{
				m_pctrlDMSWidget->hide();
				TurbineDockSize.setWidth(m_pctrlDMSWidget->m_storedSize.width());
                m_pctrlDMSWidget->show();

			}
		}

	}
    else
	{
		if (newView == POLARVIEW)
		{
			m_pctrlBEMWidget->setMinimumWidth(220);
			m_pctrlBEMWidget->setMaximumWidth(220);

			qApp->processEvents();  // needed to avoid optimization that would skip size change

//            m_pctrlBEMWidget->setMinimumWidth(0);
//            m_pctrlBEMWidget->setMaximumWidth(9999);

		}
		if (newView == BLADEVIEW)
		{
				m_pctrlBEMWidget->setMinimumWidth(BladeDockSize.width());//500
				m_pctrlBEMWidget->setMaximumWidth(BladeDockSize.width());

                m_pctrlDMSWidget->setMinimumWidth(VBladeDockSize.width());
                m_pctrlDMSWidget->setMaximumWidth(VBladeDockSize.width());

                qApp->processEvents();  // needed to avoid optimization that would skip size change

//                m_pctrlBEMWidget->setMinimumWidth(0);
//                m_pctrlBEMWidget->setMaximumWidth(9999);

//                m_pctrlDMSWidget->setMinimumWidth(0);
//                m_pctrlDMSWidget->setMaximumWidth(9999);

		}
		else if (newView == TURBINEVIEW)
		{
				m_pctrlBEMWidget->setMinimumWidth(TurbineDockSize.width());//320
				m_pctrlBEMWidget->setMaximumWidth(TurbineDockSize.width());

                m_pctrlSimuWidget->setMinimumWidth(290);
                m_pctrlSimuWidget->setMaximumWidth(290);

				m_pctrlDMSWidget->setMinimumWidth(TurbineDockSize.width());
				m_pctrlDMSWidget->setMaximumWidth(TurbineDockSize.width());

                m_pctrlSimuWidgetDMS->setMinimumWidth(290);
                m_pctrlSimuWidgetDMS->setMaximumWidth(290);

                qApp->processEvents();  // needed to avoid optimization that would skip size change

//                m_pctrlBEMWidget->setMinimumWidth(0);
//                m_pctrlBEMWidget->setMaximumWidth(9999);

//                m_pctrlDMSWidget->setMinimumWidth(0);
//                m_pctrlDMSWidget->setMaximumWidth(9999);
		}
        else if (newView == CHARSIMVIEW)
        {
            m_pctrlSimuWidget->setMinimumWidth(290);
            m_pctrlSimuWidget->setMaximumWidth(290);


            m_pctrlSimuWidgetDMS->setMinimumWidth(290);
            m_pctrlSimuWidgetDMS->setMaximumWidth(290);

            qApp->processEvents();  // needed to avoid optimization that would skip size change

        }
        else if (newView == BEMSIMVIEW){
            m_pctrlSimuWidget->setMinimumWidth(290);
            m_pctrlSimuWidget->setMaximumWidth(290);


            m_pctrlSimuWidgetDMS->setMinimumWidth(290);
            m_pctrlSimuWidgetDMS->setMaximumWidth(290);

            qApp->processEvents();  // needed to avoid optimization that would skip size change

        }
	}
}
