/****************************************************************************

	GLWidget Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

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

#define GLMIAREXVIEW  3
#define GLBODYVIEW  5
#define GLWINGVIEW  7


#include <QtOpenGL>
#include "MainFrame.h"
#include "Miarex/Miarex.h" 
#include <math.h>
#include "Graph/Curve.h"
#include "Miarex/GL3dBodyDlg.h"
#include "Miarex/GL3dWingDlg.h"
#include "GLWidget.h"
#include "XGlobals.h"
#include "XUnsteadyBEM/WindFieldModule.h"

///////new code DM/////////
#include "XBEM/BEM.h"
void *GLWidget::s_pBEM;
/////end new code DM///////////

///////new code JW/////////
#include "XDMS/DMS.h"
void *GLWidget::s_pDMS;
#include "QFem/QFEMModule.h"
#include "XLLT/QLLTModule.h"
void *GLWidget::s_pQFEM;
/////end new code JW///////////

void *GLWidget::s_pMiarex;
void *GLWidget::s_pMainFrame;


GLWidget::GLWidget(QWidget *parent)
    : QGLViewer(parent)
{
	m_pParent = parent;
	m_iView = GLMIAREXVIEW;

	setAutoFillBackground(false);

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}

void GLWidget::initializeGL()
{
    glClearColor(g_mainFrame->m_BackgroundColor.redF(),
                 g_mainFrame->m_BackgroundColor.greenF(),
                 g_mainFrame->m_BackgroundColor.blueF(),
                 0.0);
}

void GLWidget::draw()
{


    QFont font("Arial",10);

    ///// new code JW /////
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame; // JW modification
    if (pMainFrame->m_iApp==BEM)
    {
    ///// end new code JW /////
        QBEM *pBEM = (QBEM *) s_pBEM;

        pBEM->GLDraw3D();
        pBEM->GLRenderView();
        if (pBEM->m_pctrlAxes->isChecked() && pBEM->m_pBlade) GLDrawAxes(pBEM->m_pBlade->m_PlanformSpan,pMainFrame->m_TextColor,1,2);

        if ((pBEM->m_pctrlPerspective->isChecked())) camera()->setType(qglviewer::Camera::PERSPECTIVE);
        else    camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);

        if (pBEM->m_pctrlFoilNames->isChecked() && pBEM->m_pBlade)
        {
            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

            glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            for (int i=0;i<=pBEM->m_pBlade->m_NPanel;i++)
                {
                    renderText(pBEM->m_pBlade->m_PlanformSpan/10, pBEM->m_pBlade->m_TPos[i], 0.0, pBEM->m_pBlade->m_Airfoils[i], font);
                }
            glPopAttrib ();

        }

        if (pBEM->m_pctrlPositions->isChecked() && pBEM->m_pBlade)
        {
            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

            glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


            for (int i=0;i<=pBEM->m_pBlade->m_NPanel;i++)
            {
                QString strong;
                renderText(-pBEM->m_pBlade->m_PlanformSpan/10, pBEM->m_pBlade->m_TPos[i], 0.0, QString(strong.number(pBEM->m_pBlade->m_TPos[i],'f',3)+" m"), font);
            }
            glPopAttrib ();
        }


    ///// new code JW /////
    }
    else if (pMainFrame->m_iApp==DMS)
    {
        QDMS *pDMS = (QDMS *) s_pDMS;

        pDMS->GLDraw3D();
        pDMS->GLRenderView();
        if (pDMS->m_pctrlAxes->isChecked() && pDMS->m_pBlade) GLDrawAxes(pDMS->m_pBlade->m_PlanformSpan,pMainFrame->m_TextColor,1,2);

        if ((pDMS->m_pctrlPerspective->isChecked())) camera()->setType(qglviewer::Camera::PERSPECTIVE);
        else    camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);

        if (pDMS->m_pctrlFoilNames->isChecked() && pDMS->m_pBlade)
        {
            glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
            for (int i=0;i<=pDMS->m_pBlade->m_NPanel;i++)
                {
                    renderText(pDMS->m_pBlade->m_PlanformSpan/10+pDMS->m_pBlade->m_TOffset[i], pDMS->m_pBlade->m_TPos[i], 0.0, pDMS->m_pBlade->m_Airfoils[i], pMainFrame->m_TextFont);
                }
        }

        if (pDMS->m_pctrlPositions->isChecked() && pDMS->m_pBlade)
        {
            glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

            for (int i=0;i<=pDMS->m_pBlade->m_NPanel;i++)
            {
                QString strong;
                renderText(-pDMS->m_pBlade->m_PlanformSpan/10+pDMS->m_pBlade->m_TOffset[i], pDMS->m_pBlade->m_TPos[i], 0.0, QString(strong.number(pDMS->m_pBlade->m_TPos[i],'f',3)+" m"), pMainFrame->m_TextFont);
            }
        }

    }
    ///// end new code JW /////
	
	/////////////// new Code NM ///////////////
	else if (g_mainFrame->m_iApp == WINDFIELDMODULE) {
		g_windFieldModule->onRedraw();
	}
    else if (g_mainFrame->m_iApp == QLLTMODULE) {
        g_QLLTModule->onRedraw();
//        GLDrawAxes(g_QLLTModule->GetCurrentBlade()->m_PlanformSpan,pMainFrame->m_TextColor,1,2);
    }
	/////////// end new Code NM ///////////////
	else if (g_mainFrame->m_iApp == QFEMMODULE) {

        g_QFEMModule->onRedraw();

        if (g_QFEMModule->m_perspective) camera()->setType(qglviewer::Camera::PERSPECTIVE);
        else    camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);

        if (g_QFEMModule->m_airfoils && g_QFEMModule->GetBlade())
        {
            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
            for (int i=0;i<=g_QFEMModule->GetBlade()->m_NPanel;i++)
                {
                    renderText(g_QFEMModule->GetBlade()->m_PlanformSpan/10, g_QFEMModule->GetBlade()->m_TPos[i], 0.0, g_QFEMModule->GetBlade()->m_Airfoils[i], pMainFrame->m_TextFont);
                }
        }

        if (g_QFEMModule->m_positions && g_QFEMModule->GetBlade())
        {
            glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

            for (int i=0;i<=g_QFEMModule->GetBlade()->m_NPanel;i++)
            {
                QString strong;
                renderText(-g_QFEMModule->GetBlade()->m_PlanformSpan/10, g_QFEMModule->GetBlade()->m_TPos[i], 0.0, QString(strong.number(g_QFEMModule->GetBlade()->m_TPos[i],'f',3)+" m"), pMainFrame->m_TextFont);
            }
        }

        if (g_QFEMModule->m_axes && g_QFEMModule->GetBlade()) GLDrawAxes(g_QFEMModule->GetBlade()->m_PlanformSpan,pMainFrame->m_TextColor,1,2);

	}
}


void GLWidget::CreateArcballList(ArcBall &ArcBall, double GLScale)
{
	int row, col, NumAngles, NumCircles;
	double R, lat_incr, lon_incr, phi, theta;
	ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	ArcBall.SetZoom(0.45,eye,up);

	glNewList(ARCBALL,GL_COMPILE);
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glColor3d(0.3,0.3,.5);
		glLineWidth(1.0);

		R = ArcBall.ab_sphere;

		NumAngles  = 50;
		NumCircles =  6;
		lat_incr =  90.0 / NumAngles;
		lon_incr = 360.0 / NumCircles;

		for (col = 0; col < NumCircles; col++)
		{
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = (row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr ) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = -(row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
		}


		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(-phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(-phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();

	glNewList(ARCPOINT,GL_COMPILE);
	{
		glPolygonMode(GL_FRONT,GL_LINE);

		glColor3d(0.3,0.1,.2);
		glLineWidth(2.0);

		NumAngles  = 10;

		lat_incr = 30.0 / NumAngles;
		lon_incr = 30.0 / NumAngles;

		glBegin(GL_LINE_STRIP);
		{
			phi = 0.0;//longitude

			for (row = -NumAngles; row < NumAngles; row++)
			{
				theta = (row * lat_incr) * PI/180.0;
				glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=-NumAngles; col<NumAngles; col++)
			{
				phi = (0.0 + (double)col*30.0/NumAngles) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();
}




void GLWidget::GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth)
{
	//
	// Draws the axis in the OpenGL view
	//



    QFont font("Arial",15);

	double l = .6*length;

	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth((GLfloat)(AxisWidth));

	glColor3d(AxisColor.redF(),AxisColor.greenF(),AxisColor.blueF());

	glEnable (GL_LINE_STIPPLE);

/*	if     (AxisStyle == DASHLINE)       glLineStipple(1, 0xCFCF);
	else if(AxisStyle == DOTLINE)        glLineStipple(1, 0x6666);
	else if(AxisStyle == DASHDOTLINE)    glLineStipple(1, 0xFF18);
	else if(AxisStyle == DASHDOTDOTLINE) glLineStipple(1, 0x7E66);
    else*/				                 glLineStipple(1, 0xFFFF);//Solid

	// X axis____________
	glBegin(GL_LINES);
        glVertex3d(-l/30, 0.0, 0.0);
        glVertex3d( l/6, 0.0, 0.0);
	glEnd();
	//Arrow
