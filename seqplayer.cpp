﻿/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifdef WIN32
#include <windows.h>
#endif

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <typeinfo>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QMessageBox>
#include <QThread>
#include <QtMath>

#include "events.h"
#include "seqplayer.h"
#include "settings.h"

using namespace drumstick::rt;

void SequencePlayer::initChannels()
{
    for (int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
        m_lastpgm[chan] = 0;
        m_volumeShift[chan] = 1.0;
        m_volume[chan] = 100;
        m_muted[chan] = false;
        m_locked[chan] = false;
        m_lockedpgm[chan] = 0;
    }
}

int SequencePlayer::boundedFloor(const int initial, const qreal factor) const
{
    int temp = qFloor(initial * factor);
    if (temp < 0) {
        temp = 0;
    }
    if (temp > 127) {
        temp = 127;
    }
    return temp;
}

bool SequencePlayer::isLoopEnabled() const
{
    return m_loopEnabled;
}

void SequencePlayer::setLoop(bool enabled)
{
    m_loopEnabled = enabled;
}

void SequencePlayer::setLoop(int loopStart, int loopEnd)
{
    m_loopEnabled = true;
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
}

int SequencePlayer::loopEnd() const
{
    return m_loopEnd;
}

int SequencePlayer::loopStart() const
{
    return m_loopStart;
}

SequencePlayer::SequencePlayer()
    : m_port(nullptr)
    , m_songPositionTicks(0)
    , m_echoResolution(50)
    , m_loopEnabled(false)
    , m_loopStart(0)
    , m_loopEnd(0)
    , m_pitchShift(0)
    , m_volumeFactor(100)
    , m_latestBeat(nullptr)
    , m_firstBeat(nullptr)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaType<std::chrono::milliseconds>();
#endif
    initChannels();
}

SequencePlayer::~SequencePlayer()
{
    //qDebug() << Q_FUNC_INFO;
    if (isRunning()) {
        stop();
    }
}

bool SequencePlayer::isRunning()
{
    return thread()->isRunning() && !thread()->isInterruptionRequested();
}

