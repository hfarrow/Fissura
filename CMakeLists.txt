cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Fissura)

include(CMakeFindBinUtils)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/scripts/cmake/modules/")

set(CMAKE_C_FLAGS                 "-Wall -std=c++11")
set(CMAKE_C_FLAGS_DEBUG           "-g -D_DEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE         "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO  "-O2 -g")

set(CMAKE_CXX_FLAGS                 "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG           "-g -D_DEBUG -DSDL_ASSERT_LEVEL=2")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG -DSDL_ASSERT_LEVEL=1")
set(CMAKE_CXX_FLAGS_RELEASE         "-O4 -DNDEBUG -DSDL_ASSERT_LEVEL=1")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DSDL_ASSERT_LEVEL=2")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
add_subdirectory(modules/fstest)
add_subdirectory(modules/fscore)
add_subdirectory(modules/fsgame)
add_subdirectory(spikes)

enable_testing()
add_test(NAME FissuraCoreTest COMMAND ${EXECUTABLE_OUTPUT_PATH}/fscore-test --log_level=all)
