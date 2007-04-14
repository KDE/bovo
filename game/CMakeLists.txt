set(src game.cc board.cc dimension.cc coord.cc square.cc move.cc)
kde4_automoc(${src})
kde4_add_library(game ${src})
#add_subdirectory(test)
