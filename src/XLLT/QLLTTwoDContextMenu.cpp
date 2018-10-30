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

#include "QLLTTwoDContextMenu.h"
#include <QAction>
#include "QLLTModule.h"
#include "../MainFrame.h"

QLLTTwoDContextMenu::QLLTTwoDContextMenu(QWidget *parent, QLLTModule *module)
    : TwoDContextMenu (parent, module)
{
    m_module = module;

    addSeparator();
    m_setGraphTypeTimeAction = new QAction (tr("Time Graph"), this);
    m_setGraphTypeTimeAction->setCheckable(true);
    connect(m_setGraphTypeTimeAction, SIGNAL(triggered()), this, SLOT(changeToTimeGraph()));
    addAction(m_setGraphTypeTimeAction);
    m_setGraphTypeBladeAction = new QAction (tr("Blade Graph"), this);
    m_setGraphTypeBladeAction->setCheckable(true);
    connect(m_setGraphTypeBladeAction, SIGNAL(triggered()), this, SLOT(changeToBladeGraph()));
    addAction(m_setGraphTypeBladeAction);
}

void QLLTTwoDContextMenu::setCurrentGraphType(NewGraph::GraphType graphType, NewGraph::GraphType /*graphTypeMulti*/) {
    m_setGraphTypeTimeAction->setChecked(graphType == NewGraph::QLLTTimeGraph);
    m_setGraphTypeBladeAction->setChecked(graphType == NewGraph::QLLTBladeGraph);
}
