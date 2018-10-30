/****************************************************************************

    DMSData Class
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

#ifndef DMSDATA_H
#define DMSDATA_H

#include <QList>
#include <QString>
#include <QColor>

#include "../StorableObject.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../ParameterObject.h"
class DData;
template <class ParameterGroup> class ParameterViewer;


class DMSData : public StorableObject, public ShowAsGraphInterface, public ParameterObject<Parameter::DMSData>
{
public:
	//enum SimulationType {};  // NM fill this later, when more Modules are reimplemented

    virtual void Compute(DData *pDData, CBlade *pWing, double lambda, double inflowspeed);
    virtual void Clear();
	virtual void serialize ();  // override from StorableObject
	void restorePointers();  // override from StorableObject
	virtual NewCurve* newCurve (QString xAxis, QString yAxis, NewGraph::GraphType graphType);  // returns NULL if var n.a.
	static QStringList getAvailableVariables (NewGraph::GraphType graphType);
	virtual QString getObjectName () { return m_objectName; }
	bool hasResults() { return !m_Cp.isEmpty(); }  // return true, if simulation was finished
	void startSimulation ();

	static DMSData* newBySerialize ();
	DMSData();
	DMSData(ParameterViewer<Parameter::DMSData> *viewer);
	virtual ~DMSData();
	static QStringList prepareMissingObjectMessage();

//private:
    QString m_WingName;
    QString m_DMSName;

    QList <double> m_Cp;
    QList <double> m_Cp1;
    QList <double> m_Cp2;
    QList <double> m_Cm;
    QList <double> m_Cm1;
    QList <double> m_Cm2;

    QList <double> m_w;
    QList <double> m_Lambda;
    QList <double> m_one_over_Lambda;
    QList <DData *> m_DData;
    QList <double> m_Kp;

    QList <double> m_P;                     //power
    QList <double> m_T;                     //torque
    QList <double> m_Thrust;                //thrust
    QList <double> m_V;                     //wind speed
    QList <double> m_Omega;                 //rotational speed

    bool m_bShowPoints;
    bool m_bIsVisible;
    int m_Style;
    int m_Width;
    QColor m_Color;

    double rho;
    double elements;
    double epsilon;
    double iterations;
    double relax;
    double visc;
    bool m_bPowerLaw;
    bool m_bConstant;
    bool m_bLogarithmic;
    double exponent;
    double roughness;
    bool m_bTipLoss;
    bool m_bVariable;
    bool m_bAspectRatio;
	
	double m_windspeed;
	double m_tipSpeedFrom, m_tipSpeedTo, m_tipSpeedDelta;
	double m_windspeedFrom, m_windspeedTo, m_windspeedDelta;  // NM only used for TDMSData

private:
	virtual QPen doGetPen (int curveIndex, int highlightedIndex, bool forTheDot);
	QVariant accessParameter(Parameter::DMSData::Key key, QVariant value = QVariant());
};




#endif // DMSDATA_H
