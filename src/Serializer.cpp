#include "Serializer.h"

#include <QDebug>
#include <QDataStream>
#include <QColor>
#include <QPen>
#include <QBitArray>

#include "StorableObject.h"


Serializer::Serializer () {
	m_stream = NULL;
	m_archiveFormat = -1;
}

void Serializer::setDataStream (QDataStream *stream) {
	m_stream = stream;
}

void Serializer::setMode (Mode mode) {
	m_oldIds.clear();
	m_newAddresses.clear();
	m_archiveFormat = -1;
	m_isReadMode = (mode == READ);
}

void Serializer::addNewObject (int oldId, StorableObject *newAddress) {
	m_oldIds.append(reinterpret_cast<StorableObject*>(oldId));
	m_newAddresses.append(newAddress);
}

void Serializer::restoreAllPointers () {
	for (int i = 0; i < m_newAddresses.size(); ++i) {
		m_newAddresses[i]->restorePointers();
	}
}

void Serializer::restorePointer (StorableObject **pointer) {
	// NM a crash here means that indexOf returned -1. That should never happen.
	*pointer = m_newAddresses[m_oldIds.indexOf(*pointer)];
}

int Serializer::readInt () {
	int value;
	*m_stream >> value;
	return value;
}

void Serializer::writeInt (int value) {
	*m_stream << value;
}

void Serializer::readOrWriteBool (bool *value) {
	m_isReadMode ? (*m_stream >> *value) : (*m_stream << *value);
}

void Serializer::readOrWriteInt (int *value) {
	m_isReadMode ? (*m_stream >> *value) : (*m_stream << *value);
}

void Serializer::readOrWriteIntArray1D (int *array, int dim1) {
	if (m_isReadMode) {
		for (int i = 0; i < dim1; ++i) {
			*m_stream >> array[i];
		}
	} else {
		for (int i = 0; i < dim1; ++i) {
			*m_stream << array[i];
		}	
	}
}

void Serializer::readOrWriteIntArray2D(int *array, int dim1, int dim2) {
	readOrWriteIntArray1D(array, dim1*dim2);
}

void Serializer::readOrWriteIntVector1D (QVector<int> *vector) {
	m_isReadMode ? (*m_stream >> *vector) : (*m_stream << *vector);
}

void Serializer::readOrWriteFloat (float *value) {
	m_isReadMode ? (*m_stream >> *value) : (*m_stream << *value);
}

void Serializer::readOrWriteFloatArray1D (float **array, int dim1) {
	if (m_isReadMode) {
		*array = new float[dim1];
		for (int i = 0; i < dim1; ++i) {
			*m_stream >> (*array)[i];
		}
	} else {
		for (int i = 0; i < dim1; ++i) {
			*m_stream << (*array)[i];
		}
	}
}

void Serializer::readOrWriteCVectorArray1D (CVector **array, int dim1) {
    if (m_isReadMode) {
        *array = new CVector[dim1];
        for (int i = 0; i < dim1; ++i) {
            double val;
            *m_stream >> val;
            (*array)[i].x = val;
            *m_stream >> val;
            (*array)[i].y = val;
            *m_stream >> val;
            (*array)[i].z = val;
        }
    } else {
        for (int i = 0; i < dim1; ++i) {
            *m_stream << (*array)[i].x;
            *m_stream << (*array)[i].y;
            *m_stream << (*array)[i].z;
        }
    }
}

void Serializer::readOrWriteCVectorfArray1D (CVectorf **array, int dim1) {
    if (m_isReadMode) {
        *array = new CVectorf[dim1];
        if (getArchiveFormat() < 100053){
            for (int i = 0; i < dim1; ++i) {
                double val;
                *m_stream >> val;
                (*array)[i].x = val;
                *m_stream >> val;
                (*array)[i].y = val;
                *m_stream >> val;
                (*array)[i].z = val;
            }
        }
        else{
            for (int i = 0; i < dim1; ++i) {
                float val;
                *m_stream >> val;
                (*array)[i].x = val;
                *m_stream >> val;
                (*array)[i].y = val;
                *m_stream >> val;
                (*array)[i].z = val;
            }
        }
    } else {
        for (int i = 0; i < dim1; ++i) {
            *m_stream << (*array)[i].x;
            *m_stream << (*array)[i].y;
            *m_stream << (*array)[i].z;
        }
    }
}

void Serializer::readOrWriteFloatArray2D (float ***array, int dim1, int dim2) {
	if (m_isReadMode) {
		*array = new float*[dim1];
	}
	
	for (int i = 0; i < dim1; ++i) {
		readOrWriteFloatArray1D (&((*array)[i]), dim2);
	}
}

