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

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTextCodec>
#include <drumstick/qsmf.h>
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
    std::chrono::milliseconds deltaTimeOfEvent(MIDIEvent* ev) const;
    std::chrono::milliseconds timeOfTicks(const int ticks) const;
    bool hasMoreEvents();
    int getFormat() const { return m_format; }
    int getDivision() const { return m_division; }
    bool isEmpty();
    void resetPosition();
    void setTickPosition(long tick);
    void setTimePosition(long time);
    qreal currentTempo() const;
    QString getName() const { return m_lblName; }
    int songLengthTicks() const;
    void updateTempo(qreal newTempo);
    qreal ticks2millis() const { return m_ticks2millis; }

    bool channelUsed(int channel);
    QString channelLabel(int channel);
    int lowestMidiNote();
    int highestMidiNote();

    int numUchardetErrors();
    int detectedUchardetMIB() const;
    QByteArray detectedCharset() const;

    int getNumTracks() const;
    int trackMaxPoints();
    int typeMaxPoints();
    QByteArray trackName(int track) const;

    QString currentFile() const;
    QByteArray getRawText(const int track, const TextType type);
    QStringList getText(const TextType type, const int mib);

    int getNumAlterations() const;

signals:
    void loadingStart(int size);
    void loadingProgress(int pos);
    void loadingFinished();

public slots:
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
    void wrkUpdateLoadProgress();
    void wrkErrorHandler(const QString& errorStr);
    void wrkFileHeader(int verh, int verl);
    void wrkEndOfFile();
    void wrkStreamEndEvent(long time);
    void wrkTrackHeader(const QString& name1, const QString& name2,
             int trackno, int channel, int pitch,
             int velocity, int port,
             bool selected, bool muted, bool loop);
    void wrkTimeBase(int timebase);
    void wrkNoteEvent(int track, long time, int chan, int pitch, int vol, int dur);
    void wrkKeyPressEvent(int track, long time, int chan, int pitch, int press);
    void wrkCtlChangeEvent(int track, long time, int chan, int ctl, int value);
    void wrkPitchBendEvent(int track, long time, int chan, int value);
    void wrkProgramEvent(int track, long time, int chan, int patch);
    void wrkChanPressEvent(int track, long time, int chan, int press);
    void wrkSysexEvent(int track, long time, int bank);
    void wrkSysexEventBank(int bank, const QString& name, bool autosend, int port, const QByteArray& data);
    void wrkTempoEvent(long time, int tempo);
    void wrkTrackPatch(int track, int patch);
    void wrkNewTrackHeader(const QString& name,
            int trackno, int channel, int pitch,
            int velocity, int port,
            bool selected, bool muted, bool loop);
    void wrkTrackVol(int track, int vol);
    void wrkTrackBank(int track, int bank);
    void wrkTimeSignatureEvent(int bar, int num, int den);
    void wrkKeySig(int bar, int alt);

private: // methods
    void sort();
    void initCodecs();
    void insertBeats(qint64 ticks);
    void timeCalculations();
    void addMetaData(int type, const QByteArray &data);
    void appendStringToList(QStringList &list, QString &s, TextType type);

private: // members
    QList<MIDIEvent*> m_list;
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
    int m_numAlterations;
    qreal m_tempo;
    qreal m_tempoFactor;
    qreal m_ticks2millis;
    qreal m_duration;
    qint64 m_lastBeat;
    qint64 m_beatLength;
    qint64 m_tick;
    QString m_lblName;

    /* This is the list of character sets
     * detected by the uchardet library */
    QList<QByteArray> m_ucharsets {
        "ASCII",
        "BIG5",
        "EUC-JP",
        "EUC-KR",
        /* "EUC-TW", Not supported by QTextCodec! :-( */
        "GB18030",
        "HZ-GB-2312",
        "IBM852",
        "IBM855",
        "IBM866",
        "ISO-2022-CN",
        "ISO-2022-JP",
        "ISO-2022-KR",
        "ISO-8859-1",
        "ISO-8859-10",
        "ISO-8859-11",
        "ISO-8859-13",
        "ISO-8859-15",
        "ISO-8859-16",
        "ISO-8859-2",
        "ISO-8859-3",
        "ISO-8859-4",
        "ISO-8859-5",
        "ISO-8859-6",
        "ISO-8859-7",
        "ISO-8859-8",
        "ISO-8859-9",
        "KOI8-R",
        "MAC-CENTRALEUROPE",
        "MAC-CYRILLIC",
        "SHIFT_JIS",
        "TIS-620",
        "UHC",
        "UTF-16BE",
        "UTF-16LE",
        "UTF-32BE",
        "UTF-32LE",
        "UTF-8",
        "VISCII",
        "WINDOWS-1250",
        "WINDOWS-1250",
        "WINDOWS-1251",
        "WINDOWS-1252",
        "WINDOWS-1252",
        "WINDOWS-1253",
        "WINDOWS-1255",
        "WINDOWS-1256",
        "WINDOWS-1257",
        "WINDOWS-1258"
    };

    /* Most of the character sets in m_ucharsets are already recognized by
     * QTextCodec, and checked in initCodecs(), except for a few ones that
     * are explictly initialized here. See also:
     * https://www.iana.org/assignments/character-sets/character-sets.xhtml
     */
    QMap<QByteArray,int> m_umibs{
        {"UHC", 38},
        {"ISO-8859-11", 2259},
        {"VISCII", 2082}
    };
    QByteArray m_charset;
    uchardet_t m_handle;

    struct SysexEventRec {
        int track;
        long time;
        int bank;
    };
    QList<SysexEventRec> m_savedSysexEvents;

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
        TextRec(QByteArray data): m_track(0), m_type(TextType::None), m_text(data) { };
        TextRec(int t, TextType e, QByteArray data): m_track(t), m_type(e), m_text(data) { };
        int m_track;
        TextType m_type;
        QByteArray m_text;
    };
    QList<TextRec> m_textEvents;

    QString m_currentFile;
    QByteArray m_trackLabel;
    bool m_channelUsed[drumstick::rt::MIDI_STD_CHANNELS];
    int m_channelEvents[drumstick::rt::MIDI_STD_CHANNELS];
    QByteArray m_channelLabel[drumstick::rt::MIDI_STD_CHANNELS];
    int m_channelPatches[drumstick::rt::MIDI_STD_CHANNELS];
    QMap<int, QByteArray> m_trkName;
    QMap<int, int> m_trkScore;
    QMap<int, int> m_typScore;
};

#endif // SEQUENCE_H
