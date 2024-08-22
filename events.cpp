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

#include <QByteArray>
#include <QString>
#include <drumstick/rtmidioutput.h>
#include <iomanip>
#include <iostream>

#include "events.h"

using namespace drumstick::rt;

/**
 * Default constructor.
 */
MIDIEvent::MIDIEvent() : QEvent(MIDIEventType),
    m_delta(0), m_tick(0), m_tag(0), m_status(0)
{ }

/**
 * Sets the event to be scheduled in delta musical time (ticks) units.
 * @param delta The delta time in ticks.
 */
void MIDIEvent::setDelta(const long delta)
{
     m_delta = delta;
}

/**
 * Sets the event to be scheduled in musical time (ticks) units.
 * @param tick The time in ticks.
 */
void MIDIEvent::setTick(const long tick)
{
    m_tick = tick;
}

/**
 * Sets the event's tag. This attribute is any arbitrary integer number.
 * @param aTag A tag number.
 */
void MIDIEvent::setTag(const int aTag)
{
    m_tag = aTag;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param key MIDI note.
 * @param vel Note velocity.
 */
NoteOnEvent::NoteOnEvent(int ch, int key, int vel) : KeyEvent(ch, key, vel)
{
    m_status = MIDI_STATUS_NOTEON;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param key MIDI note.
 * @param vel Note velocity.
 */
NoteOffEvent::NoteOffEvent(int ch, int key, int vel) : KeyEvent(ch, key, vel)
{
    m_status = MIDI_STATUS_NOTEOFF;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param key MIDI note.
 * @param vel Note velocity.
 */
KeyPressEvent::KeyPressEvent(int ch, int key, int vel) : KeyEvent(ch, key, vel)
{
    m_status = MIDI_STATUS_KEYPRESURE;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param cc MIDI Controller number.
 * @param val Controller value.
 */
ControllerEvent::ControllerEvent(int ch, int cc, int val) : ChannelEvent(ch),
    m_param(cc), m_value(val)
{
    m_status = MIDI_STATUS_CONTROLCHANGE;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param val MIDI Program number.
 */
ProgramChangeEvent::ProgramChangeEvent(int ch, int val) : ChannelEvent(ch),
    m_program(val)
{
    m_status = MIDI_STATUS_PROGRAMCHANGE;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param val Pitch Bend value. Zero centered from -8192 to 8191.
 */
PitchBendEvent::PitchBendEvent(int ch, int val) : ChannelEvent(ch),
    m_value(val)
{
    m_status = MIDI_STATUS_PITCHBEND;
}

/**
 * Constructor using proper attribute values.
 * @param ch MIDI Channel.
 * @param val Aftertouch value.
 */
ChanPressEvent::ChanPressEvent(int ch, int val) : ChannelEvent(ch),
    m_value(val)
{
    m_status = MIDI_STATUS_CHANNELPRESSURE;
}

/**
 * Default constructor.
 */
VariableEvent::VariableEvent() : MIDIEvent()
{
    m_data.clear();
}

/**
 * Constructor from an arbitrary data array.
 * @param data A data byte array.
 */
VariableEvent::VariableEvent(const QByteArray& data): MIDIEvent()
{
    m_data = data;
}

/**
 * Constructor from a data pointer.
 * @param datalen Length of the data.
 * @param dataptr Pointer the data.
 */
VariableEvent::VariableEvent(const unsigned int datalen, char* dataptr)
    : MIDIEvent()
{
    m_data = QByteArray(dataptr, datalen);
}

/**
 * Default constructor.
 */
SysExEvent::SysExEvent()
    : VariableEvent()
{
    m_status = MIDI_STATUS_SYSEX;
}

/**
 * Constructor from a data array.
 * @param data A data byte array.
 */
SysExEvent::SysExEvent(const QByteArray& data)
    : VariableEvent(data)
{
    m_status = MIDI_STATUS_SYSEX;
}

/**
 * Constructor taking a data pointer and length
 * @param datalen Data length
 * @param dataptr Data pointer
 */
SysExEvent::SysExEvent(const unsigned int datalen, char* dataptr)
    : VariableEvent( datalen, dataptr )
{
    m_status = MIDI_STATUS_SYSEX;
}

SystemEvent::SystemEvent(const int message)
    : MIDIEvent()
{
    m_status = message;
}

/**
 * Default constructor
 */
TextEvent::TextEvent()
    : VariableEvent(), m_textType(1)
{ }

/**
 * Constructor from a given string
 * @param text The event's text
 * @param textType The SMF text type
 */
TextEvent::TextEvent(const QByteArray &text, const int textType)
    : VariableEvent(text), m_textType(textType)
{ }

/**
 * Constructor from a data pointer and length
 * @param datalen Data length
 * @param dataptr Data pointer
 */
TextEvent::TextEvent(const unsigned int datalen, char* dataptr)
    : VariableEvent(datalen, dataptr), m_textType(1)
{ }

/**
 * Gets the event's SMF text type.
 * @return The SMF text type.
 */
int TextEvent::textType() const
{
    return m_textType;
}

TempoEvent::TempoEvent() : MIDIEvent(), m_tempo(0)
{ }

TempoEvent::TempoEvent(const qreal tempo): MIDIEvent(), m_tempo(tempo)
{ }

TimeSignatureEvent::TimeSignatureEvent()
    : MIDIEvent(), m_numerator(0), m_denominator(0)
{ }

TimeSignatureEvent::TimeSignatureEvent(const int numerator, const int denominator)
    : MIDIEvent(), m_numerator(numerator), m_denominator(denominator)
{ }

KeySignatureEvent::KeySignatureEvent()
    : MIDIEvent(), m_alterations(0), m_minorMode(false)
{ }

KeySignatureEvent::KeySignatureEvent(const int alterations, const bool minorMode)
    : MIDIEvent(), m_alterations(alterations), m_minorMode(minorMode)
{ }

BeatEvent::BeatEvent()
    : MIDIEvent(), m_bar(0), m_beat(0), m_max(0)
{ }

BeatEvent::BeatEvent(const int bar, const int beat, const int max)
    : MIDIEvent(), m_bar(bar), m_beat(beat), m_max(max)
{ }

#ifndef QT_NO_QDEBUG
void MIDIEvent::dumpTime()
{
    std::cout << std::setw(10) << std::right << m_tick << " ";
}

void MIDIEvent::dump()
{
    dumpTime();
}

void NoteOnEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "Note on";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << key() << " ";
    std::cout << std::setw(3) << velocity() << std::endl;
}

void NoteOffEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "note off";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << key() << " ";
    std::cout << std::setw(3) << velocity() << std::endl;
}
void KeyPressEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "polyphonic aftertouch";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << key() << " ";
    std::cout << std::setw(3) << velocity() << std::endl;
}

void ControllerEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "control change";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << param() << " ";
    std::cout << std::setw(3) << value() << std::endl;
}

void ProgramChangeEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "program change";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << program() << std::endl;
}

void PitchBendEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "pitch bend";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(6) << value() << std::endl;
}

void ChanPressEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(23) << std::left << "channel aftertouch";
    std::cout << std::setw(2) << std::right << channel() << " ";
    std::cout << std::setw(3) << value() << std::endl;
}

void SysExEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::setw(26) << std::left << "system exclusive" << std::setw(0);
    for (int i = 0; i < data().size(); ++i) {
        unsigned int digits = data()[i] & 0xff;
        std::cout << std::hex << digits << " ";
    }
    std::cout << std::dec << std::endl;
}

void TextEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "text event type: " << textType()
              << " text: " << data().toStdString();
    std::cout << std::endl;
}

void SystemEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "system event: " << std::hex << status() << std::dec << std::endl;
}

void TempoEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "tempo: " << tempo() << " (" << bpm() << " bpm)";
    std::cout << std::endl;
}

void TimeSignatureEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "time signature: " << numerator() << "/" << denominator();
    std::cout << std::endl;
}

void KeySignatureEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "key signature " << (minorMode() ? "minor" : "major") << " "
              << alterations();
    std::cout << std::endl;
}

void BeatEvent::dump()
{
    MIDIEvent::dump();
    std::cout << std::left << "rhythm. bar: " << bar() << " beat: " << beat() << "/" << barLength();
    std::cout << std::endl;
}
#endif
