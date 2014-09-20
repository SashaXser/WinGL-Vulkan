cmake_minimum_required(VERSION 3.0.0)

set(WIN_GL_SRC
./AllocConsole.cpp
./AllocConsole.h
./MathHelper.h
./Matrix.h
./MatrixHelper.h
./OpenGLExtensions.cpp
./OpenGLExtensions.h
./OpenGLWindow.cpp
./OpenGLWindow.h
./ReadTexture.cpp
./ReadTexture.h
./ReuseAllocator.h
./ShaderProgram.cpp
./ShaderProgram.h
./Shaders.cpp
./Shaders.h
./Singleton.h
./Timer.h
./Vector3.h
./Vector4.h
./VertexArrayObject.cpp
./VertexArrayObject.h
./VertexBufferObject.cpp
./VertexBufferObject.h
./WglAssert.cpp
./WglAssert.h
./Window.cpp
./Window.h
./WindowDefines.h
)

add_library(WinGL STATIC ${WIN_GL_SRC})

target_compile_definitions(WinGL PUBLIC _USE_MATH_DEFINES)

target_include_directories(WinGL INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(WinGL PUBLIC opengl32.lib)

if (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(WinGL PUBLIC glew_static)
else (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(WinGL PUBLIC glew_shared)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(WinGL PUBLIC resil)
