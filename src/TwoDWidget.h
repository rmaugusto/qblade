/****************************************************************************

	TwoDWidget Class
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

#ifndef TwoDWidget_H
#define TwoDWidget_H

#include <QWidget>


class TwoDWidget : public QWidget
{
public:
	TwoDWidget(QWidget *parent = 0);
	
protected:
	void keyPressEvent (QKeyEvent *event);
	void keyReleaseEvent (QKeyEvent *event);
	void mousePressEvent (QMouseEvent *event);
	void mouseMoveEvent (QMouseEvent *event);
	void mouseReleaseEvent (QMouseEvent *event);
	void wheelEvent (QWheelEvent *event);
	void resizeEvent (QResizeEvent *event);
	void paintEvent (QPaintEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void contextMenuEvent (QContextMenuEvent *event);
	void enterEvent (QEvent *event);
	void leaveEvent (QEvent *event);
	
public:
	void *m_pXDirect, *m_pAFoil, *m_pXInverse;
	void *m_pBEM;
	void *m_pDMS;
};

#endif
