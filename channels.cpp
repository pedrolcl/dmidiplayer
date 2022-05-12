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

#include <QDebug>
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QLineEdit>
#include <QCloseEvent>
#include <QToolBar>
#include <QMenu>
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

#include <drumstick/settingsfactory.h>

#include "settings.h"
#include "sequence.h"
#include "channels.h"
#include "vumeter.h"
#include "iconutils.h"

using namespace drumstick::rt;

Channels::Channels( QWidget* parent ) :
    FramelessWindow(parent),
    m_timerId(0),
    m_volumeFactor(1.0)
{
    setObjectName("ChannelsWindow");
    setWindowFlag(Qt::Tool, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
    QToolBar* tbar = new QToolBar(this);
    tbar->setObjectName("toolbar");
    tbar->setMovable(false);
    tbar->setFloatable(false);
    tbar->setIconSize(QSize(22,22));
    m_title = new QLabel(tr("MIDI Channels"), this);
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    tbar->addWidget(m_title);
    auto chmenu = new QMenu(this);
    //FIXME: add menu options
    auto toolBtn = new QToolButton(this);
    toolBtn->setMenu(chmenu);
    toolBtn->setPopupMode(QToolButton::InstantPopup);
    toolBtn->setIcon(IconUtils::GetIcon("application-menu"));
    tbar->addWidget(toolBtn);
    auto closeBtn = new QToolButton(this);
    closeBtn->setIcon(IconUtils::GetIcon("window-close"));
    connect(closeBtn, &QToolButton::clicked, this, &Channels::close);
    tbar->addWidget(closeBtn);
    addToolBar(tbar);
    setPseudoCaption(tbar);
    tbar->show();
    QGridLayout *layout = new QGridLayout;
    layout->setContentsMargins(5,5,5,5);
    layout->setHorizontalSpacing(10);
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(spacer, 0, 0, 1, 7);
    m_lbl1 = new QLabel(this);
    m_lbl1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl1, 1, 0, 1, 2);
    m_lbl2 = new QLabel(this);
    m_lbl2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl2, 1, 2);
    m_lbl3 = new QLabel(this);
    m_lbl3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl3, 1, 3);
    m_lbl4 = new QLabel(this);
    m_lbl4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl4, 1, 4);
    m_lbl5 = new QLabel(this);
    m_lbl5->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl5, 1, 5);
    m_lbl6 = new QLabel(this);
    m_lbl6->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_lbl6, 1, 6);
    layout->setColumnStretch(1,1);
    QSize ledSize(12,12);
    QPixmap pixMuteOn(ledSize);
    QPixmap pixMuteOff(ledSize);
    pixMuteOn.fill(Qt::red);
    pixMuteOff.fill(QColor(90,0,0));
    QIcon muteIcon;
    muteIcon.addPixmap(pixMuteOn, QIcon::Normal, QIcon::On);
    muteIcon.addPixmap(pixMuteOff,QIcon::Normal, QIcon::Off);
    QPixmap pixSoloOn(ledSize);
    QPixmap pixSoloOff(ledSize);
    pixSoloOn.fill(Qt::green);
    pixSoloOff.fill(QColor(0,90,0));
    QIcon soloIcon;
    soloIcon.addPixmap(pixSoloOn, QIcon::Normal, QIcon::On);
    soloIcon.addPixmap(pixSoloOff,QIcon::Normal, QIcon::Off);
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i) {
        int row = i + 2;
        m_lbl[i] = new QLabel(this);
        m_lbl[i]->setNum(row);
        layout->addWidget(m_lbl[i], row, 0, Qt::AlignRight | Qt::AlignVCenter);
        m_name[i] = new QLineEdit(this);
        layout->addWidget(m_name[i], row, 1);
        connect( m_name[i], &QLineEdit::editingFinished, this, [=]{ slotNameChannel(i); });
        m_mute[i] = new QToolButton(this);
        m_mute[i]->setCheckable(true);
        m_mute[i]->setIcon(muteIcon);
        layout->addWidget(m_mute[i], row, 2);
        connect( m_mute[i], &QToolButton::clicked, this, [=]{ slotMuteChannel(i); } );
        m_solo[i] = new QToolButton(this);
        m_solo[i]->setCheckable(true);
        m_solo[i]->setIcon(soloIcon);
        layout->addWidget(m_solo[i], row, 3);
        connect( m_solo[i], &QToolButton::clicked, this, [=]{ slotSoloChannel(i); } );
        m_vumeter[i] = new Vumeter(this);
        layout->addWidget(m_vumeter[i], row, 4);
        m_lock[i] = new QToolButton(this);
        m_lock[i]->setCheckable(true);
        layout->addWidget(m_lock[i], row, 5);
        connect( m_lock[i], &QToolButton::clicked, this, [=]{ slotLockChannel(i); } );
        m_patch[i] = new QComboBox(this);
        m_patch[i]->setStyleSheet("combobox-popup: 0;");
        m_patch[i]->addItems(m_instSet.names(i == MIDI_GM_STD_DRUM_CHANNEL));
        layout->addWidget(m_patch[i], row, 6);
        connect( m_patch[i], QOverload<int>::of(&QComboBox::activated), this, [=](int){ slotPatchChanged(i); });
        m_lbl[i]->setBuddy(m_patch[i]);
        m_voices[i] = 0;
        m_level[i] = 0.0;
        m_factor[i] = m_volumeFactor;
    }
    adjustSize();
    retranslateUi();
    applySettings();
}

