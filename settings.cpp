/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QFileInfo>
#include <QDir>
#include <QLibraryInfo>

#if defined(Q_OS_MACOS)
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <drumstick/settingsfactory.h>
#include <drumstick/backendmanager.h>
#include <drumstick/pianokeybd.h>
#include "settings.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

Settings::Settings(QObject *parent) : QObject(parent),
    m_trq(nullptr),
    m_trp(nullptr),
    m_trl(nullptr)
{
    ResetDefaults();
    ReadSettings();
}

Settings* Settings::instance()
{
    static Settings inst;
    return &inst;
}

void Settings::setPortableConfig(const QString fileName)
{
    QFileInfo appInfo(QCoreApplication::applicationFilePath());
#if defined(Q_OS_MACOS)
    CFURLRef url = static_cast<CFURLRef>(CFAutorelease(static_cast<CFURLRef>(CFBundleCopyBundleURL(CFBundleGetMainBundle()))));
    QString path = QUrl::fromCFURL(url).path() + "../";
    QFileInfo cfgInfo(path, appInfo.baseName() + ".conf");
#else
    QFileInfo cfgInfo(appInfo.absoluteDir(), appInfo.baseName() + ".conf");
#endif
    if (fileName.isEmpty()) {
        SettingsFactory::setFileName(cfgInfo.absoluteFilePath());
    } else {
        QFileInfo cfgInfo2(fileName);
        if (cfgInfo2.path() == ".") {
            cfgInfo2.setFile(cfgInfo.absoluteDir(), fileName);
        }
        SettingsFactory::setFileName(cfgInfo2.absoluteFilePath());
    }
}

QString Settings::dataDirectory()
{
#if defined(Q_OS_WIN)
    return QCoreApplication::applicationDirPath() + "/";
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath() + "/../Resources/";
#elif defined(Q_OS_UNIX)
    return QCoreApplication::applicationDirPath() + "/../share/dmidiplayer/";
#else
    return QString();
#endif
}

QString Settings::localeDirectory()
{
#if defined(TRANSLATIONS_EMBEDDED)
    return QLatin1String(":/");
#elif defined(Q_OS_LINUX)
    return Settings::dataDirectory();
#elif defined(Q_OS_WIN)
    return Settings::dataDirectory() + "translations/";
#else
    return Settings::dataDirectory();
#endif
}

QString Settings::drumstickLocales()
{
#if defined(TRANSLATIONS_EMBEDDED)
    return QLatin1String(":/");
#elif defined(Q_OS_WIN)
    return QCoreApplication::applicationDirPath() + "/translations/";
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath() + "/../Resources/";
#elif defined(Q_OS_UNIX)
    return QCoreApplication::applicationDirPath() + "/../share/drumstick/";
#else
    return QString();
#endif
}

QString Settings::systemLocales()
{
#if defined(TRANSLATIONS_EMBEDDED)
    return QLatin1String(":/");
#else
    #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    #else
        return QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    #endif
#endif
}

void Settings::ResetDefaults()
{
    m_advanced = false;
    m_defaultsMap = QVariantMap{
        { BackendManager::QSTR_DRUMSTICKRT_PUBLICNAMEOUT, "PlayerOutput"}
    };
    m_highlightPaletteId = drumstick::widgets::PAL_SINGLE;
    m_velocityColor = true;
    m_showStatusBar = true;
    m_showToolBar = true;
#if defined(Q_OS_WINDOWS)
    m_winSnap = true;
#endif
    emit ValuesChanged();
}

void Settings::ReadSettings()
{
    SettingsFactory settings;
    internalRead(*settings.getQSettings());
#if defined (Q_OS_MACOS)
    forceSettings(*settings.getQSettings());
#endif
}

void Settings::SaveSettings()
{
    SettingsFactory settings;
    internalSave(*settings.getQSettings());
}

void Settings::ReadFromFile(const QString &filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalRead(settings);
}

void Settings::SaveToFile(const QString &filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalSave(settings);
}

