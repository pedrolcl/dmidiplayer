set(MANPAGE ${PROJECT_NAME}.1)
include(pandoc_manpage)
if (EXISTS ${PANDOC_EXECUTABLE})
    add_manpage(${MANPAGE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
else()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${MANPAGE}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