void SequencePlayer::stop()
{
    thread()->requestInterruption();
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
    static const std::type_info &textId = typeid(TextEvent);
    static const std::type_info &tempoId = typeid(TempoEvent);
    static const std::type_info &timeSigId = typeid(TimeSignatureEvent);
    static const std::type_info &keySigId = typeid(KeySignatureEvent);
    static const std::type_info &beatId = typeid(BeatEvent);
    static const std::type_info &sysexId = typeid(SysExEvent);

    if (m_port == nullptr)
        return;

    if (ev->isChannel()) {
        ChannelEvent* ev2 = static_cast<ChannelEvent*>(ev);
        int chan = ev2->channel();
        if (m_muted[chan]) {
            //qDebug() << "muted channel:" << chan;
            return;
        }
        switch(ev->status()) {
        case MIDI_STATUS_NOTEOFF: {
                NoteOffEvent* event = static_cast<NoteOffEvent*>(ev);
                int key = event->key();
                int vel = event->velocity();
                if (chan != MIDI_GM_STD_DRUM_CHANNEL) {
                    key += m_pitchShift;
                }
                m_port->sendNoteOff(chan, key, vel);
                //qDebug() << m_songPosition << ev->tick() << " NoteOff: " << chan << key << vel;
                emit midiNoteOff(chan, key, vel);
            }
            break;
        case MIDI_STATUS_NOTEON: {
                NoteOnEvent* event = static_cast<NoteOnEvent*>(ev);
                int vel = event->velocity();
                int key = event->key();
                if (chan != MIDI_GM_STD_DRUM_CHANNEL) {
                    key += m_pitchShift;
                }
                m_port->sendNoteOn(chan, key, vel);
                //qDebug() << m_songPosition << ev->tick() << " NoteOn: " << chan << key << vel;
                emit midiNoteOn(chan, key, vel);
            }
            break;
        case MIDI_STATUS_KEYPRESURE: {
                KeyPressEvent* event = static_cast<KeyPressEvent*>(ev);
                int vel = event->velocity();
                int key = event->key();
                if (chan != MIDI_GM_STD_DRUM_CHANNEL) {
                    key += m_pitchShift;
                }
                m_port->sendKeyPressure(chan, key, vel);
                //qDebug() << m_songPosition << event->tick() << " KeyPress: " << chan << key << vel;
                emit midiKeyPressure(chan, key, vel);
            }
            break;
        case MIDI_STATUS_CONTROLCHANGE: {
                ControllerEvent* event = static_cast<ControllerEvent*>(ev);
                int par = event->param();
                int val = event->value();
                if (par == ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME) {
                    m_volume[chan] = val;
                    val = boundedFloor(val, m_volumeShift[chan] / 100.0);
                }
                m_port->sendController(chan, par, val);
                //qDebug() << m_songPosition << event->tick() << " CtrlChg: " << chan << par << val;
                emit midiController(chan, par, val);
            }
            break;
        case MIDI_STATUS_PROGRAMCHANGE: {
                ProgramChangeEvent* event = static_cast<ProgramChangeEvent*>(ev);
                int pgm = m_locked[chan] ? m_lockedpgm[chan] : event->program();
                m_port->sendProgram(chan, pgm);
                m_lastpgm[chan] = pgm;
                //qDebug() << m_songPosition << event->tick() << " PgmChg: " << chan << pgm;
                emit midiProgram(chan, pgm);
            }
            break;
        case MIDI_STATUS_CHANNELPRESSURE: {
                ChanPressEvent* event = static_cast<ChanPressEvent*>(ev);
                int val = event->value();
                m_port->sendChannelPressure(chan, val);
                //qDebug() << m_songPosition << event->tick() << " ChanPress: " << chan << val;
                emit midiChannelPressure(chan, val);
            }
            break;
        case MIDI_STATUS_PITCHBEND: {
                PitchBendEvent* event = static_cast<PitchBendEvent*>(ev);
                int val = event->value();
                m_port->sendPitchBend(chan, val);
                //qDebug() << m_songPosition << event->tick() << " Bender: " << chan << val;
                emit midiPitchBend(chan, val);
            }
            break;
        default:
            //qDebug() << m_songPosition << ev->tick() << "unknown channel event status:" << ev->status();
            break;
        }
    } else
    if (ev->isMetaEvent()) {
        if (typeid(*ev) == sysexId) {
            SysExEvent* event = static_cast<SysExEvent*>(ev);
            m_port->sendSysex(event->data());
            //qDebug() << m_songPosition << event->tick() << " SysEx: "  << event->data().toHex();
            emit midiSysex(event->data());
        } else
        if (typeid(*ev) == textId) {
            TextEvent* event = static_cast<TextEvent*>(ev);
            //qDebug() << m_songPosition << event->tick() << " Text(" << event->textType() << "): " << event->data();
            emit midiText(event->tag(), event->textType(), event->tick(), event->data());
        } else
        if (typeid(*ev) == tempoId) {
            TempoEvent* event = static_cast<TempoEvent*>(ev);
            auto tempo = event->tempo();
            m_song.updateTempo(tempo);
            //qDebug() << m_songPosition << ev->tick() << " Tempo: " << tempo << "bpm:" << event->bpm();
            emit tempoChanged(tempo);
        } else
        if (typeid(*ev) == beatId) {
            BeatEvent* event = static_cast<BeatEvent*>(ev);
            emit beat(event->bar(), event->beat(), event->barLength());
            m_latestBeat = event;
            //qDebug() << m_songPosition << ev->tick() << "bar:beat ->" << event->bar() << ":" << event->beat();
        } else
        if (typeid(*ev) == timeSigId) {
            TimeSignatureEvent* event = static_cast<TimeSignatureEvent*>(ev);
            emit timeSignature(event->tag(), event->numerator(), event->denominator());
            //qDebug() << m_songPosition << ev->tick() << "TimeSignature:" << event->tag() << event->numerator() << event->denominator();
        } else
        if (typeid(*ev) == keySigId) {
            KeySignatureEvent* event = static_cast<KeySignatureEvent*>(ev);
            emit keySignature(event->tag(), event->alterations(), event->minorMode());
        } /*else {
            qDebug() << m_songPosition << ev->tick() << "unknown meta event:" << typeid(*ev).name();
        } */
    } /* else {
        qDebug() << m_songPosition << ev->tick() << "unknown event type:" << typeid(*ev).name();
    } */
}

