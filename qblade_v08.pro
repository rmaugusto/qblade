# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------
QT += opengl
QT += xml
QT += widgets
TEMPLATE = app
CONFIG += console
SOURCES += src/MainFrame.cpp \
    src/Main.cpp \
    src/Globals.cpp \
    src/TwoDWidget.cpp \
    src/GLWidget.cpp \
    src/Miarex/ArcBall.cpp \
    src/Miarex/BodyGridDlg.cpp \
    src/Miarex/BodyScaleDlg.cpp \
    src/Miarex/BodyTableDelegate.cpp \
    src/Miarex/BodyTransDlg.cpp \
    src/Miarex/CtrlTableDelegate.cpp \
    src/Miarex/GLCreateLists.cpp \
    src/Miarex/GL3dWingDlg.cpp \
    src/Miarex/GL3DScales.cpp \
    src/Miarex/GL3dBodyDlg.cpp \
    src/Miarex/GLLightDlg.cpp \
    src/Miarex/LLTAnalysisDlg.cpp \
    src/Miarex/LLTAnalysis.cpp \
    src/Miarex/ImportWingDlg.cpp \
    src/Miarex/InertiaDlg.cpp \
    src/Miarex/Miarex.cpp \
    src/Miarex/ManageBodiesDlg.cpp \
    src/Miarex/ManageUFOsDlg.cpp \
    src/Miarex/PlaneDlg.cpp \
    src/Miarex/PanelAnalysisDlg.cpp \
    src/Miarex/StabPolarDlg.cpp \
    src/Miarex/StabViewDlg.cpp \
    src/Miarex/UFOTableDelegate.cpp \
    src/Miarex/W3dPrefsDlg.cpp \
    src/Miarex/WAdvancedDlg.cpp \
    src/Miarex/WingDelegate.cpp \
    src/Miarex/WingScaleDlg.cpp \
    src/Miarex/WPolarDlg.cpp \
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
    src/Objects/Body.cpp \
    src/Objects/CVector.cpp \
    src/Objects/Foil.cpp \
    src/Objects/Frame.cpp \
    src/Objects/OpPoint.cpp \
    src/Objects/Panel.cpp \
    src/Objects/Plane.cpp \
    src/Objects/Polar.cpp \
    src/Objects/POpp.cpp \
    src/Objects/Quaternion.cpp \
    src/Objects/Sf.cpp \
    src/Objects/Spline.cpp \
    src/Objects/WPolar.cpp \
    src/Objects/WOpp.cpp \
    src/Misc/ProgressDlg.cpp \
    src/Misc/ModDlg.cpp \
    src/Misc/EditPlrDlg.cpp \
    src/Misc/PolarFilterDlg.cpp \
    src/Misc/TranslatorDlg.cpp \
    src/Misc/UnitsDlg.cpp \
    src/Misc/RenameDlg.cpp \
    src/Misc/LinePickerDlg.cpp \
    src/Misc/LineDelegate.cpp \
    src/Misc/LineCbBox.cpp \
    src/Misc/LineButton.cpp \
    src/Misc/FloatEditDelegate.cpp \
    src/Misc/DisplaySettingsDlg.cpp \
    src/Misc/ColorButton.cpp \
    src/Misc/AboutQ5.cpp \
    src/Misc/NewNameDlg.cpp \
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
    src/Objects/Wing.cpp \
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
    src/XGlobals.cpp \
    src/XMainFrame.cpp \
    src/XUnsteadyBEM/WindField.cpp \
    src/XWidgets.cpp \
    src/XUnsteadyBEM/WindFieldCreatorDock.cpp \
    src/XUnsteadyBEM/WindFieldModule.cpp \
    src/Module.cpp \
    src/XUnsteadyBEM/WindFieldToolBar.cpp \
    src/ScrolledDock.cpp \
    src/Store.cpp \
    src/XUnsteadyBEM/WindFieldMenu.cpp \
    src/QFem/taperedelem.cpp \
    src/QFem/structintegrator.cpp \
    src/QFem/structelem.cpp \
    src/QFem/polifitgsl.cpp \
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
    src/TwoDWidgetEventInterface.cpp \
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
    src/Noise/noisemodule.cpp \
    src/Noise/noisetoolbar.cpp \
    src/Noise/noisedock.cpp \
    src/Noise/noisesimulation.cpp \
    src/Noise/noisesimulationdialog.cpp \
    src/Noise/noisewarningdialog.cpp \
    src/Noise/noisecalculation.cpp \
    src/Noise/noiseoppoint.cpp \
    src/Noise/noiseparameter.cpp \
    src/Noise/newgraphlegend.cpp

