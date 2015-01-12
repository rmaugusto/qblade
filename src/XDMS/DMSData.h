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
#include "DData.h"

class DMSData : public StorableObject
{

    friend class QDMS;
    friend class MainFrame;
    friend class TDMSData;

public:

	virtual void Compute(DData *pDData, CBlade *pWing, double lambda, double pitch);
    virtual void Clear();
    virtual void Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat);
	virtual void serialize ();  // override from StorableObject
	void restorePointers();  // override from StorableObject

	static DMSData* newBySerialize ();
	DMSData();
	virtual ~DMSData();

private:
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
};




#endif // DMSDATA_H
