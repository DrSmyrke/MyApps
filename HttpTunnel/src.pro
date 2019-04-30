#QT += core
QT += network
#QT -= gui

TARGET = HttpTunnel
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    client.cpp

HEADERS += \
    server.h \
    client.h
