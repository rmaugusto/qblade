/****************************************************************************

    TData Class
        Copyright (C) 2010 David Marten david.marten@tu-berlin.de

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

#ifndef TDATA_H
#define TDATA_H

#include "../StorableObject.h"
#include "../ParameterObject.h"
#include "../ParameterKeys.h"
template <class KeyType>
class ParameterViewer;


// NM class should be split into TurbineCommon, TurbineBEM, TurbineDMS
class TData : public StorableObject, public ParameterObject<Parameter::TData>
{
    friend class QBEM;
	friend class QDMS;  // new friend class JW
    friend class MainFrame;

public:
	double getRotorRadius ();
	
	static QStringList prepareMissingObjectMessage(bool forDMS);
	static TData* newBySerialize ();
	void serialize();  // override from StorableObject
	TData();
	TData(ParameterViewer<Parameter::TData> *viewer);
	virtual ~TData();

private:
	QVariant accessParameter(Parameter::TData::Key key, QVariant value = QVariant());
	
public:  //private:
    QString m_TurbineName;
    QString m_WingName;
    bool turbtype; //new JW variable 0:HAWT, 1:VAWT

    bool isStall;
    bool isPitch;
    bool isFixed;
    bool is2Step;
    bool isVariable;
    bool isPrescribedPitch;
    bool isPrescribedRot;

    double VariableLosses;
    double m_fixedLosses;
    double OuterRadius;
    double Generator;
    double Rot1;
    double Rot2;
    double Lambda0;
    double CutIn;
    double CutOut;
    double Switch;
    double FixedPitch;
    // new JW variables //
    double Offset;
    double RHeight;  // TODO NM this value is redundant and seems to be unused
    double THeight;
    double MaxRadius;
    double SweptArea;
    // end new JW variables //

    QList <double> pitchwindspeeds;
    QList <double> rotwindspeeds;
    QList <double> pitchangles;
    QList <double> rotspeeds;
};

#endif // TDATA_H
