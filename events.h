/*
    Drumstick MIDI File Player Multiplatform Program
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

#ifndef EVENTS_H
#define EVENTS_H

#include <QEvent>


/**
 * @file events.h
 * Classes managing MIDI events.
 *
 * @defgroup Sequencer MIDI Events
 * @{
 */

/**
 * Constant SequencerEventType is the QEvent::type() of any MIDIEvent
 * object to be used to check the argument in QObject::customEvent().
 */
const QEvent::Type MIDIEventType = QEvent::Type(QEvent::User + 3161);

/**
 * Base class for the event's hierarchy
 *
 * All event classes share this base class. It provides common properties
 */
class MIDIEvent : public QEvent
{
public:
    MIDIEvent();
    virtual ~MIDIEvent() = default;
    virtual MIDIEvent *clone() { return new MIDIEvent(*this); }

    long delta() const { return m_delta; }
    long tick() const { return m_tick; }
    void setDelta(const long delta);
    void setTick(const long tick);
    int tag() const { return m_tag; }
    void setTag(const int aTag);
    int status() const { return m_status; }

    virtual bool isChannel() { return false; }
    virtual bool isTempo() { return false; }
    virtual bool isText() { return false; }
    virtual bool isMetaEvent() { return false; }
protected:
    long m_delta;
    long m_tick;
    int m_tag;
    int m_status;
};

/**
 * Base class for the events having a Channel property
 */
class ChannelEvent : public MIDIEvent
{
public:
    ChannelEvent() : MIDIEvent(), m_channel(0) {}
    ChannelEvent(int ch): MIDIEvent(), m_channel(ch) {}
    virtual bool isChannel() { return true; }
    /**
     * Sets the channel of the event
     * @param c A channel, between 0 and 15.
     * @see channel()
     */
    void setChannel(const int c) { m_channel = (c & 0xf); }
    /**
     * Gets the event's channel
     * @return The event's channel
     * @see setChannel()
     */
    int channel() const { return m_channel; }
protected:
    int m_channel;
};

/**
 * Base class for the events having Key and Velocity properties.
 */
class KeyEvent : public ChannelEvent
{
public:
    KeyEvent() : ChannelEvent(), m_note(0), m_velocity(0) {}
    KeyEvent(int ch, int key, int vel): ChannelEvent(ch), m_note(key), m_velocity(vel) {}
    /**
     * Gets the MIDI note of this event.
     * @return The event's MIDI note.
     * @see setKey()
     */
    int key() const { return m_note; }
    /**
     * Sets the MIDI note of this event.
     * @param b A MIDI note, between 0 and 127.
     * @see getKey()
     */
    void setKey(const int b) { m_note = b; }
    /**
     * Gets the note velocity of this event.
     * @return The event's note velocity.
     * @see setVelocity()
     */
    int velocity() const { return m_velocity; }
    /**
     * Sets the note velocity of this event.
     * @param b A velocity value, between 0 and 127.
     * @see velocity()
     */
    void setVelocity(const int b) { m_velocity = b; }
protected:
    int m_note;
    int m_velocity;
};


/**
 * Event representing a note-on MIDI event
 */
class NoteOnEvent : public KeyEvent
{
public:
    NoteOnEvent() : KeyEvent() { }
    NoteOnEvent(const int ch, const int key, const int vel);
    virtual NoteOnEvent *clone() { return new NoteOnEvent(*this); }
};

/**
 * Event representing a note-off MIDI event
 */
class NoteOffEvent : public KeyEvent
{
public:
    NoteOffEvent() : KeyEvent() {}
    NoteOffEvent(const int ch, const int key, const int vel);
    virtual NoteOffEvent *clone() { return new NoteOffEvent(*this); }
};

/**
 * Event representing a MIDI key pressure, or polyphonic after-touch event
 */
class KeyPressEvent : public KeyEvent
{
public:
    KeyPressEvent() : KeyEvent() { }
    KeyPressEvent(const int ch, const int key, const int vel);
    virtual KeyPressEvent *clone() { return new KeyPressEvent(*this); }
};

/**
 * Event representing a MIDI control change event
 */
