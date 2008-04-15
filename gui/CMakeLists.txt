# Graphical User Interface

set(bovogui_SRCS
    hintitem.cc
    mark.cc
    theme.cc
    scene.cc
    view.cc
    mainwindow.cc
    main.cc
)

kde4_add_kcfg_files(bovogui_SRCS settings.kcfgc)

kde4_add_app_icon(bovogui_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/bovo.png")
kde4_add_executable(bovo ${bovogui_SRCS})

target_link_libraries(bovo ${KDE4_KDEUI_LIBS} kdegames ${KDE4_} game ai)

install (TARGETS bovo ${INSTALL_TARGETS_DEFAULT_ARGS})
install (FILES bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )
