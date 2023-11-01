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

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QCloseEvent>
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QTextEdit>
#include <QFrame>
#include <QFontDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextCodec>
#include <QScrollBar>
#include <QToolBar>
#include <QClipboard>
#include <QSaveFile>
#include <QPrintDialog>
#include <QPrinter>
#include <QRegularExpression>
#include <QTextStream>
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

#include <drumstick/settingsfactory.h>
#include "settings.h"
#include "iconutils.h"
#include "lyrics.h"
#include "sequence.h"

Lyrics::Lyrics(QWidget *parent) : FramelessWindow(parent),
    m_track(0),
    m_mib(0),
    m_type(0),
    m_song(nullptr),
    m_codec(nullptr)
{
    setObjectName(QString::fromUtf8("Lyrics"));
    setContextMenuPolicy(Qt::CustomContextMenu); // prevent default ctx
    QToolBar* tbar = new QToolBar(this);
    tbar->setObjectName("toolbar");
    tbar->setMovable(false);
    tbar->setFloatable(false);
    tbar->setIconSize(QSize(22,22));
    addToolBar(tbar);
    setPseudoCaption(tbar);
    m_actionCopy = new QAction(this);
    m_actionCopy->setObjectName(QString::fromUtf8("actionCopy"));
    m_actionSave = new QAction(this);
    m_actionSave->setObjectName(QString::fromUtf8("actionSave"));
    m_actionPrint = new QAction(this);
    m_actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
    m_label1 = new QLabel(this);
    m_label1->setObjectName(QString::fromUtf8("label1"));
    tbar->addWidget(m_label1);
    m_comboTrack = new QComboBox(this);
    m_comboTrack->setObjectName(QString::fromUtf8("comboTrack"));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_comboTrack->sizePolicy().hasHeightForWidth());
    m_comboTrack->setSizePolicy(sizePolicy);
    tbar->addWidget(m_comboTrack);
    m_label2 = new QLabel(this);
    m_label2->setObjectName(QString::fromUtf8("label2"));
    tbar->addWidget(m_label2);
    m_comboType = new QComboBox(this);
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->addItem(QString());
    m_comboType->setObjectName(QString::fromUtf8("comboType"));
    sizePolicy.setHeightForWidth(m_comboType->sizePolicy().hasHeightForWidth());
    m_comboType->setSizePolicy(sizePolicy);
    tbar->addWidget(m_comboType);
    m_label3 = new QLabel(this);
    m_label3->setObjectName(QString::fromUtf8("label3"));
    tbar->addWidget(m_label3);
    m_comboCodec = new QComboBox(this);
    m_comboCodec->setObjectName(QString::fromUtf8("comboCodec"));
    sizePolicy.setHeightForWidth(m_comboCodec->sizePolicy().hasHeightForWidth());
    m_comboCodec->setSizePolicy(sizePolicy);
    m_comboCodec->setMaxVisibleItems(10);
    m_comboCodec->setStyleSheet("combobox-popup: 0;");
    tbar->addWidget(m_comboCodec);
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    tbar->addWidget(spacer);
    m_chmenu = new QMenu(this);
    m_chmenu->addAction(m_actionCopy);
    m_chmenu->addAction(m_actionSave);
    m_chmenu->addAction(m_actionPrint);
    m_actionFullScreen = new QAction(this); // Full Screen
    m_actionFullScreen->setShortcut(QKeySequence::FullScreen);
    m_chmenu->addAction(m_actionFullScreen);
    m_actionFont = new QAction(this); // Font dialog
    m_chmenu->addAction(m_actionFont);
    m_toolButton = new QToolButton(this);
    m_toolButton->setObjectName(QString::fromUtf8("toolButton"));
    m_toolButton->setMenu(m_chmenu);
    m_toolButton->setPopupMode(QToolButton::InstantPopup);
    m_toolButton->setIcon(IconUtils::GetIcon("application-menu"));
    tbar->addWidget(m_toolButton);
    auto closeBtn = new QToolButton(this);
    closeBtn->setIcon(IconUtils::GetIcon("window-close"));
    connect(closeBtn, &QToolButton::clicked, this, &Lyrics::close);
    tbar->addWidget(closeBtn);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(5,5,5,5);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(vlayout);
    m_textViewer = new QTextEdit(this);
    m_textViewer->setObjectName(QString::fromUtf8("textViewer"));
    m_textViewer->setFont(Settings::instance()->lyricsFont());
    m_textViewer->setReadOnly(true);
    m_textViewer->setTextInteractionFlags(Qt::NoTextInteraction);
    m_normalColor = Settings::instance()->getFutureColor();
    m_otherColor = Settings::instance()->getPastColor();
    vlayout->addWidget(m_textViewer);
    this->setCentralWidget(centralWidget);
