cmake_minimum_required(VERSION 3.0.0)

# assimp project defines
set(BUILD_TESTS OFF)
set(BUILD_STATIC_LIB ON)
set(OPT_BUILD_PACKAGES OFF)
set(BUILD_ASSIMP_TOOLS OFF)
set(BUILD_ASSIMP_SAMPLES OFF)
set(ENABLE_BOOST_WORKAROUND ON)

# do a lookup of zlib first, so other cmake module paths are not overwritten...
find_package(ZLIB REQUIRED)

add_subdirectory(./glew-1.11.0)
add_subdirectory(./assimp-3.0.1270)
add_subdirectory(./resil-1.8.2)

# move the zlib project into the ext-src folder
set_target_properties(zlibstatic PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/zlib")

# move assimp and dependencies into ext-src folder
set_target_properties(assimp PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/assimp")
set_target_properties(uninstall PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/assimp")
