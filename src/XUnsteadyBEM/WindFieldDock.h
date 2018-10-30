/****************************************************************************

	WindFieldCreator Class
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

#ifndef WINDFIELDDOCK_H
#define WINDFIELDDOCK_H

#include "../CreatorDock.h"
#include "../ParameterViewer.h"
class WindFieldModule;
class WindField;


class WindFieldDock : public CreatorDock<WindField>, public ParameterViewer<Parameter::Windfield>
{
	Q_OBJECT
	
public:
	WindFieldDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, WindFieldModule *module);
	
	void setShownObject (WindField *newObject);

private:
	WindFieldModule *m_module;

private slots:
	void onUnitsChanged ();
	void onEditCopyButtonClicked ();
	void onRenameButtonClicked ();
	void onDeleteButtonClicked ();
	void onNewButtonClicked ();
};

#endif // WINDFIELDDOCK_H
