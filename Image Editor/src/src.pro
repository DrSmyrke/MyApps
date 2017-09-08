#-------------------------------------------------
#
# Project created by QtCreator 2017-08-16T14:18:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app

QMAKE_CXXFLAGS += "-std=c++11"

SOURCES += main.cpp\
        mainwindow.cpp \
    widgets/imageeditor.cpp

HEADERS  += mainwindow.h \
    widgets/imageeditor.h
