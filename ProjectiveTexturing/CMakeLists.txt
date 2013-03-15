set(PROJ_TEX_SRC
Main.cpp
ProjectiveTextureWindow.cpp
ProjectiveTextureWindow.h
)

add_executable(${PROJECT_NAME_PROJ_TEX} WIN32 ${PROJ_TEX_SRC})

include_directories(../WinGL)

target_link_libraries(${PROJECT_NAME_PROJ_TEX} ${PROJECT_NAME_WIN_GL})

copy_to_bin(BMLogo.rgb)
