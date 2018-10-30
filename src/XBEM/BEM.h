/****************************************************************************

    BEM Class
        Copyright (C) 2010 David Marten david.marten@tu-berlin.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef BEM_H
#define BEM_H

#include <QtWidgets>
#include "../MainFrame.h"
#include "../Misc/ColorButton.h"
#include "../Misc/GLLightDlg.h"
#include "BladeDelegate.h"
#include "BladeAxisDelegate.h"
#include "../Graph/QGraph.h"
#include "BData.h"
#include "BEMData.h"
#include "../Misc/NumberEdit.h"
#include "TData.h"
#include "TBEMData.h"
#include "CBEMData.h"
#include "SimuWidget.h"
#include "../Misc/ColorButton.h"
#include "../Misc/LineCbBox.h"
#include "../Misc/LineButton.h"
#include "../Misc/LineDelegate.h"
#include "BEMToolbar.h"
class GLWidget;


class QBEM : public QWidget
{

    friend class MainFrame;
    friend class BData;
    friend class TBEMData;
    friend class BEMData;
    friend class TwoDWidget;
    friend class OptimizeDlg;
    friend class CreateBEMDlg;
    friend class SimuWidget;
    friend class Edit360PolarDlg;
    friend class DisplaySettingsDlg;
    ///// new code JW /////
    friend class QDMS;
    friend class OptimizeDlgVAWT;
    friend class CreateDMSDlg;
	friend class SimuWidgetDMS;
	friend class BEMDock;
	friend class DMSDock;
    ///// new code JW /////
	friend class WindFieldMenu;  // new NM

    Q_OBJECT

public:


        QBEM(QWidget *parent = NULL);
		
		QStringList prepareMissingObjectMessage();
		
private slots:
        //methods
        ///// JW virtual methods //////////
        virtual void CheckButtons();
        virtual void OnSaveBlade();
        virtual void OnNewBlade();
        virtual void InitBladeTable();
        virtual void OnDeleteBlade();
        virtual void OnEditBlade();
        virtual void OnInsertBefore();
        virtual void OnInsertAfter();
        //virtual void OnCellChanged(QWidget *pWidget);
        virtual void OnCellChanged();
        virtual void OnOptimize();
        virtual void OnScaleBlade();
        virtual void OnStartRotorSimulation();
        virtual void OnCreateRotorSimulation();
        virtual void OnStartTurbineSimulation();
        virtual void OnCreateTurbineSimulation();
		virtual void OnStartCharacteristicSimulation();
		virtual void OnCreateCharacteristicSimulation();
        virtual void UpdateRotorSimulation();
        virtual void UpdateBladeData();
        virtual void OnSelChangeBladeData(int i);
		virtual void OnSelChangeRotorSimulation();
		virtual void OnSelChangeTurbineSimulation();
        virtual void OnSelChangeHeightData(int i);
        virtual void OnSelChangeTurbineHeightData(int i);
        virtual void SetRotorGraphTitles(Graph* pGraph);
		virtual void SetCharGraphTitles(Graph* pGraph);
        virtual void SetPowerGraphTitles(Graph* pGraph);
        virtual void OnGraphSettings();
        virtual void OnNewTurbine();
        virtual void OnSaveTurbine();
        virtual void OnEditTurbine();
        virtual void OnDeleteTurbine();
        virtual void InitTurbineData(TData *pTData);
        virtual void UpdateUnits();
        virtual void UpdateTurbineBladeData();
        virtual void OnDeleteRotorSim();
		virtual void OnDeleteCharSim();
        virtual void OnDeleteTurbineSim();
        virtual void OnSelChangeTurbineBladeData(int i);
        virtual void OnHubChanged();
		virtual void CheckTurbineButtons();
		virtual void UpdateCharacteristicsSimulation();
		virtual void OnSelChangeCharSimulation();
        ///// end JW virtual methods //////

        void OnEditCur360Polar();
        void OnItemClicked(const QModelIndex &index);
        virtual void OnDeleteSection();
        void OnDelete360Polar();
        void OnRename360Polar();
        void OnDiscardBlade();
        void OnDiscardTurbine();
        void OnExportBladeGeometry();
        void OnHideWidgets();
        void OnSingleGraphs();
        void OnDecompose360Polar();
        void OnNew360Polar();
        void Compute360Polar();
        void ComputePolar();
        void ComputeDecomposition();
        void ComputeViterna360Polar(); // JW method
        void OnSave360Polar();
        void CombinePolars();
		void CreatePolarCurve();
        void CreateSinglePolarCurve(bool showPolar = true);
        void OnSetCharMainWind();
        void OnSetCharMainRot();
        void OnSetCharMainPitch();
        virtual void OnResize();

        void OnExportRotorToAeroDyn();
        void OnExportRotorToWT_Perf();  // new code NM
        void OnExportBladeTable();
        void OnExport360PolarNREL();
        void OnExportAll360PolarsNREL();
        void OnImportPolar();
        void OnImport360Polar();
        void OnImportBladeGeometry();

        void OnSetCharParamWind();
        void OnSetCharParamRot();
        void OnSetCharParamPitch();
        void OnSetCharParamNone();
        virtual void OnCenterScene();

		void SetPolarGraphTitles(Graph* pGraph);
		void SetWeibullGraphTitles(Graph* pGraph);// new JW method
        BData* GetBladeData(QString Lambda);
        BData* GetTurbineBladeData(QString Windspeed);
        void OnChangeCoordinates();
		virtual void OnSelChangeTurbine(int i);
        void InitTurbineSimulationParams(TBEMData *bladedata);
        void InitBladeSimulationParams(BEMData *bladedata);
        void InitCharSimulationParams(CBEMData *bladedata);
        void TabChanged();
		void OnAlignMaxThickness();
        virtual void OnBladeColor();
        virtual void OnSectionColor();
        void OnLightDlg();
        void OnSingleMultiPolarChanged();
        void OnPolarDialog();

protected slots:
		void onPerspectiveChanged ();
		

public slots:
	virtual void onModuleChanged ();  // NM will hide this module if no longer active

        void OnStallModel(); // JW method
        void OnSetAR(double val); // JW method
        // JW virtual methods //
        virtual void UpdateBlades();
        virtual void OnSelChangeWing(int i);
        virtual void OnBladeView();
        virtual void OnRotorsimView();
		virtual void OnCharView();
        virtual void OnTurbineView();
        virtual void FillComboBoxes(bool bEnable = true);
        virtual void SetCurveParams();
        virtual void OnShowAllRotorCurves();
        virtual void OnHideAllRotorCurves();
        virtual void OnBladeGraph();
        virtual void OnRotorGraph();
        virtual void OnAziGraph();
		virtual void OnSetWeibull();
		//virtual void OnSetWeibullA(double A);
		//virtual void OnSetWeibullK(double K);
        virtual void PaintCharLegend(QPoint place, int bottom, QPainter &painter);
        virtual void PaintPowerLegend(QPoint place, int bottom, QPainter &painter);
        virtual void UpdateCurve();
        virtual void OnShowPoints();
        virtual void OnShowCurve();
        virtual void OnShowOpPoint();
        virtual void InvertedClicked();
        virtual void OnLengthHeightChanged();
        virtual void OnHubValChanged();


		// end JW virtual methods //
        void Paint360Legend(QPoint place, int bottom, QPainter &painter);
		void OnWeibullGraph();// JW method
        void On360View();
        void OnLoadCylindricFoil();
        void OnIsolateCurrentBladeCurve();
        void OnCompareIsolatedBladeCurves();
        void OnSetCD90(double val);
        void OnSelChangeFoil(int i);
        void OnSelChangePolar(int i);
        void OnSelChange360Polar(int i);
        void OnSelChangeWind(int i);
        void OnSelChangeRot(int i);
        void OnSelChangePitch(int i);
        void PitchBlade();
        void OnCurveColor();
        void OnCurveStyle(int index);
        void OnCurveWidth(int index);
        void UpdateView();
        void UpdateGeom();
        void OnPrescribePitch();
		void OnPrescribeRot();
		void OnDiscard360Polar();
        void BladeCoordsChanged();

protected:

        virtual void SaveSettings(QSettings *pSettings); // JW virt. method
        virtual void LoadSettings(QSettings *pSettings); // JW virt. method
public:
        ///// JW virtual methods //////////
        virtual void DisableAllButtons();
        virtual void EnableAllButtons();
        virtual void SetupLayout();
        virtual void Connect();
        virtual void FillTableRow(int row);
        virtual bool InitDialog(CBlade *pWing);
        virtual void ReadSectionData(int sel);
        virtual void CreateRotorCurves();
        virtual void * GetRotorVariable(void *data, int iVar);
        virtual void * GetBladeVariable(void *data, int iVar);
        virtual void UpdateTurbineSimulation();
        virtual void CreatePowerCurves();
        virtual void * GetTurbineRotorVariable(void *Data, int iVar);
        virtual void * GetTurbineBladeVariable(void *Data, int iVar);
        virtual void FillRotorCurve(CCurve *pCurve, void *Data, int XVar, int YVar);
		virtual void FillPowerCurve(CCurve *pCurve, void *Data, int XVar, int YVar);
        virtual CBlade * GetWing(QString WingName);
        virtual void CheckWing();
		virtual void UpdateCharacteristicsMatrix();
		virtual void CreateCharacteristicsCurves();
		virtual void FillCharacteristicCurve(CCurve *pCurve, void *Data, int XVar, int YVar, int num_param);
        virtual float*** GetCharVariable(void *Data, int iVar);
		virtual void * GetWeibullXVariable(int iVar);// JW method
		virtual void * GetWeibullYVariable(void *Data, int iVar);// JW method
        virtual void GLCallViewLists();
		
		virtual void configureGL ();  // NM new functions equal to the interface that GLModule offers
		virtual void drawGL ();
		virtual void overpaint (QPainter &painter);


		///// end JW virtual methods //////////
		void FillWeibullCurve(CCurve *pCurve, void *Data, int XVar);//int count);// JW method
		virtual TData * GetTurbine(QString m_TurbineName);
        bool InitAdvancedDialog(CBlade *pWing);
        void FillDataTable();
        void FillAdvancedTableRow(int row);
        void SetCurrentSection(int section);
        void ReadParams(bool isVawt = false);
        void ReadAdvancedSectionData(int sel);
        void ComputeGeometry(bool isVawt = false);
        void ComputePolarVars();
        void GLCreateGeom(CBlade *pWing, int List);
        void GLRenderView();
        void GLDraw3D();
        void MouseReleaseEvent(QMouseEvent *event);
        void MousePressEvent(QMouseEvent *event);
        void MouseMoveEvent(QMouseEvent *event);
        void mouseDoubleClickEvent ( QMouseEvent * event );
        void WheelEvent(QWheelEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void GLCreateSectionHighlight();
        void SetScale();
        void SetLegendPos();
        void SetScale(QRect CltRect);
        void Set2DScale();
        void Paint360Graphs(QPainter &painter);
        virtual void PaintView(QPainter &painter);
        QGraph* GetGraph(QPoint &pt);
		void UpdateFoils();
		virtual void UpdateTurbines();
        CFoil* GetFoil(QString strFoilName);
        void UpdatePolars();
        CPolar* GetPolar(QString m_FoilName, QString PolarName);
        void* GetVariable(CPolar *pPolar, int iVar);
        void FillPolarCurve(CCurve *pCurve, CPolar *pPolar, int XVar, int YVar);
		void* GetVariable(C360Polar *pPolar, int iVar);
		void FillPolarCurve(CCurve *pCurve, C360Polar *pPolar, int XVar, int YVar);
        double PotFlow(double CLzero, double slope, double alpha);
        double PlateFlow(double alphazero, double CLzero,  double alpha);
        double CD90(CFoil *pFoil, double alpha);
        double CDPlate(double alpha);
        void Update360Polars();
		C360Polar* Get360Polar(QString m_FoilName,  QString PolarName);
        void PaintRotorGraphs(QPainter &painter);
        virtual void PaintCharacteristicsGraphs(QPainter &painter);
		void PaintPowerGraphs(QPainter &painter);
        void SnapClient(QString const &FileName);
        double FlatPlateCL(double alpha, int pos); //// JW method ////
        double FlatPlateCD(double alpha, int pos); //// JW method ////
		CBlade* GetCurrentBlade();



public:

        //pointers
		BEMToolbar *m_BEMToolBar;
        QRegExp any_number;

        void *s_pAFoil;
        void *s_pXDirect;

        BData *m_pBData;
        BData *m_pTurbineBData; // pointer to current turbine blade data
        BEMData *m_pBEMData; // pointer to current blade data
        TData *m_pTData; // pointer to current turbine
        TBEMData *m_pTBEMData; // pointer to current turbine simulations
        CBEMData *m_pCBEMData;  // pointer to current characteristic simulations

        CBlade *m_pBlade;// the  wing that is edited (a copy of the selected wing)

        CFoil *m_pCurFoil;
        CPolar *m_pCurPolar;
        C360Polar *m_pCur360Polar;
        int m_iSection;
        GLWidget *m_pGLWidget;
        TwoDWidget *m_p2DWidget;
        void *m_pSimuWidget;

        BData *m_pBladeData;


        QTabWidget *SimpleAdvanced;
        QPushButton *m_pctrlInsertBefore, *m_pctrlInsertAfter, *m_pctrlDeleteSection, *m_pctrlOptChord, *m_pctrlOptTwist, *m_pctrlBEM, *m_pctrlBEMSim, *m_pctrlBEMRun;
        QLineEdit *m_pctrlWingName, *m_pctrlBEMLambdaStart, *m_pctrlBEMLambdaEnd, *m_pctrlBEMLambdaDelta;
        QTableView *m_pctrlBladeTable, *m_pctrlBladeAxisTable;
        QWidget *m_pctrlControlsWidget, *m_pctrl360Widget;
        QStackedWidget *m_pctrBottomControls;
        QCheckBox *m_pctrlShowCurve, *m_pctrlShowPoints;

        QGroupBox *SliderGroup, *DecomposeGroup, *ViternaGroup, *RangeGroup;
        QLabel *IsDecomposed;


        QSpinBox *m_pctrlBlades;
        NumberEdit *m_pctrlFixedPitch, *m_pctrlHubRadius;
        QLabel *m_pctrlBladesLabel, *m_pctrlHubRadiusLabel, *m_pctrlSolidityLabel, *m_pctrlHubRadiusUnitLabel, *m_pctrlSingleMultiLabel, *m_pctrlWingNameLabel, *m_pctrlBladesAndHubLabel, *m_pctrlFixedPitchUnit, *m_pctrlFixedPitchLabel;
        QTableView *m_pctrlBladeTableView;
        QPushButton *m_pctrlNewWing, *m_pctrlDeleteWing, *m_pctrlEditWing, *m_pctrlSave, *m_pctrlOptimize, *m_pctrlBack, *m_pctrlScale;
        QPushButton *m_pctrlAlignMaxThickness, *m_pctrlLightDlg, *m_pctrlResetView;
        QStackedWidget *mainWidget, *bladeWidget;
		QStackedWidget *PowWidget;// JW variable
        QWidget *SimWidget, *EditWidget, *PolarWidget, *PowerWidget, *PowerEditWidget, *AdvancedEditWidget;

        QRadioButton *m_pctrlStall, *m_pctrlPitch, *m_pctrlFixed, *m_pctrl2Step, *m_pctrlVariable, *m_pctrlPrescribedPitch, *m_pctrlPrescribedRot;
        QLabel *m_pctrlTypeLabel;
        QLabel *m_pctrlGeneratorTypeLabel, *m_pctrlVariableLossesLabel, *m_pctrlVariableLossesUnit, *m_pctrlFixedLossesLabel;
        QLabel *m_pctrlRot1Label, *m_pctrlRot2Label, *m_pctrlGeneratorLabel, *m_pctrlCutInLabel, *m_pctrlCutOutLabel, *m_pctrlSwitchLabel, *m_pctrlLambdaLabel;
        NumberEdit  *m_pctrlRot1, *m_pctrlRot2, *m_pctrlGenerator, *m_pctrlCutIn, *m_pctrlCutOut , *m_pctrlSwitch, *m_pctrlLambda, *m_pctrlVariableLosses, *m_pctrlFixedLosses;
        QLabel *Type, *Trans, *Capacity, *Rot1, *Rot2, *Lambda0, *CutIn, *CutOut, *Switch, *Generator, *Blade, *BladeLabel;
        QLabel *TypeLabel, *GeneratorTypeLabel, *CutInLabel, *SwitchLabel, *CutOutLabel, *Rot1Label, *Rot2Label, *LambdaLabel, *GeneratorLabel;
        QLabel *FixedLosses, *VariableLosses, *FixedLossesLabel, *VariableLossesLabel, *OuterRadiusLabel, *OuterRadius, *FixedPitchLabel, *FixedPitch;
        QPushButton *m_pctrlNewTurbine, *m_pctrlDeleteTurbine, *m_pctrlEditTurbine, *m_pctrlSaveTurbine, *m_pctrlDiscardTurbine, *m_pctrlPitchList, *m_pctrlRotList;
        QComboBox *WingSelection;
        QLineEdit *m_pctrlTurbineName;
        QLabel *speed1, *speed2, *speed3, *rotspeed1, *rotspeed2, *power1, *power2, *Speed1, *Speed2, *Speed3, *Power1, *Power2, *Rotspeed1, *Rotspeed2, *Length1;
		QDoubleSpinBox *m_pctrlWk, *m_pctrlWA, *m_pctrlPitchBlade;
		QLabel *m_pctrlWkLabel, *m_pctrlWALabel, *m_pctrlYield, *m_pctrlYieldLabel, *POwer;
		QDoubleSpinBox *m_pctrlPMk, *m_pctrlPMA;// JW variable
		QLabel *m_pctrlPMkLabel, *m_pctrlPMALabel;// JW variable
        QPushButton *m_pctrlSave360, *m_pctrlNew360, *m_pctrlCancel360, *m_pctrlDelete360Polar, *m_pctrlDecompose360, *m_pctrlRename360Polar;
        QLabel *m_LabelA, *m_LabelB,*m_LabelAm, *m_LabelBm, *m_pctrlBEMLS, *m_pctrlBEMLE, *m_pctrlBEMLD, *m_pctrlCD90Label;
        QSlider *m_pctrlA, *m_pctrlB, *m_pctrlAm, *m_pctrlBm;
        QLineEdit *m_360Name;
        QDoubleSpinBox *m_pctrlCD90;
        QCheckBox *m_ComparePolars, *m_pctrlBladeCoordinates, *m_pctrlBladeCoordinates2;
        QStandardItemModel *m_pWingModel, *m_pBladeAxisModel;
        QDoubleSpinBox *m_posStall, *m_posSep, *m_negStall, *m_negSep, *m_pos180Stall, *m_pos180Sep, *m_neg180Stall, *m_neg180Sep;

        QPushButton *m_pctrlPerspective, *m_pctrlShowTurbine, *m_pctrlSurfaces, *m_pctrlAirfoils, *m_pctrlOutline, *m_pctrlOutlineEdge, *m_pctrlAxes, *m_pctrlPositions, *m_pctrlFoilNames;

        BladeDelegate *m_pBladeDelegate;
        BladeAxisDelegate *m_pBladeAxisDelegate;

        ColorButton *m_pctrlWingColor, *m_pctrlSectionColor;

        QButtonGroup *m_SingleMultiGroup;



///variables///////

        bool m_bRightSide;
        bool m_bResetglGeom;
        bool m_bCrossPoint;
        bool m_bShowLight;
        bool m_bOutline;
        bool m_bglLight;
        bool m_bSurfaces;
        bool m_bResetglLegend;
        bool m_bResetglSectionHighlight;
        bool m_bXPressed;
        bool m_bYPressed;
        bool m_bHideWidgets;
        bool m_bSingleGraphs;
        bool m_b360PolarChanged;
        bool m_bNew360Polar;
        bool m_bDecompose;
        bool m_WingEdited;
        bool m_TurbineEdited;
        bool m_bIsolateBladeCurve;
        bool m_bCompareBladeCurve;
        bool m_bIs2DScaleSet;
        bool m_bAutoScales;
        bool m_bAbsoluteBlade;
        bool m_bAdvancedEdit;

        bool m_bShowOpPoint;

        ///// new code JW /////////
        bool m_bStallModel;
        double m_AR;
        QRadioButton *m_pctrlStallModelVit, *m_pctrlStallModelMontg;
        QLabel *m_pctrlARLabel;
        QDoubleSpinBox *m_pctrlAR;
        QDoubleSpinBox *m_Slope, *m_posAoA, *m_negAoA;


		QList <double> m_k;//shape parameter
		QList <double> m_A;//scale parameter
        ///// end new code JW /////



		//int m_iView;// JW comment
        int selected_windspeed, selected_lambda;
        int selected_wind, selected_pitch, selected_rot;

        double m_GLScale;
        double m_ClipPlanePos;
        double m_glScaled;


        int m_OutlineStyle, m_OutlineWidth;
        QColor m_OutlineColor;

        QPoint m_LastPoint, m_PointDown;

        CVector m_glViewportTrans;

        GLLightDlg   m_GLLightDlg;

        QRect m_rCltRect;
        QRect m_rSingleRect;


        ////GRAPHS///////

        QGraph m_360Graph1, m_360Graph2, m_360Graph3, m_360Graph4, *m_pCurGraph;
        QGraph m_RotorGraph1, m_RotorGraph2, m_RotorGraph3;
        QGraph m_PowerGraph1, m_PowerGraph2, m_PowerGraph3;
        QGraph m_CharGraph1, m_CharGraph2, m_CharGraph3, m_CharGraph4;
        QPoint m_CharLegendOffset;
        QPoint m_PowerLegendOffset;
        QPoint m_360LegendOffset;
        QColor m_CurveColor;
        int m_CurveStyle, m_CurveWidth;
        double m_CD90;

        ///////////////variables for the dialog default values//////////

        double dlg_lambda;
        double dlg_epsilon;
        int dlg_iterations;
        int dlg_elements;
        double dlg_rho;
        bool dlg_tiploss;
        bool dlg_aspectratio;
        bool dlg_variable;
        bool dlg_rootloss;
        bool dlg_3dcorrection;
        bool dlg_interpolation;
        bool dlg_newtiploss;
        bool dlg_newrootloss;
        double dlg_relax;
        double dlg_lambdastart;
        double dlg_lambdaend;
        double dlg_lambdadelta;
        double dlg_windspeed;

        double dlg_windstart;
        double dlg_windend;
        double dlg_winddelta;
        double dlg_windstart2;
        double dlg_windend2;
        double dlg_winddelta2;
        double dlg_visc;
        double dlg_pitchstart;
        double dlg_pitchend;
        double dlg_pitchdelta;
        double dlg_rotstart;
        double dlg_rotend;
        double dlg_rotdelta;
        double dlg_reynolds;

        int m_widthfrac;

        double m_PitchOld;
        double pitch_new;

        QList <double> pitchwindspeeds;
        QList <double> pitchangles;
        QList <double> rotspeeds;
        QList <double> rotwindspeeds;



        LineCbBox *m_pctrlCurveStyle, *m_pctrlCurveWidth;
        LineButton *m_pctrlCurveColor;
        LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;


};


extern QBEM *g_qbem;  /**< global pointer to the QBEM module **/

#endif // BEM_H
