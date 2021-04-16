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
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QAction>
#include <QCloseEvent>
#include <drumstick/settingsfactory.h>
#include <drumstick/pianokeybd.h>
#include <drumstick/rtmidioutput.h>

#include "settings.h"
#include "sequence.h"
#include "pianola.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

Pianola::Pianola( QWidget* parent ) : QMainWindow(parent)
{
    setObjectName("PlayerPianoWindow");
    setWindowFlag(Qt::Tool, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
    menuBar()->setNativeMenuBar(false);
    m_chmenu = menuBar()->addMenu(tr("MIDI Channels"));
    m_a1 = new QAction(this); // Show all channels
    connect(m_a1, &QAction::triggered, this, &Pianola::slotShowAllChannels);
    m_chmenu->addAction(m_a1);
    m_a2 = new QAction(this); // Hide all channels
    connect(m_a2, &QAction::triggered, this, &Pianola::slotHideAllChannels);
    m_chmenu->addAction(m_a2);
    m_a3 = new QAction(this); // Tighten the number of keys
    m_a3->setCheckable(true);
    m_tightenKeys = true;
    m_a3->setChecked(true);
    connect(m_a3, &QAction::triggered, this, &Pianola::tightenKeys);
    m_chmenu->addAction(m_a3);
    m_chmenu->addSeparator();
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(vlayout);
    int palId = Settings::instance()->highlightPaletteId();
    PianoPalette pal = Settings::instance()->getPalette(palId);
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        m_frame[i] = new QFrame(this);
        m_frame[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QGridLayout* glayout = new QGridLayout;
        glayout->setSpacing(0);
        glayout->setContentsMargins(0,0,0,0);
        m_frame[i]->setLayout(glayout);
        QLabel* lbl = new QLabel(this);
        lbl->setNum(i+1);
        lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lbl->setMinimumWidth(25);
        glayout->addWidget(lbl,0,0,2,1);
        m_label[i] = new QLabel(this);
        m_label[i]->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        glayout->addWidget(m_label[i],0,1);
        m_piano[i] = new PianoKeybd(this);
        m_piano[i]->setChannel(i);
        m_piano[i]->setFont(Settings::instance()->notesFont());
        m_piano[i]->setVelocityTint(Settings::instance()->velocityColor());
        if (palId == PAL_SINGLE) {
            m_piano[i]->setKeyPressedColor(Settings::instance()->getSingleColor());
        } else {
            m_piano[i]->setHighlightPalette(pal);
        }
        m_piano[i]->setShowLabels(Settings::instance()->namesVisibility());
        m_piano[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(m_piano[i], &PianoKeybd::noteOn, this, &Pianola::playNoteOn);
        connect(m_piano[i], &PianoKeybd::noteOff, this, &Pianola::playNoteOff);
        glayout->addWidget(m_piano[i],1,1);
        vlayout->addWidget(m_frame[i]);
        lbl->setBuddy(m_piano[i]);
        m_frame[i]->setVisible(false);
        m_action[i] = new QAction(this);
        m_action[i]->setCheckable(true);
        connect(m_action[i], &QAction::triggered, this, [=]{ slotShowChannel(i); });
        m_chmenu->addAction(m_action[i]);
    }
    readSettings();
    retranslateUi();
    applySettings();
}

Pianola::~Pianola()
{
    //qDebug() << Q_FUNC_INFO;
}

void Pianola::retranslateUi()
{
    setWindowTitle(tr("Player Piano"));
    m_chmenu->setTitle(tr("MIDI Channels"));
    m_a1->setText(tr("Show all channels"));
    m_a2->setText(tr("Hide all channels"));
    m_a3->setText(tr("Tighten the number of keys"));
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        m_action[i]->setText(tr("Channel %1").arg(i+1));
        m_piano[i]->retranslate();
    }
}

void Pianola::applySettings()
{
    int palId = Settings::instance()->highlightPaletteId();
    PianoPalette pal = Settings::instance()->getPalette(palId);
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        m_piano[i]->setFont(Settings::instance()->notesFont());
        m_piano[i]->setVelocityTint(Settings::instance()->velocityColor());
        m_piano[i]->setKeyPressedColor(Qt::red);
        m_piano[i]->setShowLabels(Settings::instance()->namesVisibility());
        if (palId == PAL_SINGLE) {
            m_piano[i]->setKeyPressedColor(Settings::instance()->getSingleColor());
        } else {
            m_piano[i]->setHighlightPalette(pal);
        }
    }
}

void Pianola::initSong(Sequence *song)
{
    m_song = song;
    int loNote = song->lowestMidiNote();
    int hiNote = song->highestMidiNote();
    setNoteRange(loNote, hiNote);
    for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        enableChannel(i, song->channelUsed(i));
        slotLabel(i, song->channelLabel(i));
        m_piano[i]->setLabelAlterations(LabelAlteration::ShowSharps);
    }
}

