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

#include <qglviewer.h>
class GLLightDlg;


/* The implementation of this class is very similar to the overpaint example of QGLViewer. Since many problems 
 * occured over the time, sticking to this concept seems reliable. Highly important seemed the separation of
 * drawing gl content and overpainting with a QPainter. Especially the QGLWidget::renderText method caused trouble
 * when used within the gl content.
 * */
class GLWidget : public QGLViewer
{
	Q_OBJECT
	
public:
	GLWidget(QWidget *parent = 0);
	
	void GLSetupLight(GLLightDlg &glLightParams, double Offset_y, double LightFactor);  // NM TODO whole Dialog has to be refactored
	
	/**
	 * @brief Draws a string within gl content.
	 *
	 * This function should be used only within gl content. It temporarily stores position and text. The text is drawn
	 * in GLWidget::overpaint with QPainter::drawText. By this approach gl content and overpaint can be separated.
	 * For convenience, it takes the position of the text in gl coordinates.
	 * @param x The x coordinate of where to draw the text.
	 * @param y The y coordinate of where to draw the text.
	 * @param z The z coordinate of where to draw the text.
	 * @param text The text that will be drawn.
	 * @return True if the store is empty.
	 */
	void overpaintText (double x, double y, double z, QString text);
	
	void setOverpaintFont (QFont font);
	
private:
    void paintEvent (QPaintEvent *event);
    void draw();
	void overpaint (QPainter &painter);
	
private:
	QHash<int, QFont> m_overpaintFont;  // temporarily store fonts for a certain text range
	QList<QPair<QPoint, QString> > m_overpaintText;  // temporarily store position and text for overpaint
};

#endif
