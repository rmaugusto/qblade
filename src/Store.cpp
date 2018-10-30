#include "Store.h"

#include <QComboBox>
#include <QDebug>
#include <QMessageBox>

#include <typeinfo>

#include "StorableObject_heirs.h"
#include "XWidgets.h"
#include "GlobalFunctions.h"
#include "Serializer.h"

/* this is the "sorting" rule for the storable objects (comparison of lower case strings) */
bool compareAlphabetically (StorableObject *T1, StorableObject *T2) {
    return T1->getName().toLower() < T2->getName().toLower();
}

/* try to sort numerically, if not possible fall back to lower case string comparison */
bool compareNumerically (StorableObject *T1, StorableObject *T2) {
    bool c1, c2;
    double v1 = T1->getName().toDouble(&c1);
    double v2 = T2->getName().toDouble(&c2);
    if (c1 && c2) return v1 < v2;
    else return compareAlphabetically(T1, T2);
}

StoreBase::StoreBase (StoreBase *parentStore, StoreBase *anotherParentStore) {
	if (parentStore) {
		parentStore->addChildStore(this);
        if (anotherParentStore) {
            anotherParentStore->addChildStore(this);
        }
	}

}

void StoreBase::addChildStore(StoreBase *childStore) {
	m_childStoreList.append(childStore);
}

template <class T>
Store<T>::Store(bool sameNameAllowed, StoreBase *parentStore, StoreBase *anotherParentStore, bool sort, SortType sortType)
	: StoreBase (parentStore, anotherParentStore)
{
	m_sameNameAllowed = sameNameAllowed;
    m_locked = false;
    m_sort = sort;
    m_sortType = sortType;
    m_lockMessage = "Store is locked!\n- Cannot add or remove objects";
}

template <class T>
Store<T>::~Store() {
	disableSignal();
	for (int i = 0; i < m_objectList.size(); ++i) {
		delete m_objectList.at(i);
	}
}

template <class T>
bool Store<T>::isLocked() {
	if (m_locked) {
		QMessageBox::critical(g_mainFrame, tr("Warning"), m_lockMessage, QMessageBox::Ok);
		return true;
	}
	return false;
}

template <class T>
void Store<T>::sortStore() {
	if (m_sort) {
		switch (m_sortType) {
		case ALPHABETICALLY: qSort(m_objectList.begin(), m_objectList.end(), compareAlphabetically);
			break;
		case NUMERICALLY: qSort(m_objectList.begin(), m_objectList.end(), compareNumerically);
			break;
		}
    }	
}

template <class T>
void Store<T>::printState()
{
	qDebug() << "Store: " << typeid(this).name();
	qDebug() << "StoreSize: " << size();
	for (int i = 0; i < size(); ++i) {
		qDebug() << i << at(i)->getName() <<
					" parent: " << (at(i)->getParent() ? at(i)->getParent()->getName() : "NULL") <<
					" grandParent: " << (at(i)->getParent() && at(i)->getParent()->getParent() ?
											 at(i)->getParent()->getParent()->getName() : "NULL");
	}
	qDebug() << "-";
}

template <class T>
bool Store<T>::add(T *newObject, int position) {
	bool nameExists = isNameExisting (newObject);
	if (nameExists) {
		nameExists = ! rename (newObject);
	}
	if (! nameExists) {
		if (position == -1) {
            m_objectList.append(newObject);
			sortStore();
		} else {
            m_objectList.insert(position, newObject);
			sortStore();
		}
		if (isSignalEnabled()) {
			emit objectListChanged(true);
		}
		return true;
	} else {
		delete newObject;
		return false;
	}
}

template <class T>
void Store<T>::remove (T *objectToRemove) {
    if (isLocked()) { return; }
	for (int i = 0; i < m_childStoreList.size(); ++i) {	 // first remove the children of the object
		m_childStoreList.at(i)->removeAllWithParent(objectToRemove);
	}
	
	m_objectList.removeOne(objectToRemove);
	if (isSignalEnabled()) {
		emit objectListChanged(true);
	}

	/* the actual deletion must be in the end, so that references can be cut as reaction to the objectListChanged signal */
	objectToRemove->deleteLater();
}

