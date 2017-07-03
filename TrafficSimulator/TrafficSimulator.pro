#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T18:02:24
#
#-------------------------------------------------

QT       += core gui opengl xml widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrafficSimulator
TEMPLATE = app

SOURCES += main.cpp\
    ParticleSystemTraffic.cpp \
    Particle.cpp \
    HashTable.cpp \
    Kernel.cpp \
    MainViewer.cpp \
    MainWindow.cpp \
    Road.cpp \
    LocalMonitor.cpp \
    KernelMonitor.cpp \
    Cell.cpp \
    SystemParameter.cpp \
    Bezier.cpp \
    BezierSequence.cpp \
    TriangleMesh.cpp \
    CellSequence.cpp \
    HashTableX.cpp \
    CarParticle.cpp \
    MapParser.cpp \
    tinyxml2.cpp \
    Trajectory.cpp \
    ParticleSystemTraffic1D.cpp \
    ParticleSystemTrafficTrj.cpp \
    MinViewer.cpp \
    roadproperty.cpp \
    particleproperty.cpp \
    carproperty.cpp \
    ParticleSystemTrafficEXP.cpp

HEADERS  += \
    ParticleSystemTraffic.h \
    Particle.h \
    HashTable.h \
    global.h \
    Road.h \
    Kernel.h \
    MainViewer.h \
    MainWindow.h \
    LocalMonitor.h \
    KernelMonitor.h \
    Cell.h \
    SystemParameter.h \
    randomer.h \
    Bezier.h \
    BezierSequence.h \
    TriangleMesh.h \
    CellSequence.h \
    HashTableX.h \
    CarParticle.h \
    MapParser.h \
    tinyxml2.h \
    Trajectory.h \
    ParticleSystemTraffic1D.h \
    ParticleSystemTrafficTrj.h \
    MinViewer.h \
    roadproperty.h \
    particleproperty.h \
    carproperty.h \
    ParticleSystemTrafficEXP.h

FORMS    += mainwindow.ui \
    roadproperty.ui \
    particleproperty.ui \
    carproperty.ui

QMAKE_LFLAGS += /STACK:32000000

INCLUDEPATH += F:/MYLIBs/libQGLViewer-2.6.3
INCLUDEPATH += F:/MYLIBs/freetype-2.7/include

LIBS += -lopengl32 -lglu32

CONFIG(debug, debug|release){
    LIBS += -LF:/MYLIBs/libQGLViewer-2.6.3/QGLViewer -lQGLViewerd2
    LIBS += -LF:/MYLIBs/freetype-2.7/objs/vc2010/x64 -lfreetype27d
} else {
    LIBS += -LF:/MYLIBs/libQGLViewer-2.6.3/QGLViewer -lQGLViewer2
    LIBS += -LF:/MYLIBs/freetype-2.7/objs/vc2010/x64 -lfreetype27
}
