#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>
#include <QVector>

#include "Store.h"
#include "Graph/ShowAsGraphInterface.h"

class ColorManager
{
public:
	ColorManager();
	QColor getColor(int index);
	template <class T>
	QColor getLeastUsedColor(Store<T> *store);
	
private:
	QVector<QColor> m_colors;
};


/* implementation */
template <class T>
QColor ColorManager::getLeastUsedColor(Store<T> *store) {
	QVector<int> incidence (m_colors.size(), 0);
	for (int i = 0; i < store->size(); ++i) {
		const int colorIndex = m_colors.indexOf(store->at(i)->getPen().color());
		if (colorIndex != -1) {
			incidence[colorIndex]++;
		}
	}
	
	int uses = 0;
	int leastUsed;
	do {
		leastUsed = incidence.indexOf(uses);  // find the first color that is used 'used' times
		uses++;
	} while (leastUsed == -1);
	
	return m_colors[leastUsed];
}

extern ColorManager g_colorManager;


#endif // COLORMANAGER_H
