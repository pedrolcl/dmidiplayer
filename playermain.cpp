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

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDebug>
#include <drumstick/backendmanager.h>
#include "guiplayer.h"

using namespace drumstick::rt;

int main(int argc, char *argv[])
{
    const QString PGM_DESCRIPTION("Drumstick MIDI File Player Multiplatform Program");
    const QString errorstr = "Fatal error from the Operating System. "
        "This usually happens when the OS doesn't have MIDI support, "
        "or the MIDI support is not enabled. "
        "Please check your OS/MIDI configuration.";

    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
    QCoreApplication::setApplicationVersion(QT_STRINGIFY(VERSION));
    QApplication app(argc, argv);

    QLocale locale;
    QTranslator qtTranslator;
    if (qtTranslator.load(locale, "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(&qtTranslator);
    }

    QTranslator appTranslator;
    if (appTranslator.load(locale, "dmidiplayer", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(&appTranslator);
    } else if (appTranslator.load(locale, "dmidiplayer", "_", QApplication::applicationDirPath() + "/../share/dmidiplayer/")) {
        QCoreApplication::installTranslator(&appTranslator);
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(PGM_DESCRIPTION);
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();
    QCommandLineOption driverOption({"d", "driver"}, "MIDI Out Driver.", "driver");
    parser.addOption(driverOption);
    QCommandLineOption portOption({"c", "connection"}, "MIDI Out Connection.", "connection");
    parser.addOption(portOption);
    parser.addPositionalArgument("file", "Input SMF/KAR/WRK file name.", "file");
    parser.process(app);

    if (parser.isSet(versionOption) || parser.isSet(helpOption)) {
        return 0;
    }

    QStringList fileNames, positionalArgs = parser.positionalArguments();
    foreach(const QString& a, positionalArgs) {
        QFileInfo f(a);
        if (f.exists())
            fileNames += f.canonicalFilePath();
        else
            qWarning() << "File not found: " << a << endl;
    }

    try {
        GUIPlayer w;
        w.setAttribute(Qt::WA_QuitOnClose);
        if(parser.isSet(portOption) && parser.isSet(driverOption)) {
            w.connectOutput(parser.value(driverOption), parser.value(portOption));
        }
        if (!fileNames.isEmpty()) {
            w.openFile(fileNames.first());
        }
        w.show();
        return app.exec();
    } catch (...) {
        QMessageBox::critical(0, "Error", errorstr );
        qWarning() << errorstr;
    }
    return 0;
}
