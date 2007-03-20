project (BOVO)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

# Not sure how to do this, but re-enabling exceptions (turned off in KDE)
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} " -fexceptions ")
set (${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -fexceptions ")


include_directories ( ${QDBUS_INCLUDE_DIRS}
        ${KDE4_INCLUDES}
        ${BOVO_SOURCE_DIR}/gui ${CMAKE_BINARY_DIR}/gui
	${BOVO_SOURCE_DIR}/game ${BOVO_SOURCE_DIR}/ai
)
link_directories ( ${BOVO_BINARY_DIR}/game ${BOVO_BINARY_DIR}/ai)

add_subdirectory (game)
add_subdirectory (ai)
add_subdirectory (gui)
add_subdirectory (themes)

install (FILES bovo.desktop DESTINATION ${XDG_APPS_DIR})
