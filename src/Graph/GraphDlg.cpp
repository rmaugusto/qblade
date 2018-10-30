/****************************************************************************

	GraphDlg  Classes
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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

#include "GraphDlg.h"
#include "../Misc/LinePickerDlg.h"
#include "../MainFrame.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>


int GraphDlg::s_ActivePage;
void *GraphDlg::s_pMainFrame;


GraphDlg::GraphDlg()
{
	setWindowTitle(tr("Graph Settings"));

	m_iGraphType = 1;
	m_pMemGraph = NULL;
	m_pGraph    = NULL;
	m_NGraph      = 0 ;
	m_bApplied = true;
	m_bVariableChanged = false;

	m_XSel = 0;
	m_YSel = 1;

	m_pTitleFont = m_pLabelFont = NULL;

	SetupLayout();
	Connect();
}


void GraphDlg::Connect()
{
	connect(m_pctrlTitleClr, SIGNAL(clicked()),  this, SLOT(OnTitleColor()));
	connect(m_pctrlLabelClr, SIGNAL(clicked()),  this, SLOT(OnLabelColor()));

	connect(m_pctrlTitleButton, SIGNAL(clicked()),  this, SLOT(OnTitleFont()));
	connect(m_pctrlLabelButton, SIGNAL(clicked()),  this, SLOT(OnLabelFont()));

	connect(m_pctrlXAuto, SIGNAL(clicked()), this, SLOT(OnAutoX()));
	connect(m_pctrlYAuto, SIGNAL(clicked()), this, SLOT(OnAutoY()));
	connect(m_pctrlYInverted, SIGNAL(clicked()), this, SLOT(OnYInverted()));

	connect(m_pctrlXMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMajGridShow(int)));
	connect(m_pctrlYMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMajGridShow(int)));
	connect(m_pctrlXMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMinGridShow(int)));
	connect(m_pctrlYMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMinGridShow(int)));

	connect(m_pctrlAxisStyle, SIGNAL(clicked()), this, SLOT(OnAxisStyle()));
	connect(m_pctrlXMajGridStyle, SIGNAL(clicked()), this, SLOT(OnXMajGridStyle()));
	connect(m_pctrlYMajGridStyle, SIGNAL(clicked()), this, SLOT(OnYMajGridStyle()));
	connect(m_pctrlXMinGridStyle, SIGNAL(clicked()), this, SLOT(OnXMinGridStyle()));
	connect(m_pctrlYMinGridStyle, SIGNAL(clicked()), this, SLOT(OnYMinGridStyle()));

	connect(m_pctrlAutoXMinUnit, SIGNAL(clicked()), this, SLOT(OnAutoMinGrid()));
	connect(m_pctrlAutoYMinUnit, SIGNAL(clicked()), this, SLOT(OnAutoMinGrid()));

	connect(m_pctrlGraphBorder, SIGNAL(stateChanged(int)), this, SLOT(OnGraphBorder(int)));
	connect(m_pctrlGraphBack, SIGNAL(clicked()), this, SLOT(OnGraphBackColor()));
	connect(m_pctrlBorderStyle, SIGNAL(clicked()), this, SLOT(OnBorderStyle()));

/*	connect(m_pctrlXSel, SIGNAL(itemActivated ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemActivated ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlXSel, SIGNAL(itemClicked ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemClicked ( QListWidgetItem*)), SLOT(OnVariableChanged()));*/
	connect(m_pctrlXSel, SIGNAL(itemSelectionChanged()), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemSelectionChanged()), SLOT(OnVariableChanged()));

	connect(m_pctrlXSel, SIGNAL(itemDoubleClicked (QListWidgetItem *)), SLOT(OnOK()));
	connect(m_pctrlYSel, SIGNAL(itemDoubleClicked (QListWidgetItem *)), SLOT(OnOK()));

	connect(RestoreButton, SIGNAL(clicked()),this, SLOT(OnRestoreParams()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ApplyButton, SIGNAL(clicked()), this, SLOT(OnApply()));
}




