cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(fsgame-basic_game_app)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

add_custom_target(${PROJECT_NAME}-content
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_ROOT_DIR}/content/
                  ${EXECUTABLE_OUTPUT_PATH}${PROJECT_NAME}-content/)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-content)

include_directories(${fscore_SOURCE_DIR}/include)
include_directories(${fsgame_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
                      fsgame
                      fscore
                      ${SDL2_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY})
