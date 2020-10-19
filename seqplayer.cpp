/*
    Drumstick MIDI File player graphic program
    Copyright (C) 2006-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QStringList>
#include <QTextStream>
#include <QtAlgorithms>
#include <QReadLocker>
#include <QWriteLocker>
#include <QThread>
#include <QtMath>
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>

#include "seqplayer.h"

using namespace drumstick::rt;

SequencePlayer::SequencePlayer() :
    m_nextEventTime(0),
    m_nextEchoTime(0),
    m_nextEventTick(0),
    m_port(nullptr),
    m_timer(nullptr),
    m_songPosition(0),
    m_echoResolution(50),
    m_pitchShift(0),
    m_volumeFactor(100)
{
    for (int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
        m_volume[chan] = 100;
    }
}

SequencePlayer::~SequencePlayer()
{
    if (isRunning()) {
        stop();
    }
}

bool SequencePlayer::isRunning()
{
    return (m_timer != nullptr) && m_timer->isActive();
    //return thread()->isRunning();
}

void SequencePlayer::timerCleanup()
{
    //qDebug() << Q_FUNC_INFO;
    shutupSound();
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    disconnect(thread(), &QThread::finished, this, &SequencePlayer::timerCleanup);
    emit songStopped();
}

void SequencePlayer::stop()
{
    thread()->quit();
}

void SequencePlayer::shutupSound()
{
    if (m_port != nullptr) {
        for (int channel = 0; channel < 16; ++channel) {
            m_port->sendController(channel, ControllerEvent::MIDI_CTL_ALL_SOUNDS_OFF, 0);
        }
    }
}

void SequencePlayer::playEvent(MIDIEvent* ev)
{
    if (m_port == nullptr)
        return;
    if (ev->isText()) {
        //TextEvent* event = static_cast<TextEvent*>(ev);
        //qDebug() << m_songPosition << event->tick() << " Text(" << event->textType() << "): " << event->text();
    } else
    if (ev->isTempo()) {
        TempoEvent* event = static_cast<TempoEvent*>(ev);
        int tempo = event->tempo();
        //qDebug() << m_songPosition << event->tick() << " Tempo: " << tempo;
        m_song.updateTempo(tempo);
    } else
    if (ev->isMetaEvent()) {
        //qDebug() << m_songPosition << event->tick() << " Meta-event";
    } else
    switch(ev->status()) {
    case MIDI_STATUS_NOTEOFF: {
            NoteOffEvent* event = static_cast<NoteOffEvent*>(ev);
            int key = event->key();
            if (event->channel() != MIDI_GM_STD_DRUM_CHANNEL)
                key += m_pitchShift;
            m_port->sendNoteOff(event->channel(), key, event->velocity());
            //qDebug() << m_songPosition << event->tick() << " NoteOff: "  << event->key();
        }
        break;
    case MIDI_STATUS_NOTEON: {
            NoteOnEvent* event = static_cast<NoteOnEvent*>(ev);
            int key = event->key();
            if (event->channel() != MIDI_GM_STD_DRUM_CHANNEL)
                key += m_pitchShift;
            m_port->sendNoteOn(event->channel(), key, event->velocity());
            //qDebug() << m_songPosition << event->tick() << " NoteOn: "  << event->key();
        }
        break;
    case MIDI_STATUS_KEYPRESURE: {
            KeyPressEvent* event = static_cast<KeyPressEvent*>(ev);
            int key = event->key();
            if (event->channel() != MIDI_GM_STD_DRUM_CHANNEL)
                key += m_pitchShift;
            m_port->sendKeyPressure(event->channel(), key, event->velocity());
            //qDebug() << m_songPosition << event->tick() << " KeyPress: "  << event->key();
        }
        break;
    case MIDI_STATUS_CONTROLCHANGE: {
            ControllerEvent* event = static_cast<ControllerEvent*>(ev);
            if (event->param() == ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME) {
                int chan = event->channel();
                int value = event->value();
                m_volume[chan] = value;
                value = qFloor(value * m_volumeFactor / 100.0);
                if (value < 0) value = 0;
                if (value > 127) value = 127;
                event->setValue(value);
            }
            m_port->sendController(event->channel(), event->param(), event->value());
            //4qDebug() << m_songPosition << event->tick() << " CtrlChg: " << event->param();
        }
        break;
    case MIDI_STATUS_PROGRAMCHANGE: {
            ProgramChangeEvent* event = static_cast<ProgramChangeEvent*>(ev);
            m_port->sendProgram(event->channel(), event->program());
            //qDebug() << m_songPosition << event->tick() << " PgmChg: " << event->program();
        }
        break;
    case MIDI_STATUS_CHANNELPRESSURE: {
            ChanPressEvent* event = static_cast<ChanPressEvent*>(ev);
            m_port->sendChannelPressure(event->channel(), event->value());
            //qDebug() << m_songPosition << event->tick() << " ChanPress: "  << event->value();
        }
        break;
    case MIDI_STATUS_PITCHBEND: {
            PitchBendEvent* event = static_cast<PitchBendEvent*>(ev);
            m_port->sendPitchBend(event->channel(), event->value());
            //qDebug() << m_songPosition << event->tick() << " Bender: "  << event->value();
        }
        break;
    case MIDI_STATUS_SYSEX: {
            SysExEvent* event = static_cast<SysExEvent*>(ev);
            m_port->sendSysex(event->data());
            //qDebug() << m_songPosition << event->tick() << " SysEx: "  << event->data().toHex();
        }
        break;
    default:
        qDebug() << m_songPosition << ev->tick() << " unknown status: " << ev->status();
        break;
    }
}

void SequencePlayer::timerExpired()
{
    auto delta = m_clock.restart();
    m_songPosition += delta;
    while ((m_nextEventTime <= m_songPosition) && m_song.hasMoreEvents()) {
        MIDIEvent* ev = m_song.nextEvent();
        playEvent(ev);
        if (m_song.hasMoreEvents()) {
            m_nextEventTime = m_song.nextEventTime();
            m_nextEventTick = m_song.nextEventTicks();
            if (m_nextEventTime <= m_songPosition) {
                continue;
            }
        } else {
            break;
        }
    }
    if (m_songPosition >= m_nextEchoTime) {
        emit songEchoTime(m_songPosition, m_nextEventTick);
        m_nextEchoTime += m_echoResolution;
    }
    if (!m_song.hasMoreEvents()) {
        qDebug() << Q_FUNC_INFO << "time since start:" << m_songclock.elapsed() << "song Position:" << m_songPosition;
        stop();
        emit songFinished();
    }
}

void SequencePlayer::start()
{
    qDebug() << Q_FUNC_INFO << "timer interval:" << m_song.millisOfTick();
    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setSingleShot(false);
    m_timer->setInterval(m_song.millisOfTick());
    thread()->setPriority(QThread::HighPriority);
    connect(thread(), &QThread::finished, this, &SequencePlayer::timerCleanup);
    connect(m_timer, &QTimer::timeout, this, &SequencePlayer::timerExpired);
    m_nextEventTime = m_song.nextEventTime();
    m_nextEventTick = m_song.nextEventTicks();
    m_nextEchoTime = m_nextEventTime + m_echoResolution;
    m_timer->start();
    m_clock.start();
    m_songclock.start();
}

void SequencePlayer::setPort(drumstick::rt::MIDIOutput *p)
{
    m_port = p;
}

MIDIOutput *SequencePlayer::port() const
{
    return m_port;
}

qreal SequencePlayer::bpm(int tempo) const
{
    return 6e7 / tempo;
}

qreal SequencePlayer::currentBPM() const
{
    return bpm(m_song.currentTempo());
}

Sequence *SequencePlayer::song()
{
    return &m_song;
}

int SequencePlayer::getPosition()
{
    //qDebug() << Q_FUNC_INFO << m_songPosition;
    return m_songPosition;
}

int SequencePlayer::getEchoResolution()
{
    return m_echoResolution;
}

int SequencePlayer::getPitchShift()
{
    return m_pitchShift;
}

int SequencePlayer::getVolumeFactor()
{
    return m_volumeFactor;
}

void SequencePlayer::loadFile(QString fileName)
{
    m_song.loadFile(fileName);
    m_echoResolution = 50; //m_song.getDivision() / 12;
    m_songPosition = 0;
}

void SequencePlayer::pause()
{
    stop();
}

void SequencePlayer::resetPosition()
{
    //qDebug() << Q_FUNC_INFO;
    if (!m_song.isEmpty()) {
        m_song.resetPosition();
        m_songPosition = 0;
    }
}

void SequencePlayer::setPosition(int pos)
{
    m_song.setTickPosition(pos);
    m_songPosition = pos;
}

void SequencePlayer::setPitchShift(unsigned int pitch)
{
    //qDebug() << Q_FUNC_INFO;
    allNotesOff();
    m_pitchShift = pitch;
}

void SequencePlayer::setVolumeFactor(unsigned int vol)
{
    //qDebug() << Q_FUNC_INFO;
    m_volumeFactor = vol;
    if (m_port != 0) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            int value = m_volume[chan];
            value = qFloor(value * m_volumeFactor / 100.0);
            if (value < 0) value = 0;
            if (value > 127) value = 127;
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, value);
        }
    }
}

void SequencePlayer::allNotesOff()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_port != 0) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_ALL_NOTES_OFF, 0);
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_ALL_SOUNDS_OFF, 0);
        }
    }
}

void SequencePlayer::sendVolumeEvents()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_port != 0) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            int value = m_volume[chan] = 100;
            value = floor(value * m_volumeFactor / 100.0);
            if (value < 0) value = 0;
            if (value > 127) value = 127;
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, value);
        }
    }
}
