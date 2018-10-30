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


// OpPoint.cpp: implementation of the OpPoint class.
// The Class associated to OpPoint objects
// resulting from XFoil calculations


#include "OpPoint.h"
#include "../Globals.h"
#include "../MainFrame.h"
#include "../Serializer.h"
#include "../Store.h"
#include "Foil.h"
#include "Polar.h"


OpPoint* OpPoint::newBySerialize() {
    OpPoint* oppoint = new OpPoint ();
    oppoint->serialize();
	return oppoint;
}

void OpPoint::writeData(QTextStream &stream, bool asCsv) {
	QString sep = (asCsv ? ", " : " ");  // comma separator or not
	
	stream << "Alpha =" << sep << Alpha << ", "
		   << "Re =" << sep << Reynolds << ", "
		   << "Ma =" << sep << Mach << ", "
		   << "ACrit =" << sep << ACrit << endl
		   << endl;
			  
	if (!m_readyForNoise) {
		stream << "This operational point does not contain the required data (available since QBlade v0.95). Please, "
			   << "run the analysis again.";
		return;
	}
		
	stream << "Top Side" << endl
		   << "       x" << sep << "      Hk" << sep << " Ue/Vinf" << sep << "      Cf" << sep << "      Cd" << sep
		   << "    A/AO" << sep << "      D*" << sep << "   Theta" << sep << "     CTq" << endl;
		
	/* NM: in the OpPoint legacy code the arrays started at some offset which seemed to vary. But in this export
	 * function the offset apparently is considered to always be 2. Therefore I make the same assumption and set the
	 * offset to 2 as well, which might be incorrect, though.
	 * Update: The offset is no longer removed from the YVectors, so it has to be applied here as usual!
	 * For the x Vector no offset was added in the first place, so this must be adjusted here be adding the offset to i.
	 * */
	for (int i = 2; i < nSide1; ++i) {
		stream << x1Values[i] << sep
			   << kinShapeParaTop.second[i] << sep
			   << edgeVelocityTop.second[i] << sep
			   << skinFrictionTop.second[i] << sep
			   << dissipationTop.second[i] << sep
			   << topShear.second[i] << sep
			   << topDStar.second[i] << sep
			   << topTheta.second[i] << sep
			   << topShearEq.second[i] << endl;
	}
	
	stream << "Bottom Side" << endl
		   << "       x" << sep << "      Hk" << sep << " Ue/Vinf" << sep << "      Cf" << sep << "      Cd" << sep
		   << "    A/AO" << sep << "      D*" << sep << "   Theta" << sep << "     CTq" << endl;
	
	for (int i = 2; i < nSide2; ++i) {
		stream << x2Values[i] << sep
			   << kinShapeParaBot.second[i] << sep
			   << edgeVelocityBot.second[i] << sep
			   << skinFrictionBot.second[i] << sep
			   << dissipationBot.second[i] << sep
			   << botShear.second[i] << sep
			   << botDStar.second[i] << sep
			   << botTheta.second[i] << sep
			   << botShearEq.second[i] << endl;
	}
}

OpPoint::OpPoint(QString name, StorableObject *parent)
    : StorableObject (name, parent)
{
	m_bVisc     = false;//not a  viscous point a priori
	m_bDispSurf = false;// no boundary layer surface either
	m_bTEFlap   = false;
	m_bLEFlap   = false;

	Xtr1   = 0.0;
	Xtr2   = 0.0;
	XForce = 0.0;
	YForce = 0.0;
	Cpmn   = 0.0;
	m_XCP  = 0.0;
	m_LEHMom   = 0.0; m_TEHMom = 0.0;

	memset(Qi,  0, sizeof(Qi));
	memset(Qv,  0, sizeof(Qv));
	memset(Cpi,  0, sizeof(Cpi));
	memset(Cpv,  0, sizeof(Cpv));
	memset(x,  0, sizeof(y));
	memset(y,  0, sizeof(x));

	memset(xd1,  0, sizeof(xd1));
	memset(xd2,  0, sizeof(xd2));
	memset(xd3,  0, sizeof(xd3));
	memset(yd1,  0, sizeof(yd1));
	memset(yd2,  0, sizeof(yd2));
	memset(yd3,  0, sizeof(yd3));
	nd1 = 0;
	nd2 = 0;
	nd3 = 0;
//	Format = 1;
	
	// new vars for noise module
	m_readyForNoise = false;
	// end

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;
	m_Width = 1;
    m_Color = QColor(255,0,100,127);
}


