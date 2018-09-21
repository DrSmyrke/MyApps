TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    iniFile.cpp \
    settings.cpp \
    gui.cpp \
    functions.cpp
HEADERS += \
    iniFile.h \
    settings.h \
    main.h \
    gui.h \
    functions.h

QMAKE_CXXFLAGS += "-std=c++11"

#INCLUDEPATH += /usr/include/gtk-2.0 \
#/usr/include/glib-2.0 \
#/usr/lib/i386-linux-gnu/glib-2.0/include/ \
#/usr/include/cairo/ \
#/usr/include/pango-1.0/ \
#/usr/lib/i386-linux-gnu/gtk-2.0/include/ \
#/usr/include/gdk-pixbuf-2.0/ \
#/usr/include/atk-1.0/

CONFIG += link_pkgconfig
#PKGCONFIG += gtk+-2.0
PKGCONFIG += gtk+-3.0
