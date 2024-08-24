/*
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

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QTextCodec>
#include <chrono>

#include <drumstick/qsmf.h>
#include <drumstick/rmid.h>
#include <drumstick/qwrk.h>
#include <drumstick/rtmidioutput.h>
#include <uchardet.h>

#include "events.h"

class Sequence : public QObject
{
    Q_OBJECT

public:
    /**
     * For Karaoke files, there may be additional metadata
     * FileType: @KMIDI KARAOKE FILE
     * Version: @V0100
     * Information: @I<text>
     * Language: @L<lang>
     * Title: @T<title, author, ...>
     * What?: @W<bla bla bla>
     */
    enum TextType {
        None = 0, Text = 1, Copyright = 2, TrackName = 3,
        InstrumentName = 4, Lyric = 5, Marker = 6, Cue = 7,
        KarFileType = 8, KarVersion = 9, KarInformation = 10,
        KarLanguage = 11, KarTitles = 12, KarWhatever = 13,
        FIRST_TYPE = Text, LAST_TYPE = KarWhatever
    };
    Q_ENUM(TextType)

    Sequence(QObject* parent = 0);
    virtual ~Sequence();

    void clear();
    void appendEvent(MIDIEvent* ev);
    void loadPattern(QList<MIDIEvent*> pattern);
    void loadFile(const QString& fileName);
    qreal tempoFactor() const;
    void setTempoFactor(const qreal factor);
    MIDIEvent *nextEvent();
    int eventTime(MIDIEvent* ev) const;
    std::chrono::microseconds timeOfEvent(MIDIEvent *ev) const;
    std::chrono::microseconds deltaTimeOfEvent(MIDIEvent *ev) const;
    std::chrono::microseconds timeOfTicks(const int ticks) const;
    bool hasMoreEvents();
    int getFormat() const { return m_format; }
    int getDivision() const { return m_division; }
    bool isEmpty();
    void resetPosition();
    void setTickPosition(long tick);
    qreal currentTempo() const;
    QString getName() const { return m_lblName; }
    int songLengthTicks() const;
    void updateTempo(qreal newTempo);
    qreal ticks2micros() const { return m_ticks2micros; }

    bool channelUsed(int channel);
    QString channelLabel(int channel);
    int lowestMidiNote();
    int highestMidiNote();

    int numUchardetErrors();
    int currentMIB() const;
    QByteArray currentCharset() const;
    void setCurrentCharset(const QByteArray& charset);

    int getNumTracks() const;
    int trackMaxPoints();
    int typeMaxPoints();
    QByteArray trackName(int track) const;
    int trackChannel(int track) const;

    QString currentFile() const;
    QString currentFullFileName() const;
    QList<QPair<int,QByteArray>> getRawText(const int track, const TextType type);
    QStringList getText(const TextType type);

    void findCodec();
    int getNumAlterations() const;
    BeatEvent* nearestBeatByTicks(int ticks);
    BeatEvent* nextBar(BeatEvent* latest);
    BeatEvent* previousBar(BeatEvent* latest);
    BeatEvent* jumpToBar(int bar);
    BeatEvent* firstBeat();
    int lastBar();

    static QByteArrayList getExtraCodecNames();
    static int getMibForName(const QByteArray name);

    QTextCodec *codec() const;
    void setCodec(QTextCodec *newCodec);

    QString getFileFormat() const;
    QString getMetadataInfo() const;

    QString loadingErrors() const;
    int errorsCount() const;

signals:
    void loadingStart(int size);
    void loadingProgress(int pos);
    void loadingFinished();

public slots:
    /* Rmidi slots */
    void dataHandler(const QString& dataType, const QByteArray& data);
    void infoHandler(const QString& infoType, const QByteArray& data);

    /* SMF slots */
    void appendSMFEvent(MIDIEvent *ev);
    void smfHeaderEvent(int format, int ntrks, int division);
    void smfNoteOnEvent(int chan, int pitch, int vol);
    void smfNoteOffEvent(int chan, int pitch, int vol);
    void smfKeyPressEvent(int chan, int pitch, int press);
    void smfCtlChangeEvent(int chan, int ctl, int value);
    void smfPitchBendEvent(int chan, int value);
    void smfProgramEvent(int chan, int patch);
    void smfChanPressEvent(int chan, int press);
    void smfSysexEvent(const QByteArray& data);
    void smfMetaEvent(int type, const QByteArray &data);
    void smfTempoEvent(int tempo);
    void smfTimeSigEvent(int b0, int b1, int b2, int b3);
    void smfTrackStartEvent();
    void smfTrackEnd();
    void smfErrorHandler(const QString& errorStr);
    void smfUpdateLoadProgress();
    void smfKeySig(int, int);

    /* WRK slots */
    void appendWRKEvent(long ticks, MIDIEvent* ev);
    void appendWRKmetadata(int track, long time, TextType typ, const QByteArray &data);
    void wrkUpdateLoadProgress();
    void wrkErrorHandler(const QString& errorStr);
    void wrkFileHeader(int verh, int verl);
    void wrkEndOfFile();
    void wrkStreamEndEvent(long time);
    void wrkTrackHeader(const QByteArray& name1, const QByteArray& name2,
             int trackno, int channel, int pitch,
             int velocity, int port,
             bool selected, bool muted, bool loop);
    void wrkTimeBase(int timebase);
    void wrkGlobalVars();
    void wrkNoteEvent(int track, long time, int chan, int pitch, int vol, int dur);
    void wrkKeyPressEvent(int track, long time, int chan, int pitch, int press);
    void wrkCtlChangeEvent(int track, long time, int chan, int ctl, int value);
    void wrkPitchBendEvent(int track, long time, int chan, int value);
    void wrkProgramEvent(int track, long time, int chan, int patch);
    void wrkChanPressEvent(int track, long time, int chan, int press);
    void wrkSysexEvent(int track, long time, int bank);
    void wrkSysexEventBank(int bank, const QString& name, bool autosend, int port, const QByteArray& data);
    void wrkTextEvent(int track, long time, int typ, const QByteArray& data);
    void wrkComments(const QByteArray& cmt);
    void wrkVariableRecord(const QString& name, const QByteArray& data);
    void wrkTempoEvent(long time, int tempo);
    void wrkTrackPatch(int track, int patch);
    void wrkNewTrackHeader(const QByteArray& name,
            int trackno, int channel, int pitch,
            int velocity, int port,
            bool selected, bool muted, bool loop);
    void wrkTrackName(int trackno, const QByteArray& name);
    void wrkTrackVol(int track, int vol);
    void wrkTrackBank(int track, int bank);
    void wrkSegment(int track, long time, const QByteArray& name);
    void wrkChord(int track, long time, const QString& name, const QByteArray& data);
    void wrkExpression(int track, long time, int code, const QByteArray& text);
    void wrkTimeSignatureEvent(int bar, int num, int den);
    void wrkKeySig(int bar, int alt);
    void wrkMarker(long time, int smpte, const QByteArray &data);

private: // methods
    void sort();
    void initCodecs();
    void insertBeats(qint64 ticks);
    void timeCalculations();
    void addMetaData(int time, int type, const QByteArray &data);
    void appendStringToList(QStringList &list, QString &s, TextType type);

private: // members
    QList<MIDIEvent*> m_list;
    drumstick::File::Rmidi* m_rmid;
    drumstick::File::QSmf* m_smf;
    drumstick::File::QWrk* m_wrk;

    int m_uchardetErrors;
    int m_format;
    int m_numTracks;
    int m_ticksDuration;
    int m_division;
    int m_pos;
    int m_curTrack;
    int m_beatMax;
    int m_barCount;
    int m_beatCount;
    int m_lowestMidiNote;
    int m_highestMidiNote;
    qreal m_tempo;
    qreal m_tempoFactor;
    qreal m_ticks2micros;
    qreal m_duration;
    qint64 m_lastBeat;
    qint64 m_beatLength;
    qint64 m_tick;
    QString m_lblName;

    static const QMap<QByteArray,int> m_umibs;

    QMap<QByteArray,int> m_supportedCodecs;
    QTextCodec *m_codec;
    int m_mib;
    QByteArray m_charset;
    uchardet_t m_handle;

    QMap<int, SysExEvent> m_savedSysexEvents;

    struct TrackMapRec {
        int channel;
        int pitch;
        int velocity;
    };
    QMap<int,TrackMapRec> m_trackMap;

    struct TimeSigRec {
        int bar;
        int num;
        int den;
        long time;
    };
    QList<TimeSigRec> m_bars;

    struct TextRec {
        TextRec(QByteArray data)
            : m_tick(0)
            , m_track(0)
            , m_type(TextType::None)
            , m_text(data)
        {}
        TextRec(int tick, int track, TextType e, QByteArray data)
            : m_tick(tick)
            , m_track(track)
            , m_type(e)
            , m_text(data)
        {}
        int m_tick;
        int m_track;
        TextType m_type;
        QByteArray m_text;
    };
    QList<TextRec> m_textEvents;

    QString m_currentFile;
    QString m_currentFileFull;
    QString m_fileFormat;
    QByteArray m_trackLabel;
    bool m_channelUsed[drumstick::rt::MIDI_STD_CHANNELS];
    int m_channelEvents[drumstick::rt::MIDI_STD_CHANNELS];
    QByteArray m_channelLabel[drumstick::rt::MIDI_STD_CHANNELS];
    int m_channelPatches[drumstick::rt::MIDI_STD_CHANNELS];
    QMap<int, QByteArray> m_trkName;
    QMap<int, int> m_trkScore;
    QMap<int, int> m_typScore;
    QMap<int, int> m_trkChannel;
    QMap<QString, QString> m_infoMap;
    QList<QString> m_loadingErrors;
};

#endif // SEQUENCE_H
