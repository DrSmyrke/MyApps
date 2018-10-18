#-------------------------------------------------
#
# Project created by QtCreator 2018-09-20T10:17:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mystats
TEMPLATE = app

OBJECTS_DIR = _build
DESTDIR  = ../bin

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}
QMAKE_CXXFLAGS += "-std=c++11"

SOURCES += main.cpp\
        mainwindow.cpp \
    widgets/hwmonitorwidget.cpp \
    hwmonitor.cpp \
    global.cpp

HEADERS  += mainwindow.h \
    widgets/hwmonitorwidget.h \
    hwmonitor.h \
    global.h

TRANSLATIONS = lang/ru_RU.ts

RESOURCES += \
    resources.qrc
