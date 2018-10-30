#include "StorableObject.h"

#include <QDebug>

#include "Serializer.h"


int StorableObject::nextId = 0;

StorableObject::StorableObject(QString name, StorableObject *parent) {
	m_objectName = name;
	if (parent) {
		addParent(parent);
	}
	
	m_objectId = nextId++;	
}

StorableObject::~StorableObject() {
	for (int i = m_parentVector.size()-1; i >= 0; --i) {
		m_parentVector[i]->removeChild(this);
	}
}

void StorableObject::addParent(StorableObject *newParent) {
	if (!m_parentVector.contains(newParent)) {
		m_parentVector.append(newParent);
		newParent->addChild(this);
	}
}

void StorableObject::addChild(StorableObject *newChild) {
	if (!m_childVector.contains(newChild)) {
		m_childVector.append(newChild);
		newChild->addParent(this);
	}	
}

void StorableObject::removeParent(StorableObject *formerParent) {
	int pos = m_parentVector.indexOf(formerParent);
	if (pos != -1) {
		m_parentVector.remove(pos);
		formerParent->removeChild(this);
	}
}

void StorableObject::removeAllParents() {
	for (int i = m_parentVector.size()-1; i >= 0; --i) {
		removeParent(m_parentVector[i]);
	}
}

void StorableObject::removeChild(StorableObject *formerChild) {
	int pos = m_childVector.indexOf(formerChild);
	if (pos != -1) {
		m_childVector.remove(pos);
		formerChild->removeParent(this);
	}
}

void StorableObject::setSingleParent(StorableObject *newParent) {
	removeAllParents();  // remove old parents
    addParent(newParent);  // and add the new one
}

int StorableObject::getId() {
	return m_objectId;
}

QString StorableObject::getName() {
	return m_objectName;
}

StorableObject* StorableObject::getParent(int position) {
	if (m_parentVector.size() > position) {
		return m_parentVector[position];
	} else {
		return NULL;
	}
}

StorableObject *StorableObject::getChild(int position) {
	if (m_childVector.size() > position) {
		return m_childVector[position];
	} else {
		return NULL;
	}
}

bool StorableObject::hasParent(StorableObject *potentialParent) {
	for (int i = 0; i < m_parentVector.size(); ++i) {
		if (m_parentVector[i] == potentialParent) {
			return true;
		}
	}
	return false;
}

void StorableObject::setName(QString newName) {
	m_objectName = newName;
}

void StorableObject::serialize() {
	if (g_serializer.isReadMode()) {
		int oldId = g_serializer.readInt();
		g_serializer.addNewObject(oldId, this);
	} else {
		g_serializer.writeInt(m_objectId);
	}
	
	g_serializer.readOrWriteString (&m_objectName);
	g_serializer.readOrWriteStorableObjectVector (&m_parentVector);
	g_serializer.readOrWriteStorableObjectVector (&m_childVector);
}

void StorableObject::restorePointers() {
	for (int i = 0; i < m_parentVector.size(); ++i) {
		g_serializer.restorePointer(&(m_parentVector[i]));
	}
	for (int i = 0; i < m_childVector.size(); ++i) {
		g_serializer.restorePointer(&(m_childVector[i]));
	}
}
