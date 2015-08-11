// platform includes
#include <windows.h>

// local includes
#include "CubeMapWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // create the window
   CubeMapWindow * const pCubeMapWnd = new CubeMapWindow();
   if (pCubeMapWnd->Create(500, 500, "Cube Map Window"))
   {
      // begin displaying the window
      error = pCubeMapWnd->Run();
   }

   return error;
}