// This Object is  used to store the data issued from an XFoil Calculation
// an array of OperatingPoints is a CPolar

bool OpPoint::SerializeOp(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	int a, b, k, Format;
	float f,gg;

    if(bIsStoring)
    {
//		ar << 100003;
//		//100003 : suppressed archiving of s coordinate
//		//100002 : first numbered archiveformat
//		//write variables
//		WriteCString(ar, m_strFoilName);
//		WriteCString(ar, m_strPlrName);
//		ar << (float)Reynolds << (float)Mach << (float)Alpha;
//		ar << n << nd1 << nd2 << nd3;
//		if(m_bVisc)     a=1; else a=0;
//		if(m_bDispSurf) b=1; else b=0;

//		ar << a << b;
//		ar << (float)Cl << (float)Cm << (float)Cd << (float)Cdp;
//		ar << (float)Xtr1 << (float)Xtr2 << (float)ACrit << (float)m_TEHMom << (float)Cpmn;
//		for (k=0; k<n; k++)	ar << (float)Cpv[k] << (float)Cpi[k];
////		for (k=0; k<n; k++)	    ar << (float)s[k] << (float)Qv[k] << (float)Qi[k];
//		for (k=0; k<n; k++)	    ar << (float)Qv[k] << (float)Qi[k];
//		for (k=0; k<=nd1; k++)  ar << (float)xd1[k] << (float)yd1[k];
//		for (k=0; k<nd2; k++)	ar << (float)xd2[k] << (float)yd2[k];
//		for (k=0; k<nd3; k++)	ar << (float)xd3[k] << (float)yd3[k];
//		ar<<m_Style<<m_Width;
//		WriteCOLORREF(ar,m_Color);
//		if(m_bIsVisible)     ar<<1; else ar<<0;
//		if(m_bShowPoints)    ar<<1; else ar<<0;
	}
	else
	{
        if(ArchiveFormat>=100002) ar>>Format;
        else Format = 0;

        //read variables
		ReadCString(ar, m_strFoilName);
		ReadCString(ar, m_strPlrName);
        // NM the following line should fix the bug that occured when loading old files
        setSingleParent(g_polarStore.getObjectByName(m_strPlrName, g_foilStore.getObjectByName(m_strFoilName, NULL)));


        ar >> f; Reynolds =f;
        ar >> f; Mach = f;
        ar >> f; Alpha = f;
        ar >> n >> nd1 >> nd2 >> nd3;
        ar >> a >> b;
        if(a) m_bVisc = true; else m_bVisc = false;
        if(a!=0 && a!=1){
            m_strFoilName = "";
            return false;
        }
        if(b) m_bDispSurf = true; else m_bDispSurf = false;
        if(b!=0 && b!=1){
            m_strFoilName = "";
            return false;
        }

        ar >> f; Cl = f;
        ar >> f; Cm = f;
        ar >> f; Cd = f;
        ar >> f; Cdp = f;
        ar >> f; Xtr1 = f;
        ar >> f; Xtr2 = f;
        ar >> f; ACrit =f;
        ar >> f; m_TEHMom = f;
        ar >> f; Cpmn = f;
        for (k=0; k<n; k++)	{
            ar >> f; Cpv[k] = f;
            ar >> f; Cpi[k] = f;
        }

//			if (Format ==2) {
		for (k=0; k<n; k++)
		{
			if(Format<=100002)	ar >> f; //s[k]  = f;
			ar >> f; Qv[k] = f;
			ar >> f; Qi[k] = f;
		}
//			}
		for (k=0; k<=nd1; k++)
		{
			ar >> f >> gg;
            xd1[k] = f;
			yd1[k] = gg;
        }
		for (k=0; k<nd2; k++)
		{
			ar >> f >> gg;
            xd2[k] = f;
			yd2[k] = gg;
        }
		for (k=0; k<nd3; k++)
		{
			ar >> f >> gg;
            xd3[k] = f;
			yd3[k] = gg;
        }
		if(ArchiveFormat>=100002)
		{
			ar>>m_Style>>m_Width;
			ReadCOLORREF(ar, m_Color);

			ar >> a ;
			if(a!=0 && a!=1)
			{
                m_strFoilName = "";
                return false;
            }
            if(a) m_bIsVisible = true; else m_bIsVisible = false;

            ar >> a ;
            if(a!=0 && a!=1){
                m_strFoilName = "";
                return false;
            }
            if(a) m_bShowPoints = true; else m_bShowPoints = false;
        }
	}
	return true;
}


