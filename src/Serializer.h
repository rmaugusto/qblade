#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QVector>
#include <QPair>
#include <QException>
#include <QString>
#include "../src/Objects/CVector.h"
#include "../src/Objects/CVectorf.h"

class QDataStream;
class StorableObject;
class QColor;
class QPen;
class QStringList;
class QBitArray;


class Serializer
{
public:
	class Exception : public QException {
	public:
		Exception(QString message) : message(message) { }
		QString message;
	};
	
	Serializer();
	
	enum Mode {READ, WRITE};
	
	void setDataStream (QDataStream *stream);
	void setArchiveFormat (int archiveFormat) { m_archiveFormat = archiveFormat; }
	int getArchiveFormat () { return m_archiveFormat; }
	void setMode (Mode mode);
	bool isReadMode () { return m_isReadMode; }
	void addNewObject (int oldId, StorableObject *newAddress);
	void restoreAllPointers ();
	void restorePointer (StorableObject **pointer);
	int readInt ();
	void writeInt (int value);
	
	void readOrWriteBool (bool*);
	void readOrWriteInt (int*);
	void readOrWriteIntArray1D (int*, int);
	void readOrWriteIntArray2D (int*, int, int);
	void readOrWriteIntVector1D (QVector<int>*);
	void readOrWriteFloat (float*);
	void readOrWriteFloatArray1D (float**, int);
	void readOrWriteFloatArray2D (float***, int, int);
	void readOrWriteFloatArray3D (float****, int, int, int);
    void readOrWriteCVectorArray1D (CVector**, int);
    void readOrWriteCVectorfArray1D (CVectorf**, int);
    void readOrWriteCVectorArray2D (CVector***, int, int);
    void readOrWriteCVectorfArray2D (CVectorf***, int, int);
    void readOrWriteCVectorArray3D (CVector****, int, int, int);
    void readOrWriteCVectorfArray3D (CVectorf****, int, int, int);
	void readOrWriteDouble (double*);
	void readOrWriteDoubleArray1D (double*, int);
	void readOrWriteDoubleArray2D (double*, int, int);
	void readOrWriteDoubleArray3D (double*, int, int, int);
	void readOrWriteDoubleVector1D (QVector<double>*);
	void readOrWriteDoubleVector2D (QVector<QVector<double> >*);
	void readOrWriteDoubleVector3D (QVector<QVector<QVector<double> > >*);
	void readOrWriteDoubleList1D (QList<double>*);
	void readOrWriteDoubleList2D (QList<QList<double> >*);
	void readOrWritePairIntDoubleVector (QPair<int,QVector<double> >*);
	void readOrWriteString (QString*);
	void readOrWriteStringList (QStringList*);
    void readOrWriteStringList1D (QList<QString>*);
	void readOrWriteColor (QColor*);
	void readOrWritePen (QPen*);
	void readOrWriteBitArray (QBitArray*);
	template <class ENUM>
	void readOrWriteEnum (ENUM*);  // attention: this function is not typesafe. Use with enums exclusively
	template <class OBJECT>
	void readOrWriteStorableObject (OBJECT*);  // attention: this function is not typesafe. Use with StorableObjects exclusively
	template <class OBJECT>
	void readOrWriteStorableObjectVector (QVector<OBJECT>*);  // attention: this function is not typesafe.

	QDataStream *m_stream;
	int m_archiveFormat;  /**< The save format of current project. Over time QBlades save format changes and the number increases. */
	bool m_isReadMode;
	QVector<StorableObject*> m_oldIds;  /**< IDs of objects when saved to project. Index corresponds to m_newAddresses. */
	QVector<StorableObject*> m_newAddresses;  /**< New adresses of objects that were loaded. Index corresponds to m_oldIds. */
};

template <class ENUM>  // template functions have to be in the header
void Serializer::readOrWriteEnum (ENUM *value) {
	int intRepresentation = static_cast<int> (*value);
	readOrWriteInt(&intRepresentation);
	*value = static_cast<ENUM> (intRepresentation);
}

template <class OBJECT>
void Serializer::readOrWriteStorableObject (OBJECT *object) {
	if (m_isReadMode) {
		int oldId = readInt();
		*object = reinterpret_cast<OBJECT> (oldId);  // save the oldId temporarily as if it was a pointer
	} else {
		writeInt((*object)->getId());
	}
}

template <class OBJECT>
void Serializer::readOrWriteStorableObjectVector (QVector<OBJECT> *vector) {
	if (m_isReadMode) {
		int n = readInt();
		vector->resize(n);
	} else {
		writeInt(vector->size());
	}
	
	for (int i = 0; i < vector->size(); ++i) {
		readOrWriteStorableObject(&((*vector)[i]));
	}
}

extern Serializer g_serializer;

#endif // SERIALIZER_H
