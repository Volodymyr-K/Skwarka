QT += qml quick
TARGET = gui
!no_desktop: QT += widgets

include(src/src.pri)

DEFINES += NOMINMAX

OTHER_FILES += \
    qml/main.qml

RESOURCES += \
    resources.qrc

CONFIG(release, debug|release): LIBS += -L$$PWD/../../Binaries/Release/ -lMath
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Binaries/Release/ -lCADImport
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Binaries/Release/ -lRaytracer
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Binaries/Release/ -lShapes
CONFIG(release, debug|release): LIBS += -L$$PWD/../../ThirdParty/TBB/4.2/lib/intel64/vc12/ -ltbb
CONFIG(release, debug|release): LIBS += -L$$PWD/../../ThirdParty/boost/libs/1.56 -llibboost_iostreams-vc120-mt-1_56
CONFIG(release, debug|release): LIBS += -L$$PWD/../../ThirdParty/FreeImage/3.16/Dist -lFreeImage

CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Binaries/Debug/ -lMathd
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Binaries/Debug/ -lCADImportd
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Binaries/Debug/ -lRaytracerd
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Binaries/Debug/ -lShapesd
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ThirdParty/TBB/4.2/lib/intel64/vc12/ -ltbb_debug
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ThirdParty/boost/libs/1.56 -llibboost_iostreams-vc120-mt-gd-1_56
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ThirdParty/FreeImage/3.16/Dist -lFreeImaged


INCLUDEPATH += $$PWD/../RayLibs $$PWD/../../ThirdParty/boost/1.56 $$PWD/../../ThirdParty/TBB/4.2/include $$PWD/../../ThirdParty/FreeImage/3.16/Dist
DEPENDPATH += $$PWD/../RayLibs $$PWD/../../ThirdParty/boost/1.56 $$PWD/../../ThirdParty/TBB/4.2/include $$PWD/../../ThirdParty/FreeImage/3.16/Dist


CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Release/Math.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Release/CADImport.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Release/Raytracer.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Release/Shapes.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/TBB/4.2/lib/intel64/vc12/tbb.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/boost/libs/1.56/libboost_iostreams-vc120-mt-1_56.lib
CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/FreeImage/3.16/Dist/FreeImage.lib

CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Debug/Mathd.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Debug/CADImportd.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Debug/Raytracerd.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Binaries/Debug/Shapesd.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/TBB/4.2/lib/intel64/vc12/tbb_debug.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/boost/libs/1.56/libboost_iostreams-vc120-mt-gd-1_56.lib
CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../ThirdParty/FreeImage/3.16/Dist/FreeImaged.lib

DISTFILES += \
    qml/tabs/CameraTab.qml \
    qml/tabs/RendererTab.qml \
    qml/ToolBarSeparator.qml \
    qml/forms/PhotonMapForm.qml \
    qml/forms/DirectLightForm.qml \
    qml/forms/PerspectiveCameraForm.qml
