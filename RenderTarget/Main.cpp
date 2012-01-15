// platform includes
#include <windows.h>

// local includes
#include "RenderTargetWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   RenderTargetWindow * pRTW = new RenderTargetWindow();
   pRTW->Create(200, 200, "Render Target Window");
   return pRTW->Run();
}