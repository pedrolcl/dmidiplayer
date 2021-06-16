/*
    Drumstick MIDI/Karaoke Player
    Copyright (C) 2009-2021 Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <algorithm>
#include <random>

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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#define endl Qt::endl
#endif

PlayList::PlayList(QWidget *parent) : QDialog(parent),
    m_isDirty(false)
{
    setWindowTitle( tr("Manage Playlist") );
    ui.setupUi(this);

    ui.fileList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.fileList->setTextElideMode(Qt::ElideMiddle);

    m_btnAdd = ui.listButtons->addButton( tr("Add"), QDialogButtonBox::ActionRole );
    m_btnAdd->setIcon( IconUtils::GetIcon("list-add") );
    connect( m_btnAdd, &QAbstractButton::clicked, this, &PlayList::addToList );

    m_btnDel = ui.listButtons->addButton( tr("Remove"), QDialogButtonBox::ActionRole );
    m_btnDel->setIcon( IconUtils::GetIcon("list-remove") );
    connect( m_btnDel, &QAbstractButton::clicked, this, &PlayList::removeFromList );

    m_btnUp  = ui.listButtons->addButton( tr("Move Up"), QDialogButtonBox::ActionRole );
    m_btnUp->setIcon( IconUtils::GetIcon("go-up") );
    connect( m_btnUp, &QAbstractButton::clicked, this, &PlayList::moveUp );

    m_btnDwn = ui.listButtons->addButton( tr("Move Down"), QDialogButtonBox::ActionRole );
    m_btnDwn->setIcon( IconUtils::GetIcon("go-down") );
    connect( m_btnDwn, &QAbstractButton::clicked, this, &PlayList::moveDown );

    m_btnShuffle = ui.listButtons->addButton( tr("Shuffle"), QDialogButtonBox::ActionRole );
    m_btnShuffle->setIcon( IconUtils::GetIcon("media-playlist-shuffle") );
    connect( m_btnShuffle, &QAbstractButton::clicked, this, &PlayList::shuffle );

    m_btnClear = ui.listButtons->addButton( tr("Clear"), QDialogButtonBox::ActionRole );
    m_btnClear->setIcon( IconUtils::GetIcon("edit-clear") );
    connect( m_btnClear, &QAbstractButton::clicked, this, &PlayList::clear );

    m_btnOpen = ui.dialogButtons->addButton( tr("Open"), QDialogButtonBox::ActionRole );
    m_btnOpen->setIcon( IconUtils::GetIcon("document-open") );
    connect( m_btnOpen, &QAbstractButton::clicked, this, &PlayList::selectFile );

    m_btnSave = ui.dialogButtons->addButton( tr("Save As"), QDialogButtonBox::ActionRole );
    m_btnSave->setIcon( IconUtils::GetIcon("document-save") );
    connect( m_btnSave, &QAbstractButton::clicked, this, &PlayList::saveFile );

    connect( ui.fileList, &QListWidget::itemClicked, this, &PlayList::itemClicked );
}

PlayList::~PlayList()
{ }

void PlayList::loadPlayList(const QString &fileName)
{
    QFile file(fileName);
    QFileInfo listInfo(file);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui.fileList->clear();
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
        m_isDirty=false;
        updateCaption();
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
            out.flush();
            file.close();
            Settings::instance()->setLastPlayList(selected.first());
            m_isDirty=false;
            updateCaption();
        }
    }
}

void
PlayList::itemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item)
}

void PlayList::updateCaption()
{
    QFileInfo info(Settings::instance()->lastPlayList());
    QString caption = tr("Manage Playlist: %1 %2").arg(info.fileName(), m_isDirty?"(*)":QString());
    //qDebug() << Q_FUNC_INFO << caption;
    setWindowTitle(caption);
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
                m_isDirty=true;
            }
        }
        updateCaption();
    }
}

void
PlayList::removeFromList()
{
    ui.fileList->takeItem(ui.fileList->currentRow());
    m_isDirty=true;
    updateCaption();
}

void
PlayList::moveUp()
{
    int currentRow = ui.fileList->currentRow();
    if (currentRow > 0) {
        QListWidgetItem* prev = ui.fileList->takeItem(currentRow - 1);
        ui.fileList->insertItem( currentRow, prev );
        m_isDirty=true;
        updateCaption();
    }
}

void
PlayList::moveDown()
{
    int currentRow = ui.fileList->currentRow();
    if ((currentRow > -1) && (currentRow < ui.fileList->count())) {
        QListWidgetItem* next = ui.fileList->takeItem(currentRow + 1);
        ui.fileList->insertItem( currentRow, next );
        m_isDirty=true;
        updateCaption();
    }
}

/*
 *	public API
 *	----------
 */

void
PlayList::clear()
{
    if(ui.fileList->count() > 0) {
        ui.fileList->clear();
        m_isDirty=true;
        updateCaption();
    }
}

void PlayList::shuffle()
{
    static std::random_device rnd;
    static auto rng = std::default_random_engine{rnd()};
    QStringList names = items();
    std::shuffle(names.begin(), names.end(), rng);
    setItems(names);
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
    m_isDirty = true;
    updateCaption();
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

void PlayList::retranslateUi()
{
    ui.retranslateUi(this);
    //setWindowTitle(tr("Manage Playlist"));
    m_btnAdd->setText(tr("Add"));
    m_btnDel->setText(tr("Remove"));
    m_btnUp->setText(tr("Move Up"));
    m_btnDwn->setText(tr("Move Down"));
    m_btnClear->setText(tr("Clear"));
    m_btnOpen->setText(tr("Open"));
    m_btnSave->setText(tr("Save As"));
    updateCaption();
}

bool PlayList::isDirty()
{
    return m_isDirty;
}