class ControllerEvent : public ChannelEvent
{
public:
    ControllerEvent() : ChannelEvent(), m_param(0), m_value(0) {}
    ControllerEvent(const int ch, const int cc, const int val);
    virtual ControllerEvent *clone() { return new ControllerEvent(*this); }

    static const int MIDI_CTL_MSB_BANK               = 0x00;    /**< Bank selection */
    static const int MIDI_CTL_MSB_MODWHEEL           = 0x01;    /**< Modulation */
    static const int MIDI_CTL_MSB_BREATH             = 0x02;    /**< Breath */
    static const int MIDI_CTL_MSB_FOOT               = 0x04;    /**< Foot */
    static const int MIDI_CTL_MSB_PORTAMENTO_TIME    = 0x05;    /**< Portamento time */
    static const int MIDI_CTL_MSB_DATA_ENTRY         = 0x06;    /**< Data entry */
    static const int MIDI_CTL_MSB_MAIN_VOLUME        = 0x07;    /**< Main volume */
    static const int MIDI_CTL_MSB_BALANCE            = 0x08;    /**< Balance */
    static const int MIDI_CTL_MSB_PAN                = 0x0a;    /**< Panpot */
    static const int MIDI_CTL_MSB_EXPRESSION         = 0x0b;    /**< Expression */
    static const int MIDI_CTL_MSB_EFFECT1            = 0x0c;    /**< Effect1 */
    static const int MIDI_CTL_MSB_EFFECT2            = 0x0d;    /**< Effect2 */
    static const int MIDI_CTL_MSB_GENERAL_PURPOSE1   = 0x10;    /**< General purpose 1 */
    static const int MIDI_CTL_MSB_GENERAL_PURPOSE2   = 0x11;    /**< General purpose 2 */
    static const int MIDI_CTL_MSB_GENERAL_PURPOSE3   = 0x12;    /**< General purpose 3 */
    static const int MIDI_CTL_MSB_GENERAL_PURPOSE4   = 0x13;    /**< General purpose 4 */
    static const int MIDI_CTL_LSB_BANK               = 0x20;    /**< Bank selection */
    static const int MIDI_CTL_LSB_MODWHEEL           = 0x21;    /**< Modulation */
    static const int MIDI_CTL_LSB_BREATH             = 0x22;    /**< Breath */
    static const int MIDI_CTL_LSB_FOOT               = 0x24;    /**< Foot */
    static const int MIDI_CTL_LSB_PORTAMENTO_TIME    = 0x25;    /**< Portamento time */
    static const int MIDI_CTL_LSB_DATA_ENTRY         = 0x26;    /**< Data entry */
    static const int MIDI_CTL_LSB_MAIN_VOLUME        = 0x27;    /**< Main volume */
    static const int MIDI_CTL_LSB_BALANCE            = 0x28;    /**< Balance */
    static const int MIDI_CTL_LSB_PAN                = 0x2a;    /**< Panpot */
    static const int MIDI_CTL_LSB_EXPRESSION         = 0x2b;    /**< Expression */
    static const int MIDI_CTL_LSB_EFFECT1            = 0x2c;    /**< Effect1 */
    static const int MIDI_CTL_LSB_EFFECT2            = 0x2d;    /**< Effect2 */
    static const int MIDI_CTL_LSB_GENERAL_PURPOSE1   = 0x30;    /**< General purpose 1 */
    static const int MIDI_CTL_LSB_GENERAL_PURPOSE2   = 0x31;    /**< General purpose 2 */
    static const int MIDI_CTL_LSB_GENERAL_PURPOSE3   = 0x32;    /**< General purpose 3 */
    static const int MIDI_CTL_LSB_GENERAL_PURPOSE4   = 0x33;    /**< General purpose 4 */
    static const int MIDI_CTL_SUSTAIN                = 0x40;    /**< Sustain pedal */
    static const int MIDI_CTL_PORTAMENTO             = 0x41;    /**< Portamento */
    static const int MIDI_CTL_SOSTENUTO              = 0x42;    /**< Sostenuto */
    static const int MIDI_CTL_SUSTENUTO              = 0x42;    /**< Sostenuto (a typo in the older version) */
    static const int MIDI_CTL_SOFT_PEDAL             = 0x43;    /**< Soft pedal */
    static const int MIDI_CTL_LEGATO_FOOTSWITCH      = 0x44;    /**< Legato foot switch */
    static const int MIDI_CTL_HOLD2                  = 0x45;    /**< Hold2 */
    static const int MIDI_CTL_SC1_SOUND_VARIATION    = 0x46;    /**< SC1 Sound Variation */
    static const int MIDI_CTL_SC2_TIMBRE             = 0x47;    /**< SC2 Timbre */
    static const int MIDI_CTL_SC3_RELEASE_TIME       = 0x48;    /**< SC3 Release Time */
    static const int MIDI_CTL_SC4_ATTACK_TIME        = 0x49;    /**< SC4 Attack Time */
    static const int MIDI_CTL_SC5_BRIGHTNESS         = 0x4a;    /**< SC5 Brightness */
    static const int MIDI_CTL_SC6                    = 0x4b;    /**< SC6 */
    static const int MIDI_CTL_SC7                    = 0x4c;    /**< SC7 */
    static const int MIDI_CTL_SC8                    = 0x4d;    /**< SC8 */
    static const int MIDI_CTL_SC9                    = 0x4e;    /**< SC9 */
    static const int MIDI_CTL_SC10                   = 0x4f;    /**< SC10 */
    static const int MIDI_CTL_GENERAL_PURPOSE5       = 0x50;    /**< General purpose 5 */
    static const int MIDI_CTL_GENERAL_PURPOSE6       = 0x51;    /**< General purpose 6 */
    static const int MIDI_CTL_GENERAL_PURPOSE7       = 0x52;    /**< General purpose 7 */
    static const int MIDI_CTL_GENERAL_PURPOSE8       = 0x53;    /**< General purpose 8 */
    static const int MIDI_CTL_PORTAMENTO_CONTROL     = 0x54;    /**< Portamento control */
    static const int MIDI_CTL_E1_REVERB_DEPTH        = 0x5b;    /**< E1 Reverb Depth */
    static const int MIDI_CTL_E2_TREMOLO_DEPTH       = 0x5c;    /**< E2 Tremolo Depth */
    static const int MIDI_CTL_E3_CHORUS_DEPTH        = 0x5d;    /**< E3 Chorus Depth */
    static const int MIDI_CTL_E4_DETUNE_DEPTH        = 0x5e;    /**< E4 Detune Depth */
    static const int MIDI_CTL_E5_PHASER_DEPTH        = 0x5f;    /**< E5 Phaser Depth */
    static const int MIDI_CTL_DATA_INCREMENT         = 0x60;    /**< Data Increment */
    static const int MIDI_CTL_DATA_DECREMENT         = 0x61;    /**< Data Decrement */
    static const int MIDI_CTL_NONREG_PARM_NUM_LSB    = 0x62;    /**< Non-registered parameter number */
    static const int MIDI_CTL_NONREG_PARM_NUM_MSB    = 0x63;    /**< Non-registered parameter number */
    static const int MIDI_CTL_REGIST_PARM_NUM_LSB    = 0x64;    /**< Registered parameter number */
    static const int MIDI_CTL_REGIST_PARM_NUM_MSB    = 0x65;    /**< Registered parameter number */
    static const int MIDI_CTL_ALL_SOUNDS_OFF         = 0x78;    /**< All sounds off */
    static const int MIDI_CTL_RESET_CONTROLLERS      = 0x79;    /**< Reset Controllers */
    static const int MIDI_CTL_LOCAL_CONTROL_SWITCH   = 0x7a;    /**< Local control switch */
    static const int MIDI_CTL_ALL_NOTES_OFF          = 0x7b;    /**< All notes off */
    static const int MIDI_CTL_OMNI_OFF               = 0x7c;    /**< Omni off */
    static const int MIDI_CTL_OMNI_ON                = 0x7d;    /**< Omni on */
    static const int MIDI_CTL_MONO1                  = 0x7e;    /**< Mono1 */
    static const int MIDI_CTL_MONO2                  = 0x7f;    /**< Mono2 */