Channels::~Channels()
{
    //qDebug() << Q_FUNC_INFO;
}

void Channels::retranslateUi()
{
    //setWindowTitle(tr("MIDI Channels"));
    m_title->setText(tr("MIDI Channels"));
    m_lbl1->setText(tr( "Channel"));
    m_lbl2->setText(tr( "Mute"));
    m_lbl3->setText(tr( "Solo"));
    m_lbl4->setText(tr( "Level"));
    m_lbl5->setText(tr( "Lock"));
    m_lbl6->setText(tr("Patch (sound setting)"));
    m_instSet.reloadNames();
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i) {
        int curr = m_patch[i]->currentIndex();
        m_patch[i]->clear();
        m_patch[i]->addItems(m_instSet.names(i == MIDI_GM_STD_DRUM_CHANNEL));
        m_patch[i]->setCurrentIndex(curr);
    }
}

void Channels::initSong(Sequence *song)
{
    if (song != nullptr) {
        for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
            setLockChannel(i, false);
            setSoloChannel(i, false);
            setMuteChannel(i, false);
            enableChannel(i, song->channelUsed(i));
            setChannelName(i, song->channelLabel(i));
        }
        centralWidget()->adjustSize();
        adjustSize();
    }
}

void Channels::applySettings()
{

    QIcon locked(IconUtils::GetIcon("object-locked"));
    QIcon unlocked(IconUtils::GetIcon("object-unlocked"));
    QIcon lockIcon;
    QSize lockSize(16,16);
    lockIcon.addPixmap(locked.pixmap(lockSize), QIcon::Normal, QIcon::On);
    lockIcon.addPixmap(unlocked.pixmap(lockSize), QIcon::Normal, QIcon::Off);
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i) {
        m_lock[i]->setIcon(lockIcon);
    }
    foreach(QComboBox *cb, findChildren<QComboBox*>()) {
        cb->setPalette(qApp->palette());
        foreach(QWidget *w, cb->findChildren<QWidget*>()) {
            w->setPalette(qApp->palette());
        }
    }
#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(Settings::instance()->winSnap());
#endif
}

