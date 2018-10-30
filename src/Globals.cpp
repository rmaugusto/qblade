/****************************************************************************

	Globals Class
	Copyright (C) 2008-2011 Andre Deperrois adeperrois@xflr5.com

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

#include "Globals.h"

#include <QTextStream>
#include <QColor>
#include <cmath>
#include <QDataStream>

#include "MainFrame.h"


void GetAreaUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm"+QString::fromUtf8("²");
			break;
		}
		case 1:
		{
			str="cm"+QString::fromUtf8("²");
			break;
		}
		case 2:
		{
			str="dm"+QString::fromUtf8("²");
			break;
		}
		case 3:
		{
			str="m"+QString::fromUtf8("²");
			break;
		}
		case 4:
		{
			str="in"+QString::fromUtf8("²");
			break;
		}
		case 5:
		{
			str="ft"+QString::fromUtf8("²");
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}

void GetLengthUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm";
			break;
		}
		case 1:
		{
			str="cm";
			break;
		}
		case 2:
		{
			str="dm";
			break;
		}
		case 3:
		{
			str="m";
			break;
		}
		case 4:
		{
			str="in";
			break;
		}
		case 5:
		{
			str="ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}

void GetForceUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:{
			str="N";
			break;
		}
		case 1:{
			str="lbf";
			break;
		}

		default:{
			str=" ";
			break;
		}
	}
}

void GetMomentUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="N.m";
			break;
		}
		case 1:
		{
			str="lbf.in";
			break;
		}
		case 2:
		{
			str="lbf.ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}

/////////////new code DM//////////////
void GetPowerUnit(QString &str, int unit)
{
        switch(unit)
        {
                case 0:
                {
                        str="MW";
                        break;
                }
                case 1:
                {
                        str="kW";
                        break;
                }
                case 2:
                {
                        str="W";
                        break;
                }
                default:
                {
                        str=" ";
                        break;
                }
        }
}
/////////////////end new code DM//////////////

void GetSpeedUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="m/s";
			break;
		}
		case 1:{
			str="km/h";
			break;
		}
		case 2:{
			str="ft/s";
			break;
		}
		case 3:{
			str="kt";
			break;
		}
		case 4:{
			str="mph";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

void GetWeightUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="g";
			break;
		}
		case 1:{
			str="kg";
			break;
		}
		case 2:{
			str="oz";
			break;
		}
		case 3:{
			str="lb";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

Qt::PenStyle GetStyle(int s)
{
	if(s==0)      return Qt::SolidLine;
	else if(s==1) return Qt::DashLine;
	else if(s==2) return Qt::DotLine;
	else if(s==3) return Qt::DashDotLine;
	else if(s==4) return Qt::DashDotDotLine;
	return Qt::SolidLine;
}

int GetStyleRevers (Qt::PenStyle s)
{
	if(s==Qt::SolidLine)      return 0;
	else if(s==Qt::DashLine) return 1;
	else if(s==Qt::DotLine) return 2;
	else if(s==Qt::DashDotLine) return 3;
	else if(s==Qt::DashDotDotLine) return 4;
	return 0;
}

bool ReadAVLString(QTextStream &in, int &Line, QString &strong)
{
	bool bComment = true;
	int pos;

	while(bComment && !in.atEnd())
	{
		bComment = false;

		strong = in.readLine();
		if(in.atEnd()) return false;

		strong = strong.trimmed();
		pos = strong.indexOf("#",0);
		if(pos>=0) strong = strong.left(pos);
		pos = strong.indexOf("!",0);
		if(pos>=0) strong = strong.left(pos);

		if(strong.isEmpty()) bComment = true;

		Line++;
	}

	if(in.atEnd())
	{
		return false;
	}
	return true;
}

void ReadCOLORREF(QDataStream &ar, QColor &color)
{
	qint32 colorref;
	int r,g,b;

	ar >> colorref;
	b = (int)(colorref/256/256);
	colorref -= b*256*256;
	g = (int)(colorref/256);
	r = colorref - g*256;
	color = QColor(r,g,b,255);

}

void ReadCString(QDataStream &ar, QString &strong)
{
	qint8 qi, ch;
	char c;

	ar >> qi;
	strong.clear();
	for(int j=0; j<qi;j++)
	{
		strong += " ";
		ar >> ch;
		c = char(ch);
		strong[j] = c;
	}
}

void ReynoldsFormat(QString &str, double f)
{
	int i, q, r, exp;
	f = (int(f/1000.0))*1000.0;

	exp = (int)log10(f);
	r = exp%3;
	q = (exp-r)/3;

	QString strong;
	strong = QString("%1").arg(f,0,'f',0);

	int l = strong.length();

	for (i=0; i<q; i++){
		strong.insert(l-3*(i+1)-i," ");
		l++;
	}

	for (i=strong.length(); i<9; i++){
		strong = " "+strong;
	}

	str = strong;
}

void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit, int PUnit, double &mtoUnit,
			  double &m2toUnit, double &mstoUnit,  double &kgtoUnit, double &NtoUnit, double &NmtoUnit, double &WtoUnit)
{
	switch(LUnit)
	{
		case 0:
		{//mdm
			mtoUnit  = 1000.0;
			break;
		}
		case 1:{//cm
			mtoUnit  = 100.0;
			break;
		}
		case 2:{//dm
			mtoUnit  = 10.0;
			break;
		}
		case 3:{//m
			mtoUnit  = 1.0;
			break;
		}
		case 4:{//in
			mtoUnit  = 1000.0/25.4;
			break;
		}
		case 5:{///ft
			mtoUnit  = 1000.0/25.4/12.0;
			break;
		}
		default:{//m
			mtoUnit  = 1.0;
			break;
		}
	}
	switch(AUnit)
	{
		case 0:{//mm²
			m2toUnit = 1000000.0;
			break;
		}
		case 1:{//cm²
			m2toUnit = 10000.0;
			break;
		}
		case 2:{//dm²
			m2toUnit = 100.0;
			break;
		}
		case 3:{//m²
			m2toUnit = 1.0;
			break;
		}
		case 4:{//in²
			m2toUnit = 1./0.254/0.254*100.0;
			break;
		}
		case 5:{//ft²
			m2toUnit = 1./0.254/0.254/144.0*100.0;
			break;
		}
		default:{
			m2toUnit = 1.0;
			break;
		}
	}

	switch(WUnit){
		case 0:{///g
			kgtoUnit = 1000.0;
			break;
		}
		case 1:{//kg
			kgtoUnit = 1.0;

			break;
		}
		case 2:{//oz
			kgtoUnit = 1./ 2.83495e-2;
			break;
		}
		case 3:{//lb
			kgtoUnit = 1.0/0.45359237;
			break;
		}
		default:{
			kgtoUnit = 1.0;
			break;
		}
	}
	switch(SUnit){
		case 0:{// m/s
			mstoUnit = 1.0;
			break;
		}
		case 1:{// km/h
			mstoUnit = 3600.0/1000.0;
			break;
		}
		case 2:{// ft/s
			mstoUnit = 100.0/2.54/12.0;
			break;
		}
		case 3:{// kt (int.)
			mstoUnit = 1.0/0.514444;
			break;
		}
		case 4:{// mph
			mstoUnit = 3600.0/1609.344;
			break;
		}
		default:{
			mstoUnit = 1.0;
			break;
		}
	}

	switch(FUnit){
		case 0:{//N
			NtoUnit = 1.0;
			break;
		}
		case 1:{//lbf
			NtoUnit = 1.0/4.44822;
			break;
		}
		default:{
			NtoUnit = 1.0;
			break;
		}
	}
	switch(MUnit){
		case 0:{//N.m
			NmtoUnit = 1.0;
			break;
		}
		case 1:{//lbf.in
			NmtoUnit = 1.0/4.44822/0.0254;
			break;
		}
		case 2:{//lbf.0t
			NmtoUnit = 1.0/4.44822/12.0/0.0254;
			break;
		}
		default:{
			NmtoUnit = 1.0;
			break;
		}
	}
        //////////////////new code DM////////////////
        switch(PUnit){
				case 0:{//MW
                        WtoUnit = 1.0/1000000;
                        break;
                }
                case 1:{//kW
                        WtoUnit = 1.0/1000;
                        break;
                }
				case 2:{//W
                        WtoUnit = 1.0;
                        break;
                }
                default:{
                        WtoUnit = 1.0;
                        break;
                }
        }
        ////////////////end new code DM///////////////
}

void WriteCOLORREF(QDataStream &ar, QColor const &color)
{
	qint32 colorref;
	int r,g,b;

	color.getRgb(&r,&g,&b);

	colorref = b*256*256+g*256+r;
	ar << colorref;
}

void WriteCString(QDataStream &ar, QString const &strong)
{
	qint8 qi = strong.length();

	QByteArray textline;
	char *text;
    textline = strong.toLatin1();
	text = textline.data();
	ar << qi;
	ar.writeRawData(text, qi);
}

void ReadValues(QString line, int &res, double &x, double &y, double &z)
{
	QString str;
	bool bOK;

	line = line.simplified();
	int pos = line.indexOf(" ");
	res = 0;
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	x = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		y=z=0.0;
		return;
	}

	line = line.trimmed();
	pos = line.indexOf(" ");
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	y = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		z=0.0;
		return;
	}

	line = line.trimmed();
	if(!line.isEmpty())
	{
		z = line.toDouble(&bOK);
		if(bOK) res++;
	}
	else z=0.0;
}

QString getUnitName(Unit unit) {  // NM TODO set up small class for Unit (QString name, double factor) and make const lists
	QString unitString;
	switch (unit) {
	case LENGTH: GetLengthUnit(unitString, g_mainFrame->m_LengthUnit); break;
	case AREA: GetAreaUnit(unitString, g_mainFrame->m_AreaUnit); break;
	case WEIGHT: GetWeightUnit(unitString, g_mainFrame->m_WeightUnit); break;
	case SPEED: GetSpeedUnit(unitString, g_mainFrame->m_SpeedUnit); break;
	case FORCE: GetForceUnit(unitString, g_mainFrame->m_ForceUnit); break;
	case MOMENT: GetMomentUnit(unitString, g_mainFrame->m_MomentUnit); break;
	case POWER: GetPowerUnit(unitString, g_mainFrame->m_PowerUnit); break;
	case PERCENT: return "%"; break;
	default: return "";
	}
	return unitString;
}

double getUnitFactor(Unit unit) {
	switch (unit) {
	case LENGTH: return g_mainFrame->m_mtoUnit;
	case AREA: return g_mainFrame->m_m2toUnit;
	case WEIGHT: return g_mainFrame->m_kgtoUnit;
	case SPEED: return g_mainFrame->m_mstoUnit;
	case FORCE: return g_mainFrame->m_NtoUnit;
	case MOMENT: return g_mainFrame->m_NmtoUnit;
	case POWER: return g_mainFrame->m_WtoUnit;
	case PERCENT: return 100;
	default: return 1;
	}
}

