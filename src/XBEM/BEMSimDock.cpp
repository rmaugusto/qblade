/****************************************************************************

	BEMSimDock Class
		Copyright (C) 2013 Juliane Wendler

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

#include "BEMSimDock.h"
#include "../Globals.h"
#include "SimuWidget.h"


BEMSimDock::BEMSimDock (const QString & title, QMainWindow * parent, Qt::WindowFlags flags)
	: ScrolledDock (title, parent, flags)
{
	g_mainFrame->m_pSimuWidget = new SimuWidget(g_mainFrame);
	SimuWidget *pSimuWidget = (SimuWidget *) g_mainFrame->m_pSimuWidget;
	pSimuWidget->setAttribute(Qt::WA_DeleteOnClose, false);
	pSimuWidget->m_pBEM = g_mainFrame->m_pBEM;

	m_contentVBox->addWidget(pSimuWidget);
	//setWidget(pSimuWidget);
	setVisible(false);

	addScrolledDock(Qt::RightDockWidgetArea , parent);
}
