#ifndef NOISEDOCK_H
#define NOISEDOCK_H

#include "../CreatorTwoDDock.h"
#include "../ParameterViewer.h"
class NoiseModule;


class NoiseDock : public CreatorTwoDDock<NoiseSimulation>, public ParameterViewer<Parameter::NoiseSimulation>
{
	Q_OBJECT

public:
	NoiseDock(const QString &title, QMainWindow *parent, Qt::WindowFlags flags, NoiseModule *module);
	~NoiseDock();
	
	bool isColorByOpPoint();
	
private:
	TwoDWidgetInterface* twoDInterface();
	virtual QList<NewGraph::GraphType> affectedGraphTypes();  // which graphs need to be rescaled after show/hide
	
	NoiseModule *m_module;
	QCheckBox *m_colorByOpPoint;
	
private slots:
	void onUnitsChanged();
	void onEditCopyButtonClicked ();
	void onRenameButtonClicked ();
	void onDeleteButtonClicked ();
	void onNewButtonClicked ();
	void onColorByOpPoint ();
};

#endif // NOISEDOCK_H
