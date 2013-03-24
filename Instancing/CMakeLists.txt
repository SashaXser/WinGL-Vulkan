set(INSTANCING_SRC
Main.cpp
InstancingWindow.cpp
InstancingWindow.h
)

add_executable(${PROJECT_NAME_INSTANCING} WIN32 ${INSTANCING_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_INSTANCING} ${PROJECT_NAME_WIN_GL})
