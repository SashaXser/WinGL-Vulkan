cmake_minimum_required(VERSION 3.0.0)

set(SHADOW_MAP_SRC
enterprise.frag
enterprise.vert
Main.cpp
ShadowMapWindow.cpp
ShadowMapWindow.h
)

add_executable(ShadowMap WIN32 ${SHADOW_MAP_SRC})

target_link_libraries(ShadowMap WinGL)
target_link_libraries(ShadowMap assimp)

copy_to_bin(enterprise)
copy_to_bin(enterprise.frag)
copy_to_bin(enterprise.vert)
