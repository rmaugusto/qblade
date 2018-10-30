# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------

# switch here to build either for 32bit or 64bit
#CONFIG += build32bit
CONFIG += build64bit

# specify which Qt modules are needed.
QT += core gui widgets opengl xml

# set the name of the executable
TARGET = QBlade

# The template to use for the project. This determines that the output will be an application.
TEMPLATE = app

# include the resources file into the binary
RESOURCES += qblade.qrc

# set the icon of the executable application file
win32:RC_FILE = win/qblade.rc

# from gcc 4.8.1 on the c++11 implementation is feature complete
QMAKE_CXXFLAGS += -std=gnu++11  # usually it would be fine to give c++11 but the OpenCL part apparently needs gnu extensions

# activate compiler support for openMP
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

# add the proper include path for libraries
     win32:CONFIG(build32bit): LIBS += -L$$PWD/libs_windows_32bit
else:win32:CONFIG(build64bit): LIBS += -L$$PWD/libs_windows_64bit
else:unix: CONFIG(build32bit): LIBS += -L$$PWD/libs_unix_32bit
else:unix: CONFIG(build64bit): LIBS += -L$$PWD/libs_unix_64bit

# includes QGLViewer
INCLUDEPATH += QGLViewer_headers
     win32:CONFIG(release, debug|release): LIBS += -lQGLViewer2
else:win32:CONFIG(debug,   debug|release): LIBS += -lQGLViewerd2
else:unix: LIBS += -lQGLViewer

# include openGL & glu
     win32: LIBS += -lopengl32 -lglu32
else:unix:  LIBS += -lGLU


