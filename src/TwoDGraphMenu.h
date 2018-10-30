#ifndef TWODGRAPHMENU_H
#define TWODGRAPHMENU_H

#include <QMenu>

class TwoDModule;
class QMainWindow;


class TwoDGraphMenu : public QMenu
{
	Q_OBJECT
	
public:
	TwoDGraphMenu(QMainWindow *parent, TwoDModule *module);
	
private:
	TwoDModule *m_module;
	QAction *m_singleGraphAction, *m_twoHorizontalGraphsAction, *m_twoVerticalGraphsAction,
            *m_threeGraphsAction, *m_fourGraphsAction, *m_fourGraphsVerticalAction;
	
private slots:
	void onAboutToShow ();
	void onGraphArrangementChanged ();
};

#endif // TWODGRAPHMENU_H
