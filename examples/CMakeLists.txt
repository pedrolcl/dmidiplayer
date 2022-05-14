file(GLOB KAR_FILES *.kar)
file(GLOB MIDI_FILES *.mid)

set(contents "")
foreach(fullfile ${KAR_FILES};${MIDI_FILES})
    get_filename_component(filename ${fullfile} NAME)
    set(contents "${contents}${filename}\n")
endforeach()

set(LST_FILE ${CMAKE_CURRENT_BINARY_DIR}/examples.lst)
file(WRITE ${LST_FILE} ${contents})

if (APPLE)
    set (EXAMPLES ${LST_FILE};${MIDI_FILES};${KAR_FILES} PARENT_SCOPE)
else()
    install( FILES ${MIDI_FILES} ${KAR_FILES}  ${LST_FILE}
             DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME} )
endif()
