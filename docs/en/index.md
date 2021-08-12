% Help Index

## Introduction

This application is a multiplatform MIDI file player for Linux, Windows and 
macOS. It reads .MID (Standard MIDI Files), .KAR (Karaoke), and .WRK (Cakewalk) 
file formats, and outputs MIDI events to hardware MIDI ports and also software 
synths.

[Drumstick](https://drumstick.sourceforge.io) is a set of GPLv3 
licensed C++/Qt libraries for MIDI applications. The project includes several 
tools as examples, and among them is the drumstick-guiplayer utility that 
leverages the Drumstick::ALSA library, so it is available only for Linux 
(because the ALSA sequencer is a linux only technology). Some people have 
requested a program with the same functionalities on Windows and macOS, and 
here it is. But this program is much more than that, and it also works on 
Linux...

Some key features:

* MIDI Output to hardware MIDI ports, or any other Drumstick backend like soft
  synths
* [Transpose](https://en.wikipedia.org/wiki/Transposition_(music)) song tonality 
  between -12 and +12 semitones
* Change MIDI volume level (using MIDI CC7)
* Scale song speed between half and double 
  [tempo](https://en.wikipedia.org/wiki/Tempo)
* Lyrics, Piano Player and MIDI Channels views
* Supports MID/KAR (Standard MIDI Files) and WRK (Cakewalk) file formats

## MIDI Output

Any MIDI Player outputs MIDI events only, but no sound. To produce sound, a MIDI 
Synthesizer is needed to render the events into digital audio that can be 
reproduced by the computer's (or external) audio speakers. The events can also 
be rendered by an external musical MIDI instrument connected to the computer, 
for instance a MIDI organ, piano, or synthesizer with an USB interface. If that 
option is not available, another usual alternative is a software synthesizer 
(a program).

There are software synthesizers included in the computer's operating systems, 
for instance Microsoft Windows includes the "Microsoft GS Wavetable Synth", and
Apple macOS includes the "Apple DLS Synthesizer". To use them, you need to open
the "MIDI Setup" dialog and then:

* In **Windows**, select the "Windows MM" option in the first list box, and 
  "Microsoft GS Wavetable Synth" in the second list box. Instead of this synth,
  which is very basic and limited, you may prefer a better/free alternative like 
  [Virtual MIDI Synth](https://coolsoft.altervista.org/en/virtualmidisynth)
  that will show as "VirtualMIDISynth #n" in the second list box when running.
  If you have an external MIDI instrument attached to the computer, its name
  should appear in the second list box as well.
* In **macOS**, select "DLS Synth" in both the first and second list boxes. 
  This synth has a configuration option for using DLS or SF2 soundfont files if 
  you want. To use another connected MIDI device or software synth, 
  you need to select "CoreMIDI" in the first list box, and the name of your
  synth in the second one.
* In **Linux** and other **Unix** Operating Systems, "SonivoxEAS" is a soft 
  synth ready to be used. It is a very basic GM synth which doesn't use files 
  and depends only on the PulseAudio sound system. For external devices attached 
  to the computer or arbitrary soft synths, you need to select "ALSA" (or "OSS") 
  in the first list box.
* **FluidSynth** is another included open software synth that is multiplatform 
  and available on all operating systems. It is very configurable, accepting 
  soundfont files and many digital audio sound systems. Select 
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) on both list boxes 
  to use it.
* **Network** is a multiplatform UDP Multicast output, that may be used to send 
  MIDI events as network packets to other computers and devices in the same 
  local area network. It is compatible with 
  [ipMIDI](https://www.nerds.de/en/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/), and 
  [multimidicast](https://llg.cubic.org/tools/multimidicast/) 
  among other software.

## Playback Controls

The following standard actions are available in the `File` menu and also the
tool bar, with the conventional functionalities on any media player: 

* `Play` 
* `Pause` (and continue)
* `Stop`, 
* `Fast Forward` (by 1 bar)
* `Rewind` (by 1 bar)

By default, `Play` is activated automatically upon loading a MIDI file except 
when this feature is disabled in the configuration dialog.

The `status bar`, optionally located at the bottom of the main window, shows 
the current playback status as "Playing", "Stopped", "Paused", etc.

### Transpose

The `Pitch control` is located in the main window. It is a spin box initialized
to zero, with a valid range from -12 to +12 semitones. The limits are equivalent 
to one octave up or down the original tone. This should be enough for singers
or instrumentists needing to adjust the tonality for their comfort.

All MIDI channels are chromatically transposed, except the percussion channel 
(GM channel 10 by default).

### Volume Level
 
The `Volume control` is located in the main window. It is a slider, initialized
to 100%, with a valid range from 0% to 200%. The upper limit effect may be lower 
because the MIDI CC 7 has a valid range from 0 to 127. There is also a button to
`Reset Volume`.
 
### Tempo

The `Tempo control` is located in the main window. It is a slider initialized
to 100% with a valid range from 50% to 200%. There is a button to `Reset Tempo`
and also a tempo display, showing the effective tempo in beats per minute with
an initial value of 120 bpm. This is the default tempo for a MIDI file if it 
does not contain a tempo change. This value is updated when changing the
tempo control, and also while a MIDI file containing tempo changes is playing.

### Jump, Loop and Positioning

`Jump` action (located in the `File` menu and the default tool bar), asks for
a bar number, between 1 and the last bar of the loaded song, to change the 
current position of the song to the given bar.

`Loop` action (located in the `File` menu and the default tool bar), asks for
two bar numbers. The song will be played between the two given bars when it is
activated. You can switch off the loop any moment selecting the action again.

The `Positioning control` is located in the main window. It is a slider that
moves while the current played position changes, and can be moved by hand to
change the position to any arbitrary point within the song.

## Views

### Channels

### Piano Player

### Lyrics

## Playlists

Playlists are collections of file names, that can be used to play a series 
of MIDI pieces, like a musical "repertoire".

The `File`->`Play List...` action manages the creation, modification, ordering, 
opening, and saving Play list files. This window shows the current playlist 
file name in the window title.

The `File`->`Next` and `File`->`Prev` actions are used to navigate the playlist
manually forward and backward. There are configuration options to start playback 
automatically after loading MIDI files, and advance automatically to the next 
playlist item. The two configuration options are enabled by default.

The initial default playlist contains a collection of example MIDI files. If
you prefer to start with an empty list, you should use the `Clear` button and
then saving the empty playlist with a meaningful name. This will be the new 
initial playlist in the future.

The last opened or saved playlist file shall be remembered the next time the 
program is started, but playlists are not saved automatically.

Playlists are simple text files, with a file name on each text line. The file 
names can be relative to the location of the playlist file itself (so they do 
not have an absolute path prepended) or each file can have an absolute path on 
its own.

You may have several playlists that can be created easily outside of the 
program, for instance in the Linux command line this command creates a playlist 
file named "my_playlist.lst" with all the MIDI files located at the working 
directory:

~~~
$ ls -1 *.mid > my_playlist.lst
~~~

## Opening MIDI Files

You can open individual files...

* Using the `File`->`Open` standard action
* Using the `File`->`Open Recent Files` menu, which remembers up to ten files 
  recently opened.
* Providing the file name(s) in the command line. The set of files becomes a
  temporary play list. This allows easy integration of the program with file
  managers using "Open With..." actions.
* Dragging and dropping file(s) from a file manager into the main window. The 
  set of files becomes a temporary play list.

## Customization

### Preferences

### Toolbar Customization

