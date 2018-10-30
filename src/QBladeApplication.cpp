/****************************************************************************

    XFLR5Application  Class
    Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com
                       Francesco Meschia francesco.meschia@gmail.com

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

#include "QBladeApplication.h"

#include <QSplashScreen>
#include <QSettings>

#include "MainFrame.h"
#include "Module.h"
#include "XBEM/BEM.h"
#include "XDMS/DMS.h"


QBladeApplication::QBladeApplication(int &argc, char** argv) : QApplication(argc, argv) {
	QSplashScreen splashScreen (QPixmap(":/images/qblade600.png"), Qt::SplashScreen);
	splashScreen.show();
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	setApplicationStyle(settings.value("StartUp/styleName", style()->objectName()).toString());
    setApplicationName("QBlade");
    setApplicationVersion("v0.95");
    setOrganizationName("ISTA - TU Berlin");
    setOrganizationDomain("http://fd.tu-berlin.de/");
	
	new MainFrame();
	
	if (settings.value("StartUp/windowState").toByteArray().isEmpty()) {
		settings.setValue("StartUp/windowState", g_mainFrame->saveState());  // required to make DockWidgets resizable
	}
	g_mainFrame->restoreState(settings.value("StartUp/windowState").toByteArray());
	g_mainFrame->restoreGeometry(settings.value("StartUp/windowGeometry").toByteArray());
	g_mainFrame->show();

	if (argc == 2) {  // argv[0] -> programm path; argv[1] -> first parameter
		g_mainFrame->loadQBladeProject(QString(argv[1]));
	}
	
	splashScreen.finish(g_mainFrame);
}

QBladeApplication::~QBladeApplication() {
	if (g_mainFrame->getCurrentModule() == NULL) {  // hide the current module to not make its docks appear on next start up
		if (g_mainFrame->m_iApp == BEM) {
			g_qbem->onModuleChanged();
		} else if (g_mainFrame->m_iApp == DMS) {
			g_qdms->onModuleChanged();
		} else {
			g_mainFrame->onModuleChanged();
		}
	} else {
		g_mainFrame->getCurrentModule()->onModuleChanged();
	}
	
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "QBLADE");
	settings.setValue("StartUp/windowGeometry", g_mainFrame->saveGeometry());
	settings.setValue("StartUp/windowState", g_mainFrame->saveState());
	settings.setValue("StartUp/styleName", m_styleName);
	
	delete g_mainFrame;
}

void QBladeApplication::setApplicationStyle (QString style) {
	m_styleName = style;
	setStyle(style);
}

QString QBladeApplication::getApplicationStyle() {
	return m_styleName;
}
