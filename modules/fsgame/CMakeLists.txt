cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(fsgame)

set(PROJECT_TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/tests/src)
set(PROJECT_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tests/include)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE PROJECT_SOURCE_FILES_WINDOWS
    "${PROJECT_SOURCE_DIR}/*_windows.cpp"
    "${PROJECT_SOURCE_DIR}/*_windows.c")
file(GLOB_RECURSE PROJECT_SOURCE_FILES_LINUX
    "${PROJECT_SOURCE_DIR}/*_linux.cpp"
    "${PROJECT_SOURCE_DIR}/*_linux.c")

#Uncomment below line if platform abstractions are added to project.
#list(REMOVE_ITEM PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES_WINDOWS})

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_FILES})

# Unit testing
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${fscore_SOURCE_DIR}/include)
include_directories(${fstest_SOURCE_DIR}/include)
include_directories(${PROJECT_TEST_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_TEST_FILES "${PROJECT_TEST_SOURCE_DIR}/*.cpp")

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(${PROJECT_NAME}-test ${PROJECT_TEST_FILES})
target_link_libraries(${PROJECT_NAME}-test
                      ${PROJECT_NAME}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      fscore
                      fstest
                      )

set_target_properties(${PROJECT_NAME}-test
                      PROPERTIES
                      COMPILE_FLAGS "-fexceptions -DFS_CUSTOM_GLOBAL_NEW_DELETE"
                      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}-test")

# Spikes
add_subdirectory(spikes)
