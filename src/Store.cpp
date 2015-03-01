#include "Store.h"

#include <QComboBox>
#include <QDebug>
#include <typeinfo>

#include "StorableObject_heirs.h"
#include "XWidgets.h"
#include "GlobalFunctions.h"
#include "Serializer.h"


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
Store<T>::Store(bool sameNameAllowed, StoreBase *parentStore, StoreBase *anotherParentStore)
	: StoreBase (parentStore, anotherParentStore)
{
	m_sameNameAllowed = sameNameAllowed;
}

template <class T>
Store<T>::~Store() {
	disableSignal();
	for (int i = 0; i < m_objectList.size(); ++i) {
		delete m_objectList.at(i);
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
		} else {
			m_objectList.insert(position, newObject);			
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
	for (int i = 0; i < m_childStoreList.size(); ++i) {	 // first remove the children of the object
		m_childStoreList.at(i)->removeAllWithParent(objectToRemove);
	}
	
//	qDebug() << "lösche: " << objectToRemove->getName();
	delete objectToRemove;
	m_objectList.removeOne(objectToRemove);
	if (isSignalEnabled()) {
		emit objectListChanged(true);
	}
}

template <class T>
void Store<T>::removeAt (int indexToRemove) {
	remove (at(indexToRemove));
}

template <class T>
void Store<T>::replace (T *objectToRemove, T *newObject) {
	int index = m_objectList.indexOf(objectToRemove);
	delete m_objectList.at(index);
	m_objectList.replace(index, newObject);
}

template <class T>
bool Store<T>::rename (T *objectToRename, QString newName) {
	if (newName != "") {
		if (! isNameExisting(newName, objectToRename->getParent())) {
			objectToRename->setName (newName);
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


WindFieldStore g_windFieldStore (false);
AirfoilStore g_foilStore (false);
PolarStore g_polarStore (true, &g_foilStore);
Polar360Store g_360PolarStore (true, &g_polarStore);
RotorStore g_rotorStore (false, &g_360PolarStore);
VerticalRotorStore g_verticalRotorStore (false, &g_360PolarStore);
BladeStructureStore g_bladeStructureStore (true, &g_rotorStore);
FASTSimulationStore g_FASTSimulationStore (false, &g_windFieldStore, &g_bladeStructureStore);
TDataStore g_tdataStore (true, &g_rotorStore);
TDataStore g_verttdataStore (true, &g_verticalRotorStore);
BEMDataStore g_bemdataStore (true, &g_rotorStore);
TBEMDataStore g_tbemdataStore (true, &g_tdataStore);
CBEMDataStore g_cbemdataStore (true, &g_rotorStore);
DMSDataStore g_dmsdataStore (true, &g_verticalRotorStore);
TDMSDataStore g_tdmsdataStore (true, &g_verttdataStore);
CDMSDataStore g_cdmsdataStore (true, &g_verticalRotorStore);
OpPointStore g_oppointStore (true, &g_polarStore);
BladeStructureLoadingStore g_bladestructureloadingStore (true, &g_bladeStructureStore);
QLLTVAWTSimulationStore g_QLLTVAWTSimulationStore(false, &g_verticalRotorStore);
QLLTHAWTSimulationStore g_QLLTHAWTSimulationStore(false, &g_rotorStore);
NoiseSimulationStore g_NoiseSimulationStore(false, &g_rotorStore);

