project (BOVO)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

KDE4_NO_ENABLE_FINAL(bovo)

# re-enabling exceptions (turned off in KDE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

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
	ai/aifactory.cc 
	ai/aron/aiaron.cc 
	ai/aron/aiboard.cc 
	ai/aron/aisquare.cc 
	ai/gabor/aigabor.cc 
	ai/gabor/standing.cpp 
	ai/gabor/node.cpp 
	ai/gabor/ai_interface.cpp 
	ai/gabor/ai_impl.cpp )

set(bovogui_SRCS
	gui/hintitem.cc
	gui/mark.cc
	gui/theme.cc
	gui/scene.cc
	gui/view.cc
	gui/mainwindow.cc
	gui/main.cc
)

set(bovo_SRCS
	${bovogui_SRCS}
	${bovoai_SRCS}
	${bovogame_SRCS}
)

kde4_add_kcfg_files(bovo_SRCS gui/settings.kcfgc)
kde4_add_app_icon(bovo_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-app-bovo.png") 
kde4_add_executable(bovo ${bovo_SRCS})

target_link_libraries(bovo kdegames ${KDE4_KDEUI_LIBS} )

add_subdirectory (themes)
add_subdirectory (icons)

install (TARGETS bovo ${INSTALL_TARGETS_DEFAULT_ARGS})
install (FILES gui/bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )
install (PROGRAMS bovo.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
