cmake_minimum_required(VERSION 3.0.0)

set(WINDOWLESS_RENDER_SRC
Main.cpp
WindowlessRenderWindow.cpp
WindowlessRenderWindow.h
)

add_executable(WindowlessRender WIN32 ${WINDOWLESS_RENDER_SRC})

target_link_libraries(WindowlessRender WinGL)

set_target_properties(
   WindowlessRender
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")
