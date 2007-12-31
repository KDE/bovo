project (BOVO)

# re-enabling exceptions (turned off in KDE)
if(NOT MSVC)
  # cmake bug - RC_FLAGS for rc.exe is wrong
  add_definitions (${KDE4_ENABLE_EXCEPTIONS})
endif(NOT MSVC)

include_directories (
        ${BOVO_SOURCE_DIR}/gui ${CMAKE_BINARY_DIR}/gui
	${BOVO_SOURCE_DIR}/game ${BOVO_SOURCE_DIR}/ai
)
link_directories ( ${BOVO_BINARY_DIR}/game ${BOVO_BINARY_DIR}/ai)

add_subdirectory (game)
add_subdirectory (ai)
add_subdirectory (gui)
add_subdirectory (themes)

install (FILES bovo.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
