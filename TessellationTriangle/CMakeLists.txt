cmake_minimum_required(VERSION 3.0.0)

set(TESS_TRI_SRC
Main.cpp
TessellationTriangleWindow.cpp
TessellationTriangleWindow.h
)

set(SHADER_SRC
triangle.frag
triangle.tctrl
triangle.teval
triangle.vert
)

add_executable(TessellationTriangle WIN32 ${TESS_TRI_SRC} ${SHADER_SRC})

target_link_libraries(TessellationTriangle WinGL)

copy_to_bin(triangle.frag)
copy_to_bin(triangle.tctrl)
copy_to_bin(triangle.teval)
copy_to_bin(triangle.vert)
