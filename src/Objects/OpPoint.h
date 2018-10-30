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

#include <QVector>
#include "../Params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <QDataStream>
#include "../StorableObject.h"


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
	
	void writeData (QTextStream &stream, bool asCsv);


//private:
	bool m_bVisc, m_bDispSurf;
	bool m_bTEFlap, m_bLEFlap;
    bool m_bIsVisible, m_bShowPoints;


	int n, nd1, nd2, nd3;
    int m_Style, m_Width;


	double Reynolds;
	double Mach;
	double Alpha; // index for polar
	double Cl, Cm, Cd, Cdp, Xtr1, Xtr2, ACrit;
	double m_XCP;
    double m_TEHMom, m_LEHMom, XForce, YForce;
    double Cpmn;

	double x[IQX], y[IQX];
	double Cpv[IQX], Cpi[IQX];
	double Qv[IQX], Qi[IQX];
	double xd1[IQX], yd1[IQX];// first...
	double xd2[IWX], yd2[IWX];// ...second...
	double xd3[IWX], yd3[IWX];// ...and third part of the boundary layer
	
	/* new variables: addidtional data from XFoil which formerly was thrown away */
	/* the data in the vectors fits to the same x values, but starts from different indices. Ussualy x1values are for
	 * the top data and x2values are for the bottom data, but better look up and check. The int value in YVector
	 * indicates at what x index to start. */
	typedef QPair<int, QVector<double> > YVector;
	QVector<double> x1Values, x2Values;  // created by m_pXFoil->CreateXBL(x, nside1, nside2);
	int nSide1, nSide2;  // created by m_pXFoil->CreateXBL(x, nside1, nside2);
	YVector topShear, topShearEq, botShear, botShearEq;  // "Max. Shear Coefficient" > QXDirect::OnCtPlot()
	YVector botDStar, botTheta;  // "Bottom Side D* and Theta" > QXDirect::OnDbPlot()
	YVector topDStar, topTheta;  // "Top Side D* and Theta" > QXDirect::OnDtPlot()
	YVector reThetaTop, reThetaBot;  // "Re_Theta" > QXDirect::OnRtPlot()
	YVector amplificationTop, amplificationBot;  // "Amplification Ratio" > QXDirect::OnNPlot()
	YVector dissipationTop, dissipationBot;  // "Dissipation Coefficient" > QXDirect::OnCdPlot()
	YVector skinFrictionTop, skinFrictionBot;  // "Skin Friction Coefficient" > QXDirect::OnCfPlot()
	YVector edgeVelocityTop, edgeVelocityBot;  // "Edge Velocity" > QXDirect::OnUePlot()
	YVector kinShapeParaTop, kinShapeParaBot;  // "Kinematic Shape Parameter" > QXDirect::OnHPlot()
	/* end */
	
	/* new variables that are required for the noise calculation and were formerly not copied from XFoil */
	bool m_readyForNoise;  // if the OpPoint contains the arrays needed for a noise simulation
	/* end */
	
	QString m_strFoilName;
	QString m_strPlrName;

	QColor m_Color;

private:
	bool ExportOpp(QTextStream &out, QString Version, int FileType, bool bDataOnly=false);
    bool SerializeOp(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	void GetOppProperties(QString &OpPointProperties, bool bData=false);
};

#endif
