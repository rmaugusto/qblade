/****************************************************************************

    CPanel Class
	Copyright (C) 2006-2008 Andre Deperrois adeperrois@xflr5.com

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

#include <QtCore>
#include "Panel.h"
#include <math.h>


CPanel::CPanel(double vortpos, double aoapos)
{
    s_CtrlPos = aoapos;
    s_VortexPos = vortpos;
	Reset();
}

void CPanel::Reset()
{
	Area   = 0.0;
    m_Gamma = 0.0;

    m_Gamma_t_minus_1 = 0.0;
    m_Gamma_t_minus_2 = 0.0;

    angularPos = 0.0;

    m_AoA = 0;
    m_AoA_old = 0;

    pTB = NULL;
    pTA = NULL;

    isHub            = false;
    isTip            = false;

    LiftVector.Set(0,0,0);
    DragVector.Set(0,0,0);
    LiftDragVector.Set(0,0,0);

    for (int i=0;i<3;i++)
    {
        Q1[i]=0;
        Q2[i]=0;
        x[i]=0;
    }
    bLEsep = false;
    dx = 0;
    xLEV = 0;
    aoa_dot = 0;
    Cv = 0;

}


void CPanel::SetFrame(CVector  &LA, CVector  &LB, CVector  &TA, CVector  &TB)
{
    Old_CtrlPt = CtrlPt;

    LATB = TB - LA;

    TALB = LB - TA;

	Normal = LATB * TALB;
	Area = Normal.VAbs()/2.0;
	Normal.Normalize();

    //////////////new code TW

    CVector MidL, MidT, A, B, lc ;

    VortPtA = LA*(1.0-s_VortexPos)+TA*s_VortexPos;


    VortPtB = LB*(1.0-s_VortexPos)+TB*s_VortexPos;


    A = LA*(1.0-s_VortexPos)+TA*s_VortexPos;


    B = LB*(1.0-s_VortexPos)+TB*s_VortexPos;


    MidL=LA+(LB-LA)*0.5;


    MidT=TA+(TB-TA)*0.5;


    //ControlPt.x =
    lc=B-A;


    // MiL und MiT
//    MidL;


//    MidT;


    a1 = MidT-MidL;


    a3 = a1 *(lc);
    a2 = a1 * a3;
    a1.Normalize();
    a3.Normalize();
    a2.Normalize();
    ///end new code TW

    CtrlPt = (MidL + (MidT - MidL)*s_CtrlPos);

    if (pTB && pTA){
    LALB = CVector(VortPtB - VortPtA).VAbs();
    LBTB = CVector(*pTB - VortPtB).VAbs();
    TALA = CVector(VortPtA - *pTA).VAbs();
    TBTA = CVector(*pTA - *pTB).VAbs();
    }

}





