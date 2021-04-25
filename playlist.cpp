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

//#include <QDebug>
#include <QListView>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "playlist.h"
#include "settings.h"
#include "iconutils.h"

PlayList::PlayList(QWidget *parent) : QDialog(parent)
{
    setWindowTitle( tr("Manage Playlist") );
    ui.setupUi(this);

    ui.fileList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.fileList->setTextElideMode(Qt::ElideMiddle);

    QPushButton* btnAdd = ui.listButtons->addButton( tr("Add"), QDialogButtonBox::ActionRole );
    btnAdd->setIcon( IconUtils::GetIcon("list-add") );
    connect( btnAdd, &QAbstractButton::clicked, this, &PlayList::addToList );

    QPushButton* btnDel = ui.listButtons->addButton( tr("Remove"), QDialogButtonBox::ActionRole );
    btnDel->setIcon( IconUtils::GetIcon("list-remove") );
    connect( btnDel, &QAbstractButton::clicked, this, &PlayList::removeFromList );

    QPushButton* btnUp  = ui.listButtons->addButton( tr("Move Up"), QDialogButtonBox::ActionRole );
    btnUp->setIcon( IconUtils::GetIcon("go-up") );
    connect( btnUp, &QAbstractButton::clicked, this, &PlayList::moveUp );

    QPushButton* btnDwn = ui.listButtons->addButton( tr("Move Down"), QDialogButtonBox::ActionRole );
    btnDwn->setIcon( IconUtils::GetIcon("go-down") );
    connect( btnDwn, &QAbstractButton::clicked, this, &PlayList::moveDown );

    QPushButton* btnClear = ui.listButtons->addButton( tr("Clear"), QDialogButtonBox::ActionRole );
    btnClear->setIcon( IconUtils::GetIcon("edit-clear") );
    connect( btnClear, &QAbstractButton::clicked, this, &PlayList::clear );

    QPushButton* btnOpen = ui.dialogButtons->addButton( tr("Open"), QDialogButtonBox::ActionRole );
    btnOpen->setIcon( IconUtils::GetIcon("document-open") );
    connect( btnOpen, &QAbstractButton::clicked, this, &PlayList::selectFile );

    QPushButton* btnSave = ui.dialogButtons->addButton( tr("Save As"), QDialogButtonBox::ActionRole );
    btnSave->setIcon( IconUtils::GetIcon("document-save") );
    connect( btnSave, &QAbstractButton::clicked, this, &PlayList::saveFile );

    connect( ui.fileList, &QListWidget::itemClicked, this, &PlayList::itemClicked );
}

PlayList::~PlayList()
{ }

void PlayList::loadPlayList(const QString &fileName)
{
    QFile file(fileName);
    QFileInfo listInfo(file);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        while (!line.isNull()) {
            QFileInfo f(listInfo.absolutePath(), line);
            if (isSupported(f.absoluteFilePath())) {
                ui.fileList->addItem(f.absoluteFilePath());
            }
            line = in.readLine();
        }
        file.close();
        Settings::instance()->setLastPlayList(fileName);
    }
}

bool
PlayList::isSupported(const QString& fileName)
{
    return  fileName.endsWith(".mid", Qt::CaseInsensitive) ||
            fileName.endsWith(".midi", Qt::CaseInsensitive) ||
            fileName.endsWith(".kar", Qt::CaseInsensitive) ||
            fileName.endsWith(".wrk", Qt::CaseInsensitive);
}

void
PlayList::selectFile()
{
    QString fName = QFileDialog::getOpenFileName( this,
       tr("Open Playlist file"), Settings::instance()->lastDirectory(),
       tr("Playlist (*.lst)") );
    if (!fName.isEmpty()) {
        loadPlayList(fName);
    }
}

