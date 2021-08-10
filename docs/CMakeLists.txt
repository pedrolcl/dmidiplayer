set(MANPAGE ${PROJECT_NAME}.1)
include(pandoc_manpage)
if (EXISTS ${PANDOC_EXECUTABLE})
    add_manpage(${MANPAGE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

#    set(help_file "en/index")
#    set(_src ${CMAKE_CURRENT_SOURCE_DIR}/${help_file}.md)
#    set(_out ${CMAKE_CURRENT_SOURCE_DIR}/${help_file}.html)
#    add_custom_command(
#        OUTPUT ${_out}
#        COMMAND ${PANDOC_EXECUTABLE} -s --toc -t html5 -o ${_out} ${_src}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        DEPENDS ${_src}
#        VERBATIM)
#    add_custom_target(helpfile ALL DEPENDS ${_out})
    add_helpfile("en/index")
    add_helpfile("es/index")

else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
