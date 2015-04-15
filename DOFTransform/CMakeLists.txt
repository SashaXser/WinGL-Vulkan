cmake_minimum_required(VERSION 3.0.0)

set(DOF_TRANSFORM_SRC
DOFTransformWindow.cpp
DOFTransformWindow.h
Main.cpp
)

add_executable(DOFTransform WIN32 ${DOF_TRANSFORM_SRC})

target_link_libraries(DOFTransform WinGL)