SOURCES += src/MainFrame.cpp \
    src/Main.cpp \
    src/Globals.cpp \
    src/TwoDWidget.cpp \
    src/GLWidget.cpp \
    src/Misc/GLLightDlg.cpp \
    src/XDirect/BatchDlg.cpp \
    src/XDirect/BatchThreadDlg.cpp \
    src/XDirect/XFoilTask.cpp \
    src/XDirect/CAddDlg.cpp \
    src/XDirect/FoilCoordDlg.cpp \
    src/XDirect/FoilGeomDlg.cpp \
    src/XDirect/FoilPolarDlg.cpp \
    src/XDirect/FlapDlg.cpp \
    src/XDirect/InterpolateFoilsDlg.cpp \
    src/XDirect/LEDlg.cpp \
    src/XDirect/ManageFoilsDlg.cpp \
    src/XDirect/NacaFoilDlg.cpp \
    src/XDirect/ReListDlg.cpp \
    src/XDirect/TEGapDlg.cpp \
    src/XDirect/TwoDPanelDlg.cpp \
    src/XDirect/XDirectStyleDlg.cpp \
    src/XDirect/XFoil.cpp \
    src/XDirect/XFoilAnalysisDlg.cpp \
    src/XDirect/XFoilAdvancedDlg.cpp \
    src/XDirect/XDirect.cpp \
    src/Objects/CVector.cpp \
    src/Objects/Foil.cpp \
    src/Objects/OpPoint.cpp \
    src/Objects/Panel.cpp \
    src/Objects/Polar.cpp \
    src/Objects/Quaternion.cpp \
    src/Objects/Sf.cpp \
    src/Objects/Spline.cpp \
    src/Misc/EditPlrDlg.cpp \
    src/Misc/PolarFilterDlg.cpp \
    src/Misc/TranslatorDlg.cpp \
    src/Misc/UnitsDlg.cpp \
    src/Misc/LinePickerDlg.cpp \
    src/Misc/LineDelegate.cpp \
    src/Misc/LineCbBox.cpp \
    src/Misc/LineButton.cpp \
    src/Misc/FloatEditDelegate.cpp \
    src/Misc/DisplaySettingsDlg.cpp \
    src/Misc/ColorButton.cpp \
    src/Misc/AboutQ5.cpp \
    src/Misc/ObjectPropsDlg.cpp \
    src/Graph/QGraph.cpp \
    src/Graph/GraphWidget.cpp \
    src/Graph/GraphDlg.cpp \
    src/Graph/Graph.cpp \
    src/Graph/Curve.cpp \
    src/XInverse/FoilSelectionDlg.cpp \
    src/XInverse/PertDlg.cpp \
    src/XInverse/XInverse.cpp \
    src/XInverse/InverseOptionsDlg.cpp \
    src/Design/FoilTableDelegate.cpp \
    src/Design/AFoilGridDlg.cpp \
    src/Design/LECircleDlg.cpp \
    src/Design/AFoil.cpp \
    src/Design/SplineCtrlsDlg.cpp \
    src/Design/AFoilTableDlg.cpp \
    src/XBEM/TData.cpp \
    src/XBEM/TBEMData.cpp \
    src/XBEM/SimuWidget.cpp \
    src/XBEM/OptimizeDlg.cpp \
    src/XBEM/Edit360PolarDlg.cpp \
    src/XBEM/CreateBEMDlg.cpp \
    src/XBEM/BladeScaleDlg.cpp \
    src/XBEM/BEMData.cpp \
    src/XBEM/BEM.cpp \
    src/XBEM/BData.cpp \
    src/XBEM/AboutBEM.cpp \
    src/XBEM/Blade.cpp \
    src/Objects/Surface.cpp \
    src/XBEM/BladeDelegate.cpp \
    src/XBEM/BladeAxisDelegate.cpp \
    src/XBEM/CBEMData.cpp \
    src/XBEM/PrescribedValuesDlg.cpp \
    src/XBEM/CircularFoilDlg.cpp \
    src/XBEM/BEMSimDock.cpp \
    src/XBEM/BEMDock.cpp \
    src/XDMS/DMS.cpp \
    src/XDMS/SimuWidgetDMS.cpp \
    src/XDMS/BladeDelegate2.cpp \
    src/XDMS/OptimizeDlgVAWT.cpp \
    src/XDMS/BladeScaleDlgVAWT.cpp \
    src/XDMS/CreateDMSDlg.cpp \
    src/XDMS/DMSData.cpp \
    src/XDMS/DData.cpp \
    src/XDMS/TDMSData.cpp \
    src/XDMS/DMSSimDock.cpp \
    src/XDMS/DMSDock.cpp \
    src/XDMS/CDMSData.cpp \
    src/XUnsteadyBEM/WindField.cpp \
    src/XWidgets.cpp \
    src/XUnsteadyBEM/WindFieldModule.cpp \
    src/Module.cpp \
    src/XUnsteadyBEM/WindFieldToolBar.cpp \
    src/ScrolledDock.cpp \
    src/Store.cpp \
    src/XUnsteadyBEM/WindFieldMenu.cpp \
    src/QFem/taperedelem.cpp \
    src/QFem/structintegrator.cpp \
    src/QFem/structelem.cpp \
    src/QFem/node.cpp \
    src/QFem/eqnmotion.cpp \
    Clipper/clipper.cpp \
    src/QFem/unitvector.cpp \
    src/QFem/mode.cpp \
    src/QFem/deformationvector.cpp \
    src/QFem/QFEMDock.cpp \
    src/QFem/QFEMToolBar.cpp \
    src/QFem/QFEMModule.cpp \
    src/QFem/QFEMMenu.cpp \
    src/QFem/StructDelegate.cpp \
    src/QFem/BladeStructure.cpp \
    src/StorableObject.cpp \
    src/XUnsteadyBEM/FASTModule.cpp \
    src/XUnsteadyBEM/FASTDock.cpp \
    src/XUnsteadyBEM/FASTSimulation.cpp \
    src/XUnsteadyBEM/FASTSimulationCreatorDialog.cpp \
    src/XUnsteadyBEM/FASTSimulationToolBar.cpp \
    src/XUnsteadyBEM/FASTSimulationProgressDialog.cpp \
    src/XBEM/BEMToolbar.cpp \
    src/XDMS/DMSToolbar.cpp \
    src/XBEM/360Polar.cpp \
    src/Misc/NumberEdit.cpp \
    src/Serializer.cpp \
    src/StoreAssociatedComboBox.cpp \
    src/GlobalFunctions.cpp \
    src/Misc/SignalBlockerInterface.cpp \
    src/XUnsteadyBEM/FASTResult.cpp \
    src/XUnsteadyBEM/FastTwoDContextMenu.cpp \
    src/Graph/NewGraph.cpp \
    src/Graph/Axis.cpp \
    src/Graph/NewCurve.cpp \
    src/Graph/GraphOptionsDialog.cpp \
    src/Graph/ShowAsGraphInterface.cpp \
    src/QFem/QFEMTwoDContextMenu.cpp \
    src/QFem/forcingterm.cpp \
    src/QFem/staticequation.cpp \
    src/QFem/BladeStructureLoading.cpp \
    src/XBEM/ExportGeomDlg.cpp \
    src/TwoDContextMenu.cpp \
    src/XUnsteadyBEM/AboutFAST.cpp \
    src/XUnsteadyBEM/FASTMenu.cpp \
    src/Misc/FixedSizeLabel.cpp \
    src/XLLT/QLLTModule.cpp \
    src/XLLT/QLLTDock.cpp \
    src/XLLT/QLLTToolBar.cpp \
    src/QBladeApplication.cpp \
    src/XLLT/QLLTSimulation.cpp \
    src/XLLT/VortexNode.cpp \
    src/XLLT/VortexLine.cpp \
    src/XLLT/QLLTCreatorDialog.cpp \
    src/XLLT/DummyLine.cpp \
    src/XLLT/QLLTTwoDContextMenu.cpp \
    src/XLLT/QLLTCutPlane.cpp \
    src/XBEM/PolarSelectionDialog.cpp \
    src/XDMS/TurDmsModule.cpp \
    src/XDMS/TurDmsTurbineDock.cpp \
    src/XDMS/TurDmsSimulationDock.cpp \
    src/XDMS/TurDmsContextMenu.cpp \
    src/XDMS/TurDmsToolBar.cpp \
    src/SimulationDock.cpp \
    src/CreatorDock.cpp \
    src/XDMS/TurDmsTurbineCreatorDialog.cpp \
    src/SimulationModule.cpp \
    src/SimulationToolBar.cpp \
    src/XDMS/TurDmsSimulationCreatorDialog.cpp \
    src/ColorManager.cpp \
    src/Misc/LineStyleButton.cpp \
    src/Misc/LineStyleDialog.cpp \
    src/Misc/LineStyleComboBox.cpp \
    src/Misc/NewColorButton.cpp \
    src/TwoDGraphMenu.cpp \
    src/XUnsteadyBEM/WindFieldCreatorDialog.cpp \
    src/ParameterViewer.cpp \
    src/ParameterObject.cpp \
    src/ParameterGrid.cpp \
    src/TwoDWidgetInterface.cpp \
    src/XUnsteadyBEM/WindFieldDock.cpp \
    src/CreatorDialog.cpp \
    src/XDMS/MultiDmsModule.cpp \
    src/XDMS/MultiDmsContextMenu.cpp \
    src/XDMS/MultiDmsToolBar.cpp \
    src/XDMS/MultiDmsSimulationDock.cpp \
    src/XDMS/MultiDmsSimulationCreatorDialog.cpp \
    src/SimulationCreatorDialog.cpp \
    src/Noise/NoiseModule.cpp \
    src/Noise/NoiseSimulation.cpp \
    src/Noise/NoiseToolBar.cpp \
    src/Noise/NoiseDock.cpp \
    src/CreatorTwoDDock.cpp \
    src/Noise/NoiseCreatorDialog.cpp \
    src/Noise/NoiseOpPoint.cpp \
    src/Noise/NoiseCalculation.cpp \
    src/Noise/NoiseParameter.cpp \
    src/Noise/NoiseException.cpp \
    src/Noise/NoiseContextMenu.cpp \
    src/Noise/NoiseMenu.cpp \
    src/XDMS/RotDmsModule.cpp \
    src/XDMS/RotDmsToolBar.cpp \
    src/XDMS/RotDmsSimulationDock.cpp \
	src/XDMS/RotDmsContextMenu.cpp \
	src/XDMS/Strut.cpp \
	src/XDMS/RotDmsSimulationCreatorDialog.cpp \
    src/Objects/CVectorf.cpp

