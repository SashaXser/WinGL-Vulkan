cmake_minimum_required(VERSION 3.0.0)

set(displacement_src
DisplacementWindow.cpp
DisplacementWindow.h
Main.cpp
)

set(shader_src
displacement/shaders/terrain_tess.frag
displacement/shaders/terrain_tess.geom
displacement/shaders/terrain_tess.tctrl
displacement/shaders/terrain_tess.teval
displacement/shaders/terrain_tess.vert
)

add_executable(Displacement WIN32 ${displacement_src} ${shader_src})

target_link_libraries(Displacement WinGL)

copy_to_bin(displacement)
