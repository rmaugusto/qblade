#include "LineStyleComboBox.h"

#include <QItemDelegate>
#include <QStylePainter>


/**
 * @brief Delegate for painting QPen items.
 *
 * This class is meant to be used in a ComboBox to paint a QPen item.
 */
class PenDelegate : public QItemDelegate
{
public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
		if (option.state & QStyle::State_Selected) {
			painter->fillRect(option.rect, option.palette.highlight());
		}
		
		painter->setPen(index.data(Qt::UserRole).value<QPen>());
		painter->drawLine(option.rect.left() + 7,
						  option.rect.top() + option.rect.height()/2,
						  option.rect.right() - 7,
						  option.rect.top() + option.rect.height()/2);
	}
};


LineStyleComboBox::LineStyleComboBox(Application application) {
	m_application = application;
	
	PenDelegate *delegate = new PenDelegate();
	setItemDelegate(delegate);
	addItem("", QVariant(QPen()));
	addItem("", QVariant(QPen()));
	addItem("", QVariant(QPen()));
	addItem("", QVariant(QPen()));
	addItem("", QVariant(QPen()));
	
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged()));
}

void LineStyleComboBox::paintEvent(QPaintEvent */*event*/) {
	QStylePainter painter (this);
	QStyleOptionComboBox styleOption;
	styleOption.init(this);
	
	painter.drawComplexControl(QStyle::CC_ComboBox, styleOption);  // paint the typical ComboBox look
	
	QRect inner = painter.style()->subElementRect(QStyle::SE_ComboBoxFocusRect, &styleOption);
	painter.setClipRect(inner);
	
	painter.setPen(currentData().value<QPen>());
	painter.drawLine(inner.left() + 7,
					 inner.top() + inner.height()/2,
					 inner.right() - 7,
					 inner.top() + inner.height()/2);
}

QPen LineStyleComboBox::currentPen() {
	return currentData().value<QPen>();
}

void LineStyleComboBox::onCurrentIndexChanged() {
	emit currentPenChanged(currentPen());
}

void LineStyleComboBox::setCurrentPen(QPen pen) {
	QPen possiblePen (pen);
	
	for (int i = 0; i < count(); ++i) {
		if (m_application == STYLE) {
			switch (i) {
			case 0: possiblePen.setStyle(Qt::SolidLine); break;
			case 1: possiblePen.setStyle(Qt::DashLine); break;
			case 2: possiblePen.setStyle(Qt::DotLine); break;
			case 3: possiblePen.setStyle(Qt::DashDotLine); break;
			case 4: possiblePen.setStyle(Qt::DashDotDotLine); break;
			}
		} else {
			possiblePen.setWidth(i+1);
		}
		setItemData(i, QVariant(possiblePen));
		
		if (possiblePen == pen) {
			setCurrentIndex(i);
		}
	}
}

QSize LineStyleComboBox::sizeHint() const {
	QSize boxSize = QComboBox::sizeHint();
	boxSize.setWidth(130);
	return boxSize;
}