//	glBegin(GL_LINES);
//        glVertex3d( 1.0*l/6,   0.0,   0.0);
//        glVertex3d(  l/6-0.02*l,  0.015*l, 0.015*l);
//	glEnd();
//	glBegin(GL_LINES);
//        glVertex3d( 1.0*l/6,  0.0,    0.0);
//        glVertex3d(  l/6-0.02*l,-0.015*l,-0.015*l);
//	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//XLabel
    glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state


    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    renderText( l*1.2/6, 0.0, 0.0, "X", font);

    glPopAttrib ();





	// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
        glVertex3d(0.0, -l/30, 0.0);
        glVertex3d(0.0,  1.0*l, 0.0);
	glEnd();

	//Arrow
//	glBegin(GL_LINES);
//		glVertex3d( 0.0,     1.0*l,  0.0);
//        glVertex3d( 0.015*l, 0.98*l, 0.015*l);
//	glEnd();
//	glBegin(GL_LINES);
//		glVertex3d( 0.0,     1.0*l,  0.0);
//        glVertex3d(-0.015*l, 0.98*l,-0.015*l);
//	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//Y Label

    glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    renderText( 0.0, l*1.01, 0.0, "Y", font);

    glPopAttrib ();


	// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, -l/30);
        glVertex3d(0.0, 0.0,  l/6);
	glEnd();

	//Arrow
//	glBegin(GL_LINES);
//        glVertex3d(  0.0,   0.0, 1.0*l/6);
//        glVertex3d( 0.015*l,  0.015*l,  l/6-0.02*l);
//	glEnd();
//	glBegin(GL_LINES);
//        glVertex3d( 0.0,    0.0, 1.0*l/6);
//        glVertex3d(-0.015*l, -0.015*l,  l/6-0.02*l);
//	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//ZLabel
    glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    renderText( 0.0, 0.0, l*1.2/6, "Z", font);

    glPopAttrib ();


}






void GLWidget::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
	//
	// Render the sphere representing the light or point masses
	//
	static double start_lat, start_lon,lat_incr, lon_incr, R;
	static double phi1, phi2, theta1, theta2;
	static GLdouble u[3], v[3], w[3], n[3];
	static int row, col;

	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glBegin(GL_TRIANGLES);
	glColor3d(cr.redF(),cr.greenF(),cr.blueF());

	start_lat = -90;
	start_lon = 0.0;
	R = radius;

	lat_incr = 180.0 / NumLatitudes;
	lon_incr = 360.0 / NumLongitudes;

	for (col = 0; col < NumLongitudes; col++)
	{
		phi1 = (start_lon + col * lon_incr) * PI/180.0;
		phi2 = (start_lon + (col + 1) * lon_incr) * PI/180.0;

		for (row = 0; row < NumLatitudes; row++)
		{
			theta1 = (start_lat + row * lat_incr) * PI/180.0;
			theta2 = (start_lat + (row + 1) * lat_incr) * PI/180.0;

			u[0] = R * cos(phi1) * cos(theta1);//x
			u[1] = R * sin(theta1);//y
			u[2] = R * sin(phi1) * cos(theta1);//z

			v[0] = R * cos(phi1) * cos(theta2);//x
			v[1] = R * sin(theta2);//y
			v[2] = R * sin(phi1) * cos(theta2);//z

			w[0] = R * cos(phi2) * cos(theta2);//x
			w[1] = R * sin(theta2);//y
			w[2] = R * sin(phi2) * cos(theta2);//z

			NormalVector(u,v,w,n);

			glNormal3dv(n);
			glVertex3dv(u);
			glVertex3dv(v);
			glVertex3dv(w);

			v[0] = R * cos(phi2) * cos(theta1);//x
			v[1] = R * sin(theta1);//y
			v[2] = R * sin(phi2) * cos(theta1);//z

			NormalVector(u,w,v,n);
			glNormal3dv(n);
			glVertex3dv(u);
			glVertex3dv(w);
			glVertex3dv(v);
		}
	}
	glEnd();
}