void GraphDlg::FillVariableList()
{
	m_pctrlXSel->clear();
	m_pctrlYSel->clear();
	if(m_iGraphType == 31)
	{
		m_pctrlXSel->addItem(tr("X - Chord"));
		m_pctrlYSel->addItem(tr("Q - Speed"));
	}
	else if(m_iGraphType==51)
	{
		//foil oppoint graph variables
		m_pctrlXSel->addItem(tr("X - chord"));
		m_pctrlYSel->addItem(tr("Cp"));
		m_pctrlYSel->addItem(tr("Q - Speed"));
	}
	else if(m_iGraphType==52)
	{
		//foil polar graph variables
		m_pctrlXSel->addItem(tr("Alpha"));
		m_pctrlXSel->addItem(tr("Cl"));
		m_pctrlXSel->addItem(tr("Cd"));
		m_pctrlXSel->addItem(tr("Cd x 10000"));
		m_pctrlXSel->addItem(tr("Cdp"));
		m_pctrlXSel->addItem(tr("Cm"));
		m_pctrlXSel->addItem(tr("Xtr1"));
		m_pctrlXSel->addItem(tr("Xtr2"));
		m_pctrlXSel->addItem(tr("HMom"));
		m_pctrlXSel->addItem(tr("Cpmin"));
		m_pctrlXSel->addItem(tr("Cl/Cd"));
		m_pctrlXSel->addItem(tr("|Cl|^(3/2)/Cd"));
		m_pctrlXSel->addItem(tr("1/Cl^1/2"));
		m_pctrlXSel->addItem(tr("Re"));
		m_pctrlXSel->addItem(tr("XCp"));

		m_pctrlYSel->addItem(tr("Alpha"));
		m_pctrlYSel->addItem(tr("Cl"));
		m_pctrlYSel->addItem(tr("Cd"));
		m_pctrlYSel->addItem(tr("Cd x 10000"));
		m_pctrlYSel->addItem(tr("Cdp"));
		m_pctrlYSel->addItem(tr("Cm"));
		m_pctrlYSel->addItem(tr("Xtr1"));
		m_pctrlYSel->addItem(tr("Xtr2"));
		m_pctrlYSel->addItem(tr("HMom"));
		m_pctrlYSel->addItem(tr("Cpmin"));
		m_pctrlYSel->addItem(tr("Cl/Cd"));
		m_pctrlYSel->addItem(tr("|Cl|^(3/2)/Cd"));
		m_pctrlYSel->addItem(tr("1/Cl^1/2"));
		m_pctrlYSel->addItem(tr("Re"));
		m_pctrlYSel->addItem(tr("XCp"));
	}
	else if(m_iGraphType==61)
	{
		//wing graph variable
		m_pctrlXSel->addItem(tr("Y - span"));

		m_pctrlYSel->addItem(tr("Induced Angle"));						//0
		m_pctrlYSel->addItem(tr("Total Angle"));						//1
		m_pctrlYSel->addItem(tr("Local lift coef."));					//2
		m_pctrlYSel->addItem(tr("Local Lift C.Cl/M.A.C."));				//3
		m_pctrlYSel->addItem(tr("Airfoil viscous drag coef."));			//4
		m_pctrlYSel->addItem(tr("Induced drag coef."));					//5
		m_pctrlYSel->addItem(tr("Total drag coef."));					//6
		m_pctrlYSel->addItem(tr("Local Drag C.Cd/M.A.C."));				//7
		m_pctrlYSel->addItem(tr("Airfoil Pitching moment coef."));		//8
		m_pctrlYSel->addItem(tr("Total Pitching moment coef."));		//9
		m_pctrlYSel->addItem(tr("Reynolds"));							//10
		m_pctrlYSel->addItem(tr("Top Transition x-pos%"));				//11
		m_pctrlYSel->addItem(tr("Bottom Transition x-pos%"));			//12
		m_pctrlYSel->addItem(tr("Centre of Pressure x-pos%"));			//13
		m_pctrlYSel->addItem(tr("Bending moment"));						//14
	}
	else if(m_iGraphType == 62)
	{
		//WingPolar Graph Variables
		m_pctrlXSel->addItem(tr("Alpha"));						//0
		m_pctrlXSel->addItem(tr("Lift coef."));					//1
		m_pctrlXSel->addItem(tr("Viscous drag coef."));			//2
		m_pctrlXSel->addItem(tr("Induced drag coef."));			//3
		m_pctrlXSel->addItem(tr("Total drag coef."));			//4

		m_pctrlXSel->addItem(tr("Total pitching moment coef."));//5
		m_pctrlXSel->addItem(tr("Viscous pitching moment coef."));//6
		m_pctrlXSel->addItem(tr("Induced pitching moment coef."));//7
		m_pctrlXSel->addItem(tr("Total rolling moment coef."));	//8
		m_pctrlXSel->addItem(tr("Total yawing moment coef."));  //9
		m_pctrlXSel->addItem(tr("Viscous yawing moment coef."));//10
		m_pctrlXSel->addItem(tr("Induced yawing moment coef."));//11

		m_pctrlXSel->addItem(tr("Glide ratio Cl/Cd"));			//12
		m_pctrlXSel->addItem(tr("Power factor Cl^(3/2)/Cd"));	//13
		m_pctrlXSel->addItem(tr("1/Rt(Cl)"));					//14
		m_pctrlXSel->addItem(tr("FX (Drag)"));					//15
		m_pctrlXSel->addItem(tr("FY (Side force)"));			//16
		m_pctrlXSel->addItem(tr("FZ (Lift)"));					//17
		m_pctrlXSel->addItem(tr("Vx"));							//18
		m_pctrlXSel->addItem(tr("Vz"));							//19
		m_pctrlXSel->addItem(tr("VInf"));						//20
		m_pctrlXSel->addItem(tr("Descent angle atan(Cd/Cl)"));	//21
		m_pctrlXSel->addItem(tr("Pitching Moment"));			//22
		m_pctrlXSel->addItem(tr("Rolling Moment"));				//23
		m_pctrlXSel->addItem(tr("Yawing Moment"));				//24
		m_pctrlXSel->addItem(tr("Centre of pressure X-Pos"));	//25
		m_pctrlXSel->addItem(tr("Centre of pressure Y-Pos"));	//26
		m_pctrlXSel->addItem(tr("Bending moment"));				//27
		m_pctrlXSel->addItem(tr("m.g.Vz"));						//28
		m_pctrlXSel->addItem(tr("Efficiency"));					//29
		m_pctrlXSel->addItem(tr("(XCp-Xcg)/MAC"));				//30
		m_pctrlXSel->addItem(tr("Control Variable"));			//31
		m_pctrlXSel->addItem(tr("Cy - Lateral force coef."));	//32
		m_pctrlXSel->addItem(tr("Neutral Point x-position"));	//33
		m_pctrlXSel->addItem(tr("Phugoid Frequency"));	        //34
		m_pctrlXSel->addItem(tr("Phugoid Damping"));	        //35
		m_pctrlXSel->addItem(tr("Short Period Frequency"));	    //36
		m_pctrlXSel->addItem(tr("Short Period Damping"));	    //37
		m_pctrlXSel->addItem(tr("Dutch Roll Frequency"));	    //38
		m_pctrlXSel->addItem(tr("Dutch Roll Damping"));         //39
		m_pctrlXSel->addItem(tr("Roll Damping"));	            //40
		m_pctrlXSel->addItem(tr("Spiral Damping"));	            //41


		//Y variable

		m_pctrlYSel->addItem(tr("Alpha"));						//0
		m_pctrlYSel->addItem(tr("Lift coef."));					//1
		m_pctrlYSel->addItem(tr("Viscous drag coef."));			//2
		m_pctrlYSel->addItem(tr("Induced drag coef."));			//3
		m_pctrlYSel->addItem(tr("Total drag coef."));			//4

		m_pctrlYSel->addItem(tr("Total pitching moment coef."));//5
		m_pctrlYSel->addItem(tr("Viscous pitching moment coef."));//6
		m_pctrlYSel->addItem(tr("Induced pitching moment coef."));//7
		m_pctrlYSel->addItem(tr("Total rolling moment coef."));	//8
		m_pctrlYSel->addItem(tr("Total yawing moment coef."));  //9
		m_pctrlYSel->addItem(tr("Viscous yawing moment coef."));//10
		m_pctrlYSel->addItem(tr("Induced yawing moment coef."));//11

		m_pctrlYSel->addItem(tr("Glide ratio Cl/Cd"));			//12
		m_pctrlYSel->addItem(tr("Power factor Cl^(3/2)/Cd"));	//13
		m_pctrlYSel->addItem(tr("1/Rt(Cl)"));					//14
		m_pctrlYSel->addItem(tr("FX (Drag)"));					//15
		m_pctrlYSel->addItem(tr("FY (Side force)"));			//16
		m_pctrlYSel->addItem(tr("FZ (Lift)"));					//17
		m_pctrlYSel->addItem(tr("Vx"));							//18
		m_pctrlYSel->addItem(tr("Vz"));							//19
		m_pctrlYSel->addItem(tr("VInf"));						//20
		m_pctrlYSel->addItem(tr("Descent angle atan(Cd/Cl)"));	//21
		m_pctrlYSel->addItem(tr("Pitching Moment"));			//22
		m_pctrlYSel->addItem(tr("Rolling Moment"));				//23
		m_pctrlYSel->addItem(tr("Yawing Moment"));				//24
		m_pctrlYSel->addItem(tr("Centre of pressure X-Pos"));	//25
		m_pctrlYSel->addItem(tr("Centre of pressure Y-Pos"));	//26
		m_pctrlYSel->addItem(tr("Bending moment"));				//27
		m_pctrlYSel->addItem(tr("m.g.Vz"));						//28
		m_pctrlYSel->addItem(tr("Efficiency"));					//29
		m_pctrlYSel->addItem(tr("(XCp-Xcg)/MAC"));				//30
		m_pctrlYSel->addItem(tr("Control Variable"));			//31
		m_pctrlYSel->addItem(tr("Cy - Lateral force coef."));	//32
		m_pctrlYSel->addItem(tr("Neutral Point x-position"));	//33
		m_pctrlYSel->addItem(tr("Phugoid Frequency"));	        //34
		m_pctrlYSel->addItem(tr("Phugoid Damping"));	        //35
		m_pctrlYSel->addItem(tr("Short Period Frequency"));	    //36
		m_pctrlYSel->addItem(tr("Short Period Damping"));	    //37
		m_pctrlYSel->addItem(tr("Dutch Roll Frequency"));	    //38
		m_pctrlYSel->addItem(tr("Dutch Roll Damping"));         //39
		m_pctrlYSel->addItem(tr("Roll Damping"));	            //40
		m_pctrlYSel->addItem(tr("Spiral Damping"));	            //41
	}
	else if(m_iGraphType == 64)
	{
		m_pctrlXSel->addItem(tr("X - Chord"));
		m_pctrlYSel->addItem(tr("Cp"));
	}

        /////////new code DM/////////
        else if(m_iGraphType == 70)
        {

                m_pctrlYSel->addItem(tr("Power Coefficient Cp"));	//13
                m_pctrlYSel->addItem(tr("Thrust Coefficient Ct "));
                m_pctrlYSel->addItem(tr("Torque Coefficient Cm"));
                m_pctrlYSel->addItem(tr("Kp"));
                m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio")); //12

                m_pctrlYSel->addItem(tr("Power"));	//13
                m_pctrlYSel->addItem(tr("Thrust"));
                m_pctrlYSel->addItem(tr("Torque"));
                m_pctrlYSel->addItem(tr("Rotational Speed"));
                m_pctrlYSel->addItem(tr("Windspeed"));
                m_pctrlYSel->addItem(tr("Root OOP Bending Moment")); //12



                m_pctrlXSel->addItem(tr("Power Coefficient Cp"));	//13
                m_pctrlXSel->addItem(tr("Thrust Coefficient Ct "));
                m_pctrlXSel->addItem(tr("Torque Coefficient Cm"));
                m_pctrlXSel->addItem(tr("Kp"));
                m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio")); //12

                m_pctrlXSel->addItem(tr("Power"));	//13
                m_pctrlXSel->addItem(tr("Thrust"));
                m_pctrlXSel->addItem(tr("Torque"));
                m_pctrlXSel->addItem(tr("Rotational Speed"));
                m_pctrlXSel->addItem(tr("Windspeed"));
                m_pctrlXSel->addItem(tr("Root OOP Bending Moment")); //12




        }
        else if(m_iGraphType == 71)
        {
                m_pctrlYSel->addItem(tr("Axial induction factor"));					//14
                m_pctrlYSel->addItem(tr("Radial Position"));						//15
                m_pctrlYSel->addItem(tr("Tangential Induction Factor"));						//16
                m_pctrlYSel->addItem(tr("Local Tip Speed Ratio in Blade"));							//17
                m_pctrlYSel->addItem(tr("Axial Blade Force Coeff"));							//18
                m_pctrlYSel->addItem(tr("Tangential Blade Force Coeff"));						//19
                m_pctrlYSel->addItem(tr("Inflow Angle Phi"));	//20
                m_pctrlYSel->addItem(tr("Angle of Attack Alpha"));			//21
                m_pctrlYSel->addItem(tr("Blade Twist Angle Theta"));				//22
                m_pctrlYSel->addItem(tr("Chord along Blade"));
                m_pctrlYSel->addItem(tr("Lift Coefficient"));
                m_pctrlYSel->addItem(tr("Drag Coefficient"));
                m_pctrlYSel->addItem(tr("Lift to Drag Ratio"));
                m_pctrlYSel->addItem(tr("Prandtl Tip Loss Factor"));

                m_pctrlYSel->addItem(tr("Reynolds Number"));
                m_pctrlYSel->addItem(tr("Delta Re (Re Blade - Re Polar)"));
                m_pctrlYSel->addItem(tr("Tangential Force"));
                m_pctrlYSel->addItem(tr("Normal Force"));
                m_pctrlYSel->addItem(tr("Circulation"));
                m_pctrlYSel->addItem(tr("Local Inflow Speed"));



                m_pctrlYSel->addItem(tr("Iterations"));
                m_pctrlYSel->addItem(tr("Annulus averaged axial ind."));
                m_pctrlYSel->addItem(tr("Annulus averaged tangential ind."));


                m_pctrlXSel->addItem(tr("Axial induction factor"));					//14
                m_pctrlXSel->addItem(tr("Radial Position"));						//15
                m_pctrlXSel->addItem(tr("Tangential Induction Factor"));						//16
                m_pctrlXSel->addItem(tr("Local Tip Speed Ratio in Blade"));							//17
                m_pctrlXSel->addItem(tr("Axial Blade Force Coeff"));							//18
                m_pctrlXSel->addItem(tr("Tangential Blade Force Coeff"));						//19
                m_pctrlXSel->addItem(tr("Inflow Angle Phi"));	//20
                m_pctrlXSel->addItem(tr("Angle of Attack Alpha"));			//21
                m_pctrlXSel->addItem(tr("Blade Twist Angle Theta"));				//22
                m_pctrlXSel->addItem(tr("Chord along Blade"));	//23
                m_pctrlXSel->addItem(tr("Lift Coefficient"));
                m_pctrlXSel->addItem(tr("Drag Coefficient"));
                m_pctrlXSel->addItem(tr("Lift to Drag Ratio"));
                m_pctrlXSel->addItem(tr("Prandtl Tip Loss Factor"));

                m_pctrlXSel->addItem(tr("Reynolds Number"));
                m_pctrlXSel->addItem(tr("Delta Re (Re Blade - Re Polar)"));
                m_pctrlXSel->addItem(tr("Tangential Force"));
                m_pctrlXSel->addItem(tr("Normal Force"));
                m_pctrlXSel->addItem(tr("Circulation"));
                m_pctrlXSel->addItem(tr("Local Inflow Speed"));

                m_pctrlXSel->addItem(tr("Iterations"));
                m_pctrlXSel->addItem(tr("Annulus averaged axial ind."));
                m_pctrlXSel->addItem(tr("Annulus averaged tangential ind."));



        }

        else if(m_iGraphType == 80)
        {
                m_pctrlYSel->addItem(tr("Power"));
                m_pctrlYSel->addItem(tr("Thrust")); //12
                m_pctrlYSel->addItem(tr("Torque"));	//13
                m_pctrlYSel->addItem(tr("Bending Moment"));
                m_pctrlYSel->addItem(tr("Windspeed"));	//13
                m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio"));
                m_pctrlYSel->addItem(tr("Rotational Speed"));
                m_pctrlYSel->addItem(tr("Pitch Angle"));
                m_pctrlYSel->addItem(tr("Power Coefficient Cp"));						//15
                m_pctrlYSel->addItem(tr("Thrust Coefficient Ct"));
                m_pctrlYSel->addItem(tr("Torque Coefficient Cm"));
                m_pctrlYSel->addItem(tr("Kp"));
                m_pctrlYSel->addItem(tr("Cp including losses"));
                m_pctrlYSel->addItem(tr("f Weibull"));
				//m_pctrlYSel->addItem(tr("f Weibull x V^3"));


                m_pctrlXSel->addItem(tr("Power"));
                m_pctrlXSel->addItem(tr("Thrust")); //12
                m_pctrlXSel->addItem(tr("Torque"));	//13
                m_pctrlXSel->addItem(tr("Bending Moment"));
                m_pctrlXSel->addItem(tr("Windspeed"));	//13
                m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio"));
                m_pctrlXSel->addItem(tr("Rotational Speed"));
                m_pctrlXSel->addItem(tr("Pitch Angle"));
                m_pctrlXSel->addItem(tr("Power Coefficient Cp"));						//15
                m_pctrlXSel->addItem(tr("Thrust Coefficient Ct"));
                m_pctrlXSel->addItem(tr("Torque Coefficient Cm"));
                m_pctrlXSel->addItem(tr("Kp"));
                m_pctrlXSel->addItem(tr("Cp including losses"));
                m_pctrlXSel->addItem(tr("f Weibull"));
				//m_pctrlXSel->addItem(tr("f Weibull x V^3"));





        }
        else if(m_iGraphType == 81)
        {
                m_pctrlYSel->addItem(tr("Axial Blade Force Coeff"));							//18
                m_pctrlYSel->addItem(tr("Tangential Blade Force Coeff"));						//19
                m_pctrlYSel->addItem(tr("Inflow Angle Phi"));	//20
                m_pctrlYSel->addItem(tr("Angle of Attack Alpha"));			//21
                m_pctrlYSel->addItem(tr("Blade Twist Angle Theta"));				//22
                m_pctrlYSel->addItem(tr("Chord along Blade"));	//23
                m_pctrlYSel->addItem(tr("Lift Coefficient"));
                m_pctrlYSel->addItem(tr("Drag Coefficient"));
                m_pctrlYSel->addItem(tr("Lift to Drag Ratio"));
                m_pctrlYSel->addItem(tr("Prandtl Tip Loss Factor"));
                m_pctrlYSel->addItem(tr("Axial induction factor"));
                m_pctrlYSel->addItem(tr("Tangential Induction Factor"));
                m_pctrlYSel->addItem(tr("Radial Position"));
                m_pctrlYSel->addItem(tr("Local Tip Speed Ratio in Blade"));
                m_pctrlYSel->addItem(tr("Reynolds Number"));
                m_pctrlYSel->addItem(tr("Re Polar - Re Blade"));
                m_pctrlYSel->addItem(tr("Critical Roughness"));
                m_pctrlYSel->addItem(tr("Resultant Velocity at Section"));
                m_pctrlYSel->addItem(tr("Tangential Blade Force"));
                m_pctrlYSel->addItem(tr("Axial Blade Force"));
                m_pctrlYSel->addItem(tr("Iterations"));
                m_pctrlYSel->addItem(tr("Mach Number"));
                m_pctrlYSel->addItem(tr("Circulation"));
                m_pctrlYSel->addItem(tr("Annulus averaged axial ind."));
                m_pctrlYSel->addItem(tr("Annulus averaged tangential ind."));









                m_pctrlXSel->addItem(tr("Axial Blade Force Coeff"));							//18
                m_pctrlXSel->addItem(tr("Tangential Blade Force Coeff"));						//19
                m_pctrlXSel->addItem(tr("Inflow Angle Phi"));	//20
                m_pctrlXSel->addItem(tr("Angle of Attack Alpha"));			//21
                m_pctrlXSel->addItem(tr("Blade Twist Angle Theta"));				//22
                m_pctrlXSel->addItem(tr("Chord along Blade"));	//23
                m_pctrlXSel->addItem(tr("Lift Coefficient"));
                m_pctrlXSel->addItem(tr("Drag Coefficient"));
                m_pctrlXSel->addItem(tr("Lift to Drag Ratio"));
                m_pctrlXSel->addItem(tr("Prandtl Tip Loss Factor"));
                m_pctrlXSel->addItem(tr("Axial induction factor"));
                m_pctrlXSel->addItem(tr("Tangential Induction Factor"));
                m_pctrlXSel->addItem(tr("Radial Position"));
                m_pctrlXSel->addItem(tr("Local Tip Speed Ratio in Blade"));
                m_pctrlXSel->addItem(tr("Reynolds Number"));
                m_pctrlXSel->addItem(tr("Re Polar - Re Blade"));
                m_pctrlXSel->addItem(tr("Critical Roughness"));
                m_pctrlXSel->addItem(tr("Resultant Velocity at Section"));
                m_pctrlXSel->addItem(tr("Tangential Blade Force"));
                m_pctrlXSel->addItem(tr("Axial Blade Force"));
                m_pctrlXSel->addItem(tr("Iterations"));
                m_pctrlXSel->addItem(tr("Mach Number"));
                m_pctrlXSel->addItem(tr("Circulation"));
                m_pctrlXSel->addItem(tr("Annulus averaged axial ind."));
                m_pctrlXSel->addItem(tr("Annulus averaged tangential ind."));





        }

        else if(m_iGraphType == 90)
        {
                m_pctrlYSel->addItem(tr("Angle of Attack Alpha"));
                m_pctrlYSel->addItem(tr("Cl")); //12
                m_pctrlYSel->addItem(tr("Cd"));	//13
                m_pctrlYSel->addItem(tr("Cl / Cd"));
                m_pctrlYSel->addItem(tr("Cl attached"));
                m_pctrlYSel->addItem(tr("Cl separated"));
                m_pctrlYSel->addItem(tr("f function"));
                                                //17



                m_pctrlXSel->addItem(tr("Angle of Attack Alpha"));
                m_pctrlXSel->addItem(tr("Cl")); //12
                m_pctrlXSel->addItem(tr("Cd"));	//13
                m_pctrlXSel->addItem(tr("Cl / Cd"));
                m_pctrlXSel->addItem(tr("Cl attached"));
                m_pctrlXSel->addItem(tr("Cl separated"));
                m_pctrlXSel->addItem(tr("f function"));
                                                        //17






        }

        else if(m_iGraphType == 100 || m_iGraphType==101 || m_iGraphType==102)
        {
                m_pctrlYSel->addItem(tr("Power"));
                m_pctrlYSel->addItem(tr("Thrust")); //12
                m_pctrlYSel->addItem(tr("Rotor Torque"));
                m_pctrlYSel->addItem(tr("Blade Bending Moment")); //12
                m_pctrlYSel->addItem(tr("Windspeed"));	//13
                m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio"));
                m_pctrlYSel->addItem(tr("Rotational Speed"));
                m_pctrlYSel->addItem(tr("Pitch Angle"));
                m_pctrlYSel->addItem(tr("Cp")); //12
                m_pctrlYSel->addItem(tr("Ct"));	//13
                m_pctrlYSel->addItem(tr("Cm"));
                m_pctrlYSel->addItem(tr("Dimensionless Power Coefficent Kp"));	//13






                m_pctrlXSel->addItem(tr("Power"));
                m_pctrlXSel->addItem(tr("Thrust")); //12
                m_pctrlXSel->addItem(tr("Rotor Torque"));
                m_pctrlXSel->addItem(tr("Blade Bending Moment")); //12
                m_pctrlXSel->addItem(tr("Windspeed"));	//13
                m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
                m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio"));
                m_pctrlXSel->addItem(tr("Rotational Speed"));
                m_pctrlXSel->addItem(tr("Pitch Angle"));
                m_pctrlXSel->addItem(tr("Cp")); //12
                m_pctrlXSel->addItem(tr("Ct"));	//13
                m_pctrlXSel->addItem(tr("Cm"));
                m_pctrlXSel->addItem(tr("Dimensionless Power Coefficent Kp"));	//13







        }
        //////////end new code DM//////////

        ///// new code JW /////
        // rotor graph rotor sim
        else if (m_iGraphType == 103)
        {
            m_pctrlXSel->addItem(tr("Cp"));
            m_pctrlXSel->addItem(tr("Cp upwind"));
            m_pctrlXSel->addItem(tr("Cp downwind"));
            m_pctrlXSel->addItem(tr("Cm"));
            m_pctrlXSel->addItem(tr("Cm upwind"));
            m_pctrlXSel->addItem(tr("Cm downwind"));
            m_pctrlXSel->addItem(tr("Kp"));
            m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
            m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio"));
            m_pctrlXSel->addItem(tr("Power"));
            m_pctrlXSel->addItem(tr("Thrust"));
            m_pctrlXSel->addItem(tr("Torque"));
            m_pctrlXSel->addItem(tr("Rotational Speed"));
            m_pctrlXSel->addItem(tr("Windspeed"));

            m_pctrlYSel->addItem(tr("Cp"));
            m_pctrlYSel->addItem(tr("Cp upwind"));
            m_pctrlYSel->addItem(tr("Cp downwind"));
            m_pctrlYSel->addItem(tr("Cm"));
            m_pctrlYSel->addItem(tr("Cm upwind"));
            m_pctrlYSel->addItem(tr("Cm downwind"));
            m_pctrlYSel->addItem(tr("Kp"));
            m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
            m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio"));
            m_pctrlYSel->addItem(tr("Power"));
            m_pctrlYSel->addItem(tr("Thrust"));
            m_pctrlYSel->addItem(tr("Torque"));
            m_pctrlYSel->addItem(tr("Rotational Speed"));
            m_pctrlYSel->addItem(tr("Windspeed"));


        }
        // blade graph rotor sim
        else if (m_iGraphType == 104)
        {
            m_pctrlXSel->addItem(tr("relative height h/H"));
            m_pctrlXSel->addItem(tr("relative radius r/R"));
            m_pctrlXSel->addItem(tr("chord c"));
            m_pctrlXSel->addItem(tr("inclination angle delta"));
            m_pctrlXSel->addItem(tr("interference factor u_up"));
            m_pctrlXSel->addItem(tr("interference factor u_dw"));
            m_pctrlXSel->addItem(tr("induction factor a_up"));
            m_pctrlXSel->addItem(tr("induction factor a_dw"));
            m_pctrlXSel->addItem(tr("inflow velocity V_inf"));
            m_pctrlXSel->addItem(tr("induced velocity V_up"));
            m_pctrlXSel->addItem(tr("equilibrium velocity V_e"));
            m_pctrlXSel->addItem(tr("induced velocity V_dw"));
            m_pctrlXSel->addItem(tr("wake velocity V_wake"));
            m_pctrlXSel->addItem(tr("tipspeed ratio TSR_up"));
            m_pctrlXSel->addItem(tr("tipspeed ratio TSR_dw"));
            m_pctrlXSel->addItem(tr("swept area S"));
            m_pctrlXSel->addItem(tr("tiploss factor F_up"));
            m_pctrlXSel->addItem(tr("tiploss factor F_dw"));
            m_pctrlXSel->addItem(tr("upwind iterations"));
            m_pctrlXSel->addItem(tr("downwind iterations"));


            m_pctrlYSel->addItem(tr("relative height h/H"));
            m_pctrlYSel->addItem(tr("relative radius r/R"));
            m_pctrlYSel->addItem(tr("chord c"));
            m_pctrlYSel->addItem(tr("inclination angle delta"));
            m_pctrlYSel->addItem(tr("interference factor u_up"));
            m_pctrlYSel->addItem(tr("interference factor u_dw"));
            m_pctrlYSel->addItem(tr("induction factor a_up"));
            m_pctrlYSel->addItem(tr("induction factor a_dw"));
            m_pctrlYSel->addItem(tr("inflow velocity V_inf"));
            m_pctrlYSel->addItem(tr("induced velocity V_up"));
            m_pctrlYSel->addItem(tr("equilibrium velocity V_e"));
            m_pctrlYSel->addItem(tr("induced velocity V_dw"));
            m_pctrlYSel->addItem(tr("wake velocity V_wake"));
            m_pctrlYSel->addItem(tr("tipspeed ratio TSR_up"));
            m_pctrlYSel->addItem(tr("tipspeed ratio TSR_dw"));
            m_pctrlYSel->addItem(tr("swept area S"));
            m_pctrlYSel->addItem(tr("tiploss factor F_up"));
            m_pctrlYSel->addItem(tr("tiploss factor F_dw"));
            m_pctrlYSel->addItem(tr("upwind iterations"));
            m_pctrlYSel->addItem(tr("downwind iterations"));

        }

        // azimuthal graph rotor sim
        else if (m_iGraphType == 105)
        {
            m_pctrlXSel->addItem(tr("azimuthal angle theta"));
            m_pctrlXSel->addItem(tr("iterations"));
            m_pctrlXSel->addItem(tr("interference factor u"));
            m_pctrlXSel->addItem(tr("induced velocity V"));
            m_pctrlXSel->addItem(tr("relative velocity W"));
            m_pctrlXSel->addItem(tr("Reynolds number Re"));
            m_pctrlXSel->addItem(tr("Delta Re (Re Blade - Re Polar)"));
            m_pctrlXSel->addItem(tr("tiploss factor F"));
            m_pctrlXSel->addItem(tr("angle of attack alpha"));
            m_pctrlXSel->addItem(tr("lift coefficient Cl"));
            m_pctrlXSel->addItem(tr("drag coefficient Cd"));
            m_pctrlXSel->addItem(tr("lift/drag ratio (Cl/Cd)"));
            m_pctrlXSel->addItem(tr("normal force coefficient Cn"));
            m_pctrlXSel->addItem(tr("tangential force coefficient Ct"));
			m_pctrlXSel->addItem(tr("blade tangential force coefficient CF_t_bla"));
			m_pctrlXSel->addItem(tr("blade lengthwise force coefficient CF_x_bla"));
			m_pctrlXSel->addItem(tr("blade crosswise force coefficient CF_y_bla"));
			m_pctrlXSel->addItem(tr("rotor tangential force coefficient CF_t_rot"));
			m_pctrlXSel->addItem(tr("rotor lengthwise force coefficient CF_x_rot"));
			m_pctrlXSel->addItem(tr("rotor crosswise force coefficient CF_y_rot"));


            m_pctrlYSel->addItem(tr("azimuthal angle theta"));
            m_pctrlYSel->addItem(tr("iterations"));
            m_pctrlYSel->addItem(tr("interference factor u"));
            m_pctrlYSel->addItem(tr("induced velocity V"));
            m_pctrlYSel->addItem(tr("relative velocity W"));
            m_pctrlYSel->addItem(tr("Reynolds number Re"));
            m_pctrlYSel->addItem(tr("Delta Re (Re Blade - Re Polar)"));
            m_pctrlYSel->addItem(tr("tiploss factor F"));
            m_pctrlYSel->addItem(tr("angle of attack alpha"));
            m_pctrlYSel->addItem(tr("lift coefficient Cl"));
            m_pctrlYSel->addItem(tr("drag coefficient Cd"));
            m_pctrlYSel->addItem(tr("lift/drag ratio (Cl/Cd)"));
            m_pctrlYSel->addItem(tr("normal force coefficient Cn"));
            m_pctrlYSel->addItem(tr("tangential force coefficient Ct"));
			m_pctrlYSel->addItem(tr("blade tangential force coefficient CF_t_bla"));
			m_pctrlYSel->addItem(tr("blade lengthwise force coefficient CF_x_bla"));
			m_pctrlYSel->addItem(tr("blade crosswise force coefficient CF_y_bla"));
			m_pctrlYSel->addItem(tr("rotor tangential force coefficient CF_t_rot"));
			m_pctrlYSel->addItem(tr("rotor lengthwise force coefficient CF_x_rot"));
			m_pctrlYSel->addItem(tr("rotor crosswise force coefficient CF_y_rot"));
        }

        // rotor graph turbine sim
        else if (m_iGraphType == 106)
        {
            m_pctrlXSel->addItem(tr("Power"));
            m_pctrlXSel->addItem(tr("Torque"));
            m_pctrlXSel->addItem(tr("Windspeed"));
            m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
            m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio"));
            m_pctrlXSel->addItem(tr("Rotational Speed"));
            m_pctrlXSel->addItem(tr("Cp"));
            m_pctrlXSel->addItem(tr("Cp upwind"));
            m_pctrlXSel->addItem(tr("Cp downwind"));
//            m_pctrlXSel->addItem(tr("Ct"));
//            m_pctrlXSel->addItem(tr("Ct upwind"));
//            m_pctrlXSel->addItem(tr("Ct downwind"));
            m_pctrlXSel->addItem(tr("Cm"));
            m_pctrlXSel->addItem(tr("Cm upwind"));
            m_pctrlXSel->addItem(tr("Cm downwind"));
            m_pctrlXSel->addItem(tr("Kp"));
            m_pctrlXSel->addItem(tr("Power P_loss"));
            m_pctrlXSel->addItem(tr("Power loss coefficient Cp_loss"));
            m_pctrlXSel->addItem(tr("f Weibull"));
			//m_pctrlXSel->addItem(tr("f Weibull x V^3"));



            m_pctrlYSel->addItem(tr("Power"));
            m_pctrlYSel->addItem(tr("Torque"));
            m_pctrlYSel->addItem(tr("Windspeed"));
            m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
            m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio"));
            m_pctrlYSel->addItem(tr("Rotational Speed"));
            m_pctrlYSel->addItem(tr("Cp"));
            m_pctrlYSel->addItem(tr("Cp upwind"));
            m_pctrlYSel->addItem(tr("Cp downwind"));
//            m_pctrlYSel->addItem(tr("Ct"));
//            m_pctrlYSel->addItem(tr("Ct upwind"));
//            m_pctrlYSel->addItem(tr("Ct downwind"));
            m_pctrlYSel->addItem(tr("Cm"));
            m_pctrlYSel->addItem(tr("Cm upwind"));
            m_pctrlYSel->addItem(tr("Cm downwind"));
            m_pctrlYSel->addItem(tr("Kp"));
            m_pctrlYSel->addItem(tr("Power P_loss"));
            m_pctrlYSel->addItem(tr("Power loss coefficient Cp_loss"));
            m_pctrlYSel->addItem(tr("f Weibull"));
			//m_pctrlYSel->addItem(tr("f Weibull x V^3"));
        }

        // blade graph turbine sim
        else if (m_iGraphType == 107)
        {
            m_pctrlXSel->addItem(tr("height h"));
            m_pctrlXSel->addItem(tr("radius r"));
            m_pctrlXSel->addItem(tr("chord c"));
            m_pctrlXSel->addItem(tr("inclination angle delta"));
            m_pctrlXSel->addItem(tr("interference factor u_up"));
            m_pctrlXSel->addItem(tr("interference factor u_dw"));
            m_pctrlXSel->addItem(tr("induction factor a_up"));
            m_pctrlXSel->addItem(tr("induction factor a_dw"));
            m_pctrlXSel->addItem(tr("inflow velocity V_inf"));
            m_pctrlXSel->addItem(tr("induced velocity V_up"));
            m_pctrlXSel->addItem(tr("equilibrium velocity V_e"));
            m_pctrlXSel->addItem(tr("induced velocity V_dw"));
            m_pctrlXSel->addItem(tr("wake velocity V_wake"));
            m_pctrlXSel->addItem(tr("tipspeed ratio TSR_up"));
            m_pctrlXSel->addItem(tr("tipspeed ratio TSR_dw"));
            m_pctrlXSel->addItem(tr("swept area S"));
            m_pctrlXSel->addItem(tr("tiploss factor F_up"));
            m_pctrlXSel->addItem(tr("tiploss factor F_dw"));
            m_pctrlXSel->addItem(tr("upwind iterations"));
            m_pctrlXSel->addItem(tr("downwind iterations"));

            m_pctrlYSel->addItem(tr("height h"));
            m_pctrlYSel->addItem(tr("radius r"));
            m_pctrlYSel->addItem(tr("chord c"));
            m_pctrlYSel->addItem(tr("inclination angle delta"));
            m_pctrlYSel->addItem(tr("interference factor u_up"));
            m_pctrlYSel->addItem(tr("interference factor u_dw"));
            m_pctrlYSel->addItem(tr("induction factor a_up"));
            m_pctrlYSel->addItem(tr("induction factor a_dw"));
            m_pctrlYSel->addItem(tr("inflow velocity V_inf"));
            m_pctrlYSel->addItem(tr("induced velocity V_up"));
            m_pctrlYSel->addItem(tr("equilibrium velocity V_e"));
            m_pctrlYSel->addItem(tr("induced velocity V_dw"));
            m_pctrlYSel->addItem(tr("wake velocity V_wake"));
            m_pctrlYSel->addItem(tr("tipspeed ratio TSR_up"));
            m_pctrlYSel->addItem(tr("tipspeed ratio TSR_dw"));
            m_pctrlYSel->addItem(tr("swept area S"));
            m_pctrlYSel->addItem(tr("tiploss factor F_up"));
            m_pctrlYSel->addItem(tr("tiploss factor F_dw"));
            m_pctrlYSel->addItem(tr("upwind iterations"));
            m_pctrlYSel->addItem(tr("downwind iterations"));

        }

        // azimuthal graph turbine sim
        else if (m_iGraphType == 108)
        {

            m_pctrlXSel->addItem(tr("azimuthal angle theta"));
            m_pctrlXSel->addItem(tr("iterations"));
            m_pctrlXSel->addItem(tr("interference factor u"));
            m_pctrlXSel->addItem(tr("induced velocity V"));
            m_pctrlXSel->addItem(tr("relative velocity W"));
            m_pctrlXSel->addItem(tr("Reynolds number Re"));
            m_pctrlXSel->addItem(tr("tiploss factor F"));
            m_pctrlXSel->addItem(tr("angle of attack alpha"));
            m_pctrlXSel->addItem(tr("lift coefficient Cl"));
            m_pctrlXSel->addItem(tr("drag coefficient Cd"));
            m_pctrlXSel->addItem(tr("lift/drag ratio (Cl/Cd)"));
            m_pctrlXSel->addItem(tr("normal force coefficient Cn"));
            m_pctrlXSel->addItem(tr("tangential force coefficient Ct"));
//            m_pctrlXSel->addItem(tr("blade normal force coefficient F_n"));
			m_pctrlXSel->addItem(tr("blade tangential force coefficient CF_t_bla"));
			m_pctrlXSel->addItem(tr("blade lengthwise force coefficient CF_x_bla"));
			m_pctrlXSel->addItem(tr("blade crosswise force coefficient CF_y_bla"));
			m_pctrlXSel->addItem(tr("blade torque Tq_bla"));
			m_pctrlXSel->addItem(tr("blade lengthwise force F_x_bla"));
			m_pctrlXSel->addItem(tr("blade crosswise force F_y_bla"));
//            m_pctrlXSel->addItem(tr("rotor normal force coefficient F_nTot"));
			m_pctrlXSel->addItem(tr("rotor tangential force coefficient CF_t_rot"));
			m_pctrlXSel->addItem(tr("rotor lengthwise force coefficient CF_x_rot"));
			m_pctrlXSel->addItem(tr("rotor crosswise force coefficient CF_y_rot"));
			m_pctrlXSel->addItem(tr("rotor torque Tq_rot"));
			m_pctrlXSel->addItem(tr("rotor lengthwise force F_x_rot"));
			m_pctrlXSel->addItem(tr("rotor crosswise force F_y_rot"));


            m_pctrlYSel->addItem(tr("azimuthal angle theta"));
            m_pctrlYSel->addItem(tr("iterations"));
            m_pctrlYSel->addItem(tr("interference factor u"));
            m_pctrlYSel->addItem(tr("induced velocity V"));
            m_pctrlYSel->addItem(tr("relative velocity W"));
            m_pctrlYSel->addItem(tr("Reynolds number Re"));
            m_pctrlYSel->addItem(tr("tiploss factor F"));
            m_pctrlYSel->addItem(tr("angle of attack alpha"));
            m_pctrlYSel->addItem(tr("lift coefficient Cl"));
            m_pctrlYSel->addItem(tr("drag coefficient Cd"));
            m_pctrlYSel->addItem(tr("lift/drag ratio (Cl/Cd)"));
            m_pctrlYSel->addItem(tr("normal force coefficient Cn"));
            m_pctrlYSel->addItem(tr("tangential force coefficient Ct"));
//            m_pctrlYSel->addItem(tr("blade normal force coefficient F_n"));
			m_pctrlYSel->addItem(tr("blade tangential force coefficient CF_t_bla"));
			m_pctrlYSel->addItem(tr("blade lengthwise force coefficient CF_x_bla"));
			m_pctrlYSel->addItem(tr("blade crosswise force coefficient CF_y_bla"));
			m_pctrlYSel->addItem(tr("blade torque Tq_bla"));
			m_pctrlYSel->addItem(tr("blade lengthwise force F_x"));
			m_pctrlYSel->addItem(tr("blade crosswise force F_y"));
//			  m_pctrlYSel->addItem(tr("rotor normal force coefficient F_nTot"));
			m_pctrlYSel->addItem(tr("rotor tangential force coefficient CF_t_rot"));
			m_pctrlYSel->addItem(tr("rotor lengthwise force coefficient CF_x_rot"));
			m_pctrlYSel->addItem(tr("rotor crosswise force coefficient CF_y_rot"));
			m_pctrlYSel->addItem(tr("rotor torque Tq_rot"));
			m_pctrlYSel->addItem(tr("rotor lengthwise force F_x_rot"));
			m_pctrlYSel->addItem(tr("rotor crosswise force F_y_rot"));

        }


	else if(m_iGraphType == 109 || m_iGraphType==110 || m_iGraphType==111)
	{
		m_pctrlXSel->addItem(tr("Power"));
		m_pctrlXSel->addItem(tr("Rotor Torque"));
		m_pctrlXSel->addItem(tr("Windspeed"));
		m_pctrlXSel->addItem(tr("Tip Speed Ratio"));
		m_pctrlXSel->addItem(tr("1 / Tip Speed Ratio"));
		m_pctrlXSel->addItem(tr("Rotational Speed"));
		m_pctrlXSel->addItem(tr("Pitch Angle"));
		m_pctrlXSel->addItem(tr("Power Coefficent Cp"));
		m_pctrlXSel->addItem(tr("Power Coefficent Cp1"));
		m_pctrlXSel->addItem(tr("Power Coefficent Cp2"));
		m_pctrlXSel->addItem(tr("Torque Coefficient Cm"));
		m_pctrlXSel->addItem(tr("Torque Coefficient Cm1"));
		m_pctrlXSel->addItem(tr("Torque Coefficient Cm2"));
		m_pctrlXSel->addItem(tr("Dimensionless Power Coefficent Kp"));

		m_pctrlYSel->addItem(tr("Power"));
		m_pctrlYSel->addItem(tr("Rotor Torque"));
		m_pctrlYSel->addItem(tr("Windspeed"));
		m_pctrlYSel->addItem(tr("Tip Speed Ratio"));
		m_pctrlYSel->addItem(tr("1 / Tip Speed Ratio"));
		m_pctrlYSel->addItem(tr("Rotational Speed"));
		m_pctrlYSel->addItem(tr("Pitch Angle"));
		m_pctrlYSel->addItem(tr("Power Coefficent Cp"));
		m_pctrlYSel->addItem(tr("Power Coefficent Cp1"));
		m_pctrlYSel->addItem(tr("Power Coefficent Cp2"));
		m_pctrlYSel->addItem(tr("Torque Coefficient Cm"));
		m_pctrlYSel->addItem(tr("Torque Coefficient Cm1"));
		m_pctrlYSel->addItem(tr("Torque Coefficient Cm2"));
		m_pctrlYSel->addItem(tr("Dimensionless Power Coefficent Kp"));

	}

	else if(m_iGraphType == 112)
	{
		m_pctrlXSel->addItem(tr("shape factor k"));
		m_pctrlXSel->addItem(tr("scale factor A"));

		m_pctrlYSel->addItem(tr("Annual Energy Production AEP"));

	}
	///// end new code JW /////

	m_pctrlXSel->adjustSize();
	m_pctrlYSel->adjustSize();
}





void GraphDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	// Generate the foil instead
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else
			{
				QDialog::accept();
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			break;
		}
		default:
			event->ignore();
	}
}


void GraphDlg::OnActivePage(int index)
{
	s_ActivePage = index;
	ApplyButton->setEnabled(m_pTabWidget->currentIndex()!=0);
}


void GraphDlg::OnApply()
{
	m_pGraph->SetAutoX(m_pctrlXAuto->isChecked());
	m_pGraph->SetXMin(m_pctrlXMin->getValue());
	m_pGraph->SetXMax(m_pctrlXMax->getValue());
	m_pGraph->SetX0(m_pctrlXOrigin->getValue());
	m_pGraph->SetXUnit(m_pctrlXUnit->getValue());

	m_pGraph->SetAutoY(m_pctrlYAuto->isChecked());
	m_pGraph->SetYMin(m_pctrlYMin->getValue());
	m_pGraph->SetYMax(m_pctrlYMax->getValue());
	m_pGraph->SetY0(m_pctrlYOrigin->getValue());
	m_pGraph->SetYUnit(m_pctrlYUnit->getValue());

	double MinUnit;
	if(!m_pctrlAutoXMinUnit->isChecked())
	{
		MinUnit = m_pctrlXMinorUnit->getValue();
		m_pGraph->SetXMinorUnit(MinUnit);
	}
	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->getValue();
		m_pGraph->SetYMinorUnit(MinUnit);
	}

	m_pGraph->SetInverted(m_pctrlYInverted->isChecked());

	for(int i=0; i<m_NGraph; i++)
	{
		if (m_GraphArray[i]) {
			m_GraphArray[i]->CopySettings(m_pGraph);
		}
	}

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->UpdateView();
	SetApplied(true);
}


