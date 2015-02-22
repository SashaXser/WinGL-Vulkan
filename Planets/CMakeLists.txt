cmake_minimum_required(VERSION 3.0.0)

set(PLANETS_SRC
Main.cpp
Earth.cpp
Earth.h
Planet.cpp
Planet.h
PlanetsWindow.cpp
PlanetsWindow.h
)

set(SHADER_SRC
planet.frag
planet.vert
)

add_executable(Planets WIN32 ${PLANETS_SRC} ${SHADER_SRC})

target_link_libraries(Planets WinGL)

copy_to_bin(./Textures)

foreach(file IN LISTS SHADER_SRC)
   copy_to_bin(${file})
endforeach(file IN LISTS SHADER_SRC)
