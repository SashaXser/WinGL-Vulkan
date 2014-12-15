cmake_minimum_required(VERSION 3.0.0)

# update the cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/ext-src/cmake-modules" ${CMAKE_MODULE_PATH})

macro(copy_to_bin file)
   foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
      file(COPY ${file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${config})
   endforeach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
endmacro(copy_to_bin)

set(CMAKE_CONFIGURATION_TYPES
    Debug Release CACHE STRING
    "Overrides the available build modes for InSight and InSight based apps"
    FORCE)
    
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(win-gl LANGUAGES C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /D _CRT_SECURE_NO_WARNINGS /D WIN32_LEAN_AND_MEAN /D VC_EXTRA_LEAN /D NOMINMAX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /D _CRT_SECURE_NO_WARNINGS /D WIN32_LEAN_AND_MEAN /D VC_EXTRA_LEAN /D NOMINMAX")

if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif ( )

set(EXT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext-src)
set(EXT_SRC_IDE_FOLDER ext-src)

set(PROJECTS_USE_GLEW_STATIC YES)

# just in case an install happens, just install locally...
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

add_subdirectory(./ext-src)
add_subdirectory(./WinGL)
add_subdirectory(./ProjectiveTexturing)
add_subdirectory(./RenderTarget)
add_subdirectory(./ShadowMap)
add_subdirectory(./SmokeParticleSystem)
add_subdirectory(./Instancing)
add_subdirectory(./WindowlessRender)
add_subdirectory(./TessellationTriangle)
add_subdirectory(./Planets)
add_subdirectory(./CubeMap)
add_subdirectory(./NormalMapping)
add_subdirectory(./AutoScale)
