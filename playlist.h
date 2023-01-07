/*
    Drumstick MIDI/Karaoke Player
    Copyright (C) 2009-2023 Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef PLAYLISTDIALOG_H
#define PLAYLISTDIALOG_H

#include <QDialog>
#include "ui_playlist.h"

class PlayList : public QDialog {
    Q_OBJECT
public:
    PlayList(QWidget *parent = 0);
    virtual ~PlayList();

    void loadPlayList(const QString& fileName);
    void clearPlayList();
    QString currentItem();
    QStringList items();
    int itemCount();
    bool isSupported(const QString& fileName);
    bool selectFirstItem();
    bool selectNextItem();
    bool selectPrevItem();
    bool atLastItem();
    bool atFirstItem();
    bool noItems();
    void retranslateUi();
    bool isDirty();

public slots:
    void clear();
    void shuffle();
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
    void updateCaption();

private:
    QPushButton* m_btnAdd;
    QPushButton* m_btnDel;
    QPushButton* m_btnUp;
    QPushButton* m_btnDwn;
    QPushButton* m_btnClear;
    QPushButton* m_btnShuffle;
    QPushButton* m_btnOpen;
    QPushButton* m_btnSave;
    Ui::PlayList ui;
    bool m_isDirty;
};

#endif /* PLAYLISTDIALOG_H */