void SequencePlayer::playerLoop()
{
    using namespace std::chrono;
    using Clock = steady_clock;
    using TimePoint = Clock::time_point;
    static const std::type_info &beatId = typeid(BeatEvent);
    bool useSimpleTimeProcess = m_song.simpleTimeProcess();
    int currentBar{0}, currentBeat{0};
    quint64 echoTicks{0};
    microseconds echoDelta{m_echoResolution};
    TimePoint currentTime{Clock::now()}, nextTime{currentTime}, nextEcho{currentTime},
        startTime{currentTime};
    emit songStarted();
    QAbstractEventDispatcher* dispatcher = thread()->eventDispatcher();
    QEventLoop::ProcessEventsFlags eventFilter = QEventLoop::ExcludeUserInputEvents;
    dispatcher->processEvents(eventFilter);
    m_songPositionTicks = 0;
#ifndef QT_NO_QDEBUG
    qDebug() << "using simple time process: " << useSimpleTimeProcess;
#endif

#ifdef WIN32
    timeBeginPeriod(1);
#endif

    do {
        while (m_song.hasMoreEvents() && !thread()->isInterruptionRequested()) {
            MIDIEvent* ev = m_song.nextEvent();
            if (typeid(*ev) == beatId) {
                currentBar = static_cast<BeatEvent*>(ev)->bar();
                currentBeat = static_cast<BeatEvent *>(ev)->beat();
                if (isLoopEnabled() && (currentBar > loopEnd())) {
                    break;
                }
            }
            if (ev->delta() > 0) {
                if (useSimpleTimeProcess) {
                    nextTime = startTime + m_song.timeOfEvent(ev);
                } else {
                    nextTime = currentTime + m_song.deltaTimeOfEvent(ev);
                }
                echoDelta = m_song.timeOfTicks(m_echoResolution);
                nextEcho = currentTime + echoDelta;
                while (nextEcho < nextTime) {
                    dispatcher->processEvents(eventFilter);
                    std::this_thread::sleep_until(nextEcho);
                    echoTicks += m_echoResolution;
                    emit songEchoTime(duration_cast<milliseconds>(m_song.timeOfTicks(echoTicks)),
                                      echoTicks);
                    currentTime = Clock::now();
                    nextEcho = currentTime + echoDelta;
                }
                dispatcher->processEvents(eventFilter);
                std::this_thread::sleep_until(nextTime);
                echoTicks = ev->tick();
                m_songPositionTicks = echoTicks;
                currentTime = Clock::now();
                emit songEchoTime(duration_cast<milliseconds>(m_song.timeOfTicks(echoTicks)),
                                  echoTicks);
            }
            playEvent(ev);
#ifndef QT_NO_QDEBUG
            if (typeid(*ev) == beatId) {
                std::cout << std::setw(10) << std::right
                          << duration_cast<milliseconds>(currentTime - startTime).count()
                          << std::left << " bar: " << currentBar << " beat: " << currentBeat
                          << std::endl;
            }
#endif
        }
        if (isLoopEnabled()) {
            jumpToBar(loopStart());
        }
    } while (m_song.hasMoreEvents() && !thread()->isInterruptionRequested());

#ifdef WIN32
    timeEndPeriod(1);
#endif

    emit songStopped();
    if (!m_song.hasMoreEvents()) {
#ifndef QT_NO_QDEBUG
        qDebug() << "Final Song Position:" << m_songPositionTicks
                 << (currentTime - startTime).count() / 1e9;
#endif
        emit songFinished();
    }
    dispatcher->processEvents(eventFilter);
    thread()->quit();
}

