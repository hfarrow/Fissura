# this module was taken from http://trac.evemu.org/browser/trunk/cmake/FindZLIB.cmake
# - Find ZLIB
# Find the native ZLIB includes and library
#
#   ZLIB_FOUND       - True if ZLIB found.
#   ZLIB_INCLUDE_DIR - where to find zlib.h, etc.
#   ZLIB_LIBRARIES   - List of libraries when using ZLIB.
#

SET(ZLIB_INCLUDE_DIR ${EXTERNAL_LIBRARY_INSTALL_PATH}/include/)

IF(DEFINED IS_MAC)
    # Mac OS X specific code
    set(ZLIB_LIBRARIES ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libz.dylib)
ELSEIF(DEFINED IS_LINUX)
    # Linux specific code
    set(ZLIB_LIBRARIES ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libz.so)
ENDIF(DEFINED IS_MAC)

SET(ZLIB_LIBRARIES ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libz.a)

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB
                                  REQUIRED_VARS ZLIB_INCLUDE_DIR ZLIB_LIBRARIES)

MARK_AS_ADVANCED( ZLIB_INCLUDE_DIR ZLIB_LIBRARIES )

