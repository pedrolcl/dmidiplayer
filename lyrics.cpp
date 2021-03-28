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

//#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QCloseEvent>
#include <QAction>
#include <QLabel>
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
#include <drumstick/settingsfactory.h>
#include "lyrics.h"
#include "sequence.h"

Lyrics::Lyrics(QWidget *parent) : QMainWindow(parent),
    m_track(0),
    m_mib(0),
    m_type(0),
    m_song(nullptr),
    m_codec(nullptr)
{
    setObjectName(QString::fromUtf8("Lyrics"));
    setWindowFlag(Qt::Tool, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);

    m_actionOpen = new QAction(this);
    m_actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    m_actionQuit = new QAction(this);
    m_actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
    m_actionAbout = new QAction(this);
    m_actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
    m_actionAbout_Qt = new QAction(this);
    m_actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
    m_actionInfo = new QAction(this);
    m_actionInfo->setObjectName(QString::fromUtf8("actionInfo"));
    m_centralwidget = new QWidget(this);
    m_centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    m_gridLayout = new QGridLayout(m_centralwidget);
    m_gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    m_frame = new QFrame(m_centralwidget);
    m_frame->setObjectName(QString::fromUtf8("frame"));
    m_frame->setFrameShape(QFrame::StyledPanel);
    m_frame->setFrameShadow(QFrame::Raised);
    m_horizontalLayout = new QHBoxLayout(m_frame);
    m_horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_label1 = new QLabel(m_frame);
    m_label1->setObjectName(QString::fromUtf8("label1"));

    m_horizontalLayout->addWidget(m_label1);

    m_comboTrack = new QComboBox(m_frame);
    m_comboTrack->setObjectName(QString::fromUtf8("comboTrack"));

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_comboTrack->sizePolicy().hasHeightForWidth());
    m_comboTrack->setSizePolicy(sizePolicy);

    m_horizontalLayout->addWidget(m_comboTrack);

    m_label2 = new QLabel(m_frame);
    m_label2->setObjectName(QString::fromUtf8("label2"));

    m_horizontalLayout->addWidget(m_label2);

    m_comboType = new QComboBox(m_frame);
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

    m_horizontalLayout->addWidget(m_comboType);

    m_label3 = new QLabel(m_frame);
    m_label3->setObjectName(QString::fromUtf8("label3"));

    m_horizontalLayout->addWidget(m_label3);

    m_comboCodec = new QComboBox(m_frame);
    m_comboCodec->setObjectName(QString::fromUtf8("comboCodec"));
    sizePolicy.setHeightForWidth(m_comboCodec->sizePolicy().hasHeightForWidth());
    m_comboCodec->setSizePolicy(sizePolicy);

    m_horizontalLayout->addWidget(m_comboCodec);

    m_label4 = new QLabel(m_frame);
    m_label4->setObjectName(QString::fromUtf8("label4"));

    m_horizontalLayout->addWidget(m_label4);

    m_fontButton = new QToolButton(m_frame);
    m_fontButton->setObjectName(QString::fromUtf8("fontButton"));

    m_horizontalLayout->addWidget(m_fontButton);

    m_gridLayout->addWidget(m_frame, 0, 0, 1, 1);

    m_textViewer = new QTextEdit(m_centralwidget);
    m_textViewer->setObjectName(QString::fromUtf8("textViewer"));
    QFont font;
    font.setFamily(QString::fromUtf8("Sans Serif"));
    font.setPointSize(36);
    m_textViewer->setFont(font);
    m_textViewer->setReadOnly(true);
    m_normalColor = qApp->palette().color(QPalette::BrightText);
    m_otherColor = m_normalColor.darker(300);
    m_gridLayout->addWidget(m_textViewer, 1, 0, 1, 1);

    this->setCentralWidget(m_centralwidget);
#ifndef QT_NO_SHORTCUT
    m_label1->setBuddy(m_comboTrack);
    m_label2->setBuddy(m_comboType);
    m_label3->setBuddy(m_comboCodec);
#endif // QT_NO_SHORTCUT

    setWindowTitle(tr("Lyrics Viewer"));
    // Populate the codecs combobox
    populateCodecsCombo();
    // connect combos
    connect(m_comboTrack, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::trackChanged);
    connect(m_comboType, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::typeChanged);
    connect(m_comboCodec, QOverload<int>::of(&QComboBox::activated), this, &Lyrics::codecChanged);
    connect(m_fontButton, &QToolButton::pressed, this, &Lyrics::changeFont);

    readSettings();
    retranslateUi();
}

