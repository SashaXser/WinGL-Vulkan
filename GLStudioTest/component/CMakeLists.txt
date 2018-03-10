add_library(glstudio-component-interface INTERFACE)

target_include_directories(glstudio-component-interface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(
   glstudio-component SHARED
   component.cpp
   component.h
   component_impl.cpp
   component_impl.h)

set_target_properties(glstudio-component PROPERTIES FOLDER "GLStudioTest")

target_link_libraries(glstudio-component PRIVATE WinGL)
target_link_libraries(glstudio-component PUBLIC glstudio-component-interface)

copy_to_bin(glstudio-component-textures)
