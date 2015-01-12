/****************************************************************************

    TDMSData Class
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

#ifndef TDMSDATA_H
#define TDMSDATA_H


#include <QList>
#include <QString>
#include <QColor>
#include "DData.h"
#include "../XBEM/TData.h"
#include "DMSData.h"


class TDMSData : public DMSData
{
    friend class QDMS;
    friend class MainFrame;
	
public:
	void Compute(DData *pDData, CBlade *pWing, double lambda, double pitch, double Toff, double windspeed);
    void Clear();
	void Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat);
	void SerializeDummy(QDataStream &ar, bool bIsStoring, int ArchiveFormat);

	static TDMSData* newBySerialize ();
	void serialize();  // override from DMSData
	TDMSData();
	virtual ~TDMSData();

private:
    QString m_TurbineName;
    QString m_SimName;

    QList <double> m_P;                     //power
    QList <double> m_P_loss;                //power with losses
    QList <double> m_T;                     //torque
    QList <double> m_Thrust;                //thrust
    QList <double> m_V;                     //wind speed
    QList <double> m_Omega;                 //rotational speed
	QList <double> m_Weibull;               //weibullprobability
	QList <double> m_aepk;					//annual energy production for k range // new variable JW
	QList <double> m_aepA;					//annual energy production for A range // new variable JW
    QList <double> m_Cp_loss;               //power coefficient including losses
};
#endif // TDMSDATA_H