    /**
     * Gets the controller event's parameter.
     * @return The controller event's parameter.
     * @see setParam()
     */
    int param() const { return m_param; }
    /**
     * Sets the controller event's parameter.
     * @param p The controller event's parameter.
     * @see getParam()
     */
    void setParam( const int p ) { m_param = p; }
    /**
     * Gets the controller event's value.
     * @return The controller event's value.
     * @see setValue()
     */
    int value() const { return m_value; }
    /**
     * Sets the controller event's value.
     * @param v The controller event's value.
     * @see value()
     */
    void setValue( const int v ) { m_value = v; }
protected:
    int m_param;
    int m_value;
};

/**
 * Event representing a MIDI program change event
 */
class ProgramChangeEvent : public ChannelEvent
{
public:
    ProgramChangeEvent() : ChannelEvent(), m_program(0) { }
    ProgramChangeEvent(const int ch, const int val);
    virtual ProgramChangeEvent *clone() { return new ProgramChangeEvent(*this); }
    /** Gets the MIDI program number */
    int program() const { return m_program; }
    /** Sets the MIDI program number */
    void setProgram( const int v ) { m_program = v; }
protected:
    int m_program;
};

/**
 * Event representing a MIDI bender, or pitch wheel event
 */
class PitchBendEvent : public ChannelEvent
{
public:
    PitchBendEvent() : ChannelEvent(), m_value(0) { }
    PitchBendEvent(const int ch, const int val);
    virtual PitchBendEvent *clone() { return new PitchBendEvent(*this); }
    /** Gets the MIDI pitch bend value, zero centered from -8192 to 8191 */
    int value() const { return m_value; }
    /** Sets the MIDI pitch bend value, zero centered from -8192 to 8191  */
    void setValue( const int v ) { m_value = v; }
protected:
    int m_value;
};

