set(MANPAGE ${PROJECT_NAME}.1)
include(pandoc_manpage)
if (EXISTS ${PANDOC_EXECUTABLE})
    add_manpage(${MANPAGE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

    add_custom_command(OUTPUT en/index.html
        COMMAND ${PANDOC_EXECUTABLE} -s -t html5 -Mtitle="Help Index" -Vtitle="" -o en/index.html en/index.md
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/en/index.md
        VERBATIM)

else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
