cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(fscore-delegates)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# add_custom_target(${PROJECT_NAME}-content
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_ROOT_DIR}/content/
#                   ${PROJECT_OUTPUT_DIR}/content/)
# add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-content)

include_directories(${fscore_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
                      fscore)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}")

