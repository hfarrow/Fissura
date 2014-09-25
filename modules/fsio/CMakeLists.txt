cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # CMake 3.0
endif()

project(fsio)

set(PROJECT_TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/tests/src)
set(PROJECT_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tests/include)
set(PROJECT_TEST_ROOT_DIR ${PROJECT_SOURCE_DIR}/tests)
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
file(GLOB_RECURSE PROJECT_SOURCE_FILES_UNIX
        "${PROJECT_SOURCE_DIR}/*_unix.cpp"
        "${PROJECT_SOURCE_DIR}/*_unix.c")

IF(DEFINED IS_UNIX)
    # list(REMOVE_ITEM PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES_WINDOWS})
ELSE()
    list(REMOVE_ITEM PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES_UNIX})
ENDIF(DEFINED IS_UNIX)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
                      ${SDL2_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      fscore)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${fscore_SOURCE_DIR}/include)
include_directories(${fstest_SOURCE_DIR}/include)
include_directories(${PROJECT_TEST_INCLUDE_DIR})
include_directories(${TINYXML_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_TEST_FILES "${PROJECT_TEST_SOURCE_DIR}/*.cpp")

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(${PROJECT_NAME}-test ${PROJECT_TEST_FILES})

add_custom_target(${PROJECT_NAME}-test-content
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_TEST_ROOT_DIR}/content/
                  ${PROJECT_OUTPUT_DIR}-test/content/)
add_dependencies(${PROJECT_NAME}-test ${PROJECT_NAME}-test-content)

target_link_libraries(${PROJECT_NAME}-test
                      ${PROJECT_NAME}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      fscore)

set_target_properties(${PROJECT_NAME}-test
    PROPERTIES
    COMPILE_FLAGS "-fexceptions"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}-test")

# Spikes
add_subdirectory(spikes)
