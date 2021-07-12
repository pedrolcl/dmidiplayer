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

#include <QDebug>
#include <QtMath>
#include <QFileInfo>
#include <QRegularExpression>
#include "sequence.h"

using namespace drumstick::File;
using namespace drumstick::rt;

Sequence::Sequence(QObject *parent) : QObject(parent),
    m_smf(nullptr),
    m_wrk(nullptr),
    m_uchardetErrors(0),
    m_format(0),
    m_numTracks(0),
    m_ticksDuration(0),
    m_division(-1),
    m_pos(0),
    m_curTrack(-1),
    m_beatMax(0),
    m_barCount(0),
    m_beatCount(0),
    m_lowestMidiNote(127),
    m_highestMidiNote(0),
    m_tempo(500000.0),
    m_tempoFactor(1.0),
    m_ticks2millis(0),
    m_duration(0),
    m_lastBeat(0),
    m_beatLength(0),
    m_tick(0)
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
    connect(m_smf, &QSmf::signalSMFTimeSig, this, &Sequence::smfTimeSigEvent);
    connect(m_smf, &QSmf::signalSMFKeySig, this, &Sequence::smfKeySig);

    m_wrk = new QWrk(this);
    connect(m_wrk, &QWrk::signalWRKError, this, &Sequence::wrkErrorHandler);
    connect(m_wrk, &QWrk::signalWRKUnknownChunk, this, &Sequence::wrkUpdateLoadProgress);
    connect(m_wrk, &QWrk::signalWRKHeader, this, &Sequence::wrkFileHeader);
    connect(m_wrk, &QWrk::signalWRKEnd, this, &Sequence::wrkEndOfFile);
    connect(m_wrk, &QWrk::signalWRKStreamEnd, this, &Sequence::wrkStreamEndEvent);
    connect(m_wrk, &QWrk::signalWRKGlobalVars, this, &Sequence::wrkGlobalVars);
    connect(m_wrk, &QWrk::signalWRKTrack2, this, &Sequence::wrkTrackHeader);
    connect(m_wrk, &QWrk::signalWRKTimeBase, this, &Sequence::wrkTimeBase);
    connect(m_wrk, &QWrk::signalWRKNote, this, &Sequence::wrkNoteEvent);
    connect(m_wrk, &QWrk::signalWRKKeyPress, this, &Sequence::wrkKeyPressEvent);
    connect(m_wrk, &QWrk::signalWRKCtlChange, this, &Sequence::wrkCtlChangeEvent);
    connect(m_wrk, &QWrk::signalWRKPitchBend, this, &Sequence::wrkPitchBendEvent);
    connect(m_wrk, &QWrk::signalWRKProgram, this, &Sequence::wrkProgramEvent);
    connect(m_wrk, &QWrk::signalWRKChanPress, this, &Sequence::wrkChanPressEvent);
    connect(m_wrk, &QWrk::signalWRKSysexEvent, this, &Sequence::wrkSysexEvent);
    connect(m_wrk, &QWrk::signalWRKSysex, this, &Sequence::wrkSysexEventBank);
    connect(m_wrk, &QWrk::signalWRKText2, this, &Sequence::wrkTextEvent);
    connect(m_wrk, &QWrk::signalWRKTimeSig, this, &Sequence::wrkTimeSignatureEvent);
    connect(m_wrk, &QWrk::signalWRKKeySig, this, &Sequence::wrkKeySig);
    connect(m_wrk, &QWrk::signalWRKTempo, this, &Sequence::wrkTempoEvent);
    connect(m_wrk, &QWrk::signalWRKTrackPatch, this, &Sequence::wrkTrackPatch);
    connect(m_wrk, &QWrk::signalWRKComments2, this, &Sequence::wrkComments);
    connect(m_wrk, &QWrk::signalWRKVariableRecord, this, &Sequence::wrkVariableRecord);
    connect(m_wrk, &QWrk::signalWRKNewTrack2, this, &Sequence::wrkNewTrackHeader);
    connect(m_wrk, &QWrk::signalWRKTrackName2, this, &Sequence::wrkTrackName);
    connect(m_wrk, &QWrk::signalWRKTrackVol, this, &Sequence::wrkTrackVol);
    connect(m_wrk, &QWrk::signalWRKTrackBank, this, &Sequence::wrkTrackBank);
    connect(m_wrk, &QWrk::signalWRKSegment2, this, &Sequence::wrkSegment);
    connect(m_wrk, &QWrk::signalWRKChord, this, &Sequence::wrkChord);
    connect(m_wrk, &QWrk::signalWRKExpression2, this, &Sequence::wrkExpression);
    /* m_wrk->setTextCodec(nullptr); // important !!! */

    m_handle = uchardet_new();
    initCodecs();
    clear();
}

