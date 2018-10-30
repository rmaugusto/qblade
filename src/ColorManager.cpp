#include "ColorManager.h"

#include <QPen>


ColorManager::ColorManager() {
	m_colors.resize(24);
	
	m_colors[0]  = QColor(255,   0,   0);
	m_colors[1]  = QColor(  0, 255,   0);
	m_colors[2]  = QColor(  0,   0, 255);
	m_colors[3]  = QColor(  0,   0,   0);
	m_colors[4]  = QColor(255,   0, 255);
	m_colors[5]  = QColor(  0, 255, 255);
	
	m_colors[6]  = QColor(200,  60,  60);
	m_colors[7]  = QColor(  0, 160,   0);
	m_colors[8]  = QColor(100, 100, 240);
	m_colors[9]  = QColor(170, 170,   0);
	m_colors[10] = QColor(130,   0, 130);
	m_colors[11] = QColor(  0, 130, 130);
	
	m_colors[12] = QColor(255, 128, 128);
	m_colors[13] = QColor(  0, 255, 128);
	m_colors[14] = QColor( 64, 200, 255);
	m_colors[15] = QColor(170, 170, 100);
	m_colors[16] = QColor(190, 100, 190);
	m_colors[17] = QColor(100, 170, 170);
	
	m_colors[18] = QColor(228, 150,  70);
	m_colors[19] = QColor(170, 255, 170);
	m_colors[20] = QColor(120, 120, 255);
	m_colors[21] = QColor(228, 228, 128);
	m_colors[22] = QColor(255, 170, 255);
	m_colors[23] = QColor(170, 255, 255);
}

QColor ColorManager::getColor(int index) {
	return m_colors[index%24];
}

ColorManager g_colorManager;
