set(SHADOW_MAP_SRC
enterprise.frag
enterprise.vert
Main.cpp
ShadowMapWindow.cpp
ShadowMapWindow.h
)

add_executable(${PROJECT_NAME_SHADOW_MAP} WIN32 ${SHADOW_MAP_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/assimp-3.0.1270/include)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_SHADOW_MAP} ${PROJECT_NAME_WIN_GL})
target_link_libraries(${PROJECT_NAME_SHADOW_MAP} ${PROJECT_NAME_ASSIMP})

copy_to_bin(enterprise-e)
copy_to_bin(enterprise.frag)
copy_to_bin(enterprise.vert)