void GraphDlg::OnAutoMinGrid()
{
	bool bAuto;
	bAuto = m_pctrlAutoXMinUnit->isChecked();
	m_pGraph->SetAutoXMinUnit(bAuto);
	m_pctrlXMinorUnit->setEnabled(!bAuto);

	bAuto = m_pctrlAutoYMinUnit->isChecked();
	m_pGraph->SetAutoYMinUnit(bAuto);
	m_pctrlYMinorUnit->setEnabled(!bAuto);
}


void GraphDlg::OnAutoX()
{
	bool bAuto = m_pctrlXAuto->checkState() == Qt::Checked;
	m_pctrlXMin->setEnabled(!bAuto);
	m_pctrlXMax->setEnabled(!bAuto);
	m_pctrlXUnit->setEnabled(!bAuto);
	m_pctrlXOrigin->setEnabled(!bAuto);
	SetApplied(false);
}


void GraphDlg::OnAutoY()
{
	bool bAuto = m_pctrlYAuto->checkState() == Qt::Checked;
	m_pctrlYMin->setEnabled(!bAuto);
	m_pctrlYMax->setEnabled(!bAuto);
	m_pctrlYUnit->setEnabled(!bAuto);
	m_pctrlYOrigin->setEnabled(!bAuto);
	SetApplied(false);
}

