#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T16:39:08
#
#-------------------------------------------------

QT       += core gui network x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mypanel
TEMPLATE = app

OBJECTS_DIR = _build
DESTDIR  = ../bin

CONFIG    += link_pkgconfig
PKGCONFIG += x11

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}
QMAKE_CXXFLAGS += "-std=c++11"

SOURCES += main.cpp\
        mainwindow.cpp \
    global.cpp \
    nativeeventfilter.cpp \
    windows/settings.cpp \
    windows/execwindow.cpp \
    windows/bookmarkswindow.cpp \
    wget.cpp \
    dataupdate.cpp

HEADERS  += mainwindow.h \
    global.h \
    nativeeventfilter.h \
    windows/settings.h \
    windows/execwindow.h \
    windows/bookmarkswindow.h \
    wget.h \
    dataupdate.h

TRANSLATIONS = lang/ru_RU.ts

RESOURCES += \
    resources.qrc

FORMS +=
