TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = drawLogs
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

QMAKE_CXXFLAGS += "-std=c++11"

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0
