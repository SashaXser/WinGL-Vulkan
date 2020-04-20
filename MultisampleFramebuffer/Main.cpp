// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "MultisampleFramebufferWindow.h"

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
   auto * const pMFW = new MultisampleFramebufferWindow();
   if (pMFW->Create(640, 480, "Multisample Framebuffer Window"))
   {
      // run the program
      error = pMFW->Run();
   }

   return error;
}