#ifndef QT_NO_SHORTCUT
    m_label1->setBuddy(m_comboTrack);
    m_label2->setBuddy(m_comboType);
    m_label3->setBuddy(m_comboCodec);
#endif // QT_NO_SHORTCUT
    // Populate the codecs combobox
    populateCodecsCombo();
    // connect combos
    connect(m_comboTrack, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::trackChanged);
    connect(m_comboType, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::typeChanged);
    connect(m_comboCodec, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::codecChanged);
    connect(m_actionFullScreen, &QAction::triggered, this, &Lyrics::toggleFullScreen);
    connect(m_actionFont, &QAction::triggered, this, &Lyrics::changeFont);
    connect(m_actionCopy, &QAction::triggered, this, &Lyrics::slotCopy);
    connect(m_actionSave, &QAction::triggered, this, &Lyrics::slotSave);
    connect(m_actionPrint, &QAction::triggered, this, &Lyrics::slotPrint);
    retranslateUi();
    emit sizeAdjustNeeded();
}

void Lyrics::readSettings()
{
    const QByteArray geometry = Settings::instance()->lyricsWindowGeometry();
    const QByteArray state = Settings::instance()->lyricsWindowState();

    if (geometry.isEmpty()) {
        const QRect availableGeometry =
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
                QApplication::desktop()->availableGeometry(parentWidget());
#else
                parentWidget()->screen()->availableGeometry();
#endif
        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        size(), availableGeometry));
    } else {
        restoreGeometry(geometry);
    }
    if (!state.isEmpty()) {
        restoreState(state);
    }
}

void Lyrics::writeSettings()
{
    Settings::instance()->setLyricsWindowGeometry(saveGeometry());
    Settings::instance()->setLyricsWindowState(saveState());
}

void Lyrics::retranslateUi()
{
    m_label1->setText(QApplication::translate("Lyrics", "Track:", nullptr));
    m_label2->setText(QApplication::translate("Lyrics", "Type:", nullptr));
    m_comboType->setItemText(0, QApplication::translate("Lyrics", "All Types", nullptr));
    m_comboType->setItemText(1, QApplication::translate("Lyrics", "Texts", nullptr));
    m_comboType->setItemText(2, QApplication::translate("Lyrics", "Copyright Notice", nullptr));
    m_comboType->setItemText(3, QApplication::translate("Lyrics", "Sequence Name", nullptr));
    m_comboType->setItemText(4, QApplication::translate("Lyrics", "Instrument Name", nullptr));
    m_comboType->setItemText(5, QApplication::translate("Lyrics", "Lyric Events", nullptr));
    m_comboType->setItemText(6, QApplication::translate("Lyrics", "Marker", nullptr));
    m_comboType->setItemText(7, QApplication::translate("Lyrics", "Cue Point", nullptr));

    m_label3->setText(QApplication::translate("Lyrics", "Encoding:", nullptr));
    m_actionFont->setText(QApplication::translate("Lyrics", "Font...", nullptr));
    m_actionFullScreen->setText(QApplication::translate("Lyrics", "Full Screen", nullptr));
    m_actionCopy->setText(QApplication::translate("Lyrics", "Copy to clipboard", nullptr));
    m_actionSave->setText(QApplication::translate("Lyrics", "Save to file...", nullptr));
    m_actionPrint->setText(QApplication::translate("Lyrics", "Print...", nullptr));
}