void Lyrics::readSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("LyricsWindow");
    restoreGeometry(settings->value("Geometry").toByteArray());
    restoreState(settings->value("State").toByteArray());
    settings->endGroup();
}

void Lyrics::writeSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("LyricsWindow");
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("State", saveState());
    settings->endGroup();
}

void Lyrics::retranslateUi()
{
    setWindowTitle(QApplication::translate("Lyrics", "Lyrics", nullptr));
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
    m_label4->setText(QApplication::translate("Lyrics", "Font:", nullptr));
    m_fontButton->setText(QApplication::translate("Lyrics", "...", nullptr));
} // retranslateUi


void Lyrics::closeEvent(QCloseEvent *event)
{
    writeSettings();
    emit closed();
    event->accept();
}

bool Lyrics::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#if defined(Q_OS_WINDOWS)
    if (m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void Lyrics::populateCodecsCombo()
{
    foreach(const auto m, QTextCodec::availableMibs()) {
        QTextCodec *codec = QTextCodec::codecForMib(m);
        m_comboCodec->addItem(codec->name(), m);
    }
}

void Lyrics::populateTracksCombo()
{
    m_comboTrack->clear();
    m_comboTrack->addItem(tr("All Tracks"));
    for(int track = 1; track <= m_song->getNumTracks(); ++track) {
        QString name = sanitizeText( m_song->trackName(track) );
        if (name.isEmpty()) {
            m_comboTrack->addItem(tr("Track %1").arg(track));
        } else {
            m_comboTrack->addItem(QString::number(track) + ": " + name);
        }
    }
}

void Lyrics::displayText()
{
    m_textViewer->clear();
    m_textViewer->setTextColor(m_normalColor);
    QByteArray text = m_song->getRawText(m_track, static_cast<Sequence::TextType>(m_type));
    QString s = sanitizeText(text);
    //qDebug() << Q_FUNC_INFO << s;
    m_textViewer->setPlainText(s.trimmed());
}

void Lyrics::initSong( Sequence *song )
{
    //qDebug() << Q_FUNC_INFO;
    m_song = song;
    setWindowTitle(tr("Lyrics Viewer (%1)").arg(m_song->currentFile()));
    m_mib = song->detectedUchardetMIB();
    m_track = song->trackMaxPoints();
    m_type = song->typeMaxPoints();
    m_textViewer->clear();
    // selected Codec:
    int idx = m_comboCodec->findData(m_mib);
    m_comboCodec->setCurrentIndex(idx);
    m_codec = QTextCodec::codecForMib(m_mib);
    populateTracksCombo();
    m_comboTrack->setCurrentIndex(m_track);
    m_comboType->setCurrentIndex(m_type);
    // populate text browser:
    displayText();
}

QString Lyrics::sanitizeText(const QByteArray& data)
{
    QString s;
    if (m_codec != nullptr) {
        s = m_codec->toUnicode(data);
        s.replace(QRegExp("@[IKLTVW]"), "\n");
        s.replace(QRegExp("[/\\\\]+"), "\n");
        s.replace(QRegExp("[\r\n]+"), "\n");
        s.replace('\0', QChar::Space);
    }
    return s;
}

void Lyrics::slotMidiText(const int track, const int type, const QByteArray &data)
{
    Q_UNUSED(type)
    //qDebug() << Q_FUNC_INFO << track << type << data;
    if ((m_codec != nullptr) &&
        (m_track == 0 || m_track == track) &&
        (m_type == 0 || m_type == type)) {
        QString s = sanitizeText(data);
        if ((m_textViewer->find(s.trimmed(), QTextDocument::FindCaseSensitively)) ) {
            m_textViewer->setTextColor(m_otherColor);
            QRect r = m_textViewer->cursorRect();
            QScrollBar *s = m_textViewer->verticalScrollBar();
            int half = m_textViewer->viewport()->height() / 2;
            int newpos = s->value() + r.top() - half;
            if ((r.top() > half) && (newpos < s->maximum()))
                s->setValue(newpos);
        }
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
    displayText();
}

void Lyrics::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_textViewer->font(), this);
    if (ok) {
        m_textViewer->setFont(font);
        displayText();
    }
}
