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
    VERSION 1.0.1
    DESCRIPTION "Drumstick MIDI File Player Multiplatform Program"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package (QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package (Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets LinguistTools REQUIRED)
find_package (Drumstick 2.0 COMPONENTS File RT Widgets REQUIRED)

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

set ( TRANSLATIONS
   translations/${PROJECT_NAME}_es.ts
)

if (QT_VERSION VERSION_LESS 5.15.0)
    qt5_add_translation (QM_FILES ${TRANSLATIONS})
else()
    qt_add_translation (QM_FILES ${TRANSLATIONS})
endif()

add_custom_target (translations ALL DEPENDS ${QM_FILES})

if (WIN32)
    set(FLUIDSYNTH_PREFIX $ENV{FLUIDSYNTH})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.nsi.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nsi IMMEDIATE @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc IMMEDIATE @ONLY)
    set (RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
    set (RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.res.obj)
    find_program (WINDRES_EXECUTABLE NAMES windres mingw32-windres i686-mingw32-windres)
    if (MINGW)
    exec_program (${WINDRES_EXECUTABLE}
        ARGS "-i ${RC_FILE} -o ${RES_FILE} --include-dir=${CMAKE_CURRENT_SOURCE_DIR}/icons")
    list (APPEND SOURCES ${RES_FILE})
    else ()
    list (APPEND SOURCES ${RC_FILE})
    endif ()
endif ()

if (APPLE)
  set (ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/icons/${PROJECT_NAME}.icns)
  list (APPEND SOURCES ${ICON_FILE} ${QM_FILES})
  set (MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
  set_source_files_properties (${ICON_FILE} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties (${QM_FILES} PROPERTIES
    MACOSX_PACKAGE_LOCATION translations)
  include(TranslationUtils)
  add_qt_translations(QT_TRANSLATIONS es)
  set_source_files_properties (${QT_TRANSLATIONS} PROPERTIES
    MACOSX_PACKAGE_LOCATION translations)
  list (APPEND SOURCES ${QT_TRANSLATIONS})
endif ()

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Drumstick::File
    Drumstick::RT
    Drumstick::Widgets
)

if (WIN32)
    set_target_properties (${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif ()

if (APPLE)
    set_target_properties (${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE true
    MACOSX_BUNDLE_GUI_IDENTIFIER "net.sourceforge.${PROJECT_NAME}"
    MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
    MACOSX_BUNDLE_DISPLAY_NAME "${PROJECT_NAME}"
    MACOSX_BUNDLE_INFO_STRING "${PROJECT_DESCRIPTION}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021, Pedro López-Cabanillas &lt;plcl@users.sf.net&gt;")
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION=${PROJECT_VERSION})

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    add_subdirectory(icons)

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(FILES ${PROJECT_NAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

    install(FILES ${QM_FILES}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
else()
    include(DeploymentUtils)
endif()
