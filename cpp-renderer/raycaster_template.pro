#-------------------------------------------------
#
# Project created by QtCreator 2011-08-03T14:59:51
#
#-------------------------------------------------

QT += core \
    gui \
    opengl \
    qt3support

TARGET = raycaster_template
TEMPLATE = app


SOURCES += main.cpp \
    Volume.cpp \
    Quaternion.cpp \
    mainwindow.cpp \
    glwidgetslicer.cpp \
    glwidgetdvr.cpp \
    glwidgetcube.cpp \
    Vector3d.cpp \
    Matrix4d.cpp \
    tf_dialog.cpp \
    gl_tf_editor.cpp \
    transfer_function.cpp \
    ViewPlane.cpp

HEADERS  += mainwindow.h \
            Quaternion.h\
            Volume.h     \
            glwidgetslicer.h  \
            glwidgetdvr.h  \
            glwidgetcube.h \  
    Vector3d.h \
    Matrix4d.h \
    tf_dialog.h \
    gl_tf_editor.h \
    transfer_function.h \
    ViewPlane.h
        

FORMS    +=
