set(TESS_TRI_SRC
Main.cpp
TessellationTriangleWindow.cpp
TessellationTriangleWindow.h
)

set(SHADER_SRC
triangle.frag
triangle.tctrl
triangle.teval
triangle.geom
triangle.vert
)

add_executable(${PROJECT_NAME_TESS_TRI} WIN32 ${TESS_TRI_SRC} ${SHADER_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_TESS_TRI} ${PROJECT_NAME_WIN_GL})

copy_to_bin(triangle.frag)
copy_to_bin(triangle.tctrl)
copy_to_bin(triangle.teval)
copy_to_bin(triangle.geom)
copy_to_bin(triangle.vert)
