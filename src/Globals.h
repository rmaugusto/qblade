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

#include <complex>
#include <QString>
class QTextStream;
class QColor;

#include "Store_include.h"


bool IsEven(int n);
bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool ReadAVLString(QTextStream &in, int &Line, QString &strong);
bool Rewind1Line(QTextStream &in, int &Line, QString &strong);

void ReadValues(QString line, int &res, double &x, double &y, double &z);

void ExpFormat(double &f, int &exp);
void ReynoldsFormat(QString &str, double f);

bool Gauss(double *A, int n, double *B, int m, bool *pbCancel);

void GetSpeedUnit(QString &str, int unit);
void GetWeightUnit(QString &str, int unit);
void GetAreaUnit(QString &str, int unit);
void GetLengthUnit(QString &str, int unit);
void GetMomentUnit(QString &str, int unit);
void GetForceUnit(QString &str, int unit);
//////////new code DM///////////
void GetPowerUnit(QString &str, int unit);
//////////////end new code DM//////////

double GLGetRed(double tau);
double GLGetGreen(double tau);
double GLGetBlue(double tau);

double IntegralC2(double y1, double y2, double c1, double c2);
double IntegralCy(double y1, double y2, double c1, double c2);

void ReadCString(QDataStream &ar, QString &strong);
void WriteCString(QDataStream &ar, QString const &strong);
void ReadCOLORREF(QDataStream &ar, QColor &color);
void WriteCOLORREF(QDataStream &ar, QColor const &color);

void Trace(int n);
void Trace(QString msg);
void Trace(QString msg, int n);
void Trace(QString msg, double f);

void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit, int PUnit,
                          double &mtoUnit, double &m2toUnit, double &mstoUnit,  double &kgtoUnit,
                          double &NtoUnit, double &NmtoUnit, double &WtoUnit);

Qt::PenStyle GetStyle(int s);
int GetStyleRevers (Qt::PenStyle s);


double Det44(double *aij);
double Det33(double *aij);
std::complex<double> Det33(std::complex<double> *aij);
std::complex<double> Det44(std::complex<double> *aij);
std::complex<double> Cofactor44(std::complex<double> *aij, int &i, int &j);
bool Invert44(std::complex<double> *ain, std::complex<double> *aout);


void TestEigen();
void CharacteristicPol(double m[][4], double p[5]);
bool LinBairstow(double *p, std::complex<double> *root, int n);
bool Eigenvector(double a[][4], std::complex<double> lambda, std::complex<double> *V);


bool Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress);
bool Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[], double x[], int n, bool *pbCancel);


void *GetPlrVariable(CPolar *pPolar, int iVar);
double GetVar(Store<CPolar>*m_poaPolar, int nVar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError);
double GetZeroLiftAngle(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau);
double GetCl(Store<CPolar>*m_poaPolar, CFoil  *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
double GetCm(Store<CPolar>*m_poaPolar, CFoil  *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
double GetCm0(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError);
double GetCd(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
double GetXCp(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau,  bool &bOutRe, bool &bError);
double GetXTr(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError);
double GetPlrPointFromCl(Store<CPolar>*m_poaPolar, CFoil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError);
double GetPlrPointFromAlpha(Store<CPolar>*m_poaPolar, CFoil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError);
void GetLinearizedPolar(Store<CPolar>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope);

bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d);

int Compare(std::complex<double> a, std::complex<double>b);
void ComplexSort(std::complex<double>*array, int ub);

#endif // FUNCTIONS_H
