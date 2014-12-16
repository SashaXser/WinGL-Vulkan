cmake_minimum_required(VERSION 3.0.0)

set(AUTO_SCALE_SRC
Main.cpp
AutoScaleWindow.cpp
AutoScaleWindow.h
)

add_executable(AutoScale WIN32 ${AUTO_SCALE_SRC})

target_link_libraries(AutoScale WinGL)
