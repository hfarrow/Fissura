cmake_minimum_required(VERSION 2.6)

project(TinyXml)

set(LIBRARY_OUTPUT_PATH ../build/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS  "-Wall -std=c++11 -stdlib=libc++")

include_directories(./)

file(
	GLOB_RECURSE
	source_files
	*.h
	*.cpp
)

add_library(
	tinyxml
    STATIC
	${source_files}
)

#SET_TARGET_PROPERTIES (tinyxml PROPERTIES DEFINE_SYMBOL  "EXPORT" )

SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../build)
INSTALL( FILES tinyxml.h tinystr.h DESTINATION include/tinyxml )
INSTALL( FILES build/lib/libtinyxml.a DESTINATION lib/ )
