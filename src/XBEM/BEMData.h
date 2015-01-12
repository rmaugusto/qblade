/****************************************************************************

    BEMData Class
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

#ifndef BEMDATA_H
#define BEMDATA_H

#include <QList>
#include <QString>
#include <QColor>
#include "BData.h"

class BEMData : public StorableObject
{

    friend class QBEM;
    friend class MainFrame;
    friend class TBEMData;
public:

    virtual void Compute(BData *pBData, CBlade *pWing, double lambda, double pitch);
    virtual void Clear();
    virtual void Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat);

	static BEMData* newBySerialize ();
	virtual void serialize ();  // override from StorableObject
	void restorePointers();  // override from StorableObject
	BEMData();
    QVector <BData *> GetBData();
	virtual ~BEMData();

private:
    QString m_WingName;
    QString m_BEMName;

    QList <double> m_Cp;
    QList <double> m_Ct;
    QList <double> m_Cm;
    QList <double> m_Lambda;
    QList <double> m_one_over_Lambda;
    QList <double> m_Kp;
	QVector <BData *> m_BData;

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
    bool m_bTipLoss;
    bool m_bRootLoss;
    bool m_b3DCorrection;
    bool m_bInterpolation;
    bool m_bNewTipLoss;
    bool m_bNewRootLoss;
    bool m_bCdReynolds;
};

#endif // BEMDATA_H