void GraphDlg::OnAxisStyle()
{
	LinePickerDlg dlg;
	dlg.InitDialog(m_pGraph->GetAxisStyle(), m_pGraph->GetAxisWidth(), m_pGraph->GetAxisColor());

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetAxisData(dlg.GetStyle(), dlg.GetWidth(), dlg.GetColor());
		m_pctrlAxisStyle->SetStyle(dlg.GetStyle());
		m_pctrlAxisStyle->SetWidth(dlg.GetWidth());
		m_pctrlAxisStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnBorderStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	s = m_pGraph->GetBorderStyle();
	w = m_pGraph->GetBorderWidth();
	color = m_pGraph->GetBorderColor();
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetBorderColor(dlg.GetColor());
		m_pGraph->SetBorderStyle(dlg.GetStyle());
		m_pGraph->SetBorderWidth(dlg.GetWidth());
		m_pctrlBorderStyle->SetStyle(dlg.GetStyle());
		m_pctrlBorderStyle->SetWidth(dlg.GetWidth());
		m_pctrlBorderStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnGraphBorder(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetBorder(bShow);
	SetApplied(false);
}


void GraphDlg::OnGraphBackColor()
{
	QColor BkColor = m_pGraph->GetBackColor();
	BkColor = QColorDialog::getColor(BkColor);
	if(BkColor.isValid()) m_pGraph->SetBkColor(BkColor);

	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());
	SetButtonColors();
	SetApplied(false);	
}


