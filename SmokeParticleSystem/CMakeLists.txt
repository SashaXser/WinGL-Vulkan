set(SMOKE_PART_SYS_SRC
Main.cpp
Particle.h
ParticleSystem.cpp
ParticleSystem.h
ParticleSystemManager.cpp
ParticleSystemManager.h
SimFrame.h
SmokeParticleSystem.cpp
SmokeParticleSystem.h
SmokeParticleSystemWindow.cpp
SmokeParticleSystemWindow.h
)

add_executable(${PROJECT_NAME_SMOKE_PART_SYS} WIN32 ${SMOKE_PART_SYS_SRC})

include_directories(../WinGL)
include_directories(${EXT_SRC_DIR}/glew-1.9.0/include)

if (PROJECTS_USE_GLEW_STATIC)
   add_definitions(/DGLEW_STATIC)
endif (PROJECTS_USE_GLEW_STATIC)

target_link_libraries(${PROJECT_NAME_SMOKE_PART_SYS} ${PROJECT_NAME_WIN_GL})

copy_to_bin(Smoke.rgb)
