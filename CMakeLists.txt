cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Fissura)

include(CMakeFindBinUtils)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Mac OS X specific code
    set(IS_MAC "1")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    set(IS_LINUX "1")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/scripts/cmake/modules/")

set(CMAKE_C_FLAGS                 "-Wall -std=c99")
set(CMAKE_C_FLAGS_DEBUG           "-g -D_DEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE         "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO  "-O2 -g")

IF(DEFINED IS_LINUX)
    set(CMAKE_CXX_FLAGS                 "-Wall -std=c++11")
ELSEIF(DEFINED IS_MAC)
    set(CMAKE_CXX_FLAGS                 "-Wall -std=c++11 -stdlib=libc++")
ENDIF(DEFINED IS_LINUX)
set(CMAKE_CXX_FLAGS_DEBUG           "-g -D_DEBUG -DSDL_ASSERT_LEVEL=2")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG -DSDL_ASSERT_LEVEL=1")
set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -DNDEBUG -DSDL_ASSERT_LEVEL=1")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DSDL_ASSERT_LEVEL=2")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXTERNAL_LIBRARY_INSTALL_PATH ${CMAKE_SOURCE_DIR}/external/build)

find_package(SDL2 REQUIRED)
find_package(Boost 1.54.0 COMPONENTS unit_test_framework date_time REQUIRED)
find_package(TinyXML REQUIRED)

add_subdirectory(modules/fstest)
add_subdirectory(modules/fscore)
# add_subdirectory(modules/fsgame)
add_subdirectory(spikes)

enable_testing()
add_test(NAME FissuraCoreTest COMMAND $<TARGET_FILE:fscore-test> --log_level=all)
# add_test(NAME FissuraGameTest COMMAND $<TARGET_FILE:fsgame-test> --log_level=all)
