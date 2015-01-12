/****************************************************************************

    BData Class
        Copyright (C) 2010 David Marten qblade@web.de

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

#ifndef BDATA_H
#define BDATA_H


#include <QList>
#include <QColor>
#include "../XBEM/Blade.h"
#include "../XBEM/360Polar.h"

class BData
{
    friend class QBEM;
    friend class MainFrame;
    friend class BEMData;

public:
    BData();
    BData(CBlade *pWing, int lambda, int sec);

	void Serialize(QDataStream &ar, bool bIsStoring);
	void serialize ();
	static BData* newBySerialize();
	void Init(CBlade *pWing, double lambda);
    void OnBEM(double pitch);
	C360Polar* Get360Polar(QString m_FoilName, QString PolarName);
    QString GetWindspeed();

	
	bool m_bTipLoss;
    bool m_bRootLoss;
    bool m_b3DCorrection;
    bool m_bInterpolation;
    bool m_bNewRootLoss;
    bool m_bNewTipLoss;
    bool m_bCdReynolds;

    double elements;
    double epsilon;
    double iterations;
    double relax;
    double rho;
    double visc;

    double windspeed;
    QString windspeedStr;

public:
    QString m_WingName;
    QString m_BEMName;
    QString lambdaglobal;

    double lambda_global;
    double length;
    double outer_radius, inner_radius;
    double cp;
    double ct;
    int from, to;
    double blades;

    QList <double> m_pos;
    QList <double> m_c_local;       //local chord
    QList <double> m_lambda_local;  //local lambda
    QList <double> m_p_tangential;  //tangential thrust
    QList <double> m_p_normal;      //radial thrust
    QList <double> m_a_axial;       //axial induction factor
    QList <double> m_a_tangential;      //radial induction factor
    QList <double> m_Fa_axial;      //averaged axial induction factor
    QList <double> m_Fa_radial;     //averaged radial induction factor
    QList <double> m_circ;          //circulation
	QList <double> m_theta;         //angles in the wind triangle
    QList <double> m_alpha;         //angles in the wind triangle
	QList <double> m_phi;           //angles in the wind triangle
    QList <double> m_CL;            //lift coeff
    QList <double> m_CD;            //drag coeff
    QList <double> m_LD;            //lift to drag coeff
    QList <double> m_Cn;            //normal coefficient
    QList <double> m_Ct;            //thrust coefficient
    QList <double> m_F;             //Tip Loss Coefficient
    QList <double> m_between;       //the percentage of foils for foil interpolation
    QList <double> m_Reynolds;      //Reynolds Number
    QList <double> m_DeltaReynolds; //Delta between local Re and Re of Polar
    QStringList m_polar;        //list of polar at sections
    QStringList m_foil;         //list of foils at sections
    QStringList m_polarTO;      //list of polars at next sections for interpolation
    QStringList m_foilTO;       //list of foils at next sections for interpolation
    QList <double>  m_Roughness;    //the critical roughness for the blade surface
    QList <double>  m_Windspeed;    //windspeed at station (turbine calc)
    QList <double>  m_Iterations;   //total number of iterations required to converge
    QList <double>  m_Mach;         //Mach Number

	QVector <C360Polar *> m_PolarPointers;
	QVector <C360Polar *> m_PolarPointersTO;

    QList <double> deltas;
    QList <double> dist;

    bool m_bShowPoints;
    bool m_bIsVisible;
    int m_Style;
    int m_Width;
    QColor m_Color;
};

#endif // BDATA_H
