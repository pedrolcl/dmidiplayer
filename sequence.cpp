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

#include <QTextStream>
#include <QtMath>
#include <QFileInfo>
#include <QDebug>
#include "sequence.h"

using namespace drumstick::File;

Sequence::Sequence(QObject *parent) : QObject(parent),
    m_ticksDuration(0),
    m_lastEventTicks(0),
    m_lastEventTime(0),
    m_division(-1),
    m_tempo(500000),
    m_stopped(true),
    m_pos(0),
    m_track(-1),
    m_tempoFactor(1.0)
{
    m_smf = new QSmf(this);
    connect(m_smf, SIGNAL(signalSMFHeader(int,int,int)),
                   SLOT(smfHeaderEvent(int,int,int)));
    connect(m_smf, SIGNAL(signalSMFNoteOn(int,int,int)),
                   SLOT(smfNoteOnEvent(int,int,int)));
    connect(m_smf, SIGNAL(signalSMFNoteOff(int,int,int)),
                   SLOT(smfNoteOffEvent(int,int,int)));
    connect(m_smf, SIGNAL(signalSMFKeyPress(int,int,int)),
                   SLOT(smfKeyPressEvent(int,int,int)));
    connect(m_smf, SIGNAL(signalSMFCtlChange(int,int,int)),
                   SLOT(smfCtlChangeEvent(int,int,int)));
    connect(m_smf, SIGNAL(signalSMFPitchBend(int,int)),
                   SLOT(smfPitchBendEvent(int,int)));
    connect(m_smf, SIGNAL(signalSMFProgram(int,int)),
                   SLOT(smfProgramEvent(int,int)));
    connect(m_smf, SIGNAL(signalSMFChanPress(int,int)),
                   SLOT(smfChanPressEvent(int,int)));
    connect(m_smf, SIGNAL(signalSMFSysex(const QByteArray&)),
                   SLOT(smfSysexEvent(const QByteArray&)));
    connect(m_smf, SIGNAL(signalSMFText(int,const QString&)),
                   SLOT(smfUpdateLoadProgress()));
    connect(m_smf, SIGNAL(signalSMFTempo(int)),
                   SLOT(smfTempoEvent(int)));
    connect(m_smf, SIGNAL(signalSMFTrackStart()),
                   SLOT(smfUpdateLoadProgress()));
    connect(m_smf, SIGNAL(signalSMFTrackEnd()),
                   SLOT(smfUpdateLoadProgress()));
    connect(m_smf, SIGNAL(signalSMFendOfTrack()),
                   SLOT(smfUpdateLoadProgress()));
    connect(m_smf, SIGNAL(signalSMFError(const QString&)),
                   SLOT(smfErrorHandler(const QString&)));

    m_wrk = new QWrk(this);
    connect(m_wrk, SIGNAL(signalWRKError(const QString&)),
                   SLOT(wrkErrorHandler(const QString&)));
    connect(m_wrk, SIGNAL(signalWRKUnknownChunk(int,const QByteArray&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKHeader(int,int)),
                   SLOT(wrkFileHeader(int,int)));
    connect(m_wrk, SIGNAL(signalWRKEnd()),
                   SLOT(wrkEndOfFile()));
    connect(m_wrk, SIGNAL(signalWRKStreamEnd(long)),
                   SLOT(wrkStreamEndEvent(long)));
    connect(m_wrk, SIGNAL(signalWRKGlobalVars()),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKTrack(const QString&, const QString&, int,int,int,int,int,bool,bool,bool)),
                   SLOT(wrkTrackHeader(const QString&, const QString&, int,int,int,int,int,bool,bool,bool)));
    connect(m_wrk, SIGNAL(signalWRKTimeBase(int)),
                   SLOT(wrkTimeBase(int)));
    connect(m_wrk, SIGNAL(signalWRKNote(int,long,int,int,int,int)),
                   SLOT(wrkNoteEvent(int,long,int,int,int,int)));
    connect(m_wrk, SIGNAL(signalWRKKeyPress(int,long,int,int,int)),
                   SLOT(wrkKeyPressEvent(int,long,int,int,int)));
    connect(m_wrk, SIGNAL(signalWRKCtlChange(int,long,int,int,int)),
                   SLOT(wrkCtlChangeEvent(int,long,int,int,int)));
    connect(m_wrk, SIGNAL(signalWRKPitchBend(int,long,int,int)),
                   SLOT(wrkPitchBendEvent(int,long,int,int)));
    connect(m_wrk, SIGNAL(signalWRKProgram(int,long,int,int)),
                   SLOT(wrkProgramEvent(int,long,int,int)));
    connect(m_wrk, SIGNAL(signalWRKChanPress(int,long,int,int)),
                   SLOT(wrkChanPressEvent(int,long,int,int)));
    connect(m_wrk, SIGNAL(signalWRKSysexEvent(int,long,int)),
                   SLOT(wrkSysexEvent(int,long,int)));
    connect(m_wrk, SIGNAL(signalWRKSysex(int,const QString&,bool,int,const QByteArray&)),
                   SLOT(wrkSysexEventBank(int,const QString&,bool,int,const QByteArray&)));
    connect(m_wrk, SIGNAL(signalWRKText(int,long,int,const QString&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKTimeSig(int,int,int)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKKeySig(int,int)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKTempo(long,int)),
                   SLOT(wrkTempoEvent(long,int)));
    connect(m_wrk, SIGNAL(signalWRKTrackPatch(int,int)),
                   SLOT(wrkTrackPatch(int,int)));
    connect(m_wrk, SIGNAL(signalWRKComments(const QString&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKVariableRecord(const QString&,const QByteArray&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKNewTrack(const QString&,int,int,int,int,int,bool,bool,bool)),
                   SLOT(wrkNewTrackHeader(const QString&,int,int,int,int,int,bool,bool,bool)));
    connect(m_wrk, SIGNAL(signalWRKTrackName(int,const QString&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKTrackVol(int,int)),
                   SLOT(wrkTrackVol(int,int)));
    connect(m_wrk, SIGNAL(signalWRKTrackBank(int,int)),
                   SLOT(wrkTrackBank(int,int)));
    connect(m_wrk, SIGNAL(signalWRKSegment(int,long,const QString&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKChord(int,long,const QString&,const QByteArray&)),
                   SLOT(wrkUpdateLoadProgress()));
    connect(m_wrk, SIGNAL(signalWRKExpression(int,long,int,const QString&)),
                   SLOT(wrkUpdateLoadProgress()));
}

static inline bool eventLessThan(const MIDIEvent* s1, const MIDIEvent *s2)
{
    return s1->tick() < s2->tick();
}

void Sequence::sort()
{
    qStableSort(m_list.begin(), m_list.end(), eventLessThan);
}

void Sequence::clear()
{
    m_lblName.clear();
    m_ticksDuration = 0;
    m_lastEventTicks = 0;
    m_lastEventTime= 0;
    m_division = -1;
    m_stopped = true;
    m_track = -1;
    m_pos = 0;
    m_tempo = 500000;
    m_trackMap.clear();
    m_savedSysexEvents.clear();
    while (!m_list.isEmpty()) {
        delete m_list.takeFirst();
    }
    timeCalculations();
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
            clear();
        }
    }
}

void Sequence::timeCalculations()
{
    m_ticks2millis = (1.0 * m_tempo) / (1000.0 * m_division * m_tempoFactor);
}

qreal Sequence::tempoFactor() const
{
    return m_tempoFactor;
}

void Sequence::setTempoFactor(const qreal factor)
{
    if (m_tempoFactor != factor && factor >= 0.1 && factor <= 10.0) {
        m_tempoFactor = factor;
        timeCalculations();
    }
}

MIDIEvent *Sequence::nextEvent()
{
    if(m_pos < m_list.count()) {
        MIDIEvent* ev = m_list[m_pos++];
        m_lastEventTime = eventTime(ev);
        m_lastEventTicks = ev->tick();
        return ev;
    }
    return 0;
}

int Sequence::nextEventDeltaTime()
{
    if(m_pos < m_list.count()) {
        MIDIEvent* ev = m_list[m_pos];
        int deltaTicks = ev->tick() - m_lastEventTicks;
        int deltaMillis = qFloor(m_ticks2millis * deltaTicks);
        return deltaMillis;
    }
    return 0;
}

int Sequence::eventTime(MIDIEvent* ev) const
{
    int deltaTicks = ev->tick() - m_lastEventTicks;
    int deltaMillis = qFloor(m_ticks2millis * deltaTicks);
    return m_lastEventTime + deltaMillis;
}

int Sequence::nextEventTime()
{
    if(m_pos < m_list.count()) {
        MIDIEvent* ev = m_list[m_pos];
        int time = eventTime(ev);
        return time;
    }
    return 0;
}

int Sequence::nextEventTicks()
{
    if(m_pos < m_list.count()) {
        MIDIEvent* ev = m_list[m_pos];
        return ev->tick();
    }
    return 0;
}

bool Sequence::hasMoreEvents()
{
    return m_pos < m_list.count();
}

void Sequence::resetPosition()
{
    m_pos = 0;
}

void Sequence::setTickPosition(int tick) {
    for(int i=0; i<m_list.count(); ++i) {
        MIDIEvent* ev = m_list[i];
        if (ev->tick() > tick) {
            m_pos = i > 0 ? i -1 : 0;
            return;
        }
    }
    m_pos = m_list.count() -1 ;
}

void Sequence::setTimePosition(int time) {
    int lastTicks = 0;
    int lastTime = 0;
    for(int i=0; i<m_list.count(); ++i) {
        MIDIEvent* ev = m_list[i];
        int deltaTicks = ev->tick() - lastTicks;
        int deltaMillis = qFloor(m_ticks2millis * deltaTicks);
        int eventMillis = lastTime + deltaMillis;
        if (eventMillis > time) {
            m_pos = i > 0 ? i -1 : 0;
            return;
        }
        lastTicks = ev->tick();
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

void Sequence::updateTempo(int newTempo)
{
    m_tempo = newTempo;
    timeCalculations();
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
    long tick = m_smf->getCurrentTime();
    long millis = long(m_smf->getRealTime() / 1.6);
    ev->setTick(tick);
    ev->setMillis(millis);
    ev->setTag(m_track);
    m_list.append(ev);
    if (tick > m_ticksDuration) {
        m_ticksDuration = tick;
    }
    smfUpdateLoadProgress();
}

void Sequence::smfHeaderEvent(int /*format*/, int /*ntrks*/, int division)
{
    m_division = division;
    timeCalculations();
    //qDebug() << "SMF Header:" << QString("Format=%1, Tracks=%2, Division=%3").arg(format).arg(ntrks).arg(division);
    smfUpdateLoadProgress();
}

void Sequence::smfNoteOnEvent(int chan, int pitch, int vol)
{
    MIDIEvent* ev = new NoteOnEvent (chan, pitch, vol);
    appendSMFEvent(ev);
}

void Sequence::smfNoteOffEvent(int chan, int pitch, int vol)
{
    MIDIEvent* ev = new NoteOffEvent (chan, pitch, vol);
    appendSMFEvent(ev);
}

void Sequence::smfKeyPressEvent(int chan, int pitch, int press)
{
    MIDIEvent* ev = new KeyPressEvent (chan, pitch, press);
    appendSMFEvent(ev);
}

void Sequence::smfCtlChangeEvent(int chan, int ctl, int value)
{
    MIDIEvent* ev = new ControllerEvent (chan, ctl, value);
    appendSMFEvent(ev);
}

void Sequence::smfPitchBendEvent(int chan, int value)
{
    MIDIEvent* ev = new PitchBendEvent (chan, value);
    appendSMFEvent(ev);
}

void Sequence::smfProgramEvent(int chan, int patch)
{
    MIDIEvent* ev = new ProgramChangeEvent (chan, patch);
    appendSMFEvent(ev);
}

void Sequence::smfChanPressEvent(int chan, int press)
{
    MIDIEvent* ev = new ChanPressEvent (chan, press);
    appendSMFEvent(ev);
}

void Sequence::smfSysexEvent(const QByteArray& data)
{
    MIDIEvent* ev = new SysExEvent (data);
    appendSMFEvent(ev);
}

void Sequence::smfTempoEvent(int tempo)
{
    MIDIEvent* ev = new TempoEvent(tempo);
    appendSMFEvent(ev);
}

void Sequence::smfTrackStartEvent()
{
    int tick = m_smf->getCurrentTime();
    if (tick > m_ticksDuration) {
        m_ticksDuration = tick;
    }
    m_track++;
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
    MIDIEvent* ev = new NoteOnEvent(channel, key, velocity);
    ev->setTag(track);
    appendWRKEvent(time, ev);
    ev = new NoteOffEvent(channel, key, velocity);
    ev->setTag(track);
    appendWRKEvent(time + dur, ev);
}

void Sequence::wrkKeyPressEvent(int track, long time, int chan, int pitch, int press)
{
    int channel = chan;
    TrackMapRec rec = m_trackMap[track];
    int key = pitch + rec.pitch;
    if (rec.channel > -1)
        channel = rec.channel;
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