void SequencePlayer::setPort(drumstick::rt::MIDIOutput *p)
{
    m_port = p;
}

MIDIOutput *SequencePlayer::port() const
{
    return m_port;
}

qreal SequencePlayer::bpm(qreal tempo) const
{
    return 6e7 / tempo;
}

qreal SequencePlayer::currentBPM() const
{
    return bpm(m_song.currentTempo());
}

qreal SequencePlayer::initialBPM() const
{
    return bpm(m_song.initialTempo());
}

Sequence *SequencePlayer::song()
{
    return &m_song;
}

int SequencePlayer::getPosition()
{
    //qDebug() << Q_FUNC_INFO << m_songPosition;
    return m_songPositionTicks;
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
    m_songPositionTicks = 0;
    m_firstBeat = m_song.firstBeat();
    m_latestBeat = m_firstBeat;
    m_loopStart = 1;
    m_loopEnd = m_song.lastBar();
    m_loopEnabled = false;
    if (m_song.errorsCount() > 0) {
        QString loadingMessages = tr("Warning, this file may be non-standard or damaged.<br/>");
        loadingMessages.append(m_song.loadingErrors());
        QMessageBox::warning(QApplication::activeWindow(), Settings::QSTR_APPNAME, loadingMessages);
    }
#ifndef QT_NO_QDEBUG
    m_song.dump();
#endif
}

void SequencePlayer::pause()
{
    stop();
    thread()->wait();
}

void SequencePlayer::resetPosition()
{
    //qDebug() << Q_FUNC_INFO;
    if (!m_song.isEmpty()) {
        m_song.resetPosition();
        m_songPositionTicks = 0;
        m_latestBeat = m_firstBeat;
    }
}

void SequencePlayer::setPosition(quint64 ticks)
{
    //qDebug() << Q_FUNC_INFO << pos;
    allNotesOff();
    m_song.setTickPosition(ticks);
    m_songPositionTicks = ticks;
}

void SequencePlayer::setPitchShift(unsigned int pitch)
{
    //qDebug() << Q_FUNC_INFO;
    allNotesOff();
    m_pitchShift = pitch;
}

void SequencePlayer::setVolumeFactor(unsigned int vol)
{
    //qDebug() << Q_FUNC_INFO << vol;
    m_volumeFactor = vol;
    if (m_port != nullptr) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            int value = m_volume[chan];
            value = boundedFloor(value, m_volumeFactor / 100.0);
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, value);
        }
    }
}

void SequencePlayer::allNotesOff()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_port != nullptr) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_ALL_NOTES_OFF, 0);
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_ALL_SOUNDS_OFF, 0);
        }
    }
}

void SequencePlayer::resetControllers()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_port != nullptr) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_RESET_CONTROLLERS, 0);
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, 100);
        }
    }
}

void SequencePlayer::resetPrograms()
{
    if (m_port != nullptr) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            int pgm = m_locked[chan] ? m_lockedpgm[chan] : 0;
            m_port->sendProgram(chan, pgm);
            //qDebug() << Q_FUNC_INFO << chan << m_locked[chan] << pgm;
        }
    }
}

void SequencePlayer::sendResetMessage()
{
    static const QByteArray gmreset = QByteArray::fromHex("f07e7f0901f7");
    static const QByteArray gsreset = QByteArray::fromHex("f04110421240007f0041f7");
    static const QByteArray xgreset = QByteArray::fromHex("f043104c00007e00f7");
    int reset_msg = Settings::instance()->getSysexResetMessage();
    if (m_port != nullptr && reset_msg > 0 ) {
        switch (reset_msg) {
        case 1:
            m_port->sendSysex(gmreset);
            break;
        case 2:
            m_port->sendSysex(gsreset);
            break;
        case 3:
            m_port->sendSysex(xgreset);
            break;
        }
        //qDebug() << Q_FUNC_INFO << reset_msg;
    }
}