void Channels::readSettings()
{
    const QByteArray geometry = Settings::instance()->channelsWindowGeometry();
    const QByteArray state = Settings::instance()->channelsWindowState();
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

void Channels::writeSettings()
{
    Settings::instance()->setChannelsWindowGeometry(saveGeometry());
    Settings::instance()->setChannelsWindowState(saveState());
}

void Channels::closeEvent(QCloseEvent *event)
{
    writeSettings();
    emit closed();
    event->accept();
}

void Channels::enableChannel(int channel, bool enable)
{
    m_mute[channel]->setChecked(false);
    m_mute[channel]->setEnabled(enable);
    m_solo[channel]->setChecked(false);
    m_solo[channel]->setEnabled(enable);
    m_vumeter[channel]->setValue(0);
    m_vumeter[channel]->setEnabled(enable);
    if (!m_lock[channel]->isChecked())
        m_patch[channel]->setCurrentIndex(-1);
    m_patch[channel]->setEnabled(enable);
    m_name[channel]->clear();
    m_name[channel]->setEnabled(enable);
    m_lock[channel]->setEnabled(enable);

    m_lbl[channel]->setVisible(enable);
    m_mute[channel]->setVisible(enable);
    m_solo[channel]->setVisible(enable);
    m_vumeter[channel]->setVisible(enable);
    m_patch[channel]->setVisible(enable);
    m_name[channel]->setVisible(enable);
    m_lock[channel]->setVisible(enable);
}

void Channels::slotDisableAllChannels()
{
    for ( int channel = 0; channel < MIDI_STD_CHANNELS; ++channel ) {
        enableChannel(channel, false);
    }
    adjustSize();
}

void Channels::slotEnableAllChannels()
{
    for ( int channel = 0; channel < MIDI_STD_CHANNELS; ++channel ) {
        enableChannel(channel, true);
    }
    adjustSize();
}

void Channels::slotPatch(int channel, int value)
{
    //qDebug() << Q_FUNC_INFO << channel << value;
    if (m_lock[channel]->isChecked())
        return;
    m_patch[channel]->setCurrentIndex(value);
}

void Channels::slotNoteOn(int channel, int /*note*/, int vel)
{
    qreal r = vel / 127.0 * m_factor[channel];
    if (r > 0.0) {
        m_level[channel] = r;
        m_voices[channel] += 1;
    } else
        m_voices[channel] -= 1;

    if (m_timerId == 0)
        m_timerId = startTimer(200);
}

void Channels::slotNoteOff(int channel, int /*note*/, int /*vel*/)
{
    m_voices[channel] -= 1;
}

void Channels::slotPatchChanged(int channel)
{
    int p = m_patch[channel]->currentIndex();
    if (p > -1) {
        emit patch(channel, p);
        //qDebug() << Q_FUNC_INFO << channel << p;
    }
}

void Channels::slotMuteChannel(int channel)
{
    //qDebug() << Q_FUNC_INFO << channel << m_mute[channel]->isChecked();
    emit mute(channel, m_mute[channel]->isChecked());
}

void Channels::slotSoloChannel(int channel)
{
    //qDebug() << Q_FUNC_INFO << channel << m_solo[channel]->isChecked();
    bool enable = m_solo[channel]->isChecked();
    for ( int ch = 0; ch < MIDI_STD_CHANNELS; ++ch ) {
        if (channel != ch) {
            m_solo[ch]->setChecked(false);
            m_factor[ch] = enable ? m_volumeFactor * 0.5 : m_volumeFactor;
            emit volume(ch, m_factor[ch]);
        }
    }
    m_factor[channel] = m_volumeFactor;
    emit volume(channel, m_volumeFactor);
}

void Channels::timerEvent(QTimerEvent *event)
{
    if (m_timerId == event->timerId()) {
        qreal v;
        bool kill = true;
        for ( int ch = 0; ch < MIDI_STD_CHANNELS; ++ch ) {
            if (m_voices[ch] > 0) {
                v = m_level[ch];
                m_vumeter[ch]->setValue(v);
            } else {
                v = m_vumeter[ch]->decay(10);
                m_level[ch] = v;
                m_voices[ch] = 0;
            }
            kill &= (v <= 0.0);
        }
        if (kill) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
    }
}

qreal Channels::volumeFactor()
{
    return m_volumeFactor;
}

void Channels::setVolumeFactor(qreal factor)
{
    m_volumeFactor = factor;
    for ( int ch = 0; ch < MIDI_STD_CHANNELS; ++ch ) {
        m_solo[ch]->setChecked(false);
        m_factor[ch] = m_volumeFactor;
    }
}

void Channels::allNotesOff()
{
    for ( int ch = 0; ch < MIDI_STD_CHANNELS; ++ch ) {
        m_voices[ch] = 0;
    }
}

void Channels::showEvent(QShowEvent *event)
{
    static bool firstTime = true;
    QMainWindow::showEvent(event);
    if (firstTime) {
        readSettings();
        firstTime = false;
    }
}

void Channels::setChannelName(int channel, const QString& name)
{
    m_name[channel]->setText(name);
}

QString Channels::channelName(int channel) const
{
    return m_name[channel]->text();
}

void Channels::slotLockChannel(int channel)
{
    //qDebug() << Q_FUNC_INFO << channel << m_lock[channel]->isChecked();
    emit lock(channel, m_lock[channel]->isChecked());
}

bool Channels::isChannelLocked(int channel) const
{
    return m_lock[channel]->isChecked();
}

bool Channels::isChannelMuted(int channel) const
{
    return m_mute[channel]->isChecked();
}

void Channels::setMuteChannel(int channel, bool mute)
{
    m_mute[channel]->setChecked(mute);
}

bool Channels::isChannelSoloed(int channel) const
{
    return m_solo[channel]->isChecked();
}

void Channels::setSoloChannel(int channel, bool solo)
{
    m_solo[channel]->setChecked(solo);
}

int Channels::channelPatch(int channel) const
{
    return m_patch[channel]->currentIndex();
}

void Channels::setPatchChannel(int channel, int patch)
{
    m_patch[channel]->setCurrentIndex(patch);
}

void Channels::setLockChannel(int channel, bool lock)
{
    m_lock[channel]->setChecked(lock);
}

void Channels::slotNameChannel(int channel)
{
    emit name(channel, m_name[channel]->text());
}

bool Channels::nativeEvent(const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                            long *result
#else
                            qintptr *result
#endif
                          )
{
#if defined(Q_OS_WINDOWS)
    if (m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}
