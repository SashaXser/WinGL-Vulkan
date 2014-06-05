// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "TessellationTriangleWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // allocate a console for the application
   AllocateDebugConsole();

   // create a new tesselation window
   TessellationTriangleWindow * const pTTW = new TessellationTriangleWindow();
   pTTW->Create(500, 500, "Tessellation Triangle Window");

   // run the program
   return pTTW->Run();
}