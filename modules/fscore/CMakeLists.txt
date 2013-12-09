cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(fscore)

set(PROJECT_TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/tests/src)
set(PROJECT_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tests/include)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

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
list(REMOVE_ITEM PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES_WINDOWS})

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_FILES})

# Unit testing
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${fstest_SOURCE_DIR}/include)
include_directories(${PROJECT_TEST_INCLUDE_DIR})
include_directories(${TINYXML_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_TEST_FILES "${PROJECT_TEST_SOURCE_DIR}/*.cpp")
#list(REMOVE_ITEM PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/core/allocators/traceallocator.cpp")

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(${PROJECT_NAME}-test ${PROJECT_TEST_FILES})
target_link_libraries(${PROJECT_NAME}-test
                      ${PROJECT_NAME}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${SDL2_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      fstest
                      )

set_target_properties(${PROJECT_NAME}-test PROPERTIES COMPILE_FLAGS "-fexceptions")

# Spikes
add_subdirectory(spikes)