Sequence::~Sequence()
{
    //qDebug() << Q_FUNC_INFO;
    clear();
    uchardet_delete(m_handle);
}

void Sequence::initCodecs()
{
    QMap<QString,int> aux;
    foreach(const auto mib, QTextCodec::availableMibs()) {
        QTextCodec *c = QTextCodec::codecForMib(mib);
        if (c != nullptr && mib != 0) {
            aux.insert(c->name(), mib);
        }
    }
    QByteArrayList umibkeys = m_umibs.keys();
    foreach(const auto k, umibkeys) {
        auto mib = m_umibs[k];
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
    m_supportedCodecs.clear();
    foreach(const auto k, keys) {
        m_supportedCodecs.insert(k.toUpper().toLatin1(), aux[k]);
    }
    //qDebug() << Q_FUNC_INFO << m_supportedCodecs.count();
}

void Sequence::findCodec()
{
    m_mib = 0;
    m_codec = nullptr;
    if (m_supportedCodecs.contains(m_charset)) {
        m_mib = m_supportedCodecs[m_charset];
        m_codec = QTextCodec::codecForMib(m_mib);
    }
}

BeatEvent* Sequence::nearestBeatByTicks(int ticks)
{
    static const std::type_info& beatId = typeid(BeatEvent);
    BeatEvent *nearest = nullptr;
    foreach(MIDIEvent* ev, m_list) {
        if (ev->isMetaEvent() && typeid(*ev) == beatId) {
            if (ev->tick() <= ticks) {
                nearest = static_cast<BeatEvent*>(ev);
            } else {
                break;
            }
        }
    }
    return nearest;
}

BeatEvent *Sequence::nextBar(BeatEvent* latest)
{
    static const std::type_info& beatId = typeid(BeatEvent);
    BeatEvent *nearest = latest;
    if (nearest != nullptr) {
        int nextBar = nearest->bar() + 1;
        int from = m_list.indexOf(latest);
        for(int i = from; i < m_list.count(); ++i) {
            MIDIEvent* ev = m_list.at(i);
            if (ev->isMetaEvent() && typeid(*ev) == beatId) {
                nearest = static_cast<BeatEvent*>(ev);
                if ((nearest->bar() >= nextBar) && (nearest->beat() == 1)) {
                    break;
                }
            }
        }
    }
    return nearest;
}

BeatEvent *Sequence::previousBar(BeatEvent* latest)
{
    static const std::type_info& beatId = typeid(BeatEvent);
    BeatEvent *nearest = latest;
    if (nearest != nullptr) {
        int prevBar = nearest->bar() - 1;
        int from = m_list.indexOf(latest);
        for(int i = from; i >= 0; --i) {
            MIDIEvent* ev = m_list.at(i);
            if (ev->isMetaEvent() && typeid(*ev) == beatId) {
                nearest = static_cast<BeatEvent*>(ev);
                if ((nearest->bar() <= prevBar) && (nearest->beat() == 1)) {
                    break;
                }
            }
        }
    }
    return nearest;
}

BeatEvent *Sequence::jumpToBar(int bar)
{
    static const std::type_info& beatId = typeid(BeatEvent);
    BeatEvent *nearest = nullptr;
    for(int i = 0; i < m_list.count(); ++i) {
        MIDIEvent* ev = m_list.at(i);
        if (ev->isMetaEvent() && typeid(*ev) == beatId) {
            nearest = static_cast<BeatEvent*>(ev);
            if ((nearest->bar() >= bar) && (nearest->beat() == 1)) {
                break;
            }
        }
    }
    return nearest;
}

BeatEvent *Sequence::firstBeat()
{
    static const std::type_info& beatId = typeid(BeatEvent);
    BeatEvent *nearest = nullptr;
    for(int i = 0; i < m_list.count(); ++i) {
        MIDIEvent* ev = m_list.at(i);
        if (ev->isMetaEvent() && typeid(*ev) == beatId) {
            nearest = static_cast<BeatEvent*>(ev);
            break;
        }
    }
    return nearest;
}

int Sequence::lastBar()
{
    return m_barCount;
}

static inline bool eventLessThan(const MIDIEvent* s1, const MIDIEvent *s2)
{
    return s1->tick() < s2->tick();
}

void Sequence::sort()
{
    //qDebug() << Q_FUNC_INFO;
    std::stable_sort(m_list.begin(), m_list.end(), eventLessThan);
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
    m_curTrack = -1;
    m_pos = 0;
    m_tempo = 500000.0;
    m_tick = 0;
    m_lastBeat = 0;
    m_barCount = 0;
    m_beatCount = 0;
    m_beatMax = 4;
    m_lowestMidiNote = 127;
    m_highestMidiNote = 0;
    m_curTrack = 0;
    for(int i=0; i<MIDI_STD_CHANNELS; ++i) {
        m_channelUsed[i] = false;
        m_channelEvents[i] = 0;
        m_channelPatches[i] = -1;
        m_channelLabel[i].clear();
    }
    m_trackLabel.clear();
    m_trackMap.clear();
    m_savedSysexEvents.clear();
    m_charset.clear();
    m_textEvents.clear();
    m_trkScore.clear();
    m_typScore.clear();
    m_trkName.clear();
    m_trkChannel.clear();
    while (!m_list.isEmpty()) {
        delete m_list.takeFirst();
    }
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
            uchardet_reset(m_handle);
            emit loadingStart(finfo.size());
            QString ext = finfo.suffix().toLower();
            if (ext == "wrk") {
                m_wrk->readFromFile(fileName);
            } else if (ext == "mid" || ext == "midi" || ext == "kar") {
                m_smf->readFromFile(fileName);
            }
            uchardet_data_end(m_handle);
            emit loadingFinished();
            if (!m_list.isEmpty()) {
                sort();
                m_lblName = finfo.fileName();
                m_currentFile = finfo.fileName();
            }
            m_charset = QByteArray(uchardet_get_charset(m_handle));
            //qDebug() << Q_FUNC_INFO << "Detected charset:" << m_charset;
            findCodec();
        } catch (...) {
            qWarning() << "corrupted file";
            clear();
        }
    }
}

int Sequence::numUchardetErrors()
{
    return m_uchardetErrors;
}

int Sequence::detectedUchardetMIB() const
{
    return m_mib;
}

QByteArray Sequence::detectedCharset() const
{
    return m_charset;
}

QString Sequence::currentFile() const
{
    return m_currentFile;
}

int Sequence::getNumTracks() const
{
    return m_numTracks;
}

void Sequence::appendStringToList(QStringList &list, QString &s, TextType type)
{
    if (type == Text || type >= KarFileType)
        s.replace(QRegularExpression("@[IKLTVW]"), "\n");
    if (type == Text || type == Lyric)
        s.replace(QRegularExpression("[/\\\\]+"), "\n");
    s.replace(QRegularExpression("[\r\n]+"), "\n");
    s.replace('\0', QChar::Space);
    list.append(s);
}

QStringList Sequence::getText(const TextType type)
{
    QStringList output;
    if ( (type >= FIRST_TYPE) && (type <= LAST_TYPE) ) {
         foreach(const auto& e, m_textEvents) {
             if (e.m_type == type) {
                 QString s;
                 if (m_codec == nullptr) {
                    s = QString::fromLatin1(e.m_text);
                 } else {
                    s = m_codec->toUnicode(e.m_text);
                 }
                 appendStringToList(output, s, type);
             }
         }
     }
    return output;
}

QByteArrayList Sequence::getExtraCodecNames()
{
    return m_umibs.keys();
}

int Sequence::getMibForName(const QByteArray name)
{
    if (m_umibs.contains(name)) {
        return m_umibs[name];
    }
    return -1;
}

QList<QPair<int,QByteArray>> Sequence::getRawText(const int track, const TextType type)
{
    QList<QPair<int,QByteArray>> output;
    if ((type < TextType::KarFileType) && !m_textEvents.isEmpty()) {
        foreach(const auto &e, m_textEvents) {
            if ((track == 0 || e.m_track == track) &&
                (type == TextType::None || e.m_type == type))
            {
                output.append(QPair<int,QByteArray>(e.m_tick, e.m_text));
            }
        }
    }
    return output;
}

int Sequence::trackMaxPoints()
{
    int k = -1;
    auto values = m_trkScore.values();
    if (!values.isEmpty()) {
        int v = *std::max_element(values.begin(), values.end());
        k = m_trkScore.key(v, -1);
    }
    return k;
}

int Sequence::typeMaxPoints()
{
    int k = -1;
    auto values = m_typScore.values();
    if (!values.isEmpty()) {
        int v = *std::max_element(values.begin(), values.end());
        k = m_typScore.key(v, -1);
    }
    return k;
}

QByteArray Sequence::trackName(int track) const
{
    return m_trkName.value(track);
}

int Sequence::trackChannel(int track) const
{
    //qDebug() << "track:" << track << "channel:" << m_trkChannel.value(track);
    return m_trkChannel.value(track);
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

void Sequence::insertBeats(qint64 ticks)
{
    if ((ticks > m_tick) && (m_beatLength > 0)) {
        qint64 diff = ticks - m_lastBeat;
        while (diff >= m_beatLength) {
            MIDIEvent* ev = new BeatEvent(m_barCount, m_beatCount, m_beatMax);
            ev->setTick(m_lastBeat);
            m_list.append(ev);

            m_lastBeat += m_beatLength;
            diff -= m_beatLength;
            m_beatCount++;
            if (m_beatCount > m_beatMax) {
                m_beatCount = 1;
                m_barCount++;
            }
        }
        m_tick = ticks;
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
    ev->setTag(m_curTrack);
    m_list.append(ev);
    if (ticks > m_ticksDuration) {
        m_ticksDuration = ticks;
    }
    //qDebug() << "tics:" << ticks << "status:" << ev->status();
    insertBeats(ticks);
    smfUpdateLoadProgress();
}

void Sequence::smfHeaderEvent(int format, int ntrks, int division)
{
    //qDebug() << "SMF Header:" << QString("Format=%1, Tracks=%2, Division=%3").arg(format).arg(ntrks).arg(division);
    m_format = format;
    m_numTracks = ntrks;
    m_division = division;
    m_beatLength = m_division;
    m_beatMax = 4;
    m_lastBeat = 0;
    m_beatCount = 1;
    m_barCount = 1;
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

void Sequence::addMetaData(int time, int type, const QByteArray& data)
{
    //if ((data.length() > 0) && (data[0] == '%'))
        //return; // ignored?
    int retval = uchardet_handle_data(m_handle, data.data(), data.length());
    if (retval == 0)
    {
        if (m_trkScore.contains(m_curTrack)) {
            m_trkScore[m_curTrack]++;
        } else {
            m_trkScore[m_curTrack] = 1;
        }
        if (m_typScore.contains(type)) {
            m_typScore[type]++;
        } else {
            m_typScore[type] = 1;
        }
        TextType t = static_cast<TextType>(type);
        if ((data.length() > 1) && (data[0] == '@')) {
            switch(data[1]) {
            case 'K':
                t = KarFileType;
                break;
            case 'V':
                t = KarVersion;
                break;
            case 'I':
                t = KarInformation;
                break;
            case 'L':
                t = KarLanguage;
                break;
            case 'T':
                t = KarTitles;
                break;
            case 'W':
                t = KarWhatever;
                break;
            }
        }
        m_textEvents.append(TextRec(time, m_curTrack, t, data));
        switch ( t ) {
        case Sequence::Lyric:
        case Sequence::Text: {
                TextEvent *ev = new TextEvent(data, t);
                ev->setTag(m_curTrack);
                appendSMFEvent(ev);
            }
            break;
        case Sequence::TrackName:
        case Sequence::InstrumentName:
            if (m_trackLabel.isEmpty()) {
                m_trackLabel = data;
            }
            if (!m_trkName.contains(m_curTrack)) {
                m_trkName[m_curTrack] = data;
            } else {
                m_trkName[m_curTrack].append(' ');
                m_trkName[m_curTrack].append(data);
            }
            break;
        default:
            break;
        }
    } else {
        m_uchardetErrors++;
        qWarning() << "uchardet - handle data error:" << retval;
    }
}

void Sequence::smfMetaEvent(int type, const QByteArray& data)
{
    if ( (data.length() > 0) &&
         (type > Sequence::None) &&
         (type <= Sequence::Cue) ) {
        addMetaData(m_smf->getCurrentTime(), type, data);
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

void Sequence::smfTimeSigEvent(int b0, int b1, int b2, int b3)
{
    Q_UNUSED(b2)
    Q_UNUSED(b3)
    //qDebug() << Q_FUNC_INFO << b0 << b1 << b2 << b3;
    int den = ::pow(2, b1);
    MIDIEvent* ev = new TimeSignatureEvent(b0, den);
    ev->setTag( m_barCount );
    appendSMFEvent(ev);
    m_beatMax = b0;
    m_beatLength = m_division * 4 / den;
}

void Sequence::smfKeySig(int alt, int mode)
{
    //qDebug() << Q_FUNC_INFO << "track:" << m_curTrack << "alt:" << alt << "mode:" << mode;
    MIDIEvent *ev = new KeySignatureEvent(alt, mode == minor_mode);
    ev->setTag( m_curTrack );
    appendSMFEvent(ev);
}

void Sequence::smfTrackStartEvent()
{
    int tick = m_smf->getCurrentTime();
    if (tick > m_ticksDuration) {
        m_ticksDuration = tick;
    }
    m_curTrack++;
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
        if (chan >= 0 && chan < MIDI_STD_CHANNELS) {
            m_channelLabel[chan] = m_trackLabel;
            m_trkChannel[m_curTrack] = chan;
        }
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
    if (ev->tag() <= 0) {
        ev->setTag(m_curTrack);
    }
    m_list.append(ev);
    if (ticks > m_ticksDuration) {
        m_ticksDuration = ticks;
    }
    insertBeats(ticks);
    wrkUpdateLoadProgress();
}

void Sequence::wrkErrorHandler(const QString& errorStr)
{
    qWarning() << QString("%1 at file offset %2<br>")
        .arg(errorStr).arg(m_wrk->getFilePos());
}

void Sequence::wrkFileHeader(int verh, int verl)
{
    m_curTrack = 0;
    m_numTracks = 0;
    m_division = 120;
    m_beatLength = m_division;
    m_beatMax = 4;
    m_lastBeat = 0;
    m_beatCount = 1;
    m_barCount = 1;
    m_fileFormat = QString("%1.%2").arg(verh).arg(verl);
    wrkUpdateLoadProgress();
}

void Sequence::wrkTimeBase(int timebase)
{
    m_division = timebase;
    wrkUpdateLoadProgress();
}

void Sequence::wrkGlobalVars()
{
    wrkKeySig(0, m_wrk->getKeySig());
    //setLast( m_wrk->getEndAllTime() );
    wrkUpdateLoadProgress();
}

void Sequence::wrkStreamEndEvent(long time)
{
    if (time > m_ticksDuration) {
        m_ticksDuration = time;
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkTrackHeader( const QByteArray& name1,
                           const QByteArray& name2,
                           int trackno, int channel,
                           int pitch, int velocity, int /*port*/,
                           bool /*selected*/, bool /*muted*/, bool /*loop*/ )
{
    TrackMapRec rec;
    rec.channel = channel;
    rec.pitch = pitch;
    rec.velocity = velocity;
    m_curTrack = trackno + 1;
    if (m_curTrack > m_numTracks) {
        m_numTracks = m_curTrack;
    }
    m_trackMap[m_curTrack] = rec;
    QByteArray trkName = name1 + ' ' + name2;
    trkName = trkName.trimmed();
    if (!trkName.isEmpty()) {
        appendWRKmetadata(m_curTrack, 0, TextType::TrackName, trkName);
        if (channel > -1) {
            m_channelLabel[channel] = m_trackLabel;
        }
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkNoteEvent(int track, long time, int chan, int pitch, int vol, int dur)
{
    TrackMapRec rec = m_trackMap[track+1];
    int key = pitch + rec.pitch;
    int velocity = vol + rec.velocity;
    int channel = (rec.channel > -1) ? rec.channel : chan;
    if (pitch > m_highestMidiNote)
        m_highestMidiNote = pitch;
    if (pitch < m_lowestMidiNote)
        m_lowestMidiNote = pitch;
    m_channelUsed[channel] = true;
    MIDIEvent* ev = new NoteOnEvent(channel, key, velocity);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
    ev = new NoteOffEvent(channel, key, velocity);
    ev->setTag(track+1);
    appendWRKEvent(time + dur, ev);
    m_channelEvents[channel]+=2;
}

void Sequence::wrkKeyPressEvent(int track, long time, int chan, int pitch, int press)
{
    TrackMapRec rec = m_trackMap[track+1];
    int key = pitch + rec.pitch;
    int channel = (rec.channel > -1) ? rec.channel : chan;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new KeyPressEvent(channel, key, press);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
}

void Sequence::wrkCtlChangeEvent(int track, long time, int chan, int ctl, int value)
{
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : chan;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ControllerEvent(channel, ctl, value);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
}

void Sequence::wrkPitchBendEvent(int track, long time, int chan, int value)
{
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : chan;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new PitchBendEvent(channel, value);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
}

void Sequence::wrkProgramEvent(int track, long time, int chan, int patch)
{
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : chan;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ProgramChangeEvent(channel, patch);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
}

void Sequence::wrkChanPressEvent(int track, long time, int chan, int press)
{
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : chan;
    m_channelUsed[channel] = true;
    m_channelEvents[channel]++;
    MIDIEvent* ev = new ChanPressEvent(channel, press);
    ev->setTag(track+1);
    appendWRKEvent(time, ev);
}

void Sequence::wrkSysexEvent(int track, long time, int bank)
{
    SysexEventRec rec;
    rec.track = track+1;
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

void Sequence::appendWRKmetadata(int track, long time, TextType type, const QByteArray& data)
{
    Q_ASSERT(track > 0);
    int retval = uchardet_handle_data(m_handle, data.data(), data.length());
    if (retval == 0)
    {
        if (m_trkScore.contains(track)) {
            m_trkScore[track]++;
        } else {
            m_trkScore[track] = 1;
        }
        if (m_typScore.contains(type)) {
            m_typScore[type]++;
        } else {
            m_typScore[type] = 1;
        }
        TextType t = static_cast<TextType>(type);
        m_textEvents.append(TextRec(time, track, t, data));
        switch ( t ) {
        case Sequence::Lyric:
        case Sequence::Text: {
                TextEvent *ev = new TextEvent(data, t);
                ev->setTag(track);
                appendWRKEvent(time, ev);
            }
            break;
        case Sequence::TrackName:
        case Sequence::InstrumentName:
            m_trackLabel = data;
            m_trkName[track] = data;
            break;
        default:
            break;
        }
    } else {
        m_uchardetErrors++;
        qWarning() << "uchardet - handle data error:" << retval;
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkTextEvent(int track, long time, int /*type*/, const QByteArray &data)
{
    //qDebug() << "track:" << track+1 << "time:" << time << "type:" << type << "data:" << data;
    appendWRKmetadata(track+1, time, TextType::Lyric, data);
}

void Sequence::wrkComments(const QByteArray &cmt)
{
    appendWRKmetadata(1, 0, TextType::Text, cmt);
}

void Sequence::wrkVariableRecord(const QString &name, const QByteArray &data)
{
    bool isReadable = (name == "Title" || name == "Author" ||
                       name == "Copyright" || name == "Subtitle" ||
                       name == "Instructions" || name == "Keywords");
    if (isReadable) {
        TextType type = TextType::None;
        if ( name == "Title" || name == "Subtitle" )
            type = TextType::TrackName;
        else if ( name == "Copyright" || name == "Author" )
            type = TextType::Copyright;
        else
            type = TextType::Text;
        appendWRKmetadata(1, 0, type, data);
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkTempoEvent(long time, int tempo)
{
    double bpm = tempo / 100.0;
    TempoEvent* ev = new TempoEvent(6e7 / bpm);
    appendWRKEvent(time, ev);
    if (time == 0) {
        updateTempo(ev->tempo());
    }
}

void Sequence::wrkTrackPatch(int track, int patch)
{
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : 0;
    wrkProgramEvent(track+1, 0, channel, patch);
}

void Sequence::wrkNewTrackHeader( const QByteArray& data,
                              int trackno, int channel,
                              int pitch, int velocity, int /*port*/,
                              bool /*selected*/, bool /*muted*/, bool /*loop*/ )
{
    TrackMapRec rec;
    rec.channel = channel;
    rec.pitch = pitch;
    rec.velocity = velocity;
    m_curTrack = trackno + 1;
    if (m_curTrack > m_numTracks) {
        m_numTracks = m_curTrack;
    }
    m_trackMap[m_curTrack] = rec;
    m_trkChannel[m_curTrack] = channel;
    appendWRKmetadata(m_curTrack, 0, TextType::TrackName, data);
    if (channel > -1) {
        m_channelLabel[channel] = m_trackLabel;
    }
    wrkUpdateLoadProgress();
}

void Sequence::wrkTrackName(int trackno, const QByteArray &data)
{
    appendWRKmetadata(trackno+1, 0, TextType::TrackName, data);
    TrackMapRec rec = m_trackMap[trackno+1];
    int channel = rec.channel;
    if (channel > -1) {
        m_channelLabel[channel] = m_trackLabel;
    }
}

void Sequence::wrkTrackVol(int track, int vol)
{
    int lsb, msb;
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : 0;
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
    int lsb, msb;
    TrackMapRec rec = m_trackMap[track+1];
    int channel = (rec.channel > -1) ? rec.channel : 0;
    lsb = bank % 0x80;
    msb = bank / 0x80;
    wrkCtlChangeEvent(track+1, 0, channel, ControllerEvent::MIDI_CTL_MSB_BANK, msb);
    wrkCtlChangeEvent(track+1, 0, channel, ControllerEvent::MIDI_CTL_LSB_BANK, lsb);
}

void Sequence::wrkSegment(int track, long time, const QByteArray &name)
{
    if (!name.isEmpty()) {
        appendWRKmetadata(track+1, time, TextType::Marker, name);
    }
}

void Sequence::wrkChord(int track, long time, const QString &name, const QByteArray& /*data*/)
{
    QByteArray data = name.toUtf8();
    appendWRKmetadata(track+1, time, TextType::Cue, data);
}

void Sequence::wrkExpression(int track, long time, int /*code*/, const QByteArray &text)
{
    appendWRKmetadata(track+1, time, TextType::Cue, text);
}

void Sequence::wrkTimeSignatureEvent(int bar, int num, int den)
{
    MIDIEvent* ev = new TimeSignatureEvent(num, den);
    m_beatMax = num;
    m_beatLength = m_division * 4 / den;

    TimeSigRec newts;
    newts.bar = bar;
    newts.num = num;
    newts.den = den;
    newts.time = 0;
    if (m_bars.isEmpty()) {
        m_bars.append(newts);
    } else {
        bool found = false;
        foreach(const TimeSigRec& ts, m_bars) {
            if (ts.bar == bar) {
                newts.time = ts.time;
                found = true;
                break;
            }
        }
        if (!found) {
            TimeSigRec& lasts = m_bars.last();
            newts.time = lasts.time +
                    (lasts.num * 4 / lasts.den * m_division * (bar - lasts.bar));
            m_bars.append(newts);
        }
    }
    ev->setTag( bar );
    appendWRKEvent(newts.time, ev);
}

void Sequence::wrkKeySig(int bar, int alt)
{
    MIDIEvent *ev = new KeySignatureEvent(alt, false);
    long time = 0;
    foreach(const TimeSigRec& ts, m_bars) {
        if (ts.bar == bar) {
            time = ts.time;
            break;
        }
    }
    appendWRKEvent(time, ev);
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
        (!m_channelLabel[channel].isEmpty())) {
        if (m_codec == nullptr)
            return QString::fromLocal8Bit(m_channelLabel[channel]);
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

/* Most of the character sets in m_ucharsets are already recognized by
 * QTextCodec, and checked in initCodecs(), except for a few ones that
 * are explictly initialized here. See also:
 * https://www.iana.org/assignments/character-sets/character-sets.xhtml
 */
const QMap<QByteArray,int> Sequence::m_umibs {
    // Linux
    {"UHC", 38},
    {"ISO-8859-11", 2259},
    {"VISCII", 2082},
    // Windows
    //{"ASCII", 0},
    {"ISO-2022-CN", 104},
    {"HZ-GB-2312", 2085},
    {"IBM852", 2010},
    {"ISO-2022-KR", 37},
    {"IBM855", 2046}
    // macOS
    //{"x-mac-ce", 0},
    //{"x-mac-cyrillic", 0}
};