void Lyrics::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    std::call_once(m_firstTime, &Lyrics::readSettings, this);
}

void Lyrics::closeEvent(QCloseEvent *event)
{
    writeSettings();
    emit closed();
    event->accept();
}

void Lyrics::populateCodecsCombo()
{
    QMap<QString,int> aux;
    foreach(const auto mib, QTextCodec::availableMibs()) {
        QTextCodec *c = QTextCodec::codecForMib(mib);
        if (c != nullptr && mib != 0) {
            aux.insert(c->name(), mib);
        }
    }
    QByteArrayList umibkeys = Sequence::getExtraCodecNames();
    foreach(const auto k, umibkeys) {
        auto mib = Sequence::getMibForName(k);
        if (!aux.contains(k) || aux.key(mib).isEmpty())
        {
            QTextCodec *c = QTextCodec::codecForMib(mib);
            if (c != nullptr)
            {
                aux.insert(k, mib);
                //qDebug() << "adding extra codec:" << k << mib;
            }
        }
    }
    QStringList keys = aux.keys();
    keys.sort();
    keys.removeDuplicates();
    m_comboCodec->clear();
    m_comboCodec->addItem(tr("Default (latin1)"), 0);
    foreach(const auto k, keys) {
        m_comboCodec->addItem(k, aux[k]);
    }
    //qDebug() << Q_FUNC_INFO << m_comboCodec->count();
}

void Lyrics::populateTracksCombo()
{
    m_comboTrack->clear();
    m_comboTrack->addItem(tr("All Tracks"));
    if (m_song != nullptr) {
        for(int track = 1; track <= m_song->getNumTracks(); ++track) {
            QString name = sanitizeText( m_song->trackName(track) );
            if (name.isEmpty()) {
                m_comboTrack->addItem(tr("Track %1").arg(track));
            } else {
                m_comboTrack->addItem(QString::number(track) + ": " + name);
            }
        }
    }
}

void Lyrics::displayText()
{
    m_textViewer->clear();
    m_textViewer->setTextColor(m_normalColor);
    m_textPos.clear();
    if (m_song != nullptr) {
        QList<QPair<int,QByteArray>> textList = m_song->getRawText(m_track, static_cast<Sequence::TextType>(m_type));
        foreach(const auto& p, textList) {
            auto cp = m_textViewer->textCursor().position();
            auto s = sanitizeText(p.second);
            if (!m_textPos.contains(p.first)) {
                m_textPos.insert(p.first, cp);
            }
            m_textViewer->insertPlainText(s);
        }
        m_textViewer->moveCursor(QTextCursor::Start);
    }
}

void Lyrics::toggleFullScreen(bool /*enabled*/)
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void Lyrics::slotCopy()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(m_textViewer->toPlainText());
}

void Lyrics::slotSave()
{
    QFileInfo info(m_song->currentFile());
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("txt");
    dialog.setNameFilters({"Text files (*.txt)","Any files (*)"});
    dialog.selectFile(info.baseName() + ".txt");
    if (dialog.exec() == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        QSaveFile file(fileNames.first());
        if (file.open(QFile::WriteOnly | QFile::Text)) {
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
            QTextStream out(&file);
            out.setCodec(m_codec);
            out << m_textViewer->toPlainText();
#else
            QByteArray encoded = m_codec->fromUnicode(m_textViewer->toPlainText());
            file.write(encoded);
#endif
        }
        file.commit();
    }
}

void Lyrics::slotPrint()
{
    QPrinter printer(QPrinter::HighResolution);
#if defined(Q_OS_LINUX)
    QFileInfo info(m_song->currentFile());
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(info.baseName() + ".pdf");
#endif
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument doc(m_textViewer->toPlainText());
        doc.print(&printer);
    }
}

