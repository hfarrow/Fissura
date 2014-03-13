cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(fscore-freelist)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

include_directories(${core_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
                      fscore
                      ${Boost_LIBRARIES}
                      ${SDL2_LIBRARIES}
                      ${TINYXML_LIBRARIES})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}")
