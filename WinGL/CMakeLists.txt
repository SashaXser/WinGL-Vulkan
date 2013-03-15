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

target_link_libraries(${PROJECT_NAME_WIN_GL} opengl32.lib)