HEADERS += src/MainFrame.h \
    src/Params.h \
    src/Globals.h \
    src/TwoDWidget.h \
    src/GLWidget.h \
    src/Miarex/Miarex.h \
    src/Miarex/W3dPrefsDlg.h \
    src/Miarex/GLCreateLists.h \
    src/Miarex/WingScaleDlg.h \
    src/Miarex/CtrlTableDelegate.h \
    src/Miarex/ManageBodiesDlg.h \
    src/Miarex/WPolarDlg.h \
    src/Miarex/WingDelegate.h \
    src/Miarex/PlaneDlg.h \
    src/Miarex/PanelAnalysisDlg.h \
    src/Miarex/ManageUFOsDlg.h \
    src/Miarex/UFOTableDelegate.h \
    src/Miarex/StabViewDlg.h \
    src/Miarex/InertiaDlg.h \
    src/Miarex/StabPolarDlg.h \
    src/Miarex/LLTAnalysisDlg.h \
    src/Miarex/LLTAnalysis.h \
    src/Miarex/ImportWingDlg.h \
    src/Miarex/GLLightDlg.h \
    src/Miarex/ArcBall.h \
    src/Miarex/BodyGridDlg.h \
    src/Miarex/BodyTableDelegate.h \
    src/Miarex/BodyScaleDlg.h \
    src/Miarex/WAdvancedDlg.h \
    src/Miarex/GL3dWingDlg.h \
    src/Miarex/BodyTransDlg.h \
    src/Miarex/GL3DScales.h \
    src/Miarex/GL3dBodyDlg.h \
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
    src/Objects/WPolar.h \
    src/Objects/WOpp.h \
    src/Objects/Wing.h \
    src/Objects/Surface.h \
    src/Objects/Spline.h \
    src/Objects/Sf.h \
    src/Objects/OpPoint.h \
    src/Objects/Quaternion.h \
    src/Objects/POpp.h \
    src/Objects/Polar.h \
    src/Objects/CVector.h \
    src/Objects/Plane.h \
    src/Objects/Panel.h \
    src/Objects/Frame.h \
    src/Objects/Foil.h \
    src/Objects/Body.h \
    src/Misc/ModDlg.h \
    src/Misc/PolarFilterDlg.h \
    src/Misc/TranslatorDlg.h \
    src/Misc/UnitsDlg.h \
    src/Misc/RenameDlg.h \
    src/Misc/LinePickerDlg.h \
    src/Misc/LineDelegate.h \
    src/Misc/FloatEditDelegate.h \
    src/Misc/DisplaySettingsDlg.h \
    src/Misc/ColorButton.h \
    src/Misc/LineCbBox.h \
    src/Misc/LineButton.h \
    src/Misc/EditPlrDlg.h \
    src/Misc/AboutQ5.h \
    src/Misc/ProgressDlg.h \
    src/Misc/NewNameDlg.h \
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
    src/XGlobals.h \
    src/XUnsteadyBEM/WindField.h \
    src/XWidgets.h \
    src/XUnsteadyBEM/WindFieldCreatorDock.h \
    src/XUnsteadyBEM/WindFieldModule.h \
    src/Module.h \
    src/XUnsteadyBEM/WindFieldToolBar.h \
    src/ScrolledDock.h \
    src/Store.h \
    src/XUnsteadyBEM/WindFieldMenu.h \
    src/QFem/taperedelem.h \
    src/QFem/structintegrator.h \
    src/QFem/structelem.h \
    src/QFem/polifitgsl.h \
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
    src/TwoDWidgetEventInterface.h \
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
    src/Noise/noisemodule.h \
    src/Noise/noisetoolbar.h \
    src/Noise/noisedock.h \
    src/Noise/noisesimulation.h \
    src/Noise/noisesimulationdialog.h \
    src/Noise/noisewarningdialog.h \
    src/Noise/noisecalculation.h \
    src/Noise/noiseoppoint.h \
    src/Noise/noiseparameter.h \
    src/Noise/newgraphlegend.h \
    src/Noise/noisedefinitions.h

TRANSLATIONS = translations/xflr5v6.ts \
    translations/xflr5v6_de.ts \
    translations/xflr5v6_fr.ts \
    translations/xflr5v6_ja.ts

RESOURCES += qblade.qrc

win32 { 
    TARGET = QBlade
    RC_FILE = win/qblade.rc
}

unix { 
    TARGET = QBlade
    
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    
    # MAKE INSTALL
    INSTALLS += target
#    target.path = $$BINDIR
}

macx { 
    CONFIG(release, debug|release) { 
        OBJECTS_DIR = ./build/release
        MOC_DIR = ./build/release
        RCC_DIR = ./build/release
        UI_HEADERS_DIR = ./build/release
    }
    CONFIG(debug, debug|release) { 
        OBJECTS_DIR = ./build/debug
        MOC_DIR = ./build/debug
        RCC_DIR = ./build/debug
        UI_HEADERS_DIR = ./build/debug
    }
    TARGET = QBlade
    TEMPLATE = app
    CONFIG += i386 \
        ppc
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
    OTHER_FILES += mac/Info.plist
    LIBS += -framework \
        CoreFoundation
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/qblade.icns
}
OTHER_FILES += doc/ReleaseNotes.txt

#QMAKE_CXXFLAGS += -pipe -static-libgcc -static-libstdc++ -ggdb
# this activates compiler support for openMP
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

# add the proper include path
win32: LIBS += -L$$PWD/libs_windows
else:linux-g++:contains(QMAKE_HOST.arch, x86   ): LIBS += -L$$PWD/libs_unix_32bit
else:linux-g++:contains(QMAKE_HOST.arch, x86_64): LIBS += -L$$PWD/libs_unix_64bit

# this includes QGLViewer on all platforms for debug or release
#LIBS += -L$$PWD/QGLViewer
win32:     CONFIG(release, debug|release): LIBS += -lQGLViewer2
else:win32:CONFIG(debug,   debug|release): LIBS += -lQGLViewerd2
else:linux-g++: LIBS += -lQGLViewer
INCLUDEPATH += $$PWD/QGLViewer
DEPENDPATH  += $$PWD/QGLViewer

# this includes gsl on all platforms
#LIBS += -L$$PWD/gsl/
win32: LIBS += -llibgsl
else:linux-g++: LIBS += -lgsl -lgslcblas
INCLUDEPATH += $$PWD/gsl
DEPENDPATH  += $$PWD/gsl

# include libGLU
linux-g++: LIBS += -lGLU

FORMS += \
    src/Noise/noisesimulationdialog.ui \
    src/Noise/noisewarningdialog.ui
