cmake_minimum_required(VERSION 2.6)

project(TinyXml)

set(LIBRARY_OUTPUT_PATH ../build/lib/${CMAKE_BUILD_TYPE})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Mac OS X specific code
    set(IS_MAC "1")
    set(IS_UNIX "1")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    set(IS_LINUX "1")
    set(IS_UNIX "1")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


IF(DEFINED IS_LINUX)
    set(CMAKE_CXX_FLAGS                 "-Wall -fPIC -std=c++11")
ELSEIF(DEFINED IS_MAC)
    set(CMAKE_CXX_FLAGS                 "-Wall -fPIC -std=c++11 -stdlib=libc++")
ENDIF(DEFINED IS_LINUX)

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
