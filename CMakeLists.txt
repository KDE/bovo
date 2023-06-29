cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "23")
set(RELEASE_SERVICE_VERSION_MINOR "04")
set(RELEASE_SERVICE_VERSION_MICRO "3")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(bovo VERSION ${RELEASE_SERVICE_VERSION})

set (QT_MIN_VERSION "5.15.0")
set (KF_MIN_VERSION "5.92.0")
find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )
include(KDEInstallDirs)



include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg Concurrent)
if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6SvgWidgets)
endif()
find_package(Qt${QT_MAJOR_VERSION}QuickWidgets ${REQUIRED_QT_VERSION} CONFIG)

find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
            CoreAddons
            Crash
            DBusAddons
            DocTools
            XmlGui
            )
find_package(KF5KDEGames 7.3.0 REQUIRED)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

# re-enabling exceptions (turned off in KDE)
kde_enable_exceptions()
ecm_set_disabled_deprecation_versions(
    QT 6.4
    KF 5.103
    KDEGAMES 7.3
)

# Force c++11 as random_shuffle is removed in freebsd c++17
set(CMAKE_CXX_STANDARD 11)

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
        gui/mainwindow.h
        gui/mark.h
        gui/hintitem.h
        gui/view.h
        gui/theme.h
        gui/scene.h
        ai/gabor/ai_impl.h
        ai/gabor/ai_interface.h
        ai/gabor/aigabor.h
        ai/gabor/node.h
        ai/gabor/standing.h
        ai/ai.h
        ai/aifactory.h
        ai/aron/aiboard.h
        ai/aron/aiaron.h
        ai/aron/aisquare.h
        game/board.h
        game/coord.h
        game/game.h
        game/square.h
        game/dimension.h
        game/common.h
        game/move.h
	bovo.qrc
)

include_directories(
	${CMAKE_SOURCE_DIR}/game
	${CMAKE_SOURCE_DIR}/gui
	${CMAKE_SOURCE_DIR}/ai
	)

kconfig_add_kcfg_files(bovo_SRCS gui/settings.kcfgc)
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-bovo.png")
ecm_add_app_icon(bovo_SRCS ICONS ${ICONS_SRCS})
add_executable(bovo ${bovo_SRCS})

target_link_libraries(bovo
	KF${KF_MAJOR_VERSION}::CoreAddons
	KF${KF_MAJOR_VERSION}::XmlGui
	KF${KF_MAJOR_VERSION}::I18n
	KF5KDEGames
	KF${KF_MAJOR_VERSION}::DBusAddons
	KF${KF_MAJOR_VERSION}::Crash
        Qt${QT_MAJOR_VERSION}::Svg
        Qt${QT_MAJOR_VERSION}::Concurrent
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(bovo Qt6::SvgWidgets)
endif()

add_subdirectory (themes)
add_subdirectory (icons)
add_subdirectory (doc)

ki18n_install(po)
kdoctools_install(po)

install (TARGETS bovo ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install (PROGRAMS org.kde.bovo.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES org.kde.bovo.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