void GraphDlg::OnLabelColor()
{
	QColor color = m_pGraph->GetLabelColor();
	m_pGraph->SetLabelColor(QColorDialog::getRgba(color.rgba()));

	QPalette palette = m_pctrlLabelClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());
		palette.setColor(QPalette::ButtonText, m_pGraph->GetLabelColor());
		m_pctrlLabelClr->setPalette(palette);
//		m_pctrlLabelClr->setAutoFillBackground(true);
	}

	SetApplied(false);
}



void GraphDlg::OnLabelFont()
{
	bool ok;
	QFont LabelFont("Courier");
	m_pGraph->GetLabelLogFont(&LabelFont);
	QFont font = QFontDialog::getFont(&ok, LabelFont, this);

	if (ok)
	{
	   m_pctrlLabelButton->setFont(font);
	   m_pctrlLabelButton->setText(font.family());
	   m_pGraph->SetLabelLogFont(&font);
		SetApplied(false);
	}
}




void GraphDlg::OnOK()
{
	m_pGraph->SetAutoX(m_pctrlXAuto->isChecked());
	m_pGraph->SetXMin(m_pctrlXMin->getValue());
	m_pGraph->SetXMax(m_pctrlXMax->getValue());
	m_pGraph->SetX0(m_pctrlXOrigin->getValue());
	m_pGraph->SetXUnit(m_pctrlXUnit->getValue());

	m_pGraph->SetAutoY(m_pctrlYAuto->isChecked());
	m_pGraph->SetYMin(m_pctrlYMin->getValue());
	m_pGraph->SetYMax(m_pctrlYMax->getValue());
	m_pGraph->SetY0(m_pctrlYOrigin->getValue());
	m_pGraph->SetYUnit(m_pctrlYUnit->getValue());

	double MinUnit;
	if(!m_pctrlAutoXMinUnit->isChecked())
	{
		MinUnit = m_pctrlXMinorUnit->getValue();
		m_pGraph->SetXMinorUnit(MinUnit);
		m_pGraph->SetAutoXMinUnit(false);
	}
	else
		m_pGraph->SetAutoXMinUnit(true);

	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->getValue();
		m_pGraph->SetYMinorUnit(MinUnit);
		m_pGraph->SetAutoYMinUnit(false);
	}
	else
		m_pGraph->SetAutoYMinUnit(true);

	for(int i=0; i<m_NGraph; i++)
	{
		if (m_GraphArray[i]) {
			m_GraphArray[i]->CopySettings(m_pGraph);
		}
	}

	m_XSel = m_pctrlXSel->currentRow();
	m_YSel = m_pctrlYSel->currentRow();
	m_pGraph->SetVariables(m_pctrlXSel->currentRow(), m_pctrlYSel->currentRow());

	accept();
}

