cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "20")
set(RELEASE_SERVICE_VERSION_MINOR "12")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(bovo VERSION ${RELEASE_SERVICE_VERSION})

set (QT_MIN_VERSION "5.7.0")
set (KF5_MIN_VERSION "5.30.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg Concurrent)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
	    CoreAddons
	    Crash
	    DBusAddons
	    DocTools
	    XmlGui
	    )
find_package(KF5KDEGames 4.9.0 REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)

# re-enabling exceptions (turned off in KDE)
kde_enable_exceptions()

add_definitions(
    -DQT_USE_FAST_CONCATENATION
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
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

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX BOVO VERSION_HEADER bovo_version.h)

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
qt5_add_resources(bovo_SRCS bovo.qrc)
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-bovo.png")
ecm_add_app_icon(bovo_SRCS ICONS ${ICONS_SRCS})
add_executable(bovo ${bovo_SRCS})

target_link_libraries(bovo
	KF5::XmlGui
	KF5::I18n
	Qt5::Svg
	KF5KDEGames
	KF5::DBusAddons
	KF5::Crash
)

add_subdirectory (themes)
add_subdirectory (icons)
add_subdirectory (doc)

install (TARGETS bovo ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install (PROGRAMS org.kde.bovo.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES org.kde.bovo.appdata.xml DESTINATION  ${CMAKE_INSTALL_METAINFODIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