void SequencePlayer::sendVolumeEvents()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_port != nullptr) {
        for(int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            int value = m_volume[chan] = 100;
            value = floor(value * m_volumeFactor / 100.0);
            if (value < 0) value = 0;
            if (value > 127) value = 127;
            m_port->sendController(chan, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, value);
        }
    }
}

qreal SequencePlayer::volume(int channel)
{
    if (channel >=0 && channel < MIDI_STD_CHANNELS)
        return m_volumeShift[channel];
    return -1.0;
}

bool SequencePlayer::isMuted(int channel)
{
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        return m_muted[channel];
    }
    return false;
}

bool SequencePlayer::isLocked(int channel)
{
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        return m_locked[channel];
    }
    return false;
}

void SequencePlayer::beatByTickPosition(int pos)
{
    BeatEvent* ev = m_song.nearestBeatByTicks(pos);
    m_latestBeat = ev;
}

void SequencePlayer::beatForward()
{
    BeatEvent* ev = m_song.nextBar(m_latestBeat);
    if (ev != nullptr) {
        setBeatPosition(ev);
    }
}

void SequencePlayer::beatBackward()
{
    BeatEvent* ev = m_song.previousBar(m_latestBeat);
    if (ev != nullptr) {
        setBeatPosition(ev);
    }
}

void SequencePlayer::setBeatPosition(BeatEvent* ev)
{
    setPosition(ev->tick());
    emit beat(ev->bar(), ev->beat(), ev->barLength());
    m_latestBeat = ev;
}

void SequencePlayer::jumpToBar(int bar)
{
    BeatEvent* ev = m_song.jumpToBar(bar);
    if (ev != nullptr) {
        setBeatPosition(ev);
    }
}

QString SequencePlayer::currentBeatStr()
{
    QString tmp;
    if (m_latestBeat != nullptr) {
        tmp = QString("%1:%2").arg(m_latestBeat->bar()).arg(m_latestBeat->beat());
    }
    return tmp;
}

void SequencePlayer::setVolume(int channel, qreal value)
{
    //qDebug() << Q_FUNC_INFO << channel << value;
    if (m_port == nullptr) {
        return;
    }
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        m_volumeShift[channel] = value;
        int volume = boundedFloor(m_volume[channel], value / 100.0);
        m_port->sendController(channel, MIDI_CONTROL_MSB_MAIN_VOLUME, volume);
        emit volumeChanged( channel, value );
    } else if ( channel == -1 ) {
        for (int chan = 0; chan < MIDI_STD_CHANNELS; ++chan) {
            m_volumeShift[chan] = value;
            int volume = boundedFloor(m_volume[chan], value / 100.0);
            m_port->sendController(chan, MIDI_CONTROL_MSB_MAIN_VOLUME, volume);
            emit volumeChanged( chan, value );
        }
    }
}

void SequencePlayer::setMuted(int channel, bool mute)
{
    //qDebug() << Q_FUNC_INFO << channel << mute;
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        if (m_muted[channel] != mute) {
            if (mute) {
                m_port->sendController(channel, MIDI_CONTROL_ALL_NOTES_OFF, 0);
                m_port->sendController(channel, MIDI_CONTROL_ALL_SOUNDS_OFF, 0);
            }
            m_muted[channel] = mute;
            emit mutedChanged( channel, mute );
        }
    }
}

void SequencePlayer::setLocked(int channel, bool lock)
{
    //qDebug() << Q_FUNC_INFO << channel << lock;
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        if (m_locked[channel] != lock) {
            m_locked[channel] = lock;
            if (lock) {
                m_lockedpgm[channel] = m_lastpgm[channel];
            }
            emit lockedChanged( channel, lock );
        }
    }
}

void SequencePlayer::setPatch(int channel, int patch)
{
    //qDebug() << Q_FUNC_INFO << channel << patch;
    if (channel >= 0 && channel < MIDI_STD_CHANNELS) {
        m_lastpgm[channel] = patch;
        if (m_locked[channel]) {
            m_lockedpgm[channel] = patch;
        }
    }
}
