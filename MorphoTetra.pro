# -------------------------------------------------
# Project created by QtCreator 2010-01-27T15:21:45
# -------------------------------------------------
QT += opengl widgets core gui opengl xml openglwidgets

VCPKG_CGAL_P = C:\dev\vcpkg\installed\x64-windows
QGLViewer_P  = C:\Users\reind\Desktop\HAI911I_Developpement_Application_interactive\libQGLViewer-2.8.0_faraj

TARGET = mophoTetra
TEMPLATE = app
MOC_DIR = ./moc
OBJECTS_DIR = ./obj


SOURCES += Main.cpp \
    Window.cpp \
    MeshViewer.cpp \
    DisplayDockWidget.cpp

HEADERS += Window.h \
    cgal_headers.h \
    MeshViewer.h \
    DisplayDockWidget.h


#LIBQGLViewer integration
INCLUDEPATH += $$QGLViewer_P
INCLUDEPATH += $$QGLViewer_P\QGLViewer

LIBS += -L$$QGLViewer_P\QGLViewer -lQGLViewer2


#CGAL integration
INCLUDEPATH += $$VCPKG_CGAL_P\include
INCLUDEPATH += $$VCPKG_CGAL_P\bin

#LIBS += -L$$VCPKG_CGAL_P\lib
#LIBS += -L$$VCPKG_CGAL_P\debug\lib
#LIBS += -L$$VCPKG_CGAL_P\debug\lib -lgmp
#LIBS += -L$$VCPKG_CGAL_P\debug\lib -lgmpxx

LIBS += -L$$VCPKG_CGAL_P\lib -lgmp
LIBS += -L$$VCPKG_CGAL_P\lib -lgmpxx



#LIBQGLViewer integration
win32 {
        # Seems to be needed for Visual Studio with Intel compiler
        DEFINES *= WIN32

        # Use native OpenGL drivers with Qt5.5
        # No longer implicit since the ANGLE driver is now an alternative
        LIBS += -lopengl32 -lglu32

        isEmpty( QGLVIEWER_STATIC ) {
                CONFIG(debug, debug|release) {
                        LIBS *= -L$${LIB_DIR} -lQGLViewerd2
                } else {
                        LIBS *= -L$${LIB_DIR} -lQGLViewer2
                }
        } else {
                DEFINES *= QGLVIEWER_STATIC
                CONFIG(debug, debug|release) {
                        LIBS *= $${LIB_DIR}/libQGLViewerd2.a
                } else {
                        LIBS *= $${LIB_DIR}/libQGLViewer2.a
                }
        }
}


QMAKE_CXXFLAGS = \#-frounding-math \
    -g \
    -DNDEBUG \
    -fno-strict-aliasing \
    -DFMLS_NOGPU \
    -DCGAL_INTRUSIVE_LIST \
    -frounding-math \
    -O3

QMAKE_CXXFLAGS_RELEASE = -std=c++14
