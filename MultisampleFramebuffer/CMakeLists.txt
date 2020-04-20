cmake_minimum_required(VERSION 3.0.0)

set(MULTISAMPLE_FRAMEBUFFER_SRC
   MultisampleFramebufferWindow.cpp
   MultisampleFramebufferWindow.h
   Main.cpp)

add_executable(
   MultisampleFramebuffer
   WIN32
   ${MULTISAMPLE_FRAMEBUFFER_SRC})

target_link_libraries(MultisampleFramebuffer WinGL)

set_target_properties(
   MultisampleFramebuffer
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")
