#[==========================================================================[
Drumstick MIDI File Player Multiplatform Program
Copyright (C) 2006-2024 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>

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
#]==========================================================================]

cmake_minimum_required(VERSION 3.16)

project ( dmidiplayer
    VERSION 1.7.6
    LANGUAGES CXX
    DESCRIPTION "Drumstick MIDI File Player Multiplatform Program"
    HOMEPAGE_URL "https://dmidiplayer.sourceforge.io"
)

#set(PROJECT_RELEASE_DATE "December 30, 2024")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(SCMRevision)

option(BUILD_DOCS "Process Markdown sources of man pages and help files" ON)
option(EMBED_TRANSLATIONS "Embed translations instead of installing" OFF)
option(USE_QT5 "Choose to build with Qt5 instead of Qt6" OFF)
option(INSTALL_DEPENDENCIES "Deploy dependency libraries" OFF)

# CMAKE_SYSTEM_PROCESSOR is broken on Windows with MSVC
# cmake bug https://gitlab.kitware.com/cmake/cmake/-/issues/15170 still open in 2024/12
if(WIN32 AND (CMAKE_VERSION VERSION_GREATER_EQUAL 4.1))
      string(TOLOWER ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} CMAKE_SYSTEM_PROCESSOR)
endif()

if (WIN32)
    # "flat" installation on Windows
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
    set(CMAKE_INSTALL_PLUGINS ".")
else()
    include(GNUInstallDirs)
    set(CMAKE_INSTALL_PLUGINS "plugins")
endif()

if (USE_QT5)
    find_package( QT NAMES Qt5 REQUIRED)
    find_package ( Qt5 5.15 COMPONENTS Gui Widgets PrintSupport LinguistTools REQUIRED )
else()
    find_package( QT NAMES Qt6 REQUIRED)
    find_package ( Qt6 6.2 COMPONENTS Gui Widgets PrintSupport LinguistTools Core5Compat REQUIRED )
endif()

find_package ( Drumstick 2.11 COMPONENTS File RT Widgets REQUIRED )
find_package ( uchardet 0.0.8 COMPONENTS libuchardet REQUIRED )

message (STATUS "Drumstick MIDI File Player v${PROJECT_VERSION}
     install prefix: ${CMAKE_INSTALL_PREFIX}
     Build configuration: ${CMAKE_BUILD_TYPE}
     Operating System: ${CMAKE_SYSTEM_NAME}
     Processor: ${CMAKE_SYSTEM_PROCESSOR}
     Compiler ID: ${CMAKE_CXX_COMPILER_ID}
     Qt Version: ${QT_VERSION}
     Drumstick Version: ${Drumstick_VERSION}
     Uchardet Version: ${uchardet_VERSION}
     CMake Version: ${CMAKE_VERSION}"
)

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
   translations/${PROJECT_NAME}_ka.ts
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

get_target_property(UCHARDET_LOCATION uchardet::libuchardet LOCATION)
get_filename_component(UCHARDET_PREFIX ${UCHARDET_LOCATION} DIRECTORY)
get_filename_component(UCHARDET_PREFIX ${UCHARDET_PREFIX} DIRECTORY)

if (EXISTS ${DRUMSTICK_PREFIX}/share/drumstick/)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/share/drumstick/drumstick-widgets_*.qm")
elseif (EXISTS ${DRUMSTICK_PREFIX}/library/widgets/)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/library/widgets/drumstick-widgets_*.qm")
endif()

