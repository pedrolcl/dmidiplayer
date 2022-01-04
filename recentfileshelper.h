/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef RECENTFILESHELPER_H
#define RECENTFILESHELPER_H

#include <QObject>
#include <QAction>
#include <QString>

class RecentFilesHelper : public QObject
{
	Q_OBJECT
public:
	RecentFilesHelper(QObject *parent);
	~RecentFilesHelper() {}

	void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    QStringList files();
    void retranslateUi();

Q_SIGNALS:
	void selectedFile(QString fileName);

private Q_SLOTS:
	void openRecentFile();
	void clear();

private:
	static const int MaxRecentFiles = 10;
    QAction *recentFileActs[MaxRecentFiles];
	QAction *clearAct;
};

#endif // RECENTFILESHELPER_H
