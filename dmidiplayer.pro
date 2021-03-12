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
    channels.h \
    connections.h \
    events.h \
    guiplayer.h \
    iconutils.h \
    instrumentset.h \
    pianola.h \
    playerabout.h \
    recentfileshelper.h \
    seqplayer.h \
    sequence.h \
    settings.h \
    vumeter.h
SOURCES += \
    channels.cpp \
    connections.cpp \
    events.cpp \
    guiplayer.cpp \
    iconutils.cpp \
    instrumentset.cpp \
    pianola.cpp \
    playerabout.cpp \
    playermain.cpp \
    recentfileshelper.cpp \
    seqplayer.cpp \
    sequence.cpp \
    settings.cpp \
    vumeter.cpp
win32 {
    HEADERS += winsnap.h
    SOURCES += winsnap.cpp
}
RESOURCES += guiplayer.qrc
TRANSLATIONS += \
  translations/dmidiplayer_es.ts \
  translations/dmidiplayer_ru.ts
