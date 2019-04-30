#QT += core
QT += network
#QT -= gui

TARGET = SOCKSproxy
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += "-std=c++11"

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    client.cpp \
    global.cpp

HEADERS += \
    server.h \
    client.h \
    global.h
