#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T16:39:08
#
#-------------------------------------------------

QT       += core gui network serialport x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mypanel
TEMPLATE = app

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
    windows/serialmonitor.cpp \
    nativeeventfilter.cpp \
    windows/settings.cpp \
    widgets/hwmonitorwidget.cpp \
    windows/hwmonitorwindow.cpp \
    hwmonitor.cpp \
    Net/ip.cpp \
    Net/mac.cpp \
    Net/netif.cpp \
    windows/execwindow.cpp \
    process.cpp \
    windows/bookmarkswindow.cpp \
    wget.cpp \
    inetdataupdate.cpp

HEADERS  += mainwindow.h \
    global.h \
    windows/serialmonitor.h \
    nativeeventfilter.h \
    windows/settings.h \
    widgets/hwmonitorwidget.h \
    windows/hwmonitorwindow.h \
    hwmonitor.h \
    Net/ip.h \
    Net/mac.h \
    Net/netif.h \
    windows/execwindow.h \
    process.h \
    windows/bookmarkswindow.h \
    wget.h \
    inetdataupdate.h

TRANSLATIONS = lang/ru_RU.ts

RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui
