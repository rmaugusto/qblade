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

#include "TData.h"
#include "../Globals.h"
#include "../Store.h"
#include "../Serializer.h"


TData::TData()
	: StorableObject ("name")
{
    OuterRadius = 0;
    Generator = 0;
    Rot1= 0;
    Rot2 = 0;
    Lambda0 = 0;
    CutIn = 0;
    CutOut = 0;
    Switch = 0;
    VariableLosses = 0;
    FixedLosses = 0;
    FixedPitch = 0;
    isPrescribedPitch=false;
    isPrescribedRot=false;
    //new code JW//
    Offset = 0;
	THeight = 0;
	RHeight = 0;
    MaxRadius = 0;
    SweptArea = 0;
    //end new code JW//

}

TData::~TData()
{

}

void TData::Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
    float f;
    int n,m;

    n = pitchwindspeeds.size();
    m = rotwindspeeds.size();

    if (bIsStoring)
    {
        WriteCString(ar,m_TurbineName);
        WriteCString(ar,m_WingName);
        //// new code JW ////
        if (turbtype) ar << 1; else ar << 0;
        //// end new code JW ////
        if (isStall) ar << 1; else ar << 0;
        if (isPitch) ar << 1; else ar << 0;
        if (isFixed) ar << 1; else ar << 0;
        if (is2Step) ar << 1; else ar << 0;
        if (isVariable) ar << 1; else ar << 0;
        if (isPrescribedPitch) ar << 1; else ar << 0;
        if (isPrescribedRot) ar << 1; else ar << 0;

        ar << (float) OuterRadius;
        ar << (float) Generator;
        ar << (float) Rot1;
        ar << (float) Rot2;
        ar << (float) Lambda0;
        ar << (float) CutIn;
        ar << (float) CutOut;
        ar << (float) Switch;
        ar << (float) FixedLosses;
        ar << (float) VariableLosses;
        ar << (float) FixedPitch;
        // new code JW //
        ar << (float) Offset;
        ar << (float) RHeight;
        ar << (float) THeight;
        ar << (float) MaxRadius;
        ar << (float) SweptArea;
        // end new code JW //

        ar << (int) n;

        for (int i=0;i<n;i++)
        {
            ar << (float) pitchwindspeeds.at(i) << (float) pitchangles.at(i);
        }

        ar << (int) m;

        for (int i=0;i<m;i++)
        {
            ar << (float) rotwindspeeds.at(i) << (float) rotspeeds.at(i);
        }

    }
    else
    {
        ReadCString(ar,m_TurbineName);
		setName(m_TurbineName);
        ReadCString(ar,m_WingName);
//        setParentName(m_WingName);  // NM REPLACE
		setSingleParent(g_rotorStore.getObjectByNameOnly(m_WingName));  // NM needed for downwards compatibility
        // new code JW //
        if (ArchiveFormat>=100022)
        {
            ar >> f;
            if (f) turbtype = true; else turbtype = false;
        }
        // end new code JW //
        ar >> f;
        if (f) isStall = true; else isStall = false;
        ar >> f;
        if (f) isPitch = true; else isPitch = false;
        ar >> f;
        if (f) isFixed = true; else isFixed = false;
        ar >> f;
        if (f) is2Step = true; else is2Step = false;
        ar >> f;
        if (f) isVariable = true; else isVariable = false;

        if (ArchiveFormat >= 100019)
        {
        ar >> f;
        if (f) isPrescribedPitch = true; else isPrescribedPitch = false;
        ar >> f;
        if (f) isPrescribedRot = true; else isPrescribedRot = false;
        }

        ar >> f;
        OuterRadius = f;
        ar >> f;
        Generator = f;
        ar >> f;
        Rot1 = f;
        ar >> f;
        Rot2 = f;
        ar >> f;
        Lambda0 = f;
        ar >> f;
        CutIn = f;
        ar >> f;
        CutOut = f;
        ar >> f;
        Switch = f;
        ar >> f;
        FixedLosses = f;
        ar >> f;
        VariableLosses = f;
        if (ArchiveFormat >= 100015)
        {
        ar >> f;
        FixedPitch = f;
        }

        // new code JW //

        if (ArchiveFormat>=100022)
        {
            ar >> f;
            Offset = f;
            ar >> f;
            RHeight = f;
            ar >> f;
            THeight = f;
            ar >> f;
            MaxRadius = f;
            ar >> f;
            SweptArea = f;
        }
        // end new code JW //

        if (ArchiveFormat >= 100020)
        {
            ar >> n;
            for (int i=0;i<n;i++)
            {
                ar >> f;
                pitchwindspeeds.append(f);
                ar >> f;
                pitchangles.append(f);
            }
            ar >> m;
            for (int i=0;i<m;i++)
            {
                ar >> f;
                rotwindspeeds.append(f);
                ar >> f;
                rotspeeds.append(f);
            }
        }


	}
}

double TData::getRotorRadius()
{
	return OuterRadius;
}

TData* TData::newBySerialize() {
	TData* tData = new TData ();
	tData->serialize();
	return tData;
}

void TData::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteString (&m_TurbineName);
	g_serializer.readOrWriteString (&m_WingName);
	g_serializer.readOrWriteBool (&turbtype);
	
	g_serializer.readOrWriteBool (&isStall);
	g_serializer.readOrWriteBool (&isPitch);
	g_serializer.readOrWriteBool (&isFixed);
	g_serializer.readOrWriteBool (&is2Step);
	g_serializer.readOrWriteBool (&isVariable);
	g_serializer.readOrWriteBool (&isPrescribedPitch);
	g_serializer.readOrWriteBool (&isPrescribedRot);
	
	g_serializer.readOrWriteDouble (&VariableLosses);
	g_serializer.readOrWriteDouble (&FixedLosses);
	g_serializer.readOrWriteDouble (&OuterRadius);
	g_serializer.readOrWriteDouble (&Generator);
	g_serializer.readOrWriteDouble (&Rot1);
	g_serializer.readOrWriteDouble (&Rot2);
	g_serializer.readOrWriteDouble (&Lambda0);
	g_serializer.readOrWriteDouble (&CutIn);
	g_serializer.readOrWriteDouble (&CutOut);
	g_serializer.readOrWriteDouble (&Switch);
	g_serializer.readOrWriteDouble (&FixedPitch);
	g_serializer.readOrWriteDouble (&Offset);
	g_serializer.readOrWriteDouble (&RHeight);
	g_serializer.readOrWriteDouble (&THeight);
	g_serializer.readOrWriteDouble (&MaxRadius);
	g_serializer.readOrWriteDouble (&SweptArea);
	
	g_serializer.readOrWriteDoubleList1D (&pitchwindspeeds);
	g_serializer.readOrWriteDoubleList1D (&rotwindspeeds);
	g_serializer.readOrWriteDoubleList1D (&pitchangles);
	g_serializer.readOrWriteDoubleList1D (&rotspeeds);
}
