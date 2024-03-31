Drumstick Multiplatform MIDI File Player
========================================

[![Build on Linux](https://github.com/pedrolcl/dmidiplayer/actions/workflows/linux-build.yml/badge.svg)](https://github.com/pedrolcl/dmidiplayer/actions/workflows/linux-build.yml)

This application is a multiplatform MIDI file player for Linux, Windows and macOS. It reads .MID (Standard MIDI Files), .KAR (Karaoke), and .WRK (Cakewalk) file formats, and outputs MIDI events to hardware MIDI ports and also software synths.

[Drumstick](https://drumstick.sourceforge.io/docs/index.html) is a set of GPLv3 licensed C++/Qt libraries for MIDI applications. The project includes several tools as examples, and among them is the drumstick-guiplayer utility that leverages the Drumstick::ALSA library, so it is available only for Linux (because the ALSA sequencer is a linux only technology). Some people have requested a program with the same functionalities on Windows and macOS, and here it is. But this program is much more than that, and also works on Linux ...

![Screenshot](screenshot.png "all windows")

[![Screencast at YouTube](https://img.youtube.com/vi/WgwxFmAsicc/0.jpg)](https://www.youtube.com/watch?v=WgwxFmAsicc)

Some key features:

* MIDI Output to hardware MIDI ports, or any other Drumstick backend like soft synths
* Transpose song tonality between -12 and +12 semitones
* Change MIDI volume level (using MIDI CC7)
* Scale song speed between half and double tempo
* Lyrics, Piano Player and MIDI Channels views
* Supports MID/KAR/RMI (Standard MIDI Files) and WRK (Cakewalk) file formats

This multiplatform version offers equivalent functionality replacing Drumstick::ALSA by Drumstick::RT. The MIDI events scheduling/timing is performed inside the program with the only  help of the C++ standard library (threads and chrono). The MIDI output still has access to each operating system's MIDI infrastructure, but also to additional backends like ipMidi and soft synths.

Alright, these are the build requirements:

* C++11 compiler
* [Qt 5](https://www.qt.io/download) >= 5.15 or Qt6 >= 6.2
* [Drumstick 2.8](https://sourceforge.net/projects/drumstick/)
* [Uchardet 0.0.8](https://www.freedesktop.org/wiki/Software/uchardet/)
* [pandoc](https://pandoc.org/)
* [CMake 3.14](https://cmake.org/)

Build and deployment commands (for Linux)

```
$ tar -xvzf dmidiplayer-x.y.z.tar.gz
$ cd dmidiplayer-x.y.z
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_PREFIX_PATH="$HOME/Qt5;$HOME/drumstick2;$HOME/uchardet"
$ make
$ make install
```

You probably don't need to use the CMake variable CMAKE_PREFIX_PATH at all, if your dependencies are installed on some standard prefix like "/usr" on Linux. Otherwise, you need to replace the contents of the parameter CMAKE_PREFIX_PATH with the actual paths in your system (in the example, each dependency was installed on a subdirectory under the $HOME directory). If you don't want to compile the program yourself, there are x86_64 precompiled packages for Linux, Windows and macOS at Sourceforge.

[![Download Drumstick Multiplatform MIDI File Player](https://a.fsdn.com/con/app/sf-download-button)](https://sourceforge.net/projects/dmidiplayer/files/latest/download)

Binary packages for several Linux distributions:

[![Packaging status](https://repology.org/badge/vertical-allrepos/dmidiplayer.svg)](https://repology.org/project/dmidiplayer/versions)

Enjoy!