bool OpPoint::ExportOpp(QTextStream &out, QString Version, int FileType, bool bDataOnly)
{
	int k;
	QString strong;

	if(!bDataOnly)
	{
		out << Version+"\n";

		strong = m_strFoilName + "\n";
		out<< strong;
		strong = m_strPlrName + "\n";
		out<< strong;
		if(FileType==1) strong=QString("Alpha = %1,  Re = %2,  Ma = %3,  ACrit =%4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		else            strong=QString("Alpha =, %1,  Re =, %2,  Ma =, %3,  ACrit =, %4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		out<< strong;
	}

	if(FileType==1) out << "   x        Cpi      Cpv        Qi        Qv\n";
	else            out << "x,Cpi,Cpv,Qi,Qv\n";

	for (k=0; k<n; k++)
	{
		if(FileType==1) strong=QString("%1  %2   %3   %4   %5\n")
									   .arg(x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		else            strong=QString("%1,%2,%3,%4,%5\n")
									   .arg(x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		out<< strong;
	}

	out << "\n\n";

	return true;
}


void OpPoint::GetOppProperties(QString &OpPointProperties, bool bData)
{
	QString strong;
	OpPointProperties.clear();

	strong  = QString(QObject::tr("Re")+"    = %1 ").arg(Reynolds,7,'f',0);
	OpPointProperties += strong +"\n";

	strong  = QString(QObject::tr("Alpha")+" = %1").arg(Alpha,6,'f',2);
	OpPointProperties += strong +QString::fromUtf8("deg")+"\n";

	strong  = QString(QObject::tr("Mach")+"  = %1 ").arg(Mach,7,'f',3);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("NCrit")+" = %1 ").arg(ACrit,5,'f',1);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CL")+"    = %1 ").arg(Cl,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CD")+"    = %1 ").arg(Cd,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cm")+"    = %1 ").arg(Cm,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cdp")+"   = %1 ").arg(Cdp,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cpmn")+"  = %1 ").arg(Cpmn,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("XCP")+"   = %1 ").arg(m_XCP,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Top Transition")+" = %1 ").arg(Xtr1,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Bot Transition")+" = %1 ").arg(Xtr2,9,'f',5);
	OpPointProperties += strong + "\n";

	if(m_bTEFlap)
	{
		strong  = QString(QObject::tr("T.E. Flap moment")+" = %1 ").arg(m_TEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}
	if(m_bLEFlap)
	{
		strong  = QString(QObject::tr("L.E. Flap moment")+" = %1 ").arg(m_LEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
    ExportOpp(out, g_mainFrame->m_VersionName, g_mainFrame->m_ExportFileType, true);
	OpPointProperties += "\n"+strong;
}

void OpPoint::serialize() {
    StorableObject::serialize();

    g_serializer.readOrWriteBool (&m_bVisc);
    g_serializer.readOrWriteBool (&m_bDispSurf);
    g_serializer.readOrWriteBool (&m_bTEFlap);
    g_serializer.readOrWriteBool (&m_bLEFlap);

    g_serializer.readOrWriteInt (&n);
    g_serializer.readOrWriteInt (&nd1);
    g_serializer.readOrWriteInt (&nd2);
    g_serializer.readOrWriteInt (&nd3);
    g_serializer.readOrWriteInt (&m_Style);
    g_serializer.readOrWriteInt (&m_Width);

    g_serializer.readOrWriteDouble (&Reynolds);
    g_serializer.readOrWriteDouble (&Mach);
    g_serializer.readOrWriteDouble (&Alpha);
    g_serializer.readOrWriteDouble (&Cl);
    g_serializer.readOrWriteDouble (&Cm);
    g_serializer.readOrWriteDouble (&Cd);
    g_serializer.readOrWriteDouble (&Cdp);
    g_serializer.readOrWriteDouble (&Xtr1);
    g_serializer.readOrWriteDouble (&Xtr2);
    g_serializer.readOrWriteDouble (&ACrit);
    g_serializer.readOrWriteDouble (&m_XCP);
    g_serializer.readOrWriteDouble (&m_TEHMom);
    g_serializer.readOrWriteDouble (&m_LEHMom);
    g_serializer.readOrWriteDouble (&XForce);
    g_serializer.readOrWriteDouble (&YForce);
    g_serializer.readOrWriteDouble (&Cpmn);

    g_serializer.readOrWriteDoubleArray1D (x, IQX);
    g_serializer.readOrWriteDoubleArray1D (y, IQX);
    g_serializer.readOrWriteDoubleArray1D (Cpv, IQX);
    g_serializer.readOrWriteDoubleArray1D (Cpi, IQX);
    g_serializer.readOrWriteDoubleArray1D (Qv, IQX);
    g_serializer.readOrWriteDoubleArray1D (Qi, IQX);
    g_serializer.readOrWriteDoubleArray1D (xd1, IQX);
    g_serializer.readOrWriteDoubleArray1D (yd1, IQX);
    g_serializer.readOrWriteDoubleArray1D (xd2, IWX);
    g_serializer.readOrWriteDoubleArray1D (yd2, IWX);
    g_serializer.readOrWriteDoubleArray1D (xd3, IWX);
    g_serializer.readOrWriteDoubleArray1D (yd3, IWX);
	
	if (g_serializer.getArchiveFormat() >= 100035) {
		g_serializer.readOrWriteDoubleVector1D(&x1Values);
		g_serializer.readOrWriteDoubleVector1D(&x2Values);
		if (g_serializer.getArchiveFormat() >= 100042) {
			g_serializer.readOrWriteInt(&nSide1);
			g_serializer.readOrWriteInt(&nSide2);
		}
		g_serializer.readOrWritePairIntDoubleVector(&topShear);
		g_serializer.readOrWritePairIntDoubleVector(&topShearEq);
		g_serializer.readOrWritePairIntDoubleVector(&botShear);
		g_serializer.readOrWritePairIntDoubleVector(&botShearEq);
		g_serializer.readOrWritePairIntDoubleVector(&botDStar);
		g_serializer.readOrWritePairIntDoubleVector(&botTheta);
		g_serializer.readOrWritePairIntDoubleVector(&topDStar);
		g_serializer.readOrWritePairIntDoubleVector(&topTheta);
		g_serializer.readOrWritePairIntDoubleVector(&reThetaTop);
		g_serializer.readOrWritePairIntDoubleVector(&reThetaBot);
		g_serializer.readOrWritePairIntDoubleVector(&amplificationTop);
		g_serializer.readOrWritePairIntDoubleVector(&amplificationBot);
		g_serializer.readOrWritePairIntDoubleVector(&dissipationTop);
		g_serializer.readOrWritePairIntDoubleVector(&dissipationBot);
		g_serializer.readOrWritePairIntDoubleVector(&skinFrictionTop);
		g_serializer.readOrWritePairIntDoubleVector(&skinFrictionBot);
		g_serializer.readOrWritePairIntDoubleVector(&edgeVelocityTop);
		g_serializer.readOrWritePairIntDoubleVector(&edgeVelocityBot);
		g_serializer.readOrWritePairIntDoubleVector(&kinShapeParaTop);
		g_serializer.readOrWritePairIntDoubleVector(&kinShapeParaBot);
	}
	
	if (g_serializer.getArchiveFormat() >= 100040 && g_serializer.getArchiveFormat() < 100042) {
		int nbl[ISX], ipan[IVX][ISX], iblte[ISX];  // NM these values are not needed anymore
		double x2[IZX];
		double dstr[IVX][ISX];
		g_serializer.readOrWriteIntArray1D (nbl, ISX);
		g_serializer.readOrWriteIntArray2D (reinterpret_cast<int*> (ipan), IVX, ISX);
		g_serializer.readOrWriteDoubleArray1D (x2, IZX);
		g_serializer.readOrWriteIntArray1D (iblte, ISX);
		g_serializer.readOrWriteDoubleArray2D (reinterpret_cast<double*> (dstr), IVX, ISX);
	}
	
	if (g_serializer.getArchiveFormat() >= 100041) {
		g_serializer.readOrWriteBool(&m_readyForNoise);
		if (g_serializer.getArchiveFormat() < 100042) {
			m_readyForNoise = false;  // invalidate versions older than 42
		}
	} else if (g_serializer.isReadMode()) {
		m_readyForNoise = false;
	}

    g_serializer.readOrWriteString (&m_strFoilName);
    g_serializer.readOrWriteString (&m_strPlrName);

    g_serializer.readOrWriteColor (&m_Color);
}


