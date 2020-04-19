cmake_minimum_required(VERSION 3.0.0)

set(RENDER_TGT_SRC
Main.cpp
RenderTargetWindow.cpp
RenderTargetWindow.h
)

add_executable(RenderTarget WIN32 ${RENDER_TGT_SRC})

target_link_libraries(RenderTarget WinGL)

set_target_properties(
   RenderTarget
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")