if (EMBED_TRANSLATIONS)
    file(COPY ${WIDGETS_TRANS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    add_app_translations_resource(APP_RES ${QM_FILES} ${WIDGETS_TRANS})
    add_qt_translations_resource(QT_RES cs de es fr gl it ka ru)
    list(APPEND SOURCES ${APP_RES} ${QT_RES})
else()
    if (APPLE)
        set_source_files_properties (${WIDGETS_TRANS} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources )
        list (APPEND SOURCES ${WIDGETS_TRANS})
    endif()
endif()

if (WIN32)
    include (InstallRequiredSystemLibraries)
    install (FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
             DESTINATION .)
    set(FLUIDSYNTH_PREFIX $ENV{FLUIDSYNTH})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_msvc.nsi.in
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
        add_qt_translations(QT_TRANSLATIONS cs de es fr gl it ru)
        set_source_files_properties (${QT_TRANSLATIONS} PROPERTIES
            MACOSX_PACKAGE_LOCATION translations)
        list (APPEND SOURCES ${QT_TRANSLATIONS})
    endif ()
endif ()

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
    uchardet::libuchardet
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
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021-2024, Pedro López-Cabanillas &lt;plcl@users.sf.net&gt;")
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
endif()

if (INSTALL_DEPENDENCIES)
    if (CMAKE_SYSTEM MATCHES "Linux")
        find_package(sonivox 4.0 CONFIG REQUIRED shared)
        if (sonivox_FOUND)
            install(IMPORTED_RUNTIME_ARTIFACTS sonivox::sonivox
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else()
            message(WARNING "Sonivox package not found")
        endif()
        find_package(Drumstick 2.11 CONFIG REQUIRED ALSA)
        if (Drumstick_FOUND)
            install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::ALSA
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else()
            message(WARNING "Drumstick::ALSA package not found")
        endif()
    endif()
    find_package(FluidSynth CONFIG REQUIRED shared)
    if (FluidSynth_FOUND)
        install(IMPORTED_RUNTIME_ARTIFACTS FluidSynth::libfluidsynth
                RUNTIME_DEPENDENCY_SET fluidsynth-dependencies
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        if(WIN32)
          set(_pre_exclude api-ms ext-ms wpaxholder hvsi azureattest pdmutilities
                           vcruntime msvcp concrt vccorlib ucrtbase )
          set(_post_exclude [[.*(\\|/)system32(\\|/).*\.dll]])
        elseif(APPLE)
          set(_pre_exclude "/usr/lib" "/System/")
          set(_post_exclude "")
        else()
          set(_pre_exclude "")
          set(_post_exclude ".*/ld-linux-.*" ".*/linux-vdso.*" ".*/libm\\..*" ".*/libc\\..*" ".*/libpthread\\..*" ".*/libdl\\..*" ".*/libstdc\\+\\+\\..*" ".*/libgcc_s.*" ".*/libgomp\\..*" ".*/libQt6.*")
        endif()
        install(RUNTIME_DEPENDENCY_SET fluidsynth-dependencies
                PRE_EXCLUDE_REGEXES ${_pre_exclude}
                POST_EXCLUDE_REGEXES ${_post_exclude}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    else()
        message(WARNING "Fluidsynth package not found")
    endif()

    install(IMPORTED_RUNTIME_ARTIFACTS uchardet::libuchardet
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    if (DEFINED Drumstick_DIR)
        if (Drumstick_DIR MATCHES "cmake")
            set(_drumstick_plugins ${Drumstick_DIR}/../../drumstick2)
        else()
            set(_drumstick_plugins ${Drumstick_DIR}/lib/drumstick2)
        endif()
        if (EXISTS ${_drumstick_plugins})
            install(DIRECTORY ${_drumstick_plugins}
                    DESTINATION ${CMAKE_INSTALL_PLUGINS})
        else()
            message(WARNING "${_drumstick_plugins} does not exists")
        endif()
    else()
        message(WARNING "Drumstick_DIR is not defined")
    endif()

    if (WIN32)
        install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::File
                LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::RT
                LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::Widgets
                LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR})
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    else()
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()

    if (QT_VERSION VERSION_GREATER_EQUAL 6.3)

        set(_deploy_tool_options_arg "")
        if (WIN32)
            set(_deploy_tool_options_arg "
                --plugindir \".\"
                --translationdir \".\"
            ")
        endif()

        if(MSVC)
            qt_generate_deploy_app_script(
                TARGET ${PROJECT_NAME}
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_COMPILER_RUNTIME
                NO_TRANSLATIONS
                DEPLOY_TOOL_OPTIONS ${_deploy_tool_options_arg}
            )
            include ( InstallRequiredSystemLibraries )
        elseif(WIN32 AND NOT MSVC)
            qt_generate_deploy_app_script(
                TARGET ${PROJECT_NAME}
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_TRANSLATIONS
            )
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            qt_generate_deploy_app_script(
                TARGET ${PROJECT_NAME}
                OUTPUT_SCRIPT deploy_script
                INCLUDE_PLUGINS xcb wayland
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_TRANSLATIONS
            )
        endif()
        if (DEFINED deploy_script)
            install(SCRIPT ${deploy_script})
        endif()
        # AppImage
        if (UNIX AND NOT APPLE)
            install( CODE
              "include(${CMAKE_SOURCE_DIR}/cmake/CreateAppImage.cmake)
               create_appimage(
                    EXE \"${PROJECT_NAME}\"
                    ICON \"${PROJECT_NAME}.png\"
                    DESKTOP \"net.sourceforge.${PROJECT_NAME}.desktop\"
                    WORKDIR \"${CMAKE_CURRENT_BINARY_DIR}\"
                    APPDIR \"${PROJECT_NAME}-v${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppDir\"
                    ARCH \"${CMAKE_SYSTEM_PROCESSOR}\"
               )"
            )
        endif()
    endif()
endif()

if (DEFINED ENV{GITHUB_OUTPUT})
    file(APPEND
        "$ENV{GITHUB_OUTPUT}"
        "config_version=${PROJECT_VERSION}\n"
    )
endif()

# CPack support
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Drumstick MIDI File Player")
set (CPACK_PACKAGE_VENDOR "${PROJECT_NAME}.sourceforge.net")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set (CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}-${PROJECT_VERSION}")
set (CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "Drumstick MIDI File Player")
set (CPACK_PACKAGE_VERSION "v${PROJECT_VERSION}")
set (CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set (CPACK_STRIP_FILES ON)

set (CPACK_SOURCE_IGNORE_FILES "/.svn/;/build/;/share/;~$;.cproject;.project;.user;${CPACK_SOURCE_IGNORE_FILES}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set (CPACK_SOURCE_STRIP_FILES OFF)

# Windows NSIS setup package
if (WIN32)
    set (CPACK_NSIS_DISPLAY_NAME "Drumstick MIDI File Player")
    set (CPACK_NSIS_WELCOME_TITLE_3LINES ON)
    set (CPACK_NSIS_FINISH_TITLE_3LINES ON)
    set (CPACK_NSIS_HELP_LINK "https://${PROJECT_NAME}.sourceforge.io")
    set (CPACK_NSIS_URL_INFO_ABOUT "https://${PROJECT_NAME}.sourceforge.io")
    set (CPACK_NSIS_CONTACT "plcl@users.sourceforge.net")
    set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${CMAKE_INSTALL_BINDIR})
    set (CPACK_NSIS_MODIFY_PATH OFF)
endif()

include(CPack)
