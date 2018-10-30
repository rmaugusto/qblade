#ifndef STORABLEOBJECT_H
#define STORABLEOBJECT_H

#include <QString>
#include <QObject>
#include <QVector>


// NM TODO the whole parent thing should be only for managing dependencies. Implement e.g. m_usedFoil for CPolar
/**
 * @brief All classes that are ment to be stored in a Store must inherit this class
 *
 * This ensures that a stored object has members and methods that are required by the Store and avoids
 * redundancy in the stored objects classes. A StorableObject can be associated with a parent object
 * and even a grand parent object.
 */
class StorableObject : public QObject
{	
public:
	int getId();
	QString getName ();
	
	StorableObject* getParent (int position = 0);
	StorableObject* getChild (int position = 0);
	bool hasParent (StorableObject *potentialParent);
	// the next two should be protected and not public. Asap
	void setSingleParent (StorableObject *newParent);  /**< first removes all current parents and then adds newParent */
	void addParent (StorableObject *newParent);
	
	/**
	 * @brief Changes the name of the object.
	 *
	 * @attention It is not allowed to change the name of an object that is stored in a Store!
	 * Use Store::rename instead.
	 * @param newName The new name of the object.
	 */
	void setName (QString newName);
	
	virtual void serialize ();
	virtual void restorePointers ();
	
protected:
    StorableObject(QString name = "< no name >", StorableObject *parent = NULL);
	virtual ~StorableObject ();
	void addChild (StorableObject *newChild);
	void removeParent (StorableObject *formerParent);
	void removeAllParents ();
	void removeChild (StorableObject *formerChild);
	
	static int nextId;  /**< Counts up each time an ID was used */
	int m_objectId;  /**< This ID is used for the restauration of references after loading a project. */
	QString m_objectName;  /**< The name of the object. Will appear in StoreAssociatedComboBox. */
	QVector<StorableObject*> m_parentVector;  /**< Holds a reference to all parent objects */
	QVector<StorableObject*> m_childVector;  /**< Holds a reference to all child objects */
};

#endif // STORABLEOBJECT_H
