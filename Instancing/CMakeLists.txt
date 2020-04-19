cmake_minimum_required(VERSION 3.0.0)

set(INSTANCING_SRC
Main.cpp
InstancingWindow.cpp
InstancingWindow.h
)

set(SHADER_SRC
buildings.frag
buildings.vert
trees.frag
trees.geom
trees.vert
)

add_executable(Instancing WIN32 ${INSTANCING_SRC} ${SHADER_SRC})

target_link_libraries(Instancing WinGL)

set_target_properties(
   Instancing
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")

copy_to_bin(buildings.rgb)
copy_to_bin(buildings.frag)
copy_to_bin(buildings.vert)
copy_to_bin(trees.rgb)
copy_to_bin(trees.frag)
copy_to_bin(trees.geom)
copy_to_bin(trees.vert)