template <class T>
void Store<T>::removeAt (int indexToRemove) {
    if (isLocked()) { return; }
	remove (at(indexToRemove));
}

template <class T>
void Store<T>::replace (T *objectToRemove, T *newObject) {
    if (isLocked()) { delete newObject; return; }
	int index = m_objectList.indexOf(objectToRemove);
	delete m_objectList.at(index);
	m_objectList.replace(index, newObject);
	sortStore();
}

template <class T>
bool Store<T>::rename (T *objectToRename, QString newName) {
	if (newName != "") {
		if (! isNameExisting(newName, objectToRename->getParent())) {
			objectToRename->setName (newName);
			sortStore();

			return true;
		}
	}
	
	bool renamed = false;
	RenameDialog<T> *dialog = new RenameDialog<T> (objectToRename, this);
	int response = dialog->exec();

	if (response == RenameDialog<T>::Ok) {
		if (isSignalEnabled()) {
			emit objectRenamed(objectToRename->getName(), dialog->getNewName());
		}
		rename (objectToRename, dialog->getNewName());
		renamed = true;
	} else if (response == RenameDialog<T>::DontSave) {
		renamed = false;
	} else if (response == RenameDialog<T>::Overwrite) {
        if (isLocked()) { return false; }
		T* objectToOverwrite = getObjectByName (dialog->getNewName(), objectToRename->getParent());
		disableSignal();
		remove (objectToOverwrite);
		rename (objectToRename, dialog->getNewName());
		enableSignal();
		if (isSignalEnabled()) {
			emit objectListChanged(true);  // update comboBoxes to delete the no longer existing old name
		}
		renamed = true;
	}
	delete dialog;
	
	if (renamed) {
		sortStore();
	}
	return renamed;
}

template <class T>
T* Store<T>::at (int position) {
	return m_objectList.value(position);  // value returns 0 if position is out of bounds
}

template <class T>
int Store<T>::size () {
	return m_objectList.size();
}

template <class T>
bool Store<T>::isEmpty() {
	return m_objectList.isEmpty();
}

template <class T>
T* Store<T>::getObjectByName (QString objectName, StorableObject *parent) {
	for (int i = 0; i < m_objectList.size(); ++i) {
		if (m_objectList[i]->getName() == objectName && (!m_sameNameAllowed || m_objectList[i]->getParent() == parent)) {
			return m_objectList[i];
		}
	}
	return NULL;	
}

template <class T>
T *Store<T>::getObjectByNameOnly(QString objectName) {
	for (int i = 0; i < m_objectList.size(); ++i) {
		if (m_objectList[i]->getName() == objectName) {
			return m_objectList[i];
		}
	}
	return NULL;	
}

template <class T>
bool Store<T>::isNameExisting(QString name, StorableObject *parent) {
	return (getObjectByName(name, parent) != NULL);
}

template <class T>
bool Store<T>::isNameExisting(T *object) {
	return isNameExisting(object->getName(), object->getParent());
}

template <class T>
QString Store<T>::getNextName(QString baseName) {
	int maxNumber = 0;
	foreach (T* object, m_objectList) {
		if (object->getName() == baseName) {
			maxNumber = std::max(maxNumber, 2);
		} else if (object->getName().startsWith(baseName)) {
			const int number = object->getName().split('(').last().remove(')').toInt();
			maxNumber = std::max(maxNumber, number + 1);
		}
	}
	return (maxNumber < 2) ? baseName : QString ("%1 (%2)").arg(baseName).arg(maxNumber);
}

template <class T>
void Store<T>::serializeContent() {
	disableSignal();
	
	if (g_serializer.isReadMode()) {
		int n = g_serializer.readInt ();
		T *object;
		for (int i = 0; i < n; ++i) {
			object = T::newBySerialize();
			add(object);
		}
	} else {
		g_serializer.writeInt (this->size());
		for (int i = 0; i < this->size(); ++i) {
			this->at(i)->serialize();
		}
	}
	
	enableSignal();
}

