/****************************************************************************

	Additional Globals for QBlade
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

#include "XGlobals.h"
#include "Serializer.h"
class MainFrame;
class WindFieldModule;
class FASTModule;


MainFrame *g_mainFrame;
QDMS *g_qdms;
WindFieldModule *g_windFieldModule;
FASTModule *g_fastModule;
QFEMModule *g_QFEMModule;
QLLTModule *g_QLLTModule;
NoiseModule *g_NoiseModule;


Serializer g_serializer;

