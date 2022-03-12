// platform includes
#include <windows.h>

// local includes
#include "SmokeParticleSystemWindow.h"

int WINAPI WinMain( HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    LPSTR /*lpCmdLine*/,
                    int /*nShowCmd*/ )
{
   SmokeParticleSystemWindow * pPartSys = new SmokeParticleSystemWindow;
   pPartSys->Create(1024, 768, "Smoke Particle System");
   return pPartSys->Run();
}
