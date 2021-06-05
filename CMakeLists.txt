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

cmake_minimum_required(VERSION 3.14)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project ( dmidiplayer
    VERSION 1.3.1
    LANGUAGES CXX
    DESCRIPTION "Drumstick MIDI File Player Multiplatform Program"
    HOMEPAGE_URL "https://sourceforge.net/p/dmidiplayer/"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_DOCS "Build man pages" ON)

include(SCMRevision)

find_package (QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package (Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets LinguistTools REQUIRED)
find_package (Drumstick 2.2 COMPONENTS File RT Widgets REQUIRED)

find_package( PkgConfig )
pkg_check_modules( UCHARDET REQUIRED IMPORTED_TARGET uchardet )

SET(SOURCES
    channels.cpp
    channels.h
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
    instrumentset.cpp
    instrumentset.h
    lyrics.cpp
    lyrics.h
    pianola.cpp
    pianola.h
    playlist.cpp
    playlist.h
    playlist.ui
    playerabout.cpp
    playerabout.h
    playerabout.ui
    playermain.cpp
    prefsdialog.cpp
    prefsdialog.h
    prefsdialog.ui
    recentfileshelper.cpp
    recentfileshelper.h
    rhythmview.cpp
    rhythmview.h
    seqplayer.cpp
    seqplayer.h
    sequence.cpp
    sequence.h
    settings.cpp
    settings.h
    toolbareditdialog.cpp
    toolbareditdialog.h
    toolbareditdialog.ui
    vumeter.cpp
    vumeter.h
)

if (WIN32)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <Windows.h>
        int main() { EnumWindows([](HWND, LPARAM) -> BOOL { return true; }, 0); return 0; }"
        COMPILER_SUPPORTED)
    if (NOT COMPILER_SUPPORTED)
        message(FATAL_ERROR "Sorry, your compiler is not supported: ${CMAKE_CXX_COMPILER}")
    endif()
    list(APPEND SOURCES winsnap.cpp winsnap.h)
endif()

set ( TRANSLATIONS
   translations/${PROJECT_NAME}_cs.ts
   translations/${PROJECT_NAME}_es.ts
   translations/${PROJECT_NAME}_ru.ts
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
        MACOSX_PACKAGE_LOCATION Resources)
    include(TranslationUtils)
    add_qt_translations(QT_TRANSLATIONS es ru)
    set_source_files_properties (${QT_TRANSLATIONS} PROPERTIES
        MACOSX_PACKAGE_LOCATION translations)
    list (APPEND SOURCES ${QT_TRANSLATIONS})
endif ()

message (STATUS "Drumstick MIDI File Player v${PROJECT_VERSION}
     install prefix: ${CMAKE_INSTALL_PREFIX}
     Build configuration: ${CMAKE_BUILD_TYPE}
     Operating System: ${CMAKE_SYSTEM_NAME}
     Processor: ${CMAKE_SYSTEM_PROCESSOR}
     Qt Version: ${QT_VERSION}
     Drumstick Version: ${Drumstick_VERSION}
     Uchardet Version: ${UCHARDET_VERSION}"
)

add_executable ( ${PROJECT_NAME} ${SOURCES} )

target_link_libraries ( ${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Drumstick::File
    Drumstick::RT
    Drumstick::Widgets
    PkgConfig::UCHARDET
)

if (WIN32)
    set_target_properties ( ${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true )
    target_link_libraries ( ${PROJECT_NAME} PRIVATE winmm dwmapi )
endif ()

if (APPLE)
    set_target_properties ( ${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE true
    MACOSX_BUNDLE_GUI_IDENTIFIER "net.sourceforge.${PROJECT_NAME}"
    MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
    MACOSX_BUNDLE_DISPLAY_NAME "${PROJECT_NAME}"
    MACOSX_BUNDLE_INFO_STRING "${PROJECT_DESCRIPTION}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021, Pedro López-Cabanillas &lt;plcl@users.sf.net&gt;")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation -framework Cocoa")
endif ()

target_compile_definitions( ${PROJECT_NAME} PRIVATE
    VERSION=${PROJECT_VERSION} )
if (DEFINED PROJECT_WC_REVISION)
    target_compile_definitions( ${PROJECT_NAME} PRIVATE
        REVISION=${PROJECT_WC_REVISION} )
endif()

include(GNUInstallDirs)
add_subdirectory(examples)

if(UNIX AND NOT APPLE)
    add_subdirectory(docs)
    add_subdirectory(icons)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ${QM_FILES}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
    install(FILES "net.sourceforge.dmidiplayer.desktop"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES "net.sourceforge.dmidiplayer.appdata.xml"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)

    # uninstall target
    if(NOT TARGET uninstall)
      configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
      add_custom_target ( uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    endif()
else()
    include(DeploymentUtils)
endif()