void Settings::internalRead(QSettings &settings)
{
    QFont defaultFont = QGuiApplication::font();

    settings.beginGroup("MainWindow");
    m_mainWindowGeometry = settings.value("Geometry", QByteArray()).toByteArray();
    m_mainWindowState = settings.value("State", QByteArray()).toByteArray();
    settings.endGroup();

    settings.beginGroup("ChannelsWindow");
    m_channelsWindowGeometry = settings.value("Geometry", QByteArray()).toByteArray();
    m_channelsWindowState = settings.value("State", QByteArray()).toByteArray();
    m_soloVolumeReduction = settings.value("SoloReduction", 50).toInt();
    settings.endGroup();

    settings.beginGroup("HelpWindow");
    m_helpWindowGeometry = settings.value("Geometry", QByteArray()).toByteArray();
    m_helpWindowState = settings.value("State", QByteArray()).toByteArray();
    m_helpWindowFontSize = settings.value("FontSize",  defaultFont.pointSize()).toInt();
    settings.endGroup();

    settings.beginGroup(BackendManager::QSTR_DRUMSTICKRT_GROUP);
    QStringList keys = settings.allKeys(), defaultKeys = m_defaultsMap.keys();
    foreach(const auto& key, defaultKeys) {
        if (!keys.contains(key)) {
            keys.append(key);
        }
    }
    foreach(const QString& key, keys) {
        QVariant defval = m_defaultsMap.contains(key) ? m_defaultsMap[key] : QString();
        m_settingsMap.insert(key, settings.value(key, defval));
    }
    settings.endGroup();

    settings.beginGroup("Connections");
    m_advanced = settings.value("advanced", false).toBool();
    m_lastOutputBackend = settings.value("outputBackend").toString();
    m_lastOutputConnection = settings.value("outputConnection").toString();
    settings.endGroup();

    QLocale loc;
    QString defLang;
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
    QString tmp = loc.name();
    auto lst = tmp.split('_');
    defLang = lst.first();
#else
    defLang = QLocale::languageToCode(loc.language());
#endif
    if (defLang.isEmpty() || defLang == "en") {
        defLang = "C";
    }

    settings.beginGroup("Preferences");
    m_lastDirectory = settings.value("LastDirectory", dataDirectory()).toString();
    m_showStatusBar = settings.value("ShowStatusBar", true).toBool();
    m_showToolBar = settings.value("ShowToolBar", true).toBool();
    m_drumsChannel = settings.value("DrumsChannel", MIDI_GM_STD_DRUM_CHANNEL+1).toInt();
    m_language = settings.value("Language", defLang).toString();
    m_darkMode = settings.value("DarkMode", false).toBool();
    m_autoPlay = settings.value("AutoPlay", true).toBool();
    m_autoAdvance = settings.value("AutoAdvance", true).toBool();
    m_autoSongSettings = settings.value("AutoSongSettings", true).toBool();
    m_octaveSubscript = settings.value("OctaveSubscript", false).toBool();
    QDir defDir(dataDirectory());
    m_lastPlayList = settings.value("LastPlaylist", defDir.absoluteFilePath("examples.lst")).toString();
    bool internalIcons = false;
    QString style;
#if defined(Q_OS_WINDOWS)
    m_winSnap = settings.value("WindowSnapping", true).toBool();
    internalIcons = true;
    style = "fusion";
#endif
#if defined (Q_OS_MACOS)
    internalIcons = true;
#endif
    if (qApp->platformName() == "wayland") {
        m_snappedTogether = false;
    } else {
        m_snappedTogether = settings.value("SnappedTogether", true).toBool();
    }
    m_internalIcons =settings.value("InternalIcons", internalIcons).toBool();
    m_style = settings.value("Style", style).toString();
    m_sysexResetMessage = settings.value("SysexReset", 0).toInt();
    m_toolbarActions = settings.value("ToolbarActions").toStringList();
    m_toolbarButtonStyle = settings.value("ToolbarButtonStyle", Qt::ToolButtonTextUnderIcon).toInt();
    settings.endGroup();

    settings.beginGroup("TextSettings");
    QFont tempFont, defTextFont = QGuiApplication::font();
    defTextFont.setPointSize(36);
    if (tempFont.fromString(settings.value("LyricsFont", defTextFont.toString()).toString())) {
        m_lyricsFont = tempFont;
    } else {
        m_lyricsFont = defTextFont;
    }
    m_futureColor = QColor(settings.value("FutureColor", qApp->palette().color(QPalette::WindowText).name(QColor::HexRgb)).toString());
    m_pastColor = QColor(settings.value("PastColor", QColor(Qt::gray).name(QColor::HexRgb)).toString());
    m_lyricsWindowGeometry = settings.value("Geometry", QByteArray()).toByteArray();
    m_lyricsWindowState = settings.value("State", QByteArray()).toByteArray();
    settings.endGroup();

    settings.beginGroup("PlayerPianoSettings");
    QFont defNoteFont = QGuiApplication::font();
    defNoteFont.setPointSize(50);
    if (tempFont.fromString(settings.value("NotesFont", defNoteFont.toString()).toString())) {
        m_notesFont = tempFont;
    } else {
        m_notesFont = defNoteFont;
    }
    m_highlightPaletteId = settings.value("PaletteId", PAL_SINGLE).toInt();
    m_velocityColor = settings.value("VelocityColor", true).toBool();
    setNamesVisibility(static_cast<LabelVisibility>(settings.value("NamesVisibility", ShowNever).toInt()));
    m_singleColor = QColor(settings.value("SingleColor", m_paletteList[0].getColor(0).name(QColor::HexRgb)).toString());
    m_pianoWindowGeometry = settings.value("Geometry", QByteArray()).toByteArray();
    m_pianoWindowState = settings.value("State", QByteArray()).toByteArray();
    settings.endGroup();

    emit ValuesChanged();
}

