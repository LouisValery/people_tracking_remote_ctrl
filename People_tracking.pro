#-------------------------------------------------
#
# Project created by QtCreator 2020-04-14T16:22:18
#
#-------------------------------------------------

INCLUDEPATH += /usr/local/restpp/include
INCLUDEPATH += /usr/local/cuda/include
INCLUDEPATH += /usr/local/cuda-10.0/include
INCLUDEPATH += /usr/local/zed/include
INCLUDEPATH += /home/user/opencv/include
LIBS += -L"/usr/local/restpp/lib"
LIBS += -L"/usr/local/zed/lib"
LIBS += -L"/usr/local/cuda/lib"
LIBS += -L"/usr/local/cuda-10.0/lib"
LIBS+= -lrestpp
LIBS+= -lopencv_core
LIBS+= -lopencv_highgui
LIBS+= -lopencv_imgproc



QT       += core gui
QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = People_tracking
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    remotecontrolwindow.cpp

HEADERS += \
        mainwindow.h \
    remotecontrolwindow.h \
    portablesleep.h

FORMS += \
        mainwindow.ui \
    remotecontrolwindow.ui
