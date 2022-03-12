cmake_minimum_required(VERSION 3.0.0)

set(WIN_GL_CAMERA_POLICIES_SRC
./CameraPolicies/RoamNoRollRestrictPitch.inl
./CameraPolicies/RoamNoRollRestrictPitch.h
)

set(WIN_GL_SRC
./AllocConsole.cpp
./AllocConsole.h
./Camera.h
./FrameBufferObject.cpp
./FrameBufferObject.h
./GeomHelper.cpp
./GeomHelper.h
./MathHelper.h
./Matrix.h
./MatrixHelper.h
./OpenGLExtensions.cpp
./OpenGLExtensions.h
./OpenGLWindow.cpp
./OpenGLWindow.h
./Pipeline.cpp
./Pipeline.h
./Quaternion.h
./QueryObject.cpp
./QueryObject.h
./ReadTexture.cpp
./ReadTexture.h
./ReuseAllocator.h
./ShaderProgram.cpp
./ShaderProgram.h
./Shaders.cpp
./Shaders.h
./Singleton.h
./Texture.cpp
./Texture.h
./TransformFeedbackObject.cpp
./TransformFeedbackObject.h
./Timer.h
./Vector.h
./VertexArrayObject.cpp
./VertexArrayObject.h
./VertexBufferObject.cpp
./VertexBufferObject.h
./WglAssert.cpp
./WglAssert.h
./Window.cpp
./Window.h
./WindowDefines.h

${WIN_GL_CAMERA_POLICIES_SRC}
)

add_library(WinGL STATIC ${WIN_GL_SRC})

source_group("Camera Policies" FILES ${WIN_GL_CAMERA_POLICIES_SRC})

target_compile_definitions(WinGL PUBLIC _USE_MATH_DEFINES)

target_compile_options(WinGL PRIVATE /wd4706)

target_include_directories(WinGL INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(WinGL PUBLIC opengl32.lib)

if (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(WinGL PUBLIC glew_static)
else (PROJECTS_USE_GLEW_STATIC)
   target_link_libraries(WinGL PUBLIC glew_shared)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(WinGL PUBLIC resil)

set_target_properties(
   WinGL
   PROPERTIES
   FOLDER
   "${OPENGL_IDE_FOLDER}")
