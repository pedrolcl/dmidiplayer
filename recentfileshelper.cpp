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

#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include <drumstick/settingsfactory.h>
#include "recentfileshelper.h"

RecentFilesHelper::RecentFilesHelper(QObject *parent)
	: QObject(parent)
{
	QMenu* menu = qobject_cast<QMenu *>(parent);
	if (menu) {
		for (int i = 0; i < MaxRecentFiles; ++i) {
			recentFileActs[i] = new QAction(this);
			recentFileActs[i]->setVisible(false);
			menu->addAction(recentFileActs[i]);
            connect(recentFileActs[i], &QAction::triggered, this, &RecentFilesHelper::openRecentFile);
		}
		menu->addSeparator();
		clearAct = new QAction(tr("Clear recent files"), this);
		menu->addAction(clearAct);
        connect(clearAct, &QAction::triggered, this, &RecentFilesHelper::clear);
	}
}

void RecentFilesHelper::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit selectedFile(action->data().toString());
    }
}

void RecentFilesHelper::clear()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("RecentFiles");
    settings->setValue("recentFileList", QStringList());
    settings->endGroup();
    updateRecentFileActions();
}

void RecentFilesHelper::setCurrentFile(const QString &fileName)
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("RecentFiles");
    QStringList files = settings->value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings->setValue("recentFileList", files);
    settings->endGroup();
	updateRecentFileActions();
}

void RecentFilesHelper::updateRecentFileActions()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("RecentFiles");
    QStringList files = settings->value("recentFileList").toStringList();
    settings->endGroup();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
        recentFileActs[j]->setVisible(false);
    }
	clearAct->setEnabled(numRecentFiles > 0);
}

QString RecentFilesHelper::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
