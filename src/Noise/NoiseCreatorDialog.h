#ifndef NOISECREATORDIALOG_H
#define NOISECREATORDIALOG_H

class QGridLayout;
class QScrollArea;

#include "../CreatorDialog.h"
#include "../ParameterViewer.h"
#include "../StoreAssociatedComboBox_include.h"
class NoiseModule;


class NoiseCreatorDialog : public CreatorDialog, public ParameterViewer<Parameter::NoiseSimulation>
{
	Q_OBJECT	
	
public:
	NoiseCreatorDialog(NoiseSimulation *presetSimulation, NoiseModule *module);
	
private:
	class OpPointRecord {  // a class representing one row in the OpPoint selection view
	public:
		OpPointRecord (QString airfoil, QString polar, QString name, OpPoint *opPoint, bool readyForNoise)
			: airfoil(airfoil), polar(polar), name(name), opPoint(opPoint), checkBox(NULL), readyForNoise(readyForNoise) { }
		static bool sort(OpPointRecord &first, OpPointRecord &second) {
			if (first.airfoil == second.airfoil) {
				if (first.polar == second.polar) {
					return first.name.toDouble() < second.name.toDouble();
				} else {
					return first.polar < second.polar;
				}
			} else {
				return first.airfoil < second.airfoil;
			}
		}
		QString airfoil, polar, name;
		OpPoint *opPoint;
		QCheckBox *checkBox;
		bool readyForNoise;
	};
	
	void initView ();
	void prepareOpPointRecords(bool allPolars);
	void fillOpPointView();

	NoiseModule *m_module;
	NoiseSimulation *m_editedSimulation;
	QList<OpPointRecord> m_opPointRecords;  // store a sorted list of all OpPoints
	
	QButtonGroup *m_selectFromButtons;
	FoilComboBox *m_airfoilComboBox;
	PolarComboBox *m_polarComboBox;
	QScrollArea *m_opPointScrollArea;
	QWidget *m_opPointViewWidget;
	QWidget *m_originalBpmWidget;
	
private slots:
	void onSelectButtonsClicked (int id);
	void onPolarBoxChange ();
	void onAllButtonToggled (bool pressed);
	
	void onCreateButtonClicked ();
	void onUnitsChanged () { }  // no need for this
};

#endif // NOISECREATORDIALOG_H
