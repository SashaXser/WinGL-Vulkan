set(SHADOW_MAP_SRC
Main.cpp
ShadowMapWindow.cpp
ShadowMapWindow.h
)

add_executable(${PROJECT_NAME_SHADOW_MAP} WIN32 ${SHADOW_MAP_SRC})

include_directories(../WinGL)

target_link_libraries(${PROJECT_NAME_SHADOW_MAP} ${PROJECT_NAME_WIN_GL})
