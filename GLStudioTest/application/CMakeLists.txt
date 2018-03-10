add_executable(glstudio-application WIN32 main.cpp)

set_target_properties(glstudio-application PROPERTIES FOLDER "GLStudioTest")

target_link_libraries(glstudio-application PRIVATE WinGL)
target_link_libraries(glstudio-application PRIVATE glstudio-component)
