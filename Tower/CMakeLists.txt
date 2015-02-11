cmake_minimum_required(VERSION 3.0.0)

set(CUBEMAP_SRC
./../CubeMap/CFont.cpp
./../CubeMap/CFont.h
)

set(TOWER_SRC
Main.cpp
TowerWindow.cpp
TowerWindow.h
)

set(SHADER_SRC
)

add_executable(Tower WIN32 ${TOWER_SRC} ${CUBEMAP_SRC} ${SHADER_SRC})

target_link_libraries(Tower WinGL)

include_directories(./../CubeMap)

copy_to_bin(./Tower)

foreach(file IN LISTS SHADER_SRC)
   copy_to_bin(${file})
endforeach(file IN LISTS SHADER_SRC)
