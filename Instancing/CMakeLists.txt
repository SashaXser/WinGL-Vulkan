set(INSTANCING_SRC
Main.cpp
InstancingWindow.cpp
InstancingWindow.h
Shaders.cpp
Shaders.h
)

set(SHADER_SRC
vertex.vert
fragment.frag
)

add_executable(${PROJECT_NAME_INSTANCING} WIN32 ${INSTANCING_SRC} ${SHADER_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_INSTANCING} ${PROJECT_NAME_WIN_GL})

copy_to_bin(buildings.rgb)
copy_to_bin(fragment.frag)
copy_to_bin(vertex.vert)
