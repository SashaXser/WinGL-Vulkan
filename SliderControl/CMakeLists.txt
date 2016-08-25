cmake_minimum_required(VERSION 3.0.0)

set(SLIDER_CONTROL_SRC
Main.cpp
SliderControlWindow.cpp
SliderControlWindow.h
)

add_executable(SliderControl WIN32 ${SLIDER_CONTROL_SRC})

target_link_libraries(SliderControl WinGL)

copy_to_bin(generic_pedal.png)
copy_to_bin(generic_yoke.png)
