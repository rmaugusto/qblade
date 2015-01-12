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


#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "Params.h"
#include "Miarex/ArcBall.h"
#include "Miarex/GLLightDlg.h"
#include <qglviewer.h>

class GLWidget : public QGLViewer
{
	Q_OBJECT
	
	friend class QMiarex;
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;
	friend class MainFrame;
	friend class ArcBall;
	friend class QBEM;
	friend class QDMS;
	
public:
	GLWidget(QWidget *parent = 0);
	void CreateArcballList(ArcBall &ArcBall, double GLScale);
	void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);
	void GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes);
	void GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth);
	void GLSetupLight(GLLightDlg &glLightParams, double Offset_y, double LightFactor);
	void ClientToGL(QPoint const &point, CVector &real);
	void GLToClient(CVector const &real, QPoint &point);
	
	
private:
	void initializeGL();
	void draw();
	
private:
	void *m_pParent;
	static void *s_pMiarex;
	static void *s_pMainFrame;
	static void *s_pBEM;
	static void *s_pDMS;
	static void *s_pQFEM;
	
	QRect m_rCltRect;  // The client window rectangle
	QRect m_GLViewRect;  // The OpenGl Viewport
	
	int m_iView;  // Wing=3 Body=5
};

#endif
