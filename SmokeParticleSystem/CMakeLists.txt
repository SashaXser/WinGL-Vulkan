cmake_minimum_required(VERSION 3.0.0)

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

add_executable(SmokeParticleSystem WIN32 ${SMOKE_PART_SYS_SRC})

target_link_libraries(SmokeParticleSystem WinGL)

copy_to_bin(Smoke.rgb)
