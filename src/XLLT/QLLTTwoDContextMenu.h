/****************************************************************************

    QLLTTwoDContextMenu Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

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

#ifndef QLLTTWODCONTEXTMENU_H
#define QLLTTWODCONTEXTMENU_H

class QAction;

#include "../TwoDContextMenu.h"
#include "QLLTModule.h"


class QLLTTwoDContextMenu : public TwoDContextMenu
{
    Q_OBJECT

public:
    QLLTTwoDContextMenu(QWidget *parent, QLLTModule *module);

	void setCurrentGraphType (NewGraph::GraphType graphType, NewGraph::GraphType graphTypeMulti = NewGraph::None);

private:
    QLLTModule *m_module;
    QAction *m_setGraphTypeTimeAction, *m_setGraphTypeBladeAction;

private slots:
    void changeToTimeGraph () { m_module->changeGraphType(NewGraph::QLLTTimeGraph); }
    void changeToBladeGraph () { m_module->changeGraphType(NewGraph::QLLTBladeGraph); }
};


#endif // QLLTTWODCONTEXTMENU_H
