cmake_minimum_required(VERSION 3.0.0)

set(NORMAL_MAPPING_SRC
normal_mapping_flat_shader.frag
normal_mapping_flat_shader.vert
normal_mapping_lighting.glsl
Main.cpp
NormalMappingWindow.cpp
NormalMappingWindow.h
)

add_executable(NormalMapping WIN32 ${NORMAL_MAPPING_SRC})

target_link_libraries(NormalMapping WinGL)

copy_to_bin(normal_mapping_flat_shader.frag)
copy_to_bin(normal_mapping_flat_shader.vert)
copy_to_bin(normal_mapping_lighting.glsl)
copy_to_bin(normal_mapping_diffuse.jpg)
copy_to_bin(normal_mapping_height.jpg)
copy_to_bin(normal_mapping_normal.jpg)
