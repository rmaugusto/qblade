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

#include "GLWidget.h"

#include <QtOpenGL>
#include "MainFrame.h"
#include <math.h>
#include "Graph/Curve.h"
#include "XUnsteadyBEM/WindFieldModule.h"

///////new code DM/////////
#include "XBEM/BEM.h"
/////end new code DM///////////

///////new code JW/////////
#include "XDMS/DMS.h"
#include "QFem/QFEMModule.h"
#include "XLLT/QLLTModule.h"
/////end new code JW///////////


GLWidget::GLWidget(QWidget *parent)
    : QGLViewer(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
	setAutoFillBackground(false);  // prevent QPainter from filling the background
	setCursor(Qt::CrossCursor);
}

void GLWidget::draw() {
	// Save current OpenGL state
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
    if (g_mainFrame->m_iApp==BEM) {
		g_qbem->drawGL();
    } else if (g_mainFrame->m_iApp==DMS) {
		g_qdms->drawGL();
	} else if (g_mainFrame->getGlModule()) {
		g_mainFrame->getGlModule()->drawGL();
	}
	
	// Restore OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

void GLWidget::paintEvent(QPaintEvent */*event*/) {
//    qDebug() << "paintEvent";
    QPainter painter (this);
    QStringList missingObjectMessages = g_mainFrame->prepareMissingObjectMessage();
    if (missingObjectMessages.isEmpty()) {
        preDraw();
        draw ();
        postDraw();
        overpaint(painter);
    } else {
        preDraw();  // clears the background and stuff
        postDraw();

        /* realize the message as overpaint */
        const QFont missingObjectFont (g_mainFrame->m_TextFont.family(), 0.02*width());
        const QFont hintFont (g_mainFrame->m_TextFont.family(), 0.015*width());

        painter.setFont(missingObjectFont);
        const int betweenTextGap = 0.08*height();
        QPoint textPosition = QPoint (0.15*width(), 0.15*height());
        for (int i = 0; i < missingObjectMessages.size() - 1; ++i) {
            painter.drawText(textPosition, missingObjectMessages[i]);
            textPosition.ry() += betweenTextGap;
        }
        painter.setFont(hintFont);
        painter.drawText(textPosition, missingObjectMessages.last());
    }
}

void GLWidget::overpaint(QPainter &painter) {
	/* draw the text collected with overpaintText */
	painter.setPen(g_mainFrame->m_TextColor);
    for (int i = 0; i < m_overpaintText.size(); ++i) {
        if (m_overpaintFont.contains(i)) {
            painter.setFont(m_overpaintFont[i]);
        }
        painter.drawText(m_overpaintText[i].first, m_overpaintText[i].second);
    }
	m_overpaintFont.clear();
	m_overpaintText.clear();
	
	if (g_mainFrame->getGlModule()) {
		g_mainFrame->getGlModule()->overpaint(painter);
	} else if (g_mainFrame->m_iApp==BEM) {
		g_qbem->overpaint(painter);
	} else if (g_mainFrame->m_iApp==DMS) {
		g_qdms->overpaint(painter);
	}
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

void GLWidget::overpaintText(double x, double y, double z, QString text) {
	qglviewer::Vec position = camera()->projectedCoordinatesOf(qglviewer::Vec(x, y, z));
	m_overpaintText.append(QPair<QPoint, QString> (QPoint(position.x, position.y), text));
}

void GLWidget::setOverpaintFont(QFont font) {
	m_overpaintFont[m_overpaintText.size()] = font;
}
