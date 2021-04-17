file(GLOB MIDI_FILES *.mid)
file(GLOB KAR_FILES *.kar)
install( FILES ${MIDI_FILES} ${KAR_FILES}
         DESTINATION ${DATA_INSTALL_DIR}/${PROJECT_NAME} )

set(contents "")
foreach(fullfile ${MIDI_FILES};${KAR_FILES})
    get_filename_component(filename ${fullfile} NAME)
    set(contents "${contents}${filename}\n")
endforeach()

set(LST_FILE ${CMAKE_CURRENT_BINARY_DIR}/examples.lst)
file(WRITE ${LST_FILE} ${contents})
install( FILES ${LST_FILE}
         DESTINATION ${DATA_INSTALL_DIR}/${PROJECT_NAME} )
