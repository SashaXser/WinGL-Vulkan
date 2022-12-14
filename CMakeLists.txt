cmake_minimum_required(VERSION 3.15.2)

# use cxx17 as the standard for all projects
set(
   CMAKE_CXX_STANDARD
   17)

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

# validate MSVC 17 (1930) or greater
if (MSVC_VERSION LESS 1930)
   message(FATAL_ERROR "MSVC 17 (1930) or greater must be used to compile projects!")
endif ( )

add_compile_options(
   /MP
   /W4
   /permissive-
   $<IF:$<CONFIG:Debug>,/MTd,/MT>)

add_compile_definitions(
   _CRT_SECURE_NO_WARNINGS
   WIN32_LEAN_AND_MEAN
   VC_EXTRA_LEAN
   NOMINMAX)

if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif ( )

set(EXT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext-src)
set(EXT_SRC_IDE_FOLDER ext-src)

set(PROJECTS_USE_GLEW_STATIC NO)

# just in case an install happens, just install locally...
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

set(OPENGL_IDE_FOLDER opengl)

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
add_subdirectory(./Tower)
add_subdirectory(./DOFTransform)
add_subdirectory(./TransformFeedback)
add_subdirectory(./Displacement)
add_subdirectory(./SliderControl)
add_subdirectory(./GLStudioTest)
add_subdirectory(./MultisampleFramebuffer)

add_subdirectory(./Vulkan)
