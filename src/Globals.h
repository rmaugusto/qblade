/****************************************************************************

	Global Functions 
	Copyright (C) 2008-2010 Andre Deperrois adeperrois@xflr5.com

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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
class QTextStream;
class QColor;


bool ReadAVLString(QTextStream &in, int &Line, QString &strong);

void ReadValues(QString line, int &res, double &x, double &y, double &z);

void ReynoldsFormat(QString &str, double f);

void GetSpeedUnit(QString &str, int unit);
void GetWeightUnit(QString &str, int unit);
void GetAreaUnit(QString &str, int unit);
void GetLengthUnit(QString &str, int unit);
void GetMomentUnit(QString &str, int unit);
void GetForceUnit(QString &str, int unit);
//////////new code DM///////////
void GetPowerUnit(QString &str, int unit);
//////////////end new code DM//////////

void ReadCString(QDataStream &ar, QString &strong);
void WriteCString(QDataStream &ar, QString const &strong);
void ReadCOLORREF(QDataStream &ar, QColor &color);
void WriteCOLORREF(QDataStream &ar, QColor const &color);

void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit, int PUnit,
                          double &mtoUnit, double &m2toUnit, double &mstoUnit,  double &kgtoUnit,
                          double &NtoUnit, double &NmtoUnit, double &WtoUnit);

Qt::PenStyle GetStyle(int s);
int GetStyleRevers (Qt::PenStyle s);

enum Unit {NONE, LENGTH, AREA, WEIGHT, SPEED, FORCE, MOMENT, POWER, PERCENT};
QString getUnitName (Unit unit);  // wrappers for the super inconvenient functions
double getUnitFactor (Unit unit);

#endif // FUNCTIONS_H
