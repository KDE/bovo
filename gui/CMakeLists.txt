# Graphical User Interface

set(bovogui
    hintitem.cc
    mark.cc
    theme.cc
    scene.cc
    view.cc
    mainwindow.cc
    main.cc
)

kde4_add_kcfg_files(bovogui settings.kcfgc)
set(bovogui_icons
    ${KDE4_ICON_DIR}/oxygen/16x16/apps/bovo.png
    ${KDE4_ICON_DIR}/oxygen/32x32/apps/bovo.png
    ${KDE4_ICON_DIR}/oxygen/48x48/apps/bovo.png
    ${KDE4_ICON_DIR}/oxygen/64x64/apps/bovo.png
    ${KDE4_ICON_DIR}/oxygen/128x128/apps/bovo.png
)
kde4_add_win32_app_icon(bovogui ${bovogui_icons})
kde4_add_executable(bovo ${bovogui})
target_link_libraries(bovo ${KDE4_KDEUI_LIBS} kdegames ${KDE4_} game ai)

install (TARGETS bovo RUNTIME DESTINATION ${BIN_INSTALL_DIR})
install (FILES bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )
