# we are going to point to the zlib inside of the resil, as it is a newer version

# include the standard find package validation macros
include(FindPackageHandleStandardArgs)

# find the directory to which zlib points
find_path(ZLIB_INCLUDE_DIR
          zlib.h
          PATHS "${CMAKE_SOURCE_DIR}/ext-src/resil-1.8.2/input libs/zlib-1.2.8"
#                "${CMAKE_SOURCE_DIR}/ext-src/assimp-3.0.1270/contrib/zlib"
          DOC "Path to zlib.h"
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

# skip install any of the zlib files
set(SKIP_INSTALL_ALL YES)
          
# since zlib is being compiled by this source tree,
# use the name of the project to setup the zlib name
set(ZLIB_LIBRARY zlibstatic CACHE INTERNAL "Library project name for zlib")
set(ZLIB_LIBRARIES zlibstatic CACHE INTERNAL "Library project name for zlib")

# confirm the location of The include directories and project name
find_package_handle_standard_args(ZLIB DEFAULT_MSG ZLIB_INCLUDE_DIR ZLIB_LIBRARY ZLIB_LIBRARIES)
