cmake_minimum_required(VERSION 3.0.0)

set(CUBEMAP_SRC
CFont.cpp
CFont.h
CFrameBuffer.cpp
CFrameBuffer.h
CMakeLists.txt
CModel.h
CModel3DS.cpp
CModel3DS.h
CPBuffer.cpp
CPBuffer.h
CSphere.cpp
CSphere.h
CubeMapWindow.cpp
CubeMapWindow.h
Main.cpp
SkyBox.cpp
SkyBox.h
StdIncludes.h
Types.h
)

set(SHADER_SRC
)

add_executable(CubeMap WIN32 ${CUBEMAP_SRC} ${SHADER_SRC})

target_link_libraries(CubeMap WinGL)

copy_to_bin(./Images)
