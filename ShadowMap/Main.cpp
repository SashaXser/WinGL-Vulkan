// platform includes
#include <windows.h>

// local includes
#include "ShadowMapWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   ShadowMapWindow * const pSMW = new ShadowMapWindow();
   pSMW->Create(200, 200, "Shadow Map Window");
   return pSMW->Run();
}