template <class T>
void Store<T>::addAllCurves(QList<NewCurve*> *curves, QString xAxis, QString yAxis, NewGraph::GraphType graphType) {
	/* Since template specialization for "all classes that inherit X" is complicated before C++11, a dynamic cast
	 * is used here. */
	for (int i = 0; i < m_objectList.size(); ++i) {
		ShowAsGraphInterface *object = dynamic_cast<ShowAsGraphInterface*>(m_objectList[i]);
		if (object && object->isShownInGraph()) {
			NewCurve *curve = object->newCurve(xAxis, yAxis, graphType);
			if (curve) {
				curves->append(curve);
			}
		}
	}
}

template <class T>
void Store<T>::showAllCurves(bool show, ShowAsGraphInterface *showThisObject) {
	for (int i = 0; i < m_objectList.size(); ++i) {
		ShowAsGraphInterface *object = dynamic_cast<ShowAsGraphInterface*>(m_objectList[i]);
		if (object) {
			object->setShownInGraph(show);			
		}
    }
	if (showThisObject) {  // for "Show Curent Only"
		showThisObject->setShownInGraph(true);
	}
}

template <class T>
void Store<T>::removeAllWithParent (StorableObject *deletedParent)
{
	bool removedSomething = false;
	disableSignal();
	for (int i = size()-1; i >= 0; --i) {
        if (at(i)->hasParent(deletedParent)) {
			remove (at(i));
			removedSomething = true;
		}
	}
	enableSignal();
	if (removedSomething) {
		emit objectListChanged(true);
	}
}

/* to make it possible to separate the template header and implementation, all needed types have to be
 * instantiated here. For more information see:
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 * */
template class Store<WindField>;
template class Store<CBlade>;
template class Store<BladeStructure>;
template class Store<FASTSimulation>;
template class Store<CPolar>;
template class Store<CFoil>;
template class Store<C360Polar>;
template class Store<BEMData>;
template class Store<TBEMData>;
template class Store<CBEMData>;
template class Store<TData>;
template class Store<DMSData>;
template class Store<TDMSData>;
template class Store<CDMSData>;
template class Store<OpPoint>;
template class Store<BladeStructureLoading>;
template class Store<QLLTSimulation>;
template class Store<QLLTCutPlane>;
template class Store<NoiseSimulation>;
template class Store<Strut>;

//dont forget to add RenameDialog<Strut> to XWidgets for the Rename Dialog...


WindFieldStore g_windFieldStore (false);
AirfoilStore g_foilStore (false);
PolarStore g_polarStore (true, &g_foilStore);
Polar360Store g_360PolarStore (true, &g_polarStore);
RotorStore g_rotorStore (false, &g_360PolarStore);
VerticalRotorStore g_verticalRotorStore (false, &g_360PolarStore);
BladeStructureStore g_bladeStructureStore (true, &g_rotorStore);
FASTSimulationStore g_FASTSimulationStore (false, &g_windFieldStore, &g_bladeStructureStore);
TDataStore g_tdataStore (false, &g_rotorStore);
TDataStore g_verttdataStore (false, &g_verticalRotorStore);
BEMDataStore g_bemdataStore (true, &g_rotorStore);
TBEMDataStore g_tbemdataStore (true, &g_tdataStore);
CBEMDataStore g_cbemdataStore (true, &g_rotorStore);
DMSDataStore g_dmsdataStore (true, &g_verticalRotorStore);
TDMSDataStore g_tdmsdataStore (true, &g_verttdataStore);
CDMSDataStore g_cdmsdataStore (true, &g_verticalRotorStore);
OpPointStore g_oppointStore (true, &g_polarStore, NULL, true, StoreBase::NUMERICALLY);
BladeStructureLoadingStore g_bladestructureloadingStore (true, &g_bladeStructureStore);
QLLTVAWTSimulationStore g_QLLTVAWTSimulationStore(false, &g_verticalRotorStore, &g_windFieldStore);
QLLTHAWTSimulationStore g_QLLTHAWTSimulationStore(false, &g_rotorStore, &g_windFieldStore);
QLLTCutPlaneStore g_QLLTCutPlaneStore(false, &g_QLLTVAWTSimulationStore, &g_QLLTHAWTSimulationStore, false);
NoiseSimulationStore g_noiseSimulationStore(false, &g_oppointStore);
StrutStore g_StrutStore(true, &g_verticalRotorStore, &g_360PolarStore, false);

