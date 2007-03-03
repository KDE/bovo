add_executable (testbovo test.cpp)
target_link_library (testbovo game)
install (TARGETS testbovo DESTINATION )
