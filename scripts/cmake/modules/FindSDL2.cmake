# - Find SDL2 library and headers
#
# This script will not find a system installed version of SDL.
# We only search for a build version of SDL2 in external/build/
# SLD2 should be built with something like...
#   cd external/SDL2; ./configure --prefix=/home/heathro/dev/fissura/external/build/; make; make install
#
# Find module for SDL 2.0 (http://www.libsdl.org/).
# It defines the following variables:
#  SDL2_INCLUDE_DIRS - The location of the headers, e.g., SDL.h.
#  SDL2_LIBRARIES - The libraries to link against to use SDL2.
#  SDL2_FOUND - If false, do not try to use SDL2.
#  SDL2_VERSION_STRING - Human-readable string containing the version of SDL2.
#
# This module responds to the the flag:
#  SDL2_BUILDING_LIBRARY
#    If this is defined, then no SDL2_main will be linked in because
#    only applications need main().
#    Otherwise, it is assumed you are building an application and this
#    module will attempt to locate and set the the proper link flags
#    as part of the returned SDL2_LIBRARIES variable.
#
# Also defined, but not for general use are:
#   SDL2_INCLUDE_DIR - The directory that contains SDL.h.
#   SDL2_LIBRARY - The location of the SDL2 library.
#   SDL2MAIN_LIBRARY - The location of the SDL2main library.
#

#find_package(PkgConfig QUIET)
#pkg_check_modules(PC_SDL2 QUIET sdl2)

set(SDL2_INCLUDE_DIR ${EXTERNAL_LIBRARY_INSTALL_PATH}/include/SDL2/)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Mac OS X specific code
    set(SDL2_LIBRARY ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libSDL2.dylib)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    set(SDL2_LIBRARY ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libSDL2.so)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(NOT SDL2_BUILDING_LIBRARY)
    set(SDL2MAIN_LIBRARY ${EXTERNAL_LIBRARY_INSTALL_PATH}/lib/libSDL2main.a)
endif()

if(SDL2_INCLUDE_DIR AND EXISTS "${SDL2_INCLUDE_DIR}/SDL_version.h")
  file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_MAJOR "${SDL2_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_MINOR "${SDL2_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_PATCH "${SDL2_VERSION_PATCH_LINE}")
  set(SDL2_VERSION_STRING ${SDL2_VERSION_MAJOR}.${SDL2_VERSION_MINOR}.${SDL2_VERSION_PATCH})
  unset(SDL2_VERSION_MAJOR_LINE)
  unset(SDL2_VERSION_MINOR_LINE)
  unset(SDL2_VERSION_PATCH_LINE)
  unset(SDL2_VERSION_MAJOR)
  unset(SDL2_VERSION_MINOR)
  unset(SDL2_VERSION_PATCH)
endif()

set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
set(SDL2_LIBRARIES ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2
                                  REQUIRED_VARS SDL2_INCLUDE_DIR SDL2_LIBRARY
                                  VERSION_VAR SDL2_VERSION_STRING)

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY)
