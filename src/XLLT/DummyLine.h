#ifndef DUMMYLINE_H
#define DUMMYLINE_H

class DummyLine
{
public:
    DummyLine(float lx, float ly, float lz, float tx, float ty, float tz, float gamma=0, int station = 0, bool shed = false, float coresize = 0, int fromtimel = 0, int fromtimet = 0, bool ishub = false, bool istip = false);
    DummyLine();
    void serialize();
    float Lx, Ly, Lz, Tx, Ty, Tz;
    int fromTimeL, fromTimeT;
    float Gamma;
    float CoreSizeSquared;
    bool Shed;
    int fromStation;
    bool isHub;
    bool isTip;

};

#endif // DUMMYLINE_H
