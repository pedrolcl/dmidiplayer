# Drumstick Multiplatform MIDI File Player

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
here it is. But this program is much more than that, and also works on Linux...

Some key features:

* MIDI Output to hardware MIDI ports, or any other Drumstick backend like soft synths
* Transpose song tonality between -12 and +12 semitones
* Change MIDI volume level (using MIDI CC7)
* Scale song speed between half and double tempo
* Lyrics, Piano Player and MIDI Channels views
* Supports MID/KAR (Standard MIDI Files) and WRK (Cakewalk) file formats
