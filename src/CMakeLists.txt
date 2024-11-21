ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX BOVO
    VERSION_HEADER bovo_version.h
)

add_executable(bovo)

target_sources(bovo PRIVATE
    game/game.cc
    game/board.cc
    game/dimension.cc
    game/coord.cc
    game/square.cc
    game/move.cc

    ai/ai.cc
    ai/aifactory.cc
    ai/aron/aiaron.cc
    ai/aron/aiboard.cc
    ai/aron/aisquare.cc
    ai/gabor/aigabor.cc
    ai/gabor/standing.cpp
    ai/gabor/node.cpp
    ai/gabor/ai_interface.cpp
    ai/gabor/ai_impl.cpp

    gui/hintitem.cc
    gui/mark.cc
    gui/theme.cc
    gui/scene.cc
    gui/view.cc
    gui/mainwindow.cc
    gui/main.cc

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

target_include_directories(bovo PRIVATE
    game/
    gui/
    ai/
)

kconfig_add_kcfg_files(bovo gui/settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-bovo.png")
ecm_add_app_icon(bovo ICONS ${ICONS_SRCS})

target_link_libraries(bovo
    KDEGames6
    KF6::CoreAddons
    KF6::XmlGui
    KF6::I18n
    KF6::DBusAddons
    KF6::Crash
    Qt6::Svg
    Qt6::Concurrent
    Qt6::SvgWidgets
    KF6::IconThemes
)

install (TARGETS bovo ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install (PROGRAMS org.kde.bovo.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES org.kde.bovo.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
