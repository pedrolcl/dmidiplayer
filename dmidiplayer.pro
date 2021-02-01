TEMPLATE = app
TARGET = dmidiplayer
VERSION = 1.0.0
QT += gui widgets
CONFIG += qt thread exceptions c++11
INCLUDEPATH += $$(DRUMSTICKINCLUDES)
LIBS += -L $$(DRUMSTICKLIBS) -ldrumstick-rt -ldrumstick-file -ldrumstick-widgets
DEFINES += VERSION=$$VERSION
FORMS += guiplayer.ui \
    playerabout.ui \
    connections.ui
HEADERS += \
    guiplayer.h \
    playerabout.h \
    events.h \
    iconutils.h \
    seqplayer.h \
    sequence.h \
    connections.h
SOURCES += playermain.cpp \
    guiplayer.cpp \
    playerabout.cpp \
    events.cpp \
    iconutils.cpp \
    seqplayer.cpp \
    sequence.cpp \
    connections.cpp
RESOURCES += guiplayer.qrc
TRANSLATIONS += translations/dmidiplayer_es.ts
