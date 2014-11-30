cmake_minimum_required(VERSION 3.0.0)

set(NORMAL_MAPPING_SRC
normal_mapping_flat_shader.frag
normal_mapping_flat_shader.vert
Main.cpp
NormalMappingWindow.cpp
NormalMappingWindow.h
)

add_executable(NormalMapping WIN32 ${NORMAL_MAPPING_SRC})

target_link_libraries(NormalMapping WinGL)

copy_to_bin(normal_mapping_flat_shader.frag)
copy_to_bin(normal_mapping_flat_shader.vert)
copy_to_bin(EC_Stone_Wall_Diffuse.jpg)
copy_to_bin(EC_Stone_Wall_Height.jpg)
copy_to_bin(EC_Stone_Wall_Normal.jpg)