void Lyrics::initSong( Sequence *song )
{
    //qDebug() << Q_FUNC_INFO;
    m_song = song;
    if (m_song != nullptr) {
        populateTracksCombo();
        m_mib = m_song->currentMIB();
        m_track = m_song->trackMaxPoints();
        m_type = m_song->typeMaxPoints();
    }
    m_textViewer->clear();
    // selected Codec:
    int idx = m_comboCodec->findData(m_mib, Qt::UserRole, Qt::MatchExactly|Qt::MatchWrap);
    m_comboCodec->setCurrentIndex(idx);
    m_codec = QTextCodec::codecForMib(m_mib);
    if (idx < 0 && m_codec != nullptr) {
        idx = m_comboCodec->findText(m_codec->name(), Qt::MatchFixedString|Qt::MatchWrap);
        m_comboCodec->setCurrentIndex(idx);
    }
    //qDebug() << Q_FUNC_INFO << "mib:" << m_mib << "combo index:" << idx << "codec:" << (m_codec == nullptr ? "null" : m_codec->name());
    m_comboTrack->setCurrentIndex(m_track);
    m_comboType->setCurrentIndex(m_type);
    // populate text browser:
    displayText();
}

void Lyrics::applySettings()
{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    m_chmenu->setPalette(qApp->palette());
#endif
    m_toolButton->setIcon(IconUtils::GetIcon("application-menu"));
    m_textViewer->setFont(Settings::instance()->lyricsFont());
    m_normalColor = Settings::instance()->getFutureColor();
    m_otherColor = Settings::instance()->getPastColor();
    if (!m_textViewer->document()->isEmpty()) {
        displayText();
    }
    foreach(QComboBox *cb, findChildren<QComboBox*>()) {
        cb->setPalette(qApp->palette());
        foreach(QWidget *w, cb->findChildren<QWidget*>()) {
            w->setPalette(qApp->palette());
        }
    }
	FramelessWindow::applySettings();
}

QString Lyrics::sanitizeText(const QByteArray& data)
{
    QString s;
    if (m_codec == nullptr) {
        s = QString::fromLatin1(data);
    } else {
        s = m_codec->toUnicode(data);
    }
    s.replace(QRegularExpression("@[IKLTVW]"), "\n");
    s.replace(QRegularExpression("[/\\\\]+"), "\n");
    s.replace(QRegularExpression("[\r\n]+"), "\n");
    s.replace('\0', QChar::Space);
    return s;
}

void Lyrics::slotMidiText(const int track, const int type, int ticks, const QByteArray &data)
{
    if ((m_track == 0 || m_track == track) &&
        (m_type == 0 || m_type == type)) {
        QString stext = sanitizeText(data).trimmed();
        if (stext.isEmpty()) {
            return;
        }
        if (m_textPos.contains(ticks)) {
            //qDebug() << ticks << m_textPos[ticks] << stext;
            QTextCursor cursor = m_textViewer->textCursor();
            cursor.setPosition(m_textPos[ticks]);
            m_textViewer->setTextCursor(cursor);
        }
        if (m_textViewer->find(stext)) {
            m_textViewer->setTextColor(m_otherColor);
            QRect r = m_textViewer->cursorRect();
            QScrollBar *s = m_textViewer->verticalScrollBar();
            int half = m_textViewer->viewport()->height() / 2;
            int newpos = s->value() + r.top() - half;
            if ((r.top() > half) && (newpos < s->maximum())) {
                s->setValue(newpos);
            }
        } /*else {
            qDebug() << Q_FUNC_INFO << "not found:" << stext;
        }*/
    }
}

void Lyrics::trackChanged(int index)
{
    m_track = index;
    //qDebug() << "track:" << m_track;
    displayText();
}

void Lyrics::typeChanged(int index)
{
    m_type = index;
    //qDebug() << "type:" <<  m_type;
    displayText();
}

void Lyrics::codecChanged(int index)
{
    m_mib = m_comboCodec->itemData(index).toInt();
    //qDebug() << "mib:" << m_mib;
    m_codec = QTextCodec::codecForMib(m_mib);
    m_song->setCodec(m_codec);
    displayText();
}

void Lyrics::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_textViewer->font(), this);
    if (ok) {
        m_textViewer->setFont(font);
        Settings::instance()->setLyricsFont(font);
        displayText();
    }
}
