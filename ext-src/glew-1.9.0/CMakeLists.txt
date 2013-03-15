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

add_library(${PROJECT_NAME_GLEW_STATIC} STATIC ${GLEW_SRC})
add_library(${PROJECT_NAME_GLEW_DYNAMIC} SHARED ${GLEW_SRC})

add_executable(glewinfo ${GLEW_INFO_SRC})
add_executable(visualinfo ${GLEW_VISUAL_INFO_SRC})

include_directories(./include)

set_target_properties(${PROJECT_NAME_GLEW_STATIC} PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")
set_target_properties(${PROJECT_NAME_GLEW_DYNAMIC} PROPERTIES COMPILE_DEFINITIONS "GLEW_BUILD")
set_target_properties(glewinfo PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")
set_target_properties(visualinfo PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")

target_link_libraries(${PROJECT_NAME_GLEW_STATIC} opengl32.lib)
target_link_libraries(${PROJECT_NAME_GLEW_DYNAMIC} opengl32.lib)

target_link_libraries(glewinfo ${PROJECT_NAME_GLEW_STATIC})
target_link_libraries(visualinfo ${PROJECT_NAME_GLEW_STATIC})
target_link_libraries(visualinfo glu32.lib)

set_target_properties(${PROJECT_NAME_GLEW_STATIC} PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(${PROJECT_NAME_GLEW_DYNAMIC} PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(glewinfo PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
set_target_properties(visualinfo PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/glew")
