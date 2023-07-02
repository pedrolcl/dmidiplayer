/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>
 
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

#ifndef SEQPLAYER_H_
#define SEQPLAYER_H_

#include <QObject>
#include <drumstick/backendmanager.h>

#include "sequence.h"
#include "events.h"

class SequencePlayer : public QObject
{
    Q_OBJECT
public:
    SequencePlayer();
    virtual ~SequencePlayer();

    void loadFile(QString fileName);
    void pause();
    void stop();
    void appendEvent(MIDIEvent* ev);
    void shutupSound();
    void info();
    void setPort(drumstick::rt::MIDIOutput *p);
    drumstick::rt::MIDIOutput* port() const;
    qreal bpm(qreal tempo) const;
    qreal currentBPM() const;
    bool isRunning();
    Sequence *song();
    int getPosition();
    int getEchoResolution();
    int getPitchShift();
    int getVolumeFactor();
    void resetPosition();
    void setPosition(long pos);
    void setPitchShift(unsigned int pitch);
    void setVolumeFactor(unsigned int vol);
    void allNotesOff();
    void resetControllers();
    void resetPrograms();
    void sendResetMessage();
    void sendVolumeEvents();
    qreal volume(int channel);
    bool isMuted(int channel);
    bool isLocked(int channel);
    void beatByTickPosition(int pos);
    void beatForward();
    void beatBackward();
    void jumpToBar(int bar);
    QString currentBeatStr();

    int loopStart() const;
    int loopEnd() const;
    bool isLoopEnabled() const;
    void setLoop(bool enabled);
    void setLoop(int loopStart, int loopEnd);

public slots:
    void playerLoop();
    void setVolume(int channel, qreal);
    void setMuted(int channel, bool mute);
    void setLocked(int channel, bool lock);
    void setPatch(int channel, int patch);

signals:
    void songStarted();
    void songFinished();
    void songStopped();
    void songEchoTime(long millis, long ticks);
    void volumeChanged(int channel, qreal newVolume);
    void mutedChanged(int channel, bool);
    void lockedChanged(int channel, bool);

    /**
     * Sequenced SMF events (for feedback to the application)
     */
    void tempoChanged(const qreal tempo);
    //void timeSignatureChanged(const int numerator, const int denominator);
    void midiText(const int track, const int type, const int ticks, const QByteArray &text);
    void midiNoteOn(const int chan, const int note, const int vel);
    void midiNoteOff(const int chan, const int note, const int vel);
    void midiController(const int chan, const int control, const int value);
    void midiKeyPressure(const int chan, const int note, const int value);
    void midiProgram(const int chan, const int program);
    void midiChannelPressure(const int chan, const int value);
    void midiPitchBend(const int chan, const int value);
    void midiSysex(const QByteArray &data);
    void beat(const int bar, const int beat, const int max);
    void timeSignature(const int bar, const int n, const int d);
    void keySignature(const int track, const int a, const bool m);

private slots:
    void playEvent(MIDIEvent* ev);
    void setBeatPosition(BeatEvent *ev);

private:
    void initChannels();
    int boundedFloor(const int initial, const qreal factor) const;

    Sequence m_song;
    drumstick::rt::MIDIOutput* m_port;
    long m_songPosition;
    bool m_loopEnabled;
    int m_loopStart;
    int m_loopEnd;
    int m_echoResolution;
    int m_pitchShift;
    int m_volumeFactor;
    int m_volume[drumstick::rt::MIDI_STD_CHANNELS];
    int m_lastpgm[drumstick::rt::MIDI_STD_CHANNELS];
    int m_lockedpgm[drumstick::rt::MIDI_STD_CHANNELS];
    qreal m_volumeShift[drumstick::rt::MIDI_STD_CHANNELS];
    bool m_muted[drumstick::rt::MIDI_STD_CHANNELS];
    bool m_locked[drumstick::rt::MIDI_STD_CHANNELS];
    BeatEvent *m_latestBeat;
    BeatEvent *m_firstBeat;
};

#endif /*SEQPLAYER_H_*/
