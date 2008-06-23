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

set( bovogame_SRCS
	game/game.cc 
	game/board.cc 
	game/dimension.cc 
	game/coord.cc 
	game/square.cc 
	game/move.cc )

set( bovoai_SRCS 
	ai/ai.cc 
	ai/aiboard.cc 
	ai/aisquare.cc )

set(bovogui_SRCS
    gui/hintitem.cc
    gui/mark.cc
    gui/theme.cc
    gui/scene.cc
    gui/view.cc
    gui/mainwindow.cc
    gui/main.cc
)

kde4_add_kcfg_files(bovogui_SRCS gui/settings.kcfgc)
kde4_add_app_icon(bovogui_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/bovo.png")
kde4_add_executable(bovo ${bovogui_SRCS} ${bovoai_SRCS} ${bovogame_SRCS})

target_link_libraries(bovo kdegames ${KDE4_KDEUI_LIBS} )

add_subdirectory (themes)

install (TARGETS bovo ${INSTALL_TARGETS_DEFAULT_ARGS})
install (FILES gui/bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )
install (FILES bovo.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