/**
 * Event representing a MIDI channel pressure or after-touch event
 */
class ChanPressEvent : public ChannelEvent
{
public:
    ChanPressEvent() : ChannelEvent(), m_value(0) { }
    ChanPressEvent( const int ch, const int val);
    virtual ChanPressEvent *clone() { return new ChanPressEvent(*this); }
    /** Gets the channel aftertouch value */
    int value() const { return m_value; }
    /** Sets the channel aftertouch value */
    void setValue( const int v ) { m_value = v; }
protected:
    int m_value;
};

/**
 * Base class for variable length events
 */
class VariableEvent : public MIDIEvent
{
public:
    VariableEvent();
    VariableEvent(const QByteArray& data);
    VariableEvent(const unsigned int datalen, char* dataptr);
    virtual VariableEvent *clone() { return new VariableEvent(*this); }
    unsigned int length() const { return m_data.length(); }
    QByteArray data() const { return m_data; }
    void setData(const QByteArray& d) { m_data = d; }
protected:
    QByteArray m_data;
};

/**
 * Event representing a MIDI system exclusive event
 */
class SysExEvent : public VariableEvent
{
public:
    SysExEvent();
    SysExEvent(const QByteArray& data);
    SysExEvent(const unsigned int datalen, char* dataptr);
    virtual SysExEvent *clone() { return new SysExEvent(*this); }
};

/**
 * Event representing a SMF text event
 *
 * This event type is not intended to be transmitted over the wire to an
 * external device, but it is useful for sequencer programs or MIDI applications
 */
class TextEvent : public VariableEvent
{
public:
    TextEvent();
    TextEvent(const unsigned int datalen, char* dataptr);
    explicit TextEvent(const QString& text, const int textType = 1);
    virtual bool isText() { return true; }
    virtual TextEvent *clone() { return new TextEvent(*this); }
    QString text() const;
    int textType() const;
protected:
    int m_textType;
};

/**
 * Generic event
 */
class SystemEvent : public MIDIEvent
{
public:
    SystemEvent() : MIDIEvent() {}
    SystemEvent(const int message);
    virtual SystemEvent *clone() { return new SystemEvent(*this); }
    int message() const { return m_status; }
};


/**
 * Event representing a tempo change
 */
class TempoEvent : public MIDIEvent
{
public:
    TempoEvent();
    TempoEvent(const qreal tempo);
    virtual TempoEvent *clone() { return new TempoEvent(*this); }
    virtual bool isTempo() { return true; }
    qreal tempo() const { return m_tempo; }
    void setTempo(const qreal t) { m_tempo = t; }
protected:
    qreal m_tempo;
};

/** @} */

#endif //EVENTS_H
