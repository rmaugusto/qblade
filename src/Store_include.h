#ifndef STORE_INCLUDE_H
#define STORE_INCLUDE_H

class WindField;
class BladeStructure;
class FASTSimulation;
class CPolar;
class CBlade;
class CFoil;
class C360Polar;
class TData;
class BEMData;
class TBEMData;
class CBEMData;
class DMSData;
class TDMSData;
class CDMSData;
class OpPoint;
class BladeStructureLoading;
class QLLTSimulation;
class QLLTCutPlane;
class NoiseSimulation;
class Strut;


template <class T> class Store;
typedef Store<WindField> WindFieldStore;
typedef Store<CBlade> RotorStore;
typedef Store<CBlade> VerticalRotorStore;
typedef Store<BladeStructure> BladeStructureStore;
typedef Store<FASTSimulation> FASTSimulationStore;
typedef Store<CPolar> PolarStore;
typedef Store<CFoil> AirfoilStore;
typedef Store<C360Polar> Polar360Store;
typedef Store<BEMData> BEMDataStore;
typedef Store<TBEMData> TBEMDataStore;
typedef Store<CBEMData> CBEMDataStore;
typedef Store<TData> TDataStore;
typedef Store<DMSData> DMSDataStore;
typedef Store<TDMSData> TDMSDataStore;
typedef Store<CDMSData> CDMSDataStore;
typedef Store<OpPoint> OpPointStore;
typedef Store<BladeStructureLoading> BladeStructureLoadingStore;
typedef Store<QLLTSimulation> QLLTHAWTSimulationStore;
typedef Store<QLLTSimulation> QLLTVAWTSimulationStore;
typedef Store<QLLTCutPlane> QLLTCutPlaneStore;
typedef Store<NoiseSimulation> NoiseSimulationStore;
typedef Store<Strut> StrutStore;


#endif // STORE_INCLUDE_H
