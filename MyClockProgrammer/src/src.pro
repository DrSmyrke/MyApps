QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myClockProgrammer
CONFIG += c++11

CONFIG(debug, debug|release):CONFIGURATION=debug

CONFIG(release, debug|release){
    CONFIGURATION=release
    #from static build
    QMAKE_LFLAGS_RELEASE += -static -static-libgcc
}

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_debug)
}

DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR         = ../build/$$CONFIGURATION
MOC_DIR             = ../build/$$CONFIGURATION
RCC_DIR             = ../build/$$CONFIGURATION
UI_DIR              = ../build/$$CONFIGURATION
DESTDIR             = ../bin

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += resources.qrc

exists(./gitversion.pri):include(./gitversion.pri)
exists(./myLibs.pri):include(./myLibs.pri)
