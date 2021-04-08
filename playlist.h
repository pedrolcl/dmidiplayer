/*
    Drumstick MIDI/Karaoke Player
    Copyright (C) 2009-2010 Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef PLAYLISTDIALOG_H
#define PLAYLISTDIALOG_H

#include <QDialog>
#include "ui_playlist.h"

class PlayList : public QDialog {
    Q_OBJECT
public:
    PlayList(QWidget *parent = 0);
    virtual ~PlayList();

    QString currentItem();
    QStringList items();
    int itemCount();
    bool isSupported(const QString& fileName);

public slots:
    void clear();
    void setItems(QStringList);
    void setCurrentItem(QString);

private slots:
    void selectFile();
    void saveFile();
    void addToList();
    void removeFromList();
    void moveUp();
    void moveDown();
    void itemClicked ( QListWidgetItem * item );

private:
    Ui::PlayList ui;
};

#endif /* PLAYLISTDIALOG_H */
