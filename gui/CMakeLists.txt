# Graphical User Interface

set(bovogui
    hintitem.cc
    mark.cc
    scene.cc
    view.cc
    mainwindow.cc
    main.cc
)

kde4_automoc(${bovogui})
kde4_add_executable(bovo ${bovogui})
target_link_libraries(bovo ${KDE4_KDEUI_LIBS} ${KDE4_} game ai)

install (TARGETS bovo RUNTIME DESTINATION ${BIN_INSTALL_DIR})
install (FILES bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )
