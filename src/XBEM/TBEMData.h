/****************************************************************************

    TBEMData Class
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

#ifndef TBEMDATA_H
#define TBEMDATA_H


#include <QList>
#include <QString>
#include <QColor>
#include "BData.h"
#include "TData.h"
#include "BEMData.h"


class TBEMData : public BEMData
{

    friend class QBEM;
    friend class MainFrame;

public:

    void Compute(BData *pBData, CBlade *pWing, double lambda, double pitch, double windspeed);
    void Clear();
	static QStringList prepareMissingObjectMessage();

	static TBEMData* newBySerialize();
	void serialize();  // override from BEMData
	TBEMData();

private:
    QString m_TurbineName;
    QString m_SimName;

	QList <double> m_Pitch;                 //pitch angle
    QList <double> m_Cp_loss;               //power coefficient including losses

    double OuterRadius;

	QList <double> m_Weibull;               //weibullprobability
	QList <double> m_aepk;					//annual energy production // new variable JW
	QList <double> m_aepA;					//annual energy production // new variable JW
};

#endif // TBEMDATA_H
