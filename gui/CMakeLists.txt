# Graphical User Interface

set(bovogui 
    mark.cc
    game.cc
    scene.cc
    view.cc
    mainwindow.cc
    main.cc
)

#find_package(KDE4 REQUIRED)
#include(KDE4Defaults)
#include(MacroLibrary)
#
#add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} " -fexceptions ")
#
#include_directories ( ${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
#	${QT_INCLUDES} ${QT4_INCLUDES}  ${QT_INCLUDE_DIR} ${QT4_INCLUDE_DIR}
#	${QT_INCLDUDE_DIRS} ${QT4_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR}
#	${KDE4_INCLUDE_DIRS} ../ai ../luffarschack ${LUFFARSCHACK_BINARY_DIR}/gui ../gui ${CMAKE_BINARY_DIR}/gui
#)
#
#set (${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -fexceptions ")
#

kde4_automoc(${bovogui})
kde4_add_executable(bovo ${bovogui})
target_link_libraries(bovo ${KDE4_KDEUI_LIBS} game ai)

install (TARGETS bovo RUNTIME DESTINATION bin)
install (FILES bovoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bovo )