void PlayList::saveFile()
{
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Playlist (*.lst)"));
    dlg.setDirectory(Settings::instance()->lastDirectory());
    dlg.setWindowTitle(tr("Save Playlist file"));
    dlg.setDefaultSuffix("lst");
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (dlg.exec() == QDialog::Accepted) {
        auto selected = dlg.selectedFiles();
        if (!selected.isEmpty()) {
            QFile file(selected.first());
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;
            QTextStream out(&file);
            foreach(const QString& line, items()) {
                out << line << endl;
            }
            file.close();
            Settings::instance()->setLastPlayList(selected.first());
        }
    }
}

void
PlayList::itemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item)
}

void
PlayList::addToList()
{
    QStringList fNames = QFileDialog::getOpenFileNames( this,
       tr("Add MIDI File"), Settings::instance()->lastDirectory(),
       tr("All files (*.kar *.mid *.midi *.wrk);;"
       "Karaoke files (*.kar);;"
       "MIDI Files (*.mid *.midi);;"
       "Cakewalk files (*.wrk)") );
    if (!fNames.isEmpty()) {
        foreach(QString fName, fNames) {
            if (isSupported(fName)) {
                ui.fileList->addItem(fName);
            }
        }
    }
}

void
PlayList::removeFromList()
{
    ui.fileList->takeItem(ui.fileList->currentRow());
}

void
PlayList::moveUp()
{
    int currentRow = ui.fileList->currentRow();
    if (currentRow > 0) {
        QListWidgetItem* prev = ui.fileList->takeItem(currentRow - 1);
        ui.fileList->insertItem( currentRow, prev );
    }
}

void
PlayList::moveDown()
{
    int currentRow = ui.fileList->currentRow();
    if (currentRow < ui.fileList->count()) {
        QListWidgetItem* next = ui.fileList->takeItem(currentRow + 1);
        ui.fileList->insertItem( currentRow, next );
    }
}

/*
 *	public API
 *	----------
 */

void
PlayList::clear()
{
    ui.fileList->clear();
}

QStringList
PlayList::items()
{
    QStringList tmp;
    for(int i=0; i<ui.fileList->count(); ++i)
    {
        tmp << ui.fileList->item(i)->text();
    }
    return tmp;
}

void
PlayList::setItems(QStringList items)
{
    ui.fileList->clear();
    ui.fileList->addItems(items);
}

QString
PlayList::currentItem()
{
    if (ui.fileList->currentItem() != nullptr) {
        //qDebug() << Q_FUNC_INFO << ui.fileList->currentItem()->text();
        return ui.fileList->currentItem()->text();
    }
    return QString();
}

void
PlayList::setCurrentItem(QString item)
{
    QList<QListWidgetItem *> matches = ui.fileList->findItems(item, Qt::MatchExactly);
    if (matches.count() > 0)
        ui.fileList->setCurrentItem(matches[0]);
}

bool
PlayList::selectNextItem()
{
    int current = ui.fileList->currentRow();
    if (current < ui.fileList->count()) {
        ui.fileList->setCurrentRow( ++current );
        //qDebug() << Q_FUNC_INFO << ui.fileList->currentRow();
        return true;
    }
    return false;
}

bool
PlayList::selectPrevItem()
{
    int current = ui.fileList->currentRow();
    if (current > 0) {
        ui.fileList->setCurrentRow( --current );
        //qDebug() << Q_FUNC_INFO << ui.fileList->currentRow();
        return true;
    }
    return false;
}

int
PlayList::itemCount()
{
    return ui.fileList->count();
}

bool
PlayList::atLastItem()
{
    //qDebug() << Q_FUNC_INFO << ui.fileList->currentRow();
    return ui.fileList->currentRow() == (ui.fileList->count() - 1);
}

bool
PlayList::atFirstItem()
{
    //qDebug() << Q_FUNC_INFO << ui.fileList->currentRow();
    return ui.fileList->currentRow() < 1;
}

bool
PlayList::noItems()
{
    //qDebug() << Q_FUNC_INFO << ui.fileList->count();
    return ui.fileList->count() == 0;
}
