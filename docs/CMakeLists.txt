if(BUILD_DOCS)
    if(NOT EXISTS ${PANDOC_EXECUTABLE})
        find_program(PANDOC_EXECUTABLE pandoc)
        mark_as_advanced(PANDOC_EXECUTABLE)
        if(NOT EXISTS ${PANDOC_EXECUTABLE})
            message(FATAL_ERROR "Pandoc not found. Install Pandoc or set the cache variable PANDOC_EXECUTABLE.")
        endif()
    endif()

    if(EXISTS ${PANDOC_EXECUTABLE})
        macro(add_manpage out_file)
            set(_src  ${CMAKE_CURRENT_SOURCE_DIR}/${out_file}.md)
            unset(_date)
            execute_process (
                COMMAND bash -c "LANG=C;date +'%B %d, %Y'"
                OUTPUT_VARIABLE _date
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            add_custom_command (
                OUTPUT ${out_file}
                COMMAND ${PANDOC_EXECUTABLE} -s -t man -Vdate=${_date} ${_src} -o ${out_file}
                DEPENDS ${_src}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                VERBATIM
            )
            add_custom_target(manpages ALL DEPENDS ${out_file})
        endmacro()
        
        add_manpage(dmidiplayer.1)
        
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dmidiplayer.1
                DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
    endif()
else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dmidiplayer.1
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
