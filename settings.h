﻿/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2021, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PORTABLESETTINGS_H
#define PORTABLESETTINGS_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <drumstick/backendmanager.h>
#include <drumstick/pianopalette.h>
#include <drumstick/pianokeybd.h>

using namespace drumstick::widgets;

class Settings : public QObject
{
    Q_OBJECT

public:
    QByteArray mainWindowGeometry() const;
    void setMainWindowGeometry(const QByteArray &geometry);

    QByteArray mainWindowState() const;
    void setMainWindowState(const QByteArray &state);

    QString lastDirectory() const;
    void setLastDirectory(const QString &lastDirectory);

    QString lastOutputBackend() const;
    void setLastOutputBackend(const QString &lastOutputBackend);

    QString lastOutputConnection() const;
    void setLastOutputConnection(const QString &lastOutputConnection);

    bool advanced() const;
    void setAdvanced(bool advanced);

    QVariantMap settingsMap() const;

    QString language() const;
    void setLanguage(const QString &language);

    QString lastPlayList() const;
    void setLastPlayList(const QString &lastPlayList);

    int drumsChannel() const;
    void setDrumsChannel(int drumsChannel);

    bool velocityColor() const;
    void setVelocityColor(bool velocityColor);

    bool winSnap() const;
    void setWinSnap(bool winSnap);

    int highlightPaletteId() const;
    void setHighlightPaletteId(int paletteId);

    bool showStatusBar() const;
    void setShowStatusBar(bool showStatusBar);

    bool showToolBar() const;
    void setShowToolBar(bool showToolBar);

    int availablePalettes() const;
    PianoPalette getPalette(int pal);
    QList<QString> availablePaletteNames(bool forHighlight);
    void retranslatePalettes();

    LabelVisibility namesVisibility() const;
    void setNamesVisibility(const LabelVisibility namesVisibility);

    QFont notesFont() const;
    void setNotesFont(const QFont &namesFont);

    bool getDarkMode() const;
    void setDarkMode(bool darkMode);

    QFont lyricsFont() const;
    void setLyricsFont(const QFont &lyricsFont);

    QColor getFutureColor() const;
    void setFutureColor(const QColor &futureColor);

    QColor getPastColor() const;
    void setPastColor(const QColor &pastColor);

    QColor getSingleColor() const;
    void setSingleColor(const QColor &singleColor);

    bool getAutoPlay() const;
    void setAutoPlay(bool autoPlay);

    bool autoAdvance() const;
    void setAutoAdvance(bool autoAdvance);

    bool useInternalIcons() const;
    void setInternalIcons(bool internalIcons);

    QString getStyle() const;
    void setStyle(const QString &style);

    int getSysexResetMessage() const;
    void setSysexResetMessage(const int sysexResetMessage);

    const QStringList &toolbarActions() const;
    void setToolbarActions(const QStringList &newToolbarActions);

    int toolbarButtonStyle() const;
    void setToolbarButtonStyle(int newToolbarButtonStyle);

    // static methods
    static Settings* instance();
    static void setPortableConfig(const QString fileName = QString());
    static QString dataDirectory();
    static QString localeDirectory();
    static QString drumstickLocales();
    static QString systemLocales();

signals:
    void ValuesChanged();

public slots:
    void ResetDefaults();
    void ReadSettings();
    void ReadFromFile(const QString &filepath);
    void SaveSettings();
    void SaveToFile(const QString &filepath);

private:
    explicit Settings(QObject *parent = nullptr);
    void internalRead(QSettings &settings);
    void internalSave(QSettings &settings);

    QByteArray m_mainWindowGeometry;
    QByteArray m_mainWindowState;
    QString m_lastDirectory;
    QString m_lastOutputBackend;
    QString m_lastOutputConnection;
    QString m_lastPlayList;
    QString m_style;

    bool m_advanced;
    int m_highlightPaletteId;
    bool m_velocityColor;
    bool m_showStatusBar;
    bool m_showToolBar;
    bool m_winSnap;
    bool m_darkMode;
    int m_drumsChannel;
    bool m_autoPlay;
    bool m_autoAdvance;
    bool m_internalIcons;
    int m_sysexResetMessage;
    LabelVisibility m_namesVisibility;
    QFont m_notesFont;
    QFont m_lyricsFont;
    QColor m_futureColor;
    QColor m_pastColor;
    QColor m_singleColor;
    QString m_language;
    QVariantMap m_settingsMap;
    QVariantMap m_defaultsMap;

    QList<PianoPalette> m_paletteList {
        PianoPalette(PAL_SINGLE),
        PianoPalette(PAL_DOUBLE),
        PianoPalette(PAL_CHANNELS),
        PianoPalette(PAL_SCALE),
        PianoPalette(PAL_KEYS),
        PianoPalette(PAL_FONT),
        PianoPalette(PAL_HISCALE)
    };

    QStringList m_toolbarActions;
    int m_toolbarButtonStyle;
};

#endif // PORTABLESETTINGS_H
