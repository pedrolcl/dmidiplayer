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
#include <QListView>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

#include "playlist.h"

PlayList::PlayList(QWidget *parent) : QDialog(parent)
{
    setWindowTitle( tr("Manage Playlist") );
    ui.setupUi(this);

    QPushButton* btnAdd = ui.listButtons->addButton( tr("Add"), QDialogButtonBox::NoRole );
    btnAdd->setIcon( QIcon(":/resources/list-add.png") );
    connect( btnAdd, &QAbstractButton::clicked, this, &PlayList::addToList );

    QPushButton* btnDel = ui.listButtons->addButton( tr("Remove"), QDialogButtonBox::NoRole );
    btnDel->setIcon( QIcon(":/resources/list-remove.png") );
    connect( btnDel, &QAbstractButton::clicked, this, &PlayList::removeFromList );

    QPushButton* btnUp  = ui.listButtons->addButton( tr("Move Up"), QDialogButtonBox::NoRole );
    btnUp->setIcon( QIcon(":/resources/go-up.png") );
    connect( btnUp, &QAbstractButton::clicked, this, &PlayList::moveUp );

    QPushButton* btnDwn = ui.listButtons->addButton( tr("Move Down"), QDialogButtonBox::NoRole );
    btnDwn->setIcon( QIcon(":/resources/go-down.png") );
    connect( btnDwn, &QAbstractButton::clicked, this, &PlayList::moveDown );

    QPushButton* btnClear = ui.listButtons->addButton( tr("Clear"), QDialogButtonBox::NoRole );
    btnClear->setIcon( QIcon(":/resources/edit-clear.png") );
    connect( btnClear, &QAbstractButton::clicked, this, &PlayList::clear );

    ui.fileButton->setIcon( QIcon(":/resources/open.png") );
    connect( ui.fileButton, &QAbstractButton::clicked, this, &PlayList::selectFile );

    connect( ui.fileList, &QListWidget::itemClicked, this, &PlayList::itemClicked );
}

PlayList::~PlayList()
{ }

void
PlayList::selectFile()
{
     QString fName = QFileDialog::getOpenFileName( this,
        tr("Add MIDI/Karaoke File"), QDir::homePath(),
        tr("MIDI files (*.mid;*.midi;*.kar;*.wrk)") );
     if (!fName.isEmpty()) {
        ui.fileEdit->setText(fName);
     }
}

void
PlayList::itemClicked(QListWidgetItem* item)
{
    ui.fileEdit->setText(item->text());
}

void
PlayList::addToList()
{
    QString fName = ui.fileEdit->text();
    if (!fName.isEmpty()) {
        ui.fileList->addItem(fName);
        ui.fileEdit->clear();
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
    if (ui.fileList->currentItem() != 0)
        return ui.fileList->currentItem()->text();
    return QString();
}

void
PlayList::setCurrentItem(QString item)
{
    QList<QListWidgetItem *> matches = ui.fileList->findItems(item, Qt::MatchExactly);
    if (matches.count() > 0)
        ui.fileList->setCurrentItem(matches[0]);
}

int
PlayList::itemCount()
{
    return ui.fileList->count();
}
