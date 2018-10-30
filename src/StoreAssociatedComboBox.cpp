#include "StoreAssociatedComboBox.h"

#include "QDebug"

#include "Store.h"
#include "StorableObject_heirs.h"


StoreAssociatedComboBoxBase::StoreAssociatedComboBoxBase () {
    m_parentBox = NULL;
}

void StoreAssociatedComboBoxBase::setParentBox (StoreAssociatedComboBoxBase *parentBox) {
	m_parentBox = parentBox;
	updateContent();
	connect(parentBox, SIGNAL(valueChanged(QString)), this, SLOT(onParentBoxChanged()));
}

StoreAssociatedComboBoxBase* StoreAssociatedComboBoxBase::getParentBox() {
	return m_parentBox;
}

//StorableObject* StoreAssociatedComboBoxBase::getCurrentObjectUncasted () {
//	return (currentIndex() == -1 ? NULL : m_shownObjects.at(currentIndex()));
//}

template <class T>
StoreAssociatedComboBox<T>::StoreAssociatedComboBox (Store<T> *associatedStore, bool disableIfEmpty) {
	m_disableIfEmpty = disableIfEmpty;
	m_associatedStore = associatedStore;
	m_changeSignalEnabled = true;
	connect (this, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)));
	connect (this, SIGNAL(currentIndexChanged(QString)), SLOT(onCurrentIndexChanged(QString)));
	connect (m_associatedStore, SIGNAL(objectListChanged(bool)), this, SLOT(updateContent(bool)));
	connect (m_associatedStore, SIGNAL(objectRenamed(QString,QString)), this,
																SLOT(onObjectRenamed(QString,QString)));
	updateContent();
}

template <class T>
void StoreAssociatedComboBox<T>::onCurrentIndexChanged(int newIndex) {
	if (m_changeSignalEnabled) {
		emit valueChanged(newIndex);
		emit valueChanged(m_shownObjects[newIndex]);
	}
}

template <class T>
void StoreAssociatedComboBox<T>::onCurrentIndexChanged(const QString newText) {
	if (m_changeSignalEnabled) {
		emit valueChanged(newText);
	}
}

template <class T>
void StoreAssociatedComboBox<T>::onObjectRenamed (QString oldName, QString newName) {
	setItemText(findText(oldName), newName);
}

template <class T>
void StoreAssociatedComboBox<T>::updateContent(bool searchForLastActive) {
	m_changeSignalEnabled = false;
	T *lastActive = currentObject();
	bool lastActiveFound = false;
	
	clear();
	m_shownObjects.clear();
	
	for (int i = 0; m_associatedStore->at(i) != 0; ++i) {
        if (m_parentBox == NULL || m_parentBox->getCurrentObjectUncasted() ==
                                                                    m_associatedStore->at(i)->getParent()) {
			addItem(m_associatedStore->at(i)->getName());
			m_shownObjects.append(m_associatedStore->at(i));
//            qDebug() << "folgende gefunden: " << m_associatedStore->at(i)->getName();
			if (searchForLastActive && ! lastActiveFound && m_associatedStore->at(i) == lastActive) {
				setCurrentIndex(count() - 1);
				lastActiveFound = true;
			}
		}
	}
	if (m_disableIfEmpty) {
		setEnabled(count());
	}
    m_changeSignalEnabled = true;
	if (! lastActiveFound) {
		emit valueChanged(currentIndex());
		emit valueChanged(currentText());
		emit valueChanged(m_shownObjects.isEmpty() ? NULL : m_shownObjects[currentIndex()]);
	}

}

template <class T>
void StoreAssociatedComboBox<T>::onParentBoxChanged () {
	updateContent(false);
}

template <class T>
T* StoreAssociatedComboBox<T>::currentObject () {
	// a dynamic_cast is not possible here, because in case of a deleted object the dynamic typeinfo is lost
	return (currentIndex() == -1 ? NULL : static_cast<T*> (m_shownObjects.at(currentIndex())));
}

template <class T>
T* StoreAssociatedComboBox<T>::getObjectAt (int i) {
    // a dynamic_cast is not possible here, because in case of a deleted object the dynamic typeinfo is lost
    return ((i >= m_shownObjects.size() || i < 0 ) ? NULL : static_cast<T*> (m_shownObjects.at(i)));
}

template <class T>
void StoreAssociatedComboBox<T>::setCurrentObject(T *newObject) {
	setCurrentIndex(m_shownObjects.indexOf(newObject));
}

/* to make it possible to separate the template header and implementation, all needed types have to be
 * instantiated here. For more information see:
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 * */
template class StoreAssociatedComboBox<WindField>;
template class StoreAssociatedComboBox<CBlade>;
template class StoreAssociatedComboBox<BladeStructure>;
template class StoreAssociatedComboBox<FASTSimulation>;
template class StoreAssociatedComboBox<BEMData>;
template class StoreAssociatedComboBox<TBEMData>;
template class StoreAssociatedComboBox<CBEMData>;
template class StoreAssociatedComboBox<TData>;
template class StoreAssociatedComboBox<DMSData>;
template class StoreAssociatedComboBox<TDMSData>;
template class StoreAssociatedComboBox<CDMSData>;
template class StoreAssociatedComboBox<CPolar>;
template class StoreAssociatedComboBox<C360Polar>;
template class StoreAssociatedComboBox<CFoil>;
template class StoreAssociatedComboBox<OpPoint>;
template class StoreAssociatedComboBox<BladeStructureLoading>;
template class StoreAssociatedComboBox<QLLTSimulation>;
template class StoreAssociatedComboBox<QLLTCutPlane>;
template class StoreAssociatedComboBox<NoiseSimulation>;
template class StoreAssociatedComboBox<Strut>;
