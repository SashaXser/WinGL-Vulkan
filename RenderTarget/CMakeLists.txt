set(RENDER_TGT_SRC
Main.cpp
RenderTargetWindow.cpp
RenderTargetWindow.h
)

add_executable(${PROJECT_NAME_RENDER_TGT} WIN32 ${RENDER_TGT_SRC})

include_directories(../WinGL)

target_link_libraries(${PROJECT_NAME_RENDER_TGT} ${PROJECT_NAME_WIN_GL})
