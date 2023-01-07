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

#include "instrumentset.h"

InstrumentSet::InstrumentSet()
{
    m_names.reserve(128);
    reloadNames();
}

InstrumentSet::~InstrumentSet()
{ }

void InstrumentSet::reloadNames()
{
    m_names.clear();
    m_names.insert(0, tr("Acoustic Grand Piano"));
    m_names.insert(1, tr("Bright Acoustic Piano"));
    m_names.insert(2, tr("Electric Grand Piano"));
    m_names.insert(3, tr("Honky-Tonk"));
    m_names.insert(4, tr("Rhodes Piano"));
    m_names.insert(5, tr("Chorused Piano"));
    m_names.insert(6, tr("Harpsichord"));
    m_names.insert(7, tr("Clavinet"));
    m_names.insert(8, tr("Celesta"));
    m_names.insert(9, tr("Glockenspiel"));
    m_names.insert(10, tr("Music Box"));
    m_names.insert(11, tr("Vibraphone"));
    m_names.insert(12, tr("Marimba"));
    m_names.insert(13, tr("Xylophone"));
    m_names.insert(14, tr("Tubular Bells"));
    m_names.insert(15, tr("Dulcimer"));
    m_names.insert(16, tr("Hammond Organ"));
    m_names.insert(17, tr("Percussive Organ"));
    m_names.insert(18, tr("Rock Organ"));
    m_names.insert(19, tr("Church Organ"));
    m_names.insert(20, tr("Reed Organ"));
    m_names.insert(21, tr("Accordion"));
    m_names.insert(22, tr("Harmonica"));
    m_names.insert(23, tr("Tango Accordion"));
    m_names.insert(24, tr("Acoustic Guitar (Nylon)"));
    m_names.insert(25, tr("Acoustic Guitar (Steel)"));
    m_names.insert(26, tr("Electric Guitar (Jazz)"));
    m_names.insert(27, tr("Electric Guitar (Clean)"));
    m_names.insert(28, tr("Electric Guitar (Muted)"));
    m_names.insert(29, tr("Overdriven Guitar"));
    m_names.insert(30, tr("Distortion Guitar"));
    m_names.insert(31, tr("Guitar Harmonics"));
    m_names.insert(32, tr("Acoustic Bass"));
    m_names.insert(33, tr("Electric Bass (Finger)"));
    m_names.insert(34, tr("Electric Bass (Pick)"));
    m_names.insert(35, tr("Fretless Bass"));
    m_names.insert(36, tr("Slap Bass 1"));
    m_names.insert(37, tr("Slap Bass 2"));
    m_names.insert(38, tr("Synth Bass 1"));
    m_names.insert(39, tr("Synth Bass 2"));
    m_names.insert(40, tr("Violin"));
    m_names.insert(41, tr("Viola"));
    m_names.insert(42, tr("Cello"));
    m_names.insert(43, tr("Contrabass"));
    m_names.insert(44, tr("Tremolo Strings"));
    m_names.insert(45, tr("Pizzicato Strings"));
    m_names.insert(46, tr("Orchestral Harp"));
    m_names.insert(47, tr("Timpani"));
    m_names.insert(48, tr("String Ensemble 1"));
    m_names.insert(49, tr("String Ensemble 2"));
    m_names.insert(50, tr("Synth Strings 1"));
    m_names.insert(51, tr("Synth Strings 2"));
    m_names.insert(52, tr("Choir Aahs"));
    m_names.insert(53, tr("Voice Oohs"));
    m_names.insert(54, tr("Synth Voice"));
    m_names.insert(55, tr("Orchestra Hit"));
    m_names.insert(56, tr("Trumpet"));
    m_names.insert(57, tr("Trombone"));
    m_names.insert(58, tr("Tuba"));
    m_names.insert(59, tr("Muted Trumpet"));
    m_names.insert(60, tr("French Horn"));
    m_names.insert(61, tr("Brass Section"));
    m_names.insert(62, tr("Synth Brass 1"));
    m_names.insert(63, tr("Synth Brass 2"));
    m_names.insert(64, tr("Soprano Sax"));
    m_names.insert(65, tr("Alto Sax"));
    m_names.insert(66, tr("Tenor Sax"));
    m_names.insert(67, tr("Baritone Sax"));
    m_names.insert(68, tr("Oboe"));
    m_names.insert(69, tr("English Horn"));
    m_names.insert(70, tr("Bassoon"));
    m_names.insert(71, tr("Clarinet"));
    m_names.insert(72, tr("Piccolo"));
    m_names.insert(73, tr("Flute"));
    m_names.insert(74, tr("Recorder"));
    m_names.insert(75, tr("Pan Flute"));
    m_names.insert(76, tr("Blown Bottle"));
    m_names.insert(77, tr("Shakuhachi"));
    m_names.insert(78, tr("Whistle"));
    m_names.insert(79, tr("Ocarina"));
    m_names.insert(80, tr("Lead 1 - Square Wave"));
    m_names.insert(81, tr("Lead 2 - Saw Tooth"));
    m_names.insert(82, tr("Lead 3 - Calliope"));
    m_names.insert(83, tr("Lead 4 - Chiflead"));
    m_names.insert(84, tr("Lead 5 - Charang"));
    m_names.insert(85, tr("Lead 6 - Voice"));
    m_names.insert(86, tr("Lead 7 - Fifths"));
    m_names.insert(87, tr("Lead 8 - Bass+Lead"));
    m_names.insert(88, tr("Pad 1 - New Age"));
    m_names.insert(89, tr("Pad 2 - Warm"));
    m_names.insert(90, tr("Pad 3 - Polysynth"));
    m_names.insert(91, tr("Pad 4 - Choir"));
    m_names.insert(92, tr("Pad 5 - Bow"));
    m_names.insert(93, tr("Pad 6 - Metallic"));
    m_names.insert(94, tr("Pad 7 - Halo"));
    m_names.insert(95, tr("Pad 8 - Sweep"));
    m_names.insert(96, tr("FX 1 - Rain"));
    m_names.insert(97, tr("FX 2 - Soundtrack"));
    m_names.insert(98, tr("FX 3 - Crystal"));
    m_names.insert(99, tr("FX 4 - Atmosphere"));
    m_names.insert(100, tr("FX 5 - Brightness"));
    m_names.insert(101, tr("FX 6 - Goblins"));
    m_names.insert(102, tr("FX 7 - Echoes"));
    m_names.insert(103, tr("FX 8 - Sci-fi"));
    m_names.insert(104, tr("Sitar"));
    m_names.insert(105, tr("Banjo"));
    m_names.insert(106, tr("Shamisen"));
    m_names.insert(107, tr("Koto"));
    m_names.insert(108, tr("Kalimba"));
    m_names.insert(109, tr("Bagpipe"));
    m_names.insert(110, tr("Fiddle"));
    m_names.insert(111, tr("Shannai"));
    m_names.insert(112, tr("Tinkle Bell"));
    m_names.insert(113, tr("Agogo"));
    m_names.insert(114, tr("Steel Drum"));
    m_names.insert(115, tr("Wood Block"));
    m_names.insert(116, tr("Taiko Drum"));
    m_names.insert(117, tr("Melodic Tom"));
    m_names.insert(118, tr("Synth Drum"));
    m_names.insert(119, tr("Reverse Cymbal"));
    m_names.insert(120, tr("Guitar Fret Noise"));
    m_names.insert(121, tr("Breath Noise"));
    m_names.insert(122, tr("Seashore"));
    m_names.insert(123, tr("Bird Tweet"));
    m_names.insert(124, tr("Telephone"));
    m_names.insert(125, tr("Helicopter"));
    m_names.insert(126, tr("Applause"));
    m_names.insert(127, tr("Gunshot"));

    m_numbers.clear();
    for(int i=1; i<129; ++i)
        m_numbers.append(tr("Percussion %1").arg(i));
}

QString InstrumentSet::name(int patch, bool percussion) const
{
    if (percussion)
        return m_numbers.value(patch);
    return m_names.value(patch);
}

QStringList InstrumentSet::names(bool percussion) const
{
    if (percussion)
        return m_numbers;
    return m_names.toList();
}
