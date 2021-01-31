# Drumstick MIDI File Player Multiplatform Program
# Copyright (C) 2006-2021 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
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

project(dmidiplayer
    VERSION 1.0.0
    DESCRIPTION "Drumstick MIDI File Player Multiplatform Program"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.12 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.0 COMPONENTS File RT Widgets REQUIRED)

SET(SOURCES
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

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Drumstick::File
    Drumstick::RT
    Drumstick::Widgets
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION=${PROJECT_VERSION})

include(GNUInstallDirs)

add_subdirectory(icons)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${PROJECT_NAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
