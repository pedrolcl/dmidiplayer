# Drumstick MIDI File Player Multiplatform Program
# Copyright (C) 2006-2022 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
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
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")

project ( dmidiplayer
    VERSION 1.6.0
    LANGUAGES CXX
    DESCRIPTION "Drumstick MIDI File Player Multiplatform Program"
    HOMEPAGE_URL "https://sourceforge.net/p/dmidiplayer/"
)

set(PROJECT_RELEASE_DATE "May 17, 2022")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_DOCS "Process Markdown sources of man pages and help files" ON)
option(EMBED_TRANSLATIONS "Embed translations instead of installing" OFF)
option(USE_QT "Choose which Qt major version (5 or 6) to prefer. By default uses whatever is found")

include(SCMRevision)

if (USE_QT)
    if (NOT (USE_QT EQUAL 5 OR USE_QT EQUAL 6))
        message(FATAL_ERROR "Wrong Qt major version. Only 5 and 6 are valid options")
    endif()
    find_package(QT NAMES Qt${USE_QT})
else()
    find_package(QT NAMES Qt5 Qt6)
endif()

find_package (Qt${QT_VERSION_MAJOR} 5.15 COMPONENTS Gui Widgets PrintSupport LinguistTools REQUIRED)
if(QT_VERSION VERSION_GREATER_EQUAL 6.0.0)
    find_package (Qt6Core5Compat REQUIRED)
endif()

find_package (Drumstick 2.6 COMPONENTS File RT Widgets REQUIRED)

find_package ( PkgConfig )
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
    helpwindow.cpp
    helpwindow.h
    docs/help.qrc
    framelesswindow.cpp
    framelesswindow.h
    iconutils.cpp
    iconutils.h
    instrumentset.cpp
    instrumentset.h
    loopdialog.cpp
    loopdialog.h
    loopdialog.ui
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
   translations/${PROJECT_NAME}_de.ts
   translations/${PROJECT_NAME}_es.ts
   translations/${PROJECT_NAME}_fr.ts
   translations/${PROJECT_NAME}_gl.ts
   translations/${PROJECT_NAME}_it.ts
   translations/${PROJECT_NAME}_ru.ts
)

if (QT_VERSION VERSION_LESS 5.15.0)
    qt5_add_translation (QM_FILES ${TRANSLATIONS})
else()
    qt_add_translation (QM_FILES ${TRANSLATIONS})
endif()
add_custom_target (translations ALL DEPENDS ${QM_FILES})
include(TranslationUtils)

get_target_property(DRUMSTICK_LOCATION Drumstick::Widgets LOCATION)
get_filename_component(DRUMSTICK_PREFIX ${DRUMSTICK_LOCATION} DIRECTORY)
get_filename_component(DRUMSTICK_PREFIX ${DRUMSTICK_PREFIX} DIRECTORY)

if (EXISTS ${DRUMSTICK_PREFIX}/share/drumstick/)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/share/drumstick/drumstick-widgets_*.qm")
elseif (EXISTS ${DRUMSTICK_PREFIX}/library/widgets/)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/library/widgets/drumstick-widgets_*.qm")
endif()

if (EMBED_TRANSLATIONS)
    file(COPY ${WIDGETS_TRANS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    add_app_translations_resource(APP_RES ${QM_FILES} ${WIDGETS_TRANS})
    add_qt_translations_resource(QT_RES cs de es fr gl it ru)
    list(APPEND SOURCES ${APP_RES} ${QT_RES})
else()
    if (APPLE)
        set_source_files_properties (${WIDGETS_TRANS} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources )
        list (APPEND SOURCES ${WIDGETS_TRANS})
    endif()
endif()

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

include(GNUInstallDirs)
add_subdirectory(docs)
add_subdirectory(examples)

if (APPLE)
    set_source_files_properties (${EXAMPLES} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources )
    list (APPEND SOURCES ${EXAMPLES})
    set (ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/icons/${PROJECT_NAME}.icns)
    list (APPEND SOURCES ${ICON_FILE} ${QM_FILES})
    set (MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
    set_source_files_properties (${ICON_FILE} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    if (NOT EMBED_TRANSLATIONS)
        set_source_files_properties (${QM_FILES} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources)
        add_qt_translations(QT_TRANSLATIONS cs de es fr it ru)
        set_source_files_properties (${QT_TRANSLATIONS} PROPERTIES
            MACOSX_PACKAGE_LOCATION translations)
        list (APPEND SOURCES ${QT_TRANSLATIONS})
    endif ()
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
if (BUILD_DOCS)
    add_dependencies ( ${PROJECT_NAME} update_helpfiles )
endif()

target_link_libraries ( ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Drumstick::File
    Drumstick::RT
    Drumstick::Widgets
    PkgConfig::UCHARDET
)

if (QT_VERSION VERSION_GREATER_EQUAL 6.0.0)
    target_link_libraries( ${PROJECT_NAME} PRIVATE
        Qt6::Core5Compat
    )
endif()

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
    VERSION=${PROJECT_VERSION}
    $<$<BOOL:${EMBED_TRANSLATIONS}>:TRANSLATIONS_EMBEDDED>
)
if (DEFINED PROJECT_WC_REVISION)
    target_compile_definitions( ${PROJECT_NAME} PRIVATE
        REVISION=${PROJECT_WC_REVISION} )
endif()

if(UNIX AND NOT APPLE)
    add_subdirectory(icons)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ${QM_FILES}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
    install(FILES "net.sourceforge.dmidiplayer.desktop"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES "net.sourceforge.dmidiplayer.metainfo.xml"
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

    find_package(Git)
    if (Git_FOUND)
        find_program(GZIP gzip)
        find_program(BZIP2 bzip2)
        if ((EXISTS ${GZIP}) AND (EXISTS ${BZIP2}))
            set(ARCHNAME ${PROJECT_NAME}-${PROJECT_VERSION})
            add_custom_target ( tarballs
                COMMAND ${GIT_EXECUTABLE} archive --format=zip --prefix=${ARCHNAME}/ --output ${CMAKE_BINARY_DIR}/${ARCHNAME}.zip master
                COMMAND ${GIT_EXECUTABLE} archive --format=tar --prefix=${ARCHNAME}/ master | ${GZIP} > ${CMAKE_BINARY_DIR}/${ARCHNAME}.tar.gz
                COMMAND ${GIT_EXECUTABLE} archive --format=tar --prefix=${ARCHNAME}/ master | ${BZIP2} > ${CMAKE_BINARY_DIR}/${ARCHNAME}.tar.bz2
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Creating tarballs..."
                VERBATIM )
        else()
            message(WARNING "Missing utilities for tarball creation")
        endif()
    endif()

else()
    include(DeploymentUtils)
endif()
