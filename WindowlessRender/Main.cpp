// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "WindowlessRenderWindow.h"

int __stdcall WinMain( HINSTANCE /*instance*/, HINSTANCE /*pinstance*/,
                       LPSTR /*pCmdline*/, int /*show*/ )
{
   // allocate a console for the application
   AllocateDebugConsole();

   // create the main application window
   WindowlessRenderWindow * pWnd = new WindowlessRenderWindow();
   pWnd->Create(800, 600, "Windowless Rendering");

   // begin processing the main application
   return pWnd->Run();
}
