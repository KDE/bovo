project (BOVO)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")

find_package(ECM 0.0.9 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg Concurrent Qml Quick QuickWidgets)
find_package(KF5 REQUIRED COMPONENTS 
	    CoreAddons
	    XmlGui
            NewStuff
            Declarative
	    KDELibs4Support
	    )
find_package(KDEGames 4.9.0 REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(KDECoreMacros)
include(ECMInstallIcons)

# re-enabling exceptions (turned off in KDE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

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

include_directories(
	${CMAKE_SOURCE_DIR}/game
	${CMAKE_SOURCE_DIR}/gui
	${CMAKE_SOURCE_DIR}/ai
	${Qt5Concurrent_INCLUDE_DIRS}
	)

kconfig_add_kcfg_files(bovo_SRCS gui/settings.kcfgc)
kde4_add_app_icon(bovo_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-app-bovo.png")
add_executable(bovo ${bovo_SRCS})

target_link_libraries(bovo Qt5::Svg KDEGames)

add_subdirectory (themes)
add_subdirectory (icons)
add_subdirectory (doc)

install (TARGETS bovo ${INSTALL_TARGETS_DEFAULT_ARGS})
install (FILES gui/bovoui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/bovo)
install (PROGRAMS bovo.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install (FILES bovo.appdata.xml DESTINATION  ${DATA_INSTALL_DIR}/appdata)