void Settings::internalSave(QSettings &settings)
{
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", m_mainWindowGeometry);
    settings.setValue("State", m_mainWindowState);
    settings.endGroup();

    settings.beginGroup(BackendManager::QSTR_DRUMSTICKRT_GROUP);
    QStringList settingsKeys = m_settingsMap.keys();
    foreach(const auto& key, settingsKeys) {
        settings.setValue(key, m_settingsMap[key]);
    }
    settings.endGroup();

    settings.beginGroup("Connections");
    settings.setValue("advanced", m_advanced);
    settings.setValue("outputBackend", m_lastOutputBackend);
    settings.setValue("outputConnection", m_lastOutputConnection);
    settings.endGroup();

    settings.beginGroup("Preferences");
    settings.setValue("LastDirectory", m_lastDirectory);
    settings.setValue("ShowStatusBar", m_showStatusBar);
    settings.setValue("ShowToolBar", m_showToolBar);
    settings.setValue("Language", m_language);
    settings.setValue("DrumsChannel", m_drumsChannel);
    settings.setValue("DarkMode", m_darkMode);
    settings.setValue("AutoPlay", m_autoPlay);
    settings.setValue("AutoAdvance", m_autoAdvance);
    settings.setValue("AutoSongSettings", m_autoSongSettings);
    settings.setValue("OctaveSubscript", m_octaveSubscript);
    settings.setValue("LastPlaylist", m_lastPlayList);
#if defined(Q_OS_WINDOWS)
    settings.setValue("WindowSnapping", m_winSnap);
#endif
    settings.setValue("SnappedTogether", m_snappedTogether);
    settings.setValue("InternalIcons", m_internalIcons);
    settings.setValue("Style", m_style);
    settings.setValue("SysexReset", m_sysexResetMessage);
    settings.setValue("ToolbarActions", m_toolbarActions);
    settings.setValue("ToolbarButtonStyle", m_toolbarButtonStyle);
    settings.endGroup();

    settings.beginGroup("TextSettings");
    settings.setValue("LyricsFont", m_lyricsFont.toString());
    settings.setValue("FutureColor", m_futureColor.name(QColor::HexRgb));
    settings.setValue("PastColor", m_pastColor.name(QColor::HexRgb));
    settings.setValue("Geometry", m_lyricsWindowGeometry);
    settings.setValue("State", m_lyricsWindowState);
    settings.endGroup();

    settings.beginGroup("PlayerPianoSettings");
    settings.setValue("NotesFont", m_notesFont.toString());
    settings.setValue("VelocityColor", m_velocityColor);
    settings.setValue("PaletteId", m_highlightPaletteId);
    settings.setValue("NamesVisibility", m_namesVisibility);
    settings.setValue("SingleColor", m_singleColor.name(QColor::HexRgb));
    settings.setValue("Geometry", m_pianoWindowGeometry);
    settings.setValue("State", m_pianoWindowState);
    settings.endGroup();

    settings.beginGroup("HelpWindow");
    settings.setValue("Geometry", m_helpWindowGeometry);
    settings.setValue("State", m_helpWindowState);
    settings.setValue("FontSize", m_helpWindowFontSize);
    settings.endGroup();

    settings.beginGroup("ChannelsWindow");
    settings.setValue("Geometry", m_channelsWindowGeometry);
    settings.setValue("State", m_channelsWindowState);
    settings.setValue("SoloReduction", m_soloVolumeReduction);
    settings.endGroup();

    settings.sync();
}

