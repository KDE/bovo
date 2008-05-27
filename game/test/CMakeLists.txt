kde4_add_executable (testbovo test.cpp)
target_link_library (testbovo game)
install (TARGETS testbovo ${INSTALL_TARGETS_DEFAULT_ARGS})
