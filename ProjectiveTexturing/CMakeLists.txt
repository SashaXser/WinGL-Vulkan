cmake_minimum_required(VERSION 3.0.0)

set(PROJ_TEX_SRC
Main.cpp
ProjectiveTextureWindow.cpp
ProjectiveTextureWindow.h
projective_texture.frag
projective_texture.vert
)

add_executable(ProjectiveTexturing WIN32 ${PROJ_TEX_SRC})

target_link_libraries(ProjectiveTexturing WinGL)

copy_to_bin(BMLogo.rgb)
copy_to_bin(projective_texture.frag)
copy_to_bin(projective_texture.vert)
