cmake_minimum_required(VERSION 3.0.0)

set(GLEW_SRC
./src/glew.c

./include/gl/glew.h
./include/gl/wglew.h
)

set(GLEW_INFO_SRC
./src/glewinfo.c
)

set(GLEW_VISUAL_INFO_SRC
./src/visualinfo.c
)

add_library(glew_static STATIC ${GLEW_SRC})
add_library(glew_shared SHARED ${GLEW_SRC})

add_executable(glewinfo ${GLEW_INFO_SRC})
add_executable(visualinfo ${GLEW_VISUAL_INFO_SRC})

target_include_directories(glew_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(glew_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(glew_static PUBLIC "GLEW_STATIC")
target_compile_definitions(glew_shared PRIVATE "GLEW_BUILD")
set_target_properties(glewinfo PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")
set_target_properties(visualinfo PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")

target_link_libraries(glew_static opengl32.lib)
target_link_libraries(glew_shared opengl32.lib)

target_link_libraries(glewinfo glew_static)
target_link_libraries(visualinfo glew_static)
target_link_libraries(visualinfo glu32.lib)

set_target_properties(glew_static PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(glew_shared PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(glewinfo PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(visualinfo PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