void GLWidget::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{
	//
	// calculate two vectors, using the middle point as the common origin
	//

	GLdouble v1[3], v2[3], d;
	v1[0] = p3[0] - p1[0];
	v1[1] = p3[1] - p1[1];
	v1[2] = p3[2] - p1[2];
	v2[0] = p3[0] - p2[0];
	v2[1] = p3[1] - p2[1];
	v2[2] = p3[2] - p2[2];

	// calculate the cross product of the two vectors
	n[0] = v1[1] * v2[2] - v2[1] * v1[2];
	n[1] = v1[2] * v2[0] - v2[2] * v1[0];
	n[2] = v1[0] * v2[1] - v2[0] * v1[1];

	// normalize the vector
	d = ( n[0] * n[0] + n[1] * n[1] + n[2] * n[2] );
	// try to catch very small vectors
	if (d < (GLdouble)0.00000001)
	{
		d = (GLdouble)100000000.0;
	}
	else
	{
		d = (GLdouble)1.0 / sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
}


void GLWidget::GLSetupLight(GLLightDlg &glLightParams, double Offset_y, double LightFactor)
{
	//
	// Sets the light parameters for the OpenGl display
	//

    if (!glLightParams.s_bLight)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        return;
    }

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // the ambient light
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	float fLightAmbient0[4];
	float fLightDiffuse0[4];
	float fLightSpecular0[4];
	float fLightPosition0[4];

	if(LightFactor>1.0) LightFactor = 1.0f;

	// the ambient light conditions.
	fLightAmbient0[0] = LightFactor*glLightParams.s_Ambient * glLightParams.s_Red; // red component
	fLightAmbient0[1] = LightFactor*glLightParams.s_Ambient * glLightParams.s_Green; // green component
	fLightAmbient0[2] = LightFactor*glLightParams.s_Ambient * glLightParams.s_Blue; // blue component
	fLightAmbient0[3] = 1.0; // alpha

	fLightDiffuse0[0] = LightFactor*glLightParams.s_Diffuse * glLightParams.s_Red; // red component
	fLightDiffuse0[1] = LightFactor*glLightParams.s_Diffuse * glLightParams.s_Green; // green component
	fLightDiffuse0[2] = LightFactor*glLightParams.s_Diffuse * glLightParams.s_Blue; // blue component
	fLightDiffuse0[3] = 1.0; // alpha

	fLightSpecular0[0] = LightFactor*glLightParams.s_Specular * glLightParams.s_Red; // red component
	fLightSpecular0[1] = LightFactor*glLightParams.s_Specular * glLightParams.s_Green; // green component
	fLightSpecular0[2] = LightFactor*glLightParams.s_Specular * glLightParams.s_Blue; // blue component
	fLightSpecular0[3] = 1.0; // alpha

	// And finally, its position

	fLightPosition0[0] = (GLfloat)((glLightParams.s_XLight));
	fLightPosition0[1] = (GLfloat)((glLightParams.s_YLight + Offset_y));
	fLightPosition0[2] = (GLfloat)((glLightParams.s_ZLight));
	fLightPosition0[3] = 1.0; // W (positional light)



	// Enable the basic light
	glLightfv(GL_LIGHT0, GL_AMBIENT,  fLightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  fLightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition0);


	float fMatAmbient[4]   = {glLightParams.s_MatAmbient,  glLightParams.s_MatAmbient,   glLightParams.s_MatAmbient,  1.0f};
	float fMatSpecular[4]  = {glLightParams.s_MatSpecular, glLightParams.s_MatSpecular,  glLightParams.s_MatSpecular, 1.0f};
	float fMatDiffuse[4]   = {glLightParams.s_MatDiffuse,  glLightParams.s_MatDiffuse,   glLightParams.s_MatDiffuse,  1.0f};
	float fMatEmission[4]  = {glLightParams.s_MatEmission, glLightParams.s_MatEmission,  glLightParams.s_MatEmission, 1.0f};

	if(glLightParams.s_bColorMaterial)
	{
		glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

//        glColorMaterial(GL_FRONT, GL_AMBIENT);
//        glColorMaterial(GL_FRONT, GL_DIFFUSE);
//        glColorMaterial(GL_FRONT, GL_SPECULAR);

	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);

	}
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  fMatSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   fMatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   fMatDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  fMatEmission);
    glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, glLightParams.s_iMatShininess);

	if(glLightParams.s_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(glLightParams.s_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
    /////new new code DM

    if(glLightParams.s_bSmooth)
    {
      //glEnable(GL_POLYGON_SMOOTH); sometimes causes problems with newer graphic cards
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);

    }
    else
    {
        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);

    }
	if(glLightParams.s_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(glLightParams.s_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                           glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}


void GLWidget::ClientToGL(QPoint const &point, CVector &real)
{
	//
	// In input, takes the 2D point in screen client area coordinates
	// In output, returns the 2D OpenGL point
	//
	static double h2, w2;
	h2 = (double)geometry().height() /2.0;
	w2 = (double)geometry().width()  /2.0;

	if(w2>h2)
	{
		real.x =  ((double)point.x() - w2) / w2;
		real.y = -((double)point.y() - h2) / w2;
	}
	else
	{
		real.x =  ((double)point.x() - w2) / h2;
		real.y = -((double)point.y() - h2) / h2;
	}
}



void GLWidget::GLToClient(CVector const &real, QPoint &point)
{
	//
	//converts an opengl 2D vector to screen client coordinates
	//
	static double dx, dy, h2, w2;

	h2 = m_GLViewRect.height() /2.0;
	w2 = m_GLViewRect.width()  /2.0;

	dx = ( real.x + w2)/2.0;
	dy = (-real.y + h2)/2.0;

	if(w2>h2)
	{
		point.setX((int)(dx * (double)geometry().width()));
		point.setY((int)(dy * (double)geometry().width()));
	}
	else
	{
		point.setX((int)(dx * (double)geometry().height()));
		point.setY((int)(dy * (double)geometry().height()));
	}
}
