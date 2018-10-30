#include "DummyLine.h"
#include "src/Serializer.h"

DummyLine::DummyLine(float lx, float ly, float lz, float tx, float ty, float tz, float gamma, int station, bool shed, float coresizesquared, int fromtimel, int fromtimet, bool ishub, bool istip)
{
    Lx = lx;
    Ly = ly;
    Lz = lz;
    Tx = tx;
    Ty = ty;
    Tz = tz;
    Gamma = gamma;
    Shed = shed;
    fromStation = station;
    CoreSizeSquared = coresizesquared;
    fromTimeL = fromtimel;
    fromTimeT = fromtimet;
    isHub = ishub;
    isTip = istip;
}

DummyLine::DummyLine(){
    Lx = 0;
    Ly = 0;
    Lz = 0;
    Tx = 0;
    Ty = 0;
    Tz = 0;
    Gamma = 0;
    Shed = false;
    fromStation = 0;
    fromTimeL = 0;
    fromTimeT = 0;
    isHub = false;
    isTip = false;
}


void DummyLine::serialize(){
    if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100053){
    double dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Gamma = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    CoreSizeSquared = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Lx = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Tx = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Ly = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Ty = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Lz = dummy;
    g_serializer.readOrWriteDouble(&dummy);
    Tz = dummy;
    }
    else{
        g_serializer.readOrWriteFloat(&Gamma);
        g_serializer.readOrWriteFloat(&CoreSizeSquared);
        g_serializer.readOrWriteFloat(&Lx);
        g_serializer.readOrWriteFloat(&Tx);
        g_serializer.readOrWriteFloat(&Ly);
        g_serializer.readOrWriteFloat(&Ty);
        g_serializer.readOrWriteFloat(&Lz);
        g_serializer.readOrWriteFloat(&Tz);
    }

    g_serializer.readOrWriteInt(&fromTimeL);
    g_serializer.readOrWriteInt(&fromTimeT);
    g_serializer.readOrWriteBool(&Shed);
    g_serializer.readOrWriteInt(&fromStation);
    if (g_serializer.getArchiveFormat() >= 100054){
        g_serializer.readOrWriteBool(&isHub);
        g_serializer.readOrWriteBool(&isTip);
    }

}

