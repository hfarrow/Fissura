cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(fscore-logger)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")

message(${PROJECT_ROOT_DIR})
message(${EXECUTABLE_OUTPUT_PATH})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

add_custom_target(${PROJECT_NAME}-content
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_ROOT_DIR}/content/
                  ${EXECUTABLE_OUTPUT_PATH}${PROJECT_NAME}-content/)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-content)

include_directories(${core_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
                      fscore
                      ${Boost_LIBRARIES}
                      ${SDL2_LIBRARIES}
                      ${TINYXML_LIBRARIES})

