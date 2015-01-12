#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QString>
#include "XGlobals.h"
#include "GLWidget.h"
#include "TwoDWidget.h"


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
bool removeDir(const QString & dirName);  // QDir lacks a function like this before QT 5
QString UnifyString(QString strong);

// TEST
void printStoreState ();
bool GLscreenMessage(int iApp, int iView, GLWidget *glWidget);
bool TwoDscreenMessage(int iApp, int iView, QPainter &painter, TwoDWidget *twodwidget);

hsv      rgb2hsv(rgb in);
rgb      hsv2rgb(hsv in);


#endif // GLOBALFUNCTIONS_H
