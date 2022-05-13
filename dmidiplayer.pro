TEMPLATE = app
TARGET = dmidiplayer
VERSION = 1.6.0
QT += gui widgets printsupport
CONFIG += qt thread exceptions c++11 link_pkgconfig
INCLUDEPATH += $$(DRUMSTICKINCLUDES)
LIBS += -L $$(DRUMSTICKLIBS) -ldrumstick-rt -ldrumstick-file -ldrumstick-widgets
PKGCONFIG += uchardet
DEFINES += VERSION=$$VERSION
FORMS += guiplayer.ui \
    loopdialog.ui \
    playerabout.ui \
    connections.ui \
    playlist.ui \
    prefsdialog.ui \
    toolbareditdialog.ui
HEADERS += \
    channels.h \
    connections.h \
    events.h \
    framelesswindow.h \
    guiplayer.h \
    helpwindow.h \
    iconutils.h \
    instrumentset.h \
    loopdialog.h \
    lyrics.h \
    pianola.h \
    playerabout.h \
    playlist.h \
    prefsdialog.h \
    recentfileshelper.h \
    rhythmview.h \
    seqplayer.h \
    sequence.h \
    settings.h \
    toolbareditdialog.h \
    vumeter.h
SOURCES += \
    channels.cpp \
    connections.cpp \
    events.cpp \
    framelesswindow.cpp \
    guiplayer.cpp \
    helpwindow.cpp \
    iconutils.cpp \
    instrumentset.cpp \
    loopdialog.cpp \
    lyrics.cpp \
    pianola.cpp \
    playerabout.cpp \
    playermain.cpp \
    playlist.cpp \
    prefsdialog.cpp \
    recentfileshelper.cpp \
    rhythmview.cpp \
    seqplayer.cpp \
    sequence.cpp \
    settings.cpp \
    toolbareditdialog.cpp \
    vumeter.cpp
win32 {
    HEADERS += winsnap.h
    SOURCES += winsnap.cpp
}
RESOURCES += \
  guiplayer.qrc \
  docs/help.qrc
TRANSLATIONS += \
  translations/dmidiplayer_cs.ts \
  translations/dmidiplayer_de.ts \
  translations/dmidiplayer_en.ts \
  translations/dmidiplayer_es.ts \
  translations/dmidiplayer_fr.ts \
  translations/dmidiplayer_gl.ts \
  translations/dmidiplayer_it.ts \
  translations/dmidiplayer_ru.ts
