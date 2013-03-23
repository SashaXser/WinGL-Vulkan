set(PROJ_TEX_SRC
Main.cpp
ProjectiveTextureWindow.cpp
ProjectiveTextureWindow.h
)

add_executable(${PROJECT_NAME_PROJ_TEX} WIN32 ${PROJ_TEX_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_PROJ_TEX} ${PROJECT_NAME_WIN_GL})

copy_to_bin(BMLogo.rgb)
