set(WIN_GL_SRC
./AllocConsole.cpp
./AllocConsole.h
./Matrix.h
./MatrixHelper.h
./OpenGLExtensions.cpp
./OpenGLExtensions.h
./OpenGLWindow.cpp
./OpenGLWindow.h
./ReadTexture.cpp
./ReadTexture.h
./ReuseAllocator.h
./Shaders.cpp
./Shaders.h
./Singleton.h
./Timer.h
./Vector3.h
./Vector4.h
./WglAssert.cpp
./WglAssert.h
./Window.cpp
./Window.h
./WindowDefines.h
)

add_library(${PROJECT_NAME_WIN_GL} STATIC ${WIN_GL_SRC})

include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_WIN_GL} opengl32.lib)

if (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(${PROJECT_NAME_WIN_GL} ${PROJECT_NAME_GLEW_STATIC})
else (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(${PROJECT_NAME_WIN_GL} ${PROJECT_NAME_GLEW_DYNAMIC})
endif (PROJECTS_USE_GLEW_STATIC)
