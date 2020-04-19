cmake_minimum_required(VERSION 3.0.0)

set(PROJ_TEX_SRC
Main.cpp
ProjectiveTextureWindow.cpp
ProjectiveTextureWindow.h
projective_texture.frag
projective_texture.vert
viz_projective_texture.frag
viz_projective_texture.vert
)

add_executable(ProjectiveTexturing WIN32 ${PROJ_TEX_SRC})

target_link_libraries(ProjectiveTexturing WinGL)

set_target_properties(
   ProjectiveTexturing
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")

copy_to_bin(BMLogo.rgb)
copy_to_bin(projective_texture.frag)
copy_to_bin(projective_texture.vert)
copy_to_bin(viz_projective_texture.frag)
copy_to_bin(viz_projective_texture.vert)
