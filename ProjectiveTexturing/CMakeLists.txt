cmake_minimum_required(VERSION 3.0.0)

set(PROJ_TEX_SRC
Main.cpp
ProjectiveTextureWindow.cpp
ProjectiveTextureWindow.h
)

add_executable(ProjectiveTexturing WIN32 ${PROJ_TEX_SRC})

target_link_libraries(ProjectiveTexturing WinGL)

copy_to_bin(BMLogo.rgb)