void GraphDlg::OnRestoreParams()
{
	m_pGraph->CopySettings(m_pMemGraph);

	for(int i=0; i<m_NGraph; i++)
	{
		if (m_GraphArray[i]) {
			m_GraphArray[i]->CopySettings(m_pMemGraph);
		}
	}

	SetParams();
	SetApplied(true);

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->UpdateView();
}


void GraphDlg::OnTitleColor()
{
	QColor color = m_pGraph->GetTitleColor();
	m_pGraph->SetTitleColor(QColorDialog::getRgba(color.rgba()));

	QPalette palette = m_pctrlTitleClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());
		palette.setColor(QPalette::ButtonText, m_pGraph->GetTitleColor());
		m_pctrlTitleClr->setPalette(palette);
//		m_pctrlTitleClr->setAutoFillBackground(true);
	}

	SetApplied(false);
}


void GraphDlg::OnTitleFont()
{
	bool ok;
    QFont TitleFont(g_mainFrame->m_TextFont.family());
	m_pGraph->GetTitleLogFont(&TitleFont);

	QFont font = QFontDialog::getFont(&ok, TitleFont, this);

	if (ok)
	{
	   m_pctrlTitleButton->setFont(font);
	   m_pctrlTitleButton->setText(font.family());
	   m_pGraph->SetTitleLogFont(&font);
		SetApplied(false);
	}
}


void GraphDlg::OnVariableChanged()
{
	m_bVariableChanged = true;
}


void GraphDlg::OnXMajGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow;
	m_pGraph->GetXMajGrid(bShow,color,s,w);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetXMajGrid(bShow, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth());
		m_pctrlXMajGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMajGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMajGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnXMinGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow, bAuto;
	double unit;
	m_pGraph->GetXMinGrid(bShow, bAuto,color,s,w,unit);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetXMinGrid(bShow, bAuto, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth(),unit);
		m_pctrlXMinGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMinGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMinGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}

}


void GraphDlg::OnXMajGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetXMajGrid(bShow);
	m_pctrlXMajGridStyle->setEnabled(bShow);
	SetApplied(false);
}


void GraphDlg::OnXMinGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetXMinGrid(bShow);
	m_pctrlXMinGridStyle->setEnabled(bShow);
	m_pctrlAutoXMinUnit->setEnabled(bShow);
	m_pctrlXMinorUnit->setEnabled(bShow && !m_pGraph->GetAutoXMin());

	SetApplied(false);
}


void GraphDlg::OnYInverted()
{
	m_pGraph->SetInverted(m_pctrlYInverted->checkState() == Qt::Checked);
	SetApplied(false);
}


void GraphDlg::OnYMajGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetYMajGrid(bShow);
	m_pctrlYMajGridStyle->setEnabled(bShow);
	SetApplied(false);
}

void GraphDlg::OnYMajGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow;
	m_pGraph->GetYMajGrid(bShow,color,s,w);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetYMajGrid(bShow, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth());
		m_pctrlYMajGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMajGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMajGridStyle->SetColor(dlg.GetColor());
	}
}


void GraphDlg::OnYMinGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetYMinGrid(bShow);
	m_pctrlYMinGridStyle->setEnabled(bShow);
	m_pctrlAutoYMinUnit->setEnabled(bShow);
	m_pctrlYMinorUnit->setEnabled(bShow && !m_pGraph->GetAutoYMin());

	SetApplied(false);
}


void GraphDlg::OnYMinGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow, bAuto;
	double unit;
	m_pGraph->GetYMinGrid(bShow, bAuto,color,s,w,unit);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetYMinGrid(bShow, bAuto, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth(),unit);
		m_pctrlYMinGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMinGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMinGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}


void GraphDlg::SetApplied(bool bApplied)
{
	m_bApplied = bApplied;
//	ApplyButton->setEnabled(!bApplied);
}


void GraphDlg::SetButtonColors()
{
	QPalette palette = m_pctrlTitleClr->palette();
	QColor listColor = palette.color(QPalette::Button);

	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());

		palette.setColor(QPalette::ButtonText, m_pGraph->GetTitleColor());
		m_pctrlTitleClr->setPalette(palette);

		palette.setColor(QPalette::ButtonText, m_pGraph->GetLabelColor());
		m_pctrlLabelClr->setPalette(palette);
	}
}


void GraphDlg::SetParams()
{
	m_pctrlXAuto->setChecked(m_pGraph->GetAutoX());
	m_pctrlYAuto->setChecked(m_pGraph->GetAutoY());

	m_pctrlXMin->setValue(m_pGraph->GetXMin());
	m_pctrlXMax->setValue(m_pGraph->GetXMax());
	m_pctrlXOrigin->setValue(m_pGraph->GetX0());
	m_pctrlXUnit->setValue(m_pGraph->GetXUnit());
	m_pctrlYMin->setValue(m_pGraph->GetYMin());
	m_pctrlYMax->setValue(m_pGraph->GetYMax());
	m_pctrlYOrigin->setValue(m_pGraph->GetY0());
	m_pctrlYUnit->setValue(m_pGraph->GetYUnit());

	OnAutoX();
	OnAutoY();

	SetButtonColors();

	QFont font;
	m_pGraph->GetLabelLogFont(&font);
	m_pctrlLabelButton->setText(font.family());
	m_pctrlLabelButton->setFont(font);

	m_pGraph->GetTitleLogFont(&font);
	m_pctrlTitleButton->setText(font.family());
	m_pctrlTitleButton->setFont(font);


	bool bState, bAuto;
	QColor color;
	int style, width;
	double unit;

	m_pGraph->GetXMajGrid(bState, color, style, width);
	m_pctrlXMajGridShow->setChecked(bState);
	m_pctrlXMajGridStyle->SetColor(color);
	m_pctrlXMajGridStyle->SetStyle(style);
	m_pctrlXMajGridStyle->SetWidth(width);
	m_pctrlXMajGridStyle->setEnabled(bState);

	m_pGraph->GetXMinGrid(bState, bAuto,color, style, width, unit);
	m_pctrlXMinGridShow->setChecked(bState);
	m_pctrlXMinGridStyle->SetColor(color);
	m_pctrlXMinGridStyle->SetStyle(style);
	m_pctrlXMinGridStyle->SetWidth(width);
	m_pctrlXMinGridStyle->setEnabled(bState);
	m_pctrlXMinorUnit->setValue(unit);
	m_pctrlAutoXMinUnit->setChecked(bAuto);
	m_pctrlAutoXMinUnit->setEnabled(bState);
	m_pctrlXMinorUnit->setEnabled(!bAuto && bState);
	
	m_pGraph->GetYMajGrid(bState, color, style, width);
	m_pctrlYMajGridShow->setChecked(bState);
	m_pctrlYMajGridStyle->SetColor(color);
	m_pctrlYMajGridStyle->SetStyle(style);
	m_pctrlYMajGridStyle->SetWidth(width);
	m_pctrlYMajGridStyle->setEnabled(bState);

	m_pGraph->GetYMinGrid(bState, bAuto,color, style, width, unit);
	m_pctrlYMinGridShow->setChecked(bState);
	m_pctrlYMinGridStyle->SetColor(color);
	m_pctrlYMinGridStyle->SetStyle(style);
	m_pctrlYMinGridStyle->SetWidth(width);
	m_pctrlYMinGridStyle->setEnabled(bState);
	m_pctrlYMinorUnit->setValue(unit);
	m_pctrlAutoYMinUnit->setChecked(bAuto);
	m_pctrlAutoYMinUnit->setEnabled(bState);
	m_pctrlYMinorUnit->setEnabled(!bAuto && bState);

	m_pctrlAxisStyle->SetColor(m_pGraph->GetAxisColor());
	m_pctrlAxisStyle->SetStyle(m_pGraph->GetAxisStyle());
	m_pctrlAxisStyle->SetWidth(m_pGraph->GetAxisWidth());

	m_pctrlGraphBorder->setChecked(m_pGraph->GetBorder());
	m_pctrlBorderStyle->SetColor(m_pGraph->GetBorderColor());
	m_pctrlBorderStyle->SetStyle(m_pGraph->GetBorderStyle());
	m_pctrlBorderStyle->SetWidth(m_pGraph->GetBorderWidth());

	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());

	m_pctrlYInverted->setChecked(m_pGraph->GetInverted());

	FillVariableList();

	m_pctrlXSel->setCurrentRow(m_pGraph->GetXVariable());
	m_pctrlYSel->setCurrentRow(m_pGraph->GetYVariable());
	m_bVariableChanged = false;

	SetApplied(true);

	ApplyButton->setEnabled(m_pTabWidget->currentIndex()!=0);
}



