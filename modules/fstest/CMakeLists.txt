cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(fstest)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")
#file(GLOB_RECURSE PROJECT_SOURCE_FILES_WINDOWS 
#    "${PROJECT_SOURCE_DIR}/*_windows.cpp"
#    "${PROJECT_SOURCE_DIR}/*_windows.c")
#file(GLOB_RECURSE PROJECT_SOURCE_FILES_LINUX 
#    "${PROJECT_SOURCE_DIR}/*_linux.cpp"
#    "${PROJECT_SOURCE_DIR}/*_linux.c")
#list(REMOVE_ITEM PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES_WINDOWS})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_TEST_DYN_LINK)
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )

# Spikes
add_subdirectory(spikes)
