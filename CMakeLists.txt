project (BOVO)

# re-enabling exceptions (turned off in KDE)
add_definitions (${KDE4_ENABLE_EXCEPTIONS})


include_directories (
        ${CMAKE_SOURCE_DIR}/libkdegames
        ${BOVO_SOURCE_DIR}/gui ${CMAKE_BINARY_DIR}/gui
	${BOVO_SOURCE_DIR}/game ${BOVO_SOURCE_DIR}/ai
)
link_directories ( ${BOVO_BINARY_DIR}/game ${BOVO_BINARY_DIR}/ai)

add_subdirectory (game)
add_subdirectory (ai)
add_subdirectory (gui)
add_subdirectory (themes)

install (FILES bovo.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
