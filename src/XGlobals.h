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

#ifndef XGLOBALS_H
#define XGLOBALS_H

class QDMS;
class QFEMModule;
class QLLTModule;
class NoiseModule;


extern QDMS *g_qdms;								/**< global pointer to the QDMS module **/
extern QFEMModule *g_QFEMModule;					/**< global pointer to the QFEMModule **/
extern QLLTModule *g_QLLTModule;					/**< global pointer to the QLLTModule **/
extern NoiseModule *g_NoiseModule;

#endif // XGLOBALS_H