void Settings::forceSettings(QSettings &settings)
{
#if defined(Q_OS_MACOS)
    settings.beginGroup("DLS Synth");
    settings.setValue("reverb_dls", true);
    settings.endGroup();
#endif
}

bool Settings::snappedTogether() const
{
    return m_snappedTogether;
}

void Settings::setSnappedTogether(bool newSnappedTogether)
{
    if (qApp->platformName() != "wayland") {
        m_snappedTogether = newSnappedTogether;
    }
}

bool Settings::octaveSubscript() const
{
    return m_octaveSubscript;
}

void Settings::setOctaveSubscript(bool newOctaveSubscript)
{
    m_octaveSubscript = newOctaveSubscript;
}

bool Settings::autoSongSettings() const
{
    return m_autoSongSettings;
}

void Settings::setAutoSongSettings(bool newAutoSongSettings)
{
    m_autoSongSettings = newAutoSongSettings;
}

int Settings::soloVolumeReduction() const
{
    return m_soloVolumeReduction;
}

void Settings::setSoloVolumeReduction(int newSoloVolumeReduction)
{
    m_soloVolumeReduction = newSoloVolumeReduction;
}

int Settings::helpWindowFontSize() const
{
    return m_helpWindowFontSize;
}

void Settings::setHelpWindowFontSize(int newHelpWindowFontSize)
{
    m_helpWindowFontSize = newHelpWindowFontSize;
}

void Settings::loadTranslations()
{
    QLocale loc;
    QString lang = language();
    if (!lang.isEmpty()) {
        loc = QLocale(lang);
    }
    if (m_trq) {
        QCoreApplication::removeTranslator(m_trq);
        delete m_trq;
    }
    if (m_trp) {
        QCoreApplication::removeTranslator(m_trp);
        delete m_trp;
    }
    if (m_trl) {
        QCoreApplication::removeTranslator(m_trl);
        delete m_trl;
    }
    if (loc.language() != QLocale::C && loc.language() != QLocale::English) {
        bool ok = false;
        m_trq = new QTranslator(this);
        if (m_trq->load(loc, "qt", "_", Settings::systemLocales())) {
            ok = QCoreApplication::installTranslator(m_trq);
        }
        if(!ok) {
            qWarning() << "Failure loading Qt system translations for" << lang
                       << "from" << Settings::systemLocales();
            delete m_trq;
        }
        ok = false;
        m_trp = new QTranslator(this);
        if (m_trp->load(loc, "dmidiplayer", "_", Settings::localeDirectory())) {
            ok = QCoreApplication::installTranslator(m_trp);
        }
        if (!ok) {
            qWarning() << "Failure loading application translations for" << lang
                       << "from" << Settings::localeDirectory();
            delete m_trp;
        }
        ok = false;
        m_trl = new QTranslator(this);
        if (m_trl->load(loc, "drumstick-widgets", "_", Settings::drumstickLocales())) {
            ok = QCoreApplication::installTranslator(m_trl);
        }
        if (!ok) {
            qWarning() << "Failure loading widgets library translations for" << lang
                       << "from" << Settings::drumstickLocales();
            delete m_trl;
        }
    }
    retranslatePalettes();
}

const QByteArray &Settings::pianoWindowState() const
{
    return m_pianoWindowState;
}

void Settings::setPianoWindowState(const QByteArray &newPianoWindowState)
{
    m_pianoWindowState = newPianoWindowState;
}

const QByteArray &Settings::pianoWindowGeometry() const
{
    return m_pianoWindowGeometry;
}

void Settings::setPianoWindowGeometry(const QByteArray &newPianoWindowGeometry)
{
    m_pianoWindowGeometry = newPianoWindowGeometry;
}

const QByteArray &Settings::lyricsWindowState() const
{
    return m_lyricsWindowState;
}

void Settings::setLyricsWindowState(const QByteArray &newLyricsWindowState)
{
    m_lyricsWindowState = newLyricsWindowState;
}

const QByteArray &Settings::lyricsWindowGeometry() const
{
    return m_lyricsWindowGeometry;
}

void Settings::setLyricsWindowGeometry(const QByteArray &newLyricsWindowGeometry)
{
    m_lyricsWindowGeometry = newLyricsWindowGeometry;
}

const QByteArray &Settings::helpWindowState() const
{
    return m_helpWindowState;
}

void Settings::setHelpWindowState(const QByteArray &newHelpWindowState)
{
    m_helpWindowState = newHelpWindowState;
}

const QByteArray &Settings::helpWindowGeometry() const
{
    return m_helpWindowGeometry;
}

void Settings::setHelpWindowGeometry(const QByteArray &newHelpWindowGeometry)
{
    m_helpWindowGeometry = newHelpWindowGeometry;
}

const QByteArray &Settings::channelsWindowState() const
{
    return m_channelsWindowState;
}

void Settings::setChannelsWindowState(const QByteArray &newChannelsWindowState)
{
    m_channelsWindowState = newChannelsWindowState;
}

const QByteArray &Settings::channelsWindowGeometry() const
{
    return m_channelsWindowGeometry;
}

void Settings::setChannelsWindowGeometry(const QByteArray &newChannelsWindowGeometry)
{
    m_channelsWindowGeometry = newChannelsWindowGeometry;
}

int Settings::toolbarButtonStyle() const
{
    return m_toolbarButtonStyle;
}

void Settings::setToolbarButtonStyle(int newToolbarButtonStyle)
{
    m_toolbarButtonStyle = newToolbarButtonStyle;
}

const QStringList &Settings::toolbarActions() const
{
    return m_toolbarActions;
}

void Settings::setToolbarActions(const QStringList &newToolbarActions)
{
    m_toolbarActions = newToolbarActions;
}

int Settings::getSysexResetMessage() const
{
    return m_sysexResetMessage;
}

void Settings::setSysexResetMessage(const int sysexResetMessage)
{
    m_sysexResetMessage = sysexResetMessage;
}

QString Settings::getStyle() const
{
    return m_style;
}

void Settings::setStyle(const QString &style)
{
    m_style = style;
}

bool Settings::useInternalIcons() const
{
    return m_internalIcons;
}

void Settings::setInternalIcons(bool internalIcons)
{
    m_internalIcons = internalIcons;
}

QString Settings::lastPlayList() const
{
    return m_lastPlayList;
}

void Settings::setLastPlayList(const QString &defaultPlayList)
{
    m_lastPlayList = defaultPlayList;
}

bool Settings::getAutoPlay() const
{
    return m_autoPlay;
}

void Settings::setAutoPlay(bool autoPlay)
{
    m_autoPlay = autoPlay;
}

bool Settings::autoAdvance() const
{
    return m_autoAdvance;
}

void Settings::setAutoAdvance(bool autoAdvance)
{
    m_autoAdvance = autoAdvance;
}

QColor Settings::getSingleColor() const
{
    return m_singleColor;
}

void Settings::setSingleColor(const QColor &singleColor)
{
    m_singleColor = singleColor;
}

QColor Settings::getPastColor() const
{
    return m_pastColor;
}

void Settings::setPastColor(const QColor &pastColor)
{
    m_pastColor = pastColor;
}

QColor Settings::getFutureColor() const
{
    return m_futureColor;
}

void Settings::setFutureColor(const QColor &futureColor)
{
    m_futureColor = futureColor;
}

bool Settings::getDarkMode() const
{
    return m_darkMode;
}

void Settings::setDarkMode(bool darkMode)
{
    m_darkMode = darkMode;
}

bool Settings::showToolBar() const
{
    return m_showToolBar;
}

void Settings::setShowToolBar(bool showToolBar)
{
    m_showToolBar = showToolBar;
}

bool Settings::showStatusBar() const
{
    return m_showStatusBar;
}

void Settings::setShowStatusBar(bool showStatusBar)
{
    m_showStatusBar = showStatusBar;
}

int Settings::highlightPaletteId() const
{
    return m_highlightPaletteId;
}

void Settings::setHighlightPaletteId(int paletteId)
{
    m_highlightPaletteId = paletteId;
}

bool Settings::velocityColor() const
{
    return m_velocityColor;
}

void Settings::setVelocityColor(bool velocityColor)
{
    m_velocityColor = velocityColor;
}

QString Settings::language() const
{
    //qDebug() << Q_FUNC_INFO << m_language;
    if (m_language.length() > 2) {
        return m_language.left(2);
    }
    return m_language;
}

void Settings::setLanguage(const QString &language)
{
    m_language = language;
}

int Settings::drumsChannel() const
{
    return m_drumsChannel;
}

void Settings::setDrumsChannel(int drumsChannel)
{
    m_drumsChannel = drumsChannel;
}

QVariantMap Settings::settingsMap() const
{
    return m_settingsMap;
}

bool Settings::advanced() const
{
    return m_advanced;
}

void Settings::setAdvanced(bool advanced)
{
    m_advanced = advanced;
}

QString Settings::lastOutputConnection() const
{
    return m_lastOutputConnection;
}

void Settings::setLastOutputConnection(const QString &lastOutputConnection)
{
    m_lastOutputConnection = lastOutputConnection;
}

QString Settings::lastOutputBackend() const
{
    return m_lastOutputBackend;
}

void Settings::setLastOutputBackend(const QString &lastOutputBackend)
{
    m_lastOutputBackend = lastOutputBackend;
}

QByteArray Settings::mainWindowState() const
{
    return m_mainWindowState;
}

void Settings::setMainWindowState(const QByteArray &state)
{
    m_mainWindowState = state;
}

QByteArray Settings::mainWindowGeometry() const
{
    return m_mainWindowGeometry;
}

void Settings::setMainWindowGeometry(const QByteArray &geometry)
{
    m_mainWindowGeometry = geometry;
}

PianoPalette Settings::getPalette(int pal)
{
    if (pal >= 0 && pal < m_paletteList.count()) {
        return m_paletteList[pal];
    }
    return m_paletteList[0];
}

QList<QString> Settings::availablePaletteNames(bool forHighlight)
{
    QList<QString> tmp;
    for (PianoPalette& p : m_paletteList) {
        if (forHighlight && !p.isHighLight()) {
            continue;
        }
        tmp << p.paletteName();
    }
    return tmp;
}

int Settings::availablePalettes() const
{
    return m_paletteList.length();
}

void Settings::retranslatePalettes()
{
    for (PianoPalette& pal : m_paletteList) {
        pal.retranslateStrings();
    }
}

QString Settings::lastDirectory() const
{
    return m_lastDirectory;
}

void Settings::setLastDirectory(const QString &lastDirectory)
{
    m_lastDirectory = lastDirectory;
}

bool Settings::winSnap() const
{
    return m_winSnap;
}

void Settings::setWinSnap(bool winSnap)
{
    m_winSnap = winSnap;
}

QFont Settings::notesFont() const
{
    return m_notesFont;
}

QFont Settings::lyricsFont() const
{
    return m_lyricsFont;
}

void Settings::setNotesFont(const QFont &notesFont)
{
    m_notesFont = notesFont;
}

void Settings::setLyricsFont(const QFont &lyricsFont)
{
    m_lyricsFont = lyricsFont;
}

LabelVisibility Settings::namesVisibility() const
{
    return m_namesVisibility;
}

void Settings::setNamesVisibility(const LabelVisibility namesVisibility)
{
    m_namesVisibility = namesVisibility;
}