void GraphDlg::SetupLayout()
{
//	QDesktopWidget desktop;
//	QRect r = desktop.geometry();

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	RestoreButton = new QPushButton(tr("Restore"));
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(true);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	ApplyButton = new QPushButton(tr("Apply"));
	ApplyButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(RestoreButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pTabWidget = new QTabWidget(this);


	ScalePage    = new QWidget(this);
	GridPage     = new QWidget(this);
	FontPage     = new QWidget(this);
	VariablePage = new QWidget(this);

	//________Variable Page______________________

	QHBoxLayout *AxisNames = new QHBoxLayout;
	QLabel *YAxis = new QLabel(tr("YAxis"));
	QLabel *vs = new QLabel(tr("vs."));
	QLabel *XAxis = new QLabel(tr("XAxis"));
	AxisNames->addStretch(1);
	AxisNames->addWidget(YAxis);
	AxisNames->addStretch(1);
	AxisNames->addWidget(vs);
	AxisNames->addStretch(1);
	AxisNames->addWidget(XAxis);
	AxisNames->addStretch(1);

	QHBoxLayout *VariableBox = new QHBoxLayout;
	m_pctrlXSel = new QListWidget;
	m_pctrlYSel = new QListWidget;
/*	m_pctrlXSel->setMinimumWidth(30);
	m_pctrlYSel->setMinimumWidth(30);
	m_pctrlXSel->setMinimumHeight(200);
	m_pctrlYSel->setMinimumHeight(200);*/
//	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlYSel);
//	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlXSel);
//	VariableBox->addStretch(1);

	QVBoxLayout *VariableLayout = new QVBoxLayout(this);
	VariableLayout->addLayout(AxisNames);
	VariableLayout->addLayout(VariableBox);
//	VariableLayout->addStretch(1);

	VariablePage->setLayout(VariableLayout);

	//________Font Page______________________
	QGridLayout *FontButtons = new QGridLayout;

	QLabel *lab1  = new QLabel(tr("Title"));
	QLabel *lab2  = new QLabel(tr("Label"));
	QLabel *lab402  = new QLabel(tr("Font"));
	QLabel *lab403  = new QLabel(tr("Color"));
	lab1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	lab2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	lab402->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	lab403->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	FontButtons->addWidget(lab402,1,2);
	FontButtons->addWidget(lab403,1,3);
	FontButtons->addWidget(lab1,2,1);
	FontButtons->addWidget(lab2,3,1);

	m_pctrlTitleButton  = new QPushButton(tr("Set Title Font"));
	m_pctrlLabelButton  = new QPushButton(tr("Set Label Font"));
	FontButtons->addWidget(m_pctrlTitleButton,2,2);
	FontButtons->addWidget(m_pctrlLabelButton,3,2);

	m_pctrlTitleClr  = new QPushButton(tr("Title Color"));
	m_pctrlLabelClr  = new QPushButton(tr("Label Color"));
//	m_pctrlTitleClr->setAutoFillBackground(true);
//	m_pctrlLabelClr->setAutoFillBackground(true);

	FontButtons->addWidget(m_pctrlTitleClr,2,3);
	FontButtons->addWidget(m_pctrlLabelClr,3,3);

	QGroupBox *FontBox = new QGroupBox(tr("Fonts"));
	FontBox->setLayout(FontButtons);


	QGridLayout *BackData = new QGridLayout;
	QLabel *GraphBackLabel = new QLabel(tr("Graph Background"));
	GraphBackLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlGraphBorder = new QCheckBox(tr("Graph Border"));

	m_pctrlGraphBack = new ColorButton;
	m_pctrlGraphBack->setMinimumWidth(100);
	m_pctrlBorderStyle = new LineButton;
	m_pctrlBorderStyle->setMinimumWidth(100);

	BackData->addWidget(GraphBackLabel,1,1);
	BackData->addWidget(m_pctrlGraphBorder,2,1,1,1,Qt::AlignRight | Qt::AlignVCenter);

	BackData->addWidget(m_pctrlGraphBack,1,2);
	BackData->addWidget(m_pctrlBorderStyle,2,2);

	QGroupBox *BackBox = new QGroupBox(tr("BackGround"));
	BackBox->setLayout(BackData);


	QVBoxLayout *FontLayout = new QVBoxLayout;
	FontLayout->addWidget(FontBox);
	FontLayout->addStretch(1);
	FontLayout->addWidget(BackBox);
	FontLayout->addStretch(1);
	FontPage->setLayout(FontLayout);
	//________End Font Page______________________

	//________Scale Page______________________

	QGridLayout *ScaleData = new QGridLayout;

	QLabel *XAxis2 = new QLabel(tr("X Axis"));
	QLabel *YAxis2 = new QLabel(tr("Y Axis"));
	XAxis2->setAlignment(Qt::AlignCenter);
	YAxis2->setAlignment(Qt::AlignCenter);

	QLabel *MinLabel = new QLabel(tr("Min"));
	QLabel *MaxLabel = new QLabel(tr("Max"));
	QLabel *OriginLabel = new QLabel(tr("Origin"));
	QLabel *UnitLabel = new QLabel(tr("Unit"));
	MinLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	MaxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	OriginLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	UnitLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//	ScaleData->addStretch(1);
	ScaleData->addWidget(MinLabel,4,1);
	ScaleData->addWidget(MaxLabel,5,1);
	ScaleData->addWidget(OriginLabel,6,1);
	ScaleData->addWidget(UnitLabel,7,1);

	m_pctrlXAuto    = new QCheckBox(tr("Auto Scale"));
	m_pctrlXMin     = new NumberEdit;
	m_pctrlXMax     = new NumberEdit;
	m_pctrlXOrigin  = new NumberEdit;
	m_pctrlXUnit    = new NumberEdit;

	ScaleData->addWidget(XAxis2,1,2);
	ScaleData->addWidget(m_pctrlXAuto,3,2);
	ScaleData->addWidget(m_pctrlXMin,4,2);
	ScaleData->addWidget(m_pctrlXMax,5,2);
	ScaleData->addWidget(m_pctrlXOrigin,6,2);
	ScaleData->addWidget(m_pctrlXUnit,7,2);

	m_pctrlYInverted = new QCheckBox(tr("Inverted Axis"));
	m_pctrlYAuto     = new QCheckBox(tr("Auto Scale"));
	m_pctrlYMin      = new NumberEdit;
	m_pctrlYMax      = new NumberEdit;
	m_pctrlYOrigin   = new NumberEdit;
	m_pctrlYUnit     = new NumberEdit;

	ScaleData->addWidget(YAxis2,1,3);
	ScaleData->addWidget(m_pctrlYInverted,2,3);
	ScaleData->addWidget(m_pctrlYAuto,3,3);
	ScaleData->addWidget(m_pctrlYMin,4,3);
	ScaleData->addWidget(m_pctrlYMax,5,3);
	ScaleData->addWidget(m_pctrlYOrigin,6,3);
	ScaleData->addWidget(m_pctrlYUnit,7,3);

	ScalePage->setLayout(ScaleData);
	//________End Scale Page______________________
	//________Axis Page______________________
	QGridLayout *AxisData = new QGridLayout;
	QLabel *AxisStyleLabel = new QLabel(tr("Axis Style"));
	AxisStyleLabel->setMinimumWidth(100);
	m_pctrlXMajGridShow = new QCheckBox(tr("X Major Grid"));
	m_pctrlYMajGridShow = new QCheckBox(tr("Y Major Grid"));
	m_pctrlXMinGridShow = new QCheckBox(tr("X Minor Grid"));
	m_pctrlYMinGridShow = new QCheckBox(tr("Y Minor Grid"));
	m_pctrlAutoXMinUnit = new QCheckBox(tr("Auto Unit"));
	m_pctrlAutoYMinUnit = new QCheckBox(tr("Auto Unit"));

	m_pctrlAxisStyle = new LineButton;
	m_pctrlAxisStyle->setMinimumWidth(100);
	m_pctrlXMajGridStyle = new LineButton;
	m_pctrlYMajGridStyle = new LineButton;
	m_pctrlXMinGridStyle = new LineButton;
	m_pctrlYMinGridStyle = new LineButton;

	m_pctrlXMinorUnit = new NumberEdit;
	m_pctrlYMinorUnit = new NumberEdit;

	AxisData->addWidget(AxisStyleLabel,1,1);
	AxisData->addWidget(m_pctrlXMajGridShow,2,1);
	AxisData->addWidget(m_pctrlYMajGridShow,3,1);
	AxisData->addWidget(m_pctrlXMinGridShow,4,1);
	AxisData->addWidget(m_pctrlYMinGridShow,5,1);

	AxisData->addWidget(m_pctrlAxisStyle,1,2);
	AxisData->addWidget(m_pctrlXMajGridStyle,2,2);
	AxisData->addWidget(m_pctrlYMajGridStyle,3,2);
	AxisData->addWidget(m_pctrlXMinGridStyle,4,2);
	AxisData->addWidget(m_pctrlYMinGridStyle,5,2);

	AxisData->addWidget(m_pctrlAutoXMinUnit,4,3);
	AxisData->addWidget(m_pctrlAutoYMinUnit,5,3);
	AxisData->addWidget(m_pctrlXMinorUnit,4,4);
	AxisData->addWidget(m_pctrlYMinorUnit,5,4);

	GridPage->setLayout(AxisData);
	//________End Axis Page______________________


	m_pTabWidget->addTab(VariablePage, tr("Variables"));
	m_pTabWidget->addTab(ScalePage, tr("Scales"));
	m_pTabWidget->addTab(GridPage, tr("Axis and Grids"));
	m_pTabWidget->addTab(FontPage, tr("Fonts and BackGround"));

	m_pTabWidget->setCurrentIndex(s_ActivePage);
	connect(m_pTabWidget, SIGNAL(currentChanged (int)), this, SLOT(OnActivePage(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pTabWidget);
	mainLayout->addLayout(CommandButtons);
	setLayout(mainLayout);
}




