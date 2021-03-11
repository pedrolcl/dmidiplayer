TEMPLATE = app
TARGET = dmidiplayer
VERSION = 1.0.1
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
    connections.h \
    pianola.h \
    channels.h \
    instrumentset.h \
    recentfileshelper.h \
    vumeter.h
SOURCES += playermain.cpp \
    guiplayer.cpp \
    playerabout.cpp \
    events.cpp \
    iconutils.cpp \
    seqplayer.cpp \
    sequence.cpp \
    connections.cpp \
    pianola.cpp \
    channels.cpp \
    instrumentset.cpp \
    recentfileshelper.cpp \
    vumeter.cpp
RESOURCES += guiplayer.qrc
TRANSLATIONS += \
  translations/dmidiplayer_es.ts \
  translations/dmidiplayer_ru.ts
