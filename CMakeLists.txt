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

project(win-gl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(PROJECT_NAME_WIN_GL WinGL)
set(PROJECT_NAME_PROJ_TEX ProjectiveTexturing)
set(PROJECT_NAME_RENDER_TGT RenderTarget)
set(PROJECT_NAME_SHADOW_MAP ShadowMap)
set(PROJECT_NAME_SMOKE_PART_SYS SmokeParticleSystem)

add_subdirectory(./WinGL)
add_subdirectory(./ProjectiveTexturing)
add_subdirectory(./RenderTarget)
add_subdirectory(./ShadowMap)
add_subdirectory(./SmokeParticleSystem)
