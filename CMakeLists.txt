# Drumstick MIDI File player graphic program
# Copyright (C) 2006-2020 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)
project(drumstick-guiplayer VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.12 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.0 COMPONENTS File RT Widgets REQUIRED)

SET(guiplayer_SRCS
    connections.cpp
    connections.h
    connections.ui
    events.cpp
    events.h
    guiplayer.cpp
    guiplayer.h
    guiplayer.qrc
    guiplayer.ui
    iconutils.cpp
    iconutils.h
    playerabout.cpp
    playerabout.h
    playerabout.ui
    playermain.cpp
    seqplayer.cpp
    seqplayer.h
    sequence.cpp
    sequence.h
)

add_executable(drumstick-guiplayer
    ${guiplayer_SRCS}
)

target_link_libraries(drumstick-guiplayer
    Qt5::Widgets
    Drumstick::File
    Drumstick::RT
    Drumstick::Widgets
)

target_compile_definitions(drumstick-guiplayer PRIVATE
    VERSION=${CMAKE_PROJECT_VERSION}
)

install(TARGETS drumstick-guiplayer
        RUNTIME DESTINATION bin)

install(FILES drumstick-guiplayer.desktop
        DESTINATION share/applications)
