/****************************************************************************

    OpPoint Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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


#ifndef OPPOINT_H
#define OPPOINT_H

#include "../Params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <QDataStream>
#include "../StorableObject.h"
#include "../Noise/noisesimulationdialog.h"


class OpPoint : public StorableObject
{

	friend class MainFrame;
	friend class CPolar;
	friend class QXDirect;
	friend class ObjectPropsDlg;

	// An operating point is the result of an XFoil calculation
	// for a given Reynolds
public:

    OpPoint(QString name = "< no name >", StorableObject *parent = NULL);
    void serialize ();
    static OpPoint* newBySerialize ();

    double getReynolds() const;
    double getMach() const;
    double getAlpha() const;

    int getNblAt(int x);
    int getIblteAt(int x);
    double getXAt(int x);
    int getIpanAt(int x, int y);
    double getDstrAt(int x, int y);

    void setReynolds(double value);
    void setAlpha(double value);


    void CreateXBL(double xs[IVX][3],int &nside1, int &nside2);
    void FillHk(double ws[IVX][3], int nside1, int nside2);
    void FillRTheta(double ws[IVX][3], int nside1, int nside2);
    bool hkin(double h, double msq, double &hk, double &hk_h, double &hk_msq);
    bool getLvconv() const;
    void setLvconv(bool Lvconv);

private:
    bool m_bVisc, m_bDispSurf,m_Lvconv=false;
    bool m_bTEFlap, m_bLEFlap;
    bool m_bIsVisible, m_bShowPoints;


    int n, nd1, nd2, nd3,iblte[ISX],nbl[ISX],ipan[IVX][ISX];
    int m_Style, m_Width;


    double Reynolds;
    double Mach;
    double Alpha; // index for polar
    double Cl, Cm, Cd, Cdp, Xtr1, Xtr2, ACrit;
	double m_XCP;
    double m_TEHMom, m_LEHMom, XForce, YForce;
    double Cpmn;

    double y[IQX],x[IQX];
	double Cpv[IQX], Cpi[IQX];
	double Qv[IQX], Qi[IQX];
	double xd1[IQX], yd1[IQX];// first...
	double xd2[IWX], yd2[IWX];// ...second...
	double xd3[IWX], yd3[IWX];// ...and third part of the boundary layer
    double dstr[IVX][ISX];

    double gamm1;
    double qinf, reinf1, minf1, tklam;
    double uedg[IVX][ISX], tau[IVX][ISX], dis[IVX][ISX], ctau[IVX][ISX],thet[IVX][ISX],ctq[IVX][ISX];


	QString m_strFoilName;
	QString m_strPlrName;

	QColor m_Color;


private:
	bool ExportOpp(QTextStream &out, QString Version, int FileType, bool bDataOnly=false);
    bool SerializeOp(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	void GetOppProperties(QString &OpPointProperties, bool bData=false);
};

#endif