void Serializer::readOrWriteCVectorArray2D (CVector ***array, int dim1, int dim2) {
    if (m_isReadMode) {
        *array = new CVector*[dim1];
    }

    for (int i = 0; i < dim1; ++i) {
        readOrWriteCVectorArray1D (&((*array)[i]), dim2);
    }
}

void Serializer::readOrWriteCVectorfArray2D (CVectorf ***array, int dim1, int dim2) {
    if (m_isReadMode) {
        *array = new CVectorf*[dim1];
    }

    for (int i = 0; i < dim1; ++i) {
        readOrWriteCVectorfArray1D (&((*array)[i]), dim2);
    }
}

void Serializer::readOrWriteFloatArray3D (float ****array, int dim1, int dim2, int dim3) {
	if (m_isReadMode) {
		*array = new float**[dim1];
	}
	
	for (int i = 0; i < dim1; ++i) {
		readOrWriteFloatArray2D (&((*array)[i]), dim2, dim3);
	}
}

void Serializer::readOrWriteCVectorArray3D (CVector ****array, int dim1, int dim2, int dim3) {
    if (m_isReadMode) {
        *array = new CVector**[dim1];
    }

    for (int i = 0; i < dim1; ++i) {
        readOrWriteCVectorArray2D (&((*array)[i]), dim2, dim3);
    }
}

void Serializer::readOrWriteCVectorfArray3D (CVectorf ****array, int dim1, int dim2, int dim3) {
    if (m_isReadMode) {
        *array = new CVectorf**[dim1];
    }

    for (int i = 0; i < dim1; ++i) {
        readOrWriteCVectorfArray2D (&((*array)[i]), dim2, dim3);
    }
}


void Serializer::readOrWriteDouble (double *value) {
	m_isReadMode ? (*m_stream >> *value) : (*m_stream << *value);
}

void Serializer::readOrWriteDoubleArray1D (double *array, int dim1) {
	if (m_isReadMode) {
		for (int i = 0; i < dim1; ++i) {
			*m_stream >> array[i];
		}
	} else {
		for (int i = 0; i < dim1; ++i) {
			*m_stream << array[i];
		}
	}
}

void Serializer::readOrWriteDoubleArray2D (double *array, int dim1, int dim2) {
	readOrWriteDoubleArray1D(array, dim1*dim2);
}

void Serializer::readOrWriteDoubleArray3D (double *array, int dim1, int dim2, int dim3) {
	readOrWriteDoubleArray1D(array, dim1*dim2*dim3);
}

void Serializer::readOrWriteDoubleVector1D (QVector<double> *vector) {
	m_isReadMode ? (*m_stream >> *vector) : (*m_stream << *vector);
}

void Serializer::readOrWriteDoubleVector2D (QVector<QVector<double> > *vector) {
	m_isReadMode ? (*m_stream >> *vector) : (*m_stream << *vector);
}

void Serializer::readOrWriteDoubleVector3D (QVector<QVector<QVector<double> > > *vector) {
	m_isReadMode ? (*m_stream >> *vector) : (*m_stream << *vector);
}

void Serializer::readOrWriteDoubleList1D (QList<double> *list) {
	m_isReadMode ? (*m_stream >> *list) : (*m_stream << *list);
}

void Serializer::readOrWriteDoubleList2D (QList<QList<double> > *list) {
	m_isReadMode ? (*m_stream >> *list) : (*m_stream << *list);
}

void Serializer::readOrWritePairIntDoubleVector(QPair<int, QVector<double> > *pair) {
	m_isReadMode ? (*m_stream >> *pair) : (*m_stream << *pair);
}

void Serializer::readOrWriteString (QString *value) {
	m_isReadMode ? (*m_stream >> *value) : (*m_stream << *value);
}

void Serializer::readOrWriteStringList (QStringList *list) {
	m_isReadMode ? (*m_stream >> *list) : (*m_stream << *list);
}

void Serializer::readOrWriteStringList1D (QList<QString> *list) {
    m_isReadMode ? (*m_stream >> *list) : (*m_stream << *list);
}

void Serializer::readOrWriteColor (QColor *color) {
	m_isReadMode ? (*m_stream >> *color) : (*m_stream << *color);
}

void Serializer::readOrWritePen(QPen *pen) {
	m_isReadMode ? (*m_stream >> *pen) : (*m_stream << *pen);
}

void Serializer::readOrWriteBitArray(QBitArray *array) {
	m_isReadMode ? (*m_stream >> *array) : (*m_stream << *array);
}

Serializer g_serializer;
