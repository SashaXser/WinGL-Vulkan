cmake_minimum_required(VERSION 3.0.0)

set(NORMAL_MAPPING_SRC
./normal_mapping/shaders/normal_mapping_dir_light.frag
./normal_mapping/shaders/normal_mapping_dir_light.vert
./normal_mapping/shaders/normal_mapping_flat_shader.frag
./normal_mapping/shaders/normal_mapping_flat_shader.vert
./normal_mapping/shaders/normal_mapping_normal_shader.frag
./normal_mapping/shaders/normal_mapping_normal_shader.vert
./normal_mapping/shaders/normal_mapping_parallax_shader.frag
./normal_mapping/shaders/normal_mapping_parallax_shader.vert
./normal_mapping/shaders/normal_mapping_tess_shader.frag
./normal_mapping/shaders/normal_mapping_tess_shader.vert
./normal_mapping/shaders/normal_mapping_tess_shader.tctrl
./normal_mapping/shaders/normal_mapping_tess_shader.teval
./normal_mapping/shaders/normal_mapping_lighting.glsl
Main.cpp
NormalMappingWindow.cpp
NormalMappingWindow.h
)

add_executable(NormalMapping WIN32 ${NORMAL_MAPPING_SRC})

target_link_libraries(NormalMapping WinGL)

set_target_properties(
   NormalMapping
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")

copy_to_bin(normal_mapping)
