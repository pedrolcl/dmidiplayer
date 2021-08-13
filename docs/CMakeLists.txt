set(MANPAGE ${PROJECT_NAME}.1)
include(PandocMacros)
if (EXISTS ${PANDOC_EXECUTABLE})
    add_manpage(${MANPAGE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

    add_helpfile("en/index")
    add_helpfile("es/index")
    add_helpfile("cs/index")
    add_helpfile("ru/index")
else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
