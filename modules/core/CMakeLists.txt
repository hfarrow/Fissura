cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(core)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/tests)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.c")
list(REMOVE_ITEM PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/core/allocators/heap_allocator.cpp")

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_FILES})

# Unit testing
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/*.cpp")
list(REMOVE_ITEM PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/core/allocators/heapallocator.cpp")
list(REMOVE_ITEM PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/core/allocators/traceallocator.cpp")
list(REMOVE_ITEM PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/core/allocators/pageallocator.cpp")
list(REMOVE_ITEM PROJECT_TEST_FILES "${PROJECT_TEST_DIR}/core/allocators/allocator.cpp")

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(${PROJECT_NAME}Test ${PROJECT_TEST_FILES})
target_link_libraries(${PROJECT_NAME}Test
                      ${PROJECT_NAME}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )
