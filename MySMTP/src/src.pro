QT += core network
QT -= gui

TARGET = mysmtp
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += "-std=c++11"

TEMPLATE = app

SOURCES += main.cpp \
    global.cpp \
    smtp_server.cpp

HEADERS += \
    global.h \
    smtp_server.h

