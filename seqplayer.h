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

#ifndef SEQPLAYER_H_
#define SEQPLAYER_H_

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include <QElapsedTimer>
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
    void sendVolumeEvents();

public slots:
    void playerLoop();

signals:
    void songFinished();
    void songEchoTime(long millis, long ticks);
    void songStopped();

private slots:
    void playEvent(MIDIEvent* ev);

private:
    Sequence m_song;
    drumstick::rt::MIDIOutput* m_port;
    long m_songPosition;
    int m_echoResolution;
    int m_pitchShift;
    int m_volumeFactor;
    int m_volume[drumstick::rt::MIDI_STD_CHANNELS];
};

#endif /*SEQPLAYER_H_*/
