set(WINDOWLESS_RENDER_SRC
Main.cpp
WindowlessRenderWindow.cpp
WindowlessRenderWindow.h
)

add_executable(${PROJECT_NAME_WINDOWLESS_RENDER} WIN32 ${WINDOWLESS_RENDER_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

add_definitions(/D NOMINMAX)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_WINDOWLESS_RENDER} ${PROJECT_NAME_WIN_GL})
