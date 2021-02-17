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

#include <chrono>
#include <QDebug>
#include <QtMath>
#include <QFileInfo>
#include "sequence.h"

using namespace drumstick::File;
using namespace drumstick::rt;

Sequence::Sequence(QObject *parent) : QObject(parent),
    m_codec(nullptr),
    m_ticksDuration(0),
    m_division(-1),
    m_pos(0),
    m_track(-1),
    m_tempo(500000.0),
    m_tempoFactor(1.0),
    m_duration(0),
    m_lastBeat(0),
    m_beatLength(0),
    m_beatMax(0),
    m_barCount(0),
    m_beatCount(0),
    m_lowestMidiNote(127),
    m_highestMidiNote(0)
{
    m_smf = new QSmf(this);
    connect(m_smf, &QSmf::signalSMFHeader, this, &Sequence::smfHeaderEvent);
    connect(m_smf, &QSmf::signalSMFNoteOn, this, &Sequence::smfNoteOnEvent);
    connect(m_smf, &QSmf::signalSMFNoteOff, this, &Sequence::smfNoteOffEvent);
    connect(m_smf, &QSmf::signalSMFKeyPress, this, &Sequence::smfKeyPressEvent);
    connect(m_smf, &QSmf::signalSMFCtlChange, this, &Sequence::smfCtlChangeEvent);
    connect(m_smf, &QSmf::signalSMFPitchBend, this, &Sequence::smfPitchBendEvent);
    connect(m_smf, &QSmf::signalSMFProgram, this, &Sequence::smfProgramEvent);
    connect(m_smf, &QSmf::signalSMFChanPress, this, &Sequence::smfChanPressEvent);
    connect(m_smf, &QSmf::signalSMFMetaMisc, this, &Sequence::smfMetaEvent);
    connect(m_smf, &QSmf::signalSMFSysex, this, &Sequence::smfSysexEvent);
    connect(m_smf, &QSmf::signalSMFText, this, &Sequence::smfUpdateLoadProgress);
    connect(m_smf, &QSmf::signalSMFTempo, this, &Sequence::smfTempoEvent);
    connect(m_smf, &QSmf::signalSMFTrackStart, this, &Sequence::smfTrackStartEvent);
    connect(m_smf, &QSmf::signalSMFTrackEnd, this, &Sequence::smfTrackEnd);
    connect(m_smf, &QSmf::signalSMFendOfTrack, this, &Sequence::smfUpdateLoadProgress);
    connect(m_smf, &QSmf::signalSMFError, this, &Sequence::smfErrorHandler);

    m_wrk = new QWrk(this);
    connect(m_wrk, &QWrk::signalWRKError, this, &Sequence::wrkErrorHandler);
    connect(m_wrk, &QWrk::signalWRKUnknownChunk, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKHeader, this, &Sequence::wrkFileHeader);
    connect(m_wrk, &QWrk::signalWRKEnd, this, &Sequence::wrkEndOfFile);
    connect(m_wrk, &QWrk::signalWRKStreamEnd, this, &Sequence::wrkStreamEndEvent);
    connect(m_wrk, &QWrk::signalWRKGlobalVars, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKTrack, this, &Sequence::wrkTrackHeader);
    connect(m_wrk, &QWrk::signalWRKTimeBase, this, &Sequence::wrkTimeBase);
    connect(m_wrk, &QWrk::signalWRKNote, this, &Sequence::wrkNoteEvent);
    connect(m_wrk, &QWrk::signalWRKKeyPress, this, &Sequence::wrkKeyPressEvent);
    connect(m_wrk, &QWrk::signalWRKCtlChange, this, &Sequence::wrkCtlChangeEvent);
    connect(m_wrk, &QWrk::signalWRKPitchBend, this, &Sequence::wrkPitchBendEvent);
    connect(m_wrk, &QWrk::signalWRKProgram, this, &Sequence::wrkProgramEvent);
    connect(m_wrk, &QWrk::signalWRKChanPress, this, &Sequence::wrkChanPressEvent);
    connect(m_wrk, &QWrk::signalWRKSysexEvent, this, &Sequence::wrkSysexEvent);
    connect(m_wrk, &QWrk::signalWRKSysex, this, &Sequence::wrkSysexEventBank);
    connect(m_wrk, &QWrk::signalWRKText, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKTimeSig, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKKeySig, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKTempo, this, &Sequence::wrkTempoEvent);
    connect(m_wrk, &QWrk::signalWRKTrackPatch, this, &Sequence::wrkTrackPatch);
    connect(m_wrk, &QWrk::signalWRKComments, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKVariableRecord, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKNewTrack, this, &Sequence::wrkNewTrackHeader);
    connect(m_wrk, &QWrk::signalWRKTrackName, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKTrackVol, this, &Sequence::wrkTrackVol);
    connect(m_wrk, &QWrk::signalWRKTrackBank, this, &Sequence::wrkTrackBank);
    connect(m_wrk, &QWrk::signalWRKSegment, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKChord, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKExpression, this, &Sequence::wrkUpdateLoadProgress);

    for(int i=0; i<MIDI_STD_CHANNELS; ++i) {
        m_channelUsed[i] = false;
        m_channelEvents[i] = 0;
        m_channelPatches[i] = -1;
    }
}

static inline bool eventLessThan(const MIDIEvent* s1, const MIDIEvent *s2)
{
    return s1->tick() < s2->tick();
}

void Sequence::sort()
{
    //qDebug() << Q_FUNC_INFO;
    qStableSort(m_list.begin(), m_list.end(), eventLessThan);
    // Calculate deltas
    long lastEventTicks = 0;
    foreach(MIDIEvent* ev, m_list) {
        ev->setDelta(ev->tick() - lastEventTicks);
        lastEventTicks = ev->tick();
    }
}

void Sequence::clear()
{
    //qDebug() << Q_FUNC_INFO;
    m_lblName.clear();
    m_ticksDuration = 0;
    m_division = -1;
    m_track = -1;
    m_pos = 0;
    m_tempo = 500000.0;
    m_trackMap.clear();
    m_savedSysexEvents.clear();
    while (!m_list.isEmpty()) {
        delete m_list.takeFirst();
    }
}

Sequence::~Sequence()
{
    clear();
}

bool Sequence::isEmpty()
{
    return m_list.isEmpty();
}

void Sequence::loadPattern(QList<MIDIEvent*> pattern)
{
    clear();
    m_list = pattern;
}

void Sequence::loadFile(const QString& fileName)
{
    QFileInfo finfo(fileName);
    if (finfo.exists()) {
        clear();
        m_lastBeat = 0;
        m_barCount = 0;
        m_beatCount = 0;
        m_beatMax = 4;
        m_lowestMidiNote = 127;
        m_highestMidiNote = 0;
        for(int i=0; i<MIDI_STD_CHANNELS; ++i) {
            m_channelUsed[i] = false;
            m_channelEvents[i] = 0;
            m_channelLabel[i].clear();
            m_channelPatches[i] = -1;
            m_trackLabel.clear();
        }
        try {
            emit loadingStart(finfo.size());
            QString ext = finfo.suffix().toLower();
            if (ext == "wrk") {
                m_wrk->readFromFile(fileName);
            } else if (ext == "mid" || ext == "midi" || ext == "kar") {
                m_smf->readFromFile(fileName);
            }
            emit loadingFinished();
            if (!m_list.isEmpty()) {
                sort();
                m_lblName = finfo.fileName();
            }
        } catch (...) {
            qDebug() << "corrupted file";
            clear();
        }
    }
}

void Sequence::timeCalculations()
{
    m_ticks2millis = m_tempo / (1000.0 * m_division * m_tempoFactor);
    //qDebug() << Q_FUNC_INFO << "tempo:" << m_tempo << "div:" << m_division << "ticks2millis:" << m_ticks2millis;
}

qreal Sequence::tempoFactor() const
{
    return m_tempoFactor;
}

void Sequence::setTempoFactor(const qreal factor)
{
    if (m_tempoFactor != factor && factor >= 0.1 && factor <= 10.0) {
        //qDebug() << Q_FUNC_INFO << factor;
        m_tempoFactor = factor;
        timeCalculations();
    }
}

MIDIEvent *Sequence::nextEvent()
{
    if(m_pos < m_list.count()) {
        MIDIEvent* ev = m_list[m_pos++];
        return ev;
    }
    return 0;
}

std::chrono::milliseconds Sequence::deltaTimeOfEvent(MIDIEvent *ev) const
{
    return std::chrono::milliseconds(std::lround(ev->delta() * m_ticks2millis));
}

std::chrono::milliseconds Sequence::timeOfTicks(const int ticks) const
{
    return std::chrono::milliseconds(std::lround(ticks * m_ticks2millis));
}

bool Sequence::hasMoreEvents()
{
    return m_pos < m_list.count();
}

void Sequence::resetPosition()
{
    m_pos = 0;
}

void Sequence::setTickPosition(long tick) {
    for(int i=0; i<m_list.count(); ++i) {
        MIDIEvent* ev = m_list[i];
        if (ev->tick() > tick) {
            m_pos = i > 0 ? i -1 : 0;
            return;
        }
    }
    m_pos = m_list.count() -1 ;
}

void Sequence::setTimePosition(long time) {
    long lastTime = 0;
    for(int i=0; i<m_list.count(); ++i) {
        MIDIEvent* ev = m_list[i];
        long deltaTicks = ev->delta();
        long deltaMillis = std::lround(m_ticks2millis * deltaTicks);
        long eventMillis = lastTime + deltaMillis;
        if (eventMillis > time) {
            m_pos = i > 0 ? i -1 : 0;
            return;
        }
        lastTime = eventMillis;
    }
    m_pos = m_list.count() -1 ;
}

qreal Sequence::currentTempo() const
{
    return m_tempo / m_tempoFactor;
}

int Sequence::songLengthTicks() const
{
    return m_ticksDuration;
}

void Sequence::updateTempo(qreal newTempo)
{
    if (m_tempo != newTempo) {
        //qDebug() << Q_FUNC_INFO << newTempo;
        m_tempo = newTempo;
        timeCalculations();
    }
}

/* **************************************** *
 * SMF (Standard MIDI file) format handling
 * **************************************** */

void Sequence::smfUpdateLoadProgress()
{
    emit loadingProgress(m_smf->getFilePos());
}

void Sequence::appendSMFEvent(MIDIEvent *ev)
{
    long ticks = m_smf->getCurrentTime();
    ev->setTick(ticks);
    ev->setTag(m_track);
    m_list.append(ev);
    if (ticks > m_ticksDuration) {
        m_ticksDuration = ticks;
    }
    //qDebug() << "tics:" << ticks << "status:" << ev->status();
    smfUpdateLoadProgress();
}

void Sequence::smfHeaderEvent(int format, int ntrks, int division)
{
    Q_UNUSED(format)
    Q_UNUSED(ntrks)
    //qDebug() << "SMF Header:" << QString("Format=%1, Tracks=%2, Division=%3").arg(format).arg(ntrks).arg(division);
    m_division = division;
    timeCalculations();
    smfUpdateLoadProgress();
}

void Sequence::smfNoteOnEvent(int chan, int pitch, int vol)
{
    if (pitch > m_highestMidiNote)
        m_highestMidiNote = pitch;
    if (pitch < m_lowestMidiNote)
        m_lowestMidiNote = pitch;
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new NoteOnEvent (chan, pitch, vol);
    appendSMFEvent(ev);
}

void Sequence::smfNoteOffEvent(int chan, int pitch, int vol)
{
    if (pitch > m_highestMidiNote)
        m_highestMidiNote = pitch;
    if (pitch < m_lowestMidiNote)
        m_lowestMidiNote = pitch;
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new NoteOffEvent (chan, pitch, vol);
    appendSMFEvent(ev);
}

void Sequence::smfKeyPressEvent(int chan, int pitch, int press)
{
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new KeyPressEvent (chan, pitch, press);
    appendSMFEvent(ev);
}

void Sequence::smfCtlChangeEvent(int chan, int ctl, int value)
{
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new ControllerEvent (chan, ctl, value);
    appendSMFEvent(ev);
}

void Sequence::smfPitchBendEvent(int chan, int value)
{
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new PitchBendEvent (chan, value);
    appendSMFEvent(ev);
}

void Sequence::smfProgramEvent(int chan, int patch)
{
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new ProgramChangeEvent (chan, patch);
    appendSMFEvent(ev);
}

void Sequence::smfChanPressEvent(int chan, int press)
{
    m_channelUsed[chan] = true;
    m_channelEvents[chan]++;
    MIDIEvent* ev = new ChanPressEvent (chan, press);
    appendSMFEvent(ev);
}

void Sequence::smfSysexEvent(const QByteArray& data)
{
    MIDIEvent* ev = new SysExEvent (data);
    appendSMFEvent(ev);
}

void Sequence::smfMetaEvent(int type, const QByteArray& data)
{
    if ( (type >= Sequence::FIRST_TYPE) && (type <= Sequence::Cue) ) {
        //qint64 tick = m_smf->getCurrentTime();
        //addMetaData(static_cast<Sequence::TextType>(type), data, tick);
        switch ( type ) {
        case Sequence::Lyric:
        case Sequence::Text:
            if ((data.length() > 0) && (data[0] != '@') && (data[0] != '%') ) {
                TextEvent *ev = new TextEvent(data, type);
                ev->setTag(type);
                appendSMFEvent(ev);
            }
            break;
        case Sequence::TrackName:
        case Sequence::InstrumentName:
            if (m_trackLabel.isEmpty()) {
                m_trackLabel = data;
            }
            break;
        }
    }
}

void Sequence::smfTempoEvent(int tempo)
{
    //qDebug() << Q_FUNC_INFO << tempo;
    MIDIEvent* ev = new TempoEvent(tempo);
    appendSMFEvent(ev);
    if (ev->tick() == 0) {
        updateTempo(tempo);
    }
}

void Sequence::smfTrackStartEvent()
{
    int tick = m_smf->getCurrentTime();
    if (tick > m_ticksDuration) {
        m_ticksDuration = tick;
    }
    m_track++;
    m_trackLabel.clear();
    for(int i=0; i<MIDI_STD_CHANNELS; ++i) {
        m_channelEvents[i] = 0;
    }
    //qDebug() << "starting track:" << m_track << "ticks:" << tick;
    smfUpdateLoadProgress();
}

void Sequence::smfTrackEnd()
{
    int max = 0;
    int chan = -1;
    //qDebug() << Q_FUNC_INFO << m_trackLabel;
    if (!m_trackLabel.isEmpty()) {
        for(int i=0; i<MIDI_STD_CHANNELS; ++i)
            if (m_channelEvents[i] > max) {
                max = m_channelEvents[i];
                chan = i;
            }
        //qDebug() << Q_FUNC_INFO << m_trackLabel << chan;
        if (chan >= 0 && chan < MIDI_STD_CHANNELS)
            m_channelLabel[chan] = m_trackLabel;
    }
    smfUpdateLoadProgress();
}

void Sequence::smfErrorHandler(const QString& errorStr)
{
    qWarning() << QString("%1 at file offset %2").arg(errorStr).arg(m_smf->getFilePos());
}

/* ********************************* *
 * Cakewalk WRK file format handling
 * ********************************* */

void Sequence::wrkUpdateLoadProgress()
{
    emit loadingProgress(m_wrk->getFilePos());
}

void Sequence::appendWRKEvent(long ticks, MIDIEvent* ev)
{
    ev->setTick(ticks);
    m_list.append(ev);
    if (ticks > m_ticksDuration) {
        m_ticksDuration = ticks;
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkErrorHandler(const QString& errorStr)
{
    qWarning() << QString("%1 at file offset %2<br>")
        .arg(errorStr).arg(m_wrk->getFilePos());
}

void Sequence::wrkFileHeader(int /*verh*/, int /*verl*/)
{
    m_track = 0;
    m_division = 120;
    wrkUpdateLoadProgress();
}

void Sequence::wrkTimeBase(int timebase)
{
    m_division = timebase;
    wrkUpdateLoadProgress();
}

void Sequence::wrkStreamEndEvent(long time)
{
    if (time > m_ticksDuration) {
        m_ticksDuration = time;
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkTrackHeader( const QString& /*name1*/,
                           const QString& /*name2*/,
                           int trackno, int channel,
                           int pitch, int velocity, int /*port*/,
                           bool /*selected*/, bool /*muted*/, bool /*loop*/ )
{
    TrackMapRec rec;
    rec.channel = channel;
    rec.pitch = pitch;
    rec.velocity = velocity;
    m_trackMap[trackno] = rec;
    wrkUpdateLoadProgress();
}

void Sequence::wrkNoteEvent(int track, long time, int chan, int pitch, int vol, int dur)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    int key = pitch + rec.pitch;
    int velocity = vol + rec.velocity;
    if (rec.channel > -1)
        channel = rec.channel;
    if (pitch > m_highestMidiNote)
        m_highestMidiNote = pitch;
    if (pitch < m_lowestMidiNote)
        m_lowestMidiNote = pitch;
    m_channelUsed[channel] = true;
    MIDIEvent* ev = new NoteOnEvent(channel, key, velocity);
    ev->setTag(track);
    appendWRKEvent(time, ev);
    m_channelEvents[channel]++;
    ev = new NoteOffEvent(channel, key, velocity);
    ev->setTag(track);
    appendWRKEvent(time + dur, ev);
    m_channelEvents[channel]++;
}

void Sequence::wrkKeyPressEvent(int track, long time, int chan, int pitch, int press)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    int key = pitch + rec.pitch;
    if (rec.channel > -1)
        channel = rec.channel;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new KeyPressEvent(channel, key, press);
    ev->setTag(track);
    appendWRKEvent(time, ev);
}

void Sequence::wrkCtlChangeEvent(int track, long time, int chan, int ctl, int value)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ControllerEvent(channel, ctl, value);
    ev->setTag(track);
    appendWRKEvent(time, ev);
}

void Sequence::wrkPitchBendEvent(int track, long time, int chan, int value)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new PitchBendEvent(channel, value);
    ev->setTag(track);
    appendWRKEvent(time, ev);
}

void Sequence::wrkProgramEvent(int track, long time, int chan, int patch)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ProgramChangeEvent(channel, patch);
    ev->setTag(track);
    appendWRKEvent(time, ev);
}

void Sequence::wrkChanPressEvent(int track, long time, int chan, int press)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ChanPressEvent(channel, press);
    ev->setTag(track);
    appendWRKEvent(time, ev);
}

void Sequence::wrkSysexEvent(int track, long time, int bank)
{
    SysexEventRec rec;
    rec.track = track;
    rec.time = time;
    rec.bank = bank;
    m_savedSysexEvents.append(rec);
    wrkUpdateLoadProgress();
}

void Sequence::wrkSysexEventBank(int bank, const QString& /*name*/,
        bool autosend, int /*port*/, const QByteArray& data)
{
    SysExEvent* ev = new SysExEvent(data);
    if (autosend)
        appendWRKEvent(0, ev->clone());
    foreach(const SysexEventRec& rec, m_savedSysexEvents) {
        if (rec.bank == bank) {
            appendWRKEvent(rec.time, ev->clone());
        }
    }
    delete ev;
    wrkUpdateLoadProgress();
}

void Sequence::wrkTempoEvent(long time, int tempo)
{
    double bpm = tempo / 100.0;
    MIDIEvent* ev = new TempoEvent(qRound ( 6e7 / bpm ) );
    appendWRKEvent(time, ev);
    if (time == 0) {
        updateTempo(tempo);
    }
}

void Sequence::wrkTrackPatch(int track, int patch)
{
    int channel = 0;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    wrkProgramEvent(track, 0, channel, patch);
}

void Sequence::wrkNewTrackHeader( const QString& /*name*/,
                              int trackno, int channel,
                              int pitch, int velocity, int /*port*/,
                              bool /*selected*/, bool /*muted*/, bool /*loop*/ )
{
    TrackMapRec rec;
    rec.channel = channel;
    rec.pitch = pitch;
    rec.velocity = velocity;
    m_trackMap[trackno] = rec;
    wrkUpdateLoadProgress();
}

void Sequence::wrkTrackVol(int track, int vol)
{
    int channel = 0;
    int lsb, msb;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    if (vol < 128)
        wrkCtlChangeEvent(track, 0, channel, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, vol);
    else {
        lsb = vol % 0x80;
        msb = vol / 0x80;
        wrkCtlChangeEvent(track, 0, channel, ControllerEvent::MIDI_CTL_LSB_MAIN_VOLUME, lsb);
        wrkCtlChangeEvent(track, 0, channel, ControllerEvent::MIDI_CTL_MSB_MAIN_VOLUME, msb);
    }
}

void Sequence::wrkTrackBank(int track, int bank)
{
    // assume GM/GS bank method
    int channel = 0;
    int lsb, msb;
    TrackMapRec rec = m_trackMap[track];
    if (rec.channel > -1)
        channel = rec.channel;
    lsb = bank % 0x80;
    msb = bank / 0x80;
    wrkCtlChangeEvent(track, 0, channel, ControllerEvent::MIDI_CTL_MSB_BANK, msb);
    wrkCtlChangeEvent(track, 0, channel, ControllerEvent::MIDI_CTL_LSB_BANK, lsb);
}

void Sequence::wrkEndOfFile()
{
    wrkUpdateLoadProgress();
}

bool Sequence::channelUsed(int channel)
{
    if (channel >= 0 && channel < MIDI_STD_CHANNELS)
        return m_channelUsed[channel];
    return false;
}

QString Sequence::channelLabel(int channel)
{
    if ((channel >= 0) && (channel < MIDI_STD_CHANNELS) &&
        !m_channelLabel[channel].isEmpty()) {
        if (m_codec == NULL)
            return QString(m_channelLabel[channel]);
        else
            return m_codec->toUnicode(m_channelLabel[channel]);
    }
    return QString();
}

int Sequence::lowestMidiNote()
{
    return m_lowestMidiNote;
}

int Sequence::highestMidiNote()
{
    return m_highestMidiNote;
}
