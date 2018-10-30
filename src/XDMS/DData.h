/****************************************************************************

    DData Class
        Copyright (C) 2012 Juliane Wendler

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

#ifndef DDATA_H
#define DDATA_H


#include <QList>
#include <QColor>
#include "../XBEM/Blade.h"
#include "../XBEM/360Polar.h"
#include "../Graph/ShowAsGraphInterface.h"


class DData : public ShowAsGraphInterface {
    friend class QDMS;
    friend class MainFrame;
    friend class DMSData;

public:
	virtual NewCurve* newCurve (QString /*xAxis*/, QString /*yAxis*/, NewGraph::GraphType /*graphType*/) { return NULL; }
	NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType, int heightIndex);
	static QStringList getAvailableVariables (NewGraph::GraphType graphType);
	QString getObjectName () { return QString(m_DMSName + " " + windspeedStr); }
	bool isDrawPoints ();
	bool isDrawCurve ();

    double elements;
    double epsilon;
    double iterations;
    double relax;
    double rho;
    double visc;
    bool bPowerLaw;
    bool bConstant;
    bool bLogarithmic;
    double exponent;
    double roughness;

    double Toff;
    double windspeed;
    QString windspeedStr;
    bool m_bTipLoss;
    bool m_bVariable;
    bool m_bAspectRatio;

	DData(QString dmsName);
    virtual ~DData();

	void serialize();
	static DData* newBySerialize();
	void Init(CBlade *pWing, double lambda, double pitch);
    void OnDMS(CBlade *pBlade);
	C360Polar* Get360Polar(QString m_FoilName, QString PolarName);

//private:
    QString m_WingName;
    QString m_DMSName;
    QString lambdaglobal;

    double sweptArea;
    double lambda_global;
    double max_radius;
    double max_r_pos;
    double max_r_count;
    double positions;
    int m_sections; //number of sections
    double h_top, h_bottom, height;
    double delta;
    double cp, cp1, cp2;//power coefficient
    double cm, cm1, cm2;// torque coefficient
    double w;// rotational speed
    double power, torque, thrust;//turbine power output and torque
    double ct, ct1, ct2; // thrust coefficient
    double blades;
    bool m_bIsInverted;

    QList <double> deltas;

    // one value per height position
    QList <double> m_pos;           //local height
    QList <double> m_zeta;          //local relative height
    QList <double> m_c_local;       //local chord
    QList <double> m_t_local;       //local thickness
    QList <double> m_delta;         //local inclination angle
    QList <double> m_radius_local;  //local radius
    QList <double> m_theta_local;   //local azi angle
    QList <double> m_eta;           //local relative radius
    QList <double> m_lambda_up;     //local upwind tip speed ratio
    QList <double> m_lambda_down;   //local downwind tip speed ratio
    QList <double> m_velocity_inf;  //local inflow velocity
    QList <double> m_velocity_up;   //local upwind induced velocity
    QList <double> m_velocity_equil;//local equilibrium-induced velocity
    QList <double> m_velocity_down; //local downwind induced velocity
    QList <double> m_velocity_wake; //local wake velocity

    QList <double> m_u_up;          //local (average) upwind interference factor
    QList <double> m_u_down;        //local (average) downwind induction factor

    QList <double> m_a_up;          //local upwind induction factor
    QList <double> m_a_down;        //local downwind interference factor
    QList <double> m_area;          //projection area for swept area
    QList <double> m_Ftip_up;       //local upwind tiploss factor
    QList <double> m_Ftip_dw;       //local downwind tiploss factor
    QList <double> m_it_up;         //iterations for convergence upwind
    QList <double> m_it_dw;         //iterations for convergence downwind
    QList <double> m_twist;         //twist angle

    // one value per azimuthal position
    QList <double> m_theta;         //azimuthal angle
    QList <double> m_theta_deg;     //azimuthal angle in degrees
    QList <double> m_theta_plot;    //azimuthal angle in degrees +90deg

    QList <double> m_alpha_eq;      //equatorial local angle of attack
    QList <double> m_Re_eq;         //equatorial local Reynolds Number
    QList <double> m_vrel_eq;       //equatorial local relative velocity
    QList <double> m_CD_eq;         //equatorial drag coeff
    QList <double> m_CL_eq;         //equatorial drag coeff
    QList <double> m_LD_eq;         //equatorial lift to drag coeff
    QList <double> m_Cn_eq;         //equatorial normal force coefficient
    QList <double> m_Ct_eq;         //equatorial tangential force coefficient
    QList <double> m_CFN_eq;        //blade element normal force coefficient
    QList <double> m_CFT_eq;        //blade element tangential force coefficient
	QList <double> m_CF_length;     //lengthwise force coefficient for one blade
	QList <double> m_CF_cross;      //crosswise force coefficient for one blade
	QList <double> m_CF_length_tot; //lengthwise force coefficient for all blades
	QList <double> m_CF_cross_tot;  //crosswise force coefficient for all blades

    QList <double> m_FN;            //normal force distribution, one blade
    QList <double> m_FT;            //tangential force distribution, one blade
    QList <double> m_T;             //torque distribution, one blade
	QList <double> m_F_length_tot;	//lengthwise force districution, one blade
	QList <double> m_F_cross_tot;	//crosswise force districution, one blade
	QList <double> m_F_length;		//lengthwise force districution, all blades
	QList <double> m_F_cross;		//crosswise force districution, all blades

    QList <double> m_FN_tot;        //total normal force distribution, all blades
    QList <double> m_FT_tot;        //total tangential force distribution, all blades
    QList <double> m_T_tot;         //total torque distribution, all blades

    // azimuthal arrays per height position
    QList<QList<double> > m_iterations;//local angle of attack
    QList<QList<double> > m_alpha;  //local angle of attack
    QList<QList<double> > m_alpha_deg;//local angle of attack in degrees
    QList<QList<double> > m_Ftip;   //local angle of attack in degrees
    QList<QList<double> > m_u;      //local interference factor
    QList<QList<double> > m_V;      //local induced velocity
    QList<QList<double> > m_Re;     //local Reynolds Number
    QList<QList<double> > m_DeltaRe;     //delta Reynolds Number

    QList<QList<double> > m_vrel;   //local relative velocity
    QList<QList<double> > m_CD;     //drag coeff
    QList<QList<double> > m_CL;     //drag coeff
    QList<QList<double> > m_LD;     //lift to drag coeff
    QList<QList<double> > m_Cn;     //normal force coefficient
    QList<QList<double> > m_Ct;     //tangential force coefficient

    bool m_bShowPoints;
    bool m_bIsVisible;
    bool m_bBackflow;
    int m_Style;
    int m_Width;
    QColor m_Color;

private:
	virtual QPen doGetPen (int curveIndex, int highlightedIndex, bool forTheDot);
};

#endif // DDATA_H
