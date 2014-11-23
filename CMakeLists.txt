cmake_minimum_required(VERSION 2.8.4)
project(cpp_raft)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I./ -g -I../ -I/usr/include/ ")

set(SOURCE_FILES
    tests/main_test.cpp
    tests/mock_send_functions.cpp
    tests/mock_send_functions.h
    tests/test_log.cpp
    tests/test_node.cpp
    tests/test_scenario.cpp
    tests/test_server.cpp
    raft.h
    raft_logger.cpp
    raft_logger.h
    raft_msg.h
    raft_node.cpp
    raft_node.h
    raft_private.h
    raft_server.cpp
    raft_server.h
    raft_server_properties.cpp
    state_mach.cpp
    state_mach.h)

find_library(boost /usr/local/)
include_directories(".")
include_directories(/usr/include/gtest)
include_directories("./gtest/")
include_directories(SYSTEM)
link_directories(/usr/local/lib)
link_directories(/usr/lib)
link_directories(SYSTEM)
set(CMAKE_VERBOSE_MAKEFILE on)
add_executable(cpp_raft ${SOURCE_FILES})
target_link_libraries(cpp_raft gtest pthread)
