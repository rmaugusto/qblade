/****************************************************************************

    TData Class
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

#ifndef TDATA_H
#define TDATA_H


#include <QList>
#include <QString>
#include <QColor>
#include "BData.h"

class TData : public StorableObject
{
    friend class QBEM;
	friend class QDMS;  // new friend class JW
    friend class MainFrame;

public:
    void Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat);
	double getRotorRadius ();
	
	static TData* newBySerialize ();
	void serialize();  // override from StorableObject
	TData();

	virtual ~TData();

private:
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
    double FixedLosses;
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
    double RHeight;
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
