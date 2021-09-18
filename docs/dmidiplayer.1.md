% DMIDIPLAYER(1) dmidiplayer 0.0.0 | Drumstick MIDI File Player
% Pedro López-Cabanillas <plcl@users.sf.net>

# NAME

**dmidiplayer** — Plays MIDI, Karaoke and Cakewalk Files

# SYNOPSIS

| **dmidiplayer** \[**-p**|**--portable**] [**-f**|**--file** _file_] [**-d**|**--driver** _driver_] [**-c**|**--connection** _connection_] \[_midi_files_]
| **dmidiplayer** \[**-h**|**--help**|**-v**|**--version**]

# DESCRIPTION

This program is a multiplatform MIDI file player for Linux, Windows and macOS. 
It reads .MID (Standard MIDI Files), .KAR (Karaoke), .RMI (RIFF RMID), and .WRK (Cakewalk) file formats, 
and outputs MIDI events to hardware MIDI ports and also software synths.

## Options

-h, --help

:   Prints brief usage information.

-v, --version

:   Prints the current version number.

-p, --portable

:   Portable settings mode.

-f, --file _portableFile_

:   Portable settings file name.
  
-d, --driver _driver_

:   Drumstick::RT MIDI Out Driver.

-c, --connection _connection_

:   Drumstick::RT MIDI Out Connection.

## Standard Options

The following options apply to all Qt5 applications:

-style= _style_ / -style _style_
:   Set the application GUI style. Possible values depend on the system configuration. 
    If Qt is compiled with additional styles or has additional styles as plugins these
    will be available to the -style command line option.

-stylesheet= _stylesheet_ / -stylesheet _stylesheet_
:   Set the application styleSheet. The value must be a path to a file that contains the Style Sheet.

-widgetcount
:   Print debug message at the end about number of widgets left undestroyed and maximum number of widgets existed at the same time.

-reverse
:   Set the application's layout direction to Qt::RightToLeft. This option is intended to aid debugging and should not be used in production. 
    The default value is automatically detected from the user's locale (see also QLocale::textDirection()).

-platform _platformName[:options]_
:   Specify the Qt Platform Abstraction (QPA) plugin.

-platformpluginpath _path_
:   Specify the path to platform plugins.

-platformtheme _platformTheme_
:   Specify the platform theme.

-plugin _plugin_
:   Specify additional plugins to load. The argument may appear multiple times.

-qwindowgeometry _geometry_
:   Specify the window geometry for the main window using the X11-syntax. For example: -qwindowgeometry 100x100+50+50

-qwindowicon _icon_
:   Set the default window icon.

-qwindowtitle _title_
:   Set the title of the first window.

-session _session_
:   Restore the application from an earlier session.

-display _hostname:screen_number_
:   Switch displays on X11. Overrides the DISPLAY environment variable.

-geometry _geometry_
:   Specify the window geometry for the main window on X11. For example: -geometry 100x100+50+50

-dialogs= _[xp|none]_
:   Only available for the Windows platform. XP uses native style dialogs and none disables them.

-fontengine= _freetype_
:   Use the FreeType font engine.

## Arguments

_midi_files_

:   Input MID/KAR/RMI/WRK file name(s). The set of provided file names becomes a temporary playlist.

# FILES

*\*.lst*

:   Playlist files. The last opened or saved playlist file shall be remembered the next time the program is started, but playlists are not saved automatically.

    Playlists are simple text files, with a file name on each text line. The file names can be relative to the location of the playlist file itself (so they do not have an absolute path prepended) or each file can have an absolute path on its own.

    Users may have several playlists that can be created easily outside of the program, for instance in the Linux command line this command creates a playlist file named "my_playlist.lst" with all the MIDI files located at the working directory:

~~~
$ ls -1 *.mid > my_playlist.lst
~~~

# ENVIRONMENT

**DRUMSTICKRT**

:   Location of Drumstick::RT plugins. Useful if the plugins are not installed at a standard place.

# BUGS

See Tickets at Sourceforge <https://sourceforge.net/p/dmidiplayer/tickets/> and GitHub <https://github.com/pedrolcl/dmidiplayer/issues/>

# SEE ALSO

**qt5options (7)**
