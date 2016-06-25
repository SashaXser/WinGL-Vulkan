// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "DisplacementWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // allocate a console for the application
   AllocateDebugConsole();

   // create a new tesselation window
   DisplacementWindow * const pDisWnd = new DisplacementWindow();
   if (pDisWnd->Create(540, 540, "Displacement Window"))
   {
      // run the program
      error = pDisWnd->Run();
   }

   return error;
}
