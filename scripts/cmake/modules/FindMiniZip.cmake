# - Find MiniZip
#
#   MINIZIP_FOUND       - True if MiniZip found.
#   MINIZIP_INCLUDE_DIR - where to find zip.h, etc.
#   MINIZIP_LIBRARIES   - List of libraries when using MiniZip.
#

SET(MINIZIP_INCLUDE_DIR ${EXTERNAL_LIBRARY_INSTALL_PATH}/include/minizip/)
SET(MINIZIP_LIBRARIES ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libminizip.a)

# handle the QUIETLY and REQUIRED arguments and set MINIZIPL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MiniZip
    REQUIRED_VARS MINIZIP_INCLUDE_DIR MINIZIP_LIBRARIES)

MARK_AS_ADVANCED( MINIZIP_INCLUDE_DIR MINIZIP_LIBRARIES )
