// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "DOFTransformWindow.h"

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
   DOFTransformWindow * const pDTW = new DOFTransformWindow();
   if (pDTW->Create(540, 540, "DOF Transform Window"))
   {
      // run the program
      error = pDTW->Run();
   }

   return error;
}
