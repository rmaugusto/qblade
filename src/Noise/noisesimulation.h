#ifndef NOISESIMULATION_H
#define NOISESIMULATION_H

#include "../Objects/CVector.h"
#include "NoiseModule.h"
#include "../XBEM/Blade.h"
#include "../Graph/ShowAsGraphInterface.h"
#include "../StorableObject.h"

class NoiseModule;

class NoiseSimulation : public StorableObject, public ShowAsGraphInterface
{
    friend class NoiseModule;
    Q_OBJECT
public:
    explicit NoiseSimulation();

signals:

public slots:

};

#endif // NOISESIMULATION_H
