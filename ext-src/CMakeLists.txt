cmake_minimum_required(VERSION 3.0.0)

# assimp project defines
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "ext-src forced override for assimp")
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "ext-src forced override for assimp")
set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "ext-src forced override for assimp")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "ext-src forced override for assimp")
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "ext-src forced override for assimp")
set(ASSIMP_ENABLE_BOOST_WORKAROUND ON CACHE BOOL "ext-src forced override for assimp")

# do a lookup of zlib first, so other cmake module paths are not overwritten...
find_package(ZLIB REQUIRED)

add_subdirectory(./glew-2.1.0)
add_subdirectory(./assimp-3.1.1)
add_subdirectory(./resil-1.8.2)

# move the zlib project into the ext-src folder
set_target_properties(zlibstatic PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/zlib")

# move assimp and dependencies into ext-src folder
set_target_properties(assimp PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/assimp")
set_target_properties(uninstall PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/assimp")
