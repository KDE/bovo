set(src gamegame.cc board.cc dimension.cc coord.cc square.cc move.cc)
kde4_automoc(gamegame.cc)
kde4_add_library(game ${src})
#add_subdirectory(test)
