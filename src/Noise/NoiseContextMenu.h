#ifndef NOISECONTEXTMENU_H
#define NOISECONTEXTMENU_H

#include "../TwoDContextMenu.h"
class NoiseModule;


class NoiseContextMenu : public TwoDContextMenu
{
	Q_OBJECT
	
public:
	NoiseContextMenu(QWidget *parent, NoiseModule *module);
	
private:
	NoiseModule *m_module;
};

#endif // NOISECONTEXTMENU_H
