// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "ProjectiveTextureWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // allocate a console for the application
   AllocateDebugConsole();

   // create the main application window
   ProjectiveTextureWindow * pProjWnd = new ProjectiveTextureWindow;
   if (pProjWnd->Create(800, 600, "Projective Texture"))
   {
      // begin processing the main app
      return pProjWnd->Run();
   }

   return -100;
}