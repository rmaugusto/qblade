#ifndef WINDFIELDMENU_H
#define WINDFIELDMENU_H

#include <QMenu>
class QMainWindow;

class WindFieldModule;


class WindFieldMenu : public QMenu
{
	Q_OBJECT
	
public:
	WindFieldMenu (QMainWindow *parent, WindFieldModule *module);
	
private:
	WindFieldModule *m_module;
    QAction *m_writeWindfieldBinaryFileAction, *m_writeWindfieldTxtFileAction, *m_importWindfieldBinaryFileAction;
	
private slots:
	void onAboutToShow ();
	void onWriteWindfieldBinaryFile ();
	void onWriteWindfieldTxtFile ();
    void onImportWindfieldBinaryFile();

};

#endif // WINDFIELDMENU_H
