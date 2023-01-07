/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QScreen>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QSplashScreen>
#include <QDebug>
#include "guiplayer.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    const char* PGM_DESCRIPTION = QT_TRANSLATE_NOOP("QCoreApplication",
        "Copyright (C) 2006-2023 Pedro Lopez-Cabanillas\n"
        "This program comes with ABSOLUTELY NO WARRANTY;\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under certain conditions; see the LICENSE for details.");

    const char* errorstr = QT_TRANSLATE_NOOP("QCoreApplication",
        "Fatal error from the Operating System. "
        "This usually happens when the OS doesn't have MIDI support, "
        "or the MIDI support is not enabled. "
        "Please check your OS/MIDI configuration.");

    const char* strVersion = QT_STRINGIFY(VERSION);

#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
    QCoreApplication::setApplicationVersion(strVersion);
#if defined(Q_OS_WINDOWS)
    QApplication::setStyle("fusion");
#endif
    QGuiApplication::setDesktopFileName("net.sourceforge.dmidiplayer");
    QApplication::setWindowIcon(QIcon(":/dmidiplayer.png"));

    Settings::instance()->loadTranslations();

    QCommandLineParser parser;
    parser.setApplicationDescription(QString("%1 v%2\n\n%3").arg(
        QCoreApplication::applicationName(),
        QCoreApplication::applicationVersion(),
        QCoreApplication::tr(PGM_DESCRIPTION)));
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();
    QCommandLineOption portableOption({"p", "portable"}, QCoreApplication::tr("Portable settings mode."));
    QCommandLineOption portableFileName({"f", "file"}, QCoreApplication::tr("Portable settings file name."), "portableFile");
    parser.addOption(portableOption);
    parser.addOption(portableFileName);
    QCommandLineOption driverOption({"d", "driver"}, QCoreApplication::tr("MIDI Out Driver."), "driver");
    parser.addOption(driverOption);
    QCommandLineOption portOption({"c", "connection"}, QCoreApplication::tr("MIDI Out Connection."), "connection");
    parser.addOption(portOption);
    parser.addPositionalArgument("file", QCoreApplication::tr("Input SMF/KAR/WRK file name."), "[file]");
    parser.process(app);

    QScopedPointer<QSplashScreen> pSplash;
    if (app.platformName() != "wayland") {
        QPixmap px(":/splash.png");
        QSize newsize = px.size() * app.primaryScreen()->devicePixelRatio();
        px = px.scaled(newsize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        pSplash.reset(new QSplashScreen(px));
        QFont sf = QApplication::font();
        sf.setPointSize(20);
        pSplash->setFont(sf);
        pSplash->show();
        app.processEvents();
        pSplash->showMessage(QSTR_APPNAME + " v" + strVersion,
                             Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
        app.processEvents();
    }
    if (parser.isSet(portableOption) || parser.isSet(portableFileName)) {
        QString portableFile;
        if (parser.isSet(portableFileName)) {
            portableFile = parser.value(portableFileName);
        }
        Settings::setPortableConfig(portableFile);
    } else {
        QSettings::setDefaultFormat(QSettings::NativeFormat);
    }

    QStringList fileNames, positionalArgs = parser.positionalArguments();
    foreach(const QString& a, positionalArgs) {
        QFileInfo f(a);
        if (f.exists())
            fileNames += f.canonicalFilePath();
        else
            qWarning() << QCoreApplication::tr("File not found:") << a;
    }

    try {
        GUIPlayer w;
        if (!pSplash.isNull()) {
            pSplash->finish(&w);
        }
        w.setAttribute(Qt::WA_QuitOnClose);
        if(parser.isSet(portOption) && parser.isSet(driverOption)) {
            w.connectOutput(parser.value(driverOption), parser.value(portOption));
        }
        if (!fileNames.isEmpty()) {
            w.openFileList(fileNames);
        }
        w.show();
        return app.exec();
    } catch (...) {
        QMessageBox::critical(0, "Error", QCoreApplication::tr(errorstr) );
        qWarning() << QCoreApplication::tr(errorstr);
    }
    return 0;
}
