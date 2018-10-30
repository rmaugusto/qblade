#include "NoiseContextMenu.h"

#include "NoiseModule.h"


NoiseContextMenu::NoiseContextMenu(QWidget *parent, NoiseModule *module)
	: TwoDContextMenu (parent, module),
	  m_module(module)
{
	
}
