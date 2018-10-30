#ifndef LINESTYLEDIALOG_H
#define LINESTYLEDIALOG_H

#include <QDialog>
#include <QPen>

class LineStyleComboBox;
class NewColorButton;


class LineStyleDialog : public QDialog
{
	Q_OBJECT
	
public:
	LineStyleDialog(QWidget *parent, QPen pen, bool style, bool width, bool color);
	QPen getPen();
	
public slots:
	void setPen(QPen pen);
	void setColor(QColor color);
	
private:
	LineStyleComboBox *m_styleButton, *m_widthButton;
	NewColorButton *m_colorButton;
	QPushButton *m_okButton, *m_cancelButton;
};

#endif // LINESTYLEDIALOG_H
