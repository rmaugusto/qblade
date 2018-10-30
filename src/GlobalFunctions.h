#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QString>


typedef struct {
	double r;       // percent
	double g;       // percent
	double b;       // percent
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // percent
	double v;       // percent
} hsv;

QString makeNameWithHigherNumber (QString oldName);
QString UnifyString(QString strong);

void printStoreState ();  // debug function

hsv      rgb2hsv(rgb in);
rgb      hsv2rgb(hsv in);


#endif // GLOBALFUNCTIONS_H
