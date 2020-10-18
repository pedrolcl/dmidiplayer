/*
    Drumstick MIDI File player graphic program
    Copyright (C) 2006-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <drumstick/backendmanager.h>

#include "guiplayer.h"

using namespace drumstick::rt;


const QString errorstr = "Fatal error from the Operating System. "
    "This usually happens when the OS doesn't have MIDI support, "
    "or the MIDI support is not enabled. "
    "Please check your OS/MIDI configuration.";

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
    QCoreApplication::setApplicationVersion(QT_STRINGIFY(VERSION));
    QApplication app(argc, argv);

    try {
        GUIPlayer w;
        w.setAttribute(Qt::WA_QuitOnClose);
        w.show();
        return app.exec();
    } catch (...) {
        QMessageBox::critical(0, "Error", errorstr );
        qWarning() << errorstr;
    }
    return 0;
}
