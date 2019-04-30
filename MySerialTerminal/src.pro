#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T16:39:08
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myserialterminal
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
    global.cpp \
    serial.cpp

HEADERS  += mainwindow.h \
    global.h \
    serial.h

TRANSLATIONS = lang/ru_RU.ts
