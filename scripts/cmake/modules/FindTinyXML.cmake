# this module was taken from http://trac.evemu.org/browser/trunk/cmake/FindTinyXML.cmake
# - Find TinyXML
# Find the native TinyXML includes and library
#
#   TINYXML_FOUND       - True if TinyXML found.
#   TINYXML_INCLUDE_DIR - where to find tinyxml.h, etc.
#   TINYXML_LIBRARIES   - List of libraries when using TinyXML.
#

SET(TINYXML_INCLUDE_DIR ${EXTERNAL_LIBRARY_INSTALL_PATH}/include/tinyxml/)
SET(TINYXML_LIBRARIES ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libtinyxml.a)

# handle the QUIETLY and REQUIRED arguments and set TINYXML_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TinyXML
                                  REQUIRED_VARS TINYXML_INCLUDE_DIR TINYXML_LIBRARIES)

MARK_AS_ADVANCED( TINYXML_INCLUDE_DIR TINYXML_LIBRARIES )
