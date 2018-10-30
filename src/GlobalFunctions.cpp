#include "GlobalFunctions.h"

#include <QRegExp>
#include <QDir>
#include <QDebug>

#include "Store.h"
#include "Params.h"
#include "MainFrame.h"
#include "XLLT/QLLTModule.h"
#include "GLWidget.h"


extern bool ObjectIsEdited;

hsv rgb2hsv(rgb in)
{
	hsv         out;
	double      min, max, delta;
	
	min = in.r < in.g ? in.r : in.g;
	min = min  < in.b ? min  : in.b;
	
	max = in.r > in.g ? in.r : in.g;
	max = max  > in.b ? max  : in.b;
	
	out.v = max;                                // v
	delta = max - min;
	if( max > 0.0 ) {
		out.s = (delta / max);                  // s
	} else {
		// r = g = b = 0                        // s = 0, v is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if( in.r >= max )                           // > is bogus, just keeps compilor happy
		out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	else
		if( in.g >= max )
			out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan
	
	out.h *= 60.0;                              // degrees
	
	if( out.h < 0.0 )
		out.h += 360.0;
	
	return out;
}

rgb hsv2rgb(hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;
	
	if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));
	
	switch(i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;
		
	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

QString makeNameWithHigherNumber (QString oldName) {
	int number = 2;
	int position = oldName.lastIndexOf(QRegExp(" [(][0-9]+[)]$"));
	if (position >= 0) {  // read and truncate old number if existing
		number = oldName.mid(position+2, oldName.length()-position-3).toInt() + 1;
		oldName.truncate(position);
	}
	
	return QString(oldName + " (%1)").arg(number);
}

QString UnifyString(QString strong){
    strong.replace(" e","_e");
    strong.replace(" E","_E");
    strong.replace("e ","e_");
    strong.replace("E ","E_");
    strong.replace("\\- ","\\-_");
    strong.replace(" \\-","_\\-");
    strong.replace("\\+ ","\\+_");
    strong.replace(" \\+","_\\+");
    strong.replace("_","");
    strong.replace(",",".");

    return strong;
}

void printStoreState () {
	g_windFieldStore.printState();
	g_FASTSimulationStore.printState();
	g_foilStore.printState();
	g_polarStore.printState();
	g_360PolarStore.printState();
	g_rotorStore.printState();
	g_verticalRotorStore.printState();
	g_bladeStructureStore.printState();
	g_tdataStore.printState();
	g_verttdataStore.printState();
	g_bemdataStore.printState();
	g_tbemdataStore.printState();
	g_cbemdataStore.printState();
	g_dmsdataStore.printState();
	g_tdmsdataStore.printState();
	g_cdmsdataStore.printState();
	
	qDebug() << "-";
	qDebug() << "-";
	qDebug() << "-";	
}
