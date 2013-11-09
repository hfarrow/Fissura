cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Fissura)

include(CMakeFindBinUtils)

SET (CMAKE_C_FLAGS                 "-Wall -std=c++11")
SET (CMAKE_C_FLAGS_DEBUG           "-g -D_DEBUG")
SET (CMAKE_C_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE         "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO  "-O2 -g")

SET (CMAKE_CXX_FLAGS                 "-Wall -std=c++11")
SET (CMAKE_CXX_FLAGS_DEBUG           "-g -D_DEBUG")
SET (CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE         "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(modules/fstest)
add_subdirectory(modules/fscore)
add_subdirectory(spikes)

enable_testing()
add_test(NAME FissuraCoreTest COMMAND ${EXECUTABLE_OUTPUT_PATH}/fscore-test --log_level=all)
