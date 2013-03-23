set(RENDER_TGT_SRC
Main.cpp
RenderTargetWindow.cpp
RenderTargetWindow.h
)

add_executable(${PROJECT_NAME_RENDER_TGT} WIN32 ${RENDER_TGT_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_RENDER_TGT} ${PROJECT_NAME_WIN_GL})
