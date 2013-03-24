cmake_minimum_required(VERSION 2.8)

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

project(win-gl)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /D _CRT_SECURE_NO_WARNINGS /D WIN32_LEAN_AND_MEAN /D VC_EXTRA_LEAN")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /D _CRT_SECURE_NO_WARNINGS /D WIN32_LEAN_AND_MEAN /D VC_EXTRA_LEAN")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(EXT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext-src)
set(EXT_SRC_IDE_FOLDER ext-src)

set(PROJECTS_USE_GLEW_STATIC TRUE)

set(PROJECT_NAME_GLEW_STATIC glew_static)
set(PROJECT_NAME_GLEW_DYNAMIC glew_shared)
set(PROJECT_NAME_WIN_GL WinGL)
set(PROJECT_NAME_PROJ_TEX ProjectiveTexturing)
set(PROJECT_NAME_RENDER_TGT RenderTarget)
set(PROJECT_NAME_SHADOW_MAP ShadowMap)
set(PROJECT_NAME_SMOKE_PART_SYS SmokeParticleSystem)
set(PROJECT_NAME_INSTANCING Instancing)

add_subdirectory(./ext-src/glew-1.9.0)
add_subdirectory(./WinGL)
add_subdirectory(./ProjectiveTexturing)
add_subdirectory(./RenderTarget)
add_subdirectory(./ShadowMap)
add_subdirectory(./SmokeParticleSystem)
add_subdirectory(./Instancing)
