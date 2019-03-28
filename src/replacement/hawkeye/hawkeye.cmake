# Getting the MPPPB-related source files.
file(GLOB_RECURSE CHAMPSIM_REPLACEMENT_POLICY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/replacement/hawkeye/*.cc)

list(APPEND CHAMPSIM_REPLACEMENT_POLICY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/replacement/base_replacement.cc)
