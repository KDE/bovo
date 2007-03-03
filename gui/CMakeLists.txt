# Graphical User Interface

set(bovogui 
    mark.cc
    game.cc
    scene.cc
    view.cc
    mainwindow.cc
    main.cc
)

kde4_automoc(${bovogui})
kde4_add_executable(bovo ${bovogui})
target_link_libraries(bovo ${KDE4_KDEUI_LIBS} game ai)

install (TARGETS bovo RUNTIME DESTINATION bin)
install (FILES bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )

