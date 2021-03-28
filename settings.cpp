/*
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

#include <QFileInfo>
#include <QDir>
//#include <QDebug>
#include <QTouchDevice>
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

Settings::Settings(QObject *parent) : QObject(parent)
{
#if defined(Q_OS_LINUX)
    m_defaultOutputBackend = QLatin1String("SonivoxEAS");
    m_defaultOutputConnection = QLatin1String("SonivoxEAS");
#elif defined(Q_OS_MACOS)
    m_defaultOutputBackend = QLatin1String("DLS Synth");
    m_defaultOutputConnection = QLatin1String("DLS Synth");
#elif defined(Q_OS_WIN)
    m_defaultOutputBackend = QLatin1String("Windows MM");
    m_defaultOutputConnection = QLatin1String("Microsoft GS Wavetable Synth");
#else
    m_defaultOutputBackend = m_defaultInputBackend;
    m_defaultOutputConnection = m_defaultInputConnection;
#endif
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
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
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
    settings.beginGroup("MainWindow");
    m_mainWindowGeometry = settings.value("Geometry").toByteArray();
    m_mainWindowState = settings.value("State").toByteArray();
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
    m_lastOutputBackend = settings.value("outputBackend", m_defaultOutputBackend).toString();
    m_lastOutputConnection = settings.value("outputConnection", m_defaultOutputConnection).toString();
    settings.endGroup();

    settings.beginGroup("Preferences");
    m_lastDirectory = settings.value("LastDirectory").toString();
    m_showStatusBar = settings.value("ShowStatusBar", false).toBool();
    m_showToolBar = settings.value("ShowToolBar", false).toBool();
    m_velocityColor = settings.value("VelocityColor", true).toBool();
    m_highlightPaletteId = settings.value("PaletteId", PAL_SINGLE).toInt();
    m_language = settings.value("Language", QLocale::system().name()).toString();
#if defined(Q_OS_WINDOWS)
    m_winSnap = settings.value("WindowSnapping", true).toBool();
#endif
    settings.endGroup();

    settings.beginGroup("TextSettings");
    QFont f;
    if (f.fromString(settings.value("namesFont", "Sans Serif,50").toString())) {
        setNamesFont(f);
    }
    setNamesOrientation(static_cast<LabelOrientation>(settings.value("namesOrientation", HorizontalOrientation).toInt()));
    setNamesVisibility(static_cast<LabelVisibility>(settings.value("namesVisibility", ShowNever).toInt()));
    setNamesAlterations(static_cast<LabelAlteration>(settings.value("namesAlteration", ShowSharps).toInt()));
    setNamesOctave(static_cast<LabelCentralOctave>(settings.value("namesOctave", OctaveC4).toInt()));
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
    settings.setValue("VelocityColor", m_velocityColor);
    settings.setValue("PaletteId", m_highlightPaletteId);
    settings.setValue("Language", m_language);
#if defined(Q_OS_WINDOWS)
    settings.setValue("WindowSnapping", m_winSnap);
#endif
    settings.endGroup();

    settings.beginGroup("TextSettings");
    settings.setValue("namesFont", m_namesFont.toString());
    settings.setValue("namesOrientation", m_namesOrientation);
    settings.setValue("namesVisibility", m_namesVisibility);
    settings.setValue("namesAlteration", m_namesAlteration);
    settings.setValue("namesOctave", m_namesOctave);
    settings.endGroup();

    settings.sync();
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
    return m_language;
}

void Settings::setLanguage(const QString &language)
{
    m_language = language;
}

QVariantMap Settings::settingsMap() const
{
    return m_settingsMap;
}

QString Settings::nativeOutput() const
{
    return m_defaultOutputBackend;
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

QFont Settings::namesFont() const
{
    return m_namesFont;
}

void Settings::setNamesFont(const QFont &namesFont)
{
    m_namesFont = namesFont;
}

LabelAlteration Settings::alterations() const
{
    return m_namesAlteration;
}

void Settings::setNamesAlterations(const LabelAlteration alterations)
{
    m_namesAlteration = alterations;
}

LabelVisibility Settings::namesVisibility() const
{
    return m_namesVisibility;
}

void Settings::setNamesVisibility(const LabelVisibility namesVisibility)
{
    m_namesVisibility = namesVisibility;
}

LabelOrientation Settings::namesOrientation() const
{
    return m_namesOrientation;
}

void Settings::setNamesOrientation(const LabelOrientation namesOrientation)
{
    m_namesOrientation = namesOrientation;
}

LabelCentralOctave Settings::namesOctave() const
{
    return m_namesOctave;
}

void Settings::setNamesOctave(const LabelCentralOctave namesOctave)
{
    m_namesOctave = namesOctave;
}
