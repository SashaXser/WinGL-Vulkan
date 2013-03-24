// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "InstancingWindow.h"

int __stdcall WinMain( HINSTANCE instance, HINSTANCE pinstance,
                       LPSTR pCmdline, int show )
{
   // allocate a console for the application
   AllocateDebugConsole();

   // create the main application window
   InstancingWindow * pWnd = new InstancingWindow();
   pWnd->Create(800, 600, "Instancing");

   // begin processing the main application
   return pWnd->Run();
}
