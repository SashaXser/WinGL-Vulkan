cmake_minimum_required(VERSION 3.0.0)

set(SHADOW_MAP_SRC
enterprise.frag
enterprise.vert
enterprise_normal.frag
enterprise_normal.geom
enterprise_normal.vert
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
copy_to_bin(enterprise_normal.frag)
copy_to_bin(enterprise_normal.geom)
copy_to_bin(enterprise_normal.vert)
