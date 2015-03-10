#ifndef STOREASSOCIATEDCOMBOBOX_INCLUDE_H
#define STOREASSOCIATEDCOMBOBOX_INCLUDE_H

template <class T> class StoreAssociatedComboBox;
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
class NoiseSimulation;

typedef StoreAssociatedComboBox<WindField> WindFieldComboBox;
typedef StoreAssociatedComboBox<CBlade> RotorComboBox;
typedef StoreAssociatedComboBox<BladeStructure> BladeStructureComboBox;
typedef StoreAssociatedComboBox<FASTSimulation> FASTSimulationComboBox;
typedef StoreAssociatedComboBox<BEMData> BEMDataComboBox;
typedef StoreAssociatedComboBox<CBEMData> CBEMDataComboBox;
typedef StoreAssociatedComboBox<TBEMData> TBEMDataComboBox;
typedef StoreAssociatedComboBox<TData> TDataComboBox;
typedef StoreAssociatedComboBox<DMSData> DMSDataComboBox;
typedef StoreAssociatedComboBox<TDMSData> TDMSDataComboBox;
typedef StoreAssociatedComboBox<CDMSData> CDMSDataComboBox;
typedef StoreAssociatedComboBox<CPolar> PolarComboBox;
typedef StoreAssociatedComboBox<C360Polar> Polar360ComboBox;
typedef StoreAssociatedComboBox<CFoil> FoilComboBox;
typedef StoreAssociatedComboBox<OpPoint> OpPointComboBox;
typedef StoreAssociatedComboBox<BladeStructureLoading> BladeStructureLoadingComboBox;
typedef StoreAssociatedComboBox<QLLTSimulation> LLTSimulationComboBox;
typedef StoreAssociatedComboBox<NoiseSimulation> NoiseSimulationComboBox;






#endif // STOREASSOCIATEDCOMBOBOX_INCLUDE_H
