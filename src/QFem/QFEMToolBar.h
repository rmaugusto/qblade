/****************************************************************************

    QFEMToolBar Class
        Copyright (C) 2014 David Marten david.marten@tu-berlin.de

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

#ifndef QFEMTOOLBAR_H
#define QFEMTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include "QFEMModule.h"
#include "../XBEM/Blade.h"
#include "BladeStructure.h"
#include "../StoreAssociatedComboBox_include.h"


class QFEMToolBar : public QToolBar
{
	friend class QFEMModule;
    friend class QFEMDock;
	Q_OBJECT

public:
	QFEMToolBar (QMainWindow *parent, QFEMModule *module);
    QAction *TwoDView, *HideWidgetsAct, *GLView;

private:
    QFEMModule *m_module;

	RotorComboBox *m_rotorComboBox;
	BladeStructureComboBox *m_BladeStructureComboBox;
    BladeStructureLoadingComboBox *m_BladeStructureLoadingComboBox;

};

#endif // QFEMTOOLBAR_H
