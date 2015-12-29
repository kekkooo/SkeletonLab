#-------------------------------------------------
#
# Project created by QtCreator 2014-06-16T18:03:37
#
#-------------------------------------------------

QT       += core gui opengl xml widgets
CONFIG   += qt core opengl thread debug app_bundle c++11

#QMAKE_CFLAGS_PPC_64     -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
#QMAKE_OBJECTIVE_CFLAGS_PPC_64  -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
#QMAKE_CFLAGS_X86_64     -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
#QMAKE_OBJECTIVE_CFLAGS_X86_64  -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
#QMAKE_CXXFLAGS_PPC_64   -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
#QMAKE_CXXFLAGS_X86_64   -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
#QMAKE_LFLAGS_PPC_64     -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
#QMAKE_LFLAGS_X86_64     -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5

QMAKE_CXXFLAGS = -mmacosx-version-min=10.9
#QMAKE_CXXFLAGS = -std=c++11
QMAKE_CXXFLAGS = -stdlib=libc++
QMAKE_CFLAGS = -mmacosx-version-min=10.9

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9


DEFINES += UNIX
DEFINES += OSX
DEFINES += use_boost        # <- comment if you do not have Boost installed
DEFINES += use_cgal         # <- comment if you do not have CGAL installed
DEFINES += use_gurobi       # <- comment if you do not have GUROBI installed
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = skeleton_editing
TEMPLATE = app

#INCLUDEPATH += /Users/francescousai/Documents/Sviluppo/Libs/eigen/
#INCLUDEPATH += /Users/francescousai/Documents/Sviluppo/Libs/gsl-lite/include/
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /Users/francescousai/Documents/Sviluppo/Libs/libQGLViewer-2.6.3
INCLUDEPATH += /Users/francescousai/Documents/Sviluppo/Libs/boost_1_59_0/BUILD109/include
INCLUDEPATH += /Users/francescousai/Documents/Sviluppo/Libs/cgal-releases-CGAL-4.7/BUILD109/include
#use_gurobi{
INCLUDEPATH += /Library/gurobi650/mac64/include
#}
LIBS    += -L/Users/francescousai/Documents/Sviluppo/Libs/libQGLViewer-2.6.3/QGLViewer/ -lQGLViewer
LIBS    += -L/Users/francescousai/Documents/Sviluppo/Libs/boost_1_59_0/BUILD109/lib -lboost_system-mt-s
LIBS    += -L/Users/francescousai/Documents/Sviluppo/Libs/boost_1_59_0/BUILD109/lib -lboost_thread-mt-s
LIBS    += -L/Users/francescousai/Documents/Sviluppo/Libs/cgal-releases-CGAL-4.7/BUILD109/lib -lCGAL
#use_gurobi{
LIBS    += -L/Library/gurobi650/mac64/lib -lgurobi_c++
LIBS    += -L/Library/gurobi650/mac64/lib -lgurobi65
#}

unix{
#INCLUDEPATH *= /Users/simonebarbieri/Developer/libQGLViewer-2.5.3
#LIBS    *= -F/Users/simonebarbieri/Library/Frameworks -framework QGLViewer

INCLUDEPATH += /usr/local/include/

#LIBS    += -L/usr/local/lib/ -lCGAL
#LIBS    += -L/usr/local/lib/ -lboost_system
#LIBS    += -L/opt/local/lib -lboost_system-mt
}

win32{
DEFINES += WIN32
DEFINES += _USE_MATH_DEFINES
}

SOURCES += main.cpp \
	engine.cpp \
	bufferStatus.cpp \
	skel/import.cpp \
	skel/select.cpp \
	skel/edit.cpp \
	GUI/mainwindow.cpp \
	GUI/viewer.cpp \
	GUI/skelvisualizationwidget.cpp \
	utilities/manipulatedFrameSetConstraint.cpp \
	utilities/object.cpp \
	GUI/samplingwidget.cpp \
	mesh/mesh.cpp \
	utilities/skel_mesh_helper.cpp \
	GUI/singlebranchingnodedeletionwidget.cpp \
    GUI/choosepastenodewidget.cpp \
    collision_detection/collision_detection.cpp \
    collision_detection/bvh_balancing.cpp
	#GUI/deletenodewithmultipleneighbors.cpp

HEADERS  += engine.h \
	bufferStatus.h \
	skel/base.h \
	skel/bone/base.h \
	skel/skelpoint/base.h \
	skel/import.h \
	skel/export.h \
	skel/update/color.h \
	skel/update/topology.h \
	skel/select.h \
	skel/edit.h \
	skel/paint.h \
	utilities/myGLutils.h \
	utilities/givensQR.h \
	utilities/matrix.h \
	utilities/polyfit.h \
	utilities/skel_mesh_helper.h \
	primitives/geometry.h \
	primitives/BBox.h \
	primitives/color.h \
	GUI/mainwindow.h \
	GUI/viewer.h \
	GUI/skelvisualizationwidget.h \
	utilities/manipulatedFrameSetConstraint.h \
	utilities/object.h \
	GUI/samplingwidget.h \
	mesh/mesh.h \
	mesh/paint.h \
	mesh/import.h \
	GUI/singlebranchingnodedeletionwidget.h \
    GUI/choosepastenodewidget.h \
    collision_detection/collision_detection.h \
    collision_detection/bvh_balancing.h \
    skel/update/approximation.h
#GUI/deletenodewithmultipleneighbors.h

FORMS += GUI/mainwindow.ui \
	GUI/skelvisualizationwidget.ui \
	GUI/samplingwidget.ui \
	GUI/singlebranchingnodedeletionwidget.ui \
    GUI/choosepastenodewidget.ui
	#GUI/deletenodewithmultipleneighbors.ui