void Pianola::readSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("PlayerPianoWindow");
    restoreGeometry(settings->value("Geometry").toByteArray());
    restoreState(settings->value("State").toByteArray());
    settings->endGroup();
}

void Pianola::writeSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("PlayerPianoWindow");
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("State", saveState());
    settings->endGroup();
}

void Pianola::closeEvent(QCloseEvent *event)
{
    writeSettings();
    emit closed();
    event->accept();
}

void Pianola::allNotesOff()
{
    for (int ch = 0; ch < MIDI_STD_CHANNELS; ++ch ) {
        if (m_action[ch] != nullptr && m_action[ch]->isChecked()) {
            for( int n = 0; n < 128; ++n ) {
                if (m_piano[ch] != nullptr) {
                    m_piano[ch]->showNoteOff(n);
                }
            }
        }
    }
}

void Pianola::enableChannel(int channel, bool enable)
{
    if (!enable) {
        m_action[channel]->setChecked(false);
        m_frame[channel]->setVisible(false);
    }
    m_action[channel]->setEnabled(enable);
    update();
}

void Pianola::setNoteRange(int lowerNote, int upperNote)
{
    int numKeys = DEFAULTNUMBEROFKEYS;
    int startKey = DEFAULTSTARTINGKEY;
    int octaveBase = DEFAULTBASEOCTAVE;
    if (m_tightenKeys) {
        octaveBase = lowerNote / 12;
        int upperOctave = upperNote / 12;
        numKeys = (upperOctave - octaveBase + 1) * 12 + 1;
        startKey = 0;
        m_lowerNote = lowerNote;
        m_upperNote = upperNote;
    }
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        m_piano[i]->setNumKeys(numKeys, startKey);
        m_piano[i]->setBaseOctave(octaveBase);
    }
}

void Pianola::slotShowChannel(int chan)
{
    m_frame[chan]->setVisible(m_action[chan]->isChecked());
    update();
}

void Pianola::slotNoteOn(int channel, int note, int vel)
{
    //qDebug() << Q_FUNC_INFO << channel << note << vel << m_action[channel]->isChecked();
    if (m_action[channel]->isChecked()) {
        if (vel == 0)
            m_piano[channel]->showNoteOff(note, vel);
        else
            m_piano[channel]->showNoteOn(note, vel);
	}
}

void Pianola::slotNoteOff(int channel, int note, int /*vel*/)
{
    //qDebug() << Q_FUNC_INFO << channel << note;
    if (m_action[channel]->isChecked())
        m_piano[channel]->showNoteOff(note);
}

void Pianola::playNoteOn(int note, int vel)
{
    PianoKeybd* p = static_cast<PianoKeybd*>(sender());
    if (p != NULL) {
        int channel = p->getChannel();
        emit noteOn(channel, note, vel);
    }
}

void Pianola::playNoteOff(int note, int vel)
{
    PianoKeybd* p = static_cast<PianoKeybd*>(sender());
    if (p != nullptr) {
        int channel = p->getChannel();
        emit noteOff(channel, note, vel);
    }
}

void Pianola::showEvent( QShowEvent* /*event*/ )
{
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        if (m_action[i]->isChecked())
            return;
    }
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        if (m_action[i]->isEnabled()) {
            m_action[i]->setChecked(true);
            slotShowChannel(i);
            return;
        }
    }
}

void Pianola::slotShowAllChannels()
{
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        if (m_action[i]->isEnabled() && !m_action[i]->isChecked()) {
            m_action[i]->setChecked(true);
            slotShowChannel(i);
        }
    }
}

void Pianola::slotHideAllChannels()
{
    for (int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
        if (m_action[i]->isEnabled() && m_action[i]->isChecked()) {
            m_action[i]->setChecked(false);
            slotShowChannel(i);
        }
    }
}

void Pianola::slotLabel(int channel, const QString& text)
{
    //qDebug() << Q_FUNC_INFO << channel << text << m_action[channel]->isEnabled();
    if (m_action[channel]->isEnabled())
        m_label[channel]->setText(text);
}

void Pianola::tightenKeys(bool enabled)
{
    if (m_tightenKeys != enabled) {
        m_tightenKeys = enabled;
        setNoteRange(m_lowerNote, m_upperNote);
    }
}

void Pianola::slotKeySignature(int track, int alt, bool /*minor*/)
{
    //qDebug() << Q_FUNC_INFO << track << alt << minor;
    LabelAlteration alterations = (alt < 0) ? LabelAlteration::ShowFlats : LabelAlteration::ShowSharps;
    if (track < 0) { // all tracks
        for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
            m_piano[i]->setLabelAlterations(alterations);
        }
    } else {
        int channel = m_song->trackChannel(track);
        if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
            m_piano[channel]->setLabelAlterations(alterations);
        }
    }
}

bool Pianola::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#if defined(Q_OS_WINDOWS)
    if (m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}