HEADERS += src/MainFrame.h \
    src/Params.h \
    src/Globals.h \
    src/TwoDWidget.h \
    src/GLWidget.h \
    src/Misc/GLLightDlg.h \
    src/XDirect/XFoil.h \
    src/XDirect/XFoilAnalysisDlg.h \
    src/XDirect/XFoilAdvancedDlg.h \
    src/XDirect/XDirect.h \
    src/XDirect/TwoDPanelDlg.h \
    src/XDirect/TEGapDlg.h \
    src/XDirect/InterpolateFoilsDlg.h \
    src/XDirect/FoilGeomDlg.h \
    src/XDirect/FoilCoordDlg.h \
    src/XDirect/ReListDlg.h \
    src/XDirect/XDirectStyleDlg.h \
    src/XDirect/ManageFoilsDlg.h \
    src/XDirect/NacaFoilDlg.h \
    src/XDirect/LEDlg.h \
    src/XDirect/FoilPolarDlg.h \
    src/XDirect/FlapDlg.h \
    src/XDirect/CAddDlg.h \
    src/XDirect/BatchDlg.h \
    src/XDirect/BatchThreadDlg.h \
    src/XDirect/XFoilTask.h \
    src/XInverse/XInverse.h \
    src/XInverse/InverseOptionsDlg.h \
    src/XInverse/FoilSelectionDlg.h \
    src/XInverse/PertDlg.h \
    src/Objects/Surface.h \
    src/Objects/Spline.h \
    src/Objects/Sf.h \
    src/Objects/OpPoint.h \
    src/Objects/Quaternion.h \
    src/Objects/Polar.h \
    src/Objects/CVector.h \
    src/Objects/Panel.h \
    src/Objects/Foil.h \
    src/Misc/PolarFilterDlg.h \
    src/Misc/TranslatorDlg.h \
    src/Misc/UnitsDlg.h \
    src/Misc/LinePickerDlg.h \
    src/Misc/LineDelegate.h \
    src/Misc/FloatEditDelegate.h \
    src/Misc/DisplaySettingsDlg.h \
    src/Misc/ColorButton.h \
    src/Misc/LineCbBox.h \
    src/Misc/LineButton.h \
    src/Misc/EditPlrDlg.h \
    src/Misc/AboutQ5.h \
    src/Misc/ObjectPropsDlg.h \
    src/Graph/GraphWidget.h \
    src/Graph/Graph.h \
    src/Graph/GraphDlg.h \
    src/Graph/Curve.h \
    src/Graph/QGraph.h \
    src/Design/AFoil.h \
    src/Design/AFoilGridDlg.h \
    src/Design/LECircleDlg.h \
    src/Design/SplineCtrlsDlg.h \
    src/Design/FoilTableDelegate.h \
    src/Design/AFoilTableDlg.h \
    src/XBEM/TData.h \
    src/XBEM/TBEMData.h \
    src/XBEM/SimuWidget.h \
    src/XBEM/OptimizeDlg.h \
    src/XBEM/Edit360PolarDlg.h \
    src/XBEM/CreateBEMDlg.h \
    src/XBEM/BladeScaleDlg.h \
    src/XBEM/BEMData.h \
    src/XBEM/BEM.h \
    src/XBEM/BData.h \
    src/XBEM/AboutBEM.h \
    src/XBEM/Blade.h \
    src/XBEM/BladeDelegate.h \
    src/XBEM/BladeAxisDelegate.h \
    src/XBEM/CBEMData.h \
    src/XBEM/PrescribedValuesDlg.h \
    src/XBEM/CircularFoilDlg.h \
    src/XBEM/BEMSimDock.h \
    src/XBEM/BEMDock.h \
    src/XDMS/DMS.h \
    src/XDMS/SimuWidgetDMS.h \
    src/XDMS/BladeDelegate2.h \
    src/XDMS/OptimizeDlgVAWT.h \
    src/XDMS/BladeScaleDlgVAWT.h \
    src/XDMS/CreateDMSDlg.h \
    src/XDMS/DMSData.h \
    src/XDMS/DData.h \
    src/XDMS/TDMSData.h \
    src/XDMS/DMSSimDock.h \
    src/XDMS/DMSDock.h \
    src/XDMS/CDMSData.h \
    src/XUnsteadyBEM/WindField.h \
    src/XWidgets.h \
    src/XUnsteadyBEM/WindFieldModule.h \
    src/Module.h \
    src/XUnsteadyBEM/WindFieldToolBar.h \
    src/ScrolledDock.h \
    src/Store.h \
    src/XUnsteadyBEM/WindFieldMenu.h \
    src/QFem/taperedelem.h \
    src/QFem/structintegrator.h \
    src/QFem/structelem.h \
    src/QFem/node.h \
    src/QFem/eqnmotion.h \
    Clipper/clipper.hpp \
    src/QFem/unitvector.h \
    src/QFem/mode.h \
    src/QFem/deformationvector.h \
    src/QFem/QFEMDock.h \
    src/QFem/QFEMToolBar.h \
    src/QFem/QFEMModule.h \
    src/QFem/QFEMMenu.h \
    src/QFem/BladeStructure.h \
    src/QFem/StructDelegate.h \
    src/StorableObject.h \
    src/XUnsteadyBEM/FASTModule.h \
    src/XUnsteadyBEM/FASTDock.h \
    src/XUnsteadyBEM/FASTSimulation.h \
    src/XUnsteadyBEM/FASTSimulationCreatorDialog.h \
    src/XUnsteadyBEM/FASTSimulationToolBar.h \
    src/XUnsteadyBEM/FASTSimulationProgressDialog.h \
    src/XBEM/BEMToolbar.h \
    src/XDMS/DMSToolbar.h \
    src/XBEM/360Polar.h \
    src/Misc/NumberEdit.h \
    src/Serializer.h \
    src/StoreAssociatedComboBox.h \
    src/StoreAssociatedComboBox_include.h \
    src/Store_include.h \
    src/StorableObject_heirs.h \
    src/GlobalFunctions.h \
    src/Misc/SignalBlockerInterface.h \
    src/XUnsteadyBEM/FASTResult.h \
    src/XUnsteadyBEM/FastTwoDContextMenu.h \
    src/Graph/NewGraph.h \
    src/Graph/Axis.h \
    src/Graph/NewCurve.h \
    src/Graph/GraphOptionsDialog.h \
    src/Graph/ShowAsGraphInterface.h \
    src/QFem/QFEMTwoDContextMenu.h \
    src/QFem/forcingterm.h \
    src/QFem/staticequation.h \
    src/QFem/BladeStructureLoading.h \
    src/XBEM/ExportGeomDlg.h \
    src/TwoDContextMenu.h \
    src/XUnsteadyBEM/AboutFAST.h \
    src/XUnsteadyBEM/FASTMenu.h \
    src/Misc/FixedSizeLabel.h \
    src/XLLT/QLLTModule.h \
    src/XLLT/QLLTDock.h \
    src/XLLT/QLLTToolBar.h \
    src/QBladeApplication.h \
    src/XLLT/QLLTSimulation.h \
    src/XLLT/VortexNode.h \
    src/XLLT/VortexLine.h \
    src/XLLT/QLLTCreatorDialog.h \
    src/XLLT/DummyLine.h \
    src/XLLT/QLLTTwoDContextMenu.h \
    src/XLLT/QLLTCutPlane.h \
    src/XBEM/PolarSelectionDialog.h \
    src/XDMS/TurDmsModule.h \
    src/XDMS/TurDmsTurbineDock.h \
    src/XDMS/TurDmsSimulationDock.h \
    src/XDMS/TurDmsContextMenu.h \
    src/XDMS/TurDmsToolBar.h \
    src/SimulationDock.h \
    src/CreatorDock.h \
    src/XDMS/TurDmsTurbineCreatorDialog.h \
    src/SimulationModule.h \
    src/SimulationToolBar.h \
    src/XDMS/TurDmsSimulationCreatorDialog.h \
    src/ColorManager.h \
    src/Misc/LineStyleButton.h \
    src/Misc/LineStyleDialog.h \
    src/Misc/LineStyleComboBox.h \
    src/Misc/NewColorButton.h \
    src/TwoDGraphMenu.h \
    src/XUnsteadyBEM/WindFieldCreatorDialog.h \
    src/ParameterViewer.h \
    src/ParameterObject.h \
    src/ParameterGrid.h \
    src/ParameterKeys.h \
    src/TwoDWidgetInterface.h \
    src/XUnsteadyBEM/WindFieldDock.h \
    src/CreatorDialog.h \
    src/XDMS/MultiDmsModule.h \
    src/XDMS/MultiDmsContextMenu.h \
    src/XDMS/MultiDmsToolBar.h \
    src/XDMS/MultiDmsSimulationDock.h \
    src/XDMS/MultiDmsSimulationCreatorDialog.h \
    src/SimulationCreatorDialog.h \
    src/Noise/NoiseModule.h \
    src/Noise/NoiseSimulation.h \
    src/Noise/NoiseToolBar.h \
    src/Noise/NoiseDock.h \
    src/CreatorTwoDDock.h \
    src/Noise/NoiseCreatorDialog.h \
    src/Noise/NoiseOpPoint.h \
    src/Noise/NoiseCalculation.h \
    src/Noise/NoiseParameter.h \
    src/Noise/NoiseException.h \
    src/Noise/NoiseContextMenu.h \
    src/Noise/NoiseMenu.h \
    src/XDMS/RotDmsModule.h \
    src/XDMS/RotDmsToolBar.h \
    src/XDMS/RotDmsSimulationDock.h \
	src/XDMS/RotDmsContextMenu.h \
	src/XDMS/Strut.h \
	src/XDMS/RotDmsSimulationCreatorDialog.h \
    src/Objects/CVectorf.